//han_tokenize.c

#include "han_chibicc.h"

// 输入文件
static File *current_file;

// 所有输入文件的列表
static File **input_files;

// 如果当前位置在行的开头，则为真
static bool at_bol;

// 如果当前位置之前有空格字符，则为真
static bool has_space;

// 报告错误并退出
void error(char *fmt, ...) {
  va_list ap; // 定义一个 va_list 类型的变量，用于存储可变参数的列表
  va_start(ap, fmt); // 初始化 va_list 变量，将其指向可变参数列表的起始位置
  vfprintf(stderr, fmt, ap); // 将格式化后的错误信息写入标准错误流中
  fprintf(stderr, "\n"); // 输出换行符
  exit(1); // 退出程序，返回状态码 1
}

// 以以下格式报告错误信息
//
// foo.c:10: x = y + 1;
//               ^ <错误信息>

static void verror_at(char *filename, char *input, int line_no,
                      char *loc, char *fmt, va_list ap) {
  // 找到包含 `loc` 的行
  char *line = loc; // 初始化 line 指针为 loc
  while (input < line && line[-1] != '\n') // 循环向前遍历，直到找到行的开头
    line--;

  char *end = loc; // 初始化 end 指针为 loc
  while (*end && *end != '\n') // 循环向后遍历，直到找到行的结尾
    end++;

  // 打印出该行
  int indent = fprintf(stderr, "%s:%d: ", filename, line_no); // 打印文件名和行号，并计算缩进量
  fprintf(stderr, "%.*s\n", (int)(end - line), line); // 打印行的内容

  // 显示错误信息
  int pos = display_width(line, loc - line) + indent; // 计算错误位置的偏移量

  fprintf(stderr, "%*s", pos, ""); // 打印 pos 个空格
  fprintf(stderr, "^ "); // 打印错误指示符
  vfprintf(stderr, fmt, ap); // 打印错误信息
  fprintf(stderr, "\n"); // 打印换行符
}

// 在指定位置报告错误
void error_at(char *loc, char *fmt, ...) {
  int line_no = 1; // 行号初始化为 1
  for (char *p = current_file->contents; p < loc; p++) // 遍历文件内容，计算行号
    if (*p == '\n')
      line_no++; // 如果遇到换行符，则行号加一

  va_list ap; // 定义一个 va_list 类型的变量，用于存储可变参数的列表
  va_start(ap, fmt); // 初始化 va_list 变量，将其指向可变参数列表的起始位置
  verror_at(current_file->name, current_file->contents, line_no, loc, fmt, ap); // 调用 verror_at 函数报告错误
  exit(1); // 退出程序，返回状态码 1
}

// 报告错误信息并退出
void error_tok(Token *tok, char *fmt, ...) {
  va_list ap; // 定义一个 va_list 类型的变量，用于存储可变参数的列表
  va_start(ap, fmt); // 初始化 va_list 变量，将其指向可变参数列表的起始位置
  verror_at(tok->file->name, tok->file->contents, tok->line_no, tok->loc, fmt, ap); // 调用 verror_at 函数报告错误
  exit(1); // 退出程序，返回状态码 1
}

// 发出警告信息
void warn_tok(Token *tok, char *fmt, ...) {
  va_list ap; // 定义一个 va_list 类型的变量，用于存储可变参数的列表
  va_start(ap, fmt); // 初始化 va_list 变量，将其指向可变参数列表的起始位置
  verror_at(tok->file->name, tok->file->contents, tok->line_no, tok->loc, fmt, ap); // 调用 verror_at 函数报告警告
  va_end(ap); // 结束可变参数的获取
}

// 如果当前的标记与op匹配，则消耗该标记
bool equal(Token *tok, char *op) {
  return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0'; // 返回比较当前标记的位置、给定操作符`op`和标记长度`tok->len`的结果是否为0，
}  // 并且检查操作符`op`的结尾是否为字符串结束符'\0'。

// 确保当前的标记是`op`。
Token *skip(Token *tok, char *op) {
  if (!equal(tok, op)) // 如果当前的标记与`op`不相等
    error_tok(tok, "expected '%s'", op); // 报告错误，提示期望的操作符
  return tok->next; // 返回下一个标记
}

// 消耗标记并检查是否与给定的字符串`str`相等。
bool consume(Token **rest, Token *tok, char *str) {
  if (equal(tok, str)) { // 如果当前的标记与`str`相等
    *rest = tok->next; // 将下一个标记存储在`rest`中
    return true; // 返回true表示成功消耗标记
  }
  *rest = tok; // 如果不相等，则将当前标记存储在`rest`中
  return false; // 返回false表示未成功消耗标记
}

// 创建一个新的标记。
static Token *new_token(TokenKind kind, char *start, char *end) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind; // 设置标记的类型
  tok->loc = start; // 设置标记的起始位置
  tok->len = end - start; // 计算标记的长度
  tok->file = current_file; // 设置标记所属的文件
  tok->filename = current_file->display_name; // 设置标记所属文件的显示名称
  tok->at_bol = at_bol; // 设置标记是否在行首
  tok->has_space = has_space; // 设置标记是否有空格

  at_bol = has_space = false; // 重置行首和空格标志
  return tok; // 返回创建的标记
}

