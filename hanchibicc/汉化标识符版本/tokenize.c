#include "chibicc.h"

// Input 文件_小写
static 文件_大写 *当前_文件;

// A list of all input files.
static 文件_大写 **输入_文件们;

// True if the current position is at the beginning of a 行号_小写
static bool 在_行开头;

// True if the current position follows a space character
static bool 有_空格;

// Reports an 错误_小写 and exit.
void 错误_小写(char *格式_缩写, ...) {
  va_list ap;
  va_start(ap, 格式_缩写);
  vfprintf(stderr, 格式_缩写, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// Reports an 错误_小写 message in the following 格式化_函.
//
// foo.c:10: x = y + 1;
//               ^ <错误_小写 message here>
static void 啰嗦错误_在(char *文件名_小写, char *input, int 行号_数目,
                      char *定位_小写_短, char *格式_缩写, va_list ap) {
  // Find a 行号_小写 containing `定位_小写_短`.
  char *行号_小写 = 定位_小写_短;
  while (input < 行号_小写 && 行号_小写[-1] != '\n')
    行号_小写--;

  char *终_小写 = 定位_小写_短;
  while (*终_小写 && *终_小写 != '\n')
    终_小写++;

  // Print out the 行号_小写.
  int 标识_短_小写 = fprintf(stderr, "%s:%d: ", 文件名_小写, 行号_数目);
  fprintf(stderr, "%.*s\n", (int)(终_小写 - 行号_小写), 行号_小写);

  // Show the 错误_小写 message.
  int pos = 显示_宽度(行号_小写, 定位_小写_短 - 行号_小写) + 标识_短_小写;

  fprintf(stderr, "%*s", pos, ""); // print pos spaces.
  fprintf(stderr, "^ ");
  vfprintf(stderr, 格式_缩写, ap);
  fprintf(stderr, "\n");
}

void 错误_在_小写(char *定位_小写_短, char *格式_缩写, ...) {
  int 行号_数目 = 1;
  for (char *p = 当前_文件->内容; p < 定位_小写_短; p++)
    if (*p == '\n')
      行号_数目++;

  va_list ap;
  va_start(ap, 格式_缩写);
  啰嗦错误_在(当前_文件->名称_小写, 当前_文件->内容, 行号_数目, 定位_小写_短, 格式_缩写, ap);
  exit(1);
}

void 错误_牌_小写(牌 *牌_短_小写, char *格式_缩写, ...) {
  va_list ap;
  va_start(ap, 格式_缩写);
  啰嗦错误_在(牌_短_小写->文件_小写->名称_小写, 牌_短_小写->文件_小写->内容, 牌_短_小写->行号_数目, 牌_短_小写->定位_小写_短, 格式_缩写, ap);
  exit(1);
}

void 警告_牌_短(牌 *牌_短_小写, char *格式_缩写, ...) {
  va_list ap;
  va_start(ap, 格式_缩写);
  啰嗦错误_在(牌_短_小写->文件_小写->名称_小写, 牌_短_小写->文件_小写->内容, 牌_短_小写->行号_数目, 牌_短_小写->定位_小写_短, 格式_缩写, ap);
  va_end(ap);
}

// Consumes the current token if it matches `op`.
bool 相等吗_小写(牌 *牌_短_小写, char *op) {
  return memcmp(牌_短_小写->定位_小写_短, op, 牌_短_小写->长度_短) == 0 && op[牌_短_小写->长度_短] == '\0';
}

// Ensure that the current token is `op`.
牌 *跳过_小写(牌 *牌_短_小写, char *op) {
  if (!相等吗_小写(牌_短_小写, op))
    错误_牌_小写(牌_短_小写, "expected '%s'", op);
  return 牌_短_小写->下一个_小写;
}

bool 消耗吗_小写(牌 **其余的, 牌 *牌_短_小写, char *串_小写_短) {
  if (相等吗_小写(牌_短_小写, 串_小写_短)) {
    *其余的 = 牌_短_小写->下一个_小写;
    return true;
  }
  *其余的 = 牌_短_小写;
  return false;
}

// Create a new token.
static 牌 *新_牌_小写(牌种类 种类_小写, char *开起, char *终_小写) {
  牌 *牌_短_小写 = calloc(1, sizeof(牌));
  牌_短_小写->种类_小写 = 种类_小写;
  牌_短_小写->定位_小写_短 = 开起;
  牌_短_小写->长度_短 = 终_小写 - 开起;
  牌_短_小写->文件_小写 = 当前_文件;
  牌_短_小写->文件名_小写 = 当前_文件->显示_名称;
  牌_短_小写->在_行开头 = 在_行开头;
  牌_短_小写->有_空格 = 有_空格;

  在_行开头 = 有_空格 = false;
  return 牌_短_小写;
}

static bool 以什么开始(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

// Read an identifier and returns the length of it.
// If p does not point to a valid identifier, 0 is returned.
static int 读_标识(char *开起) {
  char *p = 开起;
  uint32_t c = 解码_万国码变形格式8(&p, p);
  if (!是否_标识1(c))
    return 0;

  for (;;) {
    char *q;
    c = 解码_万国码变形格式8(&q, p);
    if (!是否_标识2(c))
      return p - 开起;
    p = q;
  }
}

static int 来自_十六进制(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  if ('a' <= c && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

// Read a punctuator token from p and returns its length.
static int 读_标点符号_短(char *p) {
  static char *关键字_缩[] = {
    "<<=", ">>=", "...", "==", "!=", "<=", ">=", "->", "+=",
    "-=", "*=", "/=", "++", "--", "%=", "&=", "|=", "^=", "&&",
    "||", "<<", ">>", "##",
  };

  for (int i = 0; i < sizeof(关键字_缩) / sizeof(*关键字_缩); i++)
    if (以什么开始(p, 关键字_缩[i]))
      return strlen(关键字_缩[i]);

  return ispunct(*p) ? 1 : 0;
}

static bool 是否_关键字(牌 *牌_短_小写) {
  static 哈希映射_驼峰名 映射_小写;

  if (映射_小写.容量 == 0) {
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
      哈希映射_放置(&映射_小写, 关键字_缩[i], (void *)1);
  }

  return 哈希映射_获取2(&映射_小写, 牌_短_小写->定位_小写_短, 牌_短_小写->长度_短);
}

static int 读_已跳出_印刻(char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // Read an octal number.
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;
    return c;
  }

  if (*p == 'x') {
    // Read a hexadecimal number.
    p++;
    if (!isxdigit(*p))
      错误_在_小写(p, "invalid hex escape sequence");

    int c = 0;
    for (; isxdigit(*p); p++)
      c = (c << 4) + 来自_十六进制(*p);
    *new_pos = p;
    return c;
  }

  *new_pos = p + 1;

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

// Find a closing double-quote.
static char *字符串_字面_终(char *p) {
  char *开起 = p;
  for (; *p != '"'; p++) {
    if (*p == '\n' || *p == '\0')
      错误_在_小写(开起, "unclosed string literal");
    if (*p == '\\')
      p++;
  }
  return p;
}

static 牌 *读_字符串_字面(char *开起, char *quote) {
  char *终_小写 = 字符串_字面_终(quote + 1);
  char *buf = calloc(1, 终_小写 - quote);
  int 长度_短 = 0;

  for (char *p = quote + 1; p < 终_小写;) {
    if (*p == '\\')
      buf[长度_短++] = 读_已跳出_印刻(&p, p + 1);
    else
      buf[长度_短++] = *p++;
  }

  牌 *牌_短_小写 = 新_牌_小写(牌_串字面, 开起, 终_小写 + 1);
  牌_短_小写->类型_小写_短 = 数组_的(类型_印刻_小写, 长度_短 + 1);
  牌_短_小写->串_小写_短 = buf;
  return 牌_短_小写;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-16.
//
// UTF-16 is yet another variable-width encoding for Unicode. Code
// points smaller than U+10000 are encoded in 2 bytes. Code points
// 相等吗_小写 to or larger than that are encoded in 4 bytes. Each 2 bytes
// in the 4 byte sequence is called "surrogate", and a 4 byte sequence
// is called a "surrogate pair".
static 牌 *读_万国码变形格式16_字符串_字面(char *开起, char *quote) {
  char *终_小写 = 字符串_字面_终(quote + 1);
  uint16_t *buf = calloc(2, 终_小写 - 开起);
  int 长度_短 = 0;

  for (char *p = quote + 1; p < 终_小写;) {
    if (*p == '\\') {
      buf[长度_短++] = 读_已跳出_印刻(&p, p + 1);
      continue;
    }

    uint32_t c = 解码_万国码变形格式8(&p, p);
    if (c < 0x10000) {
      // Encode a code point in 2 bytes.
      buf[长度_短++] = c;
    } else {
      // Encode a code point in 4 bytes.
      c -= 0x10000;
      buf[长度_短++] = 0xd800 + ((c >> 10) & 0x3ff);
      buf[长度_短++] = 0xdc00 + (c & 0x3ff);
    }
  }

  牌 *牌_短_小写 = 新_牌_小写(牌_串字面, 开起, 终_小写 + 1);
  牌_短_小写->类型_小写_短 = 数组_的(类型_无符短的_小写, 长度_短 + 1);
  牌_短_小写->串_小写_短 = (char *)buf;
  return 牌_短_小写;
}

// Read a UTF-8-encoded string literal and transcode it in UTF-32.
//
// UTF-32 is a fixed-width encoding for Unicode. Each code point is
// encoded in 4 bytes.
static 牌 *读_万国码变形格式32_字符串_字面(char *开起, char *quote, 类型 *类型_小写_短) {
  char *终_小写 = 字符串_字面_终(quote + 1);
  uint32_t *buf = calloc(4, 终_小写 - quote);
  int 长度_短 = 0;

  for (char *p = quote + 1; p < 终_小写;) {
    if (*p == '\\')
      buf[长度_短++] = 读_已跳出_印刻(&p, p + 1);
    else
      buf[长度_短++] = 解码_万国码变形格式8(&p, p);
  }

  牌 *牌_短_小写 = 新_牌_小写(牌_串字面, 开起, 终_小写 + 1);
  牌_短_小写->类型_小写_短 = 数组_的(类型_小写_短, 长度_短 + 1);
  牌_短_小写->串_小写_短 = (char *)buf;
  return 牌_短_小写;
}

static 牌 *读_印刻_字面(char *开起, char *quote, 类型 *类型_小写_短) {
  char *p = quote + 1;
  if (*p == '\0')
    错误_在_小写(开起, "unclosed char literal");

  int c;
  if (*p == '\\')
    c = 读_已跳出_印刻(&p, p + 1);
  else
    c = 解码_万国码变形格式8(&p, p);

  char *终_小写 = strchr(p, '\'');
  if (!终_小写)
    错误_在_小写(p, "unclosed char literal");

  牌 *牌_短_小写 = 新_牌_小写(牌_数值字面, 开起, 终_小写 + 1);
  牌_短_小写->值_小写_短 = c;
  牌_短_小写->类型_小写_短 = 类型_小写_短;
  return 牌_短_小写;
}

static bool 转换_预处理器_整型(牌 *牌_短_小写) {
  char *p = 牌_短_小写->定位_小写_短;

  // Read a binary, octal, decimal or hexadecimal number.
  int 基础_小写 = 10;
  if (!strncasecmp(p, "0x", 2) && isxdigit(p[2])) {
    p += 2;
    基础_小写 = 16;
  } else if (!strncasecmp(p, "0b", 2) && (p[2] == '0' || p[2] == '1')) {
    p += 2;
    基础_小写 = 2;
  } else if (*p == '0') {
    基础_小写 = 8;
  }

  int64_t 值_小写_短 = strtoul(p, &p, 基础_小写);

  // Read U, L or LL suffixes.
  bool l = false;
  bool u = false;

  if (以什么开始(p, "LLU") || 以什么开始(p, "LLu") ||
      以什么开始(p, "llU") || 以什么开始(p, "llu") ||
      以什么开始(p, "ULL") || 以什么开始(p, "Ull") ||
      以什么开始(p, "uLL") || 以什么开始(p, "ull")) {
    p += 3;
    l = u = true;
  } else if (!strncasecmp(p, "lu", 2) || !strncasecmp(p, "ul", 2)) {
    p += 2;
    l = u = true;
  } else if (以什么开始(p, "LL") || 以什么开始(p, "ll")) {
    p += 2;
    l = true;
  } else if (*p == 'L' || *p == 'l') {
    p++;
    l = true;
  } else if (*p == 'U' || *p == 'u') {
    p++;
    u = true;
  }

  if (p != 牌_短_小写->定位_小写_短 + 牌_短_小写->长度_短)
    return false;

  // Infer a type.
  类型 *类型_小写_短;
  if (基础_小写 == 10) {
    if (l && u)
      类型_小写_短 = 类型_无符长的_小写;
    else if (l)
      类型_小写_短 = 类型_长的_小写;
    else if (u)
      类型_小写_短 = (值_小写_短 >> 32) ? 类型_无符长的_小写 : 类型_无符整型_小写;
    else
      类型_小写_短 = (值_小写_短 >> 31) ? 类型_长的_小写 : 类型_整型_小写;
  } else {
    if (l && u)
      类型_小写_短 = 类型_无符长的_小写;
    else if (l)
      类型_小写_短 = (值_小写_短 >> 63) ? 类型_无符长的_小写 : 类型_长的_小写;
    else if (u)
      类型_小写_短 = (值_小写_短 >> 32) ? 类型_无符长的_小写 : 类型_无符整型_小写;
    else if (值_小写_短 >> 63)
      类型_小写_短 = 类型_无符长的_小写;
    else if (值_小写_短 >> 32)
      类型_小写_短 = 类型_长的_小写;
    else if (值_小写_短 >> 31)
      类型_小写_短 = 类型_无符整型_小写;
    else
      类型_小写_短 = 类型_整型_小写;
  }

  牌_短_小写->种类_小写 = 牌_数值字面;
  牌_短_小写->值_小写_短 = 值_小写_短;
  牌_短_小写->类型_小写_短 = 类型_小写_短;
  return true;
}

// The definition of the numeric literal at the preprocessing stage
// is more relaxed than the definition of that at the later stages.
// In order to handle that, a numeric literal is tokenized as a
// "pp-number" token first and 那么_小写 converted to a regular number
// token after preprocessing.
//
// This 函数_全_小写 converts a pp-number token to a regular number token.
static void 转换_预处理器_数目(牌 *牌_短_小写) {
  // Try to 解析_小写 as an integer constant.
  if (转换_预处理器_整型(牌_短_小写))
    return;

  // If it's not an integer, it must be a floating point constant.
  char *终_小写;
  long double 值_小写_短 = strtold(牌_短_小写->定位_小写_短, &终_小写);

  类型 *类型_小写_短;
  if (*终_小写 == 'f' || *终_小写 == 'F') {
    类型_小写_短 = 类型_浮点_小写;
    终_小写++;
  } else if (*终_小写 == 'l' || *终_小写 == 'L') {
    类型_小写_短 = 类型_长双浮_小写;
    终_小写++;
  } else {
    类型_小写_短 = 类型_双浮_小写;
  }

  if (牌_短_小写->定位_小写_短 + 牌_短_小写->长度_短 != 终_小写)
    错误_牌_小写(牌_短_小写, "invalid numeric constant");

  牌_短_小写->种类_小写 = 牌_数值字面;
  牌_短_小写->浮点值_小写_短 = 值_小写_短;
  牌_短_小写->类型_小写_短 = 类型_小写_短;
}

void 转换_预处理器_牌们(牌 *牌_短_小写) {
  for (牌 *t = 牌_短_小写; t->种类_小写 != 牌_文件终; t = t->下一个_小写) {
    if (是否_关键字(t))
      t->种类_小写 = 牌_关键字;
    else if (t->种类_小写 == 牌_预处理器_数目)
      转换_预处理器_数目(t);
  }
}

// Initialize 行号_小写 info for all tokens.
static void 加_行号_数目们(牌 *牌_短_小写) {
  char *p = 当前_文件->内容;
  int n = 1;

  do {
    if (p == 牌_短_小写->定位_小写_短) {
      牌_短_小写->行号_数目 = n;
      牌_短_小写 = 牌_短_小写->下一个_小写;
    }
    if (*p == '\n')
      n++;
  } while (*p++);
}

牌 *化为牌_字符串_字面(牌 *牌_短_小写, 类型 *基本类型_短) {
  牌 *t;
  if (基本类型_短->大小_小写 == 2)
    t = 读_万国码变形格式16_字符串_字面(牌_短_小写->定位_小写_短, 牌_短_小写->定位_小写_短);
  else
    t = 读_万国码变形格式32_字符串_字面(牌_短_小写->定位_小写_短, 牌_短_小写->定位_小写_短, 基本类型_短);
  t->下一个_小写 = 牌_短_小写->下一个_小写;
  return t;
}

// Tokenize a given string and returns new tokens.
牌 *化为牌_小写(文件_大写 *文件_小写) {
  当前_文件 = 文件_小写;

  char *p = 文件_小写->内容;
  牌 head = {};
  牌 *当前_短 = &head;

  在_行开头 = true;
  有_空格 = false;

  while (*p) {
    // Skip 行号_小写 comments.
    if (以什么开始(p, "//")) {
      p += 2;
      while (*p != '\n')
        p++;
      有_空格 = true;
      continue;
    }

    // Skip block comments.
    if (以什么开始(p, "/*")) {
      char *q = strstr(p + 2, "*/");
      if (!q)
        错误_在_小写(p, "unclosed block comment");
      p = q + 2;
      有_空格 = true;
      continue;
    }

    // Skip newline.
    if (*p == '\n') {
      p++;
      在_行开头 = true;
      有_空格 = false;
      continue;
    }

    // Skip whitespace characters.
    if (isspace(*p)) {
      p++;
      有_空格 = true;
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
      当前_短 = 当前_短->下一个_小写 = 新_牌_小写(牌_预处理器_数目, q, p);
      continue;
    }

    // String literal
    if (*p == '"') {
      当前_短 = 当前_短->下一个_小写 = 读_字符串_字面(p, p);
      p += 当前_短->长度_短;
      continue;
    }

    // UTF-8 string literal
    if (以什么开始(p, "u8\"")) {
      当前_短 = 当前_短->下一个_小写 = 读_字符串_字面(p, p + 2);
      p += 当前_短->长度_短;
      continue;
    }

    // UTF-16 string literal
    if (以什么开始(p, "u\"")) {
      当前_短 = 当前_短->下一个_小写 = 读_万国码变形格式16_字符串_字面(p, p + 1);
      p += 当前_短->长度_短;
      continue;
    }

    // Wide string literal
    if (以什么开始(p, "L\"")) {
      当前_短 = 当前_短->下一个_小写 = 读_万国码变形格式32_字符串_字面(p, p + 1, 类型_整型_小写);
      p += 当前_短->长度_短;
      continue;
    }

    // UTF-32 string literal
    if (以什么开始(p, "U\"")) {
      当前_短 = 当前_短->下一个_小写 = 读_万国码变形格式32_字符串_字面(p, p + 1, 类型_无符整型_小写);
      p += 当前_短->长度_短;
      continue;
    }

    // Character literal
    if (*p == '\'') {
      当前_短 = 当前_短->下一个_小写 = 读_印刻_字面(p, p, 类型_整型_小写);
      当前_短->值_小写_短 = (char)当前_短->值_小写_短;
      p += 当前_短->长度_短;
      continue;
    }

    // UTF-16 character literal
    if (以什么开始(p, "u'")) {
      当前_短 = 当前_短->下一个_小写 = 读_印刻_字面(p, p + 1, 类型_无符短的_小写);
      当前_短->值_小写_短 &= 0xffff;
      p += 当前_短->长度_短;
      continue;
    }

    // Wide character literal
    if (以什么开始(p, "L'")) {
      当前_短 = 当前_短->下一个_小写 = 读_印刻_字面(p, p + 1, 类型_整型_小写);
      p += 当前_短->长度_短;
      continue;
    }

    // UTF-32 character literal
    if (以什么开始(p, "U'")) {
      当前_短 = 当前_短->下一个_小写 = 读_印刻_字面(p, p + 1, 类型_无符整型_小写);
      p += 当前_短->长度_短;
      continue;
    }

    // Identifier or keyword
    int ident_len = 读_标识(p);
    if (ident_len) {
      当前_短 = 当前_短->下一个_小写 = 新_牌_小写(牌_标识, p, p + ident_len);
      p += 当前_短->长度_短;
      continue;
    }

    // Punctuators
    int punct_len = 读_标点符号_短(p);
    if (punct_len) {
      当前_短 = 当前_短->下一个_小写 = 新_牌_小写(牌_标点符号, p, p + punct_len);
      p += 当前_短->长度_短;
      continue;
    }

    错误_在_小写(p, "invalid token");
  }

  当前_短 = 当前_短->下一个_小写 = 新_牌_小写(牌_文件终, p, p);
  加_行号_数目们(head.下一个_小写);
  return head.下一个_小写;
}

// Returns the 内容 of a given 文件_小写.
static char *读_文件(char *path) {
  FILE *fp;

  if (strcmp(path, "-") == 0) {
    // By convention, read from stdin if a given 文件名_小写 is "-".
    fp = stdin;
  } else {
    fp = fopen(path, "r");
    if (!fp)
      return NULL;
  }

  char *buf;
  size_t buflen;
  FILE *out = open_memstream(&buf, &buflen);

  // Read the entire 文件_小写.
  for (;;) {
    char buf2[4096];
    int n = fread(buf2, 1, sizeof(buf2), fp);
    if (n == 0)
      break;
    fwrite(buf2, 1, n, out);
  }

  if (fp != stdin)
    fclose(fp);

  // Make sure that the last 行号_小写 is properly terminated with '\n'.
  fflush(out);
  if (buflen == 0 || buf[buflen - 1] != '\n')
    fputc('\n', out);
  fputc('\0', out);
  fclose(out);
  return buf;
}

文件_大写 **获取_输入_文件们(void) {
  return 输入_文件们;
}

文件_大写 *新_文件(char *名称_小写, int 文件_数目, char *内容) {
  文件_大写 *文件_小写 = calloc(1, sizeof(文件_大写));
  文件_小写->名称_小写 = 名称_小写;
  文件_小写->显示_名称 = 名称_小写;
  文件_小写->文件_数目 = 文件_数目;
  文件_小写->内容 = 内容;
  return 文件_小写;
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
  // the logical 行号_小写 number matches the physical one.
  // This 计数器_小写 maintain the number of newlines we have removed.
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

static uint32_t 读_普遍_印刻(char *p, int 长度_短) {
  uint32_t c = 0;
  for (int i = 0; i < 长度_短; i++) {
    if (!isxdigit(p[i]))
      return 0;
    c = (c << 4) | 来自_十六进制(p[i]);
  }
  return c;
}

// Replace \u or \U escape sequences with corresponding UTF-8 bytes.
static void 转换_普遍_印刻们(char *p) {
  char *q = p;

  while (*p) {
    if (以什么开始(p, "\\u")) {
      uint32_t c = 读_普遍_印刻(p + 2, 4);
      if (c) {
        p += 6;
        q += 编码_万国码变形格式8(q, c);
      } else {
        *q++ = *p++;
      }
    } else if (以什么开始(p, "\\U")) {
      uint32_t c = 读_普遍_印刻(p + 2, 8);
      if (c) {
        p += 10;
        q += 编码_万国码变形格式8(q, c);
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

牌 *化为牌_文件(char *path) {
  char *p = 读_文件(path);
  if (!p)
    return NULL;

  // UTF-8 texts may 开起 with a 3-byte "BOM" marker sequence.
  // If exists, just 跳过_小写 them because they are useless bytes.
  // (It is actually not recommended to 加_短_小写 BOM markers to UTF-8
  // texts, but it's not uncommon particularly on Windows.)
  if (!memcmp(p, "\xef\xbb\xbf", 3))
    p += 3;

  规范化_新行(p);
  移除_反斜杠_新行(p);
  转换_普遍_印刻们(p);

  // Save the 文件名_小写 for assembler .文件_小写 directive.
  static int 文件_数目;
  文件_大写 *文件_小写 = 新_文件(path, 文件_数目 + 1, p);

  // Save the 文件名_小写 for assembler .文件_小写 directive.
  输入_文件们 = realloc(输入_文件们, sizeof(char *) * (文件_数目 + 2));
  输入_文件们[文件_数目] = 文件_小写;
  输入_文件们[文件_数目 + 1] = NULL;
  文件_数目++;

  return 化为牌_小写(文件_小写);
}
