// This 源文件位置 contains a recursive descent parser for C.
//
// Most functions in this 源文件位置 are named after the symbols they are
// supposed to read from an 输入 token list. For example, 语句_缩() is
// responsible for reading a statement from a token list. The 正函数_
// 那么 construct an AST 节点_小 representing a statement.
//
// Each 正函数_ conceptually returns two values, an AST 节点_小 and
// remaining part of the 输入 tokens. Since C doesn't support
// multiple return values, the remaining tokens are returned to the
// caller via a pointer argument.
//
// Input tokens are represented by a linked list. Unlike many recursive
// descent parsers, we don't have the notion of the "输入 token stream".
// Most parsing functions don't change the global state of the parser.
// So it is very easy to lookahead arbitrary number of tokens in this
// parser.

#include "chibicc.h"

// 作用域 for local variables, global variables, typedefs
// or enum constants
typedef struct {
  对象_短 *变量_短;
  类型 *类型_定义_短;
  类型 *枚举_类型_短;
  int 枚举_值_短;
} 变量作用域;

// Represents a block 作用域_小写.
typedef struct 作用域 作用域;
struct 作用域 {
  作用域 *下一个;

  // C has two block scopes; one is for variables/typedefs and
  // the other is for struct/union/enum 标签_短.
  哈希映射 变量_短;
  哈希映射 标签_短;
};

// Variable attributes such as typedef or extern.
typedef struct {
  bool 是否_类型定义;
  bool 是否_静态;
  bool 是否_外部;
  bool 是否_内联;
  bool is_tls;
  int 对齐_短;
} 变量基本属性_短;

// This struct represents a variable 初始化器_小写. Since initializers
// can be nested (e.g. `int x[2][2] = {{1, 2}, {3, 4}}`), this struct
// is a tree 数据 structure.
typedef struct 初始化器 初始化器;
struct 初始化器 {
  初始化器 *下一个;
  类型 *类型_缩;
  牌 *牌_短;
  bool 是否_有弹性;

  // If it's not an aggregate type and has an 初始化器_小写,
  // `表达式_短` has an initialization expression.
  节点 *表达式_短;

  // If it's an 初始化器_小写 for an aggregate type (e.g. array or struct),
  // `孩子们` has initializers for its 孩子们.
  初始化器 **孩子们;

  // Only one 成员_小 can be initialized for a union.
  // `成员_短` is 已用 to clarify which 成员_小 is initialized.
  成员 *成员_短;
};

// For local variable 初始化器_小写.
typedef struct 初始设计_短 初始设计_短;
struct 初始设计_短 {
  初始设计_短 *下一个;
  int 索引_缩;
  成员 *成员_小;
  对象_短 *变量_短;
};

// All local variable instances created during parsing are
// accumulated to this list.
static 对象_短 *本地;

// Likewise, global variables are accumulated to this list.
static 对象_短 *全局的;

static 作用域 *作用域_小写 = &(作用域){};

// Points to the 正函数_ object the parser is currently parsing.
static 对象_短 *当前_函;

// Lists of all goto statements and 标号的 in the curent 正函数_.
static 节点 *跳转的;
static 节点 *标号的;

// Current "goto" and "continue" jump targets.
static char *断_标号;
static char *继续_标号;

// Points to a 节点_小 representing a switch if we are parsing
// a switch statement. Otherwise, NULL.
static 节点 *当前_切换;

static 对象_短 *内建_分配a;

static bool 是否_类型名(牌 *牌_短);
static 类型 *声明规格(牌 **rest, 牌 *牌_短, 变量基本属性_短 *attr);
static 类型 *类型名(牌 **rest, 牌 *牌_短);
static 类型 *枚举_说明符(牌 **rest, 牌 *牌_短);
static 类型 *类型的_说明符(牌 **rest, 牌 *牌_短);
static 类型 *类型_字尾(牌 **rest, 牌 *牌_短, 类型 *类型_缩);
static 类型 *声明符(牌 **rest, 牌 *牌_短, 类型 *类型_缩);
static 节点 *正声明(牌 **rest, 牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr);
static void 数组_初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始, int i);
static void 结构_初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始, 成员 *成员_短);
static void 初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始);
static 初始化器 *初始化器_小写(牌 **rest, 牌 *牌_短, 类型 *类型_缩, 类型 **new_ty);
static 节点 *本地变量_初始化器(牌 **rest, 牌 *牌_短, 对象_短 *变量_短);
static void 全局变量_初始化器(牌 **rest, 牌 *牌_短, 对象_短 *变量_短);
static 节点 *复合的_语句(牌 **rest, 牌 *牌_短);
static 节点 *语句_缩(牌 **rest, 牌 *牌_短);
static 节点 *表达式_语句_缩(牌 **rest, 牌 *牌_短);
static 节点 *表达式_短(牌 **rest, 牌 *牌_短);
static int64_t 求值_短(节点 *节点_小);
static int64_t 求值_短2(节点 *节点_小, char ***标号);
static int64_t 求值_右值(节点 *节点_小, char ***标号);
static bool 是否_常量_表达式(节点 *节点_小);
static 节点 *赋值_(牌 **rest, 牌 *牌_短);
static 节点 *逻辑或_(牌 **rest, 牌 *牌_短);
static double 求值_双精度(节点 *节点_小);
static 节点 *有条件的(牌 **rest, 牌 *牌_短);
static 节点 *逻辑与_(牌 **rest, 牌 *牌_短);
static 节点 *位或_(牌 **rest, 牌 *牌_短);
static 节点 *位异或_(牌 **rest, 牌 *牌_短);
static 节点 *位与_(牌 **rest, 牌 *牌_短);
static 节点 *相等(牌 **rest, 牌 *牌_短);
static 节点 *有关系的(牌 **rest, 牌 *牌_短);
static 节点 *移位(牌 **rest, 牌 *牌_短);
static 节点 *加法_(牌 **rest, 牌 *牌_短);
static 节点 *新_加法(节点 *左手边, 节点 *右手边, 牌 *牌_短);
static 节点 *新_减法(节点 *左手边, 节点 *右手边, 牌 *牌_短);
static 节点 *乘法_(牌 **rest, 牌 *牌_短);
static 节点 *类型转换_(牌 **rest, 牌 *牌_短);
static 成员 *取_结构_成员(类型 *类型_缩, 牌 *牌_短);
static 类型 *结构_声明(牌 **rest, 牌 *牌_短);
static 类型 *联合_声明(牌 **rest, 牌 *牌_短);
static 节点 *后缀_(牌 **rest, 牌 *牌_短);
static 节点 *函数调用_(牌 **rest, 牌 *牌_短, 节点 *节点_小);
static 节点 *一元_(牌 **rest, 牌 *牌_短);
static 节点 *最初的(牌 **rest, 牌 *牌_短);
static 牌 *解析_类型定义(牌 *牌_短, 类型 *基本类型);
static bool 是否_函数(牌 *牌_短);
static 牌 *正函数_(牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr);
static 牌 *全局_变量_(牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr);

static int 对齐_下(int n, int 对齐_短) {
  return 对齐_到(n - 对齐_短 + 1, 对齐_短);
}

static void 进入_作用域(void) {
  作用域 *sc = calloc(1, sizeof(作用域));
  sc->下一个 = 作用域_小写;
  作用域_小写 = sc;
}

static void 离开_作用域(void) {
  作用域_小写 = 作用域_小写->下一个;
}

// Find a variable by 名字.
static 变量作用域 *找_变量_短(牌 *牌_短) {
  for (作用域 *sc = 作用域_小写; sc; sc = sc->下一个) {
    变量作用域 *sc2 = 哈希映射_取2(&sc->变量_短, 牌_短->位置_短, 牌_短->长度_短);
    if (sc2)
      return sc2;
  }
  return NULL;
}

static 类型 *找_标签(牌 *牌_短) {
  for (作用域 *sc = 作用域_小写; sc; sc = sc->下一个) {
    类型 *类型_缩 = 哈希映射_取2(&sc->标签_短, 牌_短->位置_短, 牌_短->长度_短);
    if (类型_缩)
      return 类型_缩;
  }
  return NULL;
}

static 节点 *新_节点_(节点种类 种类, 牌 *牌_短) {
  节点 *节点_小 = calloc(1, sizeof(节点));
  节点_小->种类 = 种类;
  节点_小->牌_短 = 牌_短;
  return 节点_小;
}

static 节点 *新_二元(节点种类 种类, 节点 *左手边, 节点 *右手边, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(种类, 牌_短);
  节点_小->左手边 = 左手边;
  节点_小->右手边 = 右手边;
  return 节点_小;
}

static 节点 *新_一元(节点种类 种类, 节点 *表达式_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(种类, 牌_短);
  节点_小->左手边 = 表达式_短;
  return 节点_小;
}

static 节点 *新_数号_短(int64_t 值_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_整数, 牌_短);
  节点_小->值_短 = 值_短;
  return 节点_小;
}

static 节点 *新_长的_(int64_t 值_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_整数, 牌_短);
  节点_小->值_短 = 值_短;
  节点_小->类型_缩 = 类型_长的_小;
  return 节点_小;
}

static 节点 *新_无符号长的(long 值_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_整数, 牌_短);
  节点_小->值_短 = 值_短;
  节点_小->类型_缩 = 类型_无符号长的;
  return 节点_小;
}

static 节点 *新_变量_节点(对象_短 *变量_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_变量, 牌_短);
  节点_小->变量_短 = 变量_短;
  return 节点_小;
}

static 节点 *新_变长数组_指针(对象_短 *变量_短, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_变长数组_指针, 牌_短);
  节点_小->变量_短 = 变量_短;
  return 节点_小;
}

节点 *新_类型转换(节点 *表达式_短, 类型 *类型_缩) {
  添加_类型_小(表达式_短);

  节点 *节点_小 = calloc(1, sizeof(节点));
  节点_小->种类 = 抽象节点_类型转换;
  节点_小->牌_短 = 表达式_短->牌_短;
  节点_小->左手边 = 表达式_短;
  节点_小->类型_缩 = 复制_类型(类型_缩);
  return 节点_小;
}

static 变量作用域 *推_作用域(char *名字) {
  变量作用域 *sc = calloc(1, sizeof(变量作用域));
  哈希映射_放置(&作用域_小写->变量_短, 名字, sc);
  return sc;
}

static 初始化器 *新_初始化器_(类型 *类型_缩, bool 是否_有弹性) {
  初始化器 *初始 = calloc(1, sizeof(初始化器));
  初始->类型_缩 = 类型_缩;

  if (类型_缩->种类 == 类型_数组) {
    if (是否_有弹性 && 类型_缩->大小 < 0) {
      初始->是否_有弹性 = true;
      return 初始;
    }

    初始->孩子们 = calloc(类型_缩->数组_长度, sizeof(初始化器 *));
    for (int i = 0; i < 类型_缩->数组_长度; i++)
      初始->孩子们[i] = 新_初始化器_(类型_缩->基础, false);
    return 初始;
  }

  if (类型_缩->种类 == 类型_结构 || 类型_缩->种类 == 类型_联合) {
    // Count the number of struct 成员_复数.
    int 长度_短 = 0;
    for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个)
      长度_短++;

    初始->孩子们 = calloc(长度_短, sizeof(初始化器 *));

    for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
      if (是否_有弹性 && 类型_缩->是否_有弹性 && !成员_短->下一个) {
        初始化器 *child = calloc(1, sizeof(初始化器));
        child->类型_缩 = 成员_短->类型_缩;
        child->是否_有弹性 = true;
        初始->孩子们[成员_短->索引_缩] = child;
      } else {
        初始->孩子们[成员_短->索引_缩] = 新_初始化器_(成员_短->类型_缩, false);
      }
    }
    return 初始;
  }

  return 初始;
}

static 对象_短 *新_变量_短(char *名字, 类型 *类型_缩) {
  对象_短 *变量_短 = calloc(1, sizeof(对象_短));
  变量_短->名字 = 名字;
  变量_短->类型_缩 = 类型_缩;
  变量_短->对齐_短 = 类型_缩->对齐_短;
  推_作用域(名字)->变量_短 = 变量_短;
  return 变量_短;
}

static 对象_短 *新_本地变量_短(char *名字, 类型 *类型_缩) {
  对象_短 *变量_短 = 新_变量_短(名字, 类型_缩);
  变量_短->是否_本地 = true;
  变量_短->下一个 = 本地;
  本地 = 变量_短;
  return 变量_短;
}

static 对象_短 *新_全局变量_短(char *名字, 类型 *类型_缩) {
  对象_短 *变量_短 = 新_变量_短(名字, 类型_缩);
  变量_短->下一个 = 全局的;
  变量_短->是否_静态 = true;
  变量_短->是否_定义了 = true;
  全局的 = 变量_短;
  return 变量_短;
}

static char *新_独特性_名字(void) {
  static int id = 0;
  return 格式化(".L..%d", id++);
}

static 对象_短 *新_匿名_全局变量_短(类型 *类型_缩) {
  return 新_全局变量_短(新_独特性_名字(), 类型_缩);
}

static 对象_短 *新_字符串_字面(char *p, 类型 *类型_缩) {
  对象_短 *变量_短 = 新_匿名_全局变量_短(类型_缩);
  变量_短->初始_数据 = p;
  return 变量_短;
}

static char *取_标识_短(牌 *牌_短) {
  if (牌_短->种类 != 牌_缩_标识符)
    错误_牌_短(牌_短, "expected an identifier");
  return strndup(牌_短->位置_短, 牌_短->长度_短);
}

static 类型 *找_类型定义_(牌 *牌_短) {
  if (牌_短->种类 == 牌_缩_标识符) {
    变量作用域 *sc = 找_变量_短(牌_短);
    if (sc)
      return sc->类型_定义_短;
  }
  return NULL;
}

static void 推_标签_作用域(牌 *牌_短, 类型 *类型_缩) {
  哈希映射_放置2(&作用域_小写->标签_短, 牌_短->位置_短, 牌_短->长度_短, 类型_缩);
}

