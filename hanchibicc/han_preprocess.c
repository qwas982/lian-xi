//han_preprocess.c

#include "han_chibicc.h"

// 此文件实现了C预处理器。

// 预处理器以一系列标记作为输入，并返回一系列新的标记作为输出。

// 预处理语言的设计方式保证了即使存在递归宏，也能够停止。
// 简单来说，每个标记仅被宏应用一次。
// 换句话说，如果宏标记T出现在直接或间接宏展开的结果中，T将不会进一步展开。
// 例如，如果T被定义为U，而U被定义为T，则标记T被展开为U，然后再展开为T，宏展开在此处停止。

// 为了实现上述行为，我们为每个标记附加一个宏名称集合，用于展开该标记。该集合称为“隐藏集”。
// 隐藏集最初为空，并且每次展开宏时，宏名称将被添加到结果标记的隐藏集中。

// 上述宏展开算法在Dave Prossor撰写的此文档中有详细解释，该文档被用作标准措辞的基础：
// https://github.com/rui314/chibicc/wiki/cpp.algo.pdf

// 定义结构体MacroParam，表示宏参数
typedef struct MacroParam MacroParam;
struct MacroParam {
  MacroParam *next; // 指向下一个宏参数的指针
  char *name; // 宏参数的名称
};

// 定义结构体MacroArg，表示宏实参
typedef struct MacroArg MacroArg;
struct MacroArg {
  MacroArg *next; // 指向下一个宏实参的指针
  char *name; // 宏实参的名称
  bool is_va_args; // 表示是否是可变参数
  Token *tok; // 实参对应的令牌
};

// 定义宏处理函数类型，接受一个Token指针作为参数，返回一个Token指针
typedef Token *macro_handler_fn(Token *);

// 定义结构体Macro，表示宏定义
typedef struct Macro Macro;
struct Macro {
  char *name; // 宏名称
  bool is_objlike; // 是否是对象宏或函数宏
  MacroParam *params; // 宏参数列表
  char *va_args_name; // 可变参数的名称
  Token *body; // 宏的主体部分（宏展开后的令牌序列）
  macro_handler_fn *handler; // 宏处理函数的指针
};

// `#if` 可以嵌套，因此我们使用栈来管理嵌套的 `#if`。
typedef struct CondIncl CondIncl;
struct CondIncl {
  CondIncl *next; // 指向下一个条件包含指令的指针
  enum { IN_THEN, IN_ELIF, IN_ELSE } ctx; // 表示当前指令的上下文：IN_THEN（在 #if 条件中）、IN_ELIF（在 #elif 条件中）、IN_ELSE（在 #else 条件中）
  Token *tok; // 当前条件包含指令对应的令牌
  bool included; // 表示是否包含该条件指令中的代码
};

// 定义结构体Hideset，表示隐藏集
typedef struct Hideset Hideset;
struct Hideset {
  Hideset *next; // 指向下一个隐藏集的指针
  char *name; // 隐藏集中的名称
};

static HashMap macros; // 宏定义的哈希表
static CondIncl *cond_incl; // 条件包含指令的链表
static HashMap pragma_once; // #pragma once 的哈希表
static int include_next_idx; // #include_next 指令的索引

static Token *preprocess2(Token *tok); // 预处理的辅助函数
static Macro *find_macro(Token *tok); // 查找宏定义的辅助函数

static bool is_hash(Token *tok) {
  return tok->at_bol && equal(tok, "#"); // 判断是否是开头的 #
}

// 这是一个用于跳过行中额外标记的函数。
// 一些预处理指令（如 #include）允许在换行符之前存在额外的标记。
static Token *skip_line(Token *tok) {
  if (tok->at_bol)
    return tok;  // 如果当前 token 已经在行首，直接返回该 token
  warn_tok(tok, "extra token");  // 发出警告，指示存在额外的标记
  while (tok->at_bol)
    tok = tok->next;  // 跳过所有位于行首的标记
  return tok;  // 返回最后一个位于行首的标记
}

static Token *copy_token(Token *tok) {
  Token *t = calloc(1, sizeof(Token));
  *t = *tok;  // 复制 token 的内容
  t->next = NULL;  // 设置下一个 token 为 NULL
  return t;  // 返回复制后的 token
}

static Token *new_eof(Token *tok) {
  Token *t = copy_token(tok);  // 复制 token
  t->kind = TK_EOF;  // 设置 token 的类型为 TK_EOF
  t->len = 0;  // 设置 token 的长度为 0
  return t;  // 返回新的 EOF token
}

static Hideset *new_hideset(char *name) {
  Hideset *hs = calloc(1, sizeof(Hideset));  // 分配 Hideset 结构的内存空间
  hs->name = name;  // 设置 Hideset 的名称
  return hs;  // 返回新的 Hideset 结构
}

static Hideset *hideset_union(Hideset *hs1, Hideset *hs2) {
  Hideset head = {};  // 创建一个空的 Hideset 结构作为头部
  Hideset *cur = &head;  // cur 指向头部

  for (; hs1; hs1 = hs1->next)
    cur = cur->next = new_hideset(hs1->name);  // 将 hs1 中的每个 Hideset 结构复制到新的 Hideset 链表中
  
  cur->next = hs2;  // 将 hs2 链接到新的 Hideset 链表的末尾
  return head.next;  // 返回新的 Hideset 链表的首个节点
}

static bool hideset_contains(Hideset *hs, char *s, int len) {
  for (; hs; hs = hs->next)
    if (strlen(hs->name) == len && !strncmp(hs->name, s, len))
      return true;  // 如果名称长度相等且名称内容相同，则返回 true
  
  return false;  // 如果未找到匹配的名称，则返回 false
}

static Hideset *hideset_intersection(Hideset *hs1, Hideset *hs2) {
  Hideset head = {};  // 创建一个空的 Hideset 结构作为头部
  Hideset *cur = &head;  // cur 指向头部

  for (; hs1; hs1 = hs1->next) {
    if (hideset_contains(hs2, hs1->name, strlen(hs1->name))) {
      cur = cur->next = new_hideset(hs1->name);  // 如果 hs2 包含 hs1 的名称，则将其添加到新的 Hideset 链表中
    }
  }

  return head.next;  // 返回新的 Hideset 链表的首个节点
}

static Token *add_hideset(Token *tok, Hideset *hs) {
  Token head = {};  // 创建一个空的 Token 结构作为头部
  Token *cur = &head;  // cur 指向头部

  for (; tok; tok = tok->next) {
    Token *t = copy_token(tok);  // 复制当前 token
    t->hideset = hideset_union(t->hideset, hs);  // 将当前 token 的隐藏集与给定的隐藏集取并集
    cur = cur->next = t;  // 将当前 token 添加到新的 Token 链表中
  }
  
  return head.next;  // 返回新的 Token 链表的首个节点
}

// 将tok2追加到tok1的末尾。
static Token *append(Token *tok1, Token *tok2) {
  if (tok1->kind == TK_EOF)  // 如果tok1的类型是TK_EOF（表示已经到达文件末尾），则直接返回tok2
    return tok2;

  Token head = {};  // 创建一个空的Token结构体作为头结点
  Token *cur = &head;  // 创建一个指针cur指向头结点

  for (; tok1->kind != TK_EOF; tok1 = tok1->next)  // 遍历tok1链表，直到到达文件末尾
    cur = cur->next = copy_token(tok1);  // 复制当前节点，并将cur指针指向下一个节点
  cur->next = tok2;  // 将tok2连接到tok1链表的末尾
  return head.next;  // 返回头结点的下一个节点，即新生成的链表
}

static Token *skip_cond_incl2(Token *tok) {
  while (tok->kind != TK_EOF) {  // 循环直到tok的类型为TK_EOF（文件末尾）
    if (is_hash(tok) &&  // 如果tok是预处理指令的#符号
        (equal(tok->next, "if") || equal(tok->next, "ifdef") ||  // 并且下一个token是"if"、"ifdef"或"ifndef"
         equal(tok->next, "ifndef"))) {
      tok = skip_cond_incl2(tok->next->next);  // 跳过条件包含的部分，递归调用skip_cond_incl2函数
      continue;  // 继续下一次循环
    }
    if (is_hash(tok) && equal(tok->next, "endif"))  // 如果tok是预处理指令的#符号，并且下一个token是"endif"
      return tok->next->next;  // 返回endif之后的下一个token
    tok = tok->next;  // 否则，将tok指向下一个token
  }
  return tok;  // 返回当前的tok
}

