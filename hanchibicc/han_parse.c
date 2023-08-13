//han_parse.c

#include "han_chibicc.h"

// 这个文件包含一个用于C语言的递归下降解析器。
//
// 这个文件中的大多数函数都以它们应该从输入令牌列表中读取的符号命名。
// 例如，stmt() 负责从令牌列表中读取一个语句。然后，函数构造一个表示语句的AST节点。
//
// 每个函数在概念上返回两个值，一个AST节点和输入令牌的剩余部分。
// 由于C不支持多返回值，剩余的令牌通过指针参数返回给调用者。
//
// 输入令牌由链表表示。不像许多递归下降解析器，我们没有"输入令牌流"的概念。
// 大多数解析函数不改变解析器的全局状态。所以在这个解析器中，很容易预先查看任意数量的令牌。

// 声明结构体 VarScope，用于存储局部变量、全局变量、类型定义或枚举常量的作用域信息
typedef struct {
  Obj *var;         // 指向变量的指针
  Type *type_def;   // 指向类型定义的指针
  Type *enum_ty;    // 指向枚举类型的指针
  int enum_val;     // 枚举常量的值
} VarScope;        // 结构体类型名为 VarScope

// 声明结构体 Scope，表示一个块作用域
typedef struct Scope Scope;
struct Scope {
  Scope *next;       // 指向下一个作用域的指针

  // C语言有两种块作用域；一种用于变量/类型定义，另一种用于结构体/联合体/枚举标签
  HashMap vars;      // 存储变量和类型定义的哈希表
  HashMap tags;      // 存储结构体/联合体/枚举标签的哈希表
};

// 变量的属性，例如 typedef 或 extern
typedef struct {
  bool is_typedef;   // 标识变量是否是 typedef 类型
  bool is_static;    // 标识变量是否是静态变量
  bool is_extern;    // 标识变量是否是外部变量
  bool is_inline;    // 标识变量是否是内联函数
  bool is_tls;       // 标识变量是否是线程局部存储
  int align;         // 变量的对齐方式
} VarAttr;            // 结构体类型名为 VarAttr

// 这个结构体表示变量的初始化器。由于初始化器可以嵌套（例如 `int x[2][2] = {{1, 2}, {3, 4}}`），
// 所以这个结构体是一个树形数据结构。
typedef struct Initializer Initializer;
struct Initializer {
  Initializer *next;     // 指向下一个初始化器的指针
  Type *ty;              // 指向类型的指针
  Token *tok;            // 指向标记的指针
  bool is_flexible;      // 标识是否为可变长度数组的初始化器

  // 如果不是聚合类型并且有一个初始化表达式，
  // `expr` 包含初始化表达式
  Node *expr;            // 指向表达式节点的指针

  // 如果是聚合类型（例如数组或结构体）的初始化器，
  // `children` 包含其子成员的初始化器
  Initializer **children;  // 指向初始化器数组的指针

  // 联合体只能初始化一个成员。
  // `mem` 用于指明初始化的成员。
  Member *mem;           // 指向成员的指针
};

// 用于局部变量的初始化器
typedef struct InitDesg InitDesg;
struct InitDesg {
  InitDesg *next;   // 指向下一个初始化设计ator的指针
  int idx;          // 数组或结构体成员的索引
  Member *member;   // 指向结构体成员的指针
  Obj *var;         // 指向变量的指针
};

// 所有在解析过程中创建的局部变量实例都会累积到这个列表中
static Obj *locals;

// 同样地，全局变量会累积到这个列表中
static Obj *globals;

static Scope *scope = &(Scope){};

// 指向当前解析的函数对象
static Obj *current_fn;

// 当前函数中所有的 goto 语句和标签的列表
static Node *gotos;
static Node *labels;

// 当前的 "goto" 和 "continue" 跳转目标
static char *brk_label;
static char *cont_label;

// 如果正在解析 switch 语句，则指向表示 switch 的节点；否则为 NULL
static Node *current_switch;

static Obj *builtin_alloca;  // 内置函数 alloca 的对象

static bool is_typename(Token *tok);  // 判断标记是否为类型名

static Type *declspec(Token **rest, Token *tok, VarAttr *attr);  // 解析类型限定符

static Type *typename(Token **rest, Token *tok);  // 解析类型名

static Type *enum_specifier(Token **rest, Token *tok);  // 解析枚举类型说明符

static Type *typeof_specifier(Token **rest, Token *tok);  // 解析 typeof 类型说明符

static Type *type_suffix(Token **rest, Token *tok, Type *ty);  // 解析类型后缀

static Type *declarator(Token **rest, Token *tok, Type *ty);  // 解析声明符

static Node *declaration(Token **rest, Token *tok, Type *basety, VarAttr *attr);  // 解析声明语句

static void array_initializer2(Token **rest, Token *tok, Initializer *init, int i);  // 解析数组初始化器

static void struct_initializer2(Token **rest, Token *tok, Initializer *init, Member *mem);  // 解析结构体初始化器

static void initializer2(Token **rest, Token *tok, Initializer *init);  // 解析初始化器

static Initializer *initializer(Token **rest, Token *tok, Type *ty, Type **new_ty);  // 解析初始化器

static Node *lvar_initializer(Token **rest, Token *tok, Obj *var);  // 解析局部变量的初始化器

static void gvar_initializer(Token **rest, Token *tok, Obj *var);  // 解析全局变量的初始化器

static Node *compound_stmt(Token **rest, Token *tok);  // 解析复合语句

static Node *stmt(Token **rest, Token *tok);  // 解析语句

static Node *expr_stmt(Token **rest, Token *tok);  // 解析表达式语句

static Node *expr(Token **rest, Token *tok);  // 解析表达式

static int64_t eval(Node *node);  // 求解节点的值

static int64_t eval2(Node *node, char ***label);  // 求解节点的值（带标签）

static int64_t eval_rval(Node *node, char ***label);  // 求解右值节点的值（带标签）

static bool is_const_expr(Node *node);  // 判断节点是否为常量表达式

static Node *assign(Token **rest, Token *tok);  // 解析赋值表达式

static Node *logor(Token **rest, Token *tok);  // 解析逻辑或表达式

static double eval_double(Node *node);  // 求解节点的双精度浮点数值

static Node *conditional(Token **rest, Token *tok);  // 解析条件表达式

static Node *logand(Token **rest, Token *tok);  // 解析逻辑与表达式

static Node *bitor(Token **rest, Token *tok);  // 解析按位或表达式

static Node *bitxor(Token **rest, Token *tok);  // 解析按位异或表达式

static Node *bitand(Token **rest, Token *tok);  // 解析按位与表达式

static Node *equality(Token **rest, Token *tok);  // 解析相等性表达式

static Node *relational(Token **rest, Token *tok);  // 解析关系表达式

static Node *shift(Token **rest, Token *tok);  // 解析移位表达式

static Node *add(Token **rest, Token *tok);  // 解析加法表达式

static Node *new_add(Node *lhs, Node *rhs, Token *tok);  // 创建加法节点

static Node *new_sub(Node *lhs, Node *rhs, Token *tok);  // 创建减法节点

static Node *mul(Token **rest, Token *tok);  // 解析乘法表达式

static Node *cast(Token **rest, Token *tok);  // 解析类型转换表达式

static Member *get_struct_member(Type *ty, Token *tok);  // 获取结构体成员

static Type *struct_decl(Token **rest, Token *tok);  // 解析结构体声明

static Type *union_decl(Token **rest, Token *tok);  // 解析联合体声明

static Node *postfix(Token **rest, Token *tok);  // 解析后缀表达式

static Node *funcall(Token **rest, Token *tok, Node *node);  // 解析函数调用表达式

static Node *unary(Token **rest, Token *tok);  // 解析一元表达式

static Node *primary(Token **rest, Token *tok);  // 解析基本表达式

static Token *parse_typedef(Token *tok, Type *basety);  // 解析 typedef 声明

static bool is_function(Token *tok);  // 判断标记是否为函数

static Token *function(Token *tok, Type *basety, VarAttr *attr);  // 解析函数定义

static Token *global_variable(Token *tok, Type *basety, VarAttr *attr);  // 解析全局变量定义

static int align_down(int n, int align) {
  return align_to(n - align + 1, align);  // 将 n 向下对齐到 align 的倍数
}

static void enter_scope(void) {
  Scope *sc = calloc(1, sizeof(Scope));  // 分配一个新的作用域
  sc->next = scope;
  scope = sc;  // 将新的作用域设置为当前作用域
}

static void leave_scope(void) {
  scope = scope->next;  // 离开当前作用域，切换到上一个作用域
}

// 根据名称查找变量
static VarScope *find_var(Token *tok) {
  for (Scope *sc = scope; sc; sc = sc->next) {  // 遍历作用域链
    VarScope *sc2 = hashmap_get2(&sc->vars, tok->loc, tok->len);  // 在当前作用域中查找变量
    if (sc2)
      return sc2;  // 如果找到变量，则返回变量作用域
  }
  return NULL;  // 如果未找到变量，则返回空指针
}

// 根据标签名称查找类型
static Type *find_tag(Token *tok) {
  for (Scope *sc = scope; sc; sc = sc->next) {  // 遍历作用域链
    Type *ty = hashmap_get2(&sc->tags, tok->loc, tok->len);  // 在当前作用域中查找标签对应的类型
    if (ty)
      return ty;  // 如果找到类型，则返回类型
  }
  return NULL;  // 如果未找到类型，则返回空指针
}

// 创建新的语法节点
static Node *new_node(NodeKind kind, Token *tok) {
  Node *node = calloc(1, sizeof(Node));  // 分配内存空间
  node->kind = kind;  // 设置节点类型
  node->tok = tok;  // 设置节点对应的标记
  return node;  // 返回新创建的节点
}

// 创建新的二元操作符节点
static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, Token *tok) {
  Node *node = new_node(kind, tok);  // 创建新的语法节点
  node->lhs = lhs;  // 设置左操作数
  node->rhs = rhs;  // 设置右操作数
  return node;  // 返回新创建的节点
}

// 创建新的一元操作符节点
static Node *new_unary(NodeKind kind, Node *expr, Token *tok) {
  Node *node = new_node(kind, tok);  // 创建新的语法节点
  node->lhs = expr;  // 设置表达式
  return node;  // 返回新创建的节点
}

static Node *new_num(int64_t val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);  // 创建一个新的Node节点，并将节点类型设置为ND_NUM，节点的标记设置为tok
  node->val = val;  // 将节点的值设置为val
  return node;  // 返回该节点
}

static Node *new_long(int64_t val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);  // 创建一个新的Node节点，并将节点类型设置为ND_NUM，节点的标记设置为tok
  node->val = val;  // 将节点的值设置为val
  node->ty = ty_long;  // 将节点的类型设置为ty_long
  return node;  // 返回该节点
}

static Node *new_ulong(long val, Token *tok) {
  Node *node = new_node(ND_NUM, tok);  // 创建一个新的Node节点，并将节点类型设置为ND_NUM，节点的标记设置为tok
  node->val = val;  // 将节点的值设置为val
  node->ty = ty_ulong;  // 将节点的类型设置为ty_ulong
  return node;  // 返回该节点
}

static Node *new_var_node(Obj *var, Token *tok) {
  Node *node = new_node(ND_VAR, tok);  // 创建一个新的Node节点，并将节点类型设置为ND_VAR，节点的标记设置为tok
  node->var = var;  // 将节点的var成员设置为var
  return node;  // 返回该节点
}

static Node *new_vla_ptr(Obj *var, Token *tok) {
  Node *node = new_node(ND_VLA_PTR, tok);  // 创建一个新的Node节点，并将节点类型设置为ND_VLA_PTR，节点的标记设置为tok
  node->var = var;  // 将节点的var成员设置为var
  return node;  // 返回该节点
}

Node *new_cast(Node *expr, Type *ty) {
  add_type(expr);  // 将表达式的类型添加到符号表中

  Node *node = calloc(1, sizeof(Node));  // 分配一个新的Node节点，大小为Node结构的大小，并将节点的所有成员初始化为0
  node->kind = ND_CAST;  // 将节点的kind成员设置为ND_CAST，表示类型转换
  node->tok = expr->tok;  // 将节点的tok成员设置为expr的tok成员，表示取expr的标记
  node->lhs = expr;  // 将节点的lhs成员设置为expr，表示类型转换的表达式
  node->ty = copy_type(ty);  // 将节点的ty成员设置为ty的副本，表示转换的目标类型
  return node;  // 返回该节点
}

static VarScope *push_scope(char *name) {
  VarScope *sc = calloc(1, sizeof(VarScope));  // 分配一个新的VarScope结构，并将所有成员初始化为0
  hashmap_put(&scope->vars, name, sc);  // 将name作为键，将sc作为值，将键值对添加到scope->vars哈希表中
  return sc;  // 返回该VarScope结构
}

static Initializer *new_initializer(Type *ty, bool is_flexible) {
  Initializer *init = calloc(1, sizeof(Initializer));  // 创建一个 Initializer 结构体并分配内存空间
  init->ty = ty;  // 初始化结构体的 ty 成员为传入的类型

  if (ty->kind == TY_ARRAY) {
    if (is_flexible && ty->size < 0) {
      init->is_flexible = true;  // 如果是可变长数组且尺寸小于 0，设置初始化器的 is_flexible 标志为 true
      return init;  // 返回初始化器
    }

    init->children = calloc(ty->array_len, sizeof(Initializer *));  // 分配子初始化器的内存空间
    for (int i = 0; i < ty->array_len; i++)
      init->children[i] = new_initializer(ty->base, false);  // 递归创建子类型的初始化器
    return init;  // 返回初始化器
  }

  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) {
    int len = 0;  // 统计结构体成员的数量
    for (Member *mem = ty->members; mem; mem = mem->next)
      len++;

    init->children = calloc(len, sizeof(Initializer *));  // 分配子初始化器的内存空间

    for (Member *mem = ty->members; mem; mem = mem->next) {
      if (is_flexible && ty->is_flexible && !mem->next) {
        // 如果是可变长结构体且当前成员是最后一个成员
        Initializer *child = calloc(1, sizeof(Initializer));  // 创建一个子初始化器并分配内存空间
        child->ty = mem->ty;  // 初始化子初始化器的类型为当前成员的类型
        child->is_flexible = true;  // 设置子初始化器的 is_flexible 标志为 true
        init->children[mem->idx] = child;  // 将子初始化器添加到父初始化器的子节点数组中
      } else {
        // 对于非可变长结构体或非最后一个成员
        init->children[mem->idx] = new_initializer(mem->ty, false);  // 递归创建子类型的初始化器
      }
    }
    return init;  // 返回初始化器
  }

  return init;  // 返回初始化器
}

static Obj *new_var(char *name, Type *ty) {
  // 创建一个 Obj 结构体并分配内存空间
  Obj *var = calloc(1, sizeof(Obj));
  var->name = name;  // 初始化变量的名称
  var->ty = ty;  // 初始化变量的类型
  var->align = ty->align;  // 初始化变量的对齐方式
  push_scope(name)->var = var;  // 将变量添加到当前作用域的符号表中
  return var;  // 返回变量
}

static Obj *new_lvar(char *name, Type *ty) {
  // 创建局部变量
  Obj *var = new_var(name, ty);  // 调用 new_var 函数创建变量
  var->is_local = true;  // 设置变量的 is_local 标志为 true
  var->next = locals;  // 将变量添加到局部变量链表的头部
  locals = var;  // 更新局部变量链表的头指针
  return var;  // 返回变量
}

static Obj *new_gvar(char *name, Type *ty) {
  // 创建全局变量
  Obj *var = new_var(name, ty);  // 调用 new_var 函数创建变量
  var->next = globals;  // 将变量添加到全局变量链表的头部
  var->is_static = true;  // 设置变量的 is_static 标志为 true
  var->is_definition = true;  // 设置变量的 is_definition 标志为 true
  globals = var;  // 更新全局变量链表的头指针
  return var;  // 返回变量
}

static char *new_unique_name(void) {
  static int id = 0;  // 静态变量，用于生成唯一的名称
  return format(".L..%d", id++);  // 格式化生成唯一的名称并返回
}

static Obj *new_anon_gvar(Type *ty) {
  // 创建匿名的全局变量
  return new_gvar(new_unique_name(), ty);  // 调用 new_gvar 函数创建全局变量
}

static Obj *new_string_literal(char *p, Type *ty) {
  // 创建字符串字面量
  Obj *var = new_anon_gvar(ty);  // 创建匿名的全局变量
  var->init_data = p;  // 设置变量的初始化数据为字符串字面量的内容
  return var;  // 返回变量
}

static char *get_ident(Token *tok) {
  // 获取标识符字符串
  if (tok->kind != TK_IDENT)
    error_tok(tok, "expected an identifier");  // 如果当前 Token 不是标识符，则报错
  return strndup(tok->loc, tok->len);  // 返回标识符字符串的拷贝
}

static Type *find_typedef(Token *tok) {
  // 查找 typedef 定义的类型
  if (tok->kind == TK_IDENT) {
    VarScope *sc = find_var(tok);  // 在符号表中查找标识符对应的变量作用域
    if (sc)
      return sc->type_def;  // 返回变量作用域中的类型定义
  }
  return NULL;  // 如果未找到类型定义，则返回 NULL
}

static void push_tag_scope(Token *tok, Type *ty) {
  // 将标签作用域入栈
  hashmap_put2(&scope->tags, tok->loc, tok->len, ty);  // 将标签和类型添加到当前作用域的标签哈希表中
}