// 声明规格 = ("void" | "_Bool" | "char" | "short" | "int" | "long"
//             | "typedef" | "static" | "extern" | "inline"
//             | "_Thread_local" | "__thread"
//             | "signed" | "unsigned"
//             | struct-decl | union-decl | typedef-名字
//             | enum-specifier | typeof-specifier
//             | "const" | "volatile" | "auto" | "register" | "restrict"
//             | "__restrict" | "__restrict__" | "_Noreturn")+
//
// The order of typenames in a type-specifier doesn't matter. For
// example, `int long static` means the same as `static long int`.
// That can also be written as `static long` because you can omit
// `int` if `long` or `short` are specified. However, something like
// `char int` is not a valid type specifier. We have to accept only a
// limited combinations of the typenames.
//
// In this 正函数_, we count the number of occurrences of each 类型名
// while keeping the "current" type object that the typenames up
// until that point represent. When we reach a non-类型名 token,
// we returns the current type object.
static 类型 *声明规格(牌 **rest, 牌 *牌_短, 变量基本属性_短 *attr) {
  // We use a single integer as counters for all typenames.
  // For example, bits 0 and 1 represents how many times we saw the
  // keyword "void" so far. With this, we can use a switch statement
  // as you can see below.
  enum {
    空的_大写     = 1 << 0,
    布尔_大写     = 1 << 2,
    字符_大写     = 1 << 4,
    短的_大写    = 1 << 6,
    整型_大写      = 1 << 8,
    长的_大写     = 1 << 10,
    浮点_大写    = 1 << 12,
    双浮_大写   = 1 << 14,
    其他的_大写    = 1 << 16,
    符号的_大写   = 1 << 17,
    无符号的_大写 = 1 << 18,
  };

  类型 *类型_缩 = 类型_整型_小;
  int 计数器_ = 0;
  bool 是否_原子 = false;

  while (是否_类型名(牌_短)) {
    // Handle storage class specifiers.
    if (等于(牌_短, "typedef") || 等于(牌_短, "static") || 等于(牌_短, "extern") ||
        等于(牌_短, "inline") || 等于(牌_短, "_Thread_local") || 等于(牌_短, "__thread")) {
      if (!attr)
        错误_牌_短(牌_短, "storage class specifier is not allowed in this context");

      if (等于(牌_短, "typedef"))
        attr->是否_类型定义 = true;
      else if (等于(牌_短, "static"))
        attr->是否_静态 = true;
      else if (等于(牌_短, "extern"))
        attr->是否_外部 = true;
      else if (等于(牌_短, "inline"))
        attr->是否_内联 = true;
      else
        attr->is_tls = true;

      if (attr->是否_类型定义 &&
          attr->是否_静态 + attr->是否_外部 + attr->是否_内联 + attr->is_tls > 1)
        错误_牌_短(牌_短, "typedef may not be 已用 together with static,"
                  " extern, inline, __thread or _Thread_local");
      牌_短 = 牌_短->下一个;
      continue;
    }

    // These keywords are recognized but ignored.
    if (消耗(&牌_短, 牌_短, "const") || 消耗(&牌_短, 牌_短, "volatile") ||
        消耗(&牌_短, 牌_短, "auto") || 消耗(&牌_短, 牌_短, "register") ||
        消耗(&牌_短, 牌_短, "restrict") || 消耗(&牌_短, 牌_短, "__restrict") ||
        消耗(&牌_短, 牌_短, "__restrict__") || 消耗(&牌_短, 牌_短, "_Noreturn"))
      continue;

    if (等于(牌_短, "_Atomic")) {
      牌_短 = 牌_短->下一个;
      if (等于(牌_短 , "(")) {
        类型_缩 = 类型名(&牌_短, 牌_短->下一个);
        牌_短 = 跳过(牌_短, ")");
      }
      是否_原子 = true;
      continue;
    }

    if (等于(牌_短, "_Alignas")) {
      if (!attr)
        错误_牌_短(牌_短, "_Alignas is not allowed in this context");
      牌_短 = 跳过(牌_短->下一个, "(");

      if (是否_类型名(牌_短))
        attr->对齐_短 = 类型名(&牌_短, 牌_短)->对齐_短;
      else
        attr->对齐_短 = 常量_表达式(&牌_短, 牌_短);
      牌_短 = 跳过(牌_短, ")");
      continue;
    }

    // Handle user-defined types.
    类型 *ty2 = 找_类型定义_(牌_短);
    if (等于(牌_短, "struct") || 等于(牌_短, "union") || 等于(牌_短, "enum") ||
        等于(牌_短, "typeof") || ty2) {
      if (计数器_)
        break;

      if (等于(牌_短, "struct")) {
        类型_缩 = 结构_声明(&牌_短, 牌_短->下一个);
      } else if (等于(牌_短, "union")) {
        类型_缩 = 联合_声明(&牌_短, 牌_短->下一个);
      } else if (等于(牌_短, "enum")) {
        类型_缩 = 枚举_说明符(&牌_短, 牌_短->下一个);
      } else if (等于(牌_短, "typeof")) {
        类型_缩 = 类型的_说明符(&牌_短, 牌_短->下一个);
      } else {
        类型_缩 = ty2;
        牌_短 = 牌_短->下一个;
      }

      计数器_ += 其他的_大写;
      continue;
    }

    // Handle built-in types.
    if (等于(牌_短, "void"))
      计数器_ += 空的_大写;
    else if (等于(牌_短, "_Bool"))
      计数器_ += 布尔_大写;
    else if (等于(牌_短, "char"))
      计数器_ += 字符_大写;
    else if (等于(牌_短, "short"))
      计数器_ += 短的_大写;
    else if (等于(牌_短, "int"))
      计数器_ += 整型_大写;
    else if (等于(牌_短, "long"))
      计数器_ += 长的_大写;
    else if (等于(牌_短, "float"))
      计数器_ += 浮点_大写;
    else if (等于(牌_短, "double"))
      计数器_ += 双浮_大写;
    else if (等于(牌_短, "signed"))
      计数器_ |= 符号的_大写;
    else if (等于(牌_短, "unsigned"))
      计数器_ |= 无符号的_大写;
    else
      不可达();

    switch (计数器_) {
    case 空的_大写:
      类型_缩 = 类型_空的_小;
      break;
    case 布尔_大写:
      类型_缩 = 类型_布尔_小;
      break;
    case 字符_大写:
    case 符号的_大写 + 字符_大写:
      类型_缩 = 类型_字符_小;
      break;
    case 无符号的_大写 + 字符_大写:
      类型_缩 = 类型_无符号字符;
      break;
    case 短的_大写:
    case 短的_大写 + 整型_大写:
    case 符号的_大写 + 短的_大写:
    case 符号的_大写 + 短的_大写 + 整型_大写:
      类型_缩 = 类型_短的_小;
      break;
    case 无符号的_大写 + 短的_大写:
    case 无符号的_大写 + 短的_大写 + 整型_大写:
      类型_缩 = 类型_无符号短的;
      break;
    case 整型_大写:
    case 符号的_大写:
    case 符号的_大写 + 整型_大写:
      类型_缩 = 类型_整型_小;
      break;
    case 无符号的_大写:
    case 无符号的_大写 + 整型_大写:
      类型_缩 = 类型_无符号整型;
      break;
    case 长的_大写:
    case 长的_大写 + 整型_大写:
    case 长的_大写 + 长的_大写:
    case 长的_大写 + 长的_大写 + 整型_大写:
    case 符号的_大写 + 长的_大写:
    case 符号的_大写 + 长的_大写 + 整型_大写:
    case 符号的_大写 + 长的_大写 + 长的_大写:
    case 符号的_大写 + 长的_大写 + 长的_大写 + 整型_大写:
      类型_缩 = 类型_长的_小;
      break;
    case 无符号的_大写 + 长的_大写:
    case 无符号的_大写 + 长的_大写 + 整型_大写:
    case 无符号的_大写 + 长的_大写 + 长的_大写:
    case 无符号的_大写 + 长的_大写 + 长的_大写 + 整型_大写:
      类型_缩 = 类型_无符号长的;
      break;
    case 浮点_大写:
      类型_缩 = 类型_浮点_小;
      break;
    case 双浮_大写:
      类型_缩 = 类型_双浮_小;
      break;
    case 长的_大写 + 双浮_大写:
      类型_缩 = 类型_长双浮_小;
      break;
    default:
      错误_牌_短(牌_短, "invalid type");
    }

    牌_短 = 牌_短->下一个;
  }

  if (是否_原子) {
    类型_缩 = 复制_类型(类型_缩);
    类型_缩->是否_原子 = true;
  }

  *rest = 牌_短;
  return 类型_缩;
}

// func-形参_短 = ("void" | param ("," param)* ("," "...")?)? ")"
// param       = 声明规格 声明符
static 类型 *函数_形参_(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  if (等于(牌_短, "void") && 等于(牌_短->下一个, ")")) {
    *rest = 牌_短->下一个->下一个;
    return 函数_类型_小(类型_缩);
  }

  类型 头部 = {};
  类型 *当前_小 = &头部;
  bool 是否_可变参数 = false;

  while (!等于(牌_短, ")")) {
    if (当前_小 != &头部)
      牌_短 = 跳过(牌_短, ",");

    if (等于(牌_短, "...")) {
      是否_可变参数 = true;
      牌_短 = 牌_短->下一个;
      跳过(牌_短, ")");
      break;
    }

    类型 *ty2 = 声明规格(&牌_短, 牌_短, NULL);
    ty2 = 声明符(&牌_短, 牌_短, ty2);

    牌 *名字 = ty2->名字;

    if (ty2->种类 == 类型_数组) {
      // "array of T" is converted to "pointer to T" only in the parameter
      // context. For example, *实参值[] is converted to **实参值 by this.
      ty2 = 指针_到(ty2->基础);
      ty2->名字 = 名字;
    } else if (ty2->种类 == 类型_函数) {
      // Likewise, a 正函数_ is converted to a pointer to a 正函数_
      // only in the parameter context.
      ty2 = 指针_到(ty2);
      ty2->名字 = 名字;
    }

    当前_小 = 当前_小->下一个 = 复制_类型(ty2);
  }

  if (当前_小 == &头部)
    是否_可变参数 = true;

  类型_缩 = 函数_类型_小(类型_缩);
  类型_缩->形参_短 = 头部.下一个;
  类型_缩->是否_可变参数 = 是否_可变参数;
  *rest = 牌_短->下一个;
  return 类型_缩;
}

// array-dimensions = ("static" | "restrict")* const-表达式_短? "]" type-suffix
static 类型 *数组_维度(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  while (等于(牌_短, "static") || 等于(牌_短, "restrict"))
    牌_短 = 牌_短->下一个;

  if (等于(牌_短, "]")) {
    类型_缩 = 类型_字尾(rest, 牌_短->下一个, 类型_缩);
    return 数组_阵列(类型_缩, -1);
  }

  节点 *表达式_短 = 有条件的(&牌_短, 牌_短);
  牌_短 = 跳过(牌_短, "]");
  类型_缩 = 类型_字尾(rest, 牌_短, 类型_缩);

  if (类型_缩->种类 == 类型_变长数组 || !是否_常量_表达式(表达式_短))
    return 变长数组_阵列(类型_缩, 表达式_短);
  return 数组_阵列(类型_缩, 求值_短(表达式_短));
}

// type-suffix = "(" func-形参_短
//             | "[" array-dimensions
//             | ε
static 类型 *类型_字尾(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  if (等于(牌_短, "("))
    return 函数_形参_(rest, 牌_短->下一个, 类型_缩);

  if (等于(牌_短, "["))
    return 数组_维度(rest, 牌_短->下一个, 类型_缩);

  *rest = 牌_短;
  return 类型_缩;
}

// 正指针_ = ("*" ("const" | "volatile" | "restrict")*)*
static 类型 *正指针_(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  while (消耗(&牌_短, 牌_短, "*")) {
    类型_缩 = 指针_到(类型_缩);
    while (等于(牌_短, "const") || 等于(牌_短, "volatile") || 等于(牌_短, "restrict") ||
           等于(牌_短, "__restrict") || 等于(牌_短, "__restrict__"))
      牌_短 = 牌_短->下一个;
  }
  *rest = 牌_短;
  return 类型_缩;
}

// 声明符 = 正指针_ ("(" ident ")" | "(" 声明符 ")" | ident) type-suffix
static 类型 *声明符(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  类型_缩 = 正指针_(&牌_短, 牌_短, 类型_缩);

  if (等于(牌_短, "(")) {
    牌 *开启 = 牌_短;
    类型 假人 = {};
    声明符(&牌_短, 开启->下一个, &假人);
    牌_短 = 跳过(牌_短, ")");
    类型_缩 = 类型_字尾(rest, 牌_短, 类型_缩);
    return 声明符(&牌_短, 开启->下一个, 类型_缩);
  }

  牌 *名字 = NULL;
  牌 *名字_位置 = 牌_短;

  if (牌_短->种类 == 牌_缩_标识符) {
    名字 = 牌_短;
    牌_短 = 牌_短->下一个;
  }

  类型_缩 = 类型_字尾(rest, 牌_短, 类型_缩);
  类型_缩->名字 = 名字;
  类型_缩->名字_位置 = 名字_位置;
  return 类型_缩;
}

// abstract-声明符 = 正指针_ ("(" abstract-声明符 ")")? type-suffix
static 类型 *抽象_声明符(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  类型_缩 = 正指针_(&牌_短, 牌_短, 类型_缩);

  if (等于(牌_短, "(")) {
    牌 *开启 = 牌_短;
    类型 假人 = {};
    抽象_声明符(&牌_短, 开启->下一个, &假人);
    牌_短 = 跳过(牌_短, ")");
    类型_缩 = 类型_字尾(rest, 牌_短, 类型_缩);
    return 抽象_声明符(&牌_短, 开启->下一个, 类型_缩);
  }

  return 类型_字尾(rest, 牌_短, 类型_缩);
}

// type-名字 = 声明规格 abstract-声明符
static 类型 *类型名(牌 **rest, 牌 *牌_短) {
  类型 *类型_缩 = 声明规格(&牌_短, 牌_短, NULL);
  return 抽象_声明符(rest, 牌_短, 类型_缩);
}

static bool 是否_终(牌 *牌_短) {
  return 等于(牌_短, "}") || (等于(牌_短, ",") && 等于(牌_短->下一个, "}"));
}

static bool 消耗_终(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, "}")) {
    *rest = 牌_短->下一个;
    return true;
  }

  if (等于(牌_短, ",") && 等于(牌_短->下一个, "}")) {
    *rest = 牌_短->下一个->下一个;
    return true;
  }

  return false;
}

// enum-specifier = ident? "{" enum-list? "}"
//                | ident ("{" enum-list? "}")?
//
// enum-list      = ident ("=" num)? ("," ident ("=" num)?)* ","?
static 类型 *枚举_说明符(牌 **rest, 牌 *牌_短) {
  类型 *类型_缩 = 枚举_类型_小();

  // Read a struct tag.
  牌 *tag = NULL;
  if (牌_短->种类 == 牌_缩_标识符) {
    tag = 牌_短;
    牌_短 = 牌_短->下一个;
  }

  if (tag && !等于(牌_短, "{")) {
    类型 *类型_缩 = 找_标签(tag);
    if (!类型_缩)
      错误_牌_短(tag, "unknown enum type");
    if (类型_缩->种类 != 类型_枚举)
      错误_牌_短(tag, "not an enum tag");
    *rest = 牌_短;
    return 类型_缩;
  }

  牌_短 = 跳过(牌_短, "{");

  // Read an enum-list.
  int i = 0;
  int 值_短 = 0;
  while (!消耗_终(rest, 牌_短)) {
    if (i++ > 0)
      牌_短 = 跳过(牌_短, ",");

    char *名字 = 取_标识_短(牌_短);
    牌_短 = 牌_短->下一个;

    if (等于(牌_短, "="))
      值_短 = 常量_表达式(&牌_短, 牌_短->下一个);

    变量作用域 *sc = 推_作用域(名字);
    sc->枚举_类型_短 = 类型_缩;
    sc->枚举_值_短 = 值_短++;
  }

  if (tag)
    推_标签_作用域(tag, 类型_缩);
  return 类型_缩;
}

// typeof-specifier = "(" (表达式_短 | 类型名) ")"
static 类型 *类型的_说明符(牌 **rest, 牌 *牌_短) {
  牌_短 = 跳过(牌_短, "(");

  类型 *类型_缩;
  if (是否_类型名(牌_短)) {
    类型_缩 = 类型名(&牌_短, 牌_短);
  } else {
    节点 *节点_小 = 表达式_短(&牌_短, 牌_短);
    添加_类型_小(节点_小);
    类型_缩 = 节点_小->类型_缩;
  }
  *rest = 跳过(牌_短, ")");
  return 类型_缩;
}

// Generate code for computing a VLA 大小.
static 节点 *计算_变长数组_大小(类型 *类型_缩, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_空指针_表达式_短, 牌_短);
  if (类型_缩->基础)
    节点_小 = 新_二元(抽象节点_逗号, 节点_小, 计算_变长数组_大小(类型_缩->基础, 牌_短), 牌_短);

  if (类型_缩->种类 != 类型_变长数组)
    return 节点_小;

  节点 *base_sz;
  if (类型_缩->基础->种类 == 类型_变长数组)
    base_sz = 新_变量_节点(类型_缩->基础->变长数组_大小, 牌_短);
  else
    base_sz = 新_数号_短(类型_缩->基础->大小, 牌_短);

  类型_缩->变长数组_大小 = 新_本地变量_短("", 类型_无符号长的);
  节点 *表达式_短 = 新_二元(抽象节点_赋值, 新_变量_节点(类型_缩->变长数组_大小, 牌_短),
                          新_二元(抽象节点_乘, 类型_缩->变长数组_长度, base_sz, 牌_短),
                          牌_短);
  return 新_二元(抽象节点_逗号, 节点_小, 表达式_短, 牌_短);
}

static 节点 *新_分配a_(节点 *sz) {
  节点 *节点_小 = 新_一元(抽象节点_函数调用, 新_变量_节点(内建_分配a, sz->牌_短), sz->牌_短);
  节点_小->函数_类型 = 内建_分配a->类型_缩;
  节点_小->类型_缩 = 内建_分配a->类型_缩->返回_类型;
  节点_小->实参_短 = sz;
  添加_类型_小(sz);
  return 节点_小;
}