// 跳过直到下一个`#else`、`#elif`或`#endif`的位置。
// 嵌套的`#if`和`#endif`会被跳过。
static Token *skip_cond_incl(Token *tok) {
  while (tok->kind != TK_EOF) {  // 循环直到tok的类型为TK_EOF（文件末尾）
    if (is_hash(tok) &&  // 如果tok是预处理指令的#符号
        (equal(tok->next, "if") || equal(tok->next, "ifdef") ||  // 并且下一个token是"if"、"ifdef"或"ifndef"
         equal(tok->next, "ifndef"))) {
      tok = skip_cond_incl2(tok->next->next);  // 跳过条件包含的部分，递归调用skip_cond_incl2函数
      continue;  // 继续下一次循环
    }

    if (is_hash(tok) &&  // 如果tok是预处理指令的#符号
        (equal(tok->next, "elif") || equal(tok->next, "else") ||  // 并且下一个token是"elif"、"else"或"endif"
         equal(tok->next, "endif")))
      break;  // 退出循环
    tok = tok->next;  // 否则，将tok指向下一个token
  }
  return tok;  // 返回当前的tok
}

// 给定一个字符串，将其用双引号括起来并返回。
static char *quote_string(char *str) {
  int bufsize = 3;  // 初始缓冲区大小为3（包括双引号和空字符结尾）

  for (int i = 0; str[i]; i++) {  // 遍历字符串
    if (str[i] == '\\' || str[i] == '"')  // 如果字符是反斜杠或双引号
      bufsize++;  // 缓冲区大小加1
    bufsize++;  // 缓冲区大小加1
  }

  char *buf = calloc(1, bufsize);  // 分配缓冲区内存
  char *p = buf;  // 创建指针p指向缓冲区

  *p++ = '"';  // 在缓冲区中添加双引号起始符
  for (int i = 0; str[i]; i++) {  // 遍历字符串
    if (str[i] == '\\' || str[i] == '"')  // 如果字符是反斜杠或双引号
      *p++ = '\\';  // 在缓冲区中添加反斜杠转义符
    *p++ = str[i];  // 在缓冲区中添加当前字符
  }
  *p++ = '"';  // 在缓冲区中添加双引号结束符
  *p++ = '\0';  // 在缓冲区中添加空字符结尾
  return buf;  // 返回缓冲区指针
}

static Token *new_str_token(char *str, Token *tmpl) {
  char *buf = quote_string(str);  // 为输入字符串创建一个引号包围的副本
  return tokenize(new_file(tmpl->file->name, tmpl->file->file_no, buf));  // 对副本进行分词并返回结果
}

// 复制所有直到下一个换行符的标记，用 EOF 标记终止它们，然后返回它们。
// 此函数用于为 `#if` 参数创建一个新的标记列表。
static Token *copy_line(Token **rest, Token *tok) {
  Token head = {};  // 创建一个空的 Token 结构作为头节点
  Token *cur = &head;  // cur 指针指向头节点

  for (; !tok->at_bol; tok = tok->next)  // 遍历直到 tok 指向行首标记
    cur = cur->next = copy_token(tok);  // 复制当前标记，并将其添加到链表中

  cur->next = new_eof(tok);  // 在链表末尾添加一个 EOF 标记
  *rest = tok;  // 将剩余的标记赋值给 rest 指针
  return head.next;  // 返回链表的第一个标记
}

static Token *new_num_token(int val, Token *tmpl) {
  char *buf = format("%d\n", val);  // 将整数 val 格式化为字符串，并添加换行符
  return tokenize(new_file(tmpl->file->name, tmpl->file->file_no, buf));  // 对字符串进行分词并返回结果
}

static Token *read_const_expr(Token **rest, Token *tok) {
  tok = copy_line(rest, tok);  // 复制一行标记，并将剩余标记赋值给 rest

  Token head = {};  // 创建一个空的 Token 结构作为头节点
  Token *cur = &head;  // cur 指针指向头节点

  while (tok->kind != TK_EOF) {  // 循环直到遇到 EOF 标记
    // 如果是 "defined(foo)" 或者 "defined foo"，则返回宏 "foo" 是否定义的结果（1 或 0）
    if (equal(tok, "defined")) {
      Token *start = tok;  // 记录 "defined" 的起始标记
      bool has_paren = consume(&tok, tok->next, "(");  // 判断是否有括号包围

      if (tok->kind != TK_IDENT)
        error_tok(start, "macro name must be an identifier");  // 错误处理：宏名必须是标识符
      Macro *m = find_macro(tok);  // 在宏表中查找宏名对应的宏
      tok = tok->next;  // 更新 tok 指针

      if (has_paren)
        tok = skip(tok, ")");  // 跳过括号

      cur = cur->next = new_num_token(m ? 1 : 0, start);  // 根据宏名是否定义创建一个整数类型的 Token，并将其添加到链表中
      continue;
    }

    cur = cur->next = tok;  // 将当前标记添加到链表中
    tok = tok->next;  // 更新 tok 指针
  }

  cur->next = tok;  // 链接链表和剩余的标记
  return head.next;  // 返回链表的第一个标记
}

// 读取并计算一个常量表达式。
static long eval_const_expr(Token **rest, Token *tok) {
  Token *start = tok;  // 记录起始标记
  Token *expr = read_const_expr(rest, tok->next);  // 读取常量表达式，将剩余标记赋值给 rest
  expr = preprocess2(expr);  // 预处理表达式

  if (expr->kind == TK_EOF)
    error_tok(start, "no expression");  // 错误处理：没有表达式

    // [https://www.sigbus.info/n1570#6.10.1p4] 标准要求在计算常量表达式之前，
    // 我们必须用"0"替换剩余的非宏标识符。例如，如果foo未定义，那么`#if foo`等同于`#if 0`。
  for (Token *t = expr; t->kind != TK_EOF; t = t->next) {  // 遍历表达式中的每个令牌，直到遇到文件结束符
    if (t->kind == TK_IDENT) {  // 检查当前令牌的类型是否为标识符
      Token *next = t->next;  // 保存下一个令牌的指针
      *t = *new_num_token(0, t);  // 将当前标识符替换为值为0的新数字令牌，并保留其其他属性
      t->next = next;  // 恢复当前令牌的next指针为原来的值
    }
  }

  // 将预处理数字转换为常规数字
  convert_pp_tokens(expr);

  Token *rest2;  // 用于保存剩余的令牌
  long val = const_expr(&rest2, expr);  // 计算常量表达式的值，并将剩余的令牌保存在rest2中
  if (rest2->kind != TK_EOF)
    error_tok(rest2, "extra token");  // 如果剩余的令牌不是文件结束符，报错：多余的令牌
  return val;  // 返回常量表达式的值
}

// 定义静态函数：将条件包含信息压入条件包含链表
static CondIncl *push_cond_incl(Token *tok, bool included) {
  CondIncl *ci = calloc(1, sizeof(CondIncl));  // 分配内存并初始化CondIncl结构体，用于保存条件包含信息
  ci->next = cond_incl;  // 将新的CondIncl节点插入链表头部
  ci->ctx = IN_THEN;  // 设置条件包含的上下文为IN_THEN
  ci->tok = tok;  // 设置条件包含的令牌为给定的令牌
  ci->included = included;  // 设置条件包含的状态为included
  cond_incl = ci;  // 更新全局变量cond_incl为新的链表头
  return ci;  // 返回新创建的CondIncl节点
}

// 定义静态函数：查找宏定义
static Macro *find_macro(Token *tok) {
  if (tok->kind != TK_IDENT)
    return NULL;  // 如果给定的令牌不是标识符类型，直接返回空指针
  return hashmap_get2(&macros, tok->loc, tok->len);  // 在宏定义哈希表中查找给定令牌的宏定义，并返回结果
}

// 定义静态函数：添加宏定义
static Macro *add_macro(char *name, bool is_objlike, Token *body) {
  Macro *m = calloc(1, sizeof(Macro));  // 分配内存并初始化Macro结构体，用于保存宏定义信息
  m->name = name;  // 设置宏定义的名称
  m->is_objlike = is_objlike;  // 设置宏定义的类型（对象宏或函数宏）
  m->body = body;  // 设置宏定义的替换体（宏展开后的内容）
  hashmap_put(&macros, name, m);  // 将宏定义添加到宏定义哈希表中
  return m;  // 返回新添加的宏定义
}

