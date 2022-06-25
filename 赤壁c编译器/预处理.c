// This 源文件位置 implements the C preprocessor.
//
// The preprocessor takes a list of tokens as an 输入 and returns a
// 新的 list of tokens as an 输出_.
//
// The preprocessing language is designed in such a way that that's
// guaranteed to stop even if there is a recursive macro.
// Informally speaking, a macro is applied only once for each token.
// That is, if a macro token T appears in a result of direct or
// indirect macro expansion of T, T won't be expanded any further.
// For example, if T is defined as U, and U is defined as T, 那么
// token T is expanded to U and 那么 to T and the macro expansion
// stops at that point.
//
// To achieve the above behavior, we attach for each token a set of
// macro names from which the token is expanded. The set is called
// "隐藏集_小". 隐藏集 is initially empty, and every time we expand a
// macro, the macro 名字 is added to the resulting tokens' hidesets.
//
// The above macro expansion algorithm is explained in this document
// written by Dave Prossor, which is 已用 as a basis for the
// standard's wording:
// https://github.com/rui314/chibicc/wiki/cpp.algo.pdf

#include "chibicc.h"

typedef struct 宏形参 宏形参;
struct 宏形参 {
  宏形参 *下一个;
  char *名字;
};

typedef struct 宏实参 宏实参;
struct 宏实参 {
  宏实参 *下一个;
  char *名字;
  bool is_va_args;
  牌 *牌_短;
};

typedef 牌 *宏_手握者_fn(牌 *);

typedef struct 宏 宏;
struct 宏 {
  char *名字;
  bool is_objlike; // Object-like or 正函数_-like
  宏形参 *形参_短;
  char *va_args_name;
  牌 *体;
  宏_手握者_fn *手握者;
};

// `#if` can be nested, so we use a 栈_小写 to manage nested `#if`s.
typedef struct 条件包括_短 条件包括_短;
struct 条件包括_短 {
  条件包括_短 *下一个;
  enum { IN_THEN, IN_ELIF, IN_ELSE } ctx;
  牌 *牌_短;
  bool 已包括;
};

typedef struct 隐藏集 隐藏集;
struct 隐藏集 {
  隐藏集 *下一个;
  char *名字;
};

static 哈希映射 宏_小写;
static 条件包括_短 *条件_包括_短;
static 哈希映射 杂注_一次;
static int 包括_下一个_索引;

static 牌 *预处理2(牌 *牌_短);
static 宏 *找_宏(牌 *牌_短);

static bool 是否_哈希(牌 *牌_短) {
  return 牌_短->位置_行开始 && 等于(牌_短, "#");
}

// Some preprocessor directives such as #include allow extraneous
// tokens before newline. This 正函数_ skips such tokens.
static 牌 *跳过_行(牌 *牌_短) {
  if (牌_短->位置_行开始)
    return 牌_短;
  警告_牌_短(牌_短, "extra token");
  while (牌_短->位置_行开始)
    牌_短 = 牌_短->下一个;
  return 牌_短;
}

static 牌 *复制_牌(牌 *牌_短) {
  牌 *t = calloc(1, sizeof(牌));
  *t = *牌_短;
  t->下一个 = NULL;
  return t;
}

static 牌 *新_文件终(牌 *牌_短) {
  牌 *t = 复制_牌(牌_短);
  t->种类 = 牌_缩_文件终;
  t->长度_短 = 0;
  return t;
}

static 隐藏集 *新_隐藏集(char *名字) {
  隐藏集 *hs = calloc(1, sizeof(隐藏集));
  hs->名字 = 名字;
  return hs;
}

static 隐藏集 *隐藏集_联合(隐藏集 *hs1, 隐藏集 *hs2) {
  隐藏集 头部 = {};
  隐藏集 *当前_小 = &头部;

  for (; hs1; hs1 = hs1->下一个)
    当前_小 = 当前_小->下一个 = 新_隐藏集(hs1->名字);
  当前_小->下一个 = hs2;
  return 头部.下一个;
}

static bool 隐藏集_内容(隐藏集 *hs, char *s, int 长度_短) {
  for (; hs; hs = hs->下一个)
    if (strlen(hs->名字) == 长度_短 && !strncmp(hs->名字, s, 长度_短))
      return true;
  return false;
}

static 隐藏集 *隐藏集_交叉点(隐藏集 *hs1, 隐藏集 *hs2) {
  隐藏集 头部 = {};
  隐藏集 *当前_小 = &头部;

  for (; hs1; hs1 = hs1->下一个)
    if (隐藏集_内容(hs2, hs1->名字, strlen(hs1->名字)))
      当前_小 = 当前_小->下一个 = 新_隐藏集(hs1->名字);
  return 头部.下一个;
}

static 牌 *添加_隐藏集(牌 *牌_短, 隐藏集 *hs) {
  牌 头部 = {};
  牌 *当前_小 = &头部;

  for (; 牌_短; 牌_短 = 牌_短->下一个) {
    牌 *t = 复制_牌(牌_短);
    t->隐藏集_小 = 隐藏集_联合(t->隐藏集_小, hs);
    当前_小 = 当前_小->下一个 = t;
  }
  return 头部.下一个;
}

// Append tok2 to the 终 of tok1.
static 牌 *追加(牌 *tok1, 牌 *tok2) {
  if (tok1->种类 == 牌_缩_文件终)
    return tok2;

  牌 头部 = {};
  牌 *当前_小 = &头部;

  for (; tok1->种类 != 牌_缩_文件终; tok1 = tok1->下一个)
    当前_小 = 当前_小->下一个 = 复制_牌(tok1);
  当前_小->下一个 = tok2;
  return 头部.下一个;
}

