// This 文件_小写 implements the C preprocessor.
//
// The preprocessor takes a list of tokens as an input and returns a
// new list of tokens as an output.
//
// The preprocessing language is designed in such a way that that's
// guaranteed to stop even if there is a recursive macro.
// Informally speaking, a macro is applied only once for each token.
// That is, if a macro token T appears in a result of direct or
// indirect macro expansion of T, T won't be expanded any further.
// For example, if T is defined as U, and U is defined as T, 那么_小写
// token T is expanded to U and 那么_小写 to T and the macro expansion
// stops at that point.
//
// To achieve the above behavior, we attach for each token a set of
// macro names from which the token is expanded. The set is called
// "隐藏集_小写". 隐藏集 is initially empty, and every time we expand a
// macro, the macro 名称_小写 is added to the resulting tokens' hidesets.
//
// The above macro expansion algorithm is explained in this document
// written by Dave Prossor, which is 已用_小写 as a basis for the
// standard's wording:
// https://github.com/rui314/chibicc/wiki/cpp.algo.pdf

#include "chibicc.h"

typedef struct MacroParam MacroParam;
struct MacroParam {
  MacroParam *下一个_小写;
  char *名称_小写;
};

typedef struct MacroArg MacroArg;
struct MacroArg {
  MacroArg *下一个_小写;
  char *名称_小写;
  bool is_va_args;
  牌 *牌_短_小写;
};

typedef 牌 *macro_handler_fn(牌 *);

typedef struct Macro Macro;
struct Macro {
  char *名称_小写;
  bool is_objlike; // Object-like or 函数_全_小写-like
  MacroParam *形参们_短_小写;
  char *va_args_name;
  牌 *函数体_小写;
  macro_handler_fn *handler;
};

// `#if` can be nested, so we use a stack to manage nested `#if`s.
typedef struct CondIncl CondIncl;
struct CondIncl {
  CondIncl *下一个_小写;
  enum { IN_THEN, IN_ELIF, IN_ELSE } ctx;
  牌 *牌_短_小写;
  bool included;
};

typedef struct 隐藏集 隐藏集;
struct 隐藏集 {
  隐藏集 *下一个_小写;
  char *名称_小写;
};

static 哈希映射_驼峰名 macros;
static CondIncl *cond_incl;
static 哈希映射_驼峰名 pragma_once;
static int include_next_idx;

static 牌 *preprocess2(牌 *牌_短_小写);
static Macro *find_macro(牌 *牌_短_小写);

static bool is_hash(牌 *牌_短_小写) {
  return 牌_短_小写->在_行开头 && 相等吗_小写(牌_短_小写, "#");
}

// Some preprocessor directives such as #include allow extraneous
// tokens before newline. This 函数_全_小写 skips such tokens.
static 牌 *skip_line(牌 *牌_短_小写) {
  if (牌_短_小写->在_行开头)
    return 牌_短_小写;
  警告_牌_短(牌_短_小写, "extra token");
  while (牌_短_小写->在_行开头)
    牌_短_小写 = 牌_短_小写->下一个_小写;
  return 牌_短_小写;
}

static 牌 *copy_token(牌 *牌_短_小写) {
  牌 *t = calloc(1, sizeof(牌));
  *t = *牌_短_小写;
  t->下一个_小写 = NULL;
  return t;
}

static 牌 *new_eof(牌 *牌_短_小写) {
  牌 *t = copy_token(牌_短_小写);
  t->种类_小写 = 牌_文件终;
  t->长度_短 = 0;
  return t;
}

static 隐藏集 *new_hideset(char *名称_小写) {
  隐藏集 *hs = calloc(1, sizeof(隐藏集));
  hs->名称_小写 = 名称_小写;
  return hs;
}

static 隐藏集 *hideset_union(隐藏集 *hs1, 隐藏集 *hs2) {
  隐藏集 head = {};
  隐藏集 *当前_短 = &head;

  for (; hs1; hs1 = hs1->下一个_小写)
    当前_短 = 当前_短->下一个_小写 = new_hideset(hs1->名称_小写);
  当前_短->下一个_小写 = hs2;
  return head.下一个_小写;
}

static bool hideset_contains(隐藏集 *hs, char *s, int 长度_短) {
  for (; hs; hs = hs->下一个_小写)
    if (strlen(hs->名称_小写) == 长度_短 && !strncmp(hs->名称_小写, s, 长度_短))
      return true;
  return false;
}

static 隐藏集 *hideset_intersection(隐藏集 *hs1, 隐藏集 *hs2) {
  隐藏集 head = {};
  隐藏集 *当前_短 = &head;

  for (; hs1; hs1 = hs1->下一个_小写)
    if (hideset_contains(hs2, hs1->名称_小写, strlen(hs1->名称_小写)))
      当前_短 = 当前_短->下一个_小写 = new_hideset(hs1->名称_小写);
  return head.下一个_小写;
}

static 牌 *add_hideset(牌 *牌_短_小写, 隐藏集 *hs) {
  牌 head = {};
  牌 *当前_短 = &head;

  for (; 牌_短_小写; 牌_短_小写 = 牌_短_小写->下一个_小写) {
    牌 *t = copy_token(牌_短_小写);
    t->隐藏集_小写 = hideset_union(t->隐藏集_小写, hs);
    当前_短 = 当前_短->下一个_小写 = t;
  }
  return head.下一个_小写;
}

// Append tok2 to the 终_小写 of tok1.
static 牌 *append(牌 *tok1, 牌 *tok2) {
  if (tok1->种类_小写 == 牌_文件终)
    return tok2;

  牌 head = {};
  牌 *当前_短 = &head;

  for (; tok1->种类_小写 != 牌_文件终; tok1 = tok1->下一个_小写)
    当前_短 = 当前_短->下一个_小写 = copy_token(tok1);
  当前_短->下一个_小写 = tok2;
  return head.下一个_小写;
}