// 定义静态函数：读取宏定义的参数列表
static MacroParam *read_macro_params(Token **rest, Token *tok, char **va_args_name) {
  MacroParam head = {};  // 定义参数链表的头节点
  MacroParam *cur = &head;  // 当前参数节点指针

  while (!equal(tok, ")")) {  // 循环读取参数直到遇到右括号结束
    if (cur != &head)
      tok = skip(tok, ",");  // 跳过逗号分隔符

    if (equal(tok, "...")) {  // 如果遇到省略号参数
      *va_args_name = "__VA_ARGS__";  // 设置可变参数的名称为"__VA_ARGS__"
      *rest = skip(tok->next, ")");  // 跳过右括号，并将剩余的令牌保存在rest中
      return head.next;  // 返回参数链表的头节点
    }

    if (tok->kind != TK_IDENT)
      error_tok(tok, "expected an identifier");  // 报错：预期标识符

    if (equal(tok->next, "...")) {  // 如果下一个令牌是省略号参数
      *va_args_name = strndup(tok->loc, tok->len);  // 设置可变参数的名称为当前标识符的字符串表示
      *rest = skip(tok->next->next, ")");  // 跳过右括号，并将剩余的令牌保存在rest中
      return head.next;  // 返回参数链表的头节点
    }

    MacroParam *m = calloc(1, sizeof(MacroParam));  // 分配内存并初始化参数节点
    m->name = strndup(tok->loc, tok->len);  // 设置参数节点的名称为当前标识符的字符串表示
    cur = cur->next = m;  // 将参数节点添加到链表中，并更新当前节点指针
    tok = tok->next;  // 跳到下一个令牌
  }

  *rest = tok->next;  // 跳过右括号，并将剩余的令牌保存在rest中
  return head.next;  // 返回参数链表的头节点
}

// 定义静态函数：读取宏定义
static void read_macro_definition(Token **rest, Token *tok) {
  if (tok->kind != TK_IDENT)
    error_tok(tok, "macro name must be an identifier");  // 报错：宏名称必须是标识符
  char *name = strndup(tok->loc, tok->len);  // 将宏名称保存为字符串
  tok = tok->next;  // 跳到下一个令牌

  if (!tok->has_space && equal(tok, "(")) {
    // 函数宏
    char *va_args_name = NULL;  // 可变参数的名称
    MacroParam *params = read_macro_params(&tok, tok->next, &va_args_name);  // 读取宏定义的参数列表

    Macro *m = add_macro(name, false, copy_line(rest, tok));  // 添加函数宏定义
    m->params = params;  // 设置函数宏的参数列表
    m->va_args_name = va_args_name;  // 设置函数宏的可变参数名称
  } else {
    // 对象宏
    add_macro(name, true, copy_line(rest, tok));  // 添加对象宏定义
  }
}

// 定义静态函数：读取宏参数中的一个参数
static MacroArg *read_macro_arg_one(Token **rest, Token *tok, bool read_rest) {
  Token head = {};  // 参数链表的头节点
  Token *cur = &head;  // 当前参数节点指针
  int level = 0;  // 括号的层级

  for (;;) {  // 循环读取参数
    if (level == 0 && equal(tok, ")"))
      break;  // 如果括号层级为0并且遇到右括号，则结束循环
    if (level == 0 && !read_rest && equal(tok, ","))
      break;  // 如果括号层级为0并且不需要读取剩余参数，并且遇到逗号，则结束循环

    if (tok->kind == TK_EOF)
      error_tok(tok, "premature end of input");  // 报错：输入过早结束

    if (equal(tok, "("))
      level++;  // 遇到左括号，括号层级加1
    else if (equal(tok, ")"))
      level--;  // 遇到右括号，括号层级减1

    cur = cur->next = copy_token(tok);  // 将当前令牌复制并添加到参数链表中
    tok = tok->next;  // 跳到下一个令牌
  }

  cur->next = new_eof(tok);  // 在参数链表的最后添加一个EOF令牌

  MacroArg *arg = calloc(1, sizeof(MacroArg));  // 分配内存并初始化参数节点
  arg->tok = head.next;  // 设置参数节点的令牌为参数链表的头节点
  *rest = tok;  // 将剩余的令牌保存在rest中
  return arg;  // 返回参数节点
}

// 定义静态函数：读取宏参数
static MacroArg *read_macro_args(Token **rest, Token *tok, MacroParam *params, char *va_args_name) {
  Token *start = tok;  // 记录参数的起始位置
  tok = tok->next->next;  // 跳过参数列表的左括号和第一个参数的逗号

  MacroArg head = {};  // 定义参数链表的头节点
  MacroArg *cur = &head;  // 当前参数节点指针

  MacroParam *pp = params;  // 宏参数链表的指针
  for (; pp; pp = pp->next) {  // 遍历宏参数链表
    if (cur != &head)
      tok = skip(tok, ",");  // 跳过逗号

    cur = cur->next = read_macro_arg_one(&tok, tok, false);  // 读取一个宏参数
    cur->name = pp->name;  // 设置宏参数的名称
  }

  if (va_args_name) {  // 如果存在可变参数名称
    MacroArg *arg;  // 定义一个宏参数节点指针

    if (equal(tok, ")")) {  // 如果当前令牌是右括号
      arg = calloc(1, sizeof(MacroArg));  // 分配内存并初始化宏参数节点
      arg->tok = new_eof(tok);  // 设置宏参数节点的令牌为一个新的EOF令牌
    } else {
      if (pp != params)
        tok = skip(tok, ",");  // 跳过逗号

      arg = read_macro_arg_one(&tok, tok, true);  // 读取一个宏参数
    }

    arg->name = va_args_name;  // 设置宏参数的名称
    arg->is_va_args = true;  // 标记宏参数为可变参数
    cur = cur->next = arg;  // 将宏参数节点添加到参数链表中
  } else if (pp) {
    error_tok(start, "too many arguments");  // 报错：参数过多
  }

  skip(tok, ")");  // 跳过右括号
  *rest = tok;  // 将剩余的令牌保存在rest中
  return head.next;  // 返回参数链表的头节点的下一个节点
}

// 定义静态函数：查找参数
static MacroArg *find_arg(MacroArg *args, Token *tok) {
  for (MacroArg *ap = args; ap; ap = ap->next)  // 遍历参数链表
    if (tok->len == strlen(ap->name) && !strncmp(tok->loc, ap->name, tok->len))  // 判断令牌的长度和名称是否匹配
      return ap;  // 返回匹配的参数节点
  return NULL;  // 如果未找到匹配的参数，返回NULL
}

// 定义静态函数：连接所有令牌并返回一个新的字符串
static char *join_tokens(Token *tok, Token *end) {
  // 计算结果令牌的长度
  int len = 1;
  for (Token *t = tok; t != end && t->kind != TK_EOF; t = t->next) {
    if (t != tok && t->has_space)
      len++;  // 如果令牌之间有空格，长度加1
    len += t->len;  // 加上令牌的长度
  }

  char *buf = calloc(1, len);  // 分配内存空间

  // 复制令牌文本
  int pos = 0;
  for (Token *t = tok; t != end && t->kind != TK_EOF; t = t->next) {
    if (t != tok && t->has_space)
      buf[pos++] = ' ';  // 如果令牌之间有空格，添加一个空格到结果字符串中
    strncpy(buf + pos, t->loc, t->len);  // 复制令牌的内容到结果字符串中
    pos += t->len;  // 更新位置指针
  }
  buf[pos] = '\0';  // 在结果字符串的末尾添加空字符
  return buf;  // 返回结果字符串
}

// 定义静态函数：连接`arg`中的所有令牌并返回一个新的字符串令牌
// 该函数用于字符串化操作符（#）
static Token *stringize(Token *hash, Token *arg) {  // 定义静态函数：连接`arg`中的所有令牌并返回一个新的字符串令牌

    // 创建一个新的字符串令牌。我们需要为其源位置设置一些值，以便进行错误报告函数，因此我们使用宏名称令牌作为模板。
  char *s = join_tokens(arg, NULL);  // 调用`join_tokens`函数将`arg`中的所有令牌连接成一个新的字符串
  return new_str_token(s, hash);  // 调用`new_str_token`函数创建一个新的字符串令牌，并将连接后的字符串作为文本内容
                                   // 传递连接后的字符串`s`和`hash`令牌作为参数
}


