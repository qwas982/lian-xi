#include "chibicc.h"

// Input 源文件位置
static 文件_大写 *当前_文件;

// A list of all 输入 files.
static 文件_大写 **输入_文件;

// True if the current position is at the beginning of a 行
static bool 位置_行开始;

// True if the current position follows a space character
static bool 有_空格字符;

// Reports an 错误 and exit.
void 错误(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an 错误 message in the following 格式化.
//
// foo.c:10: x = y + 1;
//               ^ <错误 message here>
static void verror_at(char *文件名, char *输入, int 行_号,
                      char *位置_短, char *fmt, va_list ap) {
  // Find a 行 containing `位置_短`.
  char *行 = 位置_短;
  while (输入 < 行 && 行[-1] != '\n')
    行--;

  char *终 = 位置_短;
  while (*终 && *终 != '\n')
    终++;

  // Print out the 行.
  int 标识 = fprintf(stderr, "%s:%d: ", 文件名, 行_号);
  fprintf(stderr, "%.*s\n", (int)(终 - 行), 行);

  // Show the 错误 message.
  int 地点位 = 显示_宽度(行, 位置_短 - 行) + 标识;

  fprintf(stderr, "%*s", 地点位, ""); // print 地点位 spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
}

void 错误_位置(char *位置_短, char *fmt, ...) {
  int 行_号 = 1;
  for (char *p = 当前_文件->内容; p < 位置_短; p++)
    if (*p == '\n')
      行_号++;

  va_list ap;
  va_start(ap, fmt);
  verror_at(当前_文件->名字, 当前_文件->内容, 行_号, 位置_短, fmt, ap);
  exit(1);
}

void 错误_牌_短(牌 *牌_短, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(牌_短->源文件位置->名字, 牌_短->源文件位置->内容, 牌_短->行_号, 牌_短->位置_短, fmt, ap);
  exit(1);
}

void 警告_牌_短(牌 *牌_短, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  verror_at(牌_短->源文件位置->名字, 牌_短->源文件位置->内容, 牌_短->行_号, 牌_短->位置_短, fmt, ap);
  va_end(ap);
}

// Consumes the current token if it matches `op`.
bool 等于(牌 *牌_短, char *op) {
  return memcmp(牌_短->位置_短, op, 牌_短->长度_短) == 0 && op[牌_短->长度_短] == '\0';
}

// Ensure that the current token is `op`.
牌 *跳过(牌 *牌_短, char *op) {
  if (!等于(牌_短, op))
    错误_牌_短(牌_短, "expected '%s'", op);
  return 牌_短->下一个;
}

bool 消耗(牌 **rest, 牌 *牌_短, char *串_短) {
  if (等于(牌_短, 串_短)) {
    *rest = 牌_短->下一个;
    return true;
  }
  *rest = 牌_短;
  return false;
}

// Create a 新的 token.
static 牌 *new_token(牌种类 种类, char *开启, char *终) {
  牌 *牌_短 = calloc(1, sizeof(牌));
  牌_短->种类 = 种类;
  牌_短->位置_短 = 开启;
  牌_短->长度_短 = 终 - 开启;
  牌_短->源文件位置 = 当前_文件;
  牌_短->文件名 = 当前_文件->显示_名字;
  牌_短->位置_行开始 = 位置_行开始;
  牌_短->有_空格字符 = 有_空格字符;

  位置_行开始 = 有_空格字符 = false;
  return 牌_短;
}

static bool 起始于(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Read an identifier and returns the length of it.
// If p does not point to a valid identifier, 0 is returned.
static int 读_标识(char *开启) {
  char *p = 开启;
  uint32_t c = 解码_万国码8(&p, p);
  if (!是否_标识1(c))
    return 0;

  for (;;) {
    char *q;
    c = 解码_万国码8(&q, p);
    if (!是否_标识2(c))
      return p - 开启;
    p = q;
  }
}

static int 来自_十六进制_短(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

// Read a punctuator token from p and returns its length.
static int 读_标点符号(char *p) {
  static char *关键字_缩[] = {
    "<<=", ">>=", "...", "==", "!=", "<=", ">=", "->", "+=",
    "-=", "*=", "/=", "++", "--", "%=", "&=", "|=", "^=", "&&",
    "||", "<<", ">>", "##",
  };

  for (int i = 0; i < sizeof(关键字_缩) / sizeof(*关键字_缩); i++)
    if (起始于(p, 关键字_缩[i]))
      return strlen(关键字_缩[i]);

  return ispunct(*p) ? 1 : 0;
}

static bool 是否_关键字(牌 *牌_短) {
  static 哈希映射 映射;

  if (映射.容量 == 0) {
    static char *关键字_缩[] = {
      "return", "if", "else", "for", "while", "int", "sizeof", "char",
      "struct", "union", "short", "long", "void", "typedef", "_Bool",
      "enum", "static", "goto", "break", "continue", "switch", "case",
      "default", "extern", "_Alignof", "_Alignas", "do", "signed",
      "unsigned", "const", "volatile", "auto", "register", "restrict",
      "__restrict", "__restrict__", "_Noreturn", "float", "double",
      "typeof", "asm", "_Thread_local", "__thread", "_Atomic",
      "__attribute__",
    };

    for (int i = 0; i < sizeof(关键字_缩) / sizeof(*关键字_缩); i++)
      哈希映射_放置(&映射, 关键字_缩[i], (void *)1);
  }

  return 哈希映射_取2(&映射, 牌_短->位置_短, 牌_短->长度_短);
}

static int 读_已转义_字符(char **新_位置_短, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        c = (c << 3) + (*p++ - '0');
    }
    *新_位置_短 = p;
    return c;
  }

  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p))
      错误_位置(p, "invalid hex escape sequence");

    int c = 0;
    for (; isxdigit(*p); p++)
      c = (c << 4) + 来自_十六进制_短(*p);
    *新_位置_短 = p;
    return c;
  }

  *新_位置_短 = p + 1;

  // Escape sequences are defined using themselves here. E.g.
  // '\n' is implemented using '\n'. This tautological definition
  // works because the compiler that compiles our compiler knows
  // what '\n' actually is. In other words, we "inherit" the ASCII
  // code of '\n' from the compiler that compiles our compiler,
  // so we don't have to teach the actual code here.
  //
  // This fact has huge implications not only for the correctness
  // of the compiler but also for the security of the generated code.
  // For more info, read "Reflections on Trusting Trust" by Ken Thompson.
  // https://github.com/rui314/chibicc/wiki/thompson1984.pdf
  switch (*p) {
  case 'a': return '\a';
  case 'b': return '\b';
  case 't': return '\t';
  case 'n': return '\n';
  case 'v': return '\v';
  case 'f': return '\f';
  case 'r': return '\r';
  // [GNU] \e for the ASCII escape character is a GNU C extension.
  case 'e': return 27;
  default: return *p;
  }
}