static 牌 *跳过_条件_包括2(牌 *牌_短) {
  while (牌_短->种类 != 牌_缩_文件终) {
    if (是否_哈希(牌_短) &&
        (等于(牌_短->下一个, "if") || 等于(牌_短->下一个, "ifdef") ||
         等于(牌_短->下一个, "ifndef"))) {
      牌_短 = 跳过_条件_包括2(牌_短->下一个->下一个);
      continue;
    }
    if (是否_哈希(牌_短) && 等于(牌_短->下一个, "endif"))
      return 牌_短->下一个->下一个;
    牌_短 = 牌_短->下一个;
  }
  return 牌_短;
}

// Skip until 下一个 `#else`, `#elif` or `#endif`.
// Nested `#if` and `#endif` are skipped.
static 牌 *跳过_条件_包括(牌 *牌_短) {
  while (牌_短->种类 != 牌_缩_文件终) {
    if (是否_哈希(牌_短) &&
        (等于(牌_短->下一个, "if") || 等于(牌_短->下一个, "ifdef") ||
         等于(牌_短->下一个, "ifndef"))) {
      牌_短 = 跳过_条件_包括2(牌_短->下一个->下一个);
      continue;
    }

    if (是否_哈希(牌_短) &&
        (等于(牌_短->下一个, "elif") || 等于(牌_短->下一个, "else") ||
         等于(牌_短->下一个, "endif")))
      break;
    牌_短 = 牌_短->下一个;
  }
  return 牌_短;
}

// Double-引号 a given string and returns it.
static char *引号_字符串(char *串_短) {
  int bufsize = 3;
  for (int i = 0; 串_短[i]; i++) {
    if (串_短[i] == '\\' || 串_短[i] == '"')
      bufsize++;
    bufsize++;
  }

  char *缓冲_短 = calloc(1, bufsize);
  char *p = 缓冲_短;
  *p++ = '"';
  for (int i = 0; 串_短[i]; i++) {
    if (串_短[i] == '\\' || 串_短[i] == '"')
      *p++ = '\\';
    *p++ = 串_短[i];
  }
  *p++ = '"';
  *p++ = '\0';
  return 缓冲_短;
}

static 牌 *新_串_牌(char *串_短, 牌 *tmpl) {
  char *缓冲_短 = 引号_字符串(串_短);
  return 化为牌(新_文件(tmpl->源文件位置->名字, tmpl->源文件位置->文件_号, 缓冲_短));
}

// Copy all tokens until the 下一个 newline, terminate them with
// an EOF token and 那么 returns them. This 正函数_ is 已用 to
// create a 新的 list of tokens for `#if` arguments.
static 牌 *复制_行(牌 **rest, 牌 *牌_短) {
  牌 头部 = {};
  牌 *当前_小 = &头部;

  for (; !牌_短->位置_行开始; 牌_短 = 牌_短->下一个)
    当前_小 = 当前_小->下一个 = 复制_牌(牌_短);

  当前_小->下一个 = 新_文件终(牌_短);
  *rest = 牌_短;
  return 头部.下一个;
}

static 牌 *新_数号_牌(int 值_短, 牌 *tmpl) {
  char *缓冲_短 = 格式化("%d\n", 值_短);
  return 化为牌(新_文件(tmpl->源文件位置->名字, tmpl->源文件位置->文件_号, 缓冲_短));
}

static 牌 *读_常量_表达式(牌 **rest, 牌 *牌_短) {
  牌_短 = 复制_行(rest, 牌_短);

  牌 头部 = {};
  牌 *当前_小 = &头部;

  while (牌_短->种类 != 牌_缩_文件终) {
    // "defined(foo)" or "defined foo" becomes "1" if macro "foo"
    // is defined. Otherwise "0".
    if (等于(牌_短, "defined")) {
      牌 *开启 = 牌_短;
      bool has_paren = 消耗(&牌_短, 牌_短->下一个, "(");

      if (牌_短->种类 != 牌_缩_标识符)
        错误_牌_短(开启, "macro 名字 must be an identifier");
      宏 *m = 找_宏(牌_短);
      牌_短 = 牌_短->下一个;

      if (has_paren)
        牌_短 = 跳过(牌_短, ")");

      当前_小 = 当前_小->下一个 = 新_数号_牌(m ? 1 : 0, 开启);
      continue;
    }

    当前_小 = 当前_小->下一个 = 牌_短;
    牌_短 = 牌_短->下一个;
  }

  当前_小->下一个 = 牌_短;
  return 头部.下一个;
}

// Read and evaluate a constant expression.
static long 求值_常量_表达式(牌 **rest, 牌 *牌_短) {
  牌 *开启 = 牌_短;
  牌 *表达式_短 = 读_常量_表达式(rest, 牌_短->下一个);
  表达式_短 = 预处理2(表达式_短);

  if (表达式_短->种类 == 牌_缩_文件终)
    错误_牌_短(开启, "no expression");

  // [https://www.sigbus.info/n1570#6.10.1p4] The standard requires
  // we replace remaining non-macro identifiers with "0" before
  // evaluating a constant expression. For example, `#if foo` is
  // equivalent to `#if 0` if foo is not defined.
  for (牌 *t = 表达式_短; t->种类 != 牌_缩_文件终; t = t->下一个) {
    if (t->种类 == 牌_缩_标识符) {
      牌 *下一个 = t->下一个;
      *t = *新_数号_牌(0, t);
      t->下一个 = 下一个;
    }
  }

  // Convert pp-numbers to regular numbers
  转换_预处理_牌(表达式_短);

  牌 *rest2;
  long 值_短 = 常量_表达式(&rest2, 表达式_短);
  if (rest2->种类 != 牌_缩_文件终)
    错误_牌_短(rest2, "extra token");
  return 值_短;
}