// 连接两个令牌，创建一个新的令牌
static Token *paste(Token *lhs, Token *rhs) {  // 连接两个令牌，创建一个新的令牌
  // 将两个令牌连接起来
  char *buf = format("%.*s%.*s", lhs->len, lhs->loc, rhs->len, rhs->loc);  // 将两个令牌的内容连接起来，存储在缓冲区`buf`中
  // 将结果字符串进行令牌化
  Token *tok = tokenize(new_file(lhs->file->name, lhs->file->file_no, buf));  // 对连接后的字符串进行令牌化，创建一个新的文件结构体，并将连接后的字符串作为内容

  if (tok->next->kind != TK_EOF)  // 检查令牌链表的下一个令牌是否为EOF令牌，如果不是，则表示连接的结果是一个无效的令牌
    error_tok(lhs, "pasting forms '%s', an invalid token", buf);  // 报告错误，连接的结果是一个无效的令牌

  return tok;  // 返回令牌链表的头节点
}

static bool has_varargs(MacroArg *args) {  // 判断宏参数链表中是否存在可变参数（__VA_ARGS__）

  for (MacroArg *ap = args; ap; ap = ap->next)  // 遍历宏参数链表，查找是否存在名称为"__VA_ARGS__"的参数
    if (!strcmp(ap->name, "__VA_ARGS__"))
      return ap->tok->kind != TK_EOF;  // 如果找到了"__VA_ARGS__"参数，则判断其令牌类型是否为EOF

  return false;  // 如果没有找到"__VA_ARGS__"参数，则返回false
}

// 用给定的实参替换函数式宏的形参
static Token *subst(Token *tok, MacroArg *args) {  // 用给定的参数替换函数式宏的参数
  Token head = {};  // 创建一个空的令牌作为头节点
  Token *cur = &head;  // 用于指向当前令牌的指针

  while (tok->kind != TK_EOF) {  // 循环处理令牌，直到遇到EOF令牌

    // 如果当前令牌是"#"，并且紧跟着一个宏形参，则将其替换为参数的字符串化结果
    if (equal(tok, "#")) {
      MacroArg *arg = find_arg(args, tok->next);  // 查找与宏参数匹配的参数
      if (!arg)
        error_tok(tok->next, "'#' is not followed by a macro parameter");  // 报告错误，"#"后面没有宏参数
      cur = cur->next = stringize(tok, arg->tok);  // 将字符串化后的结果作为新的令牌添加到链表中
      tok = tok->next->next;  // 跳过"#"和宏参数，继续处理下一个令牌
      continue;
    }
    
    // [GNU] 如果__VA_ARGS__为空，则`,##__VA_ARGS__`被展开为空的令牌列表。
    // 否则，它被展开为`,`和__VA_ARGS__。
    if (equal(tok, ",") && equal(tok->next, "##")) {  // 如果当前令牌是","，并且紧跟着"##"
      MacroArg *arg = find_arg(args, tok->next->next);  // 查找与宏参数匹配的参数
      if (arg && arg->is_va_args) {  // 如果找到了匹配的参数，并且是可变参数（__VA_ARGS__）
        if (arg->tok->kind == TK_EOF) {  // 如果参数的令牌类型是EOF
          tok = tok->next->next->next;  // 跳过",", "##"和参数的EOF，继续处理下一个令牌
        } else {
          cur = cur->next = copy_token(tok);  // 将当前令牌复制并添加到链表中
          tok = tok->next->next;  // 跳过",", "##"，继续处理下一个令牌
        }
        continue;
      }
    }

    if (equal(tok, "##")) {  // 如果当前令牌是"##"
      if (cur == &head)
        error_tok(tok, "'##' cannot appear at start of macro expansion");  // 报告错误，"##"不能出现在宏展开的开头

      if (tok->next->kind == TK_EOF)
        error_tok(tok, "'##' cannot appear at end of macro expansion");  // 报告错误，"##"不能出现在宏展开的末尾

      MacroArg *arg = find_arg(args, tok->next);  // 查找与宏参数匹配的参数
      if (arg) {
        if (arg->tok->kind != TK_EOF) {
          *cur = *paste(cur, arg->tok);  // 将当前令牌和参数的令牌连接起来
          for (Token *t = arg->tok->next; t->kind != TK_EOF; t = t->next)
            cur = cur->next = copy_token(t);  // 复制参数的剩余令牌并添加到链表中
        }
        tok = tok->next->next;  // 跳过"##"和参数，继续处理下一个令牌
        continue;
      }

      *cur = *paste(cur, tok->next);  // 将当前令牌和"##"后面的令牌连接起来
      tok = tok->next->next;  // 跳过"##"和令牌，继续处理下一个令牌
      continue;
    }

    MacroArg *arg = find_arg(args, tok);  // 查找与当前令牌匹配的宏参数

    if (arg && equal(tok->next, "##")) {  // 如果找到了宏参数，并且下一个令牌是"##"
      Token *rhs = tok->next->next;  // 获取"##"后面的令牌

      if (arg->tok->kind == TK_EOF) {  // 如果宏参数的令牌类型是EOF
        MacroArg *arg2 = find_arg(args, rhs);  // 查找与rhs匹配的宏参数
        if (arg2) {
          for (Token *t = arg2->tok; t->kind != TK_EOF; t = t->next)
            cur = cur->next = copy_token(t);  // 复制arg2的令牌并添加到链表中
        } else {
          cur = cur->next = copy_token(rhs);  // 复制rhs的令牌并添加到链表中
        }
        tok = rhs->next;  // 跳过rhs和它的下一个令牌，继续处理下一个令牌
        continue;
      }

      for (Token *t = arg->tok; t->kind != TK_EOF; t = t->next)
        cur = cur->next = copy_token(t);  // 复制宏参数的令牌并添加到链表中
      tok = tok->next;  // 跳过"##"，继续处理下一个令牌
      continue;
    }

    // 如果__VA_ARGS__为空，则__VA_OPT__(x)被展开为空的令牌列表。
    // 否则，__VA_OPT__(x)被展开为x。
    if (equal(tok, "__VA_OPT__") && equal(tok->next, "(")) {  // 如果当前令牌是"__VA_OPT__"，并且紧跟着"("
      MacroArg *arg = read_macro_arg_one(&tok, tok->next->next, true);  // 读取__VA_OPT__的参数
      if (has_varargs(args))  // 如果宏定义中有可变参数（__VA_ARGS__）
        for (Token *t = arg->tok; t->kind != TK_EOF; t = t->next)
          cur = cur->next = t;  // 将参数的令牌添加到链表中
      tok = skip(tok, ")");  // 跳过")"，继续处理下一个令牌
      continue;
    }

    // 处理宏令牌。在将宏参数替换到宏体之前，宏参数要先进行完全的宏展开。
    if (arg) {  // 如果找到了宏参数
      Token *t = preprocess2(arg->tok);  // 对宏参数进行预处理和宏展开
      t->at_bol = tok->at_bol;  // 设置宏参数的换行标志与当前令牌一致
      t->has_space = tok->has_space;  // 设置宏参数的空格标志与当前令牌一致
      for (; t->kind != TK_EOF; t = t->next)
        cur = cur->next = copy_token(t);  // 复制宏参数的令牌并添加到链表中
      tok = tok->next;  // 跳过宏参数，继续处理下一个令牌
      continue;
    }

    // 处理非宏令牌。
    cur = cur->next = copy_token(tok);  // 复制当前令牌并添加到链表中
    tok = tok->next;  // 继续处理下一个令牌
    continue;
  }

  cur->next = tok;  // 将当前链表的末尾指向剩余的令牌链表
  return head.next;  // 返回处理后的令牌链表的头部
}