// 检查字符串`p`是否以字符串`q`开头。
static bool startswith(char *p, char *q) {
  return strncmp(p, q, strlen(q)) == 0; // 使用strncmp函数比较字符串
}

// 读取一个标识符并返回其长度。
// 如果p不指向有效的标识符，则返回0。
// 从指定的起始位置读取一个标识符，并返回其长度。
static int read_ident(char *start) {
  char *p = start; // 指针p指向起始位置
  uint32_t c = decode_utf8(&p, p); // 解码UTF-8字符，并将指针p移动到下一个字符的位置
  if (!is_ident1(c)) // 如果c不是标识符的第一个字符
    return 0; // 返回0表示不是有效的标识符

  for (;;) {
    char *q; // 临时指针q
    c = decode_utf8(&q, p); // 解码UTF-8字符，并将指针q移动到下一个字符的位置
    if (!is_ident2(c)) // 如果c不是标识符的后续字符
      return p - start; // 返回标识符的长度
    p = q; // 将指针p移动到下一个字符的位置
  }
}

// 将十六进制字符转换为对应的整数值。
static int from_hex(char c) {
  if ('0' <= c && c <= '9') // 如果c是0到9之间的字符
    return c - '0'; // 返回对应的整数值
  if ('a' <= c && c <= 'f') // 如果c是小写字母a到f之间的字符
    return c - 'a' + 10; // 返回对应的整数值
  return c - 'A' + 10; // 否则，c是大写字母A到F之间的字符，返回对应的整数值
}

// 从p读取一个标点符号标记，并返回其长度。
static int read_punct(char *p) {
  static char *kw[] = {
    "<<=", ">>=", "...", "==", "!=", "<=", ">=", "->", "+=",
    "-=", "*=", "/=", "++", "--", "%=", "&=", "|=", "^=", "&&",
    "||", "<<", ">>", "##",
  };

  // 定义一个静态字符指针数组kw，包含了一些特殊的运算符和符号

  for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)
    // 遍历kw数组，判断传入的字符串p是否以kw数组中的元素开头

    if (startswith(p, kw[i]))
      // 如果p以kw数组中的元素开头，则返回kw[i]的长度

      return strlen(kw[i]);

  // 如果p不以kw数组中的任何元素开头，则判断p的第一个字符是否为标点符号

  return ispunct(*p) ? 1 : 0;
}

static bool is_keyword(Token *tok) {
  static HashMap map;   // 定义一个静态的HashMap变量map，用于存储关键字

  if (map.capacity == 0) {      // 如果map的容量为0，表示map还未初始化
    static char *kw[] = {
      "return", "if", "else", "for", "while", "int", "sizeof", "char",
      "struct", "union", "short", "long", "void", "typedef", "_Bool",
      "enum", "static", "goto", "break", "continue", "switch", "case",
      "default", "extern", "_Alignof", "_Alignas", "do", "signed",
      "unsigned", "const", "volatile", "auto", "register", "restrict",
      "__restrict", "__restrict__", "_Noreturn", "float", "double",
      "typeof", "asm", "_Thread_local", "__thread", "_Atomic",
      "__attribute__",
    };     // 定义一个静态字符指针数组kw，包含了C语言的关键字

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++)      // 遍历kw数组，将关键字作为键，值为1存入map中
      hashmap_put(&map, kw[i], (void *)1);
  }    

  return hashmap_get2(&map, tok->loc, tok->len);  // 根据传入的Token结构体指针tok的位置和长度，从map中查找对应的关键字
}

static int read_escaped_char(char **new_pos, char *p) {
  if ('0' <= *p && *p <= '7') {
    // 如果字符p是一个八进制数字，则读取一个八进制数
    int c = *p++ - '0';
    if ('0' <= *p && *p <= '7') {
      // 如果字符p的下一个字符也是八进制数字，则将其加入到c中
      c = (c << 3) + (*p++ - '0');
      if ('0' <= *p && *p <= '7')
        // 如果字符p的下下个字符也是八进制数字，则将其加入到c中
        c = (c << 3) + (*p++ - '0');
    }
    *new_pos = p;  // 更新new_pos的值为p的地址
    return c;  // 返回读取的八进制数
  }

  if (*p == 'x') {
    // 如果字符p是一个十六进制数字，则读取一个十六进制数
    p++;
    if (!isxdigit(*p))
      error_at(p, "invalid hex escape sequence");

    int c = 0;
    for (; isxdigit(*p); p++)
      // 将字符p转换为对应的十六进制数，并将其加入到c中
      c = (c << 4) + from_hex(*p);
    *new_pos = p;  // 更新new_pos的值为p的地址
    return c;  // 返回读取的十六进制数
  }

  *new_pos = p + 1;  // 更新new_pos的值为p的下一个字符的地址

  // 转义序列在这里使用自身进行定义。例如，'\n'使用'\n'来实现。
  // 这种自指定义的方式之所以有效，是因为编译我们编译器的编译器知道'\n'的实际含义。
  // 换句话说，我们从编译我们编译器的编译器中"继承"了'\n'的ASCII码，
  // 因此我们不需要在这里教会实际的代码。
  //
  // 这个事实不仅对编译器的正确性有巨大的影响，也对生成的代码的安全性有重要意义。
  // 想要了解更多信息，请阅读Ken Thompson的《Reflections on Trusting Trust》。
  // https://github.com/rui314/chibicc/wiki/thompson1984.pdf
  switch (*p) {
  case 'a': return '\a';  // 返回响铃字符
  case 'b': return '\b';  // 返回退格字符
  case 't': return '\t';  // 返回制表符
  case 'n': return '\n';  // 返回换行符
  case 'v': return '\v';  // 返回垂直制表符
  case 'f': return '\f';  // 返回换页符
  case 'r': return '\r';  // 返回回车符
  // [GNU] \e代表ASCII转义字符，是GNU C的扩展。
  case 'e': return 27;  // 返回ASCII转义字符
  default: return *p;  // 返回字符p本身
  }
}