// declspec = ("void" | "_Bool" | "char" | "short" | "int" | "long"
//             | "typedef" | "static" | "extern" | "inline"
//             | "_Thread_local" | "__thread"
//             | "signed" | "unsigned"
//             | struct-decl | union-decl | typedef-name
//             | enum-specifier | typeof-specifier
//             | "const" | "volatile" | "auto" | "register" | "restrict"
//             | "__restrict" | "__restrict__" | "_Noreturn")+
//
// 类型名在类型说明符中的顺序并不重要。例如，`int long static` 与 `static long int` 表示相同的类型。
// 这也可以写为 `static long`，因为如果指定了 `long` 或者 `short`，可以省略 `int`。然而，`char int` 不是有效的类型说明符。
// 我们只接受有限组合的类型名。
//
// 在这个函数中，我们统计每个类型名的出现次数，并同时维护到目前为止的“当前”类型对象。
// 当遇到一个非类型名的标记时，我们返回当前的类型对象。
static Type *declspec(Token **rest, Token *tok, VarAttr *attr) {
  // 我们使用一个整数作为所有类型名的计数器。
  // 例如，位 0 和位 1 表示我们迄今为止看到关键字 "void" 的次数。
  // 借助这一点，我们可以使用下面所示的 switch 语句。

  enum {
    VOID     = 1 << 0,    // 表示出现了关键字 "void"
    BOOL     = 1 << 2,    // 表示出现了关键字 "_Bool"
    CHAR     = 1 << 4,    // 表示出现了关键字 "char"
    SHORT    = 1 << 6,    // 表示出现了关键字 "short"
    INT      = 1 << 8,    // 表示出现了关键字 "int"
    LONG     = 1 << 10,   // 表示出现了关键字 "long"
    FLOAT    = 1 << 12,   // 表示出现了关键字 "float"
    DOUBLE   = 1 << 14,   // 表示出现了关键字 "double"
    OTHER    = 1 << 16,   // 表示出现了其他类型名
    SIGNED   = 1 << 17,   // 表示出现了关键字 "signed"
    UNSIGNED = 1 << 18,   // 表示出现了关键字 "unsigned"
  };

  Type *ty = ty_int;         // 初始化类型对象指针为 int 类型
  int counter = 0;          // 初始化计数器为 0
  bool is_atomic = false;   // 初始化 is_atomic 标志为 false

  while (is_typename(tok)) {
    // 处理存储类说明符
    if (equal(tok, "typedef") || equal(tok, "static") || equal(tok, "extern") ||
        equal(tok, "inline") || equal(tok, "_Thread_local") || equal(tok, "__thread")) {
      // 如果当前的标记是 typedef、static、extern、inline、_Thread_local 或 __thread

      if (!attr)
        error_tok(tok, "storage class specifier is not allowed in this context");
      // 如果 attr 为空，则在当前上下文中不允许使用存储类说明符，报错

      if (equal(tok, "typedef"))
        attr->is_typedef = true;   // 设置 attr 的 is_typedef 标志为 true
      else if (equal(tok, "static"))
        attr->is_static = true;    // 设置 attr 的 is_static 标志为 true
      else if (equal(tok, "extern"))
        attr->is_extern = true;    // 设置 attr 的 is_extern 标志为 true
      else if (equal(tok, "inline"))
        attr->is_inline = true;    // 设置 attr 的 is_inline 标志为 true
      else
        attr->is_tls = true;       // 设置 attr 的 is_tls 标志为 true

      if (attr->is_typedef &&
          attr->is_static + attr->is_extern + attr->is_inline + attr->is_tls > 1)
        error_tok(tok, "typedef may not be used together with static,"
                  " extern, inline, __thread or _Thread_local");
      // 如果 attr 的 is_typedef 标志为 true，并且 is_static、is_extern、is_inline、is_tls 中有超过一个标志为 true，
      // 则报错，typedef 不能与 static、extern、inline、_Thread_local 同时使用

      tok = tok->next;   // 移动到下一个标记
      continue;
    }

    // 这些关键字被识别但被忽略
    if (consume(&tok, tok, "const") || consume(&tok, tok, "volatile") ||
        consume(&tok, tok, "auto") || consume(&tok, tok, "register") ||
        consume(&tok, tok, "restrict") || consume(&tok, tok, "__restrict") ||
        consume(&tok, tok, "__restrict__") || consume(&tok, tok, "_Noreturn"))
      continue;
    // 如果当前标记是 const、volatile、auto、register、restrict、__restrict、__restrict__ 或 _Noreturn，
    // 则跳过该标记，继续处理下一个标记

    if (equal(tok, "_Atomic")) {
      // 如果当前标记是 _Atomic

      tok = tok->next;   // 移动到下一个标记
      if (equal(tok , "(")) {
        // 如果下一个标记是 "("

        ty = typename(&tok, tok->next);   // 解析类型名
        tok = skip(tok, ")");             // 跳过 ")"
      }
      is_atomic = true;   // 设置 is_atomic 标志为 true
      continue;
    }

    if (equal(tok, "_Alignas")) {
      // 如果当前标记是 _Alignas

      if (!attr)
        error_tok(tok, "_Alignas is not allowed in this context");
      // 如果 attr 为空，则在当前上下文中不允许使用 _Alignas，报错

      tok = skip(tok->next, "(");   // 跳过 "("

      if (is_typename(tok))
        attr->align = typename(&tok, tok)->align;   // 解析类型名并设置对齐方式
      else
        attr->align = const_expr(&tok, tok);        // 解析常量表达式并设置对齐方式
      tok = skip(tok, ")");   // 跳过 ")"
      continue;
    }

    // 处理用户定义的类型
    Type *ty2 = find_typedef(tok);
    // 在符号表中查找是否存在与当前标记匹配的 typedef 类型

    if (equal(tok, "struct") || equal(tok, "union") || equal(tok, "enum") ||
        equal(tok, "typeof") || ty2) {
      // 如果当前标记是 struct、union、enum、typeof 或存在与当前标记匹配的 typedef 类型

      if (counter)
        break;
      // 如果 counter 不为零，则退出循环

      if (equal(tok, "struct")) {
        // 如果当前标记是 struct

        ty = struct_decl(&tok, tok->next);   // 处理结构体声明，并返回结构体类型
      } else if (equal(tok, "union")) {
        // 如果当前标记是 union

        ty = union_decl(&tok, tok->next);    // 处理联合声明，并返回联合类型
      } else if (equal(tok, "enum")) {
        // 如果当前标记是 enum

        ty = enum_specifier(&tok, tok->next);   // 处理枚举声明，并返回枚举类型
      } else if (equal(tok, "typeof")) {
        // 如果当前标记是 typeof

        ty = typeof_specifier(&tok, tok->next);   // 处理 typeof 声明，并返回 typeof 类型
      } else {
        // 否则，使用找到的 typedef 类型

        ty = ty2;
        tok = tok->next;
      }

      counter += OTHER;   // 增加 counter 的值
      continue;
    }

    // 处理内置类型。
    if (equal(tok, "void"))  // 如果标记等于"void"
      counter += VOID;  // 计数器加上VOID
    else if (equal(tok, "_Bool"))  // 否则如果标记等于"_Bool"
      counter += BOOL;  // 计数器加上BOOL
    else if (equal(tok, "char"))  // 否则如果标记等于"char"
      counter += CHAR;  // 计数器加上CHAR
    else if (equal(tok, "short"))  // 否则如果标记等于"short"
      counter += SHORT;  // 计数器加上SHORT
    else if (equal(tok, "int"))  // 否则如果标记等于"int"
      counter += INT;  // 计数器加上INT
    else if (equal(tok, "long"))  // 否则如果标记等于"long"
      counter += LONG;  // 计数器加上LONG
    else if (equal(tok, "float"))  // 否则如果标记等于"float"
      counter += FLOAT;  // 计数器加上FLOAT
    else if (equal(tok, "double"))  // 否则如果标记等于"double"
      counter += DOUBLE;  // 计数器加上DOUBLE
    else if (equal(tok, "signed"))  // 否则如果标记等于"signed"
      counter |= SIGNED;  // 计数器按位或上SIGNED
    else if (equal(tok, "unsigned"))  // 否则如果标记等于"unsigned"
      counter |= UNSIGNED;  // 计数器按位或上UNSIGNED
    else
      unreachable();  // 否则不可达

    switch (counter) {
    case VOID:  // 如果计数器等于VOID
      ty = ty_void;  // 类型为void
      break;  // 跳出switch语句

    case BOOL:  // 如果计数器等于BOOL
      ty = ty_bool;  // 类型为bool
      break;  // 跳出switch语句

    case CHAR:  // 如果计数器等于CHAR
    case SIGNED + CHAR:  // 或者计数器等于SIGNED + CHAR
      ty = ty_char;  // 类型为char
      break;  // 跳出switch语句

    case UNSIGNED + CHAR:  // 如果计数器等于UNSIGNED + CHAR
      ty = ty_uchar;  // 类型为unsigned char
      break;  // 跳出switch语句

    case SHORT:  // 如果计数器等于SHORT
    case SHORT + INT:  // 或者计数器等于SHORT + INT
    case SIGNED + SHORT:  // 或者计数器等于SIGNED + SHORT
    case SIGNED + SHORT + INT:  // 或者计数器等于SIGNED + SHORT + INT
      ty = ty_short;  // 类型为short
      break;  // 跳出switch语句

    case UNSIGNED + SHORT:  // 如果计数器等于UNSIGNED + SHORT
    case UNSIGNED + SHORT + INT:  // 或者计数器等于UNSIGNED + SHORT + INT
      ty = ty_ushort;  // 类型为unsigned short
      break;  // 跳出switch语句

    case INT:                      // 匹配 INT 情况
    case SIGNED:                   // 匹配 SIGNED 情况
    case SIGNED + INT:             // 匹配 SIGNED + INT 情况
      ty = ty_int;                 // 将 ty 赋值为 ty_int
      break;                       // 跳出 switch 语句

    case UNSIGNED:                 // 匹配 UNSIGNED 情况
    case UNSIGNED + INT:           // 匹配 UNSIGNED + INT 情况
      ty = ty_uint;                // 将 ty 赋值为 ty_uint
      break;                       // 跳出 switch 语句

    case LONG:                     // 匹配 LONG 情况
    case LONG + INT:               // 匹配 LONG + INT 情况
    case LONG + LONG:              // 匹配 LONG + LONG 情况
    case LONG + LONG + INT:        // 匹配 LONG + LONG + INT 情况
    case SIGNED + LONG:            // 匹配 SIGNED + LONG 情况
    case SIGNED + LONG + INT:      // 匹配 SIGNED + LONG + INT 情况
    case SIGNED + LONG + LONG:     // 匹配 SIGNED + LONG + LONG 情况
    case SIGNED + LONG + LONG + INT:// 匹配 SIGNED + LONG + LONG + INT 情况
      ty = ty_long;                // 将 ty 赋值为 ty_long
      break;                       // 跳出 switch 语句

    case UNSIGNED + LONG:          // 匹配 UNSIGNED + LONG 情况
    case UNSIGNED + LONG + INT:    // 匹配 UNSIGNED + LONG + INT 情况
    case UNSIGNED + LONG + LONG:   // 匹配 UNSIGNED + LONG + LONG 情况
    case UNSIGNED + LONG + LONG + INT: // 匹配 UNSIGNED + LONG + LONG + INT 情况
      ty = ty_ulong;               // 将 ty 赋值为 ty_ulong
      break;                       // 跳出 switch 语句

    // 根据传入的类型标记选择相应的类型
    case FLOAT:
      ty = ty_float; // 将类型设置为 float 类型
      break;
    case DOUBLE:
      ty = ty_double; // 将类型设置为 double 类型
      break;
    case LONG + DOUBLE:
      ty = ty_ldouble; // 将类型设置为 long double 类型
      break;
    default:
      error_tok(tok, "invalid type"); // 如果类型无效，则报错
    }

    tok = tok->next; // 将指针移动到下一个标记
  }

  if (is_atomic) {
    ty = copy_type(ty); // 如果是原子类型，则复制类型
    ty->is_atomic = true; // 设置类型的原子性标志为 true
  }

  *rest = tok; // 更新剩余标记的指针
  return ty; // 返回解析得到的类型
}


// func-params = ("void" | param ("," param)* ("," "...")?)? ")"
// param       = declspec declarator
// 函数参数解析函数：解析函数参数列表
static Type *func_params(Token **rest, Token *tok, Type *ty) {
  if (equal(tok, "void") && equal(tok->next, ")")) { // 如果参数列表为 "void)"
    *rest = tok->next->next; // 将剩余标记指针指向下一个标记的下一个位置
    return func_type(ty); // 返回函数类型
  }

  Type head = {}; // 创建一个名为 head 的空类型结构体
  Type *cur = &head; // 创建一个指针 cur，指向 head
  bool is_variadic = false; // 创建一个布尔变量 is_variadic，表示是否为可变参数函数

  while (!equal(tok, ")")) { // 当当前标记不是 ")" 时循环执行
    if (cur != &head)
      tok = skip(tok, ","); // 如果 cur 不指向 head，则跳过逗号标记

    if (equal(tok, "...")) { // 如果当前标记为 "..."
      is_variadic = true; // 将 is_variadic 设置为 true
      tok = tok->next; // 将指针移动到下一个标记
      skip(tok, ")"); // 跳过右括号标记
      break; // 跳出循环
    }

    Type *ty2 = declspec(&tok, tok, NULL); // 解析类型说明符并更新 tok 指针
    ty2 = declarator(&tok, tok, ty2); // 解析声明符并更新 tok 指针

    Token *name = ty2->name; // 获取类型结构体中的名称标记

    if (ty2->kind == TY_ARRAY) { // 如果类型为数组类型
      // 在形参上下文中，"array of T" 被转换为 "pointer to T"。例如，*argv[] 被转换为 **argv。
      ty2 = pointer_to(ty2->base); // 将数组类型转换为指向基础类型的指针类型
      ty2->name = name; // 设置类型结构体中的名称为解析得到的名称标记
    } else if (ty2->kind == TY_FUNC) { // 如果类型为函数类型
      // 同样，在形参上下文中，函数被转换为指向函数的指针。
      ty2 = pointer_to(ty2); // 将函数类型转换为指向函数的指针类型
      ty2->name = name; // 设置类型结构体中的名称为解析得到的名称标记
    }

    cur = cur->next = copy_type(ty2); // 将当前类型的下一个指针指向复制的类型，并更新 cur 指针为下一个类型
  }

  if (cur == &head)
    is_variadic = true; // 如果 cur 指向 head，表示参数列表为空，函数为可变参数函数

  ty = func_type(ty); // 将类型 ty 转换为函数类型
  ty->params = head.next; // 将函数类型的参数列表设置为 head 结构体的下一个指针
  ty->is_variadic = is_variadic; // 设置函数类型的可变参数标志
  *rest = tok->next; // 更新剩余标记的指针为当前标记的下一个位置
  return ty; // 返回解析得到的函数类型
}

// array-dimensions = ("static" | "restrict")* const-expr? "]" type-suffix
// 数组维度解析函数：解析数组的维度
static Type *array_dimensions(Token **rest, Token *tok, Type *ty) {
  while (equal(tok, "static") || equal(tok, "restrict")) // 当当前标记为 "static" 或 "restrict" 时循环执行
    tok = tok->next; // 将指针移动到下一个标记

  if (equal(tok, "]")) { // 如果当前标记为 "]"
    ty = type_suffix(rest, tok->next, ty); // 解析类型后缀并更新剩余标记的指针
    return array_of(ty, -1); // 返回大小为 -1 的数组类型
  }

  Node *expr = conditional(&tok, tok); // 解析条件表达式并更新标记指针
  tok = skip(tok, "]"); // 跳过右括号标记
  ty = type_suffix(rest, tok, ty); // 解析类型后缀并更新剩余标记的指针

  if (ty->kind == TY_VLA || !is_const_expr(expr)) // 如果类型为可变长度数组或表达式不是常量表达式
    return vla_of(ty, expr); // 返回可变长度数组类型
  return array_of(ty, eval(expr)); // 返回指定大小的数组类型
}

// 类型后缀解析函数：解析类型的后缀
// type-suffix = "(" func-params
//             | "[" array-dimensions
//             | ε
static Type *type_suffix(Token **rest, Token *tok, Type *ty) {
  if (equal(tok, "(")) // 如果当前标记为 "("
    return func_params(rest, tok->next, ty); // 解析函数参数列表并返回函数类型

  if (equal(tok, "[")) // 如果当前标记为 "["
    return array_dimensions(rest, tok->next, ty); // 解析数组维度并返回数组类型

  *rest = tok; // 更新剩余标记的指针为当前标记
  return ty; // 返回原始类型
}

// 指针解析函数：解析类型的指针修饰符
// pointers = ("*" ("const" | "volatile" | "restrict")*)*
static Type *pointers(Token **rest, Token *tok, Type *ty) {
  while (consume(&tok, tok, "*")) { // 当当前标记为 "*" 时循环执行
    ty = pointer_to(ty); // 将类型转换为指针类型
    while (equal(tok, "const") || equal(tok, "volatile") || equal(tok, "restrict") ||
           equal(tok, "__restrict") || equal(tok, "__restrict__"))  // 当当前标记为 "const"、"volatile"、"restrict"、"__restrict" 或 "__restrict__" 时循环执行
      tok = tok->next; // 将指针移动到下一个标记    
  }
  *rest = tok; // 更新剩余标记的指针为当前标记
  return ty; // 返回带有指针修饰符的类型
}

// 声明符解析函数：解析声明符
// declarator = pointers ("(" ident ")" | "(" declarator ")" | ident) type-suffix
static Type *declarator(Token **rest, Token *tok, Type *ty) {
  ty = pointers(&tok, tok, ty); // 解析指针修饰符并更新标记指针

  if (equal(tok, "(")) { // 如果当前标记为 "("
    Token *start = tok; // 记录起始标记位置
    Type dummy = {}; // 创建一个空类型结构体
    declarator(&tok, start->next, &dummy); // 递归解析声明符
    tok = skip(tok, ")"); // 跳过右括号标记
    ty = type_suffix(rest, tok, ty); // 解析类型后缀并更新剩余标记的指针
    return declarator(&tok, start->next, ty); // 递归解析声明符
  }

  Token *name = NULL; // 声明符的名称标记
  Token *name_pos = tok; // 声明符名称的位置标记

  if (tok->kind == TK_IDENT) { // 如果当前标记为标识符
    name = tok; // 记录标识符标记
    tok = tok->next; // 将指针移动到下一个标记
  }

  ty = type_suffix(rest, tok, ty); // 解析类型后缀并更新剩余标记的指针
  ty->name = name; // 设置类型结构体中的名称为解析得到的名称标记
  ty->name_pos = name_pos; // 设置类型结构体中的名称位置为解析得到的位置标记
  return ty; // 返回解析得到的类型
}

// 抽象声明符解析函数：解析抽象声明符
// abstract-declarator = pointers ("(" abstract-declarator ")")? type-suffix
static Type *abstract_declarator(Token **rest, Token *tok, Type *ty) {
  ty = pointers(&tok, tok, ty); // 解析指针修饰符并更新标记指针

  if (equal(tok, "(")) { // 如果当前标记为 "("
    Token *start = tok; // 记录起始标记位置
    Type dummy = {}; // 创建一个空类型结构体
    abstract_declarator(&tok, start->next, &dummy); // 递归解析抽象声明符
    tok = skip(tok, ")"); // 跳过右括号标记
    ty = type_suffix(rest, tok, ty); // 解析类型后缀并更新剩余标记的指针
    return abstract_declarator(&tok, start->next, ty); // 递归解析抽象声明符
  }

  return type_suffix(rest, tok, ty); // 解析类型后缀并返回结果
}

// 类型名解析函数：解析类型名
// type-name = declspec abstract-declarator
static Type *typename(Token **rest, Token *tok) {
  Type *ty = declspec(&tok, tok, NULL); // 解析声明说明符并更新标记指针
  return abstract_declarator(rest, tok, ty); // 解析抽象声明符并返回类型
}

// 判断是否为结束标记（"}" 或 "},")
static bool is_end(Token *tok) {
  return equal(tok, "}") || (equal(tok, ",") && equal(tok->next, "}"));
}

// 消费结束标记（"}" 或 "},")，更新剩余标记的指针
static bool consume_end(Token **rest, Token *tok) {
  if (equal(tok, "}")) { // 如果当前标记为 "}"
    *rest = tok->next; // 更新剩余标记的指针为下一个标记
    return true;
  }

  if (equal(tok, ",") && equal(tok->next, "}")) { // 如果当前标记为 "," 并且下一个标记为 "}"
    *rest = tok->next->next; // 更新剩余标记的指针为下两个标记之后
    return true;
  }

  return false; // 不是结束标记，返回 false
}

// 枚举说明符解析函数：解析枚举说明符
// enum-specifier = ident? "{" enum-list? "}"
//                | ident ("{" enum-list? "}")?
//
// enum-list      = ident ("=" num)? ("," ident ("=" num)?)* ","?
static Type *enum_specifier(Token **rest, Token *tok) {
  Type *ty = enum_type(); // 创建一个枚举类型

  // 读取枚举标签
  Token *tag = NULL;                           // 声明并初始化指针变量tag为NULL
  if (tok->kind == TK_IDENT) {                 // 判断tok指向的Token的种类是否为标识符
    tag = tok;                                 // 将tag指向tok
    tok = tok->next;                           // 将tok指向下一个Token
  }

  if (tag && !equal(tok, "{")) {               // 如果tag不为空且当前Token不是左花括号
    Type *ty = find_tag(tag);                   // 声明指针变量ty，并调用find_tag函数查找tag对应的类型
    if (!ty)
      error_tok(tag, "unknown enum type");     // 如果找不到对应的类型，报错：未知的枚举类型
    if (ty->kind != TY_ENUM)
      error_tok(tag, "not an enum tag");        // 如果找到的类型不是枚举类型，报错：不是枚举标签
    *rest = tok;                               // 将rest指向tok
    return ty;                                 // 返回ty
  }

  tok = skip(tok, "{");                        // 跳过左花括号，将tok指向下一个Token

  // 读取枚举列表
  int i = 0;                                   // 声明并初始化计数变量i为0
  int val = 0;                                 // 声明并初始化枚举值变量val为0
  while (!consume_end(rest, tok)) {            // 当未遇到结束标记时，循环执行以下操作
    if (i++ > 0)
      tok = skip(tok, ",");                    // 如果计数变量i大于0，跳过逗号，将tok指向下一个Token

    char *name = get_ident(tok);               // 获取标识符的名称，并将其赋值给name
    tok = tok->next;                           // 将tok指向下一个Token

    if (equal(tok, "="))
      val = const_expr(&tok, tok->next);        // 如果当前Token为等号，调用const_expr函数计算常量表达式的值，并将结果赋值给val

    VarScope *sc = push_scope(name);            // 声明指针变量sc，并调用push_scope函数将name作为变量的作用域推入作用域栈中
    sc->enum_ty = ty;                           // 将ty赋值给作用域的枚举类型
    sc->enum_val = val++;                       // 将val赋值给作用域的枚举值，并将val自增1
  }

  if (tag)
    push_tag_scope(tag, ty);                    // 如果tag不为空，调用push_tag_scope函数将tag和ty推入标签作用域栈中
  return ty;                                   // 返回ty
}

// typeof-specifier = "(" (expr | typename ")"
// 定义一个静态函数typeof_specifier，返回一个指向Type类型的指针
// 函数参数包括一个指向Token类型指针的指针rest和一个指向Token类型的指针tok
static Type *typeof_specifier(Token **rest, Token *tok) {  // typeof-specifier = "(" (expr | typename ")"
  tok = skip(tok, "(");  // 跳过'('字符

  Type *ty;
  if (is_typename(tok)) {  // 如果当前tok是一个类型名，则调用typename函数解析类型，并将返回值赋给ty
    ty = typename(&tok, tok);
  } else {
    Node *node = expr(&tok, tok);  // 否则，调用expr函数解析表达式，将结果加入符号表，并将返回的类型赋给ty
    add_type(node);
    ty = node->ty;
  }
  *rest = skip(tok, ")");  // 跳过')'字符，并将结果赋给*rest
  return ty;  // 返回ty
}

// 生成计算 VLA 大小的代码。
// Generate code for computing a VLA size.
static Node *compute_vla_size(Type *ty, Token *tok) {
  Node *node = new_node(ND_NULL_EXPR, tok);             // 创建一个表示空表达式的节点
  if (ty->base)
    node = new_binary(ND_COMMA, node, compute_vla_size(ty->base, tok), tok);    // 如果类型有基类型，则递归计算基类型的 VLA 大小，并将结果与当前节点组合

  if (ty->kind != TY_VLA)
    return node;                                        // 如果类型不是 VLA，则直接返回当前节点

  Node *base_sz;
  if (ty->base->kind == TY_VLA)
    base_sz = new_var_node(ty->base->vla_size, tok);    // 如果基类型是 VLA，则使用基类型的 VLA 大小作为基本大小
  else
    base_sz = new_num(ty->base->size, tok);              // 否则，使用基类型的大小作为基本大小

  ty->vla_size = new_lvar("", ty_ulong);                 // 创建一个表示赋值表达式的节点，将当前 VLA 的长度乘以基本大小，并将结果赋值给 VLA 的大小
  Node *expr = new_binary(ND_ASSIGN, new_var_node(ty->vla_size, tok),
                          new_binary(ND_MUL, ty->vla_len, base_sz, tok),
                          tok);
  return new_binary(ND_COMMA, node, expr, tok);          // 将赋值表达式与之前的节点组合并返回
}

static Node *new_alloca(Node *sz) {
  Node *node = new_unary(ND_FUNCALL, new_var_node(builtin_alloca, sz->tok), sz->tok);   // 创建一个表示函数调用的节点，调用名为 "alloca" 的内置函数，参数为节点 sz 所代表的变量节点
  node->func_ty = builtin_alloca->ty;    // 将节点的函数类型设置为 builtin_alloca 的类型
  node->ty = builtin_alloca->ty->return_ty;    // 将节点的类型设置为 builtin_alloca 的返回类型
  node->args = sz;    // 将节点的参数设置为 sz
  add_type(sz);    // 将 sz 的类型信息添加到符号表中
  return node;    // 返回新创建的节点
}