static 条件包括_短 *推_条件_包括(牌 *牌_短, bool 已包括) {
  条件包括_短 *ci = calloc(1, sizeof(条件包括_短));
  ci->下一个 = 条件_包括_短;
  ci->ctx = IN_THEN;
  ci->牌_短 = 牌_短;
  ci->已包括 = 已包括;
  条件_包括_短 = ci;
  return ci;
}

static 宏 *找_宏(牌 *牌_短) {
  if (牌_短->种类 != 牌_缩_标识符)
    return NULL;
  return 哈希映射_取2(&宏_小写, 牌_短->位置_短, 牌_短->长度_短);
}

static 宏 *添加_宏(char *名字, bool is_objlike, 牌 *体) {
  宏 *m = calloc(1, sizeof(宏));
  m->名字 = 名字;
  m->is_objlike = is_objlike;
  m->体 = 体;
  哈希映射_放置(&宏_小写, 名字, m);
  return m;
}

static 宏形参 *读_宏_形参(牌 **rest, 牌 *牌_短, char **va_args_name) {
  宏形参 头部 = {};
  宏形参 *当前_小 = &头部;

  while (!等于(牌_短, ")")) {
    if (当前_小 != &头部)
      牌_短 = 跳过(牌_短, ",");

    if (等于(牌_短, "...")) {
      *va_args_name = "__VA_ARGS__";
      *rest = 跳过(牌_短->下一个, ")");
      return 头部.下一个;
    }

    if (牌_短->种类 != 牌_缩_标识符)
      错误_牌_短(牌_短, "expected an identifier");

    if (等于(牌_短->下一个, "...")) {
      *va_args_name = strndup(牌_短->位置_短, 牌_短->长度_短);
      *rest = 跳过(牌_短->下一个->下一个, ")");
      return 头部.下一个;
    }

    宏形参 *m = calloc(1, sizeof(宏形参));
    m->名字 = strndup(牌_短->位置_短, 牌_短->长度_短);
    当前_小 = 当前_小->下一个 = m;
    牌_短 = 牌_短->下一个;
  }

  *rest = 牌_短->下一个;
  return 头部.下一个;
}

static void 读_宏_定义(牌 **rest, 牌 *牌_短) {
  if (牌_短->种类 != 牌_缩_标识符)
    错误_牌_短(牌_短, "macro 名字 must be an identifier");
  char *名字 = strndup(牌_短->位置_短, 牌_短->长度_短);
  牌_短 = 牌_短->下一个;

  if (!牌_短->有_空格字符 && 等于(牌_短, "(")) {
    // Function-like macro
    char *va_args_name = NULL;
    宏形参 *形参_短 = 读_宏_形参(&牌_短, 牌_短->下一个, &va_args_name);

    宏 *m = 添加_宏(名字, false, 复制_行(rest, 牌_短));
    m->形参_短 = 形参_短;
    m->va_args_name = va_args_name;
  } else {
    // Object-like macro
    添加_宏(名字, true, 复制_行(rest, 牌_短));
  }
}

static 宏实参 *读_宏_实参_一(牌 **rest, 牌 *牌_短, bool read_rest) {
  牌 头部 = {};
  牌 *当前_小 = &头部;
  int level = 0;

  for (;;) {
    if (level == 0 && 等于(牌_短, ")"))
      break;
    if (level == 0 && !read_rest && 等于(牌_短, ","))
      break;

    if (牌_短->种类 == 牌_缩_文件终)
      错误_牌_短(牌_短, "premature 终 of 输入");

    if (等于(牌_短, "("))
      level++;
    else if (等于(牌_短, ")"))
      level--;

    当前_小 = 当前_小->下一个 = 复制_牌(牌_短);
    牌_短 = 牌_短->下一个;
  }

  当前_小->下一个 = 新_文件终(牌_短);

  宏实参 *arg = calloc(1, sizeof(宏实参));
  arg->牌_短 = 头部.下一个;
  *rest = 牌_短;
  return arg;
}

static 宏实参 *
读_宏_实参(牌 **rest, 牌 *牌_短, 宏形参 *形参_短, char *va_args_name) {
  牌 *开启 = 牌_短;
  牌_短 = 牌_短->下一个->下一个;

  宏实参 头部 = {};
  宏实参 *当前_小 = &头部;

  宏形参 *pp = 形参_短;
  for (; pp; pp = pp->下一个) {
    if (当前_小 != &头部)
      牌_短 = 跳过(牌_短, ",");
    当前_小 = 当前_小->下一个 = 读_宏_实参_一(&牌_短, 牌_短, false);
    当前_小->名字 = pp->名字;
  }

  if (va_args_name) {
    宏实参 *arg;
    if (等于(牌_短, ")")) {
      arg = calloc(1, sizeof(宏实参));
      arg->牌_短 = 新_文件终(牌_短);
    } else {
      if (pp != 形参_短)
        牌_短 = 跳过(牌_短, ",");
      arg = 读_宏_实参_一(&牌_短, 牌_短, true);
    }
    arg->名字 = va_args_name;;
    arg->is_va_args = true;
    当前_小 = 当前_小->下一个 = arg;
  } else if (pp) {
    错误_牌_短(开启, "too many arguments");
  }

  跳过(牌_短, ")");
  *rest = 牌_短;
  return 头部.下一个;
}