// 正声明 = 声明规格 (声明符 ("=" 表达式_短)? ("," 声明符 ("=" 表达式_短)?)*)? ";"
static 节点 *正声明(牌 **rest, 牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr) {
  节点 头部 = {};
  节点 *当前_小 = &头部;
  int i = 0;

  while (!等于(牌_短, ";")) {
    if (i++ > 0)
      牌_短 = 跳过(牌_短, ",");

    类型 *类型_缩 = 声明符(&牌_短, 牌_短, 基本类型);
    if (类型_缩->种类 == 类型_空的)
      错误_牌_短(牌_短, "variable declared void");
    if (!类型_缩->名字)
      错误_牌_短(类型_缩->名字_位置, "variable 名字 omitted");

    if (attr && attr->是否_静态) {
      // static local variable
      对象_短 *变量_短 = 新_匿名_全局变量_短(类型_缩);
      推_作用域(取_标识_短(类型_缩->名字))->变量_短 = 变量_短;
      if (等于(牌_短, "="))
        全局变量_初始化器(&牌_短, 牌_短->下一个, 变量_短);
      continue;
    }

    // Generate code for computing a VLA 大小. We need to do this
    // even if 类型_缩 is not VLA because 类型_缩 may be a pointer to VLA
    // (e.g. int (*foo)[n][m] where n and m are variables.)
    当前_小 = 当前_小->下一个 = 新_一元(抽象节点_表达式_语句, 计算_变长数组_大小(类型_缩, 牌_短), 牌_短);

    if (类型_缩->种类 == 类型_变长数组) {
      if (等于(牌_短, "="))
        错误_牌_短(牌_短, "variable-sized object may not be initialized");

      // Variable length arrays (VLAs) are translated to alloca() calls.
      // For example, `int x[n+2]` is translated to `tmp = n + 2,
      // x = alloca(tmp)`.
      对象_短 *变量_短 = 新_本地变量_短(取_标识_短(类型_缩->名字), 类型_缩);
      牌 *牌_短 = 类型_缩->名字;
      节点 *表达式_短 = 新_二元(抽象节点_赋值, 新_变长数组_指针(变量_短, 牌_短),
                              新_分配a_(新_变量_节点(类型_缩->变长数组_大小, 牌_短)),
                              牌_短);

      当前_小 = 当前_小->下一个 = 新_一元(抽象节点_表达式_语句, 表达式_短, 牌_短);
      continue;
    }

    对象_短 *变量_短 = 新_本地变量_短(取_标识_短(类型_缩->名字), 类型_缩);
    if (attr && attr->对齐_短)
      变量_短->对齐_短 = attr->对齐_短;

    if (等于(牌_短, "=")) {
      节点 *表达式_短 = 本地变量_初始化器(&牌_短, 牌_短->下一个, 变量_短);
      当前_小 = 当前_小->下一个 = 新_一元(抽象节点_表达式_语句, 表达式_短, 牌_短);
    }

    if (变量_短->类型_缩->大小 < 0)
      错误_牌_短(类型_缩->名字, "variable has incomplete type");
    if (变量_短->类型_缩->种类 == 类型_空的)
      错误_牌_短(类型_缩->名字, "variable declared void");
  }

  节点 *节点_小 = 新_节点_(抽象节点_块, 牌_短);
  节点_小->体 = 头部.下一个;
  *rest = 牌_短->下一个;
  return 节点_小;
}

static 牌 *跳过_过量_元素(牌 *牌_短) {
  if (等于(牌_短, "{")) {
    牌_短 = 跳过_过量_元素(牌_短->下一个);
    return 跳过(牌_短, "}");
  }

  赋值_(&牌_短, 牌_短);
  return 牌_短;
}

// string-初始化器_小写 = string-literal
static void 字符串_初始化器_(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  if (初始->是否_有弹性)
    *初始 = *新_初始化器_(数组_阵列(初始->类型_缩->基础, 牌_短->类型_缩->数组_长度), false);

  int 长度_短 = 最小_缩(初始->类型_缩->数组_长度, 牌_短->类型_缩->数组_长度);

  switch (初始->类型_缩->基础->大小) {
  case 1: {
    char *串_短 = 牌_短->串_短;
    for (int i = 0; i < 长度_短; i++)
      初始->孩子们[i]->表达式_短 = 新_数号_短(串_短[i], 牌_短);
    break;
  }
  case 2: {
    uint16_t *串_短 = (uint16_t *)牌_短->串_短;
    for (int i = 0; i < 长度_短; i++)
      初始->孩子们[i]->表达式_短 = 新_数号_短(串_短[i], 牌_短);
    break;
  }
  case 4: {
    uint32_t *串_短 = (uint32_t *)牌_短->串_短;
    for (int i = 0; i < 长度_短; i++)
      初始->孩子们[i]->表达式_短 = 新_数号_短(串_短[i], 牌_短);
    break;
  }
  default:
    不可达();
  }

  *rest = 牌_短->下一个;
}

// array-designator = "[" const-表达式_短 "]"
//
// C99 added the designated 初始化器_小写 to the language, which allows
// programmers to move the "cursor" of an 初始化器_小写 to any element.
// The syntax looks like this:
//
//   int x[10] = { 1, 2, [5]=3, 4, 5, 6, 7 };
//
// `[5]` moves the cursor to the 5th element, so the 5th element of x
// is set to 3. Initialization 那么 continues forward in order, so
// 6th, 7th, 8th and 9th elements are initialized with 4, 5, 6 and 7,
// respectively. Unspecified elements (in this case, 3rd and 4th
// elements) are initialized with zero.
//
// Nesting is allowed, so the following 初始化器_小写 is valid:
//
//   int x[5][10] = { [5][8]=1, 2, 3 };
//
// It sets x[5][8], x[5][9] and x[6][0] to 1, 2 and 3, respectively.
//
// Use `.fieldname` to move the cursor for a struct 初始化器_小写. E.g.
//
//   struct { int a, b, c; } x = { .c=5 };
//
// The above 初始化器_小写 sets x.c to 5.
static void 数组_指示符(牌 **rest, 牌 *牌_短, 类型 *类型_缩, int *开始, int *终) {
  *开始 = 常量_表达式(&牌_短, 牌_短->下一个);
  if (*开始 >= 类型_缩->数组_长度)
    错误_牌_短(牌_短, "array designator index exceeds array bounds");

  if (等于(牌_短, "...")) {
    *终 = 常量_表达式(&牌_短, 牌_短->下一个);
    if (*终 >= 类型_缩->数组_长度)
      错误_牌_短(牌_短, "array designator index exceeds array bounds");
    if (*终 < *开始)
      错误_牌_短(牌_短, "array designator 范围_短 [%d, %d] is empty", *开始, *终);
  } else {
    *终 = *开始;
  }

  *rest = 跳过(牌_短, "]");
}

// struct-designator = "." ident
static 成员 *结构_指示符(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  牌 *开启 = 牌_短;
  牌_短 = 跳过(牌_短, ".");
  if (牌_短->种类 != 牌_缩_标识符)
    错误_牌_短(牌_短, "expected a field designator");

  for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
    // Anonymous struct 成员_小
    if (成员_短->类型_缩->种类 == 类型_结构 && !成员_短->名字) {
      if (取_结构_成员(成员_短->类型_缩, 牌_短)) {
        *rest = 开启;
        return 成员_短;
      }
      continue;
    }

    // Regular struct 成员_小
    if (成员_短->名字->长度_短 == 牌_短->长度_短 && !strncmp(成员_短->名字->位置_短, 牌_短->位置_短, 牌_短->长度_短)) {
      *rest = 牌_短->下一个;
      return 成员_短;
    }
  }

  错误_牌_短(牌_短, "struct has no such 成员_小");
}

// 指定_ = ("[" const-表达式_短 "]" | "." ident)* "="? 初始化器_小写
static void 指定_(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  if (等于(牌_短, "[")) {
    if (初始->类型_缩->种类 != 类型_数组)
      错误_牌_短(牌_短, "array index in non-array 初始化器_小写");

    int 开始, 终;
    数组_指示符(&牌_短, 牌_短, 初始->类型_缩, &开始, &终);

    牌 *tok2;
    for (int i = 开始; i <= 终; i++)
      指定_(&tok2, 牌_短, 初始->孩子们[i]);
    数组_初始化器2(rest, tok2, 初始, 开始 + 1);
    return;
  }

  if (等于(牌_短, ".") && 初始->类型_缩->种类 == 类型_结构) {
    成员 *成员_短 = 结构_指示符(&牌_短, 牌_短, 初始->类型_缩);
    指定_(&牌_短, 牌_短, 初始->孩子们[成员_短->索引_缩]);
    初始->表达式_短 = NULL;
    结构_初始化器2(rest, 牌_短, 初始, 成员_短->下一个);
    return;
  }

  if (等于(牌_短, ".") && 初始->类型_缩->种类 == 类型_联合) {
    成员 *成员_短 = 结构_指示符(&牌_短, 牌_短, 初始->类型_缩);
    初始->成员_短 = 成员_短;
    指定_(rest, 牌_短, 初始->孩子们[成员_短->索引_缩]);
    return;
  }

  if (等于(牌_短, "."))
    错误_牌_短(牌_短, "field 名字 not in struct or union 初始化器_小写");

  if (等于(牌_短, "="))
    牌_短 = 牌_短->下一个;
  初始化器2(rest, 牌_短, 初始);
}

// An array length can be omitted if an array has an 初始化器_小写
// (e.g. `int x[] = {1,2,3}`). If it's omitted, count the number
// of 初始化器_小写 elements.
static int 计数_数组_初始_元素(牌 *牌_短, 类型 *类型_缩) {
  bool first = true;
  初始化器 *假人 = 新_初始化器_(类型_缩->基础, true);

  int i = 0, max = 0;

  while (!消耗_终(&牌_短, 牌_短)) {
    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    if (等于(牌_短, "[")) {
      i = 常量_表达式(&牌_短, 牌_短->下一个);
      if (等于(牌_短, "..."))
        i = 常量_表达式(&牌_短, 牌_短->下一个);
      牌_短 = 跳过(牌_短, "]");
      指定_(&牌_短, 牌_短, 假人);
    } else {
      初始化器2(&牌_短, 牌_短, 假人);
    }

    i++;
    max = 最大_缩(max, i);
  }
  return max;
}

// array-initializer1 = "{" 初始化器_小写 ("," 初始化器_小写)* ","? "}"
static void 数组_初始化器1_(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  牌_短 = 跳过(牌_短, "{");

  if (初始->是否_有弹性) {
    int 长度_短 = 计数_数组_初始_元素(牌_短, 初始->类型_缩);
    *初始 = *新_初始化器_(数组_阵列(初始->类型_缩->基础, 长度_短), false);
  }

  bool first = true;

  if (初始->是否_有弹性) {
    int 长度_短 = 计数_数组_初始_元素(牌_短, 初始->类型_缩);
    *初始 = *新_初始化器_(数组_阵列(初始->类型_缩->基础, 长度_短), false);
  }

  for (int i = 0; !消耗_终(rest, 牌_短); i++) {
    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    if (等于(牌_短, "[")) {
      int 开始, 终;
      数组_指示符(&牌_短, 牌_短, 初始->类型_缩, &开始, &终);

      牌 *tok2;
      for (int j = 开始; j <= 终; j++)
        指定_(&tok2, 牌_短, 初始->孩子们[j]);
      牌_短 = tok2;
      i = 终;
      continue;
    }

    if (i < 初始->类型_缩->数组_长度)
      初始化器2(&牌_短, 牌_短, 初始->孩子们[i]);
    else
      牌_短 = 跳过_过量_元素(牌_短);
  }
}

// array-初始化器2 = 初始化器_小写 ("," 初始化器_小写)*
static void 数组_初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始, int i) {
  if (初始->是否_有弹性) {
    int 长度_短 = 计数_数组_初始_元素(牌_短, 初始->类型_缩);
    *初始 = *新_初始化器_(数组_阵列(初始->类型_缩->基础, 长度_短), false);
  }

  for (; i < 初始->类型_缩->数组_长度 && !是否_终(牌_短); i++) {
    牌 *开启 = 牌_短;
    if (i > 0)
      牌_短 = 跳过(牌_短, ",");

    if (等于(牌_短, "[") || 等于(牌_短, ".")) {
      *rest = 开启;
      return;
    }

    初始化器2(&牌_短, 牌_短, 初始->孩子们[i]);
  }
  *rest = 牌_短;
}

// struct-initializer1 = "{" 初始化器_小写 ("," 初始化器_小写)* ","? "}"
static void 结构_初始化器1_(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  牌_短 = 跳过(牌_短, "{");

  成员 *成员_短 = 初始->类型_缩->成员_复数;
  bool first = true;

  while (!消耗_终(rest, 牌_短)) {
    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    if (等于(牌_短, ".")) {
      成员_短 = 结构_指示符(&牌_短, 牌_短, 初始->类型_缩);
      指定_(&牌_短, 牌_短, 初始->孩子们[成员_短->索引_缩]);
      成员_短 = 成员_短->下一个;
      continue;
    }

    if (成员_短) {
      初始化器2(&牌_短, 牌_短, 初始->孩子们[成员_短->索引_缩]);
      成员_短 = 成员_短->下一个;
    } else {
      牌_短 = 跳过_过量_元素(牌_短);
    }
  }
}

// struct-初始化器2 = 初始化器_小写 ("," 初始化器_小写)*
static void 结构_初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始, 成员 *成员_短) {
  bool first = true;

  for (; 成员_短 && !是否_终(牌_短); 成员_短 = 成员_短->下一个) {
    牌 *开启 = 牌_短;

    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    if (等于(牌_短, "[") || 等于(牌_短, ".")) {
      *rest = 开启;
      return;
    }

    初始化器2(&牌_短, 牌_短, 初始->孩子们[成员_短->索引_缩]);
  }
  *rest = 牌_短;
}

static void 联合_初始化器_(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  // Unlike structs, union initializers take only one 初始化器_小写,
  // and that initializes the first union 成员_小 by default.
  // You can initialize other 成员_小 using a designated 初始化器_小写.
  if (等于(牌_短, "{") && 等于(牌_短->下一个, ".")) {
    成员 *成员_短 = 结构_指示符(&牌_短, 牌_短->下一个, 初始->类型_缩);
    初始->成员_短 = 成员_短;
    指定_(&牌_短, 牌_短, 初始->孩子们[成员_短->索引_缩]);
    *rest = 跳过(牌_短, "}");
    return;
  }

  初始->成员_短 = 初始->类型_缩->成员_复数;

  if (等于(牌_短, "{")) {
    初始化器2(&牌_短, 牌_短->下一个, 初始->孩子们[0]);
    消耗(&牌_短, 牌_短, ",");
    *rest = 跳过(牌_短, "}");
  } else {
    初始化器2(rest, 牌_短, 初始->孩子们[0]);
  }
}

// 初始化器_小写 = string-初始化器_小写 | array-初始化器_小写
//             | struct-初始化器_小写 | union-初始化器_小写
//             | 赋值_
static void 初始化器2(牌 **rest, 牌 *牌_短, 初始化器 *初始) {
  if (初始->类型_缩->种类 == 类型_数组 && 牌_短->种类 == 牌_缩_串字面) {
    字符串_初始化器_(rest, 牌_短, 初始);
    return;
  }

  if (初始->类型_缩->种类 == 类型_数组) {
    if (等于(牌_短, "{"))
      数组_初始化器1_(rest, 牌_短, 初始);
    else
      数组_初始化器2(rest, 牌_短, 初始, 0);
    return;
  }

  if (初始->类型_缩->种类 == 类型_结构) {
    if (等于(牌_短, "{")) {
      结构_初始化器1_(rest, 牌_短, 初始);
      return;
    }

    // A struct can be initialized with another struct. E.g.
    // `struct T x = y;` where y is a variable of type `struct T`.
    // Handle that case first.
    节点 *表达式_短 = 赋值_(rest, 牌_短);
    添加_类型_小(表达式_短);
    if (表达式_短->类型_缩->种类 == 类型_结构) {
      初始->表达式_短 = 表达式_短;
      return;
    }

    结构_初始化器2(rest, 牌_短, 初始, 初始->类型_缩->成员_复数);
    return;
  }

  if (初始->类型_缩->种类 == 类型_联合) {
    联合_初始化器_(rest, 牌_短, 初始);
    return;
  }

  if (等于(牌_短, "{")) {
    // An 初始化器_小写 for a scalar variable can be surrounded by
    // braces. E.g. `int x = {3};`. Handle that case.
    初始化器2(&牌_短, 牌_短->下一个, 初始);
    *rest = 跳过(牌_短, "}");
    return;
  }

  初始->表达式_短 = 赋值_(rest, 牌_短);
}

static 类型 *复制_结构_类型_(类型 *类型_缩) {
  类型_缩 = 复制_类型(类型_缩);

  成员 头部 = {};
  成员 *当前_小 = &头部;
  for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
    成员 *m = calloc(1, sizeof(成员));
    *m = *成员_短;
    当前_小 = 当前_小->下一个 = m;
  }

  类型_缩->成员_复数 = 头部.下一个;
  return 类型_缩;
}