//找到一个闭合的双引号
static char *string_literal_end(char *p) {
  char *start = p;  // 保存字符串字面量的起始位置
  for (; *p != '"'; p++) {  // 遍历字符串字面量直到找到结束的双引号
    if (*p == '\n' || *p == '\0')
      error_at(start, "unclosed string literal");  // 如果字符串字面量没有关闭，则报错
    if (*p == '\\')
      p++;  // 跳过转义字符
  }
  return p;  // 返回字符串字面量的结束位置
}

static Token *read_string_literal(char *start, char *quote) {
  char *end = string_literal_end(quote + 1);  // 获取字符串字面量的结束位置
  char *buf = calloc(1, end - quote);  // 为字符串字面量分配内存空间
  int len = 0;  // 字符串字面量的长度

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(&p, p + 1);  // 读取转义字符
    else
      buf[len++] = *p++;  // 将字符添加到缓冲区
  }

  Token *tok = new_token(TK_STR, start, end + 1);  // 创建一个新的字符串字面量Token
  tok->ty = array_of(ty_char, len + 1);  // 设置Token的类型为字符数组
  tok->str = buf;  // 将缓冲区的内容赋值给Token的字符串属性
  return tok;  // 返回字符串字面量的Token
}

// 读取一个UTF-8编码的字符串字面量，并将其转码为UTF-16。
//
// UTF-16是另一种用于Unicode的可变宽度编码。小于U+10000的代码点使用2个字节进行编码。
// 大于等于U+10000的代码点使用4个字节进行编码。4个字节序列中的每2个字节被称为"代理项"，
// 而4个字节序列被称为"代理对"。
static Token *read_utf16_string_literal(char *start, char *quote) {
  char *end = string_literal_end(quote + 1);  // 获取字符串字面量的结束位置
  uint16_t *buf = calloc(2, end - start);  // 为UTF-16字符串字面量分配内存空间
  int len = 0;  // UTF-16字符串字面量的长度

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\') {
      buf[len++] = read_escaped_char(&p, p + 1);  // 读取转义字符
      continue;
    }

    uint32_t c = decode_utf8(&p, p);  // 解码UTF-8字符为Unicode码点
    if (c < 0x10000) {
      // 使用2个字节编码一个Unicode码点
      buf[len++] = c;
    } else {
      // 使用4个字节编码一个Unicode码点
      c -= 0x10000;
      buf[len++] = 0xd800 + ((c >> 10) & 0x3ff);  // 高代理项
      buf[len++] = 0xdc00 + (c & 0x3ff);  // 低代理项
    }
  }

  Token *tok = new_token(TK_STR, start, end + 1);  // 创建一个新的UTF-16字符串字面量Token
  tok->ty = array_of(ty_ushort, len + 1);  // 设置Token的类型为unsigned short数组
  tok->str = (char *)buf;  // 将UTF-16字符串字面量的缓冲区内容赋值给Token的字符串属性
  return tok;  // 返回UTF-16字符串字面量的Token
}

// 读取一个UTF-8编码的字符串字面量，并将其转码为UTF-32。
//
// UTF-32是Unicode的固定宽度编码。每个代码点使用4个字节进行编码。
static Token *read_utf32_string_literal(char *start, char *quote, Type *ty) {
  char *end = string_literal_end(quote + 1);  // 获取字符串字面量的结束位置
  uint32_t *buf = calloc(4, end - quote);  // 为UTF-32字符串字面量分配内存空间
  int len = 0;  // UTF-32字符串字面量的长度

  for (char *p = quote + 1; p < end;) {
    if (*p == '\\')
      buf[len++] = read_escaped_char(&p, p + 1);  // 读取转义字符
    else
      buf[len++] = decode_utf8(&p, p);  // 解码UTF-8字符为Unicode码点
  }

  Token *tok = new_token(TK_STR, start, end + 1);  // 创建一个新的UTF-32字符串字面量Token
  tok->ty = array_of(ty, len + 1);  // 设置Token的类型为指定的类型数组
  tok->str = (char *)buf;  // 将UTF-32字符串字面量的缓冲区内容赋值给Token的字符串属性
  return tok;  // 返回UTF-32字符串字面量的Token
}