static 宏实参 *找_实参(宏实参 *实参_短, 牌 *牌_短) {
  for (宏实参 *ap = 实参_短; ap; ap = ap->下一个)
    if (牌_短->长度_短 == strlen(ap->名字) && !strncmp(牌_短->位置_短, ap->名字, 牌_短->长度_短))
      return ap;
  return NULL;
}

// Concatenates all tokens in `牌_短` and returns a 新的 string.
static char *加入_牌(牌 *牌_短, 牌 *终) {
  // Compute the length of the resulting token.
  int 长度_短 = 1;
  for (牌 *t = 牌_短; t != 终 && t->种类 != 牌_缩_文件终; t = t->下一个) {
    if (t != 牌_短 && t->有_空格字符)
      长度_短++;
    长度_短 += t->长度_短;
  }

  char *缓冲_短 = calloc(1, 长度_短);

  // Copy token texts.
  int 地点位 = 0;
  for (牌 *t = 牌_短; t != 终 && t->种类 != 牌_缩_文件终; t = t->下一个) {
    if (t != 牌_短 && t->有_空格字符)
      缓冲_短[地点位++] = ' ';
    strncpy(缓冲_短 + 地点位, t->位置_短, t->长度_短);
    地点位 += t->长度_短;
  }
  缓冲_短[地点位] = '\0';
  return 缓冲_短;
}

// Concatenates all tokens in `arg` and returns a 新的 string token.
// This 正函数_ is 已用 for the stringizing operator (#).
static 牌 *化为串(牌 *hash, 牌 *arg) {
  // Create a 新的 string token. We need to set some value to its
  // source location for 错误 reporting 正函数_, so we use a macro
  // 名字 token as a template.
  char *s = 加入_牌(arg, NULL);
  return 新_串_牌(s, hash);
}

// Concatenate two tokens to create a 新的 token.
static 牌 *粘贴(牌 *左手边, 牌 *右手边) {
  // Paste the two tokens.
  char *缓冲_短 = 格式化("%.*s%.*s", 左手边->长度_短, 左手边->位置_短, 右手边->长度_短, 右手边->位置_短);

  // Tokenize the resulting string.
  牌 *牌_短 = 化为牌(新_文件(左手边->源文件位置->名字, 左手边->源文件位置->文件_号, 缓冲_短));
  if (牌_短->下一个->种类 != 牌_缩_文件终)
    错误_牌_短(左手边, "pasting forms '%s', an invalid token", 缓冲_短);
  return 牌_短;
}

static bool 有_变量实参(宏实参 *实参_短) {
  for (宏实参 *ap = 实参_短; ap; ap = ap->下一个)
    if (!strcmp(ap->名字, "__VA_ARGS__"))
      return ap->牌_短->种类 != 牌_缩_文件终;
  return false;
}

// Replace func-like macro parameters with given arguments.
static 牌 *替代品(牌 *牌_短, 宏实参 *实参_短) {
  牌 头部 = {};
  牌 *当前_小 = &头部;

  while (牌_短->种类 != 牌_缩_文件终) {
    // "#" followed by a parameter is replaced with stringized actuals.
    if (等于(牌_短, "#")) {
      宏实参 *arg = 找_实参(实参_短, 牌_短->下一个);
      if (!arg)
        错误_牌_短(牌_短->下一个, "'#' is not followed by a macro parameter");
      当前_小 = 当前_小->下一个 = 化为串(牌_短, arg->牌_短);
      牌_短 = 牌_短->下一个->下一个;
      continue;
    }

    // [GNU] If __VA_ARG__ is empty, `,##__VA_ARGS__` is expanded
    // to the empty token list. Otherwise, its expaned to `,` and
    // __VA_ARGS__.
    if (等于(牌_短, ",") && 等于(牌_短->下一个, "##")) {
      宏实参 *arg = 找_实参(实参_短, 牌_短->下一个->下一个);
      if (arg && arg->is_va_args) {
        if (arg->牌_短->种类 == 牌_缩_文件终) {
          牌_短 = 牌_短->下一个->下一个->下一个;
        } else {
          当前_小 = 当前_小->下一个 = 复制_牌(牌_短);
          牌_短 = 牌_短->下一个->下一个;
        }
        continue;
      }
    }

    if (等于(牌_短, "##")) {
      if (当前_小 == &头部)
        错误_牌_短(牌_短, "'##' cannot appear at 开启 of macro expansion");

      if (牌_短->下一个->种类 == 牌_缩_文件终)
        错误_牌_短(牌_短, "'##' cannot appear at 终 of macro expansion");

      宏实参 *arg = 找_实参(实参_短, 牌_短->下一个);
      if (arg) {
        if (arg->牌_短->种类 != 牌_缩_文件终) {
          *当前_小 = *粘贴(当前_小, arg->牌_短);
          for (牌 *t = arg->牌_短->下一个; t->种类 != 牌_缩_文件终; t = t->下一个)
            当前_小 = 当前_小->下一个 = 复制_牌(t);
        }
        牌_短 = 牌_短->下一个->下一个;
        continue;
      }

      *当前_小 = *粘贴(当前_小, 牌_短->下一个);
      牌_短 = 牌_短->下一个->下一个;
      continue;
    }

    宏实参 *arg = 找_实参(实参_短, 牌_短);

    if (arg && 等于(牌_短->下一个, "##")) {
      牌 *右手边 = 牌_短->下一个->下一个;

      if (arg->牌_短->种类 == 牌_缩_文件终) {
        宏实参 *arg2 = 找_实参(实参_短, 右手边);
        if (arg2) {
          for (牌 *t = arg2->牌_短; t->种类 != 牌_缩_文件终; t = t->下一个)
            当前_小 = 当前_小->下一个 = 复制_牌(t);
        } else {
          当前_小 = 当前_小->下一个 = 复制_牌(右手边);
        }
        牌_短 = 右手边->下一个;
        continue;
      }

      for (牌 *t = arg->牌_短; t->种类 != 牌_缩_文件终; t = t->下一个)
        当前_小 = 当前_小->下一个 = 复制_牌(t);
      牌_短 = 牌_短->下一个;
      continue;
    }

    // If __VA_ARG__ is empty, __VA_OPT__(x) is expanded to the
    // empty token list. Otherwise, __VA_OPT__(x) is expanded to x.
    if (等于(牌_短, "__VA_OPT__") && 等于(牌_短->下一个, "(")) {
      宏实参 *arg = 读_宏_实参_一(&牌_短, 牌_短->下一个->下一个, true);
      if (有_变量实参(实参_短))
        for (牌 *t = arg->牌_短; t->种类 != 牌_缩_文件终; t = t->下一个)
          当前_小 = 当前_小->下一个 = t;
      牌_短 = 跳过(牌_短, ")");
      continue;
    }

    // Handle a macro token. 宏 arguments are completely macro-expanded
    // before they are substituted into a macro 体.
    if (arg) {
      牌 *t = 预处理2(arg->牌_短);
      t->位置_行开始 = 牌_短->位置_行开始;
      t->有_空格字符 = 牌_短->有_空格字符;
      for (; t->种类 != 牌_缩_文件终; t = t->下一个)
        当前_小 = 当前_小->下一个 = 复制_牌(t);
      牌_短 = 牌_短->下一个;
      continue;
    }

    // Handle a non-macro token.
    当前_小 = 当前_小->下一个 = 复制_牌(牌_短);
    牌_短 = 牌_短->下一个;
    continue;
  }

  当前_小->下一个 = 牌_短;
  return 头部.下一个;
}