static 初始化器 *初始化器_小写(牌 **rest, 牌 *牌_短, 类型 *类型_缩, 类型 **new_ty) {
  初始化器 *初始 = 新_初始化器_(类型_缩, true);
  初始化器2(rest, 牌_短, 初始);

  if ((类型_缩->种类 == 类型_结构 || 类型_缩->种类 == 类型_联合) && 类型_缩->是否_有弹性) {
    类型_缩 = 复制_结构_类型_(类型_缩);

    成员 *成员_短 = 类型_缩->成员_复数;
    while (成员_短->下一个)
      成员_短 = 成员_短->下一个;
    成员_短->类型_缩 = 初始->孩子们[成员_短->索引_缩]->类型_缩;
    类型_缩->大小 += 成员_短->类型_缩->大小;

    *new_ty = 类型_缩;
    return 初始;
  }

  *new_ty = 初始->类型_缩;
  return 初始;
}

static 节点 *初始_设计_表达式_(初始设计_短 *设计_短_, 牌 *牌_短) {
  if (设计_短_->变量_短)
    return 新_变量_节点(设计_短_->变量_短, 牌_短);

  if (设计_短_->成员_小) {
    节点 *节点_小 = 新_一元(抽象节点_成员, 初始_设计_表达式_(设计_短_->下一个, 牌_短), 牌_短);
    节点_小->成员_小 = 设计_短_->成员_小;
    return 节点_小;
  }

  节点 *左手边 = 初始_设计_表达式_(设计_短_->下一个, 牌_短);
  节点 *右手边 = 新_数号_短(设计_短_->索引_缩, 牌_短);
  return 新_一元(抽象节点_解引用, 新_加法(左手边, 右手边, 牌_短), 牌_短);
}

static 节点 *创建_本地变量_初始(初始化器 *初始, 类型 *类型_缩, 初始设计_短 *设计_短_, 牌 *牌_短) {
  if (类型_缩->种类 == 类型_数组) {
    节点 *节点_小 = 新_节点_(抽象节点_空指针_表达式_短, 牌_短);
    for (int i = 0; i < 类型_缩->数组_长度; i++) {
      初始设计_短 desg2 = {设计_短_, i};
      节点 *右手边 = 创建_本地变量_初始(初始->孩子们[i], 类型_缩->基础, &desg2, 牌_短);
      节点_小 = 新_二元(抽象节点_逗号, 节点_小, 右手边, 牌_短);
    }
    return 节点_小;
  }

  if (类型_缩->种类 == 类型_结构 && !初始->表达式_短) {
    节点 *节点_小 = 新_节点_(抽象节点_空指针_表达式_短, 牌_短);

    for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
      初始设计_短 desg2 = {设计_短_, 0, 成员_短};
      节点 *右手边 = 创建_本地变量_初始(初始->孩子们[成员_短->索引_缩], 成员_短->类型_缩, &desg2, 牌_短);
      节点_小 = 新_二元(抽象节点_逗号, 节点_小, 右手边, 牌_短);
    }
    return 节点_小;
  }

  if (类型_缩->种类 == 类型_联合) {
    成员 *成员_短 = 初始->成员_短 ? 初始->成员_短 : 类型_缩->成员_复数;
    初始设计_短 desg2 = {设计_短_, 0, 成员_短};
    return 创建_本地变量_初始(初始->孩子们[成员_短->索引_缩], 成员_短->类型_缩, &desg2, 牌_短);
  }

  if (!初始->表达式_短)
    return 新_节点_(抽象节点_空指针_表达式_短, 牌_短);

  节点 *左手边 = 初始_设计_表达式_(设计_短_, 牌_短);
  return 新_二元(抽象节点_赋值, 左手边, 初始->表达式_短, 牌_短);
}

// A variable definition with an 初始化器_小写 is a shorthand notation
// for a variable definition followed by assignments. This 正函数_
// generates assignment expressions for an 初始化器_小写. For example,
// `int x[2][2] = {{6, 7}, {8, 9}}` is converted to the following
// expressions:
//
//   x[0][0] = 6;
//   x[0][1] = 7;
//   x[1][0] = 8;
//   x[1][1] = 9;
static 节点 *本地变量_初始化器(牌 **rest, 牌 *牌_短, 对象_短 *变量_短) {
  初始化器 *初始 = 初始化器_小写(rest, 牌_短, 变量_短->类型_缩, &变量_短->类型_缩);
  初始设计_短 设计_短_ = {NULL, 0, NULL, 变量_短};

  // If a partial 初始化器_小写 list is given, the standard requires
  // that unspecified elements are set to 0. Here, we simply
  // zero-initialize the entire memory region of a variable before
  // initializing it with user-supplied values.
  节点 *左手边 = 新_节点_(抽象节点_内存清零, 牌_短);
  左手边->变量_短 = 变量_短;

  节点 *右手边 = 创建_本地变量_初始(初始, 变量_短->类型_缩, &设计_短_, 牌_短);
  return 新_二元(抽象节点_逗号, 左手边, 右手边, 牌_短);
}

static uint64_t 读_缓冲(char *缓冲_短, int sz) {
  if (sz == 1)
    return *缓冲_短;
  if (sz == 2)
    return *(uint16_t *)缓冲_短;
  if (sz == 4)
    return *(uint32_t *)缓冲_短;
  if (sz == 8)
    return *(uint64_t *)缓冲_短;
  不可达();
}

static void 写_缓冲(char *缓冲_短, uint64_t 值_短, int sz) {
  if (sz == 1)
    *缓冲_短 = 值_短;
  else if (sz == 2)
    *(uint16_t *)缓冲_短 = 值_短;
  else if (sz == 4)
    *(uint32_t *)缓冲_短 = 值_短;
  else if (sz == 8)
    *(uint64_t *)缓冲_短 = 值_短;
  else
    不可达();
}

static 重定位 *
写_全局变量_数据(重定位 *当前_小, 初始化器 *初始, 类型 *类型_缩, char *缓冲_短, int 偏移) {
  if (类型_缩->种类 == 类型_数组) {
    int sz = 类型_缩->基础->大小;
    for (int i = 0; i < 类型_缩->数组_长度; i++)
      当前_小 = 写_全局变量_数据(当前_小, 初始->孩子们[i], 类型_缩->基础, 缓冲_短, 偏移 + sz * i);
    return 当前_小;
  }

  if (类型_缩->种类 == 类型_结构) {
    for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
      if (成员_短->是否_位字段) {
        节点 *表达式_短 = 初始->孩子们[成员_短->索引_缩]->表达式_短;
        if (!表达式_短)
          break;

        char *位置_短 = 缓冲_短 + 偏移 + 成员_短->偏移;
        uint64_t oldval = 读_缓冲(位置_短, 成员_短->类型_缩->大小);
        uint64_t newval = 求值_短(表达式_短);
        uint64_t mask = (1L << 成员_短->位_宽度) - 1;
        uint64_t combined = oldval | ((newval & mask) << 成员_短->位_偏移);
        写_缓冲(位置_短, combined, 成员_短->类型_缩->大小);
      } else {
        当前_小 = 写_全局变量_数据(当前_小, 初始->孩子们[成员_短->索引_缩], 成员_短->类型_缩, 缓冲_短,
                              偏移 + 成员_短->偏移);
      }
    }
    return 当前_小;
  }

  if (类型_缩->种类 == 类型_联合) {
    if (!初始->成员_短)
      return 当前_小;
    return 写_全局变量_数据(当前_小, 初始->孩子们[初始->成员_短->索引_缩],
                           初始->成员_短->类型_缩, 缓冲_短, 偏移);
  }

  if (!初始->表达式_短)
    return 当前_小;

  if (类型_缩->种类 == 类型_浮点) {
    *(float *)(缓冲_短 + 偏移) = 求值_双精度(初始->表达式_短);
    return 当前_小;
  }

  if (类型_缩->种类 == 类型_双浮) {
    *(double *)(缓冲_短 + 偏移) = 求值_双精度(初始->表达式_短);
    return 当前_小;
  }

  char **标号 = NULL;
  uint64_t 值_短 = 求值_短2(初始->表达式_短, &标号);

  if (!标号) {
    写_缓冲(缓冲_短 + 偏移, 值_短, 类型_缩->大小);
    return 当前_小;
  }

  重定位 *重定位_短 = calloc(1, sizeof(重定位));
  重定位_短->偏移 = 偏移;
  重定位_短->标号 = 标号;
  重定位_短->附加物 = 值_短;
  当前_小->下一个 = 重定位_短;
  return 当前_小->下一个;
}

// Initializers for global variables are evaluated at compile-time and
// embedded to .数据 section. This 正函数_ serializes 初始化器
// objects to a flat byte array. It is a compile 错误 if an
// 初始化器_小写 list contains a non-constant expression.
static void 全局变量_初始化器(牌 **rest, 牌 *牌_短, 对象_短 *变量_短) {
  初始化器 *初始 = 初始化器_小写(rest, 牌_短, 变量_短->类型_缩, &变量_短->类型_缩);

  重定位 头部 = {};
  char *缓冲_短 = calloc(1, 变量_短->类型_缩->大小);
  写_全局变量_数据(&头部, 初始, 变量_短->类型_缩, 缓冲_短, 0);
  变量_短->初始_数据 = 缓冲_短;
  变量_短->重定位_短 = 头部.下一个;
}

// Returns true if a given token represents a type.
static bool 是否_类型名(牌 *牌_短) {
  static 哈希映射 映射;

  if (映射.容量 == 0) {
    static char *关键字_缩[] = {
      "void", "_Bool", "char", "short", "int", "long", "struct", "union",
      "typedef", "enum", "static", "extern", "_Alignas", "signed", "unsigned",
      "const", "volatile", "auto", "register", "restrict", "__restrict",
      "__restrict__", "_Noreturn", "float", "double", "typeof", "inline",
      "_Thread_local", "__thread", "_Atomic",
    };

    for (int i = 0; i < sizeof(关键字_缩) / sizeof(*关键字_缩); i++)
      哈希映射_放置(&映射, 关键字_缩[i], (void *)1);
  }

  return 哈希映射_取2(&映射, 牌_短->位置_短, 牌_短->长度_短) || 找_类型定义_(牌_短);
}

// asm-语句_缩 = "asm" ("volatile" | "inline")* "(" string-literal ")"
static 节点 *汇编_语句_短(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_汇编, 牌_短);
  牌_短 = 牌_短->下一个;

  while (等于(牌_短, "volatile") || 等于(牌_短, "inline"))
    牌_短 = 牌_短->下一个;

  牌_短 = 跳过(牌_短, "(");
  if (牌_短->种类 != 牌_缩_串字面 || 牌_短->类型_缩->基础->种类 != 类型_字符)
    错误_牌_短(牌_短, "expected string literal");
  节点_小->汇编_字符串 = 牌_短->串_短;
  *rest = 跳过(牌_短->下一个, ")");
  return 节点_小;
}

// 语句_缩 = "return" 表达式_短? ";"
//      | "if" "(" 表达式_短 ")" 语句_缩 ("else" 语句_缩)?
//      | "switch" "(" 表达式_短 ")" 语句_缩
//      | "case" const-表达式_短 ("..." const-表达式_短)? ":" 语句_缩
//      | "default" ":" 语句_缩
//      | "for" "(" 表达式_短-语句_缩 表达式_短? ";" 表达式_短? ")" 语句_缩
//      | "while" "(" 表达式_短 ")" 语句_缩
//      | "do" 语句_缩 "while" "(" 表达式_短 ")" ";"
//      | "asm" asm-语句_缩
//      | "goto" (ident | "*" 表达式_短) ";"
//      | "break" ";"
//      | "continue" ";"
//      | ident ":" 语句_缩
//      | "{" compound-语句_缩
//      | 表达式_短-语句_缩
static 节点 *语句_缩(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, "return")) {
    节点 *节点_小 = 新_节点_(抽象节点_返回, 牌_短);
    if (消耗(rest, 牌_短->下一个, ";"))
      return 节点_小;

    节点 *exp = 表达式_短(&牌_短, 牌_短->下一个);
    *rest = 跳过(牌_短, ";");

    添加_类型_小(exp);
    类型 *类型_缩 = 当前_函->类型_缩->返回_类型;
    if (类型_缩->种类 != 类型_结构 && 类型_缩->种类 != 类型_联合)
      exp = 新_类型转换(exp, 当前_函->类型_缩->返回_类型);

    节点_小->左手边 = exp;
    return 节点_小;
  }

  if (等于(牌_短, "if")) {
    节点 *节点_小 = 新_节点_(抽象节点_若, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");
    节点_小->条件_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ")");
    节点_小->那么 = 语句_缩(&牌_短, 牌_短);
    if (等于(牌_短, "else"))
      节点_小->否则 = 语句_缩(&牌_短, 牌_短->下一个);
    *rest = 牌_短;
    return 节点_小;
  }

  if (等于(牌_短, "switch")) {
    节点 *节点_小 = 新_节点_(抽象节点_切换, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");
    节点_小->条件_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ")");

    节点 *sw = 当前_切换;
    当前_切换 = 节点_小;

    char *brk = 断_标号;
    断_标号 = 节点_小->断_标号 = 新_独特性_名字();

    节点_小->那么 = 语句_缩(rest, 牌_短);

    当前_切换 = sw;
    断_标号 = brk;
    return 节点_小;
  }

  if (等于(牌_短, "case")) {
    if (!当前_切换)
      错误_牌_短(牌_短, "stray case");

    节点 *节点_小 = 新_节点_(抽象节点_情况, 牌_短);
    int 开始 = 常量_表达式(&牌_短, 牌_短->下一个);
    int 终;

    if (等于(牌_短, "...")) {
      // [GNU] Case ranges, e.g. "case 1 ... 5:"
      终 = 常量_表达式(&牌_短, 牌_短->下一个);
      if (终 < 开始)
        错误_牌_短(牌_短, "empty case 范围_短 specified");
    } else {
      终 = 开始;
    }

    牌_短 = 跳过(牌_短, ":");
    节点_小->标号 = 新_独特性_名字();
    节点_小->左手边 = 语句_缩(rest, 牌_短);
    节点_小->开始 = 开始;
    节点_小->终 = 终;
    节点_小->情况_下一个 = 当前_切换->情况_下一个;
    当前_切换->情况_下一个 = 节点_小;
    return 节点_小;
  }

  if (等于(牌_短, "default")) {
    if (!当前_切换)
      错误_牌_短(牌_短, "stray default");

    节点 *节点_小 = 新_节点_(抽象节点_情况, 牌_短);
    牌_短 = 跳过(牌_短->下一个, ":");
    节点_小->标号 = 新_独特性_名字();
    节点_小->左手边 = 语句_缩(rest, 牌_短);
    当前_切换->默认_情况 = 节点_小;
    return 节点_小;
  }

  if (等于(牌_短, "for")) {
    节点 *节点_小 = 新_节点_(抽象节点_为, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");

    进入_作用域();

    char *brk = 断_标号;
    char *cont = 继续_标号;
    断_标号 = 节点_小->断_标号 = 新_独特性_名字();
    继续_标号 = 节点_小->继续_标号 = 新_独特性_名字();

    if (是否_类型名(牌_短)) {
      类型 *基本类型 = 声明规格(&牌_短, 牌_短, NULL);
      节点_小->初始 = 正声明(&牌_短, 牌_短, 基本类型, NULL);
    } else {
      节点_小->初始 = 表达式_语句_缩(&牌_短, 牌_短);
    }

    if (!等于(牌_短, ";"))
      节点_小->条件_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ";");

    if (!等于(牌_短, ")"))
      节点_小->增_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ")");

    节点_小->那么 = 语句_缩(rest, 牌_短);

    离开_作用域();
    断_标号 = brk;
    继续_标号 = cont;
    return 节点_小;
  }

  if (等于(牌_短, "while")) {
    节点 *节点_小 = 新_节点_(抽象节点_为, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");
    节点_小->条件_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ")");

    char *brk = 断_标号;
    char *cont = 继续_标号;
    断_标号 = 节点_小->断_标号 = 新_独特性_名字();
    继续_标号 = 节点_小->继续_标号 = 新_独特性_名字();

    节点_小->那么 = 语句_缩(rest, 牌_短);

    断_标号 = brk;
    继续_标号 = cont;
    return 节点_小;
  }

  if (等于(牌_短, "do")) {
    节点 *节点_小 = 新_节点_(抽象节点_做, 牌_短);

    char *brk = 断_标号;
    char *cont = 继续_标号;
    断_标号 = 节点_小->断_标号 = 新_独特性_名字();
    继续_标号 = 节点_小->继续_标号 = 新_独特性_名字();

    节点_小->那么 = 语句_缩(&牌_短, 牌_短->下一个);

    断_标号 = brk;
    继续_标号 = cont;

    牌_短 = 跳过(牌_短, "while");
    牌_短 = 跳过(牌_短, "(");
    节点_小->条件_短 = 表达式_短(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ")");
    *rest = 跳过(牌_短, ";");
    return 节点_小;
  }

  if (等于(牌_短, "asm"))
    return 汇编_语句_短(rest, 牌_短);

  if (等于(牌_短, "goto")) {
    if (等于(牌_短->下一个, "*")) {
      // [GNU] `goto *ptr` jumps to the address specified by `ptr`.
      节点 *节点_小 = 新_节点_(抽象节点_跳转_表达式, 牌_短);
      节点_小->左手边 = 表达式_短(&牌_短, 牌_短->下一个->下一个);
      *rest = 跳过(牌_短, ";");
      return 节点_小;
    }

    节点 *节点_小 = 新_节点_(抽象节点_跳转, 牌_短);
    节点_小->标号 = 取_标识_短(牌_短->下一个);
    节点_小->跳转_下一个 = 跳转的;
    跳转的 = 节点_小;
    *rest = 跳过(牌_短->下一个->下一个, ";");
    return 节点_小;
  }

  if (等于(牌_短, "break")) {
    if (!断_标号)
      错误_牌_短(牌_短, "stray break");
    节点 *节点_小 = 新_节点_(抽象节点_跳转, 牌_短);
    节点_小->独特性_标号 = 断_标号;
    *rest = 跳过(牌_短->下一个, ";");
    return 节点_小;
  }

  if (等于(牌_短, "continue")) {
    if (!继续_标号)
      错误_牌_短(牌_短, "stray continue");
    节点 *节点_小 = 新_节点_(抽象节点_跳转, 牌_短);
    节点_小->独特性_标号 = 继续_标号;
    *rest = 跳过(牌_短->下一个, ";");
    return 节点_小;
  }

  if (牌_短->种类 == 牌_缩_标识符 && 等于(牌_短->下一个, ":")) {
    节点 *节点_小 = 新_节点_(抽象节点_标号, 牌_短);
    节点_小->标号 = strndup(牌_短->位置_短, 牌_短->长度_短);
    节点_小->独特性_标号 = 新_独特性_名字();
    节点_小->左手边 = 语句_缩(rest, 牌_短->下一个->下一个);
    节点_小->跳转_下一个 = 标号的;
    标号的 = 节点_小;
    return 节点_小;
  }

  if (等于(牌_短, "{"))
    return 复合的_语句(rest, 牌_短->下一个);

  return 表达式_语句_缩(rest, 牌_短);
}