static Token *read_char_literal(char *start, char *quote, Type *ty) {
  char *p = quote + 1;  // 指向字符字面量的第一个字符
  if (*p == '\0')
    error_at(start, "unclosed char literal");  // 如果字符字面量没有关闭，则报错

  int c;
  if (*p == '\\')
    c = read_escaped_char(&p, p + 1);  // 读取转义字符
  else
    c = decode_utf8(&p, p);  // 解码UTF-8字符为Unicode码点

  char *end = strchr(p, '\'');  // 查找字符字面量的结束位置
  if (!end)
    error_at(p, "unclosed char literal");  // 如果字符字面量没有关闭，则报错

  Token *tok = new_token(TK_NUM, start, end + 1);  // 创建一个新的字符字面量Token
  tok->val = c;  // 设置Token的值为字符的Unicode码点
  tok->ty = ty;  // 设置Token的类型为指定的类型
  return tok;  // 返回字符字面量的Token
}

static bool convert_pp_int(Token *tok) {
  char *p = tok->loc;  // 定义一个指针变量p，指向Token结构体中的loc成员

  // Read a binary, octal, decimal or hexadecimal number.
  int base = 10;  // 初始化一个整型变量base为10
  if (!strncasecmp(p, "0x", 2) && isxdigit(p[2])) {  // 如果p指向的字符串以"0x"开头且第三个字符是十六进制数字
    p += 2;  // 将指针p向后移动两个位置
    base = 16;  // 将base设置为16，表示是十六进制数
  } else if (!strncasecmp(p, "0b", 2) && (p[2] == '0' || p[2] == '1')) {  // 如果p指向的字符串以"0b"开头且第三个字符是0或1
    p += 2;  // 将指针p向后移动两个位置
    base = 2;  // 将base设置为2，表示是二进制数
  } else if (*p == '0') {  // 如果p指向的字符是'0'
    base = 8;  // 将base设置为8，表示是八进制数
  }

  int64_t val = strtoul(p, &p, base);  // 将p指向的字符串转换为整数，并将结果存储在val变量中，同时将指针p指向转换后的字符串的下一个字符位置

  // Read U, L or LL suffixes.
  bool l = false;  // 初始化一个布尔变量l为false
  bool u = false;  // 初始化一个布尔变量u为false

  if (startswith(p, "LLU") || startswith(p, "LLu") ||  // 如果p指向的字符串以"LLU"、"LLu"、"llU"、"llu"、"ULL"、"Ull"、"uLL"、"ull"开头
      startswith(p, "llU") || startswith(p, "llu") ||
      startswith(p, "ULL") || startswith(p, "Ull") ||
      startswith(p, "uLL") || startswith(p, "ull")) {
    p += 3;  // 将指针p向后移动3个位置
    l = u = true;  // 将l和u都设置为true
  } else if (!strncasecmp(p, "lu", 2) || !strncasecmp(p, "ul", 2)) {  // 如果p指向的字符串以"lu"或"ul"开头（不区分大小写）
    p += 2;  // 将指针p向后移动2个位置
    l = u = true;  // 将l和u都设置为true
  } else if (startswith(p, "LL") || startswith(p, "ll")) {  // 如果p指向的字符串以"LL"或"ll"开头
    p += 2;  // 将指针p向后移动2个位置
    l = true;  // 将l设置为true
  } else if (*p == 'L' || *p == 'l') {  // 如果p指向的字符是'L'或'l'
    p++;  // 将指针p向后移动1个位置
    l = true;  // 将l设置为true
  } else if (*p == 'U' || *p == 'u') {  // 如果p指向的字符是'U'或'u'
    p++;  // 将指针p向后移动1个位置
    u = true;  // 将u设置为true
  }

  if (p != tok->loc + tok->len)  // 如果指针p不等于tok->loc + tok->len，即p指向的位置不是Token结构体中的loc成员加上len成员的位置
    return false;  // 返回false

  // Infer a type.
  Type *ty;  // 定义一个指向Type结构体的指针变量ty
  if (base == 10) {  // 如果base为10
    if (l && u)  // 如果l和u都为true
      ty = ty_ulong;  // 将ty指向ty_ulong
    else if (l)  // 如果l为true
      ty = ty_long;  // 将ty指向ty_long
    else if (u)  // 如果u为true
      ty = (val >> 32) ? ty_ulong : ty_uint;  // 如果val右移32位后不为0，将ty指向ty_ulong，否则将ty指向ty_uint
    else
      ty = (val >> 31) ? ty_long : ty_int;  // 如果val右移31位后不为0，将ty指向ty_long，否则将ty指向ty_int
  } else {
    if (l && u)  // 如果l和u都为true
      ty = ty_ulong;  // 将ty指向ty_ulong
    else if (l)  // 如果l为true
      ty = (val >> 63) ? ty_ulong : ty_long;  // 如果val右移63位后不为0，将ty指向ty_ulong，否则将ty指向ty_long
    else if (u)  // 如果u为true
      ty = (val >> 32) ? ty_ulong : ty_uint;  // 如果val右移32位后不为0，将ty指向ty_ulong，否则将ty指向ty_uint
    else if (val >> 63)
      ty = ty_ulong;  // 如果val右移63位后不为0，将ty指向ty_ulong
    else if (val >> 32)
      ty = ty_long;  // 如果val右移32位后不为0，将ty指向ty_long
    else if (val >> 31)
      ty = ty_uint;  // 如果val右移31位后不为0，将ty指向ty_uint
    else
      ty = ty_int;  // 否则将ty指向ty_int
  }

  tok->kind = TK_NUM;  // 将Token结构体中的kind成员设置为TK_NUM
  tok->val = val;  // 将Token结构体中的val成员设置为val
  tok->ty = ty;  // 将Token结构体中的ty成员设置为ty
  return true;  // 返回true
}