// 如果tok是一个宏，进行宏展开并返回true。
// 否则，不进行任何操作并返回false。
static bool expand_macro(Token **rest, Token *tok) {
  if (hideset_contains(tok->hideset, tok->loc, tok->len))
    return false;  // 如果tok在隐藏集中，则不进行宏展开，返回false

  Macro *m = find_macro(tok);  // 查找与tok匹配的宏
  if (!m)
    return false;  // 如果找不到匹配的宏，则不进行宏展开，返回false
  // 内置的动态宏应用，如__LINE__
  if (m->handler) {  // 如果宏具有处理函数
    *rest = m->handler(tok);  // 使用处理函数处理宏
    (*rest)->next = tok->next;  // 将处理结果连接到tok的下一个令牌
    return true;  // 返回true表示成功进行宏展开
  }

  // 对象宏应用
  if (m->is_objlike) {  // 如果宏是对象宏
    Hideset *hs = hideset_union(tok->hideset, new_hideset(m->name));  // 计算新的隐藏集
    Token *body = add_hideset(m->body, hs);  // 将宏体添加隐藏集
    for (Token *t = body; t->kind != TK_EOF; t = t->next)
      t->origin = tok;  // 设置宏体中的令牌的原始令牌为当前令牌
    *rest = append(body, tok->next);  // 将宏展开结果与当前令牌的下一个令牌连接起来
    (*rest)->at_bol = tok->at_bol;  // 设置宏展开结果的换行标志与当前令牌一致
    (*rest)->has_space = tok->has_space;  // 设置宏展开结果的空格标志与当前令牌一致
    return true;  // 返回true表示成功进行宏展开
  }

  // 如果函数宏令牌后面不是参数列表，则将其视为普通标识符。
  if (!equal(tok->next, "("))
    return false;  // 如果下一个令牌不是"("，则不进行函数宏展开，返回false。

  // 函数宏应用
  Token *macro_token = tok;  // 保存函数宏的令牌
  MacroArg *args = read_macro_args(&tok, tok, m->params, m->va_args_name);  // 读取函数宏的参数列表
  Token *rparen = tok;  // 保存参数列表的结束括号

  // 构成函数宏调用的令牌可能具有不同的隐藏集，如果是这种情况，则不清楚新令牌的隐藏集应该是什么。
  // 我们采用宏令牌和右括号的交集作为新的隐藏集，按照Dave Prossor的算法解释。
  Hideset *hs = hideset_intersection(macro_token->hideset, rparen->hideset);  // 计算宏令牌和右括号的隐藏集的交集
  hs = hideset_union(hs, new_hideset(m->name));  // 将宏名称的隐藏集与交集进行合并

  Token *body = subst(m->body, args);  // 对宏体进行参数替换
  body = add_hideset(body, hs);  // 将隐藏集添加到宏展开的结果中
  for (Token *t = body; t->kind != TK_EOF; t = t->next)
    t->origin = macro_token;  // 设置宏展开结果中的令牌的原始令牌为宏令牌
  *rest = append(body, tok->next);  // 将宏展开结果与当前令牌的下一个令牌连接起来
  (*rest)->at_bol = macro_token->at_bol;  // 设置宏展开结果的换行标志与宏令牌一致
  (*rest)->has_space = macro_token->has_space;  // 设置宏展开结果的空格标志与宏令牌一致
  return true;  // 返回true表示成功进行宏展开
}

char *search_include_paths(char *filename) {
  if (filename[0] == '/')
    return filename;  // 如果文件名以斜杠开头，则直接返回文件名

  static HashMap cache;  // 静态哈希映射用于缓存搜索结果
  char *cached = hashmap_get(&cache, filename);  // 从缓存中查找文件名对应的路径
  if (cached)
    return cached;  // 如果在缓存中找到了路径，则直接返回缓存中的结果

  // 从包含路径中搜索文件。
  for (int i = 0; i < include_paths.len; i++) {
    char *path = format("%s/%s", include_paths.data[i], filename);  // 构造搜索路径
    if (!file_exists(path))
      continue;  // 如果路径不存在，则继续搜索下一个路径
    hashmap_put(&cache, filename, path);  // 将搜索结果加入缓存
    include_next_idx = i + 1;  // 更新下一个搜索路径的索引
    return path;  // 返回找到的文件路径
  }
  return NULL;  // 如果未找到文件，则返回空指针
}

static char *search_include_next(char *filename) {
  for (; include_next_idx < include_paths.len; include_next_idx++) {
    char *path = format("%s/%s", include_paths.data[include_next_idx], filename);  // 构造下一个搜索路径，将包含路径和文件名拼接起来，形成完整的路径

    if (file_exists(path))
      return path;  // 如果路径存在，则返回找到的文件路径
  }
  return NULL;  // 如果未找到文件，则返回空指针
}

// 读取#include语句的实参（文件名）。
static char *read_include_filename(Token **rest, Token *tok, bool *is_dquote) {
  // 模式1：#include "foo.h"
  if (tok->kind == TK_STR) {
    // 双引号括起的文件名在#include语句中是一种特殊的令牌，
    // 我们不希望解释其中的转义序列。
    // 例如，在"C:\foo"中，"\f"不是一个换页符，而只是两个非控制字符，即反斜杠和f。
    // 因此，我们不希望使用token->str。
    *is_dquote = true;  // 设置双引号标志为true
    *rest = skip_line(tok->next);  // 跳过当前行剩余的令牌
    return strndup(tok->loc + 1, tok->len - 2);  // 返回去掉双引号的文件名字符串副本
  }


  // 模式2：#include <foo.h>
  if (equal(tok, "<")) {
    // 从"<"和">"之间的令牌序列中重构文件名。
    Token *start = tok;  // 记录起始位置的令牌

    // 查找闭合的">"。
    for (; !equal(tok, ">"); tok = tok->next) {
      if (tok->at_bol || tok->kind == TK_EOF)
        error_tok(tok, "expected '>'");  // 如果找不到闭合的">"，则报错
    }

    *is_dquote = false;  // 设置双引号标志为false
    *rest = skip_line(tok->next);  // 跳过当前行剩余的令牌
    return join_tokens(start->next, tok);  // 将起始位置和结束位置之间的令牌拼接成文件名
  }

  // 模式3：#include FOO
  // 在这种情况下，FOO必须被宏展开为单个字符串令牌或一系列"<" ... ">"。
  if (tok->kind == TK_IDENT) {
    Token *tok2 = preprocess2(copy_line(rest, tok));  // 对FOO进行宏展开，并获取展开后的令牌序列
    return read_include_filename(&tok2, tok2, is_dquote);  // 继续读取展开后的文件名
  }

  error_tok(tok, "expected a filename");  // 报错，期望一个文件名
}

// 检测以下的"include guard"模式。
//
//   #ifndef FOO_H
//   #define FOO_H
//   ...
//   #endif
static char *detect_include_guard(Token *tok) {  // 静态函数，用于检测包含保护符
  // 检测前两行。
  if (!is_hash(tok) || !equal(tok->next, "ifndef"))  // 如果当前token不是预处理指令的#符号，或者下一个token不是"ifndef"关键字
    return NULL;  // 返回NULL，表示未检测到包含保护符
  tok = tok->next->next;

  if (tok->kind != TK_IDENT)  // 如果当前token不是标识符
    return NULL;  // 返回NULL，表示未检测到标识符
  char *macro = strndup(tok->loc, tok->len);  // 将标识符字符串复制到宏变量中，使用strndup函数可以防止内存溢出
  tok = tok->next;

  if (!is_hash(tok) || !equal(tok->next, "define") || !equal(tok->next->next, macro))
    // 如果当前token不是预处理指令的#符号，或者下一个token不是"define"关键字，或者下下个token不等于宏变量
    return NULL;  // 返回NULL，表示未检测到定义宏的语句


  // Read until the end of the file. 读取直到文件末尾
  while (tok->kind != TK_EOF) {  // 当 tok 的类型不是 TK_EOF 时循环执行以下代码
    if (!is_hash(tok)) {  // 如果 tok 不是 "#"，则跳过此行
      tok = tok->next;
      continue;
    }

    if (equal(tok->next, "endif") && tok->next->next->kind == TK_EOF)  // 如果下一个 tok 是 "endif"，且下下一个 tok 的类型是 TK_EOF，则返回宏定义
      return macro;

    if (equal(tok, "if") || equal(tok, "ifdef") || equal(tok, "ifndef"))  // 如果 tok 是 "if"、"ifdef" 或 "ifndef"，则跳过条件包含部分
      tok = skip_cond_incl(tok->next);
    else
      tok = tok->next;  // 否则，继续向下一个 tok
  }
  return NULL;  // 返回空指针
}