// If 牌_短 is a macro, expand it and return true.
// Otherwise, do nothing and return false.
static bool 扩展_宏(牌 **rest, 牌 *牌_短) {
  if (隐藏集_内容(牌_短->隐藏集_小, 牌_短->位置_短, 牌_短->长度_短))
    return false;

  宏 *m = 找_宏(牌_短);
  if (!m)
    return false;

  // Built-in dynamic macro application such as __LINE__
  if (m->手握者) {
    *rest = m->手握者(牌_短);
    (*rest)->下一个 = 牌_短->下一个;
    return true;
  }

  // Object-like macro application
  if (m->is_objlike) {
    隐藏集 *hs = 隐藏集_联合(牌_短->隐藏集_小, 新_隐藏集(m->名字));
    牌 *体 = 添加_隐藏集(m->体, hs);
    for (牌 *t = 体; t->种类 != 牌_缩_文件终; t = t->下一个)
      t->原创 = 牌_短;
    *rest = 追加(体, 牌_短->下一个);
    (*rest)->位置_行开始 = 牌_短->位置_行开始;
    (*rest)->有_空格字符 = 牌_短->有_空格字符;
    return true;
  }

  // If a funclike macro token is not followed by an argument list,
  // treat it as a normal identifier.
  if (!等于(牌_短->下一个, "("))
    return false;

  // Function-like macro application
  牌 *macro_token = 牌_短;
  宏实参 *实参_短 = 读_宏_实参(&牌_短, 牌_短, m->形参_短, m->va_args_name);
  牌 *rparen = 牌_短;

  // Tokens that consist a func-like macro invocation may have different
  // hidesets, and if that's the case, it's not clear what the 隐藏集_小
  // for the 新的 tokens should be. We take the interesection of the
  // macro token and the closing parenthesis and use it as a 新的 隐藏集_小
  // as explained in the Dave Prossor's algorithm.
  隐藏集 *hs = 隐藏集_交叉点(macro_token->隐藏集_小, rparen->隐藏集_小);
  hs = 隐藏集_联合(hs, 新_隐藏集(m->名字));

  牌 *体 = 替代品(m->体, 实参_短);
  体 = 添加_隐藏集(体, hs);
  for (牌 *t = 体; t->种类 != 牌_缩_文件终; t = t->下一个)
    t->原创 = macro_token;
  *rest = 追加(体, 牌_短->下一个);
  (*rest)->位置_行开始 = macro_token->位置_行开始;
  (*rest)->有_空格字符 = macro_token->有_空格字符;
  return true;
}

char *搜索_包括_路径(char *文件名) {
  if (文件名[0] == '/')
    return 文件名;

  static 哈希映射 cache;
  char *cached = 哈希映射_取(&cache, 文件名);
  if (cached)
    return cached;

  // Search a 源文件位置 from the include paths.
  for (int i = 0; i < 包括_路径.长度_短; i++) {
    char *路径 = 格式化("%s/%s", 包括_路径.数据[i], 文件名);
    if (!文件_存在(路径))
      continue;
    哈希映射_放置(&cache, 文件名, 路径);
    包括_下一个_索引 = i + 1;
    return 路径;
  }
  return NULL;
}

static char *搜索_包括_下一个(char *文件名) {
  for (; 包括_下一个_索引 < 包括_路径.长度_短; 包括_下一个_索引++) {
    char *路径 = 格式化("%s/%s", 包括_路径.数据[包括_下一个_索引], 文件名);
    if (文件_存在(路径))
      return 路径;
  }
  return NULL;
}