// compound-语句_缩 = (typedef | 正声明 | 语句_缩)* "}"
static 节点 *复合的_语句(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 新_节点_(抽象节点_块, 牌_短);
  节点 头部 = {};
  节点 *当前_小 = &头部;

  进入_作用域();

  while (!等于(牌_短, "}")) {
    if (是否_类型名(牌_短) && !等于(牌_短->下一个, ":")) {
      变量基本属性_短 attr = {};
      类型 *基本类型 = 声明规格(&牌_短, 牌_短, &attr);

      if (attr.是否_类型定义) {
        牌_短 = 解析_类型定义(牌_短, 基本类型);
        continue;
      }

      if (是否_函数(牌_短)) {
        牌_短 = 正函数_(牌_短, 基本类型, &attr);
        continue;
      }

      if (attr.是否_外部) {
        牌_短 = 全局_变量_(牌_短, 基本类型, &attr);
        continue;
      }

      当前_小 = 当前_小->下一个 = 正声明(&牌_短, 牌_短, 基本类型, &attr);
    } else {
      当前_小 = 当前_小->下一个 = 语句_缩(&牌_短, 牌_短);
    }
    添加_类型_小(当前_小);
  }

  离开_作用域();

  节点_小->体 = 头部.下一个;
  *rest = 牌_短->下一个;
  return 节点_小;
}

// 表达式_短-语句_缩 = 表达式_短? ";"
static 节点 *表达式_语句_缩(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, ";")) {
    *rest = 牌_短->下一个;
    return 新_节点_(抽象节点_块, 牌_短);
  }

  节点 *节点_小 = 新_节点_(抽象节点_表达式_语句, 牌_短);
  节点_小->左手边 = 表达式_短(&牌_短, 牌_短);
  *rest = 跳过(牌_短, ";");
  return 节点_小;
}

// 表达式_短 = 赋值_ ("," 表达式_短)?
static 节点 *表达式_短(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 赋值_(&牌_短, 牌_短);

  if (等于(牌_短, ","))
    return 新_二元(抽象节点_逗号, 节点_小, 表达式_短(rest, 牌_短->下一个), 牌_短);

  *rest = 牌_短;
  return 节点_小;
}

static int64_t 求值_短(节点 *节点_小) {
  return 求值_短2(节点_小, NULL);
}

// Evaluate a given 节点_小 as a constant expression.
//
// A constant expression is either just a number or ptr+n where ptr
// is a pointer to a global variable and n is a postiive/negative
// number. The latter form is accepted only as an initialization
// expression for a global variable.
static int64_t 求值_短2(节点 *节点_小, char ***标号) {
  添加_类型_小(节点_小);

  if (是否_浮点数_小(节点_小->类型_缩))
    return 求值_双精度(节点_小);

  switch (节点_小->种类) {
  case 抽象节点_加:
    return 求值_短2(节点_小->左手边, 标号) + 求值_短(节点_小->右手边);
  case 抽象节点_减:
    return 求值_短2(节点_小->左手边, 标号) - 求值_短(节点_小->右手边);
  case 抽象节点_乘:
    return 求值_短(节点_小->左手边) * 求值_短(节点_小->右手边);
  case 抽象节点_除:
    if (节点_小->类型_缩->是否_无符号)
      return (uint64_t)求值_短(节点_小->左手边) / 求值_短(节点_小->右手边);
    return 求值_短(节点_小->左手边) / 求值_短(节点_小->右手边);
  case 抽象节点_负:
    return -求值_短(节点_小->左手边);
  case 抽象节点_余数:
    if (节点_小->类型_缩->是否_无符号)
      return (uint64_t)求值_短(节点_小->左手边) % 求值_短(节点_小->右手边);
    return 求值_短(节点_小->左手边) % 求值_短(节点_小->右手边);
  case 抽象节点_位与:
    return 求值_短(节点_小->左手边) & 求值_短(节点_小->右手边);
  case 抽象节点_位或:
    return 求值_短(节点_小->左手边) | 求值_短(节点_小->右手边);
  case 抽象节点_位异或:
    return 求值_短(节点_小->左手边) ^ 求值_短(节点_小->右手边);
  case 抽象节点_左移:
    return 求值_短(节点_小->左手边) << 求值_短(节点_小->右手边);
  case 抽象节点_右移:
    if (节点_小->类型_缩->是否_无符号 && 节点_小->类型_缩->大小 == 8)
      return (uint64_t)求值_短(节点_小->左手边) >> 求值_短(节点_小->右手边);
    return 求值_短(节点_小->左手边) >> 求值_短(节点_小->右手边);
  case 抽象节点_等于:
    return 求值_短(节点_小->左手边) == 求值_短(节点_小->右手边);
  case 抽象节点_不等:
    return 求值_短(节点_小->左手边) != 求值_短(节点_小->右手边);
  case 抽象节点_小于:
    if (节点_小->左手边->类型_缩->是否_无符号)
      return (uint64_t)求值_短(节点_小->左手边) < 求值_短(节点_小->右手边);
    return 求值_短(节点_小->左手边) < 求值_短(节点_小->右手边);
  case 抽象节点_小等:
    if (节点_小->左手边->类型_缩->是否_无符号)
      return (uint64_t)求值_短(节点_小->左手边) <= 求值_短(节点_小->右手边);
    return 求值_短(节点_小->左手边) <= 求值_短(节点_小->右手边);
  case 抽象节点_条件:
    return 求值_短(节点_小->条件_短) ? 求值_短2(节点_小->那么, 标号) : 求值_短2(节点_小->否则, 标号);
  case 抽象节点_逗号:
    return 求值_短2(节点_小->右手边, 标号);
  case 抽象节点_非:
    return !求值_短(节点_小->左手边);
  case 抽象节点_位非:
    return ~求值_短(节点_小->左手边);
  case 抽象节点_逻辑与:
    return 求值_短(节点_小->左手边) && 求值_短(节点_小->右手边);
  case 抽象节点_逻辑或:
    return 求值_短(节点_小->左手边) || 求值_短(节点_小->右手边);
  case 抽象节点_类型转换: {
    int64_t 值_短 = 求值_短2(节点_小->左手边, 标号);
    if (是否_整数_小(节点_小->类型_缩)) {
      switch (节点_小->类型_缩->大小) {
      case 1: return 节点_小->类型_缩->是否_无符号 ? (uint8_t)值_短 : (int8_t)值_短;
      case 2: return 节点_小->类型_缩->是否_无符号 ? (uint16_t)值_短 : (int16_t)值_短;
      case 4: return 节点_小->类型_缩->是否_无符号 ? (uint32_t)值_短 : (int32_t)值_短;
      }
    }
    return 值_短;
  }
  case 抽象节点_地址:
    return 求值_右值(节点_小->左手边, 标号);
  case 抽象节点_标号_值:
    *标号 = &节点_小->独特性_标号;
    return 0;
  case 抽象节点_成员:
    if (!标号)
      错误_牌_短(节点_小->牌_短, "not a compile-time constant");
    if (节点_小->类型_缩->种类 != 类型_数组)
      错误_牌_短(节点_小->牌_短, "invalid 初始化器_小写");
    return 求值_右值(节点_小->左手边, 标号) + 节点_小->成员_小->偏移;
  case 抽象节点_变量:
    if (!标号)
      错误_牌_短(节点_小->牌_短, "not a compile-time constant");
    if (节点_小->变量_短->类型_缩->种类 != 类型_数组 && 节点_小->变量_短->类型_缩->种类 != 类型_函数)
      错误_牌_短(节点_小->牌_短, "invalid 初始化器_小写");
    *标号 = &节点_小->变量_短->名字;
    return 0;
  case 抽象节点_整数:
    return 节点_小->值_短;
  }

  错误_牌_短(节点_小->牌_短, "not a compile-time constant");
}

static int64_t 求值_右值(节点 *节点_小, char ***标号) {
  switch (节点_小->种类) {
  case 抽象节点_变量:
    if (节点_小->变量_短->是否_本地)
      错误_牌_短(节点_小->牌_短, "not a compile-time constant");
    *标号 = &节点_小->变量_短->名字;
    return 0;
  case 抽象节点_解引用:
    return 求值_短2(节点_小->左手边, 标号);
  case 抽象节点_成员:
    return 求值_右值(节点_小->左手边, 标号) + 节点_小->成员_小->偏移;
  }

  错误_牌_短(节点_小->牌_短, "invalid 初始化器_小写");
}

static bool 是否_常量_表达式(节点 *节点_小) {
  添加_类型_小(节点_小);

  switch (节点_小->种类) {
  case 抽象节点_加:
  case 抽象节点_减:
  case 抽象节点_乘:
  case 抽象节点_除:
  case 抽象节点_位与:
  case 抽象节点_位或:
  case 抽象节点_位异或:
  case 抽象节点_左移:
  case 抽象节点_右移:
  case 抽象节点_等于:
  case 抽象节点_不等:
  case 抽象节点_小于:
  case 抽象节点_小等:
  case 抽象节点_逻辑与:
  case 抽象节点_逻辑或:
    return 是否_常量_表达式(节点_小->左手边) && 是否_常量_表达式(节点_小->右手边);
  case 抽象节点_条件:
    if (!是否_常量_表达式(节点_小->条件_短))
      return false;
    return 是否_常量_表达式(求值_短(节点_小->条件_短) ? 节点_小->那么 : 节点_小->否则);
  case 抽象节点_逗号:
    return 是否_常量_表达式(节点_小->右手边);
  case 抽象节点_负:
  case 抽象节点_非:
  case 抽象节点_位非:
  case 抽象节点_类型转换:
    return 是否_常量_表达式(节点_小->左手边);
  case 抽象节点_整数:
    return true;
  }

  return false;
}

int64_t 常量_表达式(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 有条件的(rest, 牌_短);
  return 求值_短(节点_小);
}

static double 求值_双精度(节点 *节点_小) {
  添加_类型_小(节点_小);

  if (是否_整数_小(节点_小->类型_缩)) {
    if (节点_小->类型_缩->是否_无符号)
      return (unsigned long)求值_短(节点_小);
    return 求值_短(节点_小);
  }

  switch (节点_小->种类) {
  case 抽象节点_加:
    return 求值_双精度(节点_小->左手边) + 求值_双精度(节点_小->右手边);
  case 抽象节点_减:
    return 求值_双精度(节点_小->左手边) - 求值_双精度(节点_小->右手边);
  case 抽象节点_乘:
    return 求值_双精度(节点_小->左手边) * 求值_双精度(节点_小->右手边);
  case 抽象节点_除:
    return 求值_双精度(节点_小->左手边) / 求值_双精度(节点_小->右手边);
  case 抽象节点_负:
    return -求值_双精度(节点_小->左手边);
  case 抽象节点_条件:
    return 求值_双精度(节点_小->条件_短) ? 求值_双精度(节点_小->那么) : 求值_双精度(节点_小->否则);
  case 抽象节点_逗号:
    return 求值_双精度(节点_小->右手边);
  case 抽象节点_类型转换:
    if (是否_浮点数_小(节点_小->左手边->类型_缩))
      return 求值_双精度(节点_小->左手边);
    return 求值_短(节点_小->左手边);
  case 抽象节点_整数:
    return 节点_小->浮点值_短;
  }

  错误_牌_短(节点_小->牌_短, "not a compile-time constant");
}