static Token *include_file(Token *tok, char *path, Token *filename_tok) {
  // 检查是否有 "#pragma once"
  if (hashmap_get(&pragma_once, path))  // 如果在 pragma_once 哈希表中找到了路径，则返回 tok

    return tok;

  // 如果我们之前读取过相同的文件，并且该文件由通常的 #ifndef ... #endif 模式保护，
  // 我们可以尝试跳过打开文件的步骤。
  static HashMap include_guards;  // 静态哈希表用于存储 include guards
  char *guard_name = hashmap_get(&include_guards, path);  // 获取路径对应的 include guard 名称
  if (guard_name && hashmap_get(&macros, guard_name))  // 如果 guard_name 存在，并且在宏哈希表中找到了对应的宏定义，则返回 tok
    return tok;

  Token *tok2 = tokenize_file(path);  // 对文件进行词法分析，返回 token 流
  if (!tok2)
    error_tok(filename_tok, "%s: cannot open file: %s", path, strerror(errno));  // 如果无法打开文件，则报错

  guard_name = detect_include_guard(tok2);  // 检测文件中的 include guard
  if (guard_name)
    hashmap_put(&include_guards, path, guard_name);  // 将路径和 guard_name 存入 include_guards 哈希表

  return append(tok2, tok);  // 将 tok2 追加到 tok 后面，并返回
}

// 读取 #line 实参
static void read_line_marker(Token **rest, Token *tok) {
  Token *start = tok;  // 记录起始的 token
  tok = preprocess(copy_line(rest, tok));  // 复制一行 token 并进行预处理

  if (tok->kind != TK_NUM || tok->ty->kind != TY_INT)  // 如果 tok 不是数字或者类型不是整型，则报错
    error_tok(tok, "invalid line marker");
  start->file->line_delta = tok->val - start->line_no;  // 更新文件行数的增量

  tok = tok->next;
  if (tok->kind == TK_EOF)  // 如果 tok 是 TK_EOF，则返回
    return;

  if (tok->kind != TK_STR)  // 如果 tok 不是字符串，则报错
    error_tok(tok, "filename expected");
  start->file->display_name = tok->str;  // 更新文件的显示名称
}

// 在评估预处理宏和指令时访问 `tok` 中的所有标记。
static Token *preprocess2(Token *tok) {
  Token head = {};  // 创建一个头节点
  Token *cur = &head;  // 指向当前节点的指针

  while (tok->kind != TK_EOF) {  // 当 tok 的类型不是 TK_EOF 时循环执行以下代码
    // 如果是宏，则展开它。
    if (expand_macro(&tok, tok))  // 如果展开了宏，则继续循环
      continue;

    // 如果不是 "#”，则直接通过。
    if (!is_hash(tok)) {  // 如果 tok 不是 "#"，则将其添加到链表中
      tok->line_delta = tok->file->line_delta;  // 更新行号增量
      tok->filename = tok->file->display_name;  // 更新文件名
      cur = cur->next = tok;  // 将 tok 添加到链表中
      tok = tok->next;  // 继续下一个 tok
      continue;
    }

    Token *start = tok;  // 定义名为start的指针变量，并将其初始化为tok的值
    tok = tok->next;  // 将tok指向当前tok节点的下一个节点

    if (equal(tok, "include")) {  // 判断tok的值是否与字符串"include"相等
      bool is_dquote;  // 声明一个布尔变量is_dquote
      char *filename = read_include_filename(&tok, tok->next, &is_dquote);  // 调用read_include_filename函数，传入参数tok、tok的下一个节点以及is_dquote的地址，并将返回值赋给filename

      if (filename[0] != '/' && is_dquote) {  // 判断filename的第一个字符是否不是'/'并且is_dquote为真
        char *path = format("%s/%s", dirname(strdup(start->file->name)), filename);  // 使用format函数格式化生成路径字符串，并赋给path。路径由start所在文件的目录名和filename组成
        if (file_exists(path)) {  // 判断路径是否存在
          tok = include_file(tok, path, start->next->next);  // 调用include_file函数，传入参数tok、path和start的下两个节点，并将返回值赋给tok
          continue;  // 跳过当前循环的剩余代码，继续下一次循环
        }
      }

      char *path = search_include_paths(filename);  // 调用search_include_paths函数，传入参数filename，并将返回值赋给path
      tok = include_file(tok, path ? path : filename, start->next->next);  // 调用include_file函数，传入参数tok、path（如果path不为空则使用path，否则使用filename）和start的下两个节点，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "include_next")) {  // 如果tok的值与"include_next"相等
      bool ignore;  // 声明一个布尔变量ignore
      char *filename = read_include_filename(&tok, tok->next, &ignore);  // 调用read_include_filename函数，传入参数tok、tok的下一个节点以及ignore的地址，并将返回值赋给filename
      char *path = search_include_next(filename);  // 调用search_include_next函数，传入参数filename，并将返回值赋给path
      tok = include_file(tok, path ? path : filename, start->next->next);  // 调用include_file函数，传入参数tok、path（如果path不为空则使用path，否则使用filename）和start的下两个节点，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "define")) {  // 如果tok的值与"define"相等
      read_macro_definition(&tok, tok->next);  // 调用read_macro_definition函数，传入参数tok和tok的下一个节点
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "undef")) {  // 如果tok的值与"undef"相等
      tok = tok->next;  // 将tok指向当前tok节点的下一个节点
      if (tok->kind != TK_IDENT)  // 如果tok的类型不是标识符
        error_tok(tok, "macro name must be an identifier");  // 报错，宏名必须是标识符
      undef_macro(strndup(tok->loc, tok->len));  // 调用undef_macro函数，传入参数为从tok的位置开始长度为tok的长度的字符串的副本
      tok = skip_line(tok->next);  // 调用skip_line函数，传入参数为tok的下一个节点，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "if")) {  // 如果tok的值与"if"相等
      long val = eval_const_expr(&tok, tok);  // 调用eval_const_expr函数，传入参数tok和tok，并将返回值赋给val
      push_cond_incl(start, val);  // 调用push_cond_incl函数，传入参数start和val
      if (!val)  // 如果val为假
        tok = skip_cond_incl(tok);  // 调用skip_cond_incl函数，传入参数tok，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "ifdef")) {  // 如果tok的值与"ifdef"相等
      bool defined = find_macro(tok->next);  // 调用find_macro函数，传入参数tok的下一个节点，并将返回值赋给defined
      push_cond_incl(tok, defined);  // 调用push_cond_incl函数，传入参数tok和defined
      tok = skip_line(tok->next->next);  // 调用skip_line函数，传入参数tok的下两个节点，并将返回值赋给tok
      if (!defined)  // 如果defined为假
        tok = skip_cond_incl(tok);  // 调用skip_cond_incl函数，传入参数tok，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "ifndef")) {  // 如果tok的值与"ifndef"相等
      bool defined = find_macro(tok->next);  // 调用find_macro函数，传入参数tok的下一个节点，并将返回值赋给defined
      push_cond_incl(tok, !defined);  // 调用push_cond_incl函数，传入参数tok和!defined
      tok = skip_line(tok->next->next);  // 调用skip_line函数，传入参数tok的下两个节点，并将返回值赋给tok
      if (defined)  // 如果defined为真
        tok = skip_cond_incl(tok);  // 调用skip_cond_incl函数，传入参数tok，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "elif")) {  // 如果tok的值与"elif"相等
      if (!cond_incl || cond_incl->ctx == IN_ELSE)  // 如果cond_incl为空或者cond_incl的ctx为IN_ELSE
        error_tok(start, "stray #elif");  // 报错，#elif没有匹配的#if或#elif
      cond_incl->ctx = IN_ELIF;  // 设置cond_incl的ctx为IN_ELIF

      if (!cond_incl->included && eval_const_expr(&tok, tok))  // 如果cond_incl尚未包含且eval_const_expr函数返回值为真
        cond_incl->included = true;  // 将cond_incl的included设置为真
      else
        tok = skip_cond_incl(tok);  // 调用skip_cond_incl函数，传入参数tok，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "else")) {  // 如果tok的值与"else"相等
      if (!cond_incl || cond_incl->ctx == IN_ELSE)  // 如果cond_incl为空或者cond_incl的ctx为IN_ELSE
        error_tok(start, "stray #else");  // 报错，#else没有匹配的#if或#elif
      cond_incl->ctx = IN_ELSE;  // 设置cond_incl的ctx为IN_ELSE
      tok = skip_line(tok->next);  // 调用skip_line函数，传入参数tok的下一个节点，并将返回值赋给tok

      if (cond_incl->included)  // 如果cond_incl已经包含
        tok = skip_cond_incl(tok);  // 调用skip_cond_incl函数，传入参数tok，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "endif")) {  // 如果tok的值与"endif"相等
      if (!cond_incl)  // 如果cond_incl为空
        error_tok(start, "stray #endif");  // 报错，#endif没有匹配的#if或#elif
      cond_incl = cond_incl->next;  // 将cond_incl指向下一个节点
      tok = skip_line(tok->next);  // 调用skip_line函数，传入参数tok的下一个节点，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "line")) {  // 如果tok的值与"line"相等
      read_line_marker(&tok, tok->next);  // 调用read_line_marker函数，传入参数tok和tok的下一个节点
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (tok->kind == TK_PP_NUM) {  // 如果tok的类型是TK_PP_NUM
      read_line_marker(&tok, tok);  // 调用read_line_marker函数，传入参数tok和tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "pragma") && equal(tok->next, "once")) {  // 如果tok的值与"pragma"相等且tok的下一个节点的值与"once"相等
      hashmap_put(&pragma_once, tok->file->name, (void *)1);  // 将tok->file->name作为键，1作为值插入到pragma_once哈希表中
      tok = skip_line(tok->next->next);  // 调用skip_line函数，传入参数tok的下两个节点，并将返回值赋给tok
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "pragma")) {  // 如果tok的值与"pragma"相等
      do {
        tok = tok->next;  // 将tok指向下一个节点
      } while (!tok->at_bol);  // 直到tok的at_bol为真
      continue;  // 跳过当前循环的剩余代码，继续下一次循环
    }

    if (equal(tok, "error"))  // 如果tok的值与"error"相等
      error_tok(tok, "error");  // 报错，输出错误信息为"error"

    // `#`-only line is legal. It's called a null directive.
    // 如果一行只有`#`，是合法的，称为空指令。
    if (tok->at_bol)  // 如果tok的at_bol为真，即位于行首
      continue;  // 跳过当前循环的剩余代码，继续下一次循环

    error_tok(tok, "invalid preprocessor directive");  // 报错，输出错误信息为"invalid preprocessor directive"
  }

  cur->next = tok;  // 将cur的下一个节点指向tok
  return head.next;  // 返回头节点的下一个节点
}