// 在预处理阶段，数字字面量的定义比后续阶段更加宽松。
// 为了处理这种情况，数字字面量首先被标记为"pp-number"标记，
// 然后在预处理后转换为常规的数字标记。
//
// 该函数将"pp-number"标记转换为常规的数字标记。
static void convert_pp_number(Token *tok) {
  if (convert_pp_int(tok)) // 尝试解析为整数常量。
    return;
  // 如果不是整数，则必须是浮点数常量。
  char *end;
  long double val = strtold(tok->loc, &end); // 将字符串转换为长双精度浮点数。

  Type *ty;
  if (*end == 'f' || *end == 'F') {
    ty = ty_float; // 如果以 'f' 或 'F' 结尾，则表示浮点数类型为 float。
    end++;
  } else if (*end == 'l' || *end == 'L') {
    ty = ty_ldouble; // 如果以 'l' 或 'L' 结尾，则表示浮点数类型为 long double。
    end++;
  } else {
    ty = ty_double; // 否则，浮点数类型为 double。
  }

  if (tok->loc + tok->len != end)
    error_tok(tok, "invalid numeric constant"); // 如果解析的部分与原始标记的长度不匹配，则表示无效的数字常量。

  tok->kind = TK_NUM; // 更新标记的类型为数字。
  tok->fval = val; // 更新标记的浮点数值。
  tok->ty = ty; // 更新标记的类型。
}

void convert_pp_tokens(Token *tok) {
  // 将预处理标记转换为普通标记

  // 遍历标记链表，直到遇到文件结束标记
  for (Token *t = tok; t->kind != TK_EOF; t = t->next) {
    if (is_keyword(t))
      t->kind = TK_KEYWORD; // 如果是关键字，则将标记类型设置为关键字
    else if (t->kind == TK_PP_NUM)
      convert_pp_number(t); // 如果是预处理数字标记，则进行转换
  }
}

// 为所有标记初始化行信息。
static void add_line_numbers(Token *tok) {
  char *p = current_file->contents; // 指向当前文件内容的指针
  int n = 1; // 行号初始化为1

  do {
    if (p == tok->loc) {
      tok->line_no = n; // 如果指针位置与标记的位置相同，则将标记的行号设置为当前行号
      tok = tok->next; // 移动到下一个标记
    }
    if (*p == '\n')
      n++; // 如果遇到换行符，则行号加1
  } while (*p++); // 遍历文件内容直到结束
}

Token *tokenize_string_literal(Token *tok, Type *basety) {
  // 标记化字符串字面量

  Token *t;
  if (basety->size == 2)
    t = read_utf16_string_literal(tok->loc, tok->loc); // 如果基础类型大小为2，则读取UTF-16字符串字面量
  else
    t = read_utf32_string_literal(tok->loc, tok->loc, basety); // 否则，读取UTF-32字符串字面量，并指定基础类型

  t->next = tok->next; // 将新生成的标记与原始标记链表连接起来
  return t; // 返回新生成的标记
}

// Tokenize a given string and returns new tokens.
// 对给定的字符串进行分词，返回新的标记。