// declaration = declspec (declarator ("=" expr)? ("," declarator ("=" expr)?)*)? ";"
static Node *declaration(Token **rest, Token *tok, Type *basety, VarAttr *attr) {
  Node head = {};    // 创建一个空的节点作为头节点
  Node *cur = &head;    // 创建一个指针 cur，并将其指向头节点
  int i = 0;    // 初始化计数器 i 为 0

  while (!equal(tok, ";")) {    // 循环直到遇到分号结束
    if (i++ > 0)
      tok = skip(tok, ",");    // 如果 i 大于 0，跳过逗号

    Type *ty = declarator(&tok, tok, basety);    // 调用 declarator 函数解析声明符号并返回类型
    if (ty->kind == TY_VOID)
      error_tok(tok, "variable declared void");    // 如果类型为 void，报错：变量声明为 void 类型
    if (!ty->name)
      error_tok(ty->name_pos, "variable name omitted");    // 如果变量名为空，报错：变量名省略

    if (attr && attr->is_static) {
      // static local variable
      Obj *var = new_anon_gvar(ty);    // 创建一个匿名的全局变量对象
      push_scope(get_ident(ty->name))->var = var;    // 将变量对象加入作用域，并将其赋值给 var
      if (equal(tok, "="))
        gvar_initializer(&tok, tok->next, var);    // 如果遇到等号，则处理全局变量的初始化
      continue;    // 继续下一次循环
    }

    // 为计算VLA（可变长度数组）的大小生成代码。即使ty不是VLA，我们也需要这样做，
    // 因为ty可能是指向VLA的指针（例如，int (*foo)[n][m]，其中n和m是变量）。
    cur = cur->next = new_unary(ND_EXPR_STMT, compute_vla_size(ty, tok), tok);  // 将新创建的一元节点赋值给cur->next，并将其作为表达式语句添加到语法树中。compute_vla_size函数用于计算可变长度数组（VLA）的大小，即使ty不是VLA，也需要进行计算，因为ty可能是指向VLA的指针（例如，int (*foo)[n][m]，其中n和m是变量）。

    if (ty->kind == TY_VLA) {  // 如果ty的类型是VLA（可变长度数组），则执行以下代码块：
      if (equal(tok, "="))  // 如果当前的标记（tok）与"="相等，说明尝试对可变长度对象进行初始化，
        error_tok(tok, "variable-sized object may not be initialized");  // 这是不允许的，因此报错并显示错误消息。
    
      // 可变长度数组（VLA）会被转化为alloca()函数调用。
      // 例如，`int x[n+2]`会被转化为`tmp = n + 2, x = alloca(tmp)`。

      // 声明一个临时变量tmp，用于存储VLA的大小
      Obj *var = new_lvar(get_ident(ty->name), ty);  // 创建一个新的局部变量对象（Obj），使用ty->name作为标识符，并将ty作为类型信息保存在该对象中。
      Token *tok = ty->name;  // 将ty->name赋值给tok，用于后续的错误处理和语法树构建。
      Node *expr = new_binary(ND_ASSIGN, new_vla_ptr(var, tok),  // 创建一个赋值表达式节点（Node），将其左操作数设置为VLA指针节点（new_vla_ptr），
                              new_alloca(new_var_node(ty->vla_size, tok)),  // 右操作数设置为使用alloca创建的新的变量节点（new_alloca）。
                              tok);
      cur = cur->next = new_unary(ND_EXPR_STMT, expr, tok);  // 将新创建的一元节点赋值给cur->next，并将其作为表达式语句添加到语法树中。
      continue;  // 继续处理下一个迭代。根据代码上下文，这段代码可能位于循环或条件语句内部。
    }

    Obj *var = new_lvar(get_ident(ty->name), ty);  // 创建一个新的局部变量对象（Obj），使用ty->name作为标识符，并将ty作为类型信息保存在该对象中。
    if (attr && attr->align)  // 如果存在属性（attr）并且属性中包含对齐方式（align），
      var->align = attr->align;  // 则将属性中指定的对齐方式赋值给变量的对齐方式（align）。

    if (equal(tok, "=")) {  // 如果当前的标记（tok）与"="相等，说明存在变量的初始化表达式。
      Node *expr = lvar_initializer(&tok, tok->next, var);  // 调用lvar_initializer函数来解析变量的初始化表达式，并将返回的表达式节点赋值给expr。
      cur = cur->next = new_unary(ND_EXPR_STMT, expr, tok);  // 将新创建的一元节点赋值给cur->next，并将其作为表达式语句添加到语法树中。
    }

    if (var->ty->size < 0)  // 如果变量的类型大小（size）小于0，说明变量的类型是不完整的，
      error_tok(ty->name, "variable has incomplete type");  // 报错并显示错误消息。

    if (var->ty->kind == TY_VOID)  // 如果变量的类型是void，说明变量被声明为void类型，
      error_tok(ty->name, "variable declared void");  // 报错并显示错误消息。
  }

  Node *node = new_node(ND_BLOCK, tok);  // 创建一个新的块节点（Node），类型为ND_BLOCK，使用tok作为标记。
  node->body = head.next;  // 将语法树的头节点的下一个节点（head.next）赋值给块节点的body成员，构建语法树的主体部分。
  *rest = tok->next;  // 将当前标记的下一个标记（tok->next）赋值给rest指针，用于更新解析器的当前标记。
  return node;  // 返回构建完成的块节点作为函数的结果。
}

static Token *skip_excess_element(Token *tok) {
  if (equal(tok, "{")) {  // 如果当前的token是左大括号
    tok = skip_excess_element(tok->next);  // 递归调用skip_excess_element函数来处理下一个token
    return skip(tok, "}");  // 跳过右大括号并返回更新后的token
  }

  assign(&tok, tok);  // 对当前的token进行赋值操作
  return tok;  // 返回更新后的token
}

// string-initializer = string-literal
// 字符串初始化器 = 字符串字面量
static void string_initializer(Token **rest, Token *tok, Initializer *init) {
  if (init->is_flexible)
    *init = *new_initializer(array_of(init->ty->base, tok->ty->array_len), false); // 如果初始化器是可变长度的，则创建一个新的初始化器

  int len = MIN(init->ty->array_len, tok->ty->array_len); // 确定要处理的字符串长度，取较小的值

  switch (init->ty->base->size) {
  case 1: {
    char *str = tok->str; // 将token的字符串值赋给str指针
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(str[i], tok); // 将str[i]转换为数字表达式，并赋给初始化器的子表达式
    break;
  }
  case 2: {
    uint16_t *str = (uint16_t *)tok->str; // 将token的字符串值转换为uint16_t型指针
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(str[i], tok); // 将str[i]转换为数字表达式，并赋给初始化器的子表达式
    break;
  }
  case 4: {
    uint32_t *str = (uint32_t *)tok->str; // 将token的字符串值转换为uint32_t型指针
    for (int i = 0; i < len; i++)
      init->children[i]->expr = new_num(str[i], tok); // 将str[i]转换为数字表达式，并赋给初始化器的子表达式
    break;
  }
  default:
    unreachable(); // 不可达的代码，表示程序执行到了不应该到达的位置
  }

  *rest = tok->next; // 更新rest指针，指向下一个token
}

// array-designator = "[" const-expr "]"
// 数组指示符 = "[" 常量表达式 "]"
//
// C99在语言中引入了指定初始化器，允许程序员将初始化器的"光标"移动到任意元素上。
// 语法如下：
//
//   int x[10] = { 1, 2, [5]=3, 4, 5, 6, 7 };
//
// `[5]`将光标移动到第5个元素上，因此x的第5个元素被设置为3。初始化继续按顺序向前进行，
// 因此第6、7、8和9个元素分别初始化为4、5、6和7。未指定的元素（在本例中是第3和第4个元素）
// 会被初始化为零。
//
// 允许嵌套使用，因此以下初始化器是有效的：
//
//   int x[5][10] = { [5][8]=1, 2, 3 };
//
// 它将x[5][8]、x[5][9]和x[6][0]分别设置为1、2和3。
//
// 对于结构体的初始化器，可以使用`.字段名`来移动光标。例如：
//
//   struct { int a, b, c; } x = { .c=5 };
//
// 上述初始化器将x.c设置为5。
static void array_designator(Token **rest, Token *tok, Type *ty, int *begin, int *end) {
  *begin = const_expr(&tok, tok->next); // 将常量表达式的值赋给begin，并更新tok指针
  if (*begin >= ty->array_len) // 如果begin超过数组的长度，报错
    error_tok(tok, "数组指示符索引超出数组界限");

  if (equal(tok, "...")) { // 如果遇到"..."表示范围指示符
    *end = const_expr(&tok, tok->next); // 将常量表达式的值赋给end，并更新tok指针
    if (*end >= ty->array_len) // 如果end超过数组的长度，报错
      error_tok(tok, "数组指示符索引超出数组界限");
    if (*end < *begin) // 如果end小于begin，表示范围为空，报错
      error_tok(tok, "数组指示符范围 [%d, %d] 为空", *begin, *end);
  } else {
    *end = *begin; // 如果没有范围指示符，则begin和end相同
  }

  *rest = skip(tok, "]"); // 更新rest指针，指向"]"之后的下一个token
}

// struct-designator = "." ident
// 结构体指示符 = "." 标识符

static Member *struct_designator(Token **rest, Token *tok, Type *ty) {
  // 静态函数struct_designator，接受指向指针的指针rest、指向Token的指针tok、指向Type的指针ty作为参数
  Token *start = tok; // 记录起始token
  tok = skip(tok, "."); // 跳过"."符号
  if (tok->kind != TK_IDENT) // 如果下一个token不是标识符，报错：期望一个字段指示符
    error_tok(tok, "期望一个字段指示符");

  for (Member *mem = ty->members; mem; mem = mem->next) {
    // 遍历结构体的成员列表
    // 匿名结构体成员
    if (mem->ty->kind == TY_STRUCT && !mem->name) {
      // 如果成员是结构体类型并且没有名称
      if (get_struct_member(mem->ty, tok)) {
        // 调用get_struct_member函数，查找匿名结构体的成员是否匹配指示符
        *rest = start; // 更新rest指针为起始token
        return mem; // 返回匹配的成员
      }
      continue;
    }

    // 常规结构体成员
    if (mem->name->len == tok->len && !strncmp(mem->name->loc, tok->loc, tok->len)) {
      // 如果成员名称的长度与指示符的长度相等，并且名称与指示符相同
      *rest = tok->next; // 更新rest指针为下一个token
      return mem; // 返回匹配的成员
    }
  }

  error_tok(tok, "结构体没有此成员"); // 报错：结构体没有此成员
}

// designation = ("[" const-expr "]" | "." ident)* "="? initializer
static void designation(Token **rest, Token *tok, Initializer *init) {
  if (equal(tok, "[")) { // 如果当前符号是"["
    if (init->ty->kind != TY_ARRAY) // 如果初始化的类型不是数组类型
      error_tok(tok, "array index in non-array initializer"); // 报错：非数组初始化中的数组索引

    int begin, end;
    array_designator(&tok, tok, init->ty, &begin, &end); // 解析数组设计ator，并更新 tok、begin、end

    Token *tok2;
    for (int i = begin; i <= end; i++) // 遍历数组设计ator中的每个索引
      designation(&tok2, tok, init->children[i]); // 递归处理每个索引对应的子初始化器
    array_initializer2(rest, tok2, init, begin + 1); // 调用 array_initializer2 处理数组的子初始化器，传入更新后的 tok2、init、begin+1
    return;
  }

  // 检查当前的 token 是否为点号（.），并且 init 所代表的类型是结构体
  if (equal(tok, ".") && init->ty->kind == TY_STRUCT) {
    Member *mem = struct_designator(&tok, tok, init->ty);  // 获取结构体成员
    designation(&tok, tok, init->children[mem->idx]);  // 将结构体成员的值赋给对应的初始化表达式
    init->expr = NULL;  // 设置初始化表达式为空
    struct_initializer2(rest, tok, init, mem->next);  // 递归处理剩余的初始化列表
    return;
  }
  // 检查当前的 token 是否为点号（.），并且 init 所代表的类型是联合体
  if (equal(tok, ".") && init->ty->kind == TY_UNION) {
    Member *mem = struct_designator(&tok, tok, init->ty);  // 获取联合体成员
    init->mem = mem;  // 设置当前初始化节点的成员指针
    designation(rest, tok, init->children[mem->idx]);  // 将联合体成员的值赋给对应的初始化表达式
    return;
  }

  if (equal(tok, "."))  // 检查当前的 token 是否为点号（.）
    error_tok(tok, "field name not in struct or union initializer");  // 如果是，则表示字段名不在结构体或联合体初始化器中，抛出错误信息

  if (equal(tok, "="))  // 检查当前的 token 是否为等号（=）
    tok = tok->next;  // 如果是，则将下一个 token 作为新的当前 token

  initializer2(rest, tok, init);  // 调用 initializer2 函数继续处理剩余的初始化列表
}

// 如果数组具有初始化器（例如 `int x[] = {1,2,3}`），则可以省略数组的长度。
// 如果长度被省略，则计算初始化器元素的数量。
// 静态函数，用于计算数组初始化的元素数量
static int count_array_init_elements(Token *tok, Type *ty) {
  bool first = true;  // 标记是否为第一个元素
  Initializer *dummy = new_initializer(ty->base, true);  // 创建一个虚拟的初始化器节点

  int i = 0, max = 0;  // 计数器和最大值

  // 循环遍历初始化器中的元素，直到遇到结束标记
  while (!consume_end(&tok, tok)) {
    if (!first)
      tok = skip(tok, ",");  // 跳过逗号
    first = false;  // 将第一个元素标记设为 false

    if (equal(tok, "[")) {  // 如果当前的 token 是左方括号（[）
      i = const_expr(&tok, tok->next);  // 解析常量表达式作为索引
      if (equal(tok, "..."))
        i = const_expr(&tok, tok->next);  // 如果遇到省略号（...），则再次解析常量表达式
      tok = skip(tok, "]");  // 跳过右方括号（]）
      designation(&tok, tok, dummy);  // 处理可能存在的指定初始化
    } else {
      initializer2(&tok, tok, dummy);  // 处理普通的初始化表达式
    }

    i++;  // 元素计数加一
    max = MAX(max, i);  // 更新最大值
  }

  return max;  // 返回最大值作为数组初始化的元素数量
}

// 数组初始化器形式为 "{" initializer ("," initializer)* ","? "}"
static void array_initializer1(Token **rest, Token *tok, Initializer *init) {
  tok = skip(tok, "{");  // 跳过左大括号 "{"

  if (init->is_flexible) {  // 如果数组是可变长的
    // 计算数组初始化器的元素数量，并根据数量创建一个新的初始化器
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);
  }

  bool first = true;  // 标记是否为第一个初始化器

  if (init->is_flexible) {  // 如果数组是可变长的
    // 计算数组初始化器的元素数量，并根据数量创建一个新的初始化器
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);
  }

  for (int i = 0; !consume_end(rest, tok); i++) {  // 使用一个循环来遍历初始化列表，直到遇到结尾标记
    if (!first)  // 如果不是第一个初始化项，则跳过逗号
      tok = skip(tok, ",");
    first = false;  // 将first标记设为false，表示已经过了第一个初始化项

    if (equal(tok, "[")) {  // 如果当前标记是左方括号，表示遇到了数组指示符
      int begin, end;  // 定义begin和end，用于保存数组指示符的范围
      array_designator(&tok, tok, init->ty, &begin, &end);  // 调用array_designator函数处理数组指示符，并更新tok、begin和end的值

      Token *tok2;  // 定义指针tok2，用于保存循环中的标记
      for (int j = begin; j <= end; j++)  // 使用循环处理数组指示符范围内的每一个元素
        designation(&tok2, tok, init->children[j]);  // 调用designation函数处理每个元素，并更新tok2的值
      tok = tok2;  // 更新tok的值为循环处理后的tok2
      i = end;  // 将i设为数组指示符的结束索引，以便跳过已处理的元素
      continue;  // 跳过当前循环，进入下一次循环
    }

    if (i < init->ty->array_len)
      initializer2(&tok, tok, init->children[i]);  // 如果当前索引小于初始化项的数组长度，则调用initializer2函数处理当前索引位置的初始化项，并更新tok的值
    else
      tok = skip_excess_element(tok);  // 否则，说明已经超过了初始化项的数组长度，需要跳过多余的元素
  }
}// 循环结束，初始化列表处理完毕

// array-initializer2 = initializer ("," initializer)*
// 处理数组初始化的辅助函数，处理数组初始化中的每个元素
static void array_initializer2(Token **rest, Token *tok, Initializer *init, int i) {  // 处理数组初始化的辅助函数，处理数组初始化中的每个元素
  if (init->is_flexible) {  // 如果数组是可变长度的，则需要确定数组的长度
    int len = count_array_init_elements(tok, init->ty);
    *init = *new_initializer(array_of(init->ty->base, len), false);  // 根据初始化列表中的元素个数创建一个新的初始化器，更新初始化器的类型和长度信息
  }

  for (; i < init->ty->array_len && !is_end(tok); i++) {  // 循环处理数组初始化列表中的每个元素，直到达到数组的长度或遇到结尾标记
    Token *start = tok;  // 记录当前标记的起始位置
    if (i > 0)
      tok = skip(tok, ",");  // 如果不是第一个元素，则跳过逗号

    if (equal(tok, "[") || equal(tok, ".")) {  // 如果当前标记是左方括号或点号，表示遇到了数组指示符或结构体成员指示符
      *rest = start;  // 更新rest指向当前标记的起始位置
      return;  // 结束函数的执行
    }

    initializer2(&tok, tok, init->children[i]);  // 调用initializer2函数处理当前元素的初始化
  }
  *rest = tok;  // 更新rest指向处理完最后一个元素后的标记位置
}

// struct-initializer1 = "{" initializer ("," initializer)* ","? "}"
// 处理结构体初始化的辅助函数，处理结构体初始化中的每个成员
static void struct_initializer1(Token **rest, Token *tok, Initializer *init) {
  tok = skip(tok, "{");  // 跳过左大括号，开始处理结构体初始化

  Member *mem = init->ty->members;  // 获取结构体的成员列表
  bool first = true;  // 标记是否是第一个成员

  while (!consume_end(rest, tok)) {
    // 循环处理结构体初始化列表中的每个成员，直到遇到结尾标记
    if (!first)
      tok = skip(tok, ",");  // 如果不是第一个成员，则跳过逗号
    first = false;

    if (equal(tok, ".")) {
      // 如果当前标记是点号，表示遇到了结构体成员指示符
      mem = struct_designator(&tok, tok, init->ty);  // 调用struct_designator函数处理结构体成员指示符，并更新mem的值
      designation(&tok, tok, init->children[mem->idx]);  // 调用designation函数处理指定成员的初始化，并更新tok的值
      mem = mem->next;  // 更新mem为下一个成员
      continue;  // 跳过当前循环，进入下一次循环
    }

    if (mem) {
      initializer2(&tok, tok, init->children[mem->idx]);  // 调用initializer2函数处理当前成员的初始化
      mem = mem->next;  // 更新mem为下一个成员
    } else {
      tok = skip_excess_element(tok);  // 否则，说明已经超过了结构体成员的个数，需要跳过多余的元素
    }
  }
}

// struct-initializer2 = initializer ("," initializer)*
// 处理结构体初始化的辅助函数，处理结构体初始化中的每个成员
static void struct_initializer2(Token **rest, Token *tok, Initializer *init, Member *mem) {
  bool first = true;  // 标记是否是第一个成员

  for (; mem && !is_end(tok); mem = mem->next) {
    // 循环处理结构体初始化列表中的每个成员，直到成员为空或遇到结尾标记
    Token *start = tok;  // 记录当前标记的起始位置

    if (!first)
      tok = skip(tok, ",");  // 如果不是第一个成员，则跳过逗号
    first = false;

    if (equal(tok, "[") || equal(tok, ".")) {
      // 如果当前标记是左方括号或点号，表示遇到了数组指示符或结构体成员指示符
      *rest = start;  // 更新rest指向当前标记的起始位置
      return;  // 结束函数的执行
    }

    initializer2(&tok, tok, init->children[mem->idx]);  // 调用initializer2函数处理当前成员的初始化
  }
  *rest = tok;  // 更新rest指向处理完最后一个成员后的标记位置
}

static void union_initializer(Token **rest, Token *tok, Initializer *init) {
  // Unlike structs, union initializers take only one initializer,
  // and that initializes the first union member by default.
  // You can initialize other member using a designated initializer.
  // 处理联合体初始化的函数，与结构体初始化不同，联合体初始化只接受一个初始化器，
  // 默认情况下，该初始化器初始化第一个联合体成员。
  // 你可以使用指定的初始化器来初始化其他成员。
  
  if (equal(tok, "{") && equal(tok->next, ".")) {
    // 如果当前标记是左大括号，并且下一个标记是点号，表示遇到了联合体成员指示符
    Member *mem = struct_designator(&tok, tok->next, init->ty);  // 调用struct_designator函数处理结构体成员指示符，并更新mem的值
    init->mem = mem;  // 更新初始化器的成员指示符为mem
    designation(&tok, tok, init->children[mem->idx]);  // 调用designation函数处理指定成员的初始化，并更新tok的值
    *rest = skip(tok, "}");  // 更新rest指向右大括号后的标记位置
    return;  // 结束函数的执行
  }

  init->mem = init->ty->members;  // 更新初始化器的成员指示符为联合体的第一个成员

  if (equal(tok, "{")) {
    // 如果当前标记是左大括号，表示遇到了联合体初始化列表
    initializer2(&tok, tok->next, init->children[0]);  // 调用initializer2函数处理第一个成员的初始化
    consume(&tok, tok, ",");  // 跳过逗号
    *rest = skip(tok, "}");  // 更新rest指向右大括号后的标记位置
  } else {
    initializer2(rest, tok, init->children[0]);  // 调用initializer2函数处理第一个成员的初始化
  }
}