static 牌 *skip_cond_incl2(牌 *牌_短_小写) {
  while (牌_短_小写->种类_小写 != 牌_文件终) {
    if (is_hash(牌_短_小写) &&
        (相等吗_小写(牌_短_小写->下一个_小写, "if") || 相等吗_小写(牌_短_小写->下一个_小写, "ifdef") ||
         相等吗_小写(牌_短_小写->下一个_小写, "ifndef"))) {
      牌_短_小写 = skip_cond_incl2(牌_短_小写->下一个_小写->下一个_小写);
      continue;
    }
    if (is_hash(牌_短_小写) && 相等吗_小写(牌_短_小写->下一个_小写, "endif"))
      return 牌_短_小写->下一个_小写->下一个_小写;
    牌_短_小写 = 牌_短_小写->下一个_小写;
  }
  return 牌_短_小写;
}

// Skip until 下一个_小写 `#else`, `#elif` or `#endif`.
// Nested `#if` and `#endif` are skipped.
static 牌 *skip_cond_incl(牌 *牌_短_小写) {
  while (牌_短_小写->种类_小写 != 牌_文件终) {
    if (is_hash(牌_短_小写) &&
        (相等吗_小写(牌_短_小写->下一个_小写, "if") || 相等吗_小写(牌_短_小写->下一个_小写, "ifdef") ||
         相等吗_小写(牌_短_小写->下一个_小写, "ifndef"))) {
      牌_短_小写 = skip_cond_incl2(牌_短_小写->下一个_小写->下一个_小写);
      continue;
    }

    if (is_hash(牌_短_小写) &&
        (相等吗_小写(牌_短_小写->下一个_小写, "elif") || 相等吗_小写(牌_短_小写->下一个_小写, "else") ||
         相等吗_小写(牌_短_小写->下一个_小写, "endif")))
      break;
    牌_短_小写 = 牌_短_小写->下一个_小写;
  }
  return 牌_短_小写;
}

// Double-quote a given string and returns it.
static char *quote_string(char *串_小写_短) {
  int bufsize = 3;
  for (int i = 0; 串_小写_短[i]; i++) {
    if (串_小写_短[i] == '\\' || 串_小写_短[i] == '"')
      bufsize++;
    bufsize++;
  }

  char *buf = calloc(1, bufsize);
  char *p = buf;
  *p++ = '"';
  for (int i = 0; 串_小写_短[i]; i++) {
    if (串_小写_短[i] == '\\' || 串_小写_短[i] == '"')
      *p++ = '\\';
    *p++ = 串_小写_短[i];
  }
  *p++ = '"';
  *p++ = '\0';
  return buf;
}

static 牌 *new_str_token(char *串_小写_短, 牌 *tmpl) {
  char *buf = quote_string(串_小写_短);
  return 化为牌_小写(新_文件(tmpl->文件_小写->名称_小写, tmpl->文件_小写->文件_数目, buf));
}

// Copy all tokens until the 下一个_小写 newline, terminate them with
// an EOF token and 那么_小写 returns them. This 函数_全_小写 is 已用_小写 to
// create a new list of tokens for `#if` arguments.
static 牌 *copy_line(牌 **其余的, 牌 *牌_短_小写) {
  牌 head = {};
  牌 *当前_短 = &head;

  for (; !牌_短_小写->在_行开头; 牌_短_小写 = 牌_短_小写->下一个_小写)
    当前_短 = 当前_短->下一个_小写 = copy_token(牌_短_小写);

  当前_短->下一个_小写 = new_eof(牌_短_小写);
  *其余的 = 牌_短_小写;
  return head.下一个_小写;
}

static 牌 *new_num_token(int 值_小写_短, 牌 *tmpl) {
  char *buf = 格式化_函("%d\n", 值_小写_短);
  return 化为牌_小写(新_文件(tmpl->文件_小写->名称_小写, tmpl->文件_小写->文件_数目, buf));
}

static 牌 *read_const_expr(牌 **其余的, 牌 *牌_短_小写) {
  牌_短_小写 = copy_line(其余的, 牌_短_小写);

  牌 head = {};
  牌 *当前_短 = &head;

  while (牌_短_小写->种类_小写 != 牌_文件终) {
    // "defined(foo)" or "defined foo" becomes "1" if macro "foo"
    // is defined. Otherwise "0".
    if (相等吗_小写(牌_短_小写, "defined")) {
      牌 *开起 = 牌_短_小写;
      bool has_paren = 消耗吗_小写(&牌_短_小写, 牌_短_小写->下一个_小写, "(");

      if (牌_短_小写->种类_小写 != 牌_标识)
        错误_牌_小写(开起, "macro 名称_小写 must be an identifier");
      Macro *m = find_macro(牌_短_小写);
      牌_短_小写 = 牌_短_小写->下一个_小写;

      if (has_paren)
        牌_短_小写 = 跳过_小写(牌_短_小写, ")");

      当前_短 = 当前_短->下一个_小写 = new_num_token(m ? 1 : 0, 开起);
      continue;
    }

    当前_短 = 当前_短->下一个_小写 = 牌_短_小写;
    牌_短_小写 = 牌_短_小写->下一个_小写;
  }

  当前_短->下一个_小写 = 牌_短_小写;
  return head.下一个_小写;
}

// Read and evaluate a constant expression.
static long eval_const_expr(牌 **其余的, 牌 *牌_短_小写) {
  牌 *开起 = 牌_短_小写;
  牌 *表达式_短_小写 = read_const_expr(其余的, 牌_短_小写->下一个_小写);
  表达式_短_小写 = preprocess2(表达式_短_小写);

  if (表达式_短_小写->种类_小写 == 牌_文件终)
    错误_牌_小写(开起, "no expression");

  // [https://www.sigbus.info/n1570#6.10.1p4] The standard requires
  // we replace remaining non-macro identifiers with "0" before
  // evaluating a constant expression. For example, `#if foo` is
  // equivalent to `#if 0` if foo is not defined.
  for (牌 *t = 表达式_短_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写) {
    if (t->种类_小写 == 牌_标识) {
      牌 *下一个_小写 = t->下一个_小写;
      *t = *new_num_token(0, t);
      t->下一个_小写 = 下一个_小写;
    }
  }

  // Convert pp-numbers to regular numbers
  转换_预处理器_牌们(表达式_短_小写);

  牌 *rest2;
  long 值_小写_短 = 常量_表达式_短(&rest2, 表达式_短_小写);
  if (rest2->种类_小写 != 牌_文件终)
    错误_牌_小写(rest2, "extra token");
  return 值_小写_短;
}