// Convert op= operators to expressions containing an assignment.
//
// In general, `A op= C` is converted to ``tmp = &A, *tmp = *tmp op B`.
// However, if a given expression is of form `A.x op= C`, the 输入 is
// converted to `tmp = &A, (*tmp).x = (*tmp).x op C` to handle assignments
// to bitfields.
static 节点 *到_赋值_短(节点 *二元_小写) {
  添加_类型_小(二元_小写->左手边);
  添加_类型_小(二元_小写->右手边);
  牌 *牌_短 = 二元_小写->牌_短;

  // Convert `A.x op= C` to `tmp = &A, (*tmp).x = (*tmp).x op C`.
  if (二元_小写->左手边->种类 == 抽象节点_成员) {
    对象_短 *变量_短 = 新_本地变量_短("", 指针_到(二元_小写->左手边->左手边->类型_缩));

    节点 *expr1 = 新_二元(抽象节点_赋值, 新_变量_节点(变量_短, 牌_短),
                             新_一元(抽象节点_地址, 二元_小写->左手边->左手边, 牌_短), 牌_短);

    节点 *expr2 = 新_一元(抽象节点_成员,
                            新_一元(抽象节点_解引用, 新_变量_节点(变量_短, 牌_短), 牌_短),
                            牌_短);
    expr2->成员_小 = 二元_小写->左手边->成员_小;

    节点 *expr3 = 新_一元(抽象节点_成员,
                            新_一元(抽象节点_解引用, 新_变量_节点(变量_短, 牌_短), 牌_短),
                            牌_短);
    expr3->成员_小 = 二元_小写->左手边->成员_小;

    节点 *expr4 = 新_二元(抽象节点_赋值, expr2,
                             新_二元(二元_小写->种类, expr3, 二元_小写->右手边, 牌_短),
                             牌_短);

    return 新_二元(抽象节点_逗号, expr1, expr4, 牌_短);
  }

  // If A is an atomic type, Convert `A op= B` to
  //
  // ({
  //   T1 *地址_短的 = &A; T2 值_短 = (B); T1 旧的 = *地址_短的; T1 新的;
  //   do {
  //    新的 = 旧的 op 值_短;
  //   } while (!atomic_compare_exchange_strong(地址_短的, &旧的, 新的));
  //   新的;
  // })
  if (二元_小写->左手边->类型_缩->是否_原子) {
    节点 头部 = {};
    节点 *当前_小 = &头部;

    对象_短 *地址_短的 = 新_本地变量_短("", 指针_到(二元_小写->左手边->类型_缩));
    对象_短 *值_短 = 新_本地变量_短("", 二元_小写->右手边->类型_缩);
    对象_短 *旧的 = 新_本地变量_短("", 二元_小写->左手边->类型_缩);
    对象_短 *新的 = 新_本地变量_短("", 二元_小写->左手边->类型_缩);

    当前_小 = 当前_小->下一个 =
      新_一元(抽象节点_表达式_语句,
                新_二元(抽象节点_赋值, 新_变量_节点(地址_短的, 牌_短),
                           新_一元(抽象节点_地址, 二元_小写->左手边, 牌_短), 牌_短),
                牌_短);

    当前_小 = 当前_小->下一个 =
      新_一元(抽象节点_表达式_语句,
                新_二元(抽象节点_赋值, 新_变量_节点(值_短, 牌_短), 二元_小写->右手边, 牌_短),
                牌_短);

    当前_小 = 当前_小->下一个 =
      新_一元(抽象节点_表达式_语句,
                新_二元(抽象节点_赋值, 新_变量_节点(旧的, 牌_短),
                           新_一元(抽象节点_解引用, 新_变量_节点(地址_短的, 牌_短), 牌_短), 牌_短),
                牌_短);

    节点 *环_小 = 新_节点_(抽象节点_做, 牌_短);
    环_小->断_标号 = 新_独特性_名字();
    环_小->继续_标号 = 新_独特性_名字();

    节点 *体 = 新_二元(抽象节点_赋值,
                            新_变量_节点(新的, 牌_短),
                            新_二元(二元_小写->种类, 新_变量_节点(旧的, 牌_短),
                                       新_变量_节点(值_短, 牌_短), 牌_短),
                            牌_短);

    环_小->那么 = 新_节点_(抽象节点_块, 牌_短);
    环_小->那么->体 = 新_一元(抽象节点_表达式_语句, 体, 牌_短);

    节点 *cas = 新_节点_(抽象节点_对比与交换, 牌_短);
    cas->对比与交换_地址 = 新_变量_节点(地址_短的, 牌_短);
    cas->对比与交换_旧 = 新_一元(抽象节点_地址, 新_变量_节点(旧的, 牌_短), 牌_短);
    cas->对比与交换_新 = 新_变量_节点(新的, 牌_短);
    环_小->条件_短 = 新_一元(抽象节点_非, cas, 牌_短);

    当前_小 = 当前_小->下一个 = 环_小;
    当前_小 = 当前_小->下一个 = 新_一元(抽象节点_表达式_语句, 新_变量_节点(新的, 牌_短), 牌_短);

    节点 *节点_小 = 新_节点_(抽象节点_语句_表达式, 牌_短);
    节点_小->体 = 头部.下一个;
    return 节点_小;
  }

  // Convert `A op= B` to ``tmp = &A, *tmp = *tmp op B`.
  对象_短 *变量_短 = 新_本地变量_短("", 指针_到(二元_小写->左手边->类型_缩));

  节点 *expr1 = 新_二元(抽象节点_赋值, 新_变量_节点(变量_短, 牌_短),
                           新_一元(抽象节点_地址, 二元_小写->左手边, 牌_短), 牌_短);

  节点 *expr2 =
    新_二元(抽象节点_赋值,
               新_一元(抽象节点_解引用, 新_变量_节点(变量_短, 牌_短), 牌_短),
               新_二元(二元_小写->种类,
                          新_一元(抽象节点_解引用, 新_变量_节点(变量_短, 牌_短), 牌_短),
                          二元_小写->右手边,
                          牌_短),
               牌_短);

  return 新_二元(抽象节点_逗号, expr1, expr2, 牌_短);
}

// 赋值_    = 有条件的 (赋值_-op 赋值_)?
// 赋值_-op = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&=" | "|=" | "^="
//           | "<<=" | ">>="
static 节点 *赋值_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 有条件的(&牌_短, 牌_短);

  if (等于(牌_短, "="))
    return 新_二元(抽象节点_赋值, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短);

  if (等于(牌_短, "+="))
    return 到_赋值_短(新_加法(节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "-="))
    return 到_赋值_短(新_减法(节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "*="))
    return 到_赋值_短(新_二元(抽象节点_乘, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "/="))
    return 到_赋值_短(新_二元(抽象节点_除, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "%="))
    return 到_赋值_短(新_二元(抽象节点_余数, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "&="))
    return 到_赋值_短(新_二元(抽象节点_位与, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "|="))
    return 到_赋值_短(新_二元(抽象节点_位或, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "^="))
    return 到_赋值_短(新_二元(抽象节点_位异或, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, "<<="))
    return 到_赋值_短(新_二元(抽象节点_左移, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  if (等于(牌_短, ">>="))
    return 到_赋值_短(新_二元(抽象节点_右移, 节点_小, 赋值_(rest, 牌_短->下一个), 牌_短));

  *rest = 牌_短;
  return 节点_小;
}

// 有条件的 = 逻辑或_ ("?" 表达式_短? ":" 有条件的)?
static 节点 *有条件的(牌 **rest, 牌 *牌_短) {
  节点 *条件_短 = 逻辑或_(&牌_短, 牌_短);

  if (!等于(牌_短, "?")) {
    *rest = 牌_短;
    return 条件_短;
  }

  if (等于(牌_短->下一个, ":")) {
    // [GNU] Compile `a ?: b` as `tmp = a, tmp ? tmp : b`.
    添加_类型_小(条件_短);
    对象_短 *变量_短 = 新_本地变量_短("", 条件_短->类型_缩);
    节点 *左手边 = 新_二元(抽象节点_赋值, 新_变量_节点(变量_短, 牌_短), 条件_短, 牌_短);
    节点 *右手边 = 新_节点_(抽象节点_条件, 牌_短);
    右手边->条件_短 = 新_变量_节点(变量_短, 牌_短);
    右手边->那么 = 新_变量_节点(变量_短, 牌_短);
    右手边->否则 = 有条件的(rest, 牌_短->下一个->下一个);
    return 新_二元(抽象节点_逗号, 左手边, 右手边, 牌_短);
  }

  节点 *节点_小 = 新_节点_(抽象节点_条件, 牌_短);
  节点_小->条件_短 = 条件_短;
  节点_小->那么 = 表达式_短(&牌_短, 牌_短->下一个);
  牌_短 = 跳过(牌_短, ":");
  节点_小->否则 = 有条件的(rest, 牌_短);
  return 节点_小;
}

// 逻辑或_ = 逻辑与_ ("||" 逻辑与_)*
static 节点 *逻辑或_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 逻辑与_(&牌_短, 牌_短);
  while (等于(牌_短, "||")) {
    牌 *开启 = 牌_短;
    节点_小 = 新_二元(抽象节点_逻辑或, 节点_小, 逻辑与_(&牌_短, 牌_短->下一个), 开启);
  }
  *rest = 牌_短;
  return 节点_小;
}

// 逻辑与_ = 位或_ ("&&" 位或_)*
static 节点 *逻辑与_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 位或_(&牌_短, 牌_短);
  while (等于(牌_短, "&&")) {
    牌 *开启 = 牌_短;
    节点_小 = 新_二元(抽象节点_逻辑与, 节点_小, 位或_(&牌_短, 牌_短->下一个), 开启);
  }
  *rest = 牌_短;
  return 节点_小;
}

// 位或_ = 位异或_ ("|" 位异或_)*
static 节点 *位或_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 位异或_(&牌_短, 牌_短);
  while (等于(牌_短, "|")) {
    牌 *开启 = 牌_短;
    节点_小 = 新_二元(抽象节点_位或, 节点_小, 位异或_(&牌_短, 牌_短->下一个), 开启);
  }
  *rest = 牌_短;
  return 节点_小;
}

// 位异或_ = 位与_ ("^" 位与_)*
static 节点 *位异或_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 位与_(&牌_短, 牌_短);
  while (等于(牌_短, "^")) {
    牌 *开启 = 牌_短;
    节点_小 = 新_二元(抽象节点_位异或, 节点_小, 位与_(&牌_短, 牌_短->下一个), 开启);
  }
  *rest = 牌_短;
  return 节点_小;
}

// 位与_ = 相等 ("&" 相等)*
static 节点 *位与_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 相等(&牌_短, 牌_短);
  while (等于(牌_短, "&")) {
    牌 *开启 = 牌_短;
    节点_小 = 新_二元(抽象节点_位与, 节点_小, 相等(&牌_短, 牌_短->下一个), 开启);
  }
  *rest = 牌_短;
  return 节点_小;
}