// Read an #include argument.
static char *读_包括_文件名(牌 **rest, 牌 *牌_短, bool *是否_双引号) {
  // Pattern 1: #include "foo.h"
  if (牌_短->种类 == 牌_缩_串字面) {
    // A double-quoted 文件名 for #include is a special 种类 of
    // token, and we don't want to interpret any escape sequences in it.
    // For example, "\f" in "C:\foo" is not a formfeed character but
    // just two non-control characters, backslash and f.
    // So we don't want to use token->串_短.
    *是否_双引号 = true;
    *rest = 跳过_行(牌_短->下一个);
    return strndup(牌_短->位置_短 + 1, 牌_短->长度_短 - 2);
  }

  // Pattern 2: #include <foo.h>
  if (等于(牌_短, "<")) {
    // Reconstruct a 文件名 from a sequence of tokens between
    // "<" and ">".
    牌 *开启 = 牌_短;

    // Find closing ">".
    for (; !等于(牌_短, ">"); 牌_短 = 牌_短->下一个)
      if (牌_短->位置_行开始 || 牌_短->种类 == 牌_缩_文件终)
        错误_牌_短(牌_短, "expected '>'");

    *是否_双引号 = false;
    *rest = 跳过_行(牌_短->下一个);
    return 加入_牌(开启->下一个, 牌_短);
  }

  // Pattern 3: #include FOO
  // In this case FOO must be macro-expanded to either
  // a single string token or a sequence of "<" ... ">".
  if (牌_短->种类 == 牌_缩_标识符) {
    牌 *tok2 = 预处理2(复制_行(rest, 牌_短));
    return 读_包括_文件名(&tok2, tok2, 是否_双引号);
  }

  错误_牌_短(牌_短, "expected a 文件名");
}

// Detect the following "include guard" pattern.
//
//   #ifndef FOO_H
//   #define FOO_H
//   ...
//   #endif
static char *侦测_包括_守卫(牌 *牌_短) {
  // Detect the first two lines.
  if (!是否_哈希(牌_短) || !等于(牌_短->下一个, "ifndef"))
    return NULL;
  牌_短 = 牌_短->下一个->下一个;

  if (牌_短->种类 != 牌_缩_标识符)
    return NULL;

  char *macro = strndup(牌_短->位置_短, 牌_短->长度_短);
  牌_短 = 牌_短->下一个;

  if (!是否_哈希(牌_短) || !等于(牌_短->下一个, "define") || !等于(牌_短->下一个->下一个, macro))
    return NULL;

  // Read until the 终 of the 源文件位置.
  while (牌_短->种类 != 牌_缩_文件终) {
    if (!是否_哈希(牌_短)) {
      牌_短 = 牌_短->下一个;
      continue;
    }

    if (等于(牌_短->下一个, "endif") && 牌_短->下一个->下一个->种类 == 牌_缩_文件终)
      return macro;

    if (等于(牌_短, "if") || 等于(牌_短, "ifdef") || 等于(牌_短, "ifndef"))
      牌_短 = 跳过_条件_包括(牌_短->下一个);
    else
      牌_短 = 牌_短->下一个;
  }
  return NULL;
}

static 牌 *包括_文件(牌 *牌_短, char *路径, 牌 *filename_tok) {
  // Check for "#pragma once"
  if (哈希映射_取(&杂注_一次, 路径))
    return 牌_短;

  // If we read the same 源文件位置 before, and if the 源文件位置 was guarded
  // by the usual #ifndef ... #endif pattern, we may be able to
  // 跳过 the 源文件位置 without opening it.
  static 哈希映射 包括_守卫;
  char *守卫_名字 = 哈希映射_取(&包括_守卫, 路径);
  if (守卫_名字 && 哈希映射_取(&宏_小写, 守卫_名字))
    return 牌_短;

  牌 *tok2 = 化为牌_文件(路径);
  if (!tok2)
    错误_牌_短(filename_tok, "%s: cannot open 源文件位置: %s", 路径, strerror(errno));

  守卫_名字 = 侦测_包括_守卫(tok2);
  if (守卫_名字)
    哈希映射_放置(&包括_守卫, 路径, 守卫_名字);

  return 追加(tok2, 牌_短);
}

// Read #行 arguments
static void 读_行_做标记(牌 **rest, 牌 *牌_短) {
  牌 *开启 = 牌_短;
  牌_短 = 预处理(复制_行(rest, 牌_短));

  if (牌_短->种类 != 牌_缩_数值字面 || 牌_短->类型_缩->种类 != 类型_整型)
    错误_牌_短(牌_短, "invalid 行 marker");
  开启->源文件位置->行_德尔塔 = 牌_短->值_短 - 开启->行_号;

  牌_短 = 牌_短->下一个;
  if (牌_短->种类 == 牌_缩_文件终)
    return;

  if (牌_短->种类 != 牌_缩_串字面)
    错误_牌_短(牌_短, "文件名 expected");
  开启->源文件位置->显示_名字 = 牌_短->串_短;
}