Token *tokenize(File *file) {
  current_file = file;  // 设置当前文件为给定的文件

  char *p = file->contents;  // 指针p指向文件内容
  Token head = {};  // 创建一个空的标记头结点
  Token *cur = &head;  // cur指针指向标记头结点

  at_bol = true;  // 设置at_bol变量为true，表示当前在行首
  has_space = false;  // 设置has_space变量为false，表示当前没有空格

  while (*p) {  // 开始遍历文件内容
    // Skip line comments.
    // 跳过行注释
    if (startswith(p, "//")) {  // 如果当前行以"//"开头
      p += 2;  // 指针p向后移动两个位置，跳过注释符号"//"
      while (*p != '\n')  // 循环直到遇到换行符
        p++;  // 指针p向后移动一个位置
      has_space = true;  // 设置has_space变量为true，表示当前有空格
      continue;  // 继续下一次循环
    }

    // Skip block comments.
    // 跳过块注释
    if (startswith(p, "/*")) {  // 如果当前行以"/*"开头
      char *q = strstr(p + 2, "*/");  // 在p+2位置开始查找"*/"的位置
      if (!q)  // 如果没有找到匹配的"*/"
        error_at(p, "unclosed block comment");  // 报错，块注释未闭合
      p = q + 2;  // 指针p移动到块注释的结束位置后面
      has_space = true;  // 设置has_space变量为true，表示当前有空格
      continue;  // 继续下一次循环
    }

    // Skip newline.
    // 跳过换行符
    if (*p == '\n') {  // 如果当前字符是换行符
      p++;  // 指针p向后移动一个位置
      at_bol = true;  // 设置at_bol变量为true，表示当前在行首
      has_space = false;  // 设置has_space变量为false，表示当前没有空格
      continue;  // 继续下一次循环
    }

    // Skip whitespace characters.
    // 跳过空白字符
    if (isspace(*p)) {  // 如果当前字符是空白字符
      p++;  // 指针p向后移动一个位置
      has_space = true;  // 设置has_space变量为true，表示当前有空格
      continue;  // 继续下一次循环
    }

    // Numeric literal
    // 数字字面量
    if (isdigit(*p) || (*p == '.' && isdigit(p[1]))) {  // 如果当前字符是数字，或者是以小数点开头且下一个字符是数字
      char *q = p++;  // 将指针q指向当前位置，然后指针p向后移动一个位置
      for (;;) {  // 开始循环
        if (p[0] && p[1] && strchr("eEpP", p[0]) && strchr("+-", p[1]))  // 如果当前字符和下一个字符组成科学计数法的指数部分
          p += 2;  // 指针p向后移动两个位置
        else if (isalnum(*p) || *p == '.')  // 如果当前字符是字母数字或者是小数点
          p++;  // 指针p向后移动一个位置
        else
          break;  // 跳出循环
      }
      cur = cur->next = new_token(TK_PP_NUM, q, p);  // 创建一个新的数字标记，并将其连接到当前标记的下一个位置
      continue;  // 继续下一次循环
    }

    // String literal
    // 字符串字面量
    if (*p == '"') {  // 如果当前字符是双引号
      cur = cur->next = read_string_literal(p, p);  // 从当前位置开始读取字符串字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符串字面量的长度
      continue;  // 继续下一次循环
    }

    // UTF-8 string literal
    // UTF-8字符串字面量
    if (startswith(p, "u8\"")) {  // 如果当前行以"u8\""开头
      cur = cur->next = read_string_literal(p, p + 2);  // 从p+2位置开始读取UTF-8字符串字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符串字面量的长度
      continue;  // 继续下一次循环
    }

    // UTF-16 string literal
    // UTF-16字符串字面量
    if (startswith(p, "u\"")) {  // 如果当前行以"u\""开头
      cur = cur->next = read_utf16_string_literal(p, p + 1);  // 从p+1位置开始读取UTF-16字符串字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符串字面量的长度
      continue;  // 继续下一次循环
    }

    // Wide string literal
    // 宽字符串字面量
    if (startswith(p, "L\"")) {  // 如果当前行以"L\""开头
      cur = cur->next = read_utf32_string_literal(p, p + 1, ty_int);  // 从p+1位置开始读取宽字符串字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符串字面量的长度
      continue;  // 继续下一次循环
    }

    // UTF-32 string literal
    // UTF-32字符串字面量
    if (startswith(p, "U\"")) {  // 如果当前行以"U\""开头
      cur = cur->next = read_utf32_string_literal(p, p + 1, ty_uint);  // 从p+1位置开始读取UTF-32字符串字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符串字面量的长度
      continue;  // 继续下一次循环
    }

    // Character literal
    // 字符字面量
    if (*p == '\'') {  // 如果当前字符是单引号
      cur = cur->next = read_char_literal(p, p, ty_int);  // 从当前位置开始读取字符字面量，并将其连接到当前标记的下一个位置
      cur->val = (char)cur->val;  // 将字符字面量的值转换为char类型
      p += cur->len;  // 指针p向后移动字符字面量的长度
      continue;  // 继续下一次循环
    }

    // UTF-16 character literal
    // UTF-16字符字面量
    if (startswith(p, "u'")) {  // 如果当前行以"u'"开头
      cur = cur->next = read_char_literal(p, p + 1, ty_ushort);  // 从p+1位置开始读取UTF-16字符字面量，并将其连接到当前标记的下一个位置
      cur->val &= 0xffff;  // 将字符字面量的值截断为16位
      p += cur->len;  // 指针p向后移动字符字面量的长度
      continue;  // 继续下一次循环
    }

    // Wide character literal
    // 宽字符字面量
    if (startswith(p, "L'")) {  // 如果当前行以"L'"开头
      cur = cur->next = read_char_literal(p, p + 1, ty_int);  // 从p+1位置开始读取宽字符字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符字面量的长度
      continue;  // 继续下一次循环
    }

    // UTF-32 character literal
    // UTF-32字符字面量
    if (startswith(p, "U'")) {  // 如果当前行以"U'"开头
      cur = cur->next = read_char_literal(p, p + 1, ty_uint);  // 从p+1位置开始读取UTF-32字符字面量，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动字符字面量的长度
      continue;  // 继续下一次循环
    }

    // Identifier or keyword
    // 标识符或关键字
    int ident_len = read_ident(p);  // 从当前位置开始读取标识符，并返回标识符的长度
    if (ident_len) {  // 如果标识符的长度大于0
      cur = cur->next = new_token(TK_IDENT, p, p + ident_len);  // 创建一个新的标识符标记，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动标识符的长度
      continue;  // 继续下一次循环
    }

    // Punctuators
    // 标点符号
    int punct_len = read_punct(p);  // 从当前位置开始读取标点符号，并返回标点符号的长度
    if (punct_len) {  // 如果标点符号的长度大于0
      cur = cur->next = new_token(TK_PUNCT, p, p + punct_len);  // 创建一个新的标点符号标记，并将其连接到当前标记的下一个位置
      p += cur->len;  // 指针p向后移动标点符号的长度
      continue;  // 继续下一次循环
    }

    error_at(p, "invalid token");  // 报告无效的标记错误

  }

  cur = cur->next = new_token(TK_EOF, p, p);  // 创建一个新的文件结束标记，并将其连接到当前标记的下一个位置
  add_line_numbers(head.next);  // 添加行号信息
  return head.next;  // 返回标记链表的头部
}