// 相等 = 有关系的 ("==" 有关系的 | "!=" 有关系的)*
static 节点 *相等(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 有关系的(&牌_短, 牌_短);

  for (;;) {
    牌 *开启 = 牌_短;

    if (等于(牌_短, "==")) {
      节点_小 = 新_二元(抽象节点_等于, 节点_小, 有关系的(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, "!=")) {
      节点_小 = 新_二元(抽象节点_不等, 节点_小, 有关系的(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// 有关系的 = 移位 ("<" 移位 | "<=" 移位 | ">" 移位 | ">=" 移位)*
static 节点 *有关系的(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 移位(&牌_短, 牌_短);

  for (;;) {
    牌 *开启 = 牌_短;

    if (等于(牌_短, "<")) {
      节点_小 = 新_二元(抽象节点_小于, 节点_小, 移位(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, "<=")) {
      节点_小 = 新_二元(抽象节点_小等, 节点_小, 移位(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, ">")) {
      节点_小 = 新_二元(抽象节点_小于, 移位(&牌_短, 牌_短->下一个), 节点_小, 开启);
      continue;
    }

    if (等于(牌_短, ">=")) {
      节点_小 = 新_二元(抽象节点_小等, 移位(&牌_短, 牌_短->下一个), 节点_小, 开启);
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// 移位 = 加法_ ("<<" 加法_ | ">>" 加法_)*
static 节点 *移位(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 加法_(&牌_短, 牌_短);

  for (;;) {
    牌 *开启 = 牌_短;

    if (等于(牌_短, "<<")) {
      节点_小 = 新_二元(抽象节点_左移, 节点_小, 加法_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, ">>")) {
      节点_小 = 新_二元(抽象节点_右移, 节点_小, 加法_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// In C, `+` operator is overloaded to perform the pointer arithmetic.
// If p is a pointer, p+n adds not n but sizeof(*p)*n to the value of p,
// so that p+n points to the location n elements (not bytes) ahead of p.
// In other words, we need to scale an integer value before adding to a
// pointer value. This 正函数_ takes care of the scaling.
static 节点 *新_加法(节点 *左手边, 节点 *右手边, 牌 *牌_短) {
  添加_类型_小(左手边);
  添加_类型_小(右手边);

  // num + num
  if (是否_数值型_小(左手边->类型_缩) && 是否_数值型_小(右手边->类型_缩))
    return 新_二元(抽象节点_加, 左手边, 右手边, 牌_短);

  if (左手边->类型_缩->基础 && 右手边->类型_缩->基础)
    错误_牌_短(牌_短, "invalid operands");

  // Canonicalize `num + ptr` to `ptr + num`.
  if (!左手边->类型_缩->基础 && 右手边->类型_缩->基础) {
    节点 *tmp = 左手边;
    左手边 = 右手边;
    右手边 = tmp;
  }

  // VLA + num
  if (左手边->类型_缩->基础->种类 == 类型_变长数组) {
    右手边 = 新_二元(抽象节点_乘, 右手边, 新_变量_节点(左手边->类型_缩->基础->变长数组_大小, 牌_短), 牌_短);
    return 新_二元(抽象节点_加, 左手边, 右手边, 牌_短);
  }

  // ptr + num
  右手边 = 新_二元(抽象节点_乘, 右手边, 新_长的_(左手边->类型_缩->基础->大小, 牌_短), 牌_短);
  return 新_二元(抽象节点_加, 左手边, 右手边, 牌_短);
}

// Like `+`, `-` is overloaded for the pointer type.
static 节点 *新_减法(节点 *左手边, 节点 *右手边, 牌 *牌_短) {
  添加_类型_小(左手边);
  添加_类型_小(右手边);

  // num - num
  if (是否_数值型_小(左手边->类型_缩) && 是否_数值型_小(右手边->类型_缩))
    return 新_二元(抽象节点_减, 左手边, 右手边, 牌_短);

  // VLA + num
  if (左手边->类型_缩->基础->种类 == 类型_变长数组) {
    右手边 = 新_二元(抽象节点_乘, 右手边, 新_变量_节点(左手边->类型_缩->基础->变长数组_大小, 牌_短), 牌_短);
    添加_类型_小(右手边);
    节点 *节点_小 = 新_二元(抽象节点_减, 左手边, 右手边, 牌_短);
    节点_小->类型_缩 = 左手边->类型_缩;
    return 节点_小;
  }

  // ptr - num
  if (左手边->类型_缩->基础 && 是否_整数_小(右手边->类型_缩)) {
    右手边 = 新_二元(抽象节点_乘, 右手边, 新_长的_(左手边->类型_缩->基础->大小, 牌_短), 牌_短);
    添加_类型_小(右手边);
    节点 *节点_小 = 新_二元(抽象节点_减, 左手边, 右手边, 牌_短);
    节点_小->类型_缩 = 左手边->类型_缩;
    return 节点_小;
  }

  // ptr - ptr, which returns how many elements are between the two.
  if (左手边->类型_缩->基础 && 右手边->类型_缩->基础) {
    节点 *节点_小 = 新_二元(抽象节点_减, 左手边, 右手边, 牌_短);
    节点_小->类型_缩 = 类型_长的_小;
    return 新_二元(抽象节点_除, 节点_小, 新_数号_短(左手边->类型_缩->基础->大小, 牌_短), 牌_短);
  }

  错误_牌_短(牌_短, "invalid operands");
}

// 加法_ = 乘法_ ("+" 乘法_ | "-" 乘法_)*
static 节点 *加法_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 乘法_(&牌_短, 牌_短);

  for (;;) {
    牌 *开启 = 牌_短;

    if (等于(牌_短, "+")) {
      节点_小 = 新_加法(节点_小, 乘法_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, "-")) {
      节点_小 = 新_减法(节点_小, 乘法_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// 乘法_ = 类型转换_ ("*" 类型转换_ | "/" 类型转换_ | "%" 类型转换_)*
static 节点 *乘法_(牌 **rest, 牌 *牌_短) {
  节点 *节点_小 = 类型转换_(&牌_短, 牌_短);

  for (;;) {
    牌 *开启 = 牌_短;

    if (等于(牌_短, "*")) {
      节点_小 = 新_二元(抽象节点_乘, 节点_小, 类型转换_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, "/")) {
      节点_小 = 新_二元(抽象节点_除, 节点_小, 类型转换_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    if (等于(牌_短, "%")) {
      节点_小 = 新_二元(抽象节点_余数, 节点_小, 类型转换_(&牌_短, 牌_短->下一个), 开启);
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// 类型转换_ = "(" type-名字 ")" 类型转换_ | 一元_
static 节点 *类型转换_(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, "(") && 是否_类型名(牌_短->下一个)) {
    牌 *开启 = 牌_短;
    类型 *类型_缩 = 类型名(&牌_短, 牌_短->下一个);
    牌_短 = 跳过(牌_短, ")");

    // compound literal
    if (等于(牌_短, "{"))
      return 一元_(rest, 开启);

    // type 类型转换_
    节点 *节点_小 = 新_类型转换(类型转换_(rest, 牌_短), 类型_缩);
    节点_小->牌_短 = 开启;
    return 节点_小;
  }

  return 一元_(rest, 牌_短);
}

// 一元_ = ("+" | "-" | "*" | "&" | "!" | "~") 类型转换_
//       | ("++" | "--") 一元_
//       | "&&" ident
//       | 后缀_
static 节点 *一元_(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, "+"))
    return 类型转换_(rest, 牌_短->下一个);

  if (等于(牌_短, "-"))
    return 新_一元(抽象节点_负, 类型转换_(rest, 牌_短->下一个), 牌_短);

  if (等于(牌_短, "&")) {
    节点 *左手边 = 类型转换_(rest, 牌_短->下一个);
    添加_类型_小(左手边);
    if (左手边->种类 == 抽象节点_成员 && 左手边->成员_小->是否_位字段)
      错误_牌_短(牌_短, "cannot take address of bitfield");
    return 新_一元(抽象节点_地址, 左手边, 牌_短);
  }

  if (等于(牌_短, "*")) {
    // [https://www.sigbus.info/n1570#6.5.3.2p4] This is an oddity
    // in the C spec, but dereferencing a 正函数_ shouldn't do
    // anything. If foo is a 正函数_, `*foo`, `**foo` or `*****foo`
    // are all equivalent to just `foo`.
    节点 *节点_小 = 类型转换_(rest, 牌_短->下一个);
    添加_类型_小(节点_小);
    if (节点_小->类型_缩->种类 == 类型_函数)
      return 节点_小;
    return 新_一元(抽象节点_解引用, 节点_小, 牌_短);
  }

  if (等于(牌_短, "!"))
    return 新_一元(抽象节点_非, 类型转换_(rest, 牌_短->下一个), 牌_短);

  if (等于(牌_短, "~"))
    return 新_一元(抽象节点_位非, 类型转换_(rest, 牌_短->下一个), 牌_短);

  // Read ++i as i+=1
  if (等于(牌_短, "++"))
    return 到_赋值_短(新_加法(一元_(rest, 牌_短->下一个), 新_数号_短(1, 牌_短), 牌_短));

  // Read --i as i-=1
  if (等于(牌_短, "--"))
    return 到_赋值_短(新_减法(一元_(rest, 牌_短->下一个), 新_数号_短(1, 牌_短), 牌_短));

  // [GNU] 标号的-as-values
  if (等于(牌_短, "&&")) {
    节点 *节点_小 = 新_节点_(抽象节点_标号_值, 牌_短);
    节点_小->标号 = 取_标识_短(牌_短->下一个);
    节点_小->跳转_下一个 = 跳转的;
    跳转的 = 节点_小;
    *rest = 牌_短->下一个->下一个;
    return 节点_小;
  }

  return 后缀_(rest, 牌_短);
}

// struct-成员_复数 = (声明规格 声明符 (","  声明符)* ";")*
static void 结构_成员_复(牌 **rest, 牌 *牌_短, 类型 *类型_缩) {
  成员 头部 = {};
  成员 *当前_小 = &头部;
  int 索引_缩 = 0;

  while (!等于(牌_短, "}")) {
    变量基本属性_短 attr = {};
    类型 *基本类型 = 声明规格(&牌_短, 牌_短, &attr);
    bool first = true;

    // Anonymous struct 成员_小
    if ((基本类型->种类 == 类型_结构 || 基本类型->种类 == 类型_联合) &&
        消耗(&牌_短, 牌_短, ";")) {
      成员 *成员_短 = calloc(1, sizeof(成员));
      成员_短->类型_缩 = 基本类型;
      成员_短->索引_缩 = 索引_缩++;
      成员_短->对齐_短 = attr.对齐_短 ? attr.对齐_短 : 成员_短->类型_缩->对齐_短;
      当前_小 = 当前_小->下一个 = 成员_短;
      continue;
    }

    // Regular struct 成员_复数
    while (!消耗(&牌_短, 牌_短, ";")) {
      if (!first)
        牌_短 = 跳过(牌_短, ",");
      first = false;

      成员 *成员_短 = calloc(1, sizeof(成员));
      成员_短->类型_缩 = 声明符(&牌_短, 牌_短, 基本类型);
      成员_短->名字 = 成员_短->类型_缩->名字;
      成员_短->索引_缩 = 索引_缩++;
      成员_短->对齐_短 = attr.对齐_短 ? attr.对齐_短 : 成员_短->类型_缩->对齐_短;

      if (消耗(&牌_短, 牌_短, ":")) {
        成员_短->是否_位字段 = true;
        成员_短->位_宽度 = 常量_表达式(&牌_短, 牌_短);
      }

      当前_小 = 当前_小->下一个 = 成员_短;
    }
  }

  // If the last element is an array of incomplete type, it's
  // called a "flexible array 成员_小". It should behave as if
  // if were a zero-sized array.
  if (当前_小 != &头部 && 当前_小->类型_缩->种类 == 类型_数组 && 当前_小->类型_缩->数组_长度 < 0) {
    当前_小->类型_缩 = 数组_阵列(当前_小->类型_缩->基础, 0);
    类型_缩->是否_有弹性 = true;
  }

  *rest = 牌_短->下一个;
  类型_缩->成员_复数 = 头部.下一个;
}

// attribute = ("__attribute__" "(" "(" "packed" ")" ")")*
static 牌 *属性特质_列表(牌 *牌_短, 类型 *类型_缩) {
  while (消耗(&牌_短, 牌_短, "__attribute__")) {
    牌_短 = 跳过(牌_短, "(");
    牌_短 = 跳过(牌_短, "(");

    bool first = true;

    while (!消耗(&牌_短, 牌_短, ")")) {
      if (!first)
        牌_短 = 跳过(牌_短, ",");
      first = false;

      if (消耗(&牌_短, 牌_短, "packed")) {
        类型_缩->是否_已打包 = true;
        continue;
      }

      if (消耗(&牌_短, 牌_短, "aligned")) {
        牌_短 = 跳过(牌_短, "(");
        类型_缩->对齐_短 = 常量_表达式(&牌_短, 牌_短);
        牌_短 = 跳过(牌_短, ")");
        continue;
      }

      错误_牌_短(牌_短, "unknown attribute");
    }

    牌_短 = 跳过(牌_短, ")");
  }

  return 牌_短;
}

// struct-union-decl = attribute? ident? ("{" struct-成员_复数)?
static 类型 *结构_联合_声明_短(牌 **rest, 牌 *牌_短) {
  类型 *类型_缩 = 结构_类型_小();
  牌_短 = 属性特质_列表(牌_短, 类型_缩);

  // Read a tag.
  牌 *tag = NULL;
  if (牌_短->种类 == 牌_缩_标识符) {
    tag = 牌_短;
    牌_短 = 牌_短->下一个;
  }

  if (tag && !等于(牌_短, "{")) {
    *rest = 牌_短;

    类型 *ty2 = 找_标签(tag);
    if (ty2)
      return ty2;

    类型_缩->大小 = -1;
    推_标签_作用域(tag, 类型_缩);
    return 类型_缩;
  }

  牌_短 = 跳过(牌_短, "{");

  // Construct a struct object.
  结构_成员_复(&牌_短, 牌_短, 类型_缩);
  *rest = 属性特质_列表(牌_短, 类型_缩);

  if (tag) {
    // If this is a redefinition, overwrite a previous type.
    // Otherwise, register the struct type.
    类型 *ty2 = 哈希映射_取2(&作用域_小写->标签_短, tag->位置_短, tag->长度_短);
    if (ty2) {
      *ty2 = *类型_缩;
      return ty2;
    }

    推_标签_作用域(tag, 类型_缩);
  }

  return 类型_缩;
}

// struct-decl = struct-union-decl
static 类型 *结构_声明(牌 **rest, 牌 *牌_短) {
  类型 *类型_缩 = 结构_联合_声明_短(rest, 牌_短);
  类型_缩->种类 = 类型_结构;

  if (类型_缩->大小 < 0)
    return 类型_缩;

  // Assign offsets within the struct to 成员_复数.
  int bits = 0;

  for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
    if (成员_短->是否_位字段 && 成员_短->位_宽度 == 0) {
      // Zero-width anonymous bitfield has a special meaning.
      // It affects only alignment.
      bits = 对齐_到(bits, 成员_短->类型_缩->大小 * 8);
    } else if (成员_短->是否_位字段) {
      int sz = 成员_短->类型_缩->大小;
      if (bits / (sz * 8) != (bits + 成员_短->位_宽度 - 1) / (sz * 8))
        bits = 对齐_到(bits, sz * 8);

      成员_短->偏移 = 对齐_下(bits / 8, sz);
      成员_短->位_偏移 = bits % (sz * 8);
      bits += 成员_短->位_宽度;
    } else {
      if (!类型_缩->是否_已打包)
        bits = 对齐_到(bits, 成员_短->对齐_短 * 8);
      成员_短->偏移 = bits / 8;
      bits += 成员_短->类型_缩->大小 * 8;
    }

    if (!类型_缩->是否_已打包 && 类型_缩->对齐_短 < 成员_短->对齐_短)
      类型_缩->对齐_短 = 成员_短->对齐_短;
  }

  类型_缩->大小 = 对齐_到(bits, 类型_缩->对齐_短 * 8) / 8;
  return 类型_缩;
}

// union-decl = struct-union-decl
static 类型 *联合_声明(牌 **rest, 牌 *牌_短) {
  类型 *类型_缩 = 结构_联合_声明_短(rest, 牌_短);
  类型_缩->种类 = 类型_联合;

  if (类型_缩->大小 < 0)
    return 类型_缩;

  // If union, we don't have to 赋值_ offsets because they
  // are already initialized to zero. We need to compute the
  // alignment and the 大小 though.
  for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
    if (类型_缩->对齐_短 < 成员_短->对齐_短)
      类型_缩->对齐_短 = 成员_短->对齐_短;
    if (类型_缩->大小 < 成员_短->类型_缩->大小)
      类型_缩->大小 = 成员_短->类型_缩->大小;
  }
  类型_缩->大小 = 对齐_到(类型_缩->大小, 类型_缩->对齐_短);
  return 类型_缩;
}

// Find a struct 成员_小 by 名字.
static 成员 *取_结构_成员(类型 *类型_缩, 牌 *牌_短) {
  for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个) {
    // Anonymous struct 成员_小
    if ((成员_短->类型_缩->种类 == 类型_结构 || 成员_短->类型_缩->种类 == 类型_联合) &&
        !成员_短->名字) {
      if (取_结构_成员(成员_短->类型_缩, 牌_短))
        return 成员_短;
      continue;
    }

    // Regular struct 成员_小
    if (成员_短->名字->长度_短 == 牌_短->长度_短 &&
        !strncmp(成员_短->名字->位置_短, 牌_短->位置_短, 牌_短->长度_短))
      return 成员_短;
  }
  return NULL;
}

// Create a 节点_小 representing a struct 成员_小 access, such as foo.bar
// where foo is a struct and bar is a 成员_小 名字.
//
// C has a feature called "anonymous struct" which allows a struct to
// have another unnamed struct as a 成员_小 like this:
//
//   struct { struct { int a; }; int b; } x;
//
// The 成员_复数 of an anonymous struct belong to the outer struct's
// 成员_小 namespace. Therefore, in the above example, you can access
// 成员_小 "a" of the anonymous struct as "x.a".
//
// This 正函数_ takes care of anonymous structs.
static 节点 *结构_引用_短(节点 *节点_小, 牌 *牌_短) {
  添加_类型_小(节点_小);
  if (节点_小->类型_缩->种类 != 类型_结构 && 节点_小->类型_缩->种类 != 类型_联合)
    错误_牌_短(节点_小->牌_短, "not a struct nor a union");

  类型 *类型_缩 = 节点_小->类型_缩;

  for (;;) {
    成员 *成员_短 = 取_结构_成员(类型_缩, 牌_短);
    if (!成员_短)
      错误_牌_短(牌_短, "no such 成员_小");
    节点_小 = 新_一元(抽象节点_成员, 节点_小, 牌_短);
    节点_小->成员_小 = 成员_短;
    if (成员_短->名字)
      break;
    类型_缩 = 成员_短->类型_缩;
  }
  return 节点_小;
}

// Convert A++ to `(typeof A)((A += 1) - 1)`
static 节点 *新_自增_自减_短(节点 *节点_小, 牌 *牌_短, int 附加物) {
  添加_类型_小(节点_小);
  return 新_类型转换(新_加法(到_赋值_短(新_加法(节点_小, 新_数号_短(附加物, 牌_短), 牌_短)),
                          新_数号_短(-附加物, 牌_短), 牌_短),
                  节点_小->类型_缩);
}

// 后缀_ = "(" type-名字 ")" "{" 初始化器_小写-list "}"
//         = ident "(" func-实参_短 ")" 后缀_-tail*
//         | 最初的 后缀_-tail*
//
// 后缀_-tail = "[" 表达式_短 "]"
//              | "(" func-实参_短 ")"
//              | "." ident
//              | "->" ident
//              | "++"
//              | "--"
static 节点 *后缀_(牌 **rest, 牌 *牌_短) {
  if (等于(牌_短, "(") && 是否_类型名(牌_短->下一个)) {
    // Compound literal
    牌 *开启 = 牌_短;
    类型 *类型_缩 = 类型名(&牌_短, 牌_短->下一个);
    牌_短 = 跳过(牌_短, ")");

    if (作用域_小写->下一个 == NULL) {
      对象_短 *变量_短 = 新_匿名_全局变量_短(类型_缩);
      全局变量_初始化器(rest, 牌_短, 变量_短);
      return 新_变量_节点(变量_短, 开启);
    }

    对象_短 *变量_短 = 新_本地变量_短("", 类型_缩);
    节点 *左手边 = 本地变量_初始化器(rest, 牌_短, 变量_短);
    节点 *右手边 = 新_变量_节点(变量_短, 牌_短);
    return 新_二元(抽象节点_逗号, 左手边, 右手边, 开启);
  }

  节点 *节点_小 = 最初的(&牌_短, 牌_短);

  for (;;) {
    if (等于(牌_短, "(")) {
      节点_小 = 函数调用_(&牌_短, 牌_短->下一个, 节点_小);
      continue;
    }

    if (等于(牌_短, "[")) {
      // x[y] is short for *(x+y)
      牌 *开启 = 牌_短;
      节点 *索引_缩 = 表达式_短(&牌_短, 牌_短->下一个);
      牌_短 = 跳过(牌_短, "]");
      节点_小 = 新_一元(抽象节点_解引用, 新_加法(节点_小, 索引_缩, 开启), 开启);
      continue;
    }

    if (等于(牌_短, ".")) {
      节点_小 = 结构_引用_短(节点_小, 牌_短->下一个);
      牌_短 = 牌_短->下一个->下一个;
      continue;
    }

    if (等于(牌_短, "->")) {
      // x->y is short for (*x).y
      节点_小 = 新_一元(抽象节点_解引用, 节点_小, 牌_短);
      节点_小 = 结构_引用_短(节点_小, 牌_短->下一个);
      牌_短 = 牌_短->下一个->下一个;
      continue;
    }

    if (等于(牌_短, "++")) {
      节点_小 = 新_自增_自减_短(节点_小, 牌_短, 1);
      牌_短 = 牌_短->下一个;
      continue;
    }

    if (等于(牌_短, "--")) {
      节点_小 = 新_自增_自减_短(节点_小, 牌_短, -1);
      牌_短 = 牌_短->下一个;
      continue;
    }

    *rest = 牌_短;
    return 节点_小;
  }
}

// 函数调用_ = (赋值_ ("," 赋值_)*)? ")"
static 节点 *函数调用_(牌 **rest, 牌 *牌_短, 节点 *fn) {
  添加_类型_小(fn);

  if (fn->类型_缩->种类 != 类型_函数 &&
      (fn->类型_缩->种类 != 类型_指针 || fn->类型_缩->基础->种类 != 类型_函数))
    错误_牌_短(fn->牌_短, "not a 正函数_");

  类型 *类型_缩 = (fn->类型_缩->种类 == 类型_函数) ? fn->类型_缩 : fn->类型_缩->基础;
  类型 *形参_类型_短 = 类型_缩->形参_短;

  节点 头部 = {};
  节点 *当前_小 = &头部;

  while (!等于(牌_短, ")")) {
    if (当前_小 != &头部)
      牌_短 = 跳过(牌_短, ",");

    节点 *arg = 赋值_(&牌_短, 牌_短);
    添加_类型_小(arg);

    if (!形参_类型_短 && !类型_缩->是否_可变参数)
      错误_牌_短(牌_短, "too many arguments");

    if (形参_类型_短) {
      if (形参_类型_短->种类 != 类型_结构 && 形参_类型_短->种类 != 类型_联合)
        arg = 新_类型转换(arg, 形参_类型_短);
      形参_类型_短 = 形参_类型_短->下一个;
    } else if (arg->类型_缩->种类 == 类型_浮点) {
      // If parameter type is omitted (e.g. in "..."), float
      // arguments are promoted to double.
      arg = 新_类型转换(arg, 类型_双浮_小);
    }

    当前_小 = 当前_小->下一个 = arg;
  }

  if (形参_类型_短)
    错误_牌_短(牌_短, "too few arguments");

  *rest = 跳过(牌_短, ")");

  节点 *节点_小 = 新_一元(抽象节点_函数调用, fn, 牌_短);
  节点_小->函数_类型 = 类型_缩;
  节点_小->类型_缩 = 类型_缩->返回_类型;
  节点_小->实参_短 = 头部.下一个;

  // If a 正函数_ returns a struct, it is caller's responsibility
  // to allocate a space for the return value.
  if (节点_小->类型_缩->种类 == 类型_结构 || 节点_小->类型_缩->种类 == 类型_联合)
    节点_小->返回_缓冲区 = 新_本地变量_短("", 节点_小->类型_缩);
  return 节点_小;
}

// generic-selection = "(" 赋值_ "," generic-assoc ("," generic-assoc)* ")"
//
// generic-assoc = type-名字 ":" 赋值_
//               | "default" ":" 赋值_
static 节点 *通用的_选择(牌 **rest, 牌 *牌_短) {
  牌 *开启 = 牌_短;
  牌_短 = 跳过(牌_短, "(");

  节点 *ctrl = 赋值_(&牌_短, 牌_短);
  添加_类型_小(ctrl);

  类型 *t1 = ctrl->类型_缩;
  if (t1->种类 == 类型_函数)
    t1 = 指针_到(t1);
  else if (t1->种类 == 类型_数组)
    t1 = 指针_到(t1->基础);

  节点 *ret = NULL;

  while (!消耗(rest, 牌_短, ")")) {
    牌_短 = 跳过(牌_短, ",");

    if (等于(牌_短, "default")) {
      牌_短 = 跳过(牌_短->下一个, ":");
      节点 *节点_小 = 赋值_(&牌_短, 牌_短);
      if (!ret)
        ret = 节点_小;
      continue;
    }

    类型 *t2 = 类型名(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ":");
    节点 *节点_小 = 赋值_(&牌_短, 牌_短);
    if (是否_兼容_小(t1, t2))
      ret = 节点_小;
  }

  if (!ret)
    错误_牌_短(开启, "controlling expression type not compatible with"
              " any generic association type");
  return ret;
}

// 最初的 = "(" "{" 语句_缩+ "}" ")"
//         | "(" 表达式_短 ")"
//         | "sizeof" "(" type-名字 ")"
//         | "sizeof" 一元_
//         | "_Alignof" "(" type-名字 ")"
//         | "_Alignof" 一元_
//         | "_Generic" generic-selection
//         | "__builtin_types_compatible_p" "(" type-名字, type-名字, ")"
//         | "__builtin_reg_class" "(" type-名字 ")"
//         | ident
//         | 串_短
//         | num
static 节点 *最初的(牌 **rest, 牌 *牌_短) {
  牌 *开启 = 牌_短;

  if (等于(牌_短, "(") && 等于(牌_短->下一个, "{")) {
    // This is a GNU statement expresssion.
    节点 *节点_小 = 新_节点_(抽象节点_语句_表达式, 牌_短);
    节点_小->体 = 复合的_语句(&牌_短, 牌_短->下一个->下一个)->体;
    *rest = 跳过(牌_短, ")");
    return 节点_小;
  }

  if (等于(牌_短, "(")) {
    节点 *节点_小 = 表达式_短(&牌_短, 牌_短->下一个);
    *rest = 跳过(牌_短, ")");
    return 节点_小;
  }

  if (等于(牌_短, "sizeof") && 等于(牌_短->下一个, "(") && 是否_类型名(牌_短->下一个->下一个)) {
    类型 *类型_缩 = 类型名(&牌_短, 牌_短->下一个->下一个);
    *rest = 跳过(牌_短, ")");

    if (类型_缩->种类 == 类型_变长数组) {
      if (类型_缩->变长数组_大小)
        return 新_变量_节点(类型_缩->变长数组_大小, 牌_短);

      节点 *左手边 = 计算_变长数组_大小(类型_缩, 牌_短);
      节点 *右手边 = 新_变量_节点(类型_缩->变长数组_大小, 牌_短);
      return 新_二元(抽象节点_逗号, 左手边, 右手边, 牌_短);
    }

    return 新_无符号长的(类型_缩->大小, 开启);
  }

  if (等于(牌_短, "sizeof")) {
    节点 *节点_小 = 一元_(rest, 牌_短->下一个);
    添加_类型_小(节点_小);
    if (节点_小->类型_缩->种类 == 类型_变长数组)
      return 新_变量_节点(节点_小->类型_缩->变长数组_大小, 牌_短);
    return 新_无符号长的(节点_小->类型_缩->大小, 牌_短);
  }

  if (等于(牌_短, "_Alignof") && 等于(牌_短->下一个, "(") && 是否_类型名(牌_短->下一个->下一个)) {
    类型 *类型_缩 = 类型名(&牌_短, 牌_短->下一个->下一个);
    *rest = 跳过(牌_短, ")");
    return 新_无符号长的(类型_缩->对齐_短, 牌_短);
  }

  if (等于(牌_短, "_Alignof")) {
    节点 *节点_小 = 一元_(rest, 牌_短->下一个);
    添加_类型_小(节点_小);
    return 新_无符号长的(节点_小->类型_缩->对齐_短, 牌_短);
  }

  if (等于(牌_短, "_Generic"))
    return 通用的_选择(rest, 牌_短->下一个);

  if (等于(牌_短, "__builtin_types_compatible_p")) {
    牌_短 = 跳过(牌_短->下一个, "(");
    类型 *t1 = 类型名(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ",");
    类型 *t2 = 类型名(&牌_短, 牌_短);
    *rest = 跳过(牌_短, ")");
    return 新_数号_短(是否_兼容_小(t1, t2), 开启);
  }

  if (等于(牌_短, "__builtin_reg_class")) {
    牌_短 = 跳过(牌_短->下一个, "(");
    类型 *类型_缩 = 类型名(&牌_短, 牌_短);
    *rest = 跳过(牌_短, ")");

    if (是否_整数_小(类型_缩) || 类型_缩->种类 == 类型_指针)
      return 新_数号_短(0, 开启);
    if (是否_浮点数_小(类型_缩))
      return 新_数号_短(1, 开启);
    return 新_数号_短(2, 开启);
  }

  if (等于(牌_短, "__builtin_compare_and_swap")) {
    节点 *节点_小 = 新_节点_(抽象节点_对比与交换, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");
    节点_小->对比与交换_地址 = 赋值_(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ",");
    节点_小->对比与交换_旧 = 赋值_(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ",");
    节点_小->对比与交换_新 = 赋值_(&牌_短, 牌_短);
    *rest = 跳过(牌_短, ")");
    return 节点_小;
  }

  if (等于(牌_短, "__builtin_atomic_exchange")) {
    节点 *节点_小 = 新_节点_(抽象节点_互换, 牌_短);
    牌_短 = 跳过(牌_短->下一个, "(");
    节点_小->左手边 = 赋值_(&牌_短, 牌_短);
    牌_短 = 跳过(牌_短, ",");
    节点_小->右手边 = 赋值_(&牌_短, 牌_短);
    *rest = 跳过(牌_短, ")");
    return 节点_小;
  }

  if (牌_短->种类 == 牌_缩_标识符) {
    // Variable or enum constant
    变量作用域 *sc = 找_变量_短(牌_短);
    *rest = 牌_短->下一个;

    // For "static inline" 正函数_
    if (sc && sc->变量_短 && sc->变量_短->是否_函数) {
      if (当前_函)
        串数组_推(&当前_函->引用_短, sc->变量_短->名字);
      else
        sc->变量_短->是否_根 = true;
    }

    if (sc) {
      if (sc->变量_短)
        return 新_变量_节点(sc->变量_短, 牌_短);
      if (sc->枚举_类型_短)
        return 新_数号_短(sc->枚举_值_短, 牌_短);
    }

    if (等于(牌_短->下一个, "("))
      错误_牌_短(牌_短, "implicit 正声明 of a 正函数_");
    错误_牌_短(牌_短, "undefined variable");
  }

  if (牌_短->种类 == 牌_缩_串字面) {
    对象_短 *变量_短 = 新_字符串_字面(牌_短->串_短, 牌_短->类型_缩);
    *rest = 牌_短->下一个;
    return 新_变量_节点(变量_短, 牌_短);
  }

  if (牌_短->种类 == 牌_缩_数值字面) {
    节点 *节点_小;
    if (是否_浮点数_小(牌_短->类型_缩)) {
      节点_小 = 新_节点_(抽象节点_整数, 牌_短);
      节点_小->浮点值_短 = 牌_短->浮点值_短;
    } else {
      节点_小 = 新_数号_短(牌_短->值_短, 牌_短);
    }

    节点_小->类型_缩 = 牌_短->类型_缩;
    *rest = 牌_短->下一个;
    return 节点_小;
  }

  错误_牌_短(牌_短, "expected an expression");
}

static 牌 *解析_类型定义(牌 *牌_短, 类型 *基本类型) {
  bool first = true;

  while (!消耗(&牌_短, 牌_短, ";")) {
    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    类型 *类型_缩 = 声明符(&牌_短, 牌_短, 基本类型);
    if (!类型_缩->名字)
      错误_牌_短(类型_缩->名字_位置, "typedef 名字 omitted");
    推_作用域(取_标识_短(类型_缩->名字))->类型_定义_短 = 类型_缩;
  }
  return 牌_短;
}

static void 创建_形参_本地变量_短(类型 *param) {
  if (param) {
    创建_形参_本地变量_短(param->下一个);
    if (!param->名字)
      错误_牌_短(param->名字_位置, "parameter 名字 omitted");
    新_本地变量_短(取_标识_短(param->名字), param);
  }
}

// This 正函数_ matches 跳转的 or 标号的-as-values with 标号的.
//
// We cannot resolve 跳转的 as we 解析 a 正函数_ because 跳转的
// can refer a 标号 that appears later in the 正函数_.
// So, we need to do this after we 解析 the entire 正函数_.
static void 决议_跳转_标号(void) {
  for (节点 *x = 跳转的; x; x = x->跳转_下一个) {
    for (节点 *y = 标号的; y; y = y->跳转_下一个) {
      if (!strcmp(x->标号, y->标号)) {
        x->独特性_标号 = y->独特性_标号;
        break;
      }
    }

    if (x->独特性_标号 == NULL)
      错误_牌_短(x->牌_短->下一个, "use of undeclared 标号");
  }

  跳转的 = 标号的 = NULL;
}

static 对象_短 *找_函数_短(char *名字) {
  作用域 *sc = 作用域_小写;
  while (sc->下一个)
    sc = sc->下一个;

  变量作用域 *sc2 = 哈希映射_取(&sc->变量_短, 名字);
  if (sc2 && sc2->变量_短 && sc2->变量_短->是否_函数)
    return sc2->变量_短;
  return NULL;
}

static void 做标记_活的(对象_短 *变量_短) {
  if (!变量_短->是否_函数 || 变量_短->是否_活着)
    return;
  变量_短->是否_活着 = true;

  for (int i = 0; i < 变量_短->引用_短.长度_短; i++) {
    对象_短 *fn = 找_函数_短(变量_短->引用_短.数据[i]);
    if (fn)
      做标记_活的(fn);
  }
}

static 牌 *正函数_(牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr) {
  类型 *类型_缩 = 声明符(&牌_短, 牌_短, 基本类型);
  if (!类型_缩->名字)
    错误_牌_短(类型_缩->名字_位置, "正函数_ 名字 omitted");
  char *name_str = 取_标识_短(类型_缩->名字);

  对象_短 *fn = 找_函数_短(name_str);
  if (fn) {
    // Redeclaration
    if (!fn->是否_函数)
      错误_牌_短(牌_短, "redeclared as a different 种类 of symbol");
    if (fn->是否_定义了 && 等于(牌_短, "{"))
      错误_牌_短(牌_短, "redefinition of %s", name_str);
    if (!fn->是否_静态 && attr->是否_静态)
      错误_牌_短(牌_短, "static 正声明 follows a non-static 正声明");
    fn->是否_定义了 = fn->是否_定义了 || 等于(牌_短, "{");
  } else {
    fn = 新_全局变量_短(name_str, 类型_缩);
    fn->是否_函数 = true;
    fn->是否_定义了 = 等于(牌_短, "{");
    fn->是否_静态 = attr->是否_静态 || (attr->是否_内联 && !attr->是否_外部);
    fn->是否_内联 = attr->是否_内联;
  }

  fn->是否_根 = !(fn->是否_静态 && fn->是否_内联);

  if (消耗(&牌_短, 牌_短, ";"))
    return 牌_短;

  当前_函 = fn;
  本地 = NULL;
  进入_作用域();
  创建_形参_本地变量_短(类型_缩->形参_短);

  // A buffer for a struct/union return value is passed
  // as the hidden first parameter.
  类型 *rty = 类型_缩->返回_类型;
  if ((rty->种类 == 类型_结构 || rty->种类 == 类型_联合) && rty->大小 > 16)
    新_本地变量_短("", 指针_到(rty));

  fn->形参_短 = 本地;

  if (类型_缩->是否_可变参数)
    fn->va_区域 = 新_本地变量_短("__va_area__", 数组_阵列(类型_字符_小, 136));
  fn->动态分配_底部 = 新_本地变量_短("__alloca_size__", 指针_到(类型_字符_小));

  牌_短 = 跳过(牌_短, "{");

  // [https://www.sigbus.info/n1570#6.4.2.2p1] "__func__" is
  // automatically defined as a local variable containing the
  // current 正函数_ 名字.
  推_作用域("__func__")->变量_短 =
    新_字符串_字面(fn->名字, 数组_阵列(类型_字符_小, strlen(fn->名字) + 1));

  // [GNU] __FUNCTION__ is yet another 名字 of __func__.
  推_作用域("__FUNCTION__")->变量_短 =
    新_字符串_字面(fn->名字, 数组_阵列(类型_字符_小, strlen(fn->名字) + 1));

  fn->体 = 复合的_语句(&牌_短, 牌_短);
  fn->本地 = 本地;
  离开_作用域();
  决议_跳转_标号();
  return 牌_短;
}

static 牌 *全局_变量_(牌 *牌_短, 类型 *基本类型, 变量基本属性_短 *attr) {
  bool first = true;

  while (!消耗(&牌_短, 牌_短, ";")) {
    if (!first)
      牌_短 = 跳过(牌_短, ",");
    first = false;

    类型 *类型_缩 = 声明符(&牌_短, 牌_短, 基本类型);
    if (!类型_缩->名字)
      错误_牌_短(类型_缩->名字_位置, "variable 名字 omitted");

    对象_短 *变量_短 = 新_全局变量_短(取_标识_短(类型_缩->名字), 类型_缩);
    变量_短->是否_定义了 = !attr->是否_外部;
    变量_短->是否_静态 = attr->是否_静态;
    变量_短->is_tls = attr->is_tls;
    if (attr->对齐_短)
      变量_短->对齐_短 = attr->对齐_短;

    if (等于(牌_短, "="))
      全局变量_初始化器(&牌_短, 牌_短->下一个, 变量_短);
    else if (!attr->是否_外部 && !attr->is_tls)
      变量_短->是否_试探性的 = true;
  }
  return 牌_短;
}

// Lookahead tokens and returns true if a given token is a 开启
// of a 正函数_ definition or 正声明.
static bool 是否_函数(牌 *牌_短) {
  if (等于(牌_短, ";"))
    return false;

  类型 假人 = {};
  类型 *类型_缩 = 声明符(&牌_短, 牌_短, &假人);
  return 类型_缩->种类 == 类型_函数;
}

// Remove redundant tentative definitions.
static void 扫描_全局(void) {
  对象_短 头部;
  对象_短 *当前_小 = &头部;

  for (对象_短 *变量_短 = 全局的; 变量_短; 变量_短 = 变量_短->下一个) {
    if (!变量_短->是否_试探性的) {
      当前_小 = 当前_小->下一个 = 变量_短;
      continue;
    }

    // Find another definition of the same identifier.
    对象_短 *var2 = 全局的;
    for (; var2; var2 = var2->下一个)
      if (变量_短 != var2 && var2->是否_定义了 && !strcmp(变量_短->名字, var2->名字))
        break;

    // If there's another definition, the tentative definition
    // is redundant
    if (!var2)
      当前_小 = 当前_小->下一个 = 变量_短;
  }

  当前_小->下一个 = NULL;
  全局的 = 头部.下一个;
}

static void 声明_内建_函数(void) {
  类型 *类型_缩 = 函数_类型_小(指针_到(类型_空的_小));
  类型_缩->形参_短 = 复制_类型(类型_整型_小);
  内建_分配a = 新_全局变量_短("alloca", 类型_缩);
  内建_分配a->是否_定义了 = false;
}

// program = (typedef | 正函数_-definition | global-variable)*
对象_短 *解析(牌 *牌_短) {
  声明_内建_函数();
  全局的 = NULL;

  while (牌_短->种类 != 牌_缩_文件终) {
    变量基本属性_短 attr = {};
    类型 *基本类型 = 声明规格(&牌_短, 牌_短, &attr);

    // Typedef
    if (attr.是否_类型定义) {
      牌_短 = 解析_类型定义(牌_短, 基本类型);
      continue;
    }

    // Function
    if (是否_函数(牌_短)) {
      牌_短 = 正函数_(牌_短, 基本类型, &attr);
      continue;
    }

    // Global variable
    牌_短 = 全局_变量_(牌_短, 基本类型, &attr);
  }

  for (对象_短 *变量_短 = 全局的; 变量_短; 变量_短 = 变量_短->下一个)
    if (变量_短->是否_根)
      做标记_活的(变量_短);

  // Remove redundant tentative definitions.
  扫描_全局();
  return 全局的;
}