// initializer = string-initializer | array-initializer
//             | struct-initializer | union-initializer
//             | assign
// 处理初始化器的辅助函数，根据初始化器的类型调用对应的处理函数
static void initializer2(Token **rest, Token *tok, Initializer *init) {
  if (init->ty->kind == TY_ARRAY && tok->kind == TK_STR) {
    // 如果初始化器的类型是数组，并且当前标记是字符串字面量
    string_initializer(rest, tok, init);  // 调用string_initializer函数处理字符串初始化器
    return;  // 结束函数的执行
  }

  if (init->ty->kind == TY_ARRAY) {
    // 如果初始化器的类型是数组
    if (equal(tok, "{"))
      array_initializer1(rest, tok, init);  // 调用array_initializer1函数处理数组初始化器
    else
      array_initializer2(rest, tok, init, 0);  // 调用array_initializer2函数处理数组初始化器
    return;  // 结束函数的执行
  }

  if (init->ty->kind == TY_STRUCT) {
    // 如果初始化器的类型是结构体
    if (equal(tok, "{")) {
      struct_initializer1(rest, tok, init);  // 调用struct_initializer1函数处理结构体初始化器
      return;  // 结束函数的执行
    }
    
    // 一个结构体可以用另一个结构体进行初始化。例如，`struct T x = y;`，其中y是类型为`struct T`的变量。
    // 首先处理这种情况。
    Node *expr = assign(rest, tok);  // 使用assign函数对rest和tok进行赋值操作，返回一个Node指针，并将其赋给expr变量。
    add_type(expr);  // 将expr的类型信息添加到类型系统中。
    if (expr->ty->kind == TY_STRUCT) {  // 如果expr的类型是结构体类型。
      init->expr = expr;  // 将expr赋值给init的expr成员变量。
      return;  // 返回当前函数。
    }

    // 对于其他情况，调用struct_initializer2函数进行处理。
    struct_initializer2(rest, tok, init, init->ty->members);  // 调用struct_initializer2函数，传递rest、tok、init以及init->ty->members作为参数。
    return;  // 返回当前函数。

    if (init->ty->kind == TY_UNION) {  // 如果init的类型是union类型。
      union_initializer(rest, tok, init);  // 调用union_initializer函数，传递rest、tok和init作为参数。
      return;  // 返回当前函数。
    }

    if (equal(tok, "{")) {
    // 如果当前的词法单元等于 "{"
    // 表示可能是一个标量变量的初始化器，被大括号括起来，例如 `int x = {3};`。
    // 处理这种情况。
    initializer2(&tok, tok->next, init); // 调用initializer2函数，传入参数tok、tok->next和init
    *rest = skip(tok, "}"); // 跳过 "}"，将结果赋值给rest指针指向的变量
    return;
  }

  init->expr = assign(rest, tok); // 初始化器的表达式部分为assign函数的返回值，传入参数rest和tok
}

static Type *copy_struct_type(Type *ty) {
  ty = copy_type(ty);  // 复制类型ty的副本

  Member head = {};  // 创建一个头节点
  Member *cur = &head;  // 创建一个指向头节点的指针cur
  for (Member *mem = ty->members; mem; mem = mem->next) {
    Member *m = calloc(1, sizeof(Member));  // 分配一个Member结构的内存空间，并将地址赋给指针m
    *m = *mem;  // 将mem指向的Member结构的值复制到m指向的内存空间
    cur = cur->next = m;  // 将m连接到cur后面，更新cur指针
  }

  ty->members = head.next;  // 将头节点的下一个节点赋值给ty的成员变量members
  return ty;  // 返回复制后的类型ty
}

static Initializer *initializer(Token **rest, Token *tok, Type *ty, Type **new_ty) {
  Initializer *init = new_initializer(ty, true);  // 创建一个包含类型ty的初始化器init
  initializer2(rest, tok, init);  // 调用initializer2函数，解析初始化表达式rest，并将结果存储在init中

  if ((ty->kind == TY_STRUCT || ty->kind == TY_UNION) && ty->is_flexible) {
    ty = copy_struct_type(ty);  // 复制结构类型ty的副本，并将副本赋值给ty

    Member *mem = ty->members;  // 创建一个指向结构类型ty的成员的指针mem，指向第一个成员
    while (mem->next)
      mem = mem->next;  // 遍历成员列表，找到最后一个成员
    mem->ty = init->children[mem->idx]->ty;  // 将初始化器init中对应成员的类型赋值给最后一个成员的类型
    ty->size += mem->ty->size;  // 更新结构类型ty的大小，加上最后一个成员的大小

    *new_ty = ty;  // 更新new_ty指向的类型为复制后的结构类型ty
    return init;  // 返回初始化器init
  }

  *new_ty = init->ty;  // 更新new_ty指向的类型为初始化器init的类型
  return init;  // 返回初始化器init
}

static Node *init_desg_expr(InitDesg *desg, Token *tok) {
  if (desg->var)  // 如果存在变量
    return new_var_node(desg->var, tok);  // 创建一个表示变量的节点并返回

  if (desg->member) {  // 如果存在成员
    Node *node = new_unary(ND_MEMBER, init_desg_expr(desg->next, tok), tok);  // 创建一个表示成员访问的节点
    node->member = desg->member;  // 设置节点的成员属性为desg的成员
    return node;  // 返回节点
  }

  Node *lhs = init_desg_expr(desg->next, tok);  // 递归调用init_desg_expr函数获取左操作数
  Node *rhs = new_num(desg->idx, tok);  // 创建一个表示索引值的节点作为右操作数
  return new_unary(ND_DEREF, new_add(lhs, rhs, tok), tok);  // 创建一个表示解引用的节点，并以左操作数和右操作数为参数创建一个加法节点，并返回解引用节点
}

static Node *create_lvar_init(Initializer *init, Type *ty, InitDesg *desg, Token *tok) {
  if (ty->kind == TY_ARRAY) {  // 如果类型是数组
    Node *node = new_node(ND_NULL_EXPR, tok);  // 创建一个空表达式节点作为初始节点
    for (int i = 0; i < ty->array_len; i++) {
      InitDesg desg2 = {desg, i};  // 创建一个包含索引值的InitDesg结构体desg2
      Node *rhs = create_lvar_init(init->children[i], ty->base, &desg2, tok);  // 递归调用create_lvar_init函数创建右操作数节点rhs
      node = new_binary(ND_COMMA, node, rhs, tok);  // 创建一个逗号表达式节点，连接初始节点和右操作数节点
    }
    return node;  // 返回初始节点
  }

  if (ty->kind == TY_STRUCT && !init->expr) {  // 如果类型是结构体且没有初始化表达式
    Node *node = new_node(ND_NULL_EXPR, tok);  // 创建一个空表达式节点作为初始节点

    for (Member *mem = ty->members; mem; mem = mem->next) {
      InitDesg desg2 = {desg, 0, mem};  // 创建一个包含成员信息的InitDesg结构体desg2
      Node *rhs = create_lvar_init(init->children[mem->idx], mem->ty, &desg2, tok);  // 递归调用create_lvar_init函数创建右操作数节点rhs
      node = new_binary(ND_COMMA, node, rhs, tok);  // 创建一个逗号表达式节点，连接初始节点和右操作数节点
    }
    return node;  // 返回初始节点
  }

  if (ty->kind == TY_UNION) {  // 如果类型是联合体
    Member *mem = init->mem ? init->mem : ty->members;  // 如果存在初始化的成员，使用该成员；否则使用联合体的第一个成员
    InitDesg desg2 = {desg, 0, mem};  // 创建一个包含成员信息的InitDesg结构体desg2
    return create_lvar_init(init->children[mem->idx], mem->ty, &desg2, tok);  // 递归调用create_lvar_init函数创建右操作数节点，并返回该节点
  }

  if (!init->expr)  // 如果不存在初始化表达式
    return new_node(ND_NULL_EXPR, tok);  // 创建一个空表达式节点作为初始节点

  Node *lhs = init_desg_expr(desg, tok);  // 调用init_desg_expr函数创建左操作数节点lhs
  return new_binary(ND_ASSIGN, lhs, init->expr, tok);  // 创建一个赋值表达式节点，并返回该节点
}

// 使用初始化器的变量定义是变量定义后跟赋值的简写形式。
// 此函数为初始化器生成赋值表达式。例如，`int x[2][2] = {{6, 7}, {8, 9}}` 被转换为以下表达式：
//
//   x[0][0] = 6;
//   x[0][1] = 7;
//   x[1][0] = 8;
//   x[1][1] = 9;
// 对局部变量进行初始化的函数。
// 使用初始化器初始化变量。例如，int x = 10; 将转换为以下表达式：
//
//   memset(&x, 0, sizeof(x));
//   x = 10;
static Node *lvar_initializer(Token **rest, Token *tok, Obj *var) {
  Initializer *init = initializer(rest, tok, var->ty, &var->ty);  // 解析初始化器并返回对应的Initializer结构体
  InitDesg desg = {NULL, 0, NULL, var};  // 创建一个InitDesg结构体，表示变量的初始化设计

  // 如果给出了部分初始化器列表，标准要求未指定的元素被设置为0。
  // 在这里，我们简单地在用用户提供的值初始化之前，将变量的整个内存区域进行零初始化。
  Node *lhs = new_node(ND_MEMZERO, tok);  // 创建一个ND_MEMZERO类型的节点，表示将变量内存区域清零
  lhs->var = var;  // 设置节点的var属性为变量var

  Node *rhs = create_lvar_init(init, var->ty, &desg, tok);  // 调用create_lvar_init函数生成变量初始化的赋值表达式节点
  return new_binary(ND_COMMA, lhs, rhs, tok);  // 创建一个逗号表达式节点，连接清零节点和赋值表达式节点，并返回该节点
}

// 从缓冲区中读取数据并返回一个无符号64位整数。
// 根据指定的大小(sz)，从缓冲区(buf)中读取数据，并将其转换为无符号64位整数返回。
static uint64_t read_buf(char *buf, int sz) {
  if (sz == 1)
    return *buf;  // 如果大小为1字节，则返回缓冲区的值
  if (sz == 2)
    return *(uint16_t *)buf;  // 如果大小为2字节，则将缓冲区转换为uint16_t类型并返回
  if (sz == 4)
    return *(uint32_t *)buf;  // 如果大小为4字节，则将缓冲区转换为uint32_t类型并返回
  if (sz == 8)
    return *(uint64_t *)buf;  // 如果大小为8字节，则将缓冲区转换为uint64_t类型并返回
  unreachable();  // 不可达代码，用于在编译器发现未覆盖的情况下产生错误
}

// 向缓冲区中写入数据。
// 根据指定的大小(sz)，将值(val)写入缓冲区(buf)中。
static void write_buf(char *buf, uint64_t val, int sz) {
  if (sz == 1)
    *buf = val;  // 如果大小为1字节，则将值直接写入缓冲区
  else if (sz == 2)
    *(uint16_t *)buf = val;  // 如果大小为2字节，则将值转换为uint16_t类型后写入缓冲区
  else if (sz == 4)
    *(uint32_t *)buf = val;  // 如果大小为4字节，则将值转换为uint32_t类型后写入缓冲区
  else if (sz == 8)
    *(uint64_t *)buf = val;  // 如果大小为8字节，则将值转换为uint64_t类型后写入缓冲区
  else
    unreachable();  // 不可达代码，用于在编译器发现未覆盖的情况下产生错误
}

// 将全局变量的数据写入缓冲区，并生成重定位信息。
// 根据初始值(init)、类型(ty)和偏移量(offset)，将全局变量的数据写入缓冲区(buf)。
// 函数返回最新的重定位信息指针。
static Relocation *
write_gvar_data(Relocation *cur, Initializer *init, Type *ty, char *buf, int offset) {
  if (ty->kind == TY_ARRAY) {  // 如果类型是数组
    int sz = ty->base->size;  // 获取基础类型的大小
    for (int i = 0; i < ty->array_len; i++) {
      cur = write_gvar_data(cur, init->children[i], ty->base, buf, offset + sz * i);  // 递归调用write_gvar_data处理数组元素
    }
    return cur;
  }

  if (ty->kind == TY_STRUCT) {  // 如果类型是结构体
    for (Member *mem = ty->members; mem; mem = mem->next) {
      if (mem->is_bitfield) {  // 如果成员是位域
        Node *expr = init->children[mem->idx]->expr;  // 获取位域的初始表达式
        if (!expr)
          break;

        char *loc = buf + offset + mem->offset;  // 计算位域在缓冲区中的地址
        uint64_t oldval = read_buf(loc, mem->ty->size);  // 从缓冲区中读取位域的旧值
        uint64_t newval = eval(expr);  // 计算位域的新值
        uint64_t mask = (1L << mem->bit_width) - 1;  // 计算掩码
        uint64_t combined = oldval | ((newval & mask) << mem->bit_offset);  // 将旧值和新值合并后写入缓冲区
        write_buf(loc, combined, mem->ty->size);
      } else {
        cur = write_gvar_data(cur, init->children[mem->idx], mem->ty, buf,
                              offset + mem->offset);  // 递归调用write_gvar_data处理非位域成员
      }
    }
    return cur;
  }

  if (ty->kind == TY_UNION) {  // 如果类型是联合体
    if (!init->mem)  // 如果初始值的成员为空
      return cur;
    return write_gvar_data(cur, init->children[init->mem->idx], init->mem->ty, buf, offset);  // 递归调用write_gvar_data处理联合体成员
  }

  if (!init->expr)  // 如果初始值的表达式为空
    return cur;

  if (ty->kind == TY_FLOAT) {  // 如果类型是float
    *(float *)(buf + offset) = eval_double(init->expr);  // 将表达式的值转换为float类型后写入缓冲区
    return cur;
  }

  if (ty->kind == TY_DOUBLE) {  // 如果类型是double
    *(double *)(buf + offset) = eval_double(init->expr);  // 将表达式的值转换为double类型后写入缓冲区
    return cur;
  }

  char **label = NULL;  // 创建指向指针的指针label，并初始化为NULL
  uint64_t val = eval2(init->expr, &label);  // 评估初始表达式的值，并将标签存储在label指针中

  if (!label) {  // 如果没有标签
    write_buf(buf + offset, val, ty->size);  // 将值写入缓冲区的指定偏移量处
    return cur;  // 返回当前重定位信息指针
  }

  Relocation *rel = calloc(1, sizeof(Relocation));  // 分配一个重定位信息结构的内存
  rel->offset = offset;  // 设置重定位信息的偏移量
  rel->label = label;  // 设置重定位信息的标签
  rel->addend = val;  // 设置重定位信息的修正值
  cur->next = rel;  // 将当前重定位信息的下一个指针指向新创建的重定位信息
  return cur->next;  // 返回新创建的重定位信息指针
}

// 全局变量的初始化器在编译时进行求值，并嵌入到.data节中。
// 该函数将Initializer对象序列化为一个扁平的字节数组。如果初始化器列表包含非常量表达式，则会产生编译错误。
// 静态函数：gvar_initializer
// 全局变量的初始化器处理函数。接收一个指向Token指针的指针rest、一个Token指针tok和一个Obj指针var作为参数。
static void gvar_initializer(Token **rest, Token *tok, Obj *var) {
  Initializer *init = initializer(rest, tok, var->ty, &var->ty);  // 调用initializer函数处理初始化器表达式，获取初始化器对象

  Relocation head = {};  // 创建一个重定位信息结构，并初始化为零
  char *buf = calloc(1, var->ty->size);  // 分配一个大小为变量类型大小的内存空间，用于存储初始化数据
  write_gvar_data(&head, init, var->ty, buf, 0);  // 将初始化数据写入缓冲区，并生成重定位信息
  var->init_data = buf;  // 将缓冲区的地址存储到变量的init_data字段中
  var->rel = head.next;  // 将重定位信息链表的第一个节点存储到变量的rel字段中
}

// 如果给定的标记表示一个类型，则返回true。
// 函数：is_typename
// 判断给定的标记是否表示类型。
static bool is_typename(Token *tok) {
  static HashMap map;  // 静态哈希映射，用于存储关键字和类型的映射关系

  if (map.capacity == 0) {  // 如果哈希映射为空
    static char *kw[] = {
      "void", "_Bool", "char", "short", "int", "long", "struct", "union",
      "typedef", "enum", "static", "extern", "_Alignas", "signed", "unsigned",
      "const", "volatile", "auto", "register", "restrict", "__restrict",
      "__restrict__", "_Noreturn", "float", "double", "typeof", "inline",
      "_Thread_local", "__thread", "_Atomic",
    };

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
      hashmap_put(&map, kw[i], (void *)1);  // 将关键字和值1放入哈希映射中，表示关键字是类型
  }

  return hashmap_get2(&map, tok->loc, tok->len) || find_typedef(tok);  // 判断给定的标记是否在哈希映射中，或者是否是typedef定义的类型
}

// asm-stmt 表示 "asm" ("volatile" | "inline")* "(" string-literal ")"，
// 即以关键字 "asm" 开头，可选地紧跟零个或多个 "volatile" 或 "inline" 关键字，然后跟着一个左括号和一个字符串字面量，最后以右括号结束。
// 函数：asm_stmt
// 解析asm语句节点。形式为 "asm" ("volatile" | "inline")* "(" string-literal ")"。
static Node *asm_stmt(Token **rest, Token *tok) {
  Node *node = new_node(ND_ASM, tok);  // 创建一个表示asm语句的节点
  tok = tok->next;

  while (equal(tok, "volatile") || equal(tok, "inline"))
    tok = tok->next;

  tok = skip(tok, "(");  // 跳过左括号
  if (tok->kind != TK_STR || tok->ty->base->kind != TY_CHAR)
    error_tok(tok, "expected string literal");  // 如果下一个标记不是字符串字面量或者类型不是字符型，则报错
  node->asm_str = tok->str;  // 将字符串字面量赋值给asm语句节点的asm_str字段
  *rest = skip(tok->next, ")");  // 跳过右括号，并更新rest指针
  return node;  // 返回asm语句节点
}

// 语句：stmt
// 语法规则：
// stmt = "return" expr? ";"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "switch" "(" expr ")" stmt
//      | "case" const-expr ("..." const-expr)? ":" stmt
//      | "default" ":" stmt
//      | "for" "(" expr-stmt expr? ";" expr? ")" stmt
//      | "while" "(" expr ")" stmt
//      | "do" stmt "while" "(" expr ")" ";"
//      | "asm" asm-stmt
//      | "goto" (ident | "*" expr) ";"
//      | "break" ";"
//      | "continue" ";"
//      | ident ":" stmt
//      | "{" compound-stmt
//      | expr-stmt
// 函数：stmt
// 解析语句节点。
static Node *stmt(Token **rest, Token *tok) {
  if (equal(tok, "return")) {  // 如果当前标记是 "return"
    Node *node = new_node(ND_RETURN, tok);  // 创建一个表示返回语句的节点
    if (consume(rest, tok->next, ";"))  // 如果下一个标记是分号，则表示无返回值，直接返回节点
      return node;

    Node *exp = expr(&tok, tok->next);  // 解析表达式并更新tok指针
    *rest = skip(tok, ";");  // 跳过分号，并更新rest指针

    add_type(exp);  // 添加表达式的类型信息
    Type *ty = current_fn->ty->return_ty;  // 获取当前函数的返回类型
    if (ty->kind != TY_STRUCT && ty->kind != TY_UNION)  // 如果返回类型不是结构体或联合体
      exp = new_cast(exp, current_fn->ty->return_ty);  // 进行类型转换

    node->lhs = exp;  // 将表达式作为返回语句的左子节点
    return node;  // 返回返回语句节点
  }

  if (equal(tok, "if")) {  // 如果当前标记是 "if"
    Node *node = new_node(ND_IF, tok);  // 创建一个表示 if 语句的节点
    tok = skip(tok->next, "(");  // 跳过左括号
    node->cond = expr(&tok, tok);  // 解析条件表达式并更新tok指针
    tok = skip(tok, ")");  // 跳过右括号
    node->then = stmt(&tok, tok);  // 解析 if 语句的主体部分，并更新tok指针
    if (equal(tok, "else"))  // 如果下一个标记是 "else"
      node->els = stmt(&tok, tok->next);  // 解析 else 语句的部分，并更新tok指针
    *rest = tok;  // 更新rest指针
    return node;  // 返回 if 语句节点
  }

  if (equal(tok, "switch")) {  // 如果当前标记是 "switch"
    Node *node = new_node(ND_SWITCH, tok);  // 创建一个表示 switch 语句的节点
    tok = skip(tok->next, "(");  // 跳过左括号
    node->cond = expr(&tok, tok);  // 解析条件表达式并更新tok指针
    tok = skip(tok, ")");  // 跳过右括号

    Node *sw = current_switch;  // 保存当前的 switch 语句节点
    current_switch = node;  // 将当前 switch 语句节点更新为新创建的节点

    char *brk = brk_label;  // 保存当前的 break 标签
    brk_label = node->brk_label = new_unique_name();  // 为当前 switch 语句节点生成一个唯一的 break 标签

    node->then = stmt(rest, tok);  // 解析 switch 语句的主体部分，并更新rest和tok指针

    current_switch = sw;  // 恢复原来的 switch 语句节点
    brk_label = brk;  // 恢复原来的 break 标签
    return node;  // 返回 switch 语句节点
  }

  if (equal(tok, "case")) {  // 如果当前标记是 "case"
    if (!current_switch)  // 如果当前没有处于 switch 语句中
      error_tok(tok, "stray case");  // 报错，case 关键字位置错误

    Node *node = new_node(ND_CASE, tok);  // 创建一个表示 case 语句的节点
    int begin = const_expr(&tok, tok->next);  // 解析常量表达式并更新tok指针
    int end;

    if (equal(tok, "...")) {  // 如果下一个标记是 "..."
      // [GNU] Case 范围，例如 "case 1 ... 5:"
      end = const_expr(&tok, tok->next);  // 解析常量表达式并更新tok指针
      if (end < begin)  // 如果范围结束值小于开始值
        error_tok(tok, "empty case range specified");  // 报错，空的 case 范围
    } else {
      end = begin;  // 如果没有范围符号，则开始值和结束值相同
    }

    tok = skip(tok, ":");  // 跳过冒号
    node->label = new_unique_name();  // 为 case 语句生成一个唯一的标签
    node->lhs = stmt(rest, tok);  // 解析 case 语句的主体部分，并更新rest和tok指针
    node->begin = begin;  // 记录 case 范围的开始值
    node->end = end;  // 记录 case 范围的结束值
    node->case_next = current_switch->case_next;  // 将当前 case 语句节点添加到 switch 语句节点的 case_next 链表中
    current_switch->case_next = node;  // 更新 switch 语句节点的 case_next 指针为当前 case 语句节点
    return node;  // 返回 case 语句节点
  }

  if (equal(tok, "default")) {  // 如果当前标记是 "default"
    if (!current_switch)  // 如果当前没有处于 switch 语句中
      error_tok(tok, "stray default");  // 报错，default 关键字位置错误

    Node *node = new_node(ND_CASE, tok);  // 创建一个表示 default 语句的节点
    tok = skip(tok->next, ":");  // 跳过冒号
    node->label = new_unique_name();  // 为 default 语句生成一个唯一的标签
    node->lhs = stmt(rest, tok);  // 解析 default 语句的主体部分，并更新rest和tok指针
    current_switch->default_case = node;  // 将当前 default 语句节点设置为 switch 语句节点的 default_case
    return node;  // 返回 default 语句节点
  }

  if (equal(tok, "for")) {  // 如果当前标记是 "for"
    Node *node = new_node(ND_FOR, tok);  // 创建一个表示 for 循环的节点
    tok = skip(tok->next, "(");  // 跳过左括号

    enter_scope();  // 进入新的作用域

    char *brk = brk_label;  // 保存当前的 break 标签
    char *cont = cont_label;  // 保存当前的 continue 标签
    brk_label = node->brk_label = new_unique_name();  // 为当前 for 循环节点生成一个唯一的 break 标签
    cont_label = node->cont_label = new_unique_name();  // 为当前 for 循环节点生成一个唯一的 continue 标签

    if (is_typename(tok)) {  // 如果当前标记是类型名
      Type *basety = declspec(&tok, tok, NULL);  // 解析类型声明并更新tok指针
      node->init = declaration(&tok, tok, basety, NULL);  // 解析变量声明并更新tok指针
    } else {
      node->init = expr_stmt(&tok, tok);  // 解析表达式语句并更新tok指针
    }

    if (!equal(tok, ";"))  // 如果当前标记不是分号
      node->cond = expr(&tok, tok);  // 解析条件表达式并更新tok指针
    tok = skip(tok, ";");  // 跳过分号

    if (!equal(tok, ")"))  // 如果当前标记不是右括号
      node->inc = expr(&tok, tok);  // 解析递增表达式并更新tok指针
    tok = skip(tok, ")");  // 跳过右括号

    node->then = stmt(rest, tok);  // 解析 for 循环的主体部分，并更新rest和tok指针

    leave_scope();  // 离开作用域
    brk_label = brk;  // 恢复原来的 break 标签
    cont_label = cont;  // 恢复原来的 continue 标签
    return node;  // 返回 for 循环节点
  }

  if (equal(tok, "while")) {  // 如果当前标记是 "while"
    Node *node = new_node(ND_FOR, tok);  // 创建一个表示 while 循环的节点
    tok = skip(tok->next, "(");  // 跳过左括号
    node->cond = expr(&tok, tok);  // 解析循环条件表达式并更新tok指针
    tok = skip(tok, ")");  // 跳过右括号

    char *brk = brk_label;  // 保存当前的 break 标签
    char *cont = cont_label;  // 保存当前的 continue 标签
    brk_label = node->brk_label = new_unique_name();  // 为当前 while 循环节点生成一个唯一的 break 标签
    cont_label = node->cont_label = new_unique_name();  // 为当前 while 循环节点生成一个唯一的 continue 标签

    node->then = stmt(rest, tok);  // 解析 while 循环的主体部分，并更新rest和tok指针

    brk_label = brk;  // 恢复原来的 break 标签
    cont_label = cont;  // 恢复原来的 continue 标签
    return node;  // 返回 while 循环节点
  }

  if (equal(tok, "do")) {  // 如果当前标记是 "do"
    Node *node = new_node(ND_DO, tok);  // 创建一个表示 do-while 循环的节点

    char *brk = brk_label;  // 保存当前的 break 标签
    char *cont = cont_label;  // 保存当前的 continue 标签
    brk_label = node->brk_label = new_unique_name();  // 为当前 do-while 循环节点生成一个唯一的 break 标签
    cont_label = node->cont_label = new_unique_name();  // 为当前 do-while 循环节点生成一个唯一的 continue 标签

    node->then = stmt(&tok, tok->next);  // 解析 do-while 循环的主体部分，并更新tok指针

    brk_label = brk;  // 恢复原来的 break 标签
    cont_label = cont;  // 恢复原来的 continue 标签

    tok = skip(tok, "while");  // 跳过关键字 "while"
    tok = skip(tok, "(");  // 跳过左括号
    node->cond = expr(&tok, tok);  // 解析循环条件表达式并更新tok指针
    tok = skip(tok, ")");  // 跳过右括号
    *rest = skip(tok, ";");  // 更新rest指针，跳过分号
    return node;  // 返回 do-while 循环节点
  }

  if (equal(tok, "asm"))
    return asm_stmt(rest, tok);  // 如果当前标记是 "asm"，则解析内联汇编语句并返回结果

  if (equal(tok, "goto")) {  // 如果当前标记是 "goto"
    if (equal(tok->next, "*")) {  // 如果下一个标记是 "*"
      // [GNU] `goto *ptr` 跳转到 `ptr` 指定的地址。
      Node *node = new_node(ND_GOTO_EXPR, tok);  // 创建一个表示带有表达式的跳转的节点
      node->lhs = expr(&tok, tok->next->next);  // 解析表达式并更新tok指针
      *rest = skip(tok, ";");  // 更新rest指针，跳过分号
      return node;  // 返回带有表达式的跳转节点
    }

    Node *node = new_node(ND_GOTO, tok);  // 创建一个表示跳转的节点
    node->label = get_ident(tok->next);  // 获取标签标识符
    node->goto_next = gotos;  // 将节点添加到跳转链表中
    gotos = node;  // 更新跳转链表的头部
    *rest = skip(tok->next->next, ";");  // 更新rest指针，跳过分号
    return node;  // 返回跳转节点
  }

  if (equal(tok, "break")) {  // 如果当前标记是 "break"
    if (!brk_label)  // 如果没有可跳转的 break 标签
      error_tok(tok, "stray break");  // 报错：孤立的 break 语句
    Node *node = new_node(ND_GOTO, tok);  // 创建一个表示跳转的节点
    node->unique_label = brk_label;  // 设置跳转目标为当前的 break 标签
    *rest = skip(tok->next, ";");  // 更新rest指针，跳过分号
    return node;  // 返回跳转节点
  }

  if (equal(tok, "continue")) {  // 如果当前标记是 "continue"
    if (!cont_label)  // 如果没有可跳转的 continue 标签
      error_tok(tok, "stray continue");  // 报错：孤立的 continue 语句
    Node *node = new_node(ND_GOTO, tok);  // 创建一个表示跳转的节点
    node->unique_label = cont_label;  // 设置跳转目标为当前的 continue 标签
    *rest = skip(tok->next, ";");  // 更新rest指针，跳过分号
    return node;  // 返回跳转节点
  }

  if (tok->kind == TK_IDENT && equal(tok->next, ":")) {  // 如果当前标记是标识符并且下一个标记是冒号
    Node *node = new_node(ND_LABEL, tok);  // 创建一个表示标签的节点
    node->label = strndup(tok->loc, tok->len);  // 复制标签的字符串内容
    node->unique_label = new_unique_name();  // 为标签生成一个唯一的名称
    node->lhs = stmt(rest, tok->next->next);  // 解析标签后面的语句，并更新rest和tok指针
    node->goto_next = labels;  // 将节点添加到标签链表中
    labels = node;  // 更新标签链表的头部
    return node;  // 返回标签节点
  }

  if (equal(tok, "{"))  // 如果当前标记是左花括号
    return compound_stmt(rest, tok->next);  // 解析复合语句，并返回结果

  return expr_stmt(rest, tok);  // 解析表达式语句，并返回结果
}

// compound-stmt = (typedef | declaration | stmt)* "}"
// 复合语句 = (typedef | 声明 | 语句)* "}"
static Node *compound_stmt(Token **rest, Token *tok) {
  Node *node = new_node(ND_BLOCK, tok);  // 创建一个表示代码块的节点
  Node head = {};  // 创建一个空的节点作为头部
  Node *cur = &head;  // 当前节点指针指向头部节点

  enter_scope();  // 进入新的作用域

  while (!equal(tok, "}")) {  // 当当前标记不是右花括号时循环执行
    if (is_typename(tok) && !equal(tok->next, ":")) {  // 如果当前标记是类型名且下一个标记不是冒号
      VarAttr attr = {};  // 创建一个变量属性结构体
      Type *basety = declspec(&tok, tok, &attr);  // 解析类型说明符，并更新tok指针和变量属性

      if (attr.is_typedef) {  // 如果是typedef声明
        tok = parse_typedef(tok, basety);  // 解析typedef声明，并更新tok指针
        continue;  // 继续循环
      }

      if (is_function(tok)) {  // 如果是函数声明
        tok = function(tok, basety, &attr);  // 解析函数声明，并更新tok指针
        continue;  // 继续循环
      }

      if (attr.is_extern) {  // 如果是extern声明
        tok = global_variable(tok, basety, &attr);  // 解析全局变量声明，并更新tok指针
        continue;  // 继续循环
      }

      cur = cur->next = declaration(&tok, tok, basety, &attr);  // 解析普通变量声明，并更新tok指针
    } else {
      cur = cur->next = stmt(&tok, tok);  // 解析语句，并更新tok指针
    }
    add_type(cur);  // 将当前节点的类型信息添加到类型列表中
  }

  leave_scope();  // 离开当前作用域

  node->body = head.next;  // 将代码块的语句链表指向头部节点的下一个节点，即为代码块的第一个语句节点
  *rest = tok->next;  // 更新rest指针，跳过右花括号
  return node;  // 返回代码块节点
}

// expr-stmt = expr? ";"
// 表达式语句 = 表达式? ";"
static Node *expr_stmt(Token **rest, Token *tok) {
  if (equal(tok, ";")) {  // 如果当前标记是分号，表示空语句
    *rest = tok->next;  // 更新rest指针，跳过分号
    return new_node(ND_BLOCK, tok);  // 创建一个表示空语句的节点
  }

  Node *node = new_node(ND_EXPR_STMT, tok);  // 创建一个表示表达式语句的节点
  node->lhs = expr(&tok, tok);  // 解析表达式，并更新tok指针
  *rest = skip(tok, ";");  // 更新rest指针，跳过分号
  return node;  // 返回表达式语句节点
}

// expr = assign ("," expr)?
// 表达式 = 赋值表达式 ("," 表达式)?
static Node *expr(Token **rest, Token *tok) {
  Node *node = assign(&tok, tok);  // 解析赋值表达式，并更新tok指针

  if (equal(tok, ","))  // 如果当前标记是逗号
    return new_binary(ND_COMMA, node, expr(rest, tok->next), tok);  // 创建一个逗号表达式节点，并递归解析下一个表达式

  *rest = tok;  // 更新rest指针
  return node;  // 返回表达式节点
}

static int64_t eval(Node *node) {
  return eval2(node, NULL);  // 调用eval2函数计算表达式的值
}

// 对给定的节点进行常量表达式求值。

// 常量表达式可以是一个数字或者是 ptr+n 的形式，其中 ptr 是一个指向全局变量的指针，n是一个正/负数。
// 后一种形式只能作为全局变量的初始化表达式被接受。
static int64_t eval2(Node *node, char ***label) {
  add_type(node);  // 将节点的类型信息添加到类型列表中

  if (is_flonum(node->ty))  // 如果节点的类型是浮点数类型
    return eval_double(node);  // 调用eval_double函数计算浮点数表达式的值

  switch (node->kind) {
  case ND_ADD:  // 加法运算
    return eval2(node->lhs, label) + eval(node->rhs);  // 递归求得左操作数的值并与右操作数的值相加
  case ND_SUB:  // 减法运算
    return eval2(node->lhs, label) - eval(node->rhs);  // 递归求得左操作数的值并减去右操作数的值
  case ND_MUL:  // 乘法运算
    return eval(node->lhs) * eval(node->rhs);  // 求得左操作数的值乘以右操作数的值
  case ND_DIV:  // 除法运算
    if (node->ty->is_unsigned)  // 如果节点的类型是无符号类型
      return (uint64_t)eval(node->lhs) / eval(node->rhs);  // 将左操作数的值转换为无符号数进行除法运算
    return eval(node->lhs) / eval(node->rhs);  // 求得左操作数的值除以右操作数的值
  case ND_NEG:  // 取负运算
    return -eval(node->lhs);  // 求得操作数的相反数
  case ND_MOD:  // 取模运算
    if (node->ty->is_unsigned)  // 如果节点的类型是无符号类型
      return (uint64_t)eval(node->lhs) % eval(node->rhs);  // 将左操作数的值转换为无符号数进行取模运算
    return eval(node->lhs) % eval(node->rhs);  // 求得左操作数的值取模右操作数的值
  case ND_BITAND:  // 按位与运算
    return eval(node->lhs) & eval(node->rhs);  // 求得左操作数的值与右操作数的值的按位与结果
  case ND_BITOR:  // 按位或运算
    return eval(node->lhs) | eval(node->rhs);  // 求得左操作数的值与右操作数的值的按位或结果
  case ND_BITXOR:  // 按位异或运算
    return eval(node->lhs) ^ eval(node->rhs);  // 求得左操作数的值与右操作数的值的按位异或结果
  case ND_SHL:  // 左移运算
    return eval(node->lhs) << eval(node->rhs);  // 求得左操作数的值左移右操作数的值位数的结果
  case ND_SHR:  // 右移运算
    if (node->ty->is_unsigned && node->ty->size == 8)  // 如果节点的类型是无符号类型且大小为8字节
      return (uint64_t)eval(node->lhs) >> eval(node->rhs);  // 将左操作数的值转换为无符号数进行右移运算
    return eval(node->lhs) >> eval(node->rhs);  // 求得左操作数的值右移右操作数的值位数的结果
  case ND_EQ:  // 等于运算
    return eval(node->lhs) == eval(node->rhs);  // 判断左操作数的值是否等于右操作数的值
  case ND_NE:  // 不等于运算
    return eval(node->lhs) != eval(node->rhs);  // 判断左操作数的值是否不等于右操作数的值
  case ND_LT:  // 小于运算
    if (node->lhs->ty->is_unsigned)  // 如果左操作数的类型是无符号类型
      return (uint64_t)eval(node->lhs) < eval(node->rhs);  // 将左操作数的值转换为无符号数进行比较
    return eval(node->lhs) < eval(node->rhs);  // 判断左操作数的值是否小于右操作数的值
  case ND_LE:  // 小于等于运算
    if (node->lhs->ty->is_unsigned)  // 如果左操作数的类型是无符号类型
      return (uint64_t)eval(node->lhs) <= eval(node->rhs);  // 将左操作数的值转换为无符号数进行比较
    return eval(node->lhs) <= eval(node->rhs);  // 判断左操作数的值是否小于等于右操作数的值
  case ND_COND:  // 条件运算
    return eval(node->cond) ? eval2(node->then, label) : eval2(node->els, label);  // 如果条件成立则求then子节点的值，否则求els子节点的值
  case ND_COMMA:  // 逗号运算
    return eval2(node->rhs, label);  // 求得右操作数的值
  case ND_NOT:  // 逻辑非运算
    return !eval(node->lhs);  // 对操作数的值取逻辑非
  case ND_BITNOT:  // 按位取反运算
    return ~eval(node->lhs);  // 对操作数的值按位取反
  case ND_LOGAND:  // 逻辑与运算
    return eval(node->lhs) && eval(node->rhs);  // 对左右操作数的值进行逻辑与运算
  case ND_LOGOR:  // 逻辑或运算
    return eval(node->lhs) || eval(node->rhs);  // 对左右操作数的值进行逻辑或运算
  case ND_CAST: {  // 类型转换运算
    int64_t val = eval2(node->lhs, label);  // 求得子节点的值
    if (is_integer(node->ty)) {  // 如果目标类型是整数类型
      switch (node->ty->size) {
      case 1: return node->ty->is_unsigned ? (uint8_t)val : (int8_t)val;  // 将值转换为8位整数类型
      case 2: return node->ty->is_unsigned ? (uint16_t)val : (int16_t)val;  // 将值转换为16位整数类型
      case 4: return node->ty->is_unsigned ? (uint32_t)val : (int32_t)val;  // 将值转换为32位整数类型
      }
    }
    return val;  // 返回转换后的值
  }
  case ND_ADDR:  // 地址运算
    return eval_rval(node->lhs, label);  // 求得左操作数的右值
  case ND_LABEL_VAL:  // 标签值
    *label = &node->unique_label;  // 将指针指向唯一标签
    return 0;  // 返回0
  case ND_MEMBER:  // 结构体成员访问
    if (!label)  // 如果label为空
      error_tok(node->tok, "not a compile-time constant");  // 报错，不是编译时常量
    if (node->ty->kind != TY_ARRAY)  // 如果节点类型不是数组类型
      error_tok(node->tok, "invalid initializer");  // 报错，无效的初始化
    return eval_rval(node->lhs, label) + node->member->offset;  // 求得左操作数的右值，加上成员偏移量
  case ND_VAR:  // 变量
    if (!label)  // 如果label为空
      error_tok(node->tok, "not a compile-time constant");  // 报错，不是编译时常量
    if (node->var->ty->kind != TY_ARRAY && node->var->ty->kind != TY_FUNC)  // 如果变量的类型不是数组类型且不是函数类型
      error_tok(node->tok, "invalid initializer");  // 报错，无效的初始化
    *label = &node->var->name;  // 将指针指向变量名
    return 0;  // 返回0
  case ND_NUM:  // 数字
    return node->val;  // 返回数字的值
  }

  error_tok(node->tok, "not a compile-time constant");  // 报错，不是编译时常量
}

static int64_t eval_rval(Node *node, char ***label) {
  switch (node->kind) {
  case ND_VAR:  // 变量
    if (node->var->is_local)  // 如果变量是局部变量
      error_tok(node->tok, "not a compile-time constant");  // 报错，不是编译时常量
    *label = &node->var->name;  // 将指针指向变量名
    return 0;  // 返回0
  case ND_DEREF:  // 解引用
    return eval2(node->lhs, label);  // 求得左操作数的值
  case ND_MEMBER:  // 结构体成员访问
    return eval_rval(node->lhs, label) + node->member->offset;  // 对左操作数进行递归求值，加上成员偏移量
  }

  error_tok(node->tok, "invalid initializer");  // 报错，无效的初始化
}

static bool is_const_expr(Node *node) {
  add_type(node);  // 添加类型信息到节点

  switch (node->kind) {
  case ND_ADD:  // 加法运算
  case ND_SUB:  // 减法运算
  case ND_MUL:  // 乘法运算
  case ND_DIV:  // 除法运算
  case ND_BITAND:  // 按位与运算
  case ND_BITOR:  // 按位或运算
  case ND_BITXOR:  // 按位异或运算
  case ND_SHL:  // 左移运算
  case ND_SHR:  // 右移运算
  case ND_EQ:  // 等于运算
  case ND_NE:  // 不等于运算
  case ND_LT:  // 小于运算
  case ND_LE:  // 小于等于运算
  case ND_LOGAND:  // 逻辑与运算
  case ND_LOGOR:  // 逻辑或运算
    return is_const_expr(node->lhs) && is_const_expr(node->rhs);  // 左右操作数都是常量表达式才返回true
  case ND_COND:  // 条件运算
    if (!is_const_expr(node->cond))  // 如果条件不是常量表达式
      return false;  // 返回false
    return is_const_expr(eval(node->cond) ? node->then : node->els);  // 根据条件选择求then子节点还是求else子节点的值判断是否是常量表达式
  case ND_COMMA:  // 逗号运算
    return is_const_expr(node->rhs);  // 判断右操作数是否是常量表达式
  case ND_NEG:  // 取反运算
  case ND_NOT:  // 逻辑非运算
  case ND_BITNOT:  // 按位取反运算
  case ND_CAST:  // 类型转换运算
    return is_const_expr(node->lhs);  // 判断操作数是否是常量表达式
  case ND_NUM:  // 数字
    return true;  // 数字是常量表达式
  }

  return false;  // 其他情况都返回false
}

int64_t const_expr(Token **rest, Token *tok) {
  Node *node = conditional(rest, tok);  // 调用conditional函数解析条件表达式并返回对应的语法树节点
  return eval(node);  // 对语法树节点进行求值并返回结果
}

static double eval_double(Node *node) {
  add_type(node);  // 添加类型信息到节点

  if (is_integer(node->ty)) {  // 如果节点是整数类型
    if (node->ty->is_unsigned)  // 如果是无符号整数
      return (unsigned long)eval(node);  // 将节点的值转换为无符号长整型并返回
    return eval(node);  // 返回节点的值
  }

  switch (node->kind) {
  case ND_ADD:  // 加法运算
    return eval_double(node->lhs) + eval_double(node->rhs);  // 对左右操作数进行浮点数求值并相加
  case ND_SUB:  // 减法运算
    return eval_double(node->lhs) - eval_double(node->rhs);  // 对左右操作数进行浮点数求值并相减
  case ND_MUL:  // 乘法运算
    return eval_double(node->lhs) * eval_double(node->rhs);  // 对左右操作数进行浮点数求值并相乘
  case ND_DIV:  // 除法运算
    return eval_double(node->lhs) / eval_double(node->rhs);  // 对左右操作数进行浮点数求值并相除
  case ND_NEG:  // 取反运算
    return -eval_double(node->lhs);  // 对操作数进行浮点数求值并取反
  case ND_COND:  // 条件运算
    return eval_double(node->cond) ? eval_double(node->then) : eval_double(node->els);  // 根据条件选择求then子节点还是求else子节点的值作为结果
  case ND_COMMA:  // 逗号运算
    return eval_double(node->rhs);  // 对右操作数进行浮点数求值并返回
  case ND_CAST:  // 类型转换运算
    if (is_flonum(node->lhs->ty))  // 如果是浮点数类型
      return eval_double(node->lhs);  // 对操作数进行浮点数求值并返回
    return eval(node->lhs);  // 对操作数进行整数求值并返回
  case ND_NUM:  // 数字
    return node->fval;  // 返回节点的浮点数值
  }

  error_tok(node->tok, "not a compile-time constant");  // 报错，不是编译时常量
}

// 将 op= 运算符转换为包含赋值的表达式。

// 一般情况下，A op= C 被转换为 ``tmp = &A, *tmp = *tmp op B。 
// 但是，如果给定的表达式是 A.x op= C的形式，输入将被转换为tmp = &A, (*tmp).x = (*tmp).x op C` 以处理对位字段的赋值。
static Node *to_assign(Node *binary) {
  add_type(binary->lhs);  // 添加左操作数的类型信息到节点
  add_type(binary->rhs);  // 添加右操作数的类型信息到节点
  Token *tok = binary->tok;  // 获取当前节点的标记信息

  // 将 `A.x op= C` 转换为 `tmp = &A, (*tmp).x = (*tmp).x op C`。
  if (binary->lhs->kind == ND_MEMBER) {  // 如果左操作数是成员访问节点
    Obj *var = new_lvar("", pointer_to(binary->lhs->lhs->ty));  // 创建一个指向左操作数类型的指针局部变量

    Node *expr1 = new_binary(ND_ASSIGN, new_var_node(var, tok),  // 创建赋值表达式节点 tmp = &A
                             new_unary(ND_ADDR, binary->lhs->lhs, tok), tok);

    Node *expr2 = new_unary(ND_MEMBER,
                            new_unary(ND_DEREF, new_var_node(var, tok), tok),
                            tok);  // 创建成员访问节点 (*tmp).x
    expr2->member = binary->lhs->member;

    Node *expr3 = new_unary(ND_MEMBER,
                            new_unary(ND_DEREF, new_var_node(var, tok), tok),
                            tok);  // 创建成员访问节点 (*tmp).x
    expr3->member = binary->lhs->member;

    Node *expr4 = new_binary(ND_ASSIGN, expr2,
                             new_binary(binary->kind, expr3, binary->rhs, tok),
                             tok);  // 创建赋值表达式节点 (*tmp).x = (*tmp).x op C

    return new_binary(ND_COMMA, expr1, expr4, tok);  // 返回逗号运算节点 tmp = &A, (*tmp).x = (*tmp).x op C
  }

  // 如果 A 是原子类型，将 `A op= B` 转换为
  //
  // ({
  //   T1 *addr = &A; T2 val = (B); T1 old = *addr; T1 new;
  //   do {
  //     new = old op val;
  //   } while (!atomic_compare_exchange_strong(addr, &old, new));
  //   new;
  // })

  if (binary->lhs->ty->is_atomic) {  // 如果左操作数的类型是原子类型
    Node head = {};  // 创建一个空节点作为头节点
    Node *cur = &head;  // 创建一个指针 cur 指向头节点

    Obj *addr = new_lvar("", pointer_to(binary->lhs->ty));  // 创建指向左操作数类型的指针局部变量 addr
    Obj *val = new_lvar("", binary->rhs->ty);  // 创建右操作数类型的局部变量 val
    Obj *old = new_lvar("", binary->lhs->ty);  // 创建左操作数类型的局部变量 old
    Obj *new = new_lvar("", binary->lhs->ty);  // 创建左操作数类型的局部变量 new

    cur = cur->next =
      new_unary(ND_EXPR_STMT,
                new_binary(ND_ASSIGN, new_var_node(addr, tok),  // 创建赋值表达式节点 addr = &A
                           new_unary(ND_ADDR, binary->lhs, tok), tok),
                tok);  // 将赋值表达式节点添加到链表中

    cur = cur->next =
      new_unary(ND_EXPR_STMT,
                new_binary(ND_ASSIGN, new_var_node(val, tok), binary->rhs, tok),
                tok);  // 创建赋值表达式节点 val = B，并将其添加到链表中

    cur = cur->next =
      new_unary(ND_EXPR_STMT,
                new_binary(ND_ASSIGN, new_var_node(old, tok),
                           new_unary(ND_DEREF, new_var_node(addr, tok), tok), tok),
                tok);  // 创建赋值表达式节点 old = *addr，并将其添加到链表中

    Node *loop = new_node(ND_DO, tok);  // 创建一个 do-while 循环节点
    loop->brk_label = new_unique_name();  // 为循环节点设置唯一的 break 标签
    loop->cont_label = new_unique_name();  // 为循环节点设置唯一的 continue 标签

    Node *body = new_binary(ND_ASSIGN,  // 创建一个赋值表达式节点作为循环体
                            new_var_node(new, tok),  // 左操作数为 new
                            new_binary(binary->kind, new_var_node(old, tok),  // 右操作数为 old op val
                                       new_var_node(val, tok), tok),
                            tok);

    loop->then = new_node(ND_BLOCK, tok);  // 创建一个块节点作为循环体的子节点
    loop->then->body = new_unary(ND_EXPR_STMT, body, tok);  // 将赋值表达式节点添加到块节点中

    Node *cas = new_node(ND_CAS, tok);  // 创建一个 CAS (Compare and Swap) 节点
    cas->cas_addr = new_var_node(addr, tok);  // 设置 CAS 节点的地址为 addr
    cas->cas_old = new_unary(ND_ADDR, new_var_node(old, tok), tok);  // 设置 CAS 节点的旧值为 &old
    cas->cas_new = new_var_node(new, tok);  // 设置 CAS 节点的新值为 new
    loop->cond = new_unary(ND_NOT, cas, tok);  // 创建一个取反节点作为循环条件

    cur = cur->next = loop;  // 将循环节点添加到链表中
    cur = cur->next = new_unary(ND_EXPR_STMT, new_var_node(new, tok), tok);  // 将 new 表达式添加到链表中

    Node *node = new_node(ND_STMT_EXPR, tok);  // 创建一个语句表达式节点
    node->body = head.next;  // 将链表中的节点作为语句表达式节点的子节点
    return node;  // 返回语句表达式节点
  }

  // 将 `A op= B` 转换为 ``tmp = &A, *tmp = *tmp op B`。
  Obj *var = new_lvar("", pointer_to(binary->lhs->ty));  // 创建一个指向左操作数类型的指针局部变量 var

  // 创建赋值表达式节点 expr1，将 var = &A 添加到节点中
  Node *expr1 = new_binary(ND_ASSIGN, new_var_node(var, tok),
                           new_unary(ND_ADDR, binary->lhs, tok), tok);

  // 创建赋值表达式节点 expr2，将 *var = *var op B 添加到节点中
  Node *expr2 =
    new_binary(ND_ASSIGN,
               new_unary(ND_DEREF, new_var_node(var, tok), tok),
               new_binary(binary->kind,
                          new_unary(ND_DEREF, new_var_node(var, tok), tok),
                          binary->rhs,
                          tok),
               tok);

  // 返回逗号表达式节点，将 expr1 和 expr2 添加到节点中
  return new_binary(ND_COMMA, expr1, expr2, tok);
}

// assign    = conditional (assign-op assign)?
// assign-op = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^="
//           | "<<=" | ">>="

// 解析赋值表达式
static Node *assign(Token **rest, Token *tok) {
  Node *node = conditional(&tok, tok);  // 解析条件表达式并得到节点

  if (equal(tok, "="))  // 检查是否为 "=" 运算符
    return new_binary(ND_ASSIGN, node, assign(rest, tok->next), tok);  // 创建赋值表达式节点

  if (equal(tok, "+="))
    return to_assign(new_add(node, assign(rest, tok->next), tok));  // 创建加法赋值表达式节点

  if (equal(tok, "-="))
    return to_assign(new_sub(node, assign(rest, tok->next), tok));  // 创建减法赋值表达式节点

  if (equal(tok, "*="))
    return to_assign(new_binary(ND_MUL, node, assign(rest, tok->next), tok));  // 创建乘法赋值表达式节点

  if (equal(tok, "/="))
    return to_assign(new_binary(ND_DIV, node, assign(rest, tok->next), tok));  // 创建除法赋值表达式节点

  if (equal(tok, "%="))
    return to_assign(new_binary(ND_MOD, node, assign(rest, tok->next), tok));  // 创建取模赋值表达式节点

  if (equal(tok, "&="))
    return to_assign(new_binary(ND_BITAND, node, assign(rest, tok->next), tok));  // 创建按位与赋值表达式节点

  if (equal(tok, "|="))
    return to_assign(new_binary(ND_BITOR, node, assign(rest, tok->next), tok));  // 创建按位或赋值表达式节点

  if (equal(tok, "^="))
    return to_assign(new_binary(ND_BITXOR, node, assign(rest, tok->next), tok));  // 创建按位异或赋值表达式节点

  if (equal(tok, "<<="))
    return to_assign(new_binary(ND_SHL, node, assign(rest, tok->next), tok));  // 创建左移赋值表达式节点

  if (equal(tok, ">>="))
    return to_assign(new_binary(ND_SHR, node, assign(rest, tok->next), tok));  // 创建右移赋值表达式节点

  *rest = tok;
  return node;
}

// 条件表达式 = 逻辑或表达式 ("?" 表达式? ":" 条件表达式)?
// conditional = logor ("?" expr? ":" conditional)?
static Node *conditional(Token **rest, Token *tok) {
  Node *cond = logor(&tok, tok);  // 解析逻辑或表达式并得到节点

  if (!equal(tok, "?")) {  // 检查是否存在 "?" 符号
    *rest = tok;
    return cond;
  }

  if (equal(tok->next, ":")) {
    // [GNU] 将 `a ?: b` 编译为 `tmp = a, tmp ? tmp : b`。
    add_type(cond);  // 添加类型信息到条件表达式节点
    Obj *var = new_lvar("", cond->ty);  // 创建一个局部变量 var，类型为条件表达式的类型
    Node *lhs = new_binary(ND_ASSIGN, new_var_node(var, tok), cond, tok);  // 创建赋值表达式节点，将 var = cond 添加到节点中
    Node *rhs = new_node(ND_COND, tok);  // 创建条件表达式节点
    rhs->cond = new_var_node(var, tok);  // 设置条件为 var
    rhs->then = new_var_node(var, tok);  // 设置真值分支为 var
    rhs->els = conditional(rest, tok->next->next);  // 递归解析条件表达式并得到假值分支
    return new_binary(ND_COMMA, lhs, rhs, tok);  // 返回逗号表达式节点，将 lhs 和 rhs 添加到节点中
  }

  Node *node = new_node(ND_COND, tok);  // 创建条件表达式节点
  node->cond = cond;  // 设置条件为 cond
  node->then = expr(&tok, tok->next);  // 解析表达式并得到真值分支
  tok = skip(tok, ":");  // 跳过 ":" 符号
  node->els = conditional(rest, tok);  // 递归解析条件表达式并得到假值分支
  return node;
}

// logor = logand ("||" logand)*
// 逻辑或表达式 = 逻辑与表达式 ("||" 逻辑与表达式)*
static Node *logor(Token **rest, Token *tok) {
  Node *node = logand(&tok, tok);  // 解析逻辑与表达式并得到节点
  while (equal(tok, "||")) {  // 如果当前符号是 "||"
    Token *start = tok;  // 记录起始位置
    node = new_binary(ND_LOGOR, node, logand(&tok, tok->next), start);  // 创建逻辑或表达式节点
  }
  *rest = tok;  // 更新 rest 指针
  return node;  // 返回节点
}

// logand = bitor ("&&" bitor)*
// 逻辑与表达式 = 按位或表达式 ("&&" 按位或表达式)*
static Node *logand(Token **rest, Token *tok) {
  Node *node = bitor(&tok, tok);  // 解析按位或表达式并得到节点
  while (equal(tok, "&&")) {  // 如果当前符号是 "&&"
    Token *start = tok;  // 记录起始位置
    node = new_binary(ND_LOGAND, node, bitor(&tok, tok->next), start);  // 创建逻辑与表达式节点
  }
  *rest = tok;  // 更新 rest 指针
  return node;  // 返回节点
}

// bitor = bitxor ("|" bitxor)*
// 按位或表达式 = 按位异或表达式 ("|" 按位异或表达式)*
static Node *bitor(Token **rest, Token *tok) {
  Node *node = bitxor(&tok, tok);  // 解析按位异或表达式并得到节点
  while (equal(tok, "|")) {  // 如果当前符号是 "|"
    Token *start = tok;  // 记录起始位置
    node = new_binary(ND_BITOR, node, bitxor(&tok, tok->next), start);  // 创建按位或表达式节点
  }
  *rest = tok;  // 更新 rest 指针
  return node;  // 返回节点
}

// bitxor = bitand ("^" bitand)*
// 按位异或表达式 = 按位与表达式 ("^" 按位与表达式)*
static Node *bitxor(Token **rest, Token *tok) {
  Node *node = bitand(&tok, tok);  // 解析按位与表达式并得到节点
  while (equal(tok, "^")) {  // 如果当前符号是 "^"
    Token *start = tok;  // 记录起始位置
    node = new_binary(ND_BITXOR, node, bitand(&tok, tok->next), start);  // 创建按位异或表达式节点
  }
  *rest = tok;  // 更新 rest 指针
  return node;  // 返回节点
}

// bitand = equality ("&" equality)*
// 按位与表达式 = 相等性表达式 ("&" 相等性表达式)*
static Node *bitand(Token **rest, Token *tok) {
  Node *node = equality(&tok, tok);  // 解析相等性表达式并得到节点
  while (equal(tok, "&")) {  // 如果当前符号是 "&"
    Token *start = tok;  // 记录起始位置
    node = new_binary(ND_BITAND, node, equality(&tok, tok->next), start);  // 创建按位与表达式节点
  }
  *rest = tok;  // 更新 rest 指针
  return node;  // 返回节点
}

// equality = relational ("==" relational | "!=" relational)*
// 相等性表达式 = 关系表达式 ("==" 关系表达式 | "!=" 关系表达式)*
static Node *equality(Token **rest, Token *tok) {
  Node *node = relational(&tok, tok);  // 解析关系表达式并得到节点

  for (;;) {
    Token *start = tok;  // 记录起始位置

    if (equal(tok, "==")) {  // 如果当前符号是 "=="
      node = new_binary(ND_EQ, node, relational(&tok, tok->next), start);  // 创建相等节点
      continue;
    }

    if (equal(tok, "!=")) {  // 如果当前符号是 "!="
      node = new_binary(ND_NE, node, relational(&tok, tok->next), start);  // 创建不等节点
      continue;
    }

    *rest = tok;  // 更新 rest 指针
    return node;  // 返回节点
  }
}

// relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
// 关系表达式 = 移位表达式 ("<" 移位表达式 | "<=" 移位表达式 | ">" 移位表达式 | ">=" 移位表达式)*
static Node *relational(Token **rest, Token *tok) {
  Node *node = shift(&tok, tok);  // 解析移位表达式并得到节点

  for (;;) {
    Token *start = tok;  // 记录起始位置

    if (equal(tok, "<")) {  // 如果当前符号是 "<"
      node = new_binary(ND_LT, node, shift(&tok, tok->next), start);  // 创建小于节点
      continue;
    }

    if (equal(tok, "<=")) {  // 如果当前符号是 "<="
      node = new_binary(ND_LE, node, shift(&tok, tok->next), start);  // 创建小于等于节点
      continue;
    }

    if (equal(tok, ">")) {  // 如果当前符号是 ">"
      node = new_binary(ND_LT, shift(&tok, tok->next), node, start);  // 创建大于节点
      continue;
    }

    if (equal(tok, ">=")) {  // 如果当前符号是 ">="
      node = new_binary(ND_LE, shift(&tok, tok->next), node, start);  // 创建大于等于节点
      continue;
    }

    *rest = tok;  // 更新 rest 指针
    return node;  // 返回节点
  }
}

// shift = add ("<<" add | ">>" add)*
// 移位表达式 = 加法表达式 ("<<" 加法表达式 | ">>" 加法表达式)*
static Node *shift(Token **rest, Token *tok) {
  Node *node = add(&tok, tok);  // 解析加法表达式并得到节点

  for (;;) {
    Token *start = tok;  // 记录起始位置

    if (equal(tok, "<<")) {  // 如果当前符号是 "<<"
      node = new_binary(ND_SHL, node, add(&tok, tok->next), start);  // 创建左移节点
      continue;
    }

    if (equal(tok, ">>")) {  // 如果当前符号是 ">>"
      node = new_binary(ND_SHR, node, add(&tok, tok->next), start);  // 创建右移节点
      continue;
    }

    *rest = tok;  // 更新 rest 指针
    return node;  // 返回节点
  }
}

// 在C语言中，+ 运算符被重载用于执行指针算术。
// 如果 p 是一个指针，p+n 不是简单地将 n 加到 p 的值上，
// 而是将 sizeof(*p)*n 加到 p 的值上，这样 p+n 就指向了 p 之后 n 个元素（而不是字节）的位置。
// 换句话说，在将整数值添加到指针值之前，我们需要进行缩放操作。这个函数负责进行缩放操作。
static Node *new_add(Node *lhs, Node *rhs, Token *tok) {
  add_type(lhs);  // 添加类型信息到左操作数
  add_type(rhs);  // 添加类型信息到右操作数

  // num + num
  // 如果左右操作数都是数字类型，则执行数字相加
  if (is_numeric(lhs->ty) && is_numeric(rhs->ty))
    return new_binary(ND_ADD, lhs, rhs, tok);  // 创建加法节点

  // 检查是否为无效操作数（即左右操作数都是指针类型）
  if (lhs->ty->base && rhs->ty->base)
    error_tok(tok, "invalid operands");  // 错误：无效的操作数

  // Canonicalize `num + ptr` to `ptr + num`.
  // 将形如 `num + ptr` 的表达式规范化为 `ptr + num`
  if (!lhs->ty->base && rhs->ty->base) {
    Node *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // VLA + num
  // 如果左操作数是可变长度数组（VLA），右操作数是数字
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(ND_MUL, rhs, new_var_node(lhs->ty->base->vla_size, tok), tok);  // 将数字乘以 VLA 的大小
    return new_binary(ND_ADD, lhs, rhs, tok);  // 创建加法节点
  }

  // ptr + num
  // 如果左操作数是指针，右操作数是数字
  rhs = new_binary(ND_MUL, rhs, new_long(lhs->ty->base->size, tok), tok);  // 将数字乘以指针所指类型的大小
  return new_binary(ND_ADD, lhs, rhs, tok);  // 创建加法节点
}

// Like `+`, `-` is overloaded for the pointer type.
// 函数用于处理指针类型的减法操作符 `-`
static Node *new_sub(Node *lhs, Node *rhs, Token *tok) {
  add_type(lhs);  // 添加类型信息到左操作数
  add_type(rhs);  // 添加类型信息到右操作数

  // num - num
  // 如果左右操作数都是数字类型，则执行数字相减
  if (is_numeric(lhs->ty) && is_numeric(rhs->ty))
    return new_binary(ND_SUB, lhs, rhs, tok);  // 创建减法节点

  // VLA + num
  // 如果左操作数是可变长度数组（VLA），右操作数是数字
  if (lhs->ty->base->kind == TY_VLA) {
    rhs = new_binary(ND_MUL, rhs, new_var_node(lhs->ty->base->vla_size, tok), tok);  // 将数字乘以 VLA 的大小
    add_type(rhs);  // 添加类型信息到右操作数
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);  // 创建减法节点
    node->ty = lhs->ty;  // 设置节点类型为左操作数的类型
    return node;
  }

  // ptr - num
  // 如果左操作数是指针，右操作数是数字
  if (lhs->ty->base && is_integer(rhs->ty)) {
    rhs = new_binary(ND_MUL, rhs, new_long(lhs->ty->base->size, tok), tok);  // 将数字乘以指针所指类型的大小
    add_type(rhs);  // 添加类型信息到右操作数
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);  // 创建减法节点
    node->ty = lhs->ty;  // 设置节点类型为左操作数的类型
    return node;
  }

  // ptr - ptr, which returns how many elements are between the two.
  // 如果左右操作数都是指针类型，则返回两者之间的元素个数
  if (lhs->ty->base && rhs->ty->base) {
    Node *node = new_binary(ND_SUB, lhs, rhs, tok);  // 创建减法节点
    node->ty = ty_long;  // 设置节点类型为长整型
    return new_binary(ND_DIV, node, new_num(lhs->ty->base->size, tok), tok);  // 创建除法节点，计算两者之间的元素个数
  }

  error_tok(tok, "invalid operands");  // 错误：无效的操作数
}