void define_macro(char *name, char *buf) {
  Token *tok = tokenize(new_file("<built-in>", 1, buf));
  add_macro(name, true, tok);
}
// 定义宏函数，接收两个参数：name（宏名称）和buf（宏内容字符串）
// 将buf字符串进行词法分析，生成Token序列
// 调用add_macro函数将宏名称、是否是内建宏、Token序列添加到宏列表中

void undef_macro(char *name) {
  hashmap_delete(&macros, name);
}
// 取消定义宏函数，接收一个参数：name（宏名称）
// 调用hashmap_delete函数从宏列表中删除指定名称的宏

static Macro *add_builtin(char *name, macro_handler_fn *fn) {
  Macro *m = add_macro(name, true, NULL);
  m->handler = fn;
  return m;
}
// 添加内建宏函数，接收两个参数：name（宏名称）和fn（宏处理函数）
// 调用add_macro函数将宏名称、是否是内建宏、NULL添加到宏列表中
// 设置宏的处理函数为传入的fn
// 返回添加的宏对象的指针

static Token *file_macro(Token *tmpl) {
  while (tmpl->origin)
    tmpl = tmpl->origin;
  return new_str_token(tmpl->file->display_name, tmpl);
}
// 文件宏函数，接收一个参数：tmpl（Token模板）
// 循环找到tmpl的起始Token
// 创建一个新的字符串Token，使用tmpl的文件显示名称作为字符串，tmpl作为原始Token
// 返回新创建的字符串Token

static Token *line_macro(Token *tmpl) {
  while (tmpl->origin)
    tmpl = tmpl->origin;
  int i = tmpl->line_no + tmpl->file->line_delta;
  return new_num_token(i, tmpl);
}
// 行号宏函数，接收一个参数：tmpl（Token模板）
// 循环找到tmpl的起始Token
// 计算行号，包括tmpl的行号和文件的行号偏移量
// 创建一个新的数字Token，使用计算得到的行号作为数值，tmpl作为原始Token
// 返回新创建的数字Token

// __COUNTER__被扩展为从0开始的连续值。
static Token *counter_macro(Token *tmpl) {
  static int i = 0; // 静态变量i用于记录计数器的当前值
  return new_num_token(i++, tmpl); // 创建一个新的数字Token，使用i的值作为数值，tmpl作为原始Token，然后将i自增
}

// __TIMESTAMP__被扩展为描述当前文件的最后修改时间的字符串。例如："Fri Jul 24 01:32:50 2020"
static Token *timestamp_macro(Token *tmpl) {
  struct stat st; // 定义一个stat结构体，用于存储文件的状态信息
  if (stat(tmpl->file->name, &st) != 0) // 获取tmpl所在文件的状态信息，如果失败则返回默认的时间字符串Token
    return new_str_token("??? ??? ?? ??:??:?? ????", tmpl);

  char buf[30]; // 创建一个字符数组，用于存储格式化后的时间字符串
  ctime_r(&st.st_mtime, buf); // 将文件的最后修改时间格式化为字符串，并存储在buf中
  buf[24] = '\0'; // 设置字符串的最后一个字符为'\0'，以去除无效字符
  return new_str_token(buf, tmpl); // 创建一个新的字符串Token，使用buf作为字符串，tmpl作为原始Token
}

static Token *base_file_macro(Token *tmpl) {
  return new_str_token(base_file, tmpl); // 创建一个新的字符串Token，使用base_file作为字符串，tmpl作为原始Token
}