// Visit all tokens in `牌_短` while evaluating preprocessing
// 宏_小写 and directives.
static 牌 *预处理2(牌 *牌_短) {
  牌 头部 = {};
  牌 *当前_小 = &头部;

  while (牌_短->种类 != 牌_缩_文件终) {
    // If it is a macro, expand it.
    if (扩展_宏(&牌_短, 牌_短))
      continue;

    // Pass through if it is not a "#".
    if (!是否_哈希(牌_短)) {
      牌_短->行_德尔塔 = 牌_短->源文件位置->行_德尔塔;
      牌_短->文件名 = 牌_短->源文件位置->显示_名字;
      当前_小 = 当前_小->下一个 = 牌_短;
      牌_短 = 牌_短->下一个;
      continue;
    }

    牌 *开启 = 牌_短;
    牌_短 = 牌_短->下一个;

    if (等于(牌_短, "include")) {
      bool 是否_双引号;
      char *文件名 = 读_包括_文件名(&牌_短, 牌_短->下一个, &是否_双引号);

      if (文件名[0] != '/' && 是否_双引号) {
        char *路径 = 格式化("%s/%s", dirname(strdup(开启->源文件位置->名字)), 文件名);
        if (文件_存在(路径)) {
          牌_短 = 包括_文件(牌_短, 路径, 开启->下一个->下一个);
          continue;
        }
      }

      char *路径 = 搜索_包括_路径(文件名);
      牌_短 = 包括_文件(牌_短, 路径 ? 路径 : 文件名, 开启->下一个->下一个);
      continue;
    }

    if (等于(牌_短, "include_next")) {
      bool ignore;
      char *文件名 = 读_包括_文件名(&牌_短, 牌_短->下一个, &ignore);
      char *路径 = 搜索_包括_下一个(文件名);
      牌_短 = 包括_文件(牌_短, 路径 ? 路径 : 文件名, 开启->下一个->下一个);
      continue;
    }

    if (等于(牌_短, "define")) {
      读_宏_定义(&牌_短, 牌_短->下一个);
      continue;
    }

    if (等于(牌_短, "undef")) {
      牌_短 = 牌_短->下一个;
      if (牌_短->种类 != 牌_缩_标识符)
        错误_牌_短(牌_短, "macro 名字 must be an identifier");
      未定义_宏(strndup(牌_短->位置_短, 牌_短->长度_短));
      牌_短 = 跳过_行(牌_短->下一个);
      continue;
    }

    if (等于(牌_短, "if")) {
      long 值_短 = 求值_常量_表达式(&牌_短, 牌_短);
      推_条件_包括(开启, 值_短);
      if (!值_短)
        牌_短 = 跳过_条件_包括(牌_短);
      continue;
    }

    if (等于(牌_短, "ifdef")) {
      bool defined = 找_宏(牌_短->下一个);
      推_条件_包括(牌_短, defined);
      牌_短 = 跳过_行(牌_短->下一个->下一个);
      if (!defined)
        牌_短 = 跳过_条件_包括(牌_短);
      continue;
    }

    if (等于(牌_短, "ifndef")) {
      bool defined = 找_宏(牌_短->下一个);
      推_条件_包括(牌_短, !defined);
      牌_短 = 跳过_行(牌_短->下一个->下一个);
      if (defined)
        牌_短 = 跳过_条件_包括(牌_短);
      continue;
    }

    if (等于(牌_短, "elif")) {
      if (!条件_包括_短 || 条件_包括_短->ctx == IN_ELSE)
        错误_牌_短(开启, "stray #elif");
      条件_包括_短->ctx = IN_ELIF;

      if (!条件_包括_短->已包括 && 求值_常量_表达式(&牌_短, 牌_短))
        条件_包括_短->已包括 = true;
      else
        牌_短 = 跳过_条件_包括(牌_短);
      continue;
    }

    if (等于(牌_短, "else")) {
      if (!条件_包括_短 || 条件_包括_短->ctx == IN_ELSE)
        错误_牌_短(开启, "stray #else");
      条件_包括_短->ctx = IN_ELSE;
      牌_短 = 跳过_行(牌_短->下一个);

      if (条件_包括_短->已包括)
        牌_短 = 跳过_条件_包括(牌_短);
      continue;
    }

    if (等于(牌_短, "endif")) {
      if (!条件_包括_短)
        错误_牌_短(开启, "stray #endif");
      条件_包括_短 = 条件_包括_短->下一个;
      牌_短 = 跳过_行(牌_短->下一个);
      continue;
    }

    if (等于(牌_短, "行")) {
      读_行_做标记(&牌_短, 牌_短->下一个);
      continue;
    }

    if (牌_短->种类 == 牌_缩_预处理号码) {
      读_行_做标记(&牌_短, 牌_短);
      continue;
    }

    if (等于(牌_短, "pragma") && 等于(牌_短->下一个, "once")) {
      哈希映射_放置(&杂注_一次, 牌_短->源文件位置->名字, (void *)1);
      牌_短 = 跳过_行(牌_短->下一个->下一个);
      continue;
    }

    if (等于(牌_短, "pragma")) {
      do {
        牌_短 = 牌_短->下一个;
      } while (!牌_短->位置_行开始);
      continue;
    }

    if (等于(牌_短, "错误"))
      错误_牌_短(牌_短, "错误");

    // `#`-only 行 is legal. It's called a null directive.
    if (牌_短->位置_行开始)
      continue;

    错误_牌_短(牌_短, "invalid preprocessor directive");
  }

  当前_小->下一个 = 牌_短;
  return 头部.下一个;
}

void 定义_宏(char *名字, char *缓冲_短) {
  牌 *牌_短 = 化为牌(新_文件("<built-in>", 1, 缓冲_短));
  添加_宏(名字, true, 牌_短);
}

void 未定义_宏(char *名字) {
  哈希映射_删除(&宏_小写, 名字);
}

static 宏 *添加_内建(char *名字, 宏_手握者_fn *fn) {
  宏 *m = 添加_宏(名字, true, NULL);
  m->手握者 = fn;
  return m;
}

static 牌 *文件_宏(牌 *tmpl) {
  while (tmpl->原创)
    tmpl = tmpl->原创;
  return 新_串_牌(tmpl->源文件位置->显示_名字, tmpl);
}

static 牌 *行_宏(牌 *tmpl) {
  while (tmpl->原创)
    tmpl = tmpl->原创;
  int i = tmpl->行_号 + tmpl->源文件位置->行_德尔塔;
  return 新_数号_牌(i, tmpl);
}