// add = mul ("+" mul | "-" mul)*
// 定义函数add，接收两个参数：指向Token类型指针的指针rest和Token类型指针tok
static Node *add(Token **rest, Token *tok) {  // add = mul ("+" mul | "-" mul)*
  Node *node = mul(&tok, tok);  // 声明一个Node类型指针变量node，并将其初始化为调用mul函数的返回值，传入参数是tok的地址和tok的值

  for (;;) {  // 进入循环，循环条件为真
    Token *start = tok;  // 声明一个Token类型指针变量start，并将其初始化为tok的值

    if (equal(tok, "+")) {  // 如果当前的tok的值与"+"相等
      node = new_add(node, mul(&tok, tok->next), start);  // 调用new_add函数创建一个新的加法节点，并将其赋值给node，传入参数为node的值、调用mul函数的返回值和tok的值
      continue;  // 继续下一轮循环
    }

    if (equal(tok, "-")) {  // 如果当前的tok的值与"-"相等
      node = new_sub(node, mul(&tok, tok->next), start);  // 调用new_sub函数创建一个新的减法节点，并将其赋值给node，传入参数为node的值、调用mul函数的返回值和tok的值
      continue;  // 继续下一轮循环
    }

    *rest = tok;  // 将当前的tok的值赋给指针rest指向的变量
    return node;  // 返回node的值
  }
}