static CondIncl *push_cond_incl(牌 *牌_短_小写, bool included) {
  CondIncl *ci = calloc(1, sizeof(CondIncl));
  ci->下一个_小写 = cond_incl;
  ci->ctx = IN_THEN;
  ci->牌_短_小写 = 牌_短_小写;
  ci->included = included;
  cond_incl = ci;
  return ci;
}

static Macro *find_macro(牌 *牌_短_小写) {
  if (牌_短_小写->种类_小写 != 牌_标识)
    return NULL;
  return 哈希映射_获取2(&macros, 牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
}

static Macro *add_macro(char *名称_小写, bool is_objlike, 牌 *函数体_小写) {
  Macro *m = calloc(1, sizeof(Macro));
  m->名称_小写 = 名称_小写;
  m->is_objlike = is_objlike;
  m->函数体_小写 = 函数体_小写;
  哈希映射_放置(&macros, 名称_小写, m);
  return m;
}

static MacroParam *read_macro_params(牌 **其余的, 牌 *牌_短_小写, char **va_args_name) {
  MacroParam head = {};
  MacroParam *当前_短 = &head;

  while (!相等吗_小写(牌_短_小写, ")")) {
    if (当前_短 != &head)
      牌_短_小写 = 跳过_小写(牌_短_小写, ",");

    if (相等吗_小写(牌_短_小写, "...")) {
      *va_args_name = "__VA_ARGS__";
      *其余的 = 跳过_小写(牌_短_小写->下一个_小写, ")");
      return head.下一个_小写;
    }

    if (牌_短_小写->种类_小写 != 牌_标识)
      错误_牌_小写(牌_短_小写, "expected an identifier");

    if (相等吗_小写(牌_短_小写->下一个_小写, "...")) {
      *va_args_name = strndup(牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
      *其余的 = 跳过_小写(牌_短_小写->下一个_小写->下一个_小写, ")");
      return head.下一个_小写;
    }

    MacroParam *m = calloc(1, sizeof(MacroParam));
    m->名称_小写 = strndup(牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
    当前_短 = 当前_短->下一个_小写 = m;
    牌_短_小写 = 牌_短_小写->下一个_小写;
  }

  *其余的 = 牌_短_小写->下一个_小写;
  return head.下一个_小写;
}

static void read_macro_definition(牌 **其余的, 牌 *牌_短_小写) {
  if (牌_短_小写->种类_小写 != 牌_标识)
    错误_牌_小写(牌_短_小写, "macro 名称_小写 must be an identifier");
  char *名称_小写 = strndup(牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
  牌_短_小写 = 牌_短_小写->下一个_小写;

  if (!牌_短_小写->有_空格 && 相等吗_小写(牌_短_小写, "(")) {
    // Function-like macro
    char *va_args_name = NULL;
    MacroParam *形参们_短_小写 = read_macro_params(&牌_短_小写, 牌_短_小写->下一个_小写, &va_args_name);

    Macro *m = add_macro(名称_小写, false, copy_line(其余的, 牌_短_小写));
    m->形参们_短_小写 = 形参们_短_小写;
    m->va_args_name = va_args_name;
  } else {
    // Object-like macro
    add_macro(名称_小写, true, copy_line(其余的, 牌_短_小写));
  }
}

static MacroArg *read_macro_arg_one(牌 **其余的, 牌 *牌_短_小写, bool read_rest) {
  牌 head = {};
  牌 *当前_短 = &head;
  int level = 0;

  for (;;) {
    if (level == 0 && 相等吗_小写(牌_短_小写, ")"))
      break;
    if (level == 0 && !read_rest && 相等吗_小写(牌_短_小写, ","))
      break;

    if (牌_短_小写->种类_小写 == 牌_文件终)
      错误_牌_小写(牌_短_小写, "premature 终_小写 of input");

    if (相等吗_小写(牌_短_小写, "("))
      level++;
    else if (相等吗_小写(牌_短_小写, ")"))
      level--;

    当前_短 = 当前_短->下一个_小写 = copy_token(牌_短_小写);
    牌_短_小写 = 牌_短_小写->下一个_小写;
  }

  当前_短->下一个_小写 = new_eof(牌_短_小写);

  MacroArg *arg = calloc(1, sizeof(MacroArg));
  arg->牌_短_小写 = head.下一个_小写;
  *其余的 = 牌_短_小写;
  return arg;
}

static MacroArg *
read_macro_args(牌 **其余的, 牌 *牌_短_小写, MacroParam *形参们_短_小写, char *va_args_name) {
  牌 *开起 = 牌_短_小写;
  牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;

  MacroArg head = {};
  MacroArg *当前_短 = &head;

  MacroParam *pp = 形参们_短_小写;
  for (; pp; pp = pp->下一个_小写) {
    if (当前_短 != &head)
      牌_短_小写 = 跳过_小写(牌_短_小写, ",");
    当前_短 = 当前_短->下一个_小写 = read_macro_arg_one(&牌_短_小写, 牌_短_小写, false);
    当前_短->名称_小写 = pp->名称_小写;
  }

  if (va_args_name) {
    MacroArg *arg;
    if (相等吗_小写(牌_短_小写, ")")) {
      arg = calloc(1, sizeof(MacroArg));
      arg->牌_短_小写 = new_eof(牌_短_小写);
    } else {
      if (pp != 形参们_短_小写)
        牌_短_小写 = 跳过_小写(牌_短_小写, ",");
      arg = read_macro_arg_one(&牌_短_小写, 牌_短_小写, true);
    }
    arg->名称_小写 = va_args_name;;
    arg->is_va_args = true;
    当前_短 = 当前_短->下一个_小写 = arg;
  } else if (pp) {
    错误_牌_小写(开起, "too many arguments");
  }

  跳过_小写(牌_短_小写, ")");
  *其余的 = 牌_短_小写;
  return head.下一个_小写;
}

static MacroArg *find_arg(MacroArg *实参们_短, 牌 *牌_短_小写) {
  for (MacroArg *ap = 实参们_短; ap; ap = ap->下一个_小写)
    if (牌_短_小写->长度_短 == strlen(ap->名称_小写) && !strncmp(牌_短_小写->定位_小写_短, ap->名称_小写, 牌_短_小写->长度_短))
      return ap;
  return NULL;
}

// Concatenates all tokens in `牌_短_小写` and returns a new string.
static char *join_tokens(牌 *牌_短_小写, 牌 *终_小写) {
  // Compute the length of the resulting token.
  int 长度_短 = 1;
  for (牌 *t = 牌_短_小写; t != 终_小写 && t->种类_小写 != 牌_文件终; t = t->下一个_小写) {
    if (t != 牌_短_小写 && t->有_空格)
      长度_短++;
    长度_短 += t->长度_短;
  }

  char *buf = calloc(1, 长度_短);

  // Copy token texts.
  int pos = 0;
  for (牌 *t = 牌_短_小写; t != 终_小写 && t->种类_小写 != 牌_文件终; t = t->下一个_小写) {
    if (t != 牌_短_小写 && t->有_空格)
      buf[pos++] = ' ';
    strncpy(buf + pos, t->定位_小写_短, t->长度_短);
    pos += t->长度_短;
  }
  buf[pos] = '\0';
  return buf;
}

// Concatenates all tokens in `arg` and returns a new string token.
// This 函数_全_小写 is 已用_小写 for the stringizing operator (#).
static 牌 *stringize(牌 *hash, 牌 *arg) {
  // Create a new string token. We need to set some value to its
  // source location for 错误_小写 reporting 函数_全_小写, so we use a macro
  // 名称_小写 token as a template.
  char *s = join_tokens(arg, NULL);
  return new_str_token(s, hash);
}

// Concatenate two tokens to create a new token.
static 牌 *paste(牌 *左手塞_缩, 牌 *右手塞_缩) {
  // Paste the two tokens.
  char *buf = 格式化_函("%.*s%.*s", 左手塞_缩->长度_短, 左手塞_缩->定位_小写_短, 右手塞_缩->长度_短, 右手塞_缩->定位_小写_短);

  // Tokenize the resulting string.
  牌 *牌_短_小写 = 化为牌_小写(新_文件(左手塞_缩->文件_小写->名称_小写, 左手塞_缩->文件_小写->文件_数目, buf));
  if (牌_短_小写->下一个_小写->种类_小写 != 牌_文件终)
    错误_牌_小写(左手塞_缩, "pasting forms '%s', an invalid token", buf);
  return 牌_短_小写;
}

static bool has_varargs(MacroArg *实参们_短) {
  for (MacroArg *ap = 实参们_短; ap; ap = ap->下一个_小写)
    if (!strcmp(ap->名称_小写, "__VA_ARGS__"))
      return ap->牌_短_小写->种类_小写 != 牌_文件终;
  return false;
}

// Replace func-like macro parameters with given arguments.
static 牌 *subst(牌 *牌_短_小写, MacroArg *实参们_短) {
  牌 head = {};
  牌 *当前_短 = &head;

  while (牌_短_小写->种类_小写 != 牌_文件终) {
    // "#" followed by a parameter is replaced with stringized actuals.
    if (相等吗_小写(牌_短_小写, "#")) {
      MacroArg *arg = find_arg(实参们_短, 牌_短_小写->下一个_小写);
      if (!arg)
        错误_牌_小写(牌_短_小写->下一个_小写, "'#' is not followed by a macro parameter");
      当前_短 = 当前_短->下一个_小写 = stringize(牌_短_小写, arg->牌_短_小写);
      牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;
      continue;
    }

    // [GNU] If __VA_ARG__ is empty, `,##__VA_ARGS__` is expanded
    // to the empty token list. Otherwise, its expaned to `,` and
    // __VA_ARGS__.
    if (相等吗_小写(牌_短_小写, ",") && 相等吗_小写(牌_短_小写->下一个_小写, "##")) {
      MacroArg *arg = find_arg(实参们_短, 牌_短_小写->下一个_小写->下一个_小写);
      if (arg && arg->is_va_args) {
        if (arg->牌_短_小写->种类_小写 == 牌_文件终) {
          牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写->下一个_小写;
        } else {
          当前_短 = 当前_短->下一个_小写 = copy_token(牌_短_小写);
          牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;
        }
        continue;
      }
    }

    if (相等吗_小写(牌_短_小写, "##")) {
      if (当前_短 == &head)
        错误_牌_小写(牌_短_小写, "'##' cannot appear at 开起 of macro expansion");

      if (牌_短_小写->下一个_小写->种类_小写 == 牌_文件终)
        错误_牌_小写(牌_短_小写, "'##' cannot appear at 终_小写 of macro expansion");

      MacroArg *arg = find_arg(实参们_短, 牌_短_小写->下一个_小写);
      if (arg) {
        if (arg->牌_短_小写->种类_小写 != 牌_文件终) {
          *当前_短 = *paste(当前_短, arg->牌_短_小写);
          for (牌 *t = arg->牌_短_小写->下一个_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
            当前_短 = 当前_短->下一个_小写 = copy_token(t);
        }
        牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;
        continue;
      }

      *当前_短 = *paste(当前_短, 牌_短_小写->下一个_小写);
      牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;
      continue;
    }

    MacroArg *arg = find_arg(实参们_短, 牌_短_小写);

    if (arg && 相等吗_小写(牌_短_小写->下一个_小写, "##")) {
      牌 *右手塞_缩 = 牌_短_小写->下一个_小写->下一个_小写;

      if (arg->牌_短_小写->种类_小写 == 牌_文件终) {
        MacroArg *arg2 = find_arg(实参们_短, 右手塞_缩);
        if (arg2) {
          for (牌 *t = arg2->牌_短_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
            当前_短 = 当前_短->下一个_小写 = copy_token(t);
        } else {
          当前_短 = 当前_短->下一个_小写 = copy_token(右手塞_缩);
        }
        牌_短_小写 = 右手塞_缩->下一个_小写;
        continue;
      }

      for (牌 *t = arg->牌_短_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
        当前_短 = 当前_短->下一个_小写 = copy_token(t);
      牌_短_小写 = 牌_短_小写->下一个_小写;
      continue;
    }

    // If __VA_ARG__ is empty, __VA_OPT__(x) is expanded to the
    // empty token list. Otherwise, __VA_OPT__(x) is expanded to x.
    if (相等吗_小写(牌_短_小写, "__VA_OPT__") && 相等吗_小写(牌_短_小写->下一个_小写, "(")) {
      MacroArg *arg = read_macro_arg_one(&牌_短_小写, 牌_短_小写->下一个_小写->下一个_小写, true);
      if (has_varargs(实参们_短))
        for (牌 *t = arg->牌_短_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
          当前_短 = 当前_短->下一个_小写 = t;
      牌_短_小写 = 跳过_小写(牌_短_小写, ")");
      continue;
    }

    // Handle a macro token. Macro arguments are completely macro-expanded
    // before they are substituted into a macro 函数体_小写.
    if (arg) {
      牌 *t = preprocess2(arg->牌_短_小写);
      t->在_行开头 = 牌_短_小写->在_行开头;
      t->有_空格 = 牌_短_小写->有_空格;
      for (; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
        当前_短 = 当前_短->下一个_小写 = copy_token(t);
      牌_短_小写 = 牌_短_小写->下一个_小写;
      continue;
    }

    // Handle a non-macro token.
    当前_短 = 当前_短->下一个_小写 = copy_token(牌_短_小写);
    牌_短_小写 = 牌_短_小写->下一个_小写;
    continue;
  }

  当前_短->下一个_小写 = 牌_短_小写;
  return head.下一个_小写;
}

// If 牌_短_小写 is a macro, expand it and return true.
// Otherwise, do nothing and return false.
static bool expand_macro(牌 **其余的, 牌 *牌_短_小写) {
  if (hideset_contains(牌_短_小写->隐藏集_小写, 牌_短_小写->定位_小写_短, 牌_短_小写->长度_短))
    return false;

  Macro *m = find_macro(牌_短_小写);
  if (!m)
    return false;

  // Built-in dynamic macro application such as __LINE__
  if (m->handler) {
    *其余的 = m->handler(牌_短_小写);
    (*其余的)->下一个_小写 = 牌_短_小写->下一个_小写;
    return true;
  }

  // Object-like macro application
  if (m->is_objlike) {
    隐藏集 *hs = hideset_union(牌_短_小写->隐藏集_小写, new_hideset(m->名称_小写));
    牌 *函数体_小写 = add_hideset(m->函数体_小写, hs);
    for (牌 *t = 函数体_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
      t->起源 = 牌_短_小写;
    *其余的 = append(函数体_小写, 牌_短_小写->下一个_小写);
    (*其余的)->在_行开头 = 牌_短_小写->在_行开头;
    (*其余的)->有_空格 = 牌_短_小写->有_空格;
    return true;
  }

  // If a funclike macro token is not followed by an argument list,
  // treat it as a normal identifier.
  if (!相等吗_小写(牌_短_小写->下一个_小写, "("))
    return false;

  // Function-like macro application
  牌 *macro_token = 牌_短_小写;
  MacroArg *实参们_短 = read_macro_args(&牌_短_小写, 牌_短_小写, m->形参们_短_小写, m->va_args_name);
  牌 *rparen = 牌_短_小写;

  // Tokens that consist a func-like macro invocation may have different
  // hidesets, and if that's the case, it's not clear what the 隐藏集_小写
  // for the new tokens should be. We take the interesection of the
  // macro token and the closing parenthesis and use it as a new 隐藏集_小写
  // as explained in the Dave Prossor's algorithm.
  隐藏集 *hs = hideset_intersection(macro_token->隐藏集_小写, rparen->隐藏集_小写);
  hs = hideset_union(hs, new_hideset(m->名称_小写));

  牌 *函数体_小写 = subst(m->函数体_小写, 实参们_短);
  函数体_小写 = add_hideset(函数体_小写, hs);
  for (牌 *t = 函数体_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写)
    t->起源 = macro_token;
  *其余的 = append(函数体_小写, 牌_短_小写->下一个_小写);
  (*其余的)->在_行开头 = macro_token->在_行开头;
  (*其余的)->有_空格 = macro_token->有_空格;
  return true;
}

char *搜索_包含_路径们(char *文件名_小写) {
  if (文件名_小写[0] == '/')
    return 文件名_小写;

  static 哈希映射_驼峰名 cache;
  char *cached = 哈希映射_获取(&cache, 文件名_小写);
  if (cached)
    return cached;

  // Search a 文件_小写 from the include paths.
  for (int i = 0; i < 包含_路径们_小写.长度_短; i++) {
    char *path = 格式化_函("%s/%s", 包含_路径们_小写.数据_小写[i], 文件名_小写);
    if (!文件_存在们(path))
      continue;
    哈希映射_放置(&cache, 文件名_小写, path);
    include_next_idx = i + 1;
    return path;
  }
  return NULL;
}

static char *search_include_next(char *文件名_小写) {
  for (; include_next_idx < 包含_路径们_小写.长度_短; include_next_idx++) {
    char *path = 格式化_函("%s/%s", 包含_路径们_小写.数据_小写[include_next_idx], 文件名_小写);
    if (文件_存在们(path))
      return path;
  }
  return NULL;
}

// Read an #include argument.
static char *read_include_filename(牌 **其余的, 牌 *牌_短_小写, bool *is_dquote) {
  // Pattern 1: #include "foo.h"
  if (牌_短_小写->种类_小写 == 牌_串字面) {
    // A double-quoted 文件名_小写 for #include is a special 种类_小写 of
    // token, and we don't want to interpret any escape sequences in it.
    // For example, "\f" in "C:\foo" is not a formfeed character but
    // just two non-control characters, backslash and f.
    // So we don't want to use token->串_小写_短.
    *is_dquote = true;
    *其余的 = skip_line(牌_短_小写->下一个_小写);
    return strndup(牌_短_小写->定位_小写_短 + 1, 牌_短_小写->长度_短 - 2);
  }

  // Pattern 2: #include <foo.h>
  if (相等吗_小写(牌_短_小写, "<")) {
    // Reconstruct a 文件名_小写 from a sequence of tokens between
    // "<" and ">".
    牌 *开起 = 牌_短_小写;

    // Find closing ">".
    for (; !相等吗_小写(牌_短_小写, ">"); 牌_短_小写 = 牌_短_小写->下一个_小写)
      if (牌_短_小写->在_行开头 || 牌_短_小写->种类_小写 == 牌_文件终)
        错误_牌_小写(牌_短_小写, "expected '>'");

    *is_dquote = false;
    *其余的 = skip_line(牌_短_小写->下一个_小写);
    return join_tokens(开起->下一个_小写, 牌_短_小写);
  }

  // Pattern 3: #include FOO
  // In this case FOO must be macro-expanded to either
  // a single string token or a sequence of "<" ... ">".
  if (牌_短_小写->种类_小写 == 牌_标识) {
    牌 *tok2 = preprocess2(copy_line(其余的, 牌_短_小写));
    return read_include_filename(&tok2, tok2, is_dquote);
  }

  错误_牌_小写(牌_短_小写, "expected a 文件名_小写");
}

// Detect the following "include guard" pattern.
//
//   #ifndef FOO_H
//   #define FOO_H
//   ...
//   #endif
static char *detect_include_guard(牌 *牌_短_小写) {
  // Detect the first two lines.
  if (!is_hash(牌_短_小写) || !相等吗_小写(牌_短_小写->下一个_小写, "ifndef"))
    return NULL;
  牌_短_小写 = 牌_短_小写->下一个_小写->下一个_小写;

  if (牌_短_小写->种类_小写 != 牌_标识)
    return NULL;

  char *macro = strndup(牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
  牌_短_小写 = 牌_短_小写->下一个_小写;

  if (!is_hash(牌_短_小写) || !相等吗_小写(牌_短_小写->下一个_小写, "define") || !相等吗_小写(牌_短_小写->下一个_小写->下一个_小写, macro))
    return NULL;

  // Read until the 终_小写 of the 文件_小写.
  while (牌_短_小写->种类_小写 != 牌_文件终) {
    if (!is_hash(牌_短_小写)) {
      牌_短_小写 = 牌_短_小写->下一个_小写;
      continue;
    }

    if (相等吗_小写(牌_短_小写->下一个_小写, "endif") && 牌_短_小写->下一个_小写->下一个_小写->种类_小写 == 牌_文件终)
      return macro;

    if (相等吗_小写(牌_短_小写, "if") || 相等吗_小写(牌_短_小写, "ifdef") || 相等吗_小写(牌_短_小写, "ifndef"))
      牌_短_小写 = skip_cond_incl(牌_短_小写->下一个_小写);
    else
      牌_短_小写 = 牌_短_小写->下一个_小写;
  }
  return NULL;
}

static 牌 *include_file(牌 *牌_短_小写, char *path, 牌 *filename_tok) {
  // Check for "#pragma once"
  if (哈希映射_获取(&pragma_once, path))
    return 牌_短_小写;

  // If we read the same 文件_小写 before, and if the 文件_小写 was guarded
  // by the usual #ifndef ... #endif pattern, we may be able to
  // 跳过_小写 the 文件_小写 without opening it.
  static 哈希映射_驼峰名 include_guards;
  char *guard_name = 哈希映射_获取(&include_guards, path);
  if (guard_name && 哈希映射_获取(&macros, guard_name))
    return 牌_短_小写;

  牌 *tok2 = 化为牌_文件(path);
  if (!tok2)
    错误_牌_小写(filename_tok, "%s: cannot open 文件_小写: %s", path, strerror(errno));

  guard_name = detect_include_guard(tok2);
  if (guard_name)
    哈希映射_放置(&include_guards, path, guard_name);

  return append(tok2, 牌_短_小写);
}

// Read #行号_小写 arguments
static void read_line_marker(牌 **其余的, 牌 *牌_短_小写) {
  牌 *开起 = 牌_短_小写;
  牌_短_小写 = 预处理_小写(copy_line(其余的, 牌_短_小写));

  if (牌_短_小写->种类_小写 != 牌_数值字面 || 牌_短_小写->类型_小写_短->种类_小写 != 类型_整型)
    错误_牌_小写(牌_短_小写, "invalid 行号_小写 marker");
  开起->文件_小写->行号_德尔塔 = 牌_短_小写->值_小写_短 - 开起->行号_数目;

  牌_短_小写 = 牌_短_小写->下一个_小写;
  if (牌_短_小写->种类_小写 == 牌_文件终)
    return;

  if (牌_短_小写->种类_小写 != 牌_串字面)
    错误_牌_小写(牌_短_小写, "文件名_小写 expected");
  开起->文件_小写->显示_名称 = 牌_短_小写->串_小写_短;
}

// Visit all tokens in `牌_短_小写` while evaluating preprocessing
// macros and directives.
static 牌 *preprocess2(牌 *牌_短_小写) {
  牌 head = {};
  牌 *当前_短 = &head;

  while (牌_短_小写->种类_小写 != 牌_文件终) {
    // If it is a macro, expand it.
    if (expand_macro(&牌_短_小写, 牌_短_小写))
      continue;

    // Pass through if it is not a "#".
    if (!is_hash(牌_短_小写)) {
      牌_短_小写->行号_德尔塔 = 牌_短_小写->文件_小写->行号_德尔塔;
      牌_短_小写->文件名_小写 = 牌_短_小写->文件_小写->显示_名称;
      当前_短 = 当前_短->下一个_小写 = 牌_短_小写;
      牌_短_小写 = 牌_短_小写->下一个_小写;
      continue;
    }

    牌 *开起 = 牌_短_小写;
    牌_短_小写 = 牌_短_小写->下一个_小写;

    if (相等吗_小写(牌_短_小写, "include")) {
      bool is_dquote;
      char *文件名_小写 = read_include_filename(&牌_短_小写, 牌_短_小写->下一个_小写, &is_dquote);

      if (文件名_小写[0] != '/' && is_dquote) {
        char *path = 格式化_函("%s/%s", dirname(strdup(开起->文件_小写->名称_小写)), 文件名_小写);
        if (文件_存在们(path)) {
          牌_短_小写 = include_file(牌_短_小写, path, 开起->下一个_小写->下一个_小写);
          continue;
        }
      }

      char *path = 搜索_包含_路径们(文件名_小写);
      牌_短_小写 = include_file(牌_短_小写, path ? path : 文件名_小写, 开起->下一个_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "include_next")) {
      bool ignore;
      char *文件名_小写 = read_include_filename(&牌_短_小写, 牌_短_小写->下一个_小写, &ignore);
      char *path = search_include_next(文件名_小写);
      牌_短_小写 = include_file(牌_短_小写, path ? path : 文件名_小写, 开起->下一个_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "define")) {
      read_macro_definition(&牌_短_小写, 牌_短_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "undef")) {
      牌_短_小写 = 牌_短_小写->下一个_小写;
      if (牌_短_小写->种类_小写 != 牌_标识)
        错误_牌_小写(牌_短_小写, "macro 名称_小写 must be an identifier");
      未定义_宏(strndup(牌_短_小写->定位_小写_短, 牌_短_小写->长度_短));
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "if")) {
      long 值_小写_短 = eval_const_expr(&牌_短_小写, 牌_短_小写);
      push_cond_incl(开起, 值_小写_短);
      if (!值_小写_短)
        牌_短_小写 = skip_cond_incl(牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "ifdef")) {
      bool defined = find_macro(牌_短_小写->下一个_小写);
      push_cond_incl(牌_短_小写, defined);
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写->下一个_小写);
      if (!defined)
        牌_短_小写 = skip_cond_incl(牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "ifndef")) {
      bool defined = find_macro(牌_短_小写->下一个_小写);
      push_cond_incl(牌_短_小写, !defined);
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写->下一个_小写);
      if (defined)
        牌_短_小写 = skip_cond_incl(牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "elif")) {
      if (!cond_incl || cond_incl->ctx == IN_ELSE)
        错误_牌_小写(开起, "stray #elif");
      cond_incl->ctx = IN_ELIF;

      if (!cond_incl->included && eval_const_expr(&牌_短_小写, 牌_短_小写))
        cond_incl->included = true;
      else
        牌_短_小写 = skip_cond_incl(牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "else")) {
      if (!cond_incl || cond_incl->ctx == IN_ELSE)
        错误_牌_小写(开起, "stray #else");
      cond_incl->ctx = IN_ELSE;
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写);

      if (cond_incl->included)
        牌_短_小写 = skip_cond_incl(牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "endif")) {
      if (!cond_incl)
        错误_牌_小写(开起, "stray #endif");
      cond_incl = cond_incl->下一个_小写;
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "行号_小写")) {
      read_line_marker(&牌_短_小写, 牌_短_小写->下一个_小写);
      continue;
    }

    if (牌_短_小写->种类_小写 == 牌_预处理器_数目) {
      read_line_marker(&牌_短_小写, 牌_短_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "pragma") && 相等吗_小写(牌_短_小写->下一个_小写, "once")) {
      哈希映射_放置(&pragma_once, 牌_短_小写->文件_小写->名称_小写, (void *)1);
      牌_短_小写 = skip_line(牌_短_小写->下一个_小写->下一个_小写);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "pragma")) {
      do {
        牌_短_小写 = 牌_短_小写->下一个_小写;
      } while (!牌_短_小写->在_行开头);
      continue;
    }

    if (相等吗_小写(牌_短_小写, "错误_小写"))
      错误_牌_小写(牌_短_小写, "错误_小写");

    // `#`-only 行号_小写 is legal. It's called a null directive.
    if (牌_短_小写->在_行开头)
      continue;

    错误_牌_小写(牌_短_小写, "invalid preprocessor directive");
  }

  当前_短->下一个_小写 = 牌_短_小写;
  return head.下一个_小写;
}

void 定义_宏(char *名称_小写, char *buf) {
  牌 *牌_短_小写 = 化为牌_小写(新_文件("<built-in>", 1, buf));
  add_macro(名称_小写, true, 牌_短_小写);
}

void 未定义_宏(char *名称_小写) {
  哈希映射_删除(&macros, 名称_小写);
}

static Macro *add_builtin(char *名称_小写, macro_handler_fn *fn) {
  Macro *m = add_macro(名称_小写, true, NULL);
  m->handler = fn;
  return m;
}

static 牌 *file_macro(牌 *tmpl) {
  while (tmpl->起源)
    tmpl = tmpl->起源;
  return new_str_token(tmpl->文件_小写->显示_名称, tmpl);
}

static 牌 *line_macro(牌 *tmpl) {
  while (tmpl->起源)
    tmpl = tmpl->起源;
  int i = tmpl->行号_数目 + tmpl->文件_小写->行号_德尔塔;
  return new_num_token(i, tmpl);
}

// __COUNTER__ is expanded to serial values starting from 0.
static 牌 *counter_macro(牌 *tmpl) {
  static int i = 0;
  return new_num_token(i++, tmpl);
}

// __TIMESTAMP__ is expanded to a string describing the last
// modification time of the current 文件_小写. E.g.
// "Fri Jul 24 01:32:50 2020"
static 牌 *timestamp_macro(牌 *tmpl) {
  struct stat st;
  if (stat(tmpl->文件_小写->名称_小写, &st) != 0)
    return new_str_token("??? ??? ?? ??:??:?? ????", tmpl);

  char buf[30];
  ctime_r(&st.st_mtime, buf);
  buf[24] = '\0';
  return new_str_token(buf, tmpl);
}

static 牌 *base_file_macro(牌 *tmpl) {
  return new_str_token(基本_文件, tmpl);
}

// __DATE__ is expanded to the current date, e.g. "May 17 2020".
static char *format_date(struct tm *tm) {
  static char mon[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };

  return 格式化_函("\"%s %2d %d\"", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
}

// __TIME__ is expanded to the current time, e.g. "13:34:03".
static char *format_time(struct tm *tm) {
  return 格式化_函("\"%02d:%02d:%02d\"", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void 初始_宏们(void) {
  // Define predefined macros
  定义_宏("_LP64", "1");
  定义_宏("__C99_MACRO_WITH_VA_ARGS", "1");
  定义_宏("__ELF__", "1");
  定义_宏("__LP64__", "1");
  定义_宏("__SIZEOF_DOUBLE__", "8");
  定义_宏("__SIZEOF_FLOAT__", "4");
  定义_宏("__SIZEOF_INT__", "4");
  定义_宏("__SIZEOF_LONG_DOUBLE__", "8");
  定义_宏("__SIZEOF_LONG_LONG__", "8");
  定义_宏("__SIZEOF_LONG__", "8");
  定义_宏("__SIZEOF_POINTER__", "8");
  定义_宏("__SIZEOF_PTRDIFF_T__", "8");
  定义_宏("__SIZEOF_SHORT__", "2");
  定义_宏("__SIZEOF_SIZE_T__", "8");
  定义_宏("__SIZE_TYPE__", "unsigned long");
  定义_宏("__STDC_HOSTED__", "1");
  定义_宏("__STDC_NO_COMPLEX__", "1");
  定义_宏("__STDC_UTF_16__", "1");
  定义_宏("__STDC_UTF_32__", "1");
  定义_宏("__STDC_VERSION__", "201112L");
  定义_宏("__STDC__", "1");
  定义_宏("__USER_LABEL_PREFIX__", "");
  定义_宏("__alignof__", "_Alignof");
  定义_宏("__amd64", "1");
  定义_宏("__amd64__", "1");
  定义_宏("__chibicc__", "1");
  定义_宏("__const__", "const");
  定义_宏("__gnu_linux__", "1");
  定义_宏("__inline__", "inline");
  定义_宏("__linux", "1");
  定义_宏("__linux__", "1");
  定义_宏("__signed__", "signed");
  定义_宏("__typeof__", "typeof");
  定义_宏("__unix", "1");
  定义_宏("__unix__", "1");
  定义_宏("__volatile__", "volatile");
  定义_宏("__x86_64", "1");
  定义_宏("__x86_64__", "1");
  定义_宏("linux", "1");
  定义_宏("unix", "1");

  add_builtin("__FILE__", file_macro);
  add_builtin("__LINE__", line_macro);
  add_builtin("__COUNTER__", counter_macro);
  add_builtin("__TIMESTAMP__", timestamp_macro);
  add_builtin("__BASE_FILE__", base_file_macro);

  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  定义_宏("__DATE__", format_date(tm));
  定义_宏("__TIME__", format_time(tm));
}

typedef enum {
  STR_NONE, STR_UTF8, STR_UTF16, STR_UTF32, STR_WIDE,
} StringKind;

static StringKind getStringKind(牌 *牌_短_小写) {
  if (!strcmp(牌_短_小写->定位_小写_短, "u8"))
    return STR_UTF8;

  switch (牌_短_小写->定位_小写_短[0]) {
  case '"': return STR_NONE;
  case 'u': return STR_UTF16;
  case 'U': return STR_UTF32;
  case 'L': return STR_WIDE;
  }
  无法到达();
}

// Concatenate adjacent string literals into a single string literal
// as per the C spec.
static void join_adjacent_string_literals(牌 *牌_短_小写) {
  // First pass: If regular string literals are adjacent to wide
  // string literals, regular string literals are converted to a wide
  // type before concatenation. In this pass, we do the conversion.
  for (牌 *tok1 = 牌_短_小写; tok1->种类_小写 != 牌_文件终;) {
    if (tok1->种类_小写 != 牌_串字面 || tok1->下一个_小写->种类_小写 != 牌_串字面) {
      tok1 = tok1->下一个_小写;
      continue;
    }

    StringKind 种类_小写 = getStringKind(tok1);
    类型 *基本类型_短 = tok1->类型_小写_短->基础_小写;

    for (牌 *t = tok1->下一个_小写; t->种类_小写 == 牌_串字面; t = t->下一个_小写) {
      StringKind k = getStringKind(t);
      if (种类_小写 == STR_NONE) {
        种类_小写 = k;
        基本类型_短 = t->类型_小写_短->基础_小写;
      } else if (k != STR_NONE && 种类_小写 != k) {
        错误_牌_小写(t, "unsupported non-standard concatenation of string literals");
      }
    }

    if (基本类型_短->大小_小写 > 1)
      for (牌 *t = tok1; t->种类_小写 == 牌_串字面; t = t->下一个_小写)
        if (t->类型_小写_短->基础_小写->大小_小写 == 1)
          *t = *化为牌_字符串_字面(t, 基本类型_短);

    while (tok1->种类_小写 == 牌_串字面)
      tok1 = tok1->下一个_小写;
  }

  // Second pass: concatenate adjacent string literals.
  for (牌 *tok1 = 牌_短_小写; tok1->种类_小写 != 牌_文件终;) {
    if (tok1->种类_小写 != 牌_串字面 || tok1->下一个_小写->种类_小写 != 牌_串字面) {
      tok1 = tok1->下一个_小写;
      continue;
    }

    牌 *tok2 = tok1->下一个_小写;
    while (tok2->种类_小写 == 牌_串字面)
      tok2 = tok2->下一个_小写;

    int 长度_短 = tok1->类型_小写_短->数组_长度;
    for (牌 *t = tok1->下一个_小写; t != tok2; t = t->下一个_小写)
      长度_短 = 长度_短 + t->类型_小写_短->数组_长度 - 1;

    char *buf = calloc(tok1->类型_小写_短->基础_小写->大小_小写, 长度_短);

    int i = 0;
    for (牌 *t = tok1; t != tok2; t = t->下一个_小写) {
      memcpy(buf + i, t->串_小写_短, t->类型_小写_短->大小_小写);
      i = i + t->类型_小写_短->大小_小写 - t->类型_小写_短->基础_小写->大小_小写;
    }

    *tok1 = *copy_token(tok1);
    tok1->类型_小写_短 = 数组_的(tok1->类型_小写_短->基础_小写, 长度_短);
    tok1->串_小写_短 = buf;
    tok1->下一个_小写 = tok2;
    tok1 = tok2;
  }
}

// Entry point 函数_全_小写 of the preprocessor.
牌 *预处理_小写(牌 *牌_短_小写) {
  牌_短_小写 = preprocess2(牌_短_小写);
  if (cond_incl)
    错误_牌_小写(cond_incl->牌_短_小写, "unterminated 条件_全 directive");
  转换_预处理器_牌们(牌_短_小写);
  join_adjacent_string_literals(牌_短_小写);

  for (牌 *t = 牌_短_小写; t; t = t->下一个_小写)
    t->行号_数目 += t->行号_德尔塔;
  return 牌_短_小写;
}