// 返回给定文件的内容。
static char *read_file(char *path) {
  FILE *fp;

  if (strcmp(path, "-") == 0) {  // 如果给定的文件名为"-"，按照约定从stdin中读取
    fp = stdin;
  } else {
    fp = fopen(path, "r");  // 打开文件
    if (!fp)
      return NULL;  // 如果文件打开失败，则返回空指针
  }

  char *buf;
  size_t buflen;
  FILE *out = open_memstream(&buf, &buflen);  // 打开用于写入内存流的输出流

  // 读取整个文件。
  for (;;) {
    char buf2[4096];
    int n = fread(buf2, 1, sizeof(buf2), fp);  // 从文件中读取数据到buf2中
    if (n == 0)
      break;  // 如果读取的字节数为0，则跳出循环
    fwrite(buf2, 1, n, out);  // 将buf2中的数据写入内存流
  }

  if (fp != stdin)
    fclose(fp);  // 如果文件指针不是stdin，则关闭文件指针

  // 确保最后一行以'\n'正确终止。
  fflush(out);  // 刷新输出缓冲区
  if (buflen == 0 || buf[buflen - 1] != '\n')
      fputc('\n', out);  // 如果缓冲区长度为0或者最后一个字符不是'\n'，则在输出流中写入'\n'
  fputc('\0', out);  // 在输出流中写入'\0'字符
  fclose(out);  // 关闭输出流
  return buf;  // 返回缓冲区
}

File **get_input_files(void) {
    return input_files;  // 返回input_files指针的指针
}

File *new_file(char *name, int file_no, char *contents) {
    File *file = calloc(1, sizeof(File));  // 为file分配内存
    file->name = name;  // 设置file的name属性为传入的name参数
    file->display_name = name;  // 设置file的display_name属性为传入的name参数
    file->file_no = file_no;  // 设置file的file_no属性为传入的file_no参数
    file->contents = contents;  // 设置file的contents属性为传入的contents参数
    return file;  // 返回file指针
}

// 将\r或\r\n替换为\n。
static void canonicalize_newline(char *p) {
  int i = 0, j = 0;  // 初始化变量i和j为0，用于循环中的索引计数

  while (p[i]) {  // 当p[i]不为'\0'时循环执行
    if (p[i] == '\r' && p[i + 1] == '\n') {  // 如果p[i]为'\r'且p[i+1]为'\n'
      i += 2;  // 将i增加2
      p[j++] = '\n';  // 在p[j]位置存储'\n'，并将j增加1
    } else if (p[i] == '\r') {  // 如果p[i]为'\r'
      i++;  // 将i增加1
      p[j++] = '\n';  // 在p[j]位置存储'\n'，并将j增加1
    } else {  // 如果以上条件都不满足
      p[j++] = p[i++];  // 将p[i]复制到p[j]，并将i和j都增加1
    }
  }

  p[j] = '\0';  // 在p[j]位置存储'\0'，表示字符串结束
}