// mul = cast ("*" cast | "/" cast | "%" cast)*
// 定义函数mul，接收两个参数：指向Token类型指针的指针rest和Token类型指针tok
static Node *mul(Token **rest, Token *tok) {  // mul = cast ("*" cast | "/" cast | "%" cast)*
  Node *node = cast(&tok, tok);  // 声明一个Node类型指针变量node，并将其初始化为调用cast函数的返回值，传入参数是tok的地址和tok的值

  for (;;) {  // 进入循环，循环条件为真
    Token *start = tok;  // 声明一个Token类型指针变量start，并将其初始化为tok的值

    if (equal(tok, "*")) {  // 如果当前的tok的值与"*"相等
      node = new_binary(ND_MUL, node, cast(&tok, tok->next), start);  // 调用new_binary函数创建一个新的二进制节点，节点类型为ND_MUL，将其赋值给node，传入参数为node的值、调用cast函数的返回值和tok的值
      continue;  // 继续下一轮循环
    }

    if (equal(tok, "/")) {  // 如果当前的tok的值与"/"相等
      node = new_binary(ND_DIV, node, cast(&tok, tok->next), start);  // 调用new_binary函数创建一个新的二进制节点，节点类型为ND_DIV，将其赋值给node，传入参数为node的值、调用cast函数的返回值和tok的值
      continue;  // 继续下一轮循环
    }

    if (equal(tok, "%")) {  // 如果当前的tok的值与"%"相等
      node = new_binary(ND_MOD, node, cast(&tok, tok->next), start);  // 调用new_binary函数创建一个新的二进制节点，节点类型为ND_MOD，将其赋值给node，传入参数为node的值、调用cast函数的返回值和tok的值
      continue;  // 继续下一轮循环
    }

    *rest = tok;  // 将当前的tok的值赋给指针rest指向的变量
    return node;  // 返回node的值
  }
}

// cast = "(" type-name ")" cast | unary
// 定义函数cast，接收两个参数：指向Token类型指针的指针rest和Token类型指针tok
static Node *cast(Token **rest, Token *tok) {  // cast = "(" type-name ")" cast | unary
  if (equal(tok, "(") && is_typename(tok->next)) {  // 如果当前的tok的值与"("相等，并且tok的下一个标记是类型名
    Token *start = tok;  // 声明一个Token类型指针变量start，并将其初始化为tok的值
    Type *ty = typename(&tok, tok->next);  // 声明一个Type类型指针变量ty，并将其初始化为typename函数的返回值，传入参数是tok的地址和tok的下一个标记的值
    tok = skip(tok, ")");  // 将tok的值更新为skip函数的返回值，传入参数是tok的值和")"

    if (equal(tok, "{"))  // 如果当前的tok的值与"{"相等，表示是复合字面量
      return unary(rest, start);  // 调用unary函数，返回其返回值，传入参数是指针rest指向的变量和start的值

    Node *node = new_cast(cast(rest, tok), ty);  // 声明一个Node类型指针变量node，并将其初始化为new_cast函数的返回值，传入参数是调用cast函数的返回值和ty的值
    node->tok = start;  // 将node的tok成员更新为start的值
    return node;  // 返回node的值
  }

  return unary(rest, tok);  // 如果不满足上述条件，直接调用unary函数，返回其返回值，传入参数是指针rest指向的变量和tok的值
}

// unary = ("+" | "-" | "*" | "&" | "!" | "~") cast
//       | ("++" | "--") unary
//       | "&&" ident
//       | postfix