// __COUNTER__ is expanded to serial values starting from 0.
static 牌 *计数器_宏(牌 *tmpl) {
  static int i = 0;
  return 新_数号_牌(i++, tmpl);
}

// __TIMESTAMP__ is expanded to a string describing the last
// modification time of the current 源文件位置. E.g.
// "Fri Jul 24 01:32:50 2020"
static 牌 *时间戳_宏(牌 *tmpl) {
  struct stat st;
  if (stat(tmpl->源文件位置->名字, &st) != 0)
    return 新_串_牌("??? ??? ?? ??:??:?? ????", tmpl);

  char 缓冲_短[30];
  ctime_r(&st.st_mtime, 缓冲_短);
  缓冲_短[24] = '\0';
  return 新_串_牌(缓冲_短, tmpl);
}

static 牌 *基础_文件_宏(牌 *tmpl) {
  return 新_串_牌(基础_文件, tmpl);
}

// __DATE__ is expanded to the current date, e.g. "May 17 2020".
static char *格式_数据(struct tm *tm) {
  static char mon[][4] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
  };

  return 格式化("\"%s %2d %d\"", mon[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
}

// __TIME__ is expanded to the current time, e.g. "13:34:03".
static char *格式_时间(struct tm *tm) {
  return 格式化("\"%02d:%02d:%02d\"", tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void 初始_宏(void) {
  // Define predefined 宏_小写
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

  添加_内建("__FILE__", 文件_宏);
  添加_内建("__LINE__", 行_宏);
  添加_内建("__COUNTER__", 计数器_宏);
  添加_内建("__TIMESTAMP__", 时间戳_宏);
  添加_内建("__BASE_FILE__", 基础_文件_宏);

  time_t now = time(NULL);
  struct tm *tm = localtime(&now);
  定义_宏("__DATE__", 格式_数据(tm));
  定义_宏("__TIME__", 格式_时间(tm));
}

typedef enum {
  STR_NONE, STR_UTF8, STR_UTF16, STR_UTF32, STR_WIDE,
} 字符串种类;

static 字符串种类 取字符串种类(牌 *牌_短) {
  if (!strcmp(牌_短->位置_短, "u8"))
    return STR_UTF8;

  switch (牌_短->位置_短[0]) {
  case '"': return STR_NONE;
  case 'u': return STR_UTF16;
  case 'U': return STR_UTF32;
  case 'L': return STR_WIDE;
  }
  不可达();
}

// Concatenate adjacent string literals into a single string literal
// as per the C spec.
static void 加入_邻近的_字符串_字面(牌 *牌_短) {
  // First pass: If regular string literals are adjacent to wide
  // string literals, regular string literals are converted to a wide
  // type before concatenation. In this pass, we do the conversion.
  for (牌 *tok1 = 牌_短; tok1->种类 != 牌_缩_文件终;) {
    if (tok1->种类 != 牌_缩_串字面 || tok1->下一个->种类 != 牌_缩_串字面) {
      tok1 = tok1->下一个;
      continue;
    }

    字符串种类 种类 = 取字符串种类(tok1);
    类型 *基本类型 = tok1->类型_缩->基础;

    for (牌 *t = tok1->下一个; t->种类 == 牌_缩_串字面; t = t->下一个) {
      字符串种类 k = 取字符串种类(t);
      if (种类 == STR_NONE) {
        种类 = k;
        基本类型 = t->类型_缩->基础;
      } else if (k != STR_NONE && 种类 != k) {
        错误_牌_短(t, "unsupported non-standard concatenation of string literals");
      }
    }

    if (基本类型->大小 > 1)
      for (牌 *t = tok1; t->种类 == 牌_缩_串字面; t = t->下一个)
        if (t->类型_缩->基础->大小 == 1)
          *t = *化为牌_字符串_字面(t, 基本类型);

    while (tok1->种类 == 牌_缩_串字面)
      tok1 = tok1->下一个;
  }

  // Second pass: concatenate adjacent string literals.
  for (牌 *tok1 = 牌_短; tok1->种类 != 牌_缩_文件终;) {
    if (tok1->种类 != 牌_缩_串字面 || tok1->下一个->种类 != 牌_缩_串字面) {
      tok1 = tok1->下一个;
      continue;
    }

    牌 *tok2 = tok1->下一个;
    while (tok2->种类 == 牌_缩_串字面)
      tok2 = tok2->下一个;

    int 长度_短 = tok1->类型_缩->数组_长度;
    for (牌 *t = tok1->下一个; t != tok2; t = t->下一个)
      长度_短 = 长度_短 + t->类型_缩->数组_长度 - 1;

    char *缓冲_短 = calloc(tok1->类型_缩->基础->大小, 长度_短);

    int i = 0;
    for (牌 *t = tok1; t != tok2; t = t->下一个) {
      memcpy(缓冲_短 + i, t->串_短, t->类型_缩->大小);
      i = i + t->类型_缩->大小 - t->类型_缩->基础->大小;
    }

    *tok1 = *复制_牌(tok1);
    tok1->类型_缩 = 数组_阵列(tok1->类型_缩->基础, 长度_短);
    tok1->串_短 = 缓冲_短;
    tok1->下一个 = tok2;
    tok1 = tok2;
  }
}

// Entry point 正函数_ of the preprocessor.
牌 *预处理(牌 *牌_短) {
  牌_短 = 预处理2(牌_短);
  if (条件_包括_短)
    错误_牌_短(条件_包括_短->牌_短, "unterminated 有条件的 directive");
  转换_预处理_牌(牌_短);
  加入_邻近的_字符串_字面(牌_短);

  for (牌 *t = 牌_短; t; t = t->下一个)
    t->行_号 += t->行_德尔塔;
  return 牌_短;
}