// __DATE__被扩展为当前日期，例如："May 17 2020"。
static char *format_date(struct tm *tm) {
  static char mon[][4] = { // 定义一个静态字符数组，存储月份的缩写
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };

  return format("\"%s %2d %d\"", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
  // 格式化日期字符串，使用月份缩写、日期、年份作为参数，返回格式化后的字符串
}

// __TIME__被扩展为当前时间，例如："13:34:03"。
static char *format_time(struct tm *tm) {
  return format("\"%02d:%02d:%02d\"", tm->tm_hour, tm->tm_min, tm->tm_sec);
  // 格式化时间字符串，使用小时、分钟、秒作为参数，返回格式化后的字符串
}

void init_macros(void) {
  // 定义预定义宏
  define_macro("_LP64", "1");  // 表示 64 位环境
  define_macro("__C99_MACRO_WITH_VA_ARGS", "1");  // 启用 C99 标准带可变参数的宏
  define_macro("__ELF__", "1");  // 表示使用 ELF 格式的可执行文件
  define_macro("__LP64__", "1");  // 表示 64 位环境
  define_macro("__SIZEOF_DOUBLE__", "8");  // double 类型的大小为 8 字节
  define_macro("__SIZEOF_FLOAT__", "4");  // float 类型的大小为 4 字节
  define_macro("__SIZEOF_INT__", "4");  // int 类型的大小为 4 字节
  define_macro("__SIZEOF_LONG_DOUBLE__", "8");  // long double 类型的大小为 8 字节
  define_macro("__SIZEOF_LONG_LONG__", "8");  // long long 类型的大小为 8 字节
  define_macro("__SIZEOF_LONG__", "8");  // long 类型的大小为 8 字节
  define_macro("__SIZEOF_POINTER__", "8");  // 指针类型的大小为 8 字节
  define_macro("__SIZEOF_PTRDIFF_T__", "8");  // ptrdiff_t 类型的大小为 8 字节
  define_macro("__SIZEOF_SHORT__", "2");  // short 类型的大小为 2 字节
  define_macro("__SIZEOF_SIZE_T__", "8");  // size_t 类型的大小为 8 字节
  define_macro("__SIZE_TYPE__", "unsigned long");  // size_t 的类型为 unsigned long
  define_macro("__STDC_HOSTED__", "1");  // 表示程序运行在支持完整标准 C 库的环境中
  define_macro("__STDC_NO_COMPLEX__", "1");  // 禁用复数类型和操作
  define_macro("__STDC_UTF_16__", "1");  // 支持 UTF-16 编码
  define_macro("__STDC_UTF_32__", "1");  // 支持 UTF-32 编码
  define_macro("__STDC_VERSION__", "201112L");  // C 的版本号为 C11
  define_macro("__STDC__", "1");  // 表示遵循 C 标准
  define_macro("__USER_LABEL_PREFIX__", "");  // 用户定义的标签前缀为空
  define_macro("__alignof__", "_Alignof");  // _Alignof 关键字的宏定义
  define_macro("__amd64", "1");  // 表示在 amd64 平台编译
  define_macro("__amd64__", "1");  // 表示在 amd64 平台编译
  define_macro("__chibicc__", "1");  // chibicc 编译器的宏定义
  define_macro("__const__", "const");  // const 关键字的宏定义
  define_macro("__gnu_linux__", "1");  // 表示在 GNU/Linux 系统编译
  define_macro("__inline__", "inline");  // inline 关键字的宏定义
  define_macro("__linux", "1");  // 表示在 Linux 系统编译
  define_macro("__linux__", "1");  // 表示在 Linux 系统编译
  define_macro("__signed__", "signed");  // signed 关键字的宏定义
  define_macro("__typeof__", "typeof");  // typeof 关键字的宏定义
  define_macro("__unix", "1");  // 表示在 UNIX 系统编译
  define_macro("__unix__", "1");  // 表示在 UNIX 系统编译
  define_macro("__volatile__", "volatile");  // volatile 关键字的宏定义
  define_macro("__x86_64", "1");  // 表示在 x86_64 平台编译
  define_macro("__x86_64__", "1");  // 表示在 x86_64 平台编译
  define_macro("linux", "1");  // 表示在 Linux 系统编译
  define_macro("unix", "1");  // 表示在UNIX 系统编译

  add_builtin("__FILE__", file_macro);  // 添加内置宏 "__FILE__"，对应的宏函数是 file_macro
  add_builtin("__LINE__", line_macro);  // 添加内置宏 "__LINE__"，对应的宏函数是 line_macro
  add_builtin("__COUNTER__", counter_macro);  // 添加内置宏 "__COUNTER__"，对应的宏函数是 counter_macro
  add_builtin("__TIMESTAMP__", timestamp_macro);  // 添加内置宏 "__TIMESTAMP__"，对应的宏函数是 timestamp_macro
  add_builtin("__BASE_FILE__", base_file_macro);  // 添加内置宏 "__BASE_FILE__"，对应的宏函数是 base_file_macro

  time_t now = time(NULL);  // 获取当前时间
  struct tm *tm = localtime(&now);  // 将当前时间转换为本地时间表示
  define_macro("__DATE__", format_date(tm));  // 定义宏 "__DATE__"，其值为当前日期的字符串表示
  define_macro("__TIME__", format_time(tm));  // 定义宏 "__TIME__"，其值为当前时间的字符串表示
}

typedef enum {
  STR_NONE, STR_UTF8, STR_UTF16, STR_UTF32, STR_WIDE,
} StringKind;  // 定义一个枚举类型StringKind，包含五个取值：STR_NONE, STR_UTF8, STR_UTF16, STR_UTF32, STR_WIDE

static StringKind getStringKind(Token *tok) {  // 定义一个静态函数getStringKind，返回值类型为StringKind，参数为指向Token类型的指针tok
  if (!strcmp(tok->loc, "u8"))  // 如果tok指向的Token的loc属性与字符串"u8"相等，则返回STR_UTF8
    return STR_UTF8;

  switch (tok->loc[0]) {
    case '"': return STR_NONE;  // 如果tok指向的Token的loc属性的第一个字符为'"'，则返回STR_NONE
    case 'u': return STR_UTF16;  // 如果tok指向的Token的loc属性的第一个字符为'u'，则返回STR_UTF16
    case 'U': return STR_UTF32;  // 如果tok指向的Token的loc属性的第一个字符为'U'，则返回STR_UTF32
    case 'L': return STR_WIDE;  // 如果tok指向的Token的loc属性的第一个字符为'L'，则返回STR_WIDE
  }

  unreachable();  // 不可达代码，如果执行到这里说明出现了不符合预期的情况
}

// 将相邻的字符串字面量连接成一个单一的字符串字面量，符合C规范。
static void join_adjacent_string_literals(Token *tok) {  // 定义一个静态函数join_adjacent_string_literals，返回值类型为void，参数为指向Token类型的指针tok
  // 第一次遍历：如果常规字符串字面量与宽字符串字面量相邻，
  // 则在连接前将常规字符串字面量转换为宽类型。在此遍历中进行转换操作。
  for (Token *tok1 = tok; tok1->kind != TK_EOF;) {  // 定义一个指向Token类型的指针tok1，初始值为tok。当tok1指向的Token的kind属性不是TK_EOF时，继续循环
    if (tok1->kind != TK_STR || tok1->next->kind != TK_STR) {  // 如果当前Token的kind属性不是TK_STR，或者下一个Token的kind属性不是TK_STR，则继续循环
      tok1 = tok1->next;
      continue;
    }

    StringKind kind = getStringKind(tok1);      // 获取第一个字符串字面量的类型
    Type *basety = tok1->ty->base;               // 获取第一个字符串字面量的基础类型

    for (Token *t = tok1->next; t->kind == TK_STR; t = t->next) {
      StringKind k = getStringKind(t);          // 获取当前字符串字面量的类型
      if (kind == STR_NONE) {
        kind = k;
        basety = t->ty->base;                    // 如果第一个字符串字面量的类型为STR_NONE，则将当前字符串字面量的类型赋值给kind变量，并更新基础类型为当前字符串字面量的基础类型
      } else if (k != STR_NONE && kind != k) {
        error_tok(t, "unsupported non-standard concatenation of string literals");    // 如果当前字符串字面量的类型不为STR_NONE，并且kind变量的值与当前字符串字面量的类型不相等，则报错，不支持非标准的字符串字面量连接
      }
    }

    if (basety->size > 1) 
      for (Token *t = tok1; t->kind == TK_STR; t = t->next) 
        if (t->ty->base->size == 1) 
          *t = *tokenize_string_literal(t, basety);     // 如果基础类型的大小大于1，遍历第一个字符串字面量及其后续的字符串字面量，如果当前字符串字面量的基础类型的大小为1，则将其替换为经过tokenize_string_literal处理后的新字符串字面量

    while (tok1->kind == TK_STR) 
      tok1 = tok1->next;                                // 跳过已处理的字符串字面量
  }
    
  // 第二轮遍历：连接相邻的字符串字面量。
  for (Token *tok1 = tok; tok1->kind != TK_EOF;) {  // 第二轮遍历：连接相邻的字符串字面量。
    if (tok1->kind != TK_STR || tok1->next->kind != TK_STR) {  // 如果当前Token不是字符串字面量或者下一个Token不是字符串字面量，继续遍历下一个Token。
      tok1 = tok1->next;
      continue;
    }

    Token *tok2 = tok1->next;                     // 获取下一个Token
    while (tok2->kind == TK_STR)
      tok2 = tok2->next;                          // 寻找下一个非字符串Token

    int len = tok1->ty->array_len;                 // 获取第一个字符串字面量的数组长度
    for (Token *t = tok1->next; t != tok2; t = t->next)
      len = len + t->ty->array_len - 1;            // 计算连接后字符串字面量的数组长度

    char *buf = calloc(tok1->ty->base->size, len); // 为连接后的字符串字面量分配内存空间

    int i = 0;
    for (Token *t = tok1; t != tok2; t = t->next) {
      memcpy(buf + i, t->str, t->ty->size);         // 将每个字符串字面量的内容拷贝到目标缓冲区中
      i = i + t->ty->size - t->ty->base->size;      // 更新下一个字符串字面量的拷贝位置
    }

    *tok1 = *copy_token(tok1);                     // 复制第一个字符串字面量的Token
    tok1->ty = array_of(tok1->ty->base, len);      // 修改Token的类型为连接后的字符串数组类型
    tok1->str = buf;                               // 更新Token的字符串指针为连接后的字符串字面量
    tok1->next = tok2;                             // 更新Token的下一个Token为下一个非字符串Token
    tok1 = tok2;
  }
}


// 预处理器的进入点函数。
Token *preprocess(Token *tok) {
  tok = preprocess2(tok);  // 第二次预处理
  if (cond_incl)  // 如果存在未结束的条件指令
    error_tok(cond_incl->tok, "unterminated conditional directive");  // 报错：未结束的条件指令
  convert_pp_tokens(tok);  // 转换预处理标记
  join_adjacent_string_literals(tok);  // 连接相邻的字符串字面量

  for (Token *t = tok; t; t = t->next)
    t->line_no += t->line_delta;  // 更新Token的行号
  return tok;  // 返回预处理后的Token链表
}