// 定义函数unary，接收两个参数：指向Token类型指针的指针rest和Token类型指针tok
static Node *unary(Token **rest, Token *tok) {  // unary = ("+" | "-" | "*" | "&" | "!" | "~") cast | ("++" | "--") unary | "&&" ident | postfix
  if (equal(tok, "+"))  // 如果当前的tok的值是"+"
    return cast(rest, tok->next);  // 返回调用cast函数的返回值，传入参数是指针rest指向的变量和tok->next的值

  if (equal(tok, "-"))  // 如果当前的tok的值是"-"
    return new_unary(ND_NEG, cast(rest, tok->next), tok);  // 返回调用new_unary函数的返回值，传入参数是ND_NEG、调用cast函数的返回值和tok的值

  if (equal(tok, "&")) {  // 如果当前的tok的值是"&"
    Node *lhs = cast(rest, tok->next);  // 声明一个Node类型指针变量lhs，并将其初始化为调用cast函数的返回值，传入参数是指针rest指向的变量和tok->next的值
    add_type(lhs);  // 调用add_type函数，传入参数是lhs的值
    if (lhs->kind == ND_MEMBER && lhs->member->is_bitfield)  // 如果lhs的kind成员是ND_MEMBER且lhs->member->is_bitfield为真
      error_tok(tok, "cannot take address of bitfield");  // 调用error_tok函数，传入参数是tok的值和"cannot take address of bitfield"
    return new_unary(ND_ADDR, lhs, tok);  // 返回调用new_unary函数的返回值，传入参数是ND_ADDR、lhs的值和tok的值
  }

  if (equal(tok, "*")) {    // 如果当前的词法单元（tok）与 "*" 相等，则执行以下代码块
    // [https://www.sigbus.info/n1570#6.5.3.2p4] 这是C规范中的一个奇怪之处，
    // 但对函数进行解引用不会产生任何效果。如果foo是一个函数，`*foo`、`**foo`或`*****foo`
    // 都等同于`foo`。
    Node *node = cast(rest, tok->next);    // 将从 tok->next 开始的词法单元序列转换为一个节点（Node）并将结果保存到 node 变量中
    add_type(node);    // 为节点（node）添加类型信息
    if (node->ty->kind == TY_FUNC)    // 如果节点（node）的类型是函数类型（TY_FUNC）
        return node;    // 直接返回节点（node）
    return new_unary(ND_DEREF, node, tok);    // 否则，创建一个一元操作符节点（ND_DEREF），其操作数为节点（node），操作符为词法单元（tok），并返回该新创建的节点
  }

  if (equal(tok, "!")) // 如果当前的符号和"!"相等
    return new_unary(ND_NOT, cast(rest, tok->next), tok); // 创建一个一元节点，操作符为ND_NOT，操作数为cast(rest, tok->next)，返回该节点

  if (equal(tok, "~")) // 如果当前的符号和"~"相等
      return new_unary(ND_BITNOT, cast(rest, tok->next), tok); // 创建一个一元节点，操作符为ND_BITNOT，操作数为cast(rest, tok->next)，返回该节点

  // 将++i解析为i+=1
  if (equal(tok, "++")) // 如果当前的符号和"++"相等
      return to_assign(new_add(unary(rest, tok->next), new_num(1, tok), tok)); // 创建一个赋值节点，左操作数为unary(rest, tok->next)，右操作数为值为1的节点new_num(1, tok)，操作符为new_add，返回该节点

  // 将--i解析为i-=1
  if (equal(tok, "--")) // 如果当前的符号和"--"相等
      return to_assign(new_sub(unary(rest, tok->next), new_num(1, tok), tok)); // 创建一个赋值节点，左操作数为unary(rest, tok->next)，右操作数为值为1的节点new_num(1, tok)，操作符为new_sub，返回该节点

  // [GNU] 标签作为值
  if (equal(tok, "&&")) { // 如果当前的符号和"&&"相等
      Node *node = new_node(ND_LABEL_VAL, tok); // 创建一个新的节点，节点类型为ND_LABEL_VAL，标记为tok
      node->label = get_ident(tok->next); // 设置节点的标签为tok的下一个标识符
      node->goto_next = gotos; // 设置节点的goto_next为当前的gotos
      gotos = node; // 更新gotos为当前节点
      *rest = tok->next->next; // 更新rest为tok的下一个的下一个
      return node; // 返回该节点
  }

  return postfix(rest, tok); // 返回postfix(rest, tok)
}

// 结构体成员的语法规则：可以是零个或多个 (declspec declarator (","  declarator)* ";")
// struct-members = (declspec declarator (","  declarator)* ";")*
// 定义静态函数struct_members，参数为指向指针的指针rest，指向Token的指针tok，指向Type的指针ty
static void struct_members(Token **rest, Token *tok, Type *ty) {
  Member head = {}; // 创建一个Member结构体变量head，并初始化为默认值
  Member *cur = &head; // 创建一个Member指针cur，指向head
  int idx = 0; // 创建一个整型变量idx，初始化为0

  while (!equal(tok, "}")) { // 当当前的符号不等于"}"时循环执行
    VarAttr attr = {}; // 创建一个VarAttr结构体变量attr，并初始化为默认值
    Type *basety = declspec(&tok, tok, &attr); // 调用declspec函数，解析类型声明符，返回解析后的类型，并更新tok和attr
    bool first = true; // 创建一个布尔型变量first，初始化为true

    // 匿名结构体成员
    if ((basety->kind == TY_STRUCT || basety->kind == TY_UNION) && // 如果基本类型的kind为TY_STRUCT或TY_UNION
        consume(&tok, tok, ";")) { // 并且当前的符号为";"
      Member *mem = calloc(1, sizeof(Member)); // 分配一个Member大小的内存空间，并创建一个Member指针mem指向该空间
      mem->ty = basety; // 设置mem的ty为basety
      mem->idx = idx++; // 设置mem的idx为idx，并将idx自增1
      mem->align = attr.align ? attr.align : mem->ty->align; // 设置mem的align为attr.align，如果attr.align为0，则使用mem->ty->align的值
      cur = cur->next = mem; // 将cur的next指针指向mem，并将cur指向mem
      continue; // 继续下一次循环
    }

    // 解析常规结构体成员
    while (!consume(&tok, tok, ";")) { // 当前符号不是";"时循环执行
      if (!first) // 如果不是第一次循环
        tok = skip(tok, ","); // 跳过逗号
      first = false; // 将first标记为false

      Member *mem = calloc(1, sizeof(Member)); // 分配一个Member结构体大小的内存空间，并创建指向该空间的Member指针mem
      mem->ty = declarator(&tok, tok, basety); // 解析声明符，返回解析后的类型，并更新tok
      mem->name = mem->ty->name; // 设置成员的名称为类型的名称
      mem->idx = idx++; // 设置成员的索引为idx，然后将idx自增1
      mem->align = attr.align ? attr.align : mem->ty->align; // 设置成员的对齐方式为attr.align，如果attr.align为0，则使用类型的对齐方式

      if (consume(&tok, tok, ":")) { // 如果当前符号是":"
        mem->is_bitfield = true; // 将成员标记为位域成员
        mem->bit_width = const_expr(&tok, tok); // 解析常量表达式，返回解析后的表达式，并更新tok
      }

      cur = cur->next = mem; // 将当前成员添加到链表中
    }
  }

  // 如果最后一个元素是一个不完整类型的数组，则称为"弹性数组成员"。
  // 它应该表现得好像它是一个大小为零的数组。
  if (cur != &head && cur->ty->kind == TY_ARRAY && cur->ty->array_len < 0) {
    cur->ty = array_of(cur->ty->base, 0); // 将最后一个元素的类型更改为大小为0的数组
    ty->is_flexible = true; // 设置类型为弹性数组类型
  }

  *rest = tok->next; // 更新rest指针，指向下一个Token
  ty->members = head.next; // 将头节点后面的成员链表赋值给类型的成员列表
}

// attribute = ("__attribute__" "(" "(" "packed" ")" ")")*
static Token *attribute_list(Token *tok, Type *ty) {
  while (consume(&tok, tok, "__attribute__")) { // 如果当前的符号是"__attribute__"，则进入循环
    tok = skip(tok, "("); // 跳过左括号
    tok = skip(tok, "("); // 跳过左括号

    bool first = true; // 标记是否是第一次循环执行

    while (!consume(&tok, tok, ")")) { // 当当前的符号不是")"时循环执行
      if (!first)
        tok = skip(tok, ","); // 如果不是第一次循环，则跳过逗号
      first = false; // 将first设置为false

      if (consume(&tok, tok, "packed")) { // 如果当前的符号是"packed"
        ty->is_packed = true; // 设置类型为packed类型
        continue; // 继续下一次循环
      }

      if (consume(&tok, tok, "aligned")) { // 如果当前的符号是"aligned"
        tok = skip(tok, "("); // 跳过左括号
        ty->align = const_expr(&tok, tok); // 解析常量表达式，返回解析后的表达式，并更新tok
        tok = skip(tok, ")"); // 跳过右括号
        continue; // 继续下一次循环
      }

      error_tok(tok, "unknown attribute"); // 报告未知的属性错误
    }

    tok = skip(tok, ")"); // 跳过右括号
  }

  return tok; // 返回更新后的tok
}

// struct-union-decl = attribute? ident? ("{" struct-members)?
static Type *struct_union_decl(Token **rest, Token *tok) {
  Type *ty = struct_type(); // 创建一个结构体类型
  tok = attribute_list(tok, ty); // 解析属性列表，并更新tok

  // 读取标签
  Token *tag = NULL;
  if (tok->kind == TK_IDENT) { // 如果当前符号是标识符
    tag = tok; // 记录标签
    tok = tok->next; // 更新tok
  }

  if (tag && !equal(tok, "{")) { // 如果存在标签并且当前符号不是"{"，表示只有标签而没有定义
    *rest = tok; // 更新rest指针，指向当前的tok

    Type *ty2 = find_tag(tag); // 在标签符号表中查找标签对应的类型
    if (ty2)
      return ty2; // 如果找到了对应的类型，则返回该类型

    ty->size = -1; // 设置类型的大小为-1，表示未定义的结构体
    push_tag_scope(tag, ty); // 将标签和类型推入标签符号表
    return ty; // 返回类型
  }

  tok = skip(tok, "{"); // 跳过左大括号

  // 构造一个结构体对象。
  struct_members(&tok, tok, ty); // 解析结构体成员，并更新tok
  *rest = attribute_list(tok, ty); // 解析属性列表，并更新rest指针

  if (tag) {
    // 如果这是一个重新定义，覆盖之前的类型。
    // 否则，注册该结构体类型。
    Type *ty2 = hashmap_get2(&scope->tags, tag->loc, tag->len); // 在标签符号表中查找标签对应的类型
    if (ty2) {
      *ty2 = *ty; // 如果找到了对应的类型，则覆盖之前的类型
      return ty2; // 返回覆盖后的类型
    }

    push_tag_scope(tag, ty); // 将标签和类型推入标签符号表
  }

  return ty; // 返回类型
}

// struct-decl = struct-union-decl
static Type *struct_decl(Token **rest, Token *tok) {
  Type *ty = struct_union_decl(rest, tok); // 解析结构体或联合体声明，并更新rest指针
  ty->kind = TY_STRUCT; // 将类型设置为结构体类型

  if (ty->size < 0)
    return ty; // 如果结构体的大小小于0，表示未定义的结构体，直接返回类型

  // 为结构体成员分配偏移量。
  int bits = 0; // 位字段的位数初始化为0

  for (Member *mem = ty->members; mem; mem = mem->next) { // 遍历结构体的成员
    if (mem->is_bitfield && mem->bit_width == 0) {
      // 零宽度的匿名位字段具有特殊含义。
      // 它只影响对齐。
      bits = align_to(bits, mem->ty->size * 8); // 对齐到指定的位数
    } else if (mem->is_bitfield) { // 如果是位字段
      int sz = mem->ty->size; // 获取位字段类型的大小
      if (bits / (sz * 8) != (bits + mem->bit_width - 1) / (sz * 8))
        bits = align_to(bits, sz * 8); // 如果位字段跨越了边界，需要对齐到指定的位数

      mem->offset = align_down(bits / 8, sz); // 计算位字段的偏移量
      mem->bit_offset = bits % (sz * 8); // 计算位字段的位偏移量
      bits += mem->bit_width; // 更新位字段的位数
    } else { // 非位字段成员
      if (!ty->is_packed)
        bits = align_to(bits, mem->align * 8); // 对齐到指定的位数
      mem->offset = bits / 8; // 计算成员的偏移量
      bits += mem->ty->size * 8; // 更新位数
    }

    if (!ty->is_packed && ty->align < mem->align)
      ty->align = mem->align; // 更新结构体的对齐方式

  }

  ty->size = align_to(bits, ty->align * 8) / 8; // 计算结构体的大小，并对齐到指定的字节
  return ty; // 返回类型
}

// union-decl = struct-union-decl
static Type *union_decl(Token **rest, Token *tok) {
  Type *ty = struct_union_decl(rest, tok); // 解析联合体声明，并更新rest指针
  ty->kind = TY_UNION; // 将类型设置为联合体类型

  if (ty->size < 0)
    return ty; // 如果联合体的大小小于0，表示未定义的联合体，直接返回类型

  // 对于联合体，我们不需要分配偏移量，因为它们已经初始化为零。但我们需要计算对齐和大小。
  for (Member *mem = ty->members; mem; mem = mem->next) { // 遍历联合体的成员
    if (ty->align < mem->align)
      ty->align = mem->align; // 更新联合体的对齐方式
    if (ty->size < mem->ty->size)
      ty->size = mem->ty->size; // 更新联合体的大小为最大成员的大小
  }
  ty->size = align_to(ty->size, ty->align); // 对齐联合体的大小
  return ty; // 返回类型
}

// Find a struct member by name.
// 根据名称查找结构体成员。
static Member *get_struct_member(Type *ty, Token *tok) {
  for (Member *mem = ty->members; mem; mem = mem->next) { // 遍历结构体的成员
    // 匿名结构体成员
    if ((mem->ty->kind == TY_STRUCT || mem->ty->kind == TY_UNION) && // 判断成员类型是否为结构体或联合体
        !mem->name) { // 判断成员是否为匿名的
      if (get_struct_member(mem->ty, tok)) // 递归查找匿名结构体成员
        return mem; // 如果找到了匿名结构体成员，返回该成员
      continue; // 继续查找下一个成员
    }

    // 常规结构体成员
    if (mem->name->len == tok->len && // 判断成员名称的长度是否与目标名称相同
        !strncmp(mem->name->loc, tok->loc, tok->len)) // 判断成员名称的字符串是否与目标名称相同
      return mem; // 如果找到了与目标名称匹配的成员，返回该成员
  }

  return NULL; // 未找到结构体成员，返回空指针
}

// 创建一个表示结构体成员访问的节点，例如 foo.bar，其中 foo 是一个结构体，bar 是成员名称。

// C语言有一个名为“匿名结构体”的特性，允许结构体拥有另一个无名结构体作为成员，例如：
//
//   struct { struct { int a; }; int b; } x;
//
// 匿名结构体的成员属于外部结构体的成员命名空间。因此，在上面的例子中，你可以将匿名结构体的成员“a”访问为“x.a”。

// 这个函数负责处理匿名结构体。
// 根据结构体节点和标记创建一个结构体成员访问的节点。
static Node *struct_ref(Node *node, Token *tok) {
  add_type(node); // 为节点添加类型信息
  if (node->ty->kind != TY_STRUCT && node->ty->kind != TY_UNION) // 判断节点的类型是否为结构体或联合体
    error_tok(node->tok, "not a struct nor a union"); // 报错，不是结构体或联合体类型

  Type *ty = node->ty; // 获取节点的类型

  for (;;) {
    Member *mem = get_struct_member(ty, tok); // 根据类型和标记查找结构体成员
    if (!mem) // 如果未找到成员
      error_tok(tok, "no such member"); // 报错，未找到该成员
    node = new_unary(ND_MEMBER, node, tok); // 创建一个表示结构体成员访问的一元节点
    node->member = mem; // 设置节点的成员属性为找到的成员
    if (mem->name) // 如果成员有名称（非匿名结构体成员）
      break; // 结束循环
    ty = mem->ty; // 继续查找匿名结构体成员
  }
  return node; // 返回结构体成员访问的节点
}

// Convert A++ to `(typeof A)((A += 1) - 1)`
// 将 A++ 转换为 `(typeof A)((A += 1) - 1)`
static Node *new_inc_dec(Node *node, Token *tok, int addend) {
  add_type(node); // 为节点添加类型信息
  return new_cast(new_add(to_assign(new_add(node, new_num(addend, tok), tok)), // 创建节点：(A += addend)
                          new_num(-addend, tok), tok), // 创建节点：(A += addend) - 1
                  node->ty); // 创建类型转换节点：(typeof A)((A += addend) - 1)
}

// 后缀表达式的语法规则:
// postfix = "(" type-name ")" "{" initializer-list "}"  // 结构体或联合体初始化
//         = ident "(" func-args ")" postfix-tail*  // 函数调用
//         | primary postfix-tail*  // 基本表达式后面跟随的后缀操作

// 后缀表达式的后缀操作:
// postfix-tail = "[" expr "]"  // 数组访问
//              | "(" func-args ")"  // 函数调用
//              | "." ident  // 结构体成员访问
//              | "->" ident  // 结构体指针成员访问
//              | "++"  // 后缀自增
//              | "--"  // 后缀自减
// 处理后缀表达式
static Node *postfix(Token **rest, Token *tok) {
  if (equal(tok, "(") && is_typename(tok->next)) {
    // 处理复合字面量
    Token *start = tok; // 记录起始标记
    Type *ty = typename(&tok, tok->next); // 解析类型名
    tok = skip(tok, ")"); // 跳过右括号

    if (scope->next == NULL) {
      // 如果当前作用域为全局作用域
      Obj *var = new_anon_gvar(ty); // 创建匿名全局变量
      gvar_initializer(rest, tok, var); // 处理全局变量的初始化
      return new_var_node(var, start); // 返回新建的变量节点
    }

    // 如果当前作用域为局部作用域
    Obj *var = new_lvar("", ty); // 创建匿名局部变量
    Node *lhs = lvar_initializer(rest, tok, var); // 处理局部变量的初始化
    Node *rhs = new_var_node(var, tok); // 创建变量节点
    return new_binary(ND_COMMA, lhs, rhs, start); // 返回逗号表达式节点
  }

  Node *node = primary(&tok, tok); // 解析基本表达式并得到初始节点

  for (;;) {
    if (equal(tok, "(")) {
      // 如果遇到左括号，表示函数调用
      node = funcall(&tok, tok->next, node); // 解析函数调用并更新节点
      continue;
    }

    if (equal(tok, "[")) {
      // 如果遇到左方括号，表示数组访问  // x[y] is short for *(x+y)
      Token *start = tok; // 记录起始标记
      Node *idx = expr(&tok, tok->next); // 解析索引表达式
      tok = skip(tok, "]"); // 跳过右方括号
      node = new_unary(ND_DEREF, new_add(node, idx, start), start); // 创建解引用和加法的一元节点
      continue;
    }

    if (equal(tok, ".")) {
      // 如果遇到点号，表示结构体成员访问
      node = struct_ref(node, tok->next); // 解析结构体成员访问并更新节点
      tok = tok->next->next; // 跳过点号和标识符
      continue;
    }

    if (equal(tok, "->")) {
      // 如果遇到箭头符号，表示结构体指针成员访问  // x->y is short for (*x).y
      node = new_unary(ND_DEREF, node, tok); // 创建解引用的一元节点
      node = struct_ref(node, tok->next); // 解析结构体成员访问并更新节点
      tok = tok->next->next; // 跳过箭头和标识符
      continue;
    }

    if (equal(tok, "++")) {
      // 如果遇到双加号，表示后缀自增
      node = new_inc_dec(node, tok, 1); // 创建后缀自增节点
      tok = tok->next; // 跳过自增符号
      continue;
    }

    if (equal(tok, "--")) {
      // 如果遇到双减号，表示后缀自减
      node = new_inc_dec(node, tok, -1); // 创建后缀自减节点
      tok = tok->next; // 跳过自减符号
      continue;
    }

    *rest = tok; // 更新剩余标记
    return node; // 返回最终节点
  }
}

// 解析函数调用表达式
// funcall = (assign ("," assign)*)? ")"
static Node *funcall(Token **rest, Token *tok, Node *fn) {
  add_type(fn); // 添加类型信息到函数节点

  if (fn->ty->kind != TY_FUNC &&
      (fn->ty->kind != TY_PTR || fn->ty->base->kind != TY_FUNC))
    error_tok(fn->tok, "not a function"); // 函数类型检查，确保是函数或函数指针类型

  Type *ty = (fn->ty->kind == TY_FUNC) ? fn->ty : fn->ty->base; // 获取函数类型
  Type *param_ty = ty->params; // 获取函数参数类型

  Node head = {}; // 创建头节点
  Node *cur = &head; // 当前节点指针

  while (!equal(tok, ")")) {  // 当当前记号不等于 ")" 时循环执行以下代码块
    if (cur != &head)  // 如果当前节点不等于头节点，则表示已经处理了一个参数，需要跳过逗号","，继续处理下一个参数
      tok = skip(tok, ",");

    Node *arg = assign(&tok, tok);  // 调用 assign 函数，将当前记号传递给它，并返回一个新的记号
                                    // assign 函数将使用当前记号构建一个语法树节点，并返回下一个记号
    add_type(arg);  // 将参数的类型信息添加到语法树节点中

    if (!param_ty && !ty->is_variadic)  // 如果没有函数参数类型，且当前函数不是可变参数函数，则表示参数过多，报错
      error_tok(tok, "too many arguments");

    if (param_ty) {  // 如果存在函数参数类型
      if (param_ty->kind != TY_STRUCT && param_ty->kind != TY_UNION)  // 如果参数类型不是结构体或联合体，则将参数进行类型转换，使其匹配参数类型
        arg = new_cast(arg, param_ty);
      param_ty = param_ty->next;  // 将参数类型指针指向下一个参数类型
    } else if (arg->ty->kind == TY_FLOAT) {  // 如果没有函数参数类型，并且当前参数的类型是浮点数，则将浮点数参数提升为双精度类型（double）
      // 如果形参类型被省略（例如在 "..." 中），浮点实参会被提升为双精度类型（double）。
      arg = new_cast(arg, ty_double);
    }

    cur = cur->next = arg;  // 将当前参数节点添加到链表中，并将链表指针指向下一个节点
  }

  if (param_ty)  // 如果存在函数参数类型，但参数数量不足，则报错
    error_tok(tok, "too few arguments");

  *rest = skip(tok, ")");  // 将当前记号跳过，指向下一个记号，即 ")"，表示函数调用的结束

  Node *node = new_unary(ND_FUNCALL, fn, tok);  // 创建一个一元操作符语法树节点，表示函数调用，操作数为函数指针 fn，记号为当前记号 tok
                                                // 这个节点代表函数调用的整体
  node->func_ty = ty;  // 设置函数调用节点的函数类型为 ty
  node->ty = ty->return_ty;  // 设置函数调用节点的类型为函数返回值的类型
  node->args = head.next;  // 设置函数调用节点的参数列表为链表头节点的下一个节点，即第一个参数节点

  // 如果函数返回结构体，则由调用者负责为返回值分配空间。
  if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION)  // 如果函数返回类型是结构体或联合体
    // 则为函数调用节点创建一个局部变量，用于存储返回值
    node->ret_buffer = new_lvar("", node->ty);
  return node;  // 返回函数调用节点作为整个函数调用表达式的结果
}

// generic-selection = "(" assign "," generic-assoc ("," generic-assoc)* ")"
// 泛型选择 = "(" 赋值 "," 泛型相关项 ("," 泛型相关项)* ")"