// 移除后面跟随换行符的反斜杠。
static void remove_backslash_newline(char *p) {
  int i = 0, j = 0;  // 初始化变量i和j为0，用于循环中的索引计数

  // 我们希望保持换行符的数量，以便逻辑行号与物理行号匹配。
  // 这个计数器维护我们已经移除的换行符的数量。
  int n = 0;  // 初始化变量n为0，用于记录移除的换行符数量

  while (p[i]) {  // 当p[i]不为'\0'时循环执行
    if (p[i] == '\\' && p[i + 1] == '\n') {  // 如果p[i]为'\'且p[i+1]为'\n'
      i += 2;  // 将i增加2
      n++;  // 将n增加1，表示移除了一个换行符
    } else if (p[i] == '\n') {  // 如果p[i]为'\n'
      p[j++] = p[i++];  // 将p[i]复制到p[j]，并将i和j都增加1
      for (; n > 0; n--)  // 循环n次
        p[j++] = '\n';  // 在p[j]位置存储'\n'，表示还原被移除的换行符
    } else {  // 如果以上条件都不满足
      p[j++] = p[i++];  // 将p[i]复制到p[j]，并将i和j都增加1
    }
  }

  for (; n > 0; n--)  // 循环n次
    p[j++] = '\n';  // 在p[j]位置存储'\n'，表示还原被移除的换行符
  p[j] = '\0';  // 在p[j]位置存储'\0'，表示字符串结束
}

static uint32_t read_universal_char(char *p, int len) {
  uint32_t c = 0;  // 初始化变量c为0，用于存储读取的Unicode字符
  for (int i = 0; i < len; i++) {  // 循环i从0到len-1
    if (!isxdigit(p[i]))  // 如果p[i]不是十六进制数字
      return 0;  // 返回0，表示读取失败
    c = (c << 4) | from_hex(p[i]);  // 将p[i]转换为十进制数，并将其与c左移4位后的值进行按位或运算，更新c的值
  }
  return c;  // 返回读取的Unicode字符
}

// 用对应的UTF-8字节替换\u或\U转义序列。
static void convert_universal_chars(char *p) {
  char *q = p;  // 创建指针q，初始化为p的值，用于存储转换后的字符串

  while (*p) {  // 当*p不为'\0'时循环执行
    if (startswith(p, "\\u")) {  // 如果以"\u"开头
      uint32_t c = read_universal_char(p + 2, 4);  // 从p + 2开始读取4个字符并转换为Unicode字符
      if (c) {  // 如果读取成功（c不为0）
        p += 6;  // 将p向后移动6个位置
        q += encode_utf8(q, c);  // 将Unicode字符c转换为UTF-8编码并存储在q指向的位置，同时更新q的值
      } else {
        *q++ = *p++;  // 将p指向的字符复制到q指向的位置，并将p和q都增加1
      }
    } else if (startswith(p, "\\U")) {  // 如果以"\U"开头
      uint32_t c = read_universal_char(p + 2, 8);  // 从p + 2开始读取8个字符并转换为Unicode字符
      if (c) {  // 如果读取成功（c不为0）
        p += 10;  // 将p向后移动10个位置
        q += encode_utf8(q, c);  // 将Unicode字符c转换为UTF-8编码并存储在q指向的位置，同时更新q的值
      } else {
        *q++ = *p++;  // 将p指向的字符复制到q指向的位置，并将p和q都增加1
      }
    } else if (p[0] == '\\') {  // 如果p的第一个字符为'\'
      *q++ = *p++;  // 将p指向的字符复制到q指向的位置，并将p和q都增加1
      *q++ = *p++;  // 将p指向的字符复制到q指向的位置，并将p和q都增加1
    } else {  // 如果以上条件都不满足
      *q++ = *p++;  // 将p指向的字符复制到q指向的位置，并将p和q都增加1
    }
  }

  *q = '\0';  // 在q指向的位置存储'\0'，表示字符串结束
}

Token *tokenize_file(char *path) {
  char *p = read_file(path);  // 从文件中读取内容并存储在指针p中
  if (!p)  // 如果读取文件失败（p为NULL）
    return NULL;  // 返回NULL

  // UTF-8文本可能以3字节的“BOM”标记序列开头。
  // 如果存在，跳过它们，因为它们是无用的字节。
  // （实际上，不建议在UTF-8文本中添加BOM标记，但在Windows上这是常见的。）
  if (!memcmp(p, "\xef\xbb\xbf", 3))  // 如果p以"\xef\xbb\xbf"开头
    p += 3;  // 将p向后移动3个位置，跳过BOM标记

  canonicalize_newline(p);  // 规范化换行符
  remove_backslash_newline(p);  // 移除反斜杠后的换行符
  convert_universal_chars(p);  // 转换Unicode字符为UTF-8编码

  // 为汇编器的.file指令保存文件名。
  static int file_no;  // 静态变量file_no，用于记录文件编号
  File *file = new_file(path, file_no + 1, p);  // 创建File结构体实例，存储文件名、文件编号和文件内容

  // 为汇编器的.file指令保存文件名。
  input_files = realloc(input_files, sizeof(char *) * (file_no + 2));  // 重新分配内存空间，用于存储输入文件名
  input_files[file_no] = file;  // 将File结构体指针存储在input_files数组中
  input_files[file_no + 1] = NULL;  // 在input_files数组的最后一个元素后面存储NULL，表示结束
  file_no++;  // 增加文件编号

  return tokenize(file);  // 对文件进行词法分析，返回Token链表的头指针
}