// Find a closing double-引号.
static char *字符串_字面_终(char *p) {
  char *开启 = p;
  for (; *p != '"'; p++) {
    if (*p == '\n' || *p == '\0')
      错误_位置(开启, "unclosed string literal");
    if (*p == '\\')
      p++;
  }
  return p;
}

static 牌 *读_字符串_字面(char *开启, char *引号) {
  char *终 = 字符串_字面_终(引号 + 1);
  char *缓冲_短 = calloc(1, 终 - 引号);
  int 长度_短 = 0;

  for (char *p = 引号 + 1; p < 终;) {
    if (*p == '\\')
      缓冲_短[长度_短++] = 读_已转义_字符(&p, p + 1);
    else
      缓冲_短[长度_短++] = *p++;
  }

  牌 *牌_短 = new_token(牌_缩_串字面, 开启, 终 + 1);
  牌_短->类型_缩 = 数组_阵列(类型_字符_小, 长度_短 + 1);
  牌_短->串_短 = 缓冲_短;
  return 牌_短;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-16.
//
// UTF-16 is yet another variable-width encoding for Unicode. Code
// points smaller than U+10000 are encoded in 2 bytes. Code points
// 等于 to or larger than that are encoded in 4 bytes. Each 2 bytes
// in the 4 byte sequence is called "surrogate", and a 4 byte sequence
// is called a "surrogate pair".
static 牌 *读_utf16_字符串_字面(char *开启, char *引号) {
  char *终 = 字符串_字面_终(引号 + 1);
  uint16_t *缓冲_短 = calloc(2, 终 - 开启);
  int 长度_短 = 0;

  for (char *p = 引号 + 1; p < 终;) {
    if (*p == '\\') {
      缓冲_短[长度_短++] = 读_已转义_字符(&p, p + 1);
      continue;
    }

    uint32_t c = 解码_万国码8(&p, p);
    if (c < 0x10000) {
      // Encode a code point in 2 bytes.
      缓冲_短[长度_短++] = c;
    } else {
      // Encode a code point in 4 bytes.
      c -= 0x10000;
      缓冲_短[长度_短++] = 0xd800 + ((c >> 10) & 0x3ff);
      缓冲_短[长度_短++] = 0xdc00 + (c & 0x3ff);
    }
  }

  牌 *牌_短 = new_token(牌_缩_串字面, 开启, 终 + 1);
  牌_短->类型_缩 = 数组_阵列(类型_无符号短的, 长度_短 + 1);
  牌_短->串_短 = (char *)缓冲_短;
  return 牌_短;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-32.
//
// UTF-32 is a fixed-width encoding for Unicode. Each code point is
// encoded in 4 bytes.
static 牌 *读_utf32_字符串_字面(char *开启, char *引号, 类型 *类型_缩) {
  char *终 = 字符串_字面_终(引号 + 1);
  uint32_t *缓冲_短 = calloc(4, 终 - 引号);
  int 长度_短 = 0;

  for (char *p = 引号 + 1; p < 终;) {
    if (*p == '\\')
      缓冲_短[长度_短++] = 读_已转义_字符(&p, p + 1);
    else
      缓冲_短[长度_短++] = 解码_万国码8(&p, p);
  }

  牌 *牌_短 = new_token(牌_缩_串字面, 开启, 终 + 1);
  牌_短->类型_缩 = 数组_阵列(类型_缩, 长度_短 + 1);
  牌_短->串_短 = (char *)缓冲_短;
  return 牌_短;
}

static 牌 *读_字符_字面(char *开启, char *引号, 类型 *类型_缩) {
  char *p = 引号 + 1;
  if (*p == '\0')
    错误_位置(开启, "unclosed char literal");

  int c;
  if (*p == '\\')
    c = 读_已转义_字符(&p, p + 1);
  else
    c = 解码_万国码8(&p, p);

  char *终 = strchr(p, '\'');
  if (!终)
    错误_位置(p, "unclosed char literal");

  牌 *牌_短 = new_token(牌_缩_数值字面, 开启, 终 + 1);
  牌_短->值_短 = c;
  牌_短->类型_缩 = 类型_缩;
  return 牌_短;
}

static bool 转化_预处理_整型(牌 *牌_短) {
  char *p = 牌_短->位置_短;

  // Read a 二元_小写, octal, decimal or hexadecimal number.
  int 基础 = 10;
  if (!strncasecmp(p, "0x", 2) && isxdigit(p[2])) {
    p += 2;
    基础 = 16;
  } else if (!strncasecmp(p, "0b", 2) && (p[2] == '0' || p[2] == '1')) {
    p += 2;
    基础 = 2;
  } else if (*p == '0') {
    基础 = 8;
  }

  int64_t 值_短 = strtoul(p, &p, 基础);

  // Read U, L or LL suffixes.
  bool l = false;
  bool u = false;

  if (起始于(p, "LLU") || 起始于(p, "LLu") ||
      起始于(p, "llU") || 起始于(p, "llu") ||
      起始于(p, "ULL") || 起始于(p, "Ull") ||
      起始于(p, "uLL") || 起始于(p, "ull")) {
    p += 3;
    l = u = true;
  } else if (!strncasecmp(p, "lu", 2) || !strncasecmp(p, "ul", 2)) {
    p += 2;
    l = u = true;
  } else if (起始于(p, "LL") || 起始于(p, "ll")) {
    p += 2;
    l = true;
  } else if (*p == 'L' || *p == 'l') {
    p++;
    l = true;
  } else if (*p == 'U' || *p == 'u') {
    p++;
    u = true;
  }

  if (p != 牌_短->位置_短 + 牌_短->长度_短)
    return false;

  // Infer a type.
  类型 *类型_缩;
  if (基础 == 10) {
    if (l && u)
      类型_缩 = 类型_无符号长的;
    else if (l)
      类型_缩 = 类型_长的_小;
    else if (u)
      类型_缩 = (值_短 >> 32) ? 类型_无符号长的 : 类型_无符号整型;
    else
      类型_缩 = (值_短 >> 31) ? 类型_长的_小 : 类型_整型_小;
  } else {
    if (l && u)
      类型_缩 = 类型_无符号长的;
    else if (l)
      类型_缩 = (值_短 >> 63) ? 类型_无符号长的 : 类型_长的_小;
    else if (u)
      类型_缩 = (值_短 >> 32) ? 类型_无符号长的 : 类型_无符号整型;
    else if (值_短 >> 63)
      类型_缩 = 类型_无符号长的;
    else if (值_短 >> 32)
      类型_缩 = 类型_长的_小;
    else if (值_短 >> 31)
      类型_缩 = 类型_无符号整型;
    else
      类型_缩 = 类型_整型_小;
  }

  牌_短->种类 = 牌_缩_数值字面;
  牌_短->值_短 = 值_短;
  牌_短->类型_缩 = 类型_缩;
  return true;
}

// The definition of the numeric literal at the preprocessing stage
// is more relaxed than the definition of that at the later stages.
// In order to handle that, a numeric literal is tokenized as a
// "pp-number" token first and 那么 converted to a regular number
// token after preprocessing.
//
// This 正函数_ converts a pp-number token to a regular number token.
static void 转化_预处理_数号(牌 *牌_短) {
  // Try to 解析 as an integer constant.
  if (转化_预处理_整型(牌_短))
    return;

  // If it's not an integer, it must be a floating point constant.
  char *终;
  long double 值_短 = strtold(牌_短->位置_短, &终);

  类型 *类型_缩;
  if (*终 == 'f' || *终 == 'F') {
    类型_缩 = 类型_浮点_小;
    终++;
  } else if (*终 == 'l' || *终 == 'L') {
    类型_缩 = 类型_长双浮_小;
    终++;
  } else {
    类型_缩 = 类型_双浮_小;
  }

  if (牌_短->位置_短 + 牌_短->长度_短 != 终)
    错误_牌_短(牌_短, "invalid numeric constant");

  牌_短->种类 = 牌_缩_数值字面;
  牌_短->浮点值_短 = 值_短;
  牌_短->类型_缩 = 类型_缩;
}

void 转换_预处理_牌(牌 *牌_短) {
  for (牌 *t = 牌_短; t->种类 != 牌_缩_文件终; t = t->下一个) {
    if (是否_关键字(t))
      t->种类 = 牌_缩_关键字;
    else if (t->种类 == 牌_缩_预处理号码)
      转化_预处理_数号(t);
  }
}

// Initialize 行 info for all tokens.
static void 添加_行_数号(牌 *牌_短) {
  char *p = 当前_文件->内容;
  int n = 1;

  do {
    if (p == 牌_短->位置_短) {
      牌_短->行_号 = n;
      牌_短 = 牌_短->下一个;
    }
    if (*p == '\n')
      n++;
  } while (*p++);
}

牌 *化为牌_字符串_字面(牌 *牌_短, 类型 *基本类型) {
  牌 *t;
  if (基本类型->大小 == 2)
    t = 读_utf16_字符串_字面(牌_短->位置_短, 牌_短->位置_短);
  else
    t = 读_utf32_字符串_字面(牌_短->位置_短, 牌_短->位置_短, 基本类型);
  t->下一个 = 牌_短->下一个;
  return t;
}

// Tokenize a given string and returns 新的 tokens.
牌 *化为牌(文件_大写 *源文件位置) {
  当前_文件 = 源文件位置;

  char *p = 源文件位置->内容;
  牌 头部 = {};
  牌 *当前_小 = &头部;

  位置_行开始 = true;
  有_空格字符 = false;

  while (*p) {
    // Skip 行 comments.
    if (起始于(p, "//")) {
      p += 2;
      while (*p != '\n')
        p++;
      有_空格字符 = true;
      continue;
    }

    // Skip block comments.
    if (起始于(p, "/*")) {
      char *q = strstr(p + 2, "*/");
      if (!q)
        错误_位置(p, "unclosed block comment");
      p = q + 2;
      有_空格字符 = true;
      continue;
    }

    // Skip newline.
    if (*p == '\n') {
      p++;
      位置_行开始 = true;
      有_空格字符 = false;
      continue;
    }

    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      有_空格字符 = true;
      continue;
    }

    // Numeric literal
    if (isdigit(*p) || (*p == '.' && isdigit(p[1]))) {
      char *q = p++;
      for (;;) {
        if (p[0] && p[1] && strchr("eEpP", p[0]) && strchr("+-", p[1]))
          p += 2;
        else if (isalnum(*p) || *p == '.')
          p++;
        else
          break;
      }
      当前_小 = 当前_小->下一个 = new_token(牌_缩_预处理号码, q, p);
      continue;
    }

    // String literal
    if (*p == '"') {
      当前_小 = 当前_小->下一个 = 读_字符串_字面(p, p);
      p += 当前_小->长度_短;
      continue;
    }

    // UTF-8 string literal
    if (起始于(p, "u8\"")) {
      当前_小 = 当前_小->下一个 = 读_字符串_字面(p, p + 2);
      p += 当前_小->长度_短;
      continue;
    }

    // UTF-16 string literal
    if (起始于(p, "u\"")) {
      当前_小 = 当前_小->下一个 = 读_utf16_字符串_字面(p, p + 1);
      p += 当前_小->长度_短;
      continue;
    }

    // Wide string literal
    if (起始于(p, "L\"")) {
      当前_小 = 当前_小->下一个 = 读_utf32_字符串_字面(p, p + 1, 类型_整型_小);
      p += 当前_小->长度_短;
      continue;
    }

    // UTF-32 string literal
    if (起始于(p, "U\"")) {
      当前_小 = 当前_小->下一个 = 读_utf32_字符串_字面(p, p + 1, 类型_无符号整型);
      p += 当前_小->长度_短;
      continue;
    }

    // Character literal
    if (*p == '\'') {
      当前_小 = 当前_小->下一个 = 读_字符_字面(p, p, 类型_整型_小);
      当前_小->值_短 = (char)当前_小->值_短;
      p += 当前_小->长度_短;
      continue;
    }

    // UTF-16 character literal
    if (起始于(p, "u'")) {
      当前_小 = 当前_小->下一个 = 读_字符_字面(p, p + 1, 类型_无符号短的);
      当前_小->值_短 &= 0xffff;
      p += 当前_小->长度_短;
      continue;
    }

    // Wide character literal
    if (起始于(p, "L'")) {
      当前_小 = 当前_小->下一个 = 读_字符_字面(p, p + 1, 类型_整型_小);
      p += 当前_小->长度_短;
      continue;
    }

    // UTF-32 character literal
    if (起始于(p, "U'")) {
      当前_小 = 当前_小->下一个 = 读_字符_字面(p, p + 1, 类型_无符号整型);
      p += 当前_小->长度_短;
      continue;
    }

    // Identifier or keyword
    int 标识_长度 = 读_标识(p);
    if (标识_长度) {
      当前_小 = 当前_小->下一个 = new_token(牌_缩_标识符, p, p + 标识_长度);
      p += 当前_小->长度_短;
      continue;
    }

    // Punctuators
    int 标点符号_长度 = 读_标点符号(p);
    if (标点符号_长度) {
      当前_小 = 当前_小->下一个 = new_token(牌_缩_标点符号, p, p + 标点符号_长度);
      p += 当前_小->长度_短;
      continue;
    }

    错误_位置(p, "invalid token");
  }

  当前_小 = 当前_小->下一个 = new_token(牌_缩_文件终, p, p);
  添加_行_数号(头部.下一个);
  return 头部.下一个;
}

// Returns the 内容 of a given 源文件位置.
static char *读_文件(char *路径) {
  FILE *浮点_;

  if (strcmp(路径, "-") == 0) {
    // By convention, read from stdin if a given 文件名 is "-".
    浮点_ = stdin;
  } else {
    浮点_ = fopen(路径, "r");
    if (!浮点_)
      return NULL;
  }

  char *缓冲_短;
  size_t 缓冲长度;
  FILE *out = open_memstream(&缓冲_短, &缓冲长度);

  // Read the entire 源文件位置.
  for (;;) {
    char 缓冲2[4096];
    int n = fread(缓冲2, 1, sizeof(缓冲2), 浮点_);
    if (n == 0)
      break;
    fwrite(缓冲2, 1, n, out);
  }

  if (浮点_ != stdin)
    fclose(浮点_);

  // Make sure that the last 行 is properly terminated with '\n'.
  fflush(out);
  if (缓冲长度 == 0 || 缓冲_短[缓冲长度 - 1] != '\n')
    fputc('\n', out);
  fputc('\0', out);
  fclose(out);
  return 缓冲_短;
}

文件_大写 **取_输入_文件(void) {
  return 输入_文件;
}

文件_大写 *新_文件(char *名字, int 文件_号, char *内容) {
  文件_大写 *源文件位置 = calloc(1, sizeof(文件_大写));
  源文件位置->名字 = 名字;
  源文件位置->显示_名字 = 名字;
  源文件位置->文件_号 = 文件_号;
  源文件位置->内容 = 内容;
  return 源文件位置;
}

// Replaces \r or \r\n with \n.
static void 规范化_新行(char *p) {
  int i = 0, j = 0;

  while (p[i]) {
    if (p[i] == '\r' && p[i + 1] == '\n') {
      i += 2;
      p[j++] = '\n';
    } else if (p[i] == '\r') {
      i++;
      p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }

  p[j] = '\0';
}

// Removes backslashes followed by a newline.
static void 移除_反斜杠_新行(char *p) {
  int i = 0, j = 0;

  // We want to keep the number of newline characters so that
  // the logical 行 number matches the physical one.
  // This 计数器_ maintain the number of newlines we have removed.
  int n = 0;

  while (p[i]) {
    if (p[i] == '\\' && p[i + 1] == '\n') {
      i += 2;
      n++;
    } else if (p[i] == '\n') {
      p[j++] = p[i++];
      for (; n > 0; n--)
        p[j++] = '\n';
    } else {
      p[j++] = p[i++];
    }
  }

  for (; n > 0; n--)
    p[j++] = '\n';
  p[j] = '\0';
}

static uint32_t 读_普遍的_字符(char *p, int 长度_短) {
  uint32_t c = 0;
  for (int i = 0; i < 长度_短; i++) {
    if (!isxdigit(p[i]))
      return 0;
    c = (c << 4) | 来自_十六进制_短(p[i]);
  }
  return c;
}

// Replace \u or \U escape sequences with corresponding UTF-8 bytes.
static void 转化_普遍的_字符(char *p) {
  char *q = p;

  while (*p) {
    if (起始于(p, "\\u")) {
      uint32_t c = 读_普遍的_字符(p + 2, 4);
      if (c) {
        p += 6;
        q += 编码_万国码8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (起始于(p, "\\U")) {
      uint32_t c = 读_普遍的_字符(p + 2, 8);
      if (c) {
        p += 10;
        q += 编码_万国码8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (p[0] == '\\') {
      *q++ = *p++;
      *q++ = *p++;
    } else {
      *q++ = *p++;
    }
  }

  *q = '\0';
}

牌 *化为牌_文件(char *路径) {
  char *p = 读_文件(路径);
  if (!p)
    return NULL;

  // UTF-8 texts may 开启 with a 3-byte "BOM" marker sequence.
  // If exists, just 跳过 them because they are useless bytes.
  // (It is actually not recommended to 加法_ BOM markers to UTF-8
  // texts, but it's not uncommon particularly on Windows.)
  if (!memcmp(p, "\xef\xbb\xbf", 3))
    p += 3;

  规范化_新行(p);
  移除_反斜杠_新行(p);
  转化_普遍的_字符(p);

  // Save the 文件名 for assembler .源文件位置 directive.
  static int 文件_号;
  文件_大写 *源文件位置 = 新_文件(路径, 文件_号 + 1, p);

  // Save the 文件名 for assembler .源文件位置 directive.
  输入_文件 = realloc(输入_文件, sizeof(char *) * (文件_号 + 2));
  输入_文件[文件_号] = 源文件位置;
  输入_文件[文件_号 + 1] = NULL;
  文件_号++;

  return 化为牌(源文件位置);
}