// generic-assoc = type-name ":" assign
//               | "default" ":" assign
// 泛型相关项 = 类型名 ":" 赋值
//           | "default" ":" 赋值
static Node *generic_selection(Token **rest, Token *tok) {  // 静态函数，返回类型为 Node 指针，接受参数 Token** 和 Token*
  Token *start = tok;  // 保存起始位置的 Token
  tok = skip(tok, "(");  // 跳过左括号

  Node *ctrl = assign(&tok, tok);  // 解析控制表达式，并将结果赋值给 ctrl
  add_type(ctrl);  // 将 ctrl 的类型添加到符号表中

  Type *t1 = ctrl->ty;  // 获取控制表达式的类型，赋值给 t1
  if (t1->kind == TY_FUNC)
    t1 = pointer_to(t1);  // 如果 t1 的类型是函数类型，将其转换为指针类型
  else if (t1->kind == TY_ARRAY)
    t1 = pointer_to(t1->base);  // 如果 t1 的类型是数组类型，将其转换为指向基础类型的指针类型

  Node *ret = NULL;  // 初始化一个指针 ret，用于存储结果

  while (!consume(rest, tok, ")")) {  // 循环直到遇到右括号
    tok = skip(tok, ",");  // 跳过逗号

    if (equal(tok, "default")) {  // 如果当前的 Token 是 "default"
      tok = skip(tok->next, ":");  // 跳过冒号
      Node *node = assign(&tok, tok);  // 解析下一个表达式，并将结果赋值给 node
      if (!ret)
        ret = node;  // 如果 ret 为 NULL，将 node 赋值给 ret
      continue;
    }

    Type *t2 = typename(&tok, tok);  // 解析类型名，并将结果赋值给 t2
    tok = skip(tok, ":");  // 跳过冒号
    Node *node = assign(&tok, tok);  // 解析下一个表达式，并将结果赋值给 node
    if (is_compatible(t1, t2))  // 如果 t1 和 t2 是兼容的类型
      ret = node;  // 将 node 赋值给 ret
  }

  if (!ret)  // 如果 ret 为 NULL
    error_tok(start, "controlling expression type not compatible with any generic association type");
    // 报错，指示控制表达式的类型与任何泛型关联类型都不兼容
  return ret;  // 返回结果
}

// 主要表达式（primary）的语法规则
// primary 可以是以下之一：
// primary = "(" "{" stmt+ "}" )" - 用括号包围的复合语句
//         | "(" expr ")" - 用括号包围的表达式
//         | "sizeof" "(" type-name ")" - sizeof 运算符后跟括号中的类型名
//         | "sizeof" unary - sizeof 运算符后跟单目表达式
//         | "_Alignof" "(" type-name ")" - _Alignof 运算符后跟括号中的类型名
//         | "_Alignof" unary - _Alignof 运算符后跟单目表达式
//         | "_Generic" generic-selection - _Generic 关键字后跟泛型选择表达式
//         | "__builtin_types_compatible_p" "(" type-name, type-name, ")" - __builtin_types_compatible_p 关键字后跟括号中的两个类型名
//         | "__builtin_reg_class" "(" type-name ")" - __builtin_reg_class 关键字后跟括号中的类型名
//         | ident - 标识符（变量名）
//         | str - 字符串字面量
//         | num - 数字字面量
static Node *primary(Token **rest, Token *tok) {
    Token *start = tok;  // 保存起始标记的位置

    if (equal(tok, "(") && equal(tok->next, "{")) {
        // 这是一个GNU语句表达式
        Node *node = new_node(ND_STMT_EXPR, tok);  // 创建一个节点，类型为ND_STMT_EXPR
        node->body = compound_stmt(&tok, tok->next->next)->body;  // 解析复合语句，并将其作为节点的body
        *rest = skip(tok, ")");  // 跳过右括号，更新rest指针
        return node;  // 返回节点
    }

    if (equal(tok, "(")) {
        Node *node = expr(&tok, tok->next);  // 解析表达式
        *rest = skip(tok, ")");  // 跳过右括号，更新rest指针
        return node;  // 返回节点
    }

    if (equal(tok, "sizeof") && equal(tok->next, "(") && is_typename(tok->next->next)) {
        Type *ty = typename(&tok, tok->next->next);  // 解析类型名，返回类型
        *rest = skip(tok, ")");  // 跳过右括号，更新rest指针

        if (ty->kind == TY_VLA) {
            if (ty->vla_size)
                return new_var_node(ty->vla_size, tok);  // 如果有指定大小的VLA，创建一个变量节点并返回

            Node *lhs = compute_vla_size(ty, tok);  // 计算VLA的大小
            Node *rhs = new_var_node(ty->vla_size, tok);  // 创建一个变量节点作为右操作数
            return new_binary(ND_COMMA, lhs, rhs, tok);  // 创建一个逗号表达式节点并返回
        }

        return new_ulong(ty->size, start);  // 返回一个表示类型大小的节点
    }

    if (equal(tok, "sizeof")) {  // 如果当前的tok与"sizeof"相等，则进入条件判断
      Node *node = unary(rest, tok->next);  // 解析sizeof表达式，并返回节点
      add_type(node);  // 添加类型信息到节点
      if (node->ty->kind == TY_VLA)
          return new_var_node(node->ty->vla_size, tok);  // 如果节点的类型是可变长度数组（VLA），返回一个表示VLA大小的变量节点
      return new_ulong(node->ty->size, tok);  // 返回一个表示类型大小的无符号长整型节点
    }

    if (equal(tok, "_Alignof") && equal(tok->next, "(") && is_typename(tok->next->next)) {  // 如果当前的tok与"_Alignof"相等，且下一个tok是"("，并且紧随其后的是一个类型名，则进入条件判断
        Type *ty = typename(&tok, tok->next->next);  // 解析_Alignof表达式中的类型名，并返回类型
        *rest = skip(tok, ")");  // 跳过右括号，更新rest指针
        return new_ulong(ty->align, tok);  // 返回一个表示类型对齐方式的无符号长整型节点
    }

    if (equal(tok, "_Alignof")) {  // 如果当前的tok与"_Alignof"相等，则进入条件判断
        Node *node = unary(rest, tok->next);  // 解析_Alignof表达式，并返回节点
        add_type(node);  // 添加类型信息到节点
        return new_ulong(node->ty->align, tok);  // 返回一个表示类型对齐方式的无符号长整型节点
    }

    if (equal(tok, "_Generic"))  // 如果当前的tok与"_Generic"相等，则进入条件判断
        return generic_selection(rest, tok->next);  // 解析_Generic表达式，并返回节点

    if (equal(tok, "__builtin_types_compatible_p")) {  // 如果当前的tok与"__builtin_types_compatible_p"相等，则进入条件判断
        tok = skip(tok->next, "(");
        Type *t1 = typename(&tok, tok);  // 解析__builtin_types_compatible_p表达式中的两个类型名
        tok = skip(tok, ",");
        Type *t2 = typename(&tok, tok);
        *rest = skip(tok, ")");  // 跳过右括号，更新rest指针
        return new_num(is_compatible(t1, t2), start);  // 返回一个表示两个类型是否兼容的整型节点
    }

    // 如果当前的tok与"__builtin_reg_class"相等，则进入条件判断
    if (equal(tok, "__builtin_reg_class")) {
        // 跳过左括号"("
        tok = skip(tok->next, "(");
        // 解析__builtin_reg_class表达式中的类型名，并返回类型
        Type *ty = typename(&tok, tok);
        // 跳过右括号")"
        *rest = skip(tok, ")");

        // 如果类型是整数或指针类型，返回一个表示寄存器类别为0的整型节点
        if (is_integer(ty) || ty->kind == TY_PTR)
            return new_num(0, start);
        // 如果类型是浮点数类型，返回一个表示寄存器类别为1的整型节点
        if (is_flonum(ty))
            return new_num(1, start);
        // 其他情况，返回一个表示寄存器类别为2的整型节点
        return new_num(2, start);
    }

    // 如果当前的tok与"__builtin_compare_and_swap"相等，则进入条件判断
    if (equal(tok, "__builtin_compare_and_swap")) {
        // 创建一个ND_CAS类型的节点
        Node *node = new_node(ND_CAS, tok);
        // 跳过左括号"("
        tok = skip(tok->next, "(");
        // 解析__builtin_compare_and_swap表达式中的地址部分，并赋值给node->cas_addr
        node->cas_addr = assign(&tok, tok);
        // 跳过逗号","
        tok = skip(tok, ",");
        // 解析__builtin_compare_and_swap表达式中的旧值部分，并赋值给node->cas_old
        node->cas_old = assign(&tok, tok);
        // 跳过逗号","
        tok = skip(tok, ",");
        // 解析__builtin_compare_and_swap表达式中的新值部分，并赋值给node->cas_new
        node->cas_new = assign(&tok, tok);
        // 跳过右括号")"
        *rest = skip(tok, ")");
        // 返回该节点
        return node;
    }

    // 如果当前的tok与"__builtin_atomic_exchange"相等，则进入条件判断
    if (equal(tok, "__builtin_atomic_exchange")) {
        // 创建一个ND_EXCH类型的节点
        Node *node = new_node(ND_EXCH, tok);
        // 跳过左括号"("
        tok = skip(tok->next, "(");
        // 解析__builtin_atomic_exchange表达式中的左值部分，并赋值给node->lhs
        node->lhs = assign(&tok, tok);
        // 跳过逗号","
        tok = skip(tok, ",");
        // 解析__builtin_atomic_exchange表达式中的右值部分，并赋值给node->rhs
        node->rhs = assign(&tok, tok);
        // 跳过右括号")"
        *rest = skip(tok, ")");
        // 返回该节点
        return node;
    }

    // 如果当前的tok是标识符类型
    if (tok->kind == TK_IDENT) {
        // 查找变量或枚举常量的作用域
        VarScope *sc = find_var(tok);
        // 将rest指向下一个token
        *rest = tok->next;

        // 对于"static inline"函数
        if (sc && sc->var && sc->var->is_function) {
            // 如果当前正在解析函数，并且函数存在，则将变量名添加到函数的引用列表中
            if (current_fn)
                strarray_push(&current_fn->refs, sc->var->name);
            // 否则将变量标记为根变量
            else
                sc->var->is_root = true;
        }

        // 如果找到了作用域
        if (sc) {
            // 如果作用域中有变量，则返回一个表示变量的节点
            if (sc->var)
                return new_var_node(sc->var, tok);
            // 如果作用域中有枚举类型，则返回一个表示枚举常量的节点
            if (sc->enum_ty)
                return new_num(sc->enum_val, tok);
        }

        // 如果下一个token是左括号"("
        if (equal(tok->next, "("))
            error_tok(tok, "implicit declaration of a function");
        // 报错，变量未定义
        error_tok(tok, "undefined variable");
    }

    // 如果当前的tok是字符串类型的token
    if (tok->kind == TK_STR) {
        // 创建一个表示字符串字面量的对象
        Obj *var = new_string_literal(tok->str, tok->ty);
        // 将rest指向下一个token
        *rest = tok->next;
        // 返回一个表示变量的节点
        return new_var_node(var, tok);
    }

    // 如果当前的tok是数字类型的token
    if (tok->kind == TK_NUM) {
        Node *node;
        // 如果是浮点数类型
        if (is_flonum(tok->ty)) {
            // 创建一个表示浮点数的节点
            node = new_node(ND_NUM, tok);
            // 设置节点的浮点数值
            node->fval = tok->fval;
        } else {
            // 创建一个表示整数的节点
            node = new_num(tok->val, tok);
        }

        // 设置节点的类型
        node->ty = tok->ty;
        // 将rest指向下一个token
        *rest = tok->next;
        // 返回节点
        return node;
    }

    // 报错，期望一个表达式
    error_tok(tok, "expected an expression");
}

// 解析typedef语句，将基本类型basety解析为typedef定义的类型
static Token *parse_typedef(Token *tok, Type *basety) {
  bool first = true;

  // 循环解析直到遇到分号";"
  while (!consume(&tok, tok, ";")) {
    // 如果不是第一个声明，跳过逗号","
    if (!first)
      tok = skip(tok, ",");
    // 设置first为false，表示已经不是第一个声明
    first = false;

    // 解析声明符号，并返回声明的类型
    Type *ty = declarator(&tok, tok, basety);
    // 如果声明的类型没有名称，则报错
    if (!ty->name)
      error_tok(ty->name_pos, "typedef name omitted");
    // 将typedef定义的类型添加到当前作用域中
    push_scope(get_ident(ty->name))->type_def = ty;
  }
  return tok;
}

// 创建函数参数的本地变量
static void create_param_lvars(Type *param) {
  // 如果还有下一个参数，则递归调用创建参数的本地变量
  if (param) {
    create_param_lvars(param->next);
    // 如果参数没有名称，则报错
    if (!param->name)
      error_tok(param->name_pos, "parameter name omitted");
    // 创建一个新的本地变量，并将其添加到符号表中
    new_lvar(get_ident(param->name), param);
  }
}

// 此函数用于将goto语句或标签作为值与标签进行匹配。
//
// 在解析函数时，我们无法解析goto语句，因为goto语句可能引用出现在函数后面的标签。
// 因此，我们需要在解析整个函数之后进行该操作。
// 解析跳转标签
static void resolve_goto_labels(void) {
  // 遍历跳转语句列表
  for (Node *x = gotos; x; x = x->goto_next) {
    // 遍历标签列表
    for (Node *y = labels; y; y = y->goto_next) {
      // 检查标签是否匹配
      if (!strcmp(x->label, y->label)) {
        // 将跳转语句的唯一标识设置为标签的唯一标识
        x->unique_label = y->unique_label;
        break;
      }
    }

    // 如果没有找到匹配的标签
    if (x->unique_label == NULL)
      // 报错：使用了未声明的标签
      error_tok(x->tok->next, "use of undeclared label");
  }

  // 清空跳转语句列表和标签列表
  gotos = labels = NULL;
}

static Obj *find_func(char *name) {
  // 声明一个指针变量sc并初始化为当前作用域的指针
  Scope *sc = scope;
  // 循环直到找到最外层的作用域
  while (sc->next)
    sc = sc->next;

  // 声明一个指针变量sc2，用于存储在变量作用域中查找到的变量信息
  VarScope *sc2 = hashmap_get(&sc->vars, name);
  // 检查是否找到变量并且变量是函数类型
  if (sc2 && sc2->var && sc2->var->is_function)
    return sc2->var; // 返回找到的函数变量
  return NULL; // 如果未找到匹配的函数，返回空指针
}

static void mark_live(Obj *var) {
  if (!var->is_function || var->is_live)  // 如果变量不是函数类型或者已经被标记为live，则直接返回
    return;
  var->is_live = true;  // 将变量标记为live

  for (int i = 0; i < var->refs.len; i++) {
    Obj *fn = find_func(var->refs.data[i]);  // 查找引用变量对应的函数
    if (fn)
      mark_live(fn);  // 如果找到函数，则递归标记函数为live
  }
}

static Token *function(Token *tok, Type *basety, VarAttr *attr) {
  // 解析声明符并返回类型
  Type *ty = declarator(&tok, tok, basety);
  if (!ty->name)
    error_tok(ty->name_pos, "function name omitted");  // 如果函数名为空，则报错
  char *name_str = get_ident(ty->name);  // 获取函数名字符串

  Obj *fn = find_func(name_str);  // 在符号表中查找函数
  if (fn) {
    // 重新声明
    if (!fn->is_function)
      error_tok(tok, "redeclared as a different kind of symbol");  // 如果已存在符号并且不是函数，则报错
    if (fn->is_definition && equal(tok, "{"))
      error_tok(tok, "redefinition of %s", name_str);  // 如果函数已定义且当前标记为左花括号，则报错
    if (!fn->is_static && attr->is_static)
      error_tok(tok, "static declaration follows a non-static declaration");  // 如果当前标记为静态声明且前一个声明不是静态，则报错
    fn->is_definition = fn->is_definition || equal(tok, "{");  // 设置函数是否定义的标志
  } else {
    fn = new_gvar(name_str, ty);  // 创建新的全局变量对象
    fn->is_function = true;  // 设置对象为函数
    fn->is_definition = equal(tok, "{");  // 设置函数是否定义的标志
    fn->is_static = attr->is_static || (attr->is_inline && !attr->is_extern);  // 设置函数是否为静态的标志
    fn->is_inline = attr->is_inline;  // 设置函数是否为内联的标志
  }

  fn->is_root = !(fn->is_static && fn->is_inline);  // 根据函数的静态和内联属性确定是否为根函数

  if (consume(&tok, tok, ";"))  // 如果遇到分号，则表示函数声明结束，直接返回
    return tok;

  current_fn = fn;  // 设置当前函数为正在处理的函数
  locals = NULL;  // 初始化本地变量列表
  enter_scope();  // 进入新的作用域
  create_param_lvars(ty->params);  // 创建参数的本地变量

  // 用于传递作为结构体/联合体返回值的缓冲区作为隐藏的第一个形参
  Type *rty = ty->return_ty;  // 获取返回值的类型
  if ((rty->kind == TY_STRUCT || rty->kind == TY_UNION) && rty->size > 16)
    new_lvar("", pointer_to(rty));  // 如果返回值是结构体/联合体且大小大于16，则创建一个指向返回类型的指针的本地变量

  fn->params = locals;  // 设置函数的参数列表为本地变量列表

  if (ty->is_variadic)
    fn->va_area = new_lvar("__va_area__", array_of(ty_char, 136));  // 如果函数是可变参数函数，则创建一个大小为136的字符数组的本地变量作为可变参数区域
  fn->alloca_bottom = new_lvar("__alloca_size__", pointer_to(ty_char));  // 创建一个指向字符类型的指针的本地变量作为alloca区域的底部

  tok = skip(tok, "{");  // 跳过左花括号，函数体开始

  // [https://www.sigbus.info/n1570#6.4.2.2p1] "__func__"是自动定义的局部变量，包含当前函数的名称。
  // 将"__func__"作为作用域标识符传递给push_scope函数，并将其变量设置为一个新的字符串字面值，内容为fn->name的字符串表示形式，长度为strlen(fn->name) + 1。
  push_scope("__func__")->var =
      new_string_literal(fn->name, array_of(ty_char, strlen(fn->name) + 1));

  // [GNU] "__FUNCTION__"是 "__func__" 的另一个名称。
  // 将"__FUNCTION__"作为作用域标识符传递给push_scope函数，并将其变量设置为一个新的字符串字面值，内容为fn->name的字符串表示形式，长度为strlen(fn->name) + 1。
  push_scope("__FUNCTION__")->var =
      new_string_literal(fn->name, array_of(ty_char, strlen(fn->name) + 1));

  // 将fn->body设置为通过compound_stmt函数解析得到的复合语句。
  fn->body = compound_stmt(&tok, tok);

  // 将fn->locals设置为locals。
  fn->locals = locals;

  // 离开当前作用域。
  leave_scope();

  // 解析并解决跳转标签。
  resolve_goto_labels();

  // 返回tok。
  return tok;
}

static Token *global_variable(Token *tok, Type *basety, VarAttr *attr) {
  bool first = true;  // 布尔变量first，用于判断是否是第一个变量

  while (!consume(&tok, tok, ";")) {  // 循环直到遇到分号
    if (!first)
      tok = skip(tok, ",");  // 如果不是第一个变量，则跳过逗号
    first = false;

    Type *ty = declarator(&tok, tok, basety);  // 解析声明符并得到变量类型
    if (!ty->name)  // 如果变量名为空
      error_tok(ty->name_pos, "variable name omitted");  // 报错，变量名省略

    Obj *var = new_gvar(get_ident(ty->name), ty);  // 创建新的全局变量
    var->is_definition = !attr->is_extern;  // 如果变量不是extern，则表示为定义
    var->is_static = attr->is_static;  // 设置变量的static属性
    var->is_tls = attr->is_tls;  // 设置变量的线程局部存储属性
    if (attr->align)
      var->align = attr->align;  // 如果有指定对齐方式，则设置变量的对齐方式

    if (equal(tok, "="))
      gvar_initializer(&tok, tok->next, var);  // 如果有等号，则解析全局变量的初始化器
    else if (!attr->is_extern && !attr->is_tls)
      var->is_tentative = true;  // 如果变量既不是extern也不是线程局部存储，则表示为不完全定义的变量
  }
  return tok;  // 返回解析后的Token
}

// 预读取Token并返回true，如果给定的Token是函数定义或声明的起点。
static bool is_function(Token *tok) {
  if (equal(tok, ";"))
    return false;  // 如果当前Token是分号，则不是函数定义或声明

  Type dummy = {};  // 创建一个空的Type结构体dummy
  Type *ty = declarator(&tok, tok, &dummy);  // 解析声明符并得到结果类型ty
  return ty->kind == TY_FUNC;  // 返回ty的kind字段是否为TY_FUNC，判断是否为函数类型
}

// 移除多余的不完全定义。
static void scan_globals(void) {
  Obj head;  // 创建一个头节点
  Obj *cur = &head;  // 创建一个指向头节点的指针cur

  for (Obj *var = globals; var; var = var->next) {  // 遍历全局变量链表
    if (!var->is_tentative) {  // 如果变量不是不完全定义的变量
      cur = cur->next = var;  // 将变量加入到链表中
      continue;  // 继续下一次循环
    }

     // 查找相同标识符的另一个定义。
    Obj *var2 = globals;  // 从头开始查找
    for (; var2; var2 = var2->next) {
      if (var != var2 && var2->is_definition && !strcmp(var->name, var2->name))
        break;  // 找到同名的定义，退出循环
    }

    // 如果存在另一个定义，则不完全定义是多余的。
    if (!var2)
      cur = cur->next = var;  // 将变量加入到链表中
  }

  cur->next = NULL;  // 设置链表末尾的下一个节点为NULL
  globals = head.next;  // 更新全局变量链表的头节点
}

static void declare_builtin_functions(void) {
  Type *ty = func_type(pointer_to(ty_void));  // 创建一个函数类型ty，返回值类型为void指针
  ty->params = copy_type(ty_int);  // 设置函数类型ty的参数类型为int类型
  builtin_alloca = new_gvar("alloca", ty);  // 创建一个全局变量builtin_alloca，名称为"alloca"，类型为ty
  builtin_alloca->is_definition = false;  // 将builtin_alloca的is_definition字段设置为false，表示该变量不是一个定义
}

// program = (typedef | function-definition | global-variable)*
// 程序由 typedef、函数定义或全局变量声明组成
Obj *parse(Token *tok) {
  declare_builtin_functions(); // 声明内置函数
  globals = NULL; // 初始化全局变量链表

  while (tok->kind != TK_EOF) { // 循环直到遇到文件结束标记
    VarAttr attr = {}; // 变量属性结构体，用于保存变量的属性
    Type *basety = declspec(&tok, tok, &attr); // 解析类型声明并返回基本类型

    // Typedef（类型定义）
    if (attr.is_typedef) {
      tok = parse_typedef(tok, basety); // 解析并处理typedef语句
      continue; // 继续下一次循环
    }

    // Function（函数定义）
    if (is_function(tok)) {
      tok = function(tok, basety, &attr); // 解析并处理函数定义
      continue; // 继续下一次循环
    }

    // Global variable（全局变量）
    tok = global_variable(tok, basety, &attr); // 解析并处理全局变量声明
  }

  for (Obj *var = globals; var; var = var->next) // 遍历全局变量链表
    if (var->is_root) // 如果变量是根变量（未被引用的变量）
      mark_live(var); // 标记该变量为活跃变量

  // Remove redundant tentative definitions.
  // 移除多余的试探性定义
  scan_globals(); // 扫描全局变量链表，移除多余的试探性定义
  return globals; // 返回全局变量链表
}