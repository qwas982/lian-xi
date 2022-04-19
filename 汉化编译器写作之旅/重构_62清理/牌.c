/*------------------------------------------------
---------包括 扫描.印名 冗杂.印名 符号.印名 声明.印名-----------
------------------------------------------------*/

#include "定义.h"

/*------------------------------------------------
---------扫描.印名 -----------
------------------------------------------------*/


// 词汇 scanning
//   (印名)   

// Return the position of 印刻 印名
// in 串 串名, or -1 if 印名 not found
static int 印刻位置(char *串名, int 印名) {
  int 甲;
  for (甲 = 0; 串名[甲] != '\0'; 甲++)
    if (串名[甲] == (char) 印名)
      return (甲);
  return (-1);
}

// Get the 下一个 印刻 from the input file.
static int 下一个(void) {
  int 印名, 小标号;

  if (放回去) {		// Use the 印刻 put
    印名 = 放回去;		// back if there is one
    放回去 = 0;
    return (印名);
  }

  印名 = fgetc(入文件);			// Read from input file

  while (行开始 && 印名 == '#') {	// We've hit a pre-processor 语句
    行开始 = 0;			// No longer at the start of the 行_
    扫描(&牌_);			// Get the 行_ number into 小标号
    if (牌_.牌 != 牌名_整型字面)
      致命符("Expecting pre-processor 行_ number, got:", 文本);
    小标号 = 牌_.整型值;

    扫描(&牌_);			// Get the 文件名 in 文本
    if (牌_.牌 != 牌名_串字面)
      致命符("Expecting pre-processor file 名字, got:", 文本);

    if (文本[0] != '<') {		// If this is a real 文件名
      if (strcmp(文本, 入文件名))	// and not the one we have now
	入文件名 = strdup(文本);	// save it. Then update the 行_ num
      行 = 小标号;
    }

    while ((印名 = fgetc(入文件)) != '\n'); // Skip to the end of the 行_
    印名 = fgetc(入文件);			// and get the 下一个 印刻
    行开始 = 1;			// Now back at the start of the 行_
  }

  行开始 = 0;			// No longer at the start of the 行_
  if ('\n' == 印名) {
    行++;				// Increment 行_ count
    行开始 = 1;			// Now back at the start of the 行_
  }
  return (印名);
}

// Put back an unwanted 印刻
static void 放回去_(int 印名) {
  放回去 = 印名;
}

// Skip past input that we don't need to deal with, 
// 甲.e. whitespace, newlines. Return the first
// 印刻 we do need to deal with.
static int 跳过(void) {
  int 印名;

  印名 = 下一个();
  while (' ' == 印名 || '\t' == 印名 || '\n' == 印名 || '\r' == 印名 || '\f' == 印名) {
    印名 = 下一个();
  }
  return (印名);
}

// Read in a hexadecimal constant from the input
static int 十六进制印刻(void) {
  int 印名, 小十六进制, 小节点 = 0, 小旗 = 0;

  // Loop getting characters
  while (isxdigit(印名 = 下一个())) {
    // Convert from char to int 值
    小十六进制 = 印刻位置("0123456789abcdef", tolower(印名));

    // Add to running hex 值
    小节点 = 小节点 * 16 + 小十六进制;
    小旗 = 1;
  }

  // We hit a non-hex 印刻, put it back
  放回去_(印名);

  // Flag tells us we never saw any hex characters
  if (!小旗)
    致命("missing digits after '\\x'");
  if (小节点 > 255)
    致命("值 out of range after '\\x'");

  return (小节点);
}

// Return the 下一个 印刻 from a 印刻
// or 串 literal
static int 扫描印刻(void) {
  int 甲, 印名, c2;

  // Get the 下一个 input 印刻 and interpret
  // metacharacters that start with a backslash
  印名 = 下一个();
  if (印名 == '\\') {
    switch (印名 = 下一个()) {
      case 'a':
	return ('\a');
      case 'b':
	return ('\b');
      case 'f':
	return ('\f');
      case 'n':
	return ('\n');
      case 'r':
	return ('\r');
      case 't':
	return ('\t');
      case 'v':
	return ('\v');
      case '\\':
	return ('\\');
      case '"':
	return ('"');
      case '\'':
	return ('\'');

	// Deal with octal constants by reading in
	// characters until we hit a non-octal digit.
	// Build up the octal 值 in c2 and count
	// # digits in 甲. Permit only 3 octal digits.
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
	for (甲 = c2 = 0; isdigit(印名) && 印名 < '8'; 印名 = 下一个()) {
	  if (++甲 > 3)
	    break;
	  c2 = c2 * 8 + (印名 - '0');
	}

	放回去_(印名);		// Put back the first non-octal char
	return (c2);
      case 'x':
	return (十六进制印刻());
      default:
	致命印名("unknown 转义序列", 印名);
    }
  }
  return (印名);			// Just an ordinary old 印刻!
}

// Scan and return an integer literal
// 值 from the input file.
static int 扫描整型(int 印名) {
  int 小键, 小值 = 0, 基数 = 10;

  // Assume the 基数 is 10, but if it starts with 0
  if (印名 == '0') {
    // and the 下一个 印刻 is 'x', it'串名 基数 16
    if ((印名 = 下一个()) == 'x') {
      基数 = 16;
      印名 = 下一个();
    } else
      // Otherwise, it'串名 基数 8
      基数 = 8;

  }

  // Convert each 印刻 into an int 值
  while ((小键 = 印刻位置("0123456789abcdef", tolower(印名))) >= 0) {
    if (小键 >= 基数)
      致命印名("invalid digit in integer literal", 印名);
    小值 = 小值 * 基数 + 小键;
    印名 = 下一个();
  }

  // We hit a non-integer 印刻, put it back.
  放回去_(印名);
  return (小值);
}

// Scan in a 串 literal from the input file,
// and store it in 小缓冲[]. Return the length of
// the 串. 
static int 扫描串(char *小缓冲) {
  int 甲, 印名;

  // Loop while we have enough buffer space
  for (甲 = 0; 甲 < 文本长 - 1; 甲++) {
    // Get the 下一个 char and 追加 to 小缓冲
    // Return when we hit the ending double quote
    if ((印名 = 扫描印刻()) == '"') {
      小缓冲[甲] = 0;
      return (甲);
    }
    小缓冲[甲] = (char)印名;
  }

  // Ran out of 小缓冲[] space
  致命("String literal too long");
  return (0);
}

// Scan an 标识符 from the input file and
// store it in 小缓冲[]. Return the 标识符'串名 length
static int 扫描标识(int 印名, char *小缓冲, int 小极限) {
  int 甲 = 0;

  // Allow digits, alpha and underscores
  while (isalpha(印名) || isdigit(印名) || '_' == 印名) {
    // Error if we hit the 标识符 length limit,
    // else 追加 to 小缓冲[] and get 下一个 印刻
    if (小极限 - 1 == 甲) {
      致命("Identifier too long");
    } else if (甲 < 小极限 - 1) {
      小缓冲[甲++] = (char)印名;
    }
    印名 = 下一个();
  }

  // We hit a non-valid 印刻, put it back.
  // NUL-terminate the 小缓冲[] and return the length
  放回去_(印名);
  小缓冲[甲] = '\0';
  return (甲);
}

// Given a word from the input, return the matching
// 关键字 牌 number or 0 if it'串名 not a 关键字.
// Switch on the first letter so that we don't have
// to waste time strcmp()ing against all the keywords.
static int 关键字(char *串名) {
  switch (*串名) {
    case 'b':
      if (!strcmp(串名, "断"))
	return (牌名_断);
      break;
    case 'c':
      if (!strcmp(串名, "情况"))
	return (牌名_情况);
      if (!strcmp(串名, "印刻"))
	return (牌名_印刻);
      if (!strcmp(串名, "继续"))
	return (牌名_继续);
      break;
    case 'd':
      if (!strcmp(串名, "默认"))
	return (牌名_默认);
      break;
    case 'e':
      if (!strcmp(串名, "否则"))
	return (牌名_否则);
      if (!strcmp(串名, "枚举"))
	return (牌名_枚举);
      if (!strcmp(串名, "外部"))
	return (牌名_外部);
      break;
    case 'f':
      if (!strcmp(串名, "为"))
	return (牌名_为);
      break;
    case 'i':
      if (!strcmp(串名, "若"))
	return (牌名_若);
      if (!strcmp(串名, "整型"))
	return (牌名_整型);
      break;
    case 'l':
      if (!strcmp(串名, "长型"))
	return (牌名_长型);
      break;
    case 'r':
      if (!strcmp(串名, "返回"))
	return (牌名_返回);
      break;
    case 's':
      if (!strcmp(串名, "求大小"))
	return (牌名_求大小);
      if (!strcmp(串名, "静态"))
	return (牌名_静态);
      if (!strcmp(串名, "结构"))
	return (牌名_结构);
      if (!strcmp(串名, "切换"))
	return (牌名_切换);
      break;
    case 't':
      if (!strcmp(串名, "类型定义"))
	return (牌名_类型定义);
      break;
    case 'u':
      if (!strcmp(串名, "联合"))
	return (牌名_联合);
      break;
    case 'v':
      if (!strcmp(串名, "空"))
	return (牌名_空的);
      break;
    case 'w':
      if (!strcmp(串名, "一会儿"))
	return (牌名_一会儿);
      break;
  }
  return (0);
}

// List of 牌 strings, for debugging purposes
char *牌名串[] = {
  "文件终", "=", "+=", "-=", "*=", "/=", "%=",
  "?", "||", "&&", "|", "^", "&",
  "==", "!=", "<", ">", "<=", ">=", "<<", ">>",
  "+", "-", "*", "/", "%", "++", "--", "~", "!",
  "空", "印刻", "整型", "长型",
  "若", "否则", "一会儿", "为", "返回",
  "结构", "联合", "枚举", "类型定义",
  "外部", "断", "继续", "切换",
  "情况", "默认", "求大小", "静态",
  "整型字面", "串列表", ";", "标识符",
  "{", "}", "(", ")", "[", "]", ",", ".",
  "->", ":"
};

// Scan and return the 下一个 牌 found in the input.
// Return 1 if 牌 valid, 0 if no tokens 左.
int 扫描(struct 牌 *牌名) {
  int 印名, 牌类型;

  // If we have a lookahead 牌, return this 牌
  if (窥牌.牌 != 0) {
    牌名->牌 = 窥牌.牌;
    牌名->牌串 = 窥牌.牌串;
    牌名->整型值 = 窥牌.整型值;
    窥牌.牌 = 0;
    return (1);
  }

  // Skip whitespace
  印名 = 跳过();

  // Determine the 牌 based on
  // the input 印刻
  switch (印名) {
    case EOF:
      牌名->牌 = 牌名_文件终;
      return (0);
    case '+':
      if ((印名 = 下一个()) == '+') {
	牌名->牌 = 牌名_自增;
      } else if (印名 == '=') {
	牌名->牌 = 牌名_赋值加;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_加;
      }
      break;
    case '-':
      if ((印名 = 下一个()) == '-') {
	牌名->牌 = 牌名_自减;
      } else if (印名 == '>') {
	牌名->牌 = 牌名_箭头;
      } else if (印名 == '=') {
	牌名->牌 = 牌名_赋值减;
      } else if (isdigit(印名)) {	// Negative int literal
	牌名->整型值 = -扫描整型(印名);
	牌名->牌 = 牌名_整型字面;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_减;
      }
      break;
    case '*':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_赋值星号;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_星号;
      }
      break;
    case '/':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_赋值斜杠;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_斜杠;
      }
      break;
    case '%':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_赋值余数;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_余数;
      }
      break;
    case ';':
      牌名->牌 = 牌名_分号;
      break;
    case '{':
      牌名->牌 = 牌名_左大括号;
      break;
    case '}':
      牌名->牌 = 牌名_右大括号;
      break;
    case '(':
      牌名->牌 = 牌名_左小括号;
      break;
    case ')':
      牌名->牌 = 牌名_右小括号;
      break;
    case '[':
      牌名->牌 = 牌名_左中括号;
      break;
    case ']':
      牌名->牌 = 牌名_右中括号;
      break;
    case '~':
      牌名->牌 = 牌名_颠倒;
      break;
    case '^':
      牌名->牌 = 牌名_异或;
      break;
    case ',':
      牌名->牌 = 牌名_逗号;
      break;
    case '.':
      牌名->牌 = 牌名_句号;
      break;
    case ':':
      牌名->牌 = 牌名_冒号;
      break;
    case '?':
      牌名->牌 = 牌名_问号;
      break;
    case '=':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_等于;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_赋值;
      }
      break;
    case '!':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_不等;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_逻辑非;
      }
      break;
    case '<':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_小等;
      } else if (印名 == '<') {
	牌名->牌 = 牌名_左移;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_小于;
      }
      break;
    case '>':
      if ((印名 = 下一个()) == '=') {
	牌名->牌 = 牌名_大等;
      } else if (印名 == '>') {
	牌名->牌 = 牌名_右移;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_大于;
      }
      break;
    case '&':
      if ((印名 = 下一个()) == '&') {
	牌名->牌 = 牌名_逻辑与;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_与;
      }
      break;
    case '|':
      if ((印名 = 下一个()) == '|') {
	牌名->牌 = 牌名_逻辑或;
      } else {
	放回去_(印名);
	牌名->牌 = 牌名_或;
      }
      break;
    case '\'':
      // If it'串名 a quote, 扫描 in the
      // literal 印刻 值 and
      // the trailing quote
      牌名->整型值 = 扫描印刻();
      牌名->牌 = 牌名_整型字面;
      if (下一个() != '\'')
	致命("Expected '\\'' at end of char literal");
      break;
    case '"':
      // Scan in a literal 串
      扫描串(文本);
      牌名->牌 = 牌名_串字面;
      break;
    default:
      // If it'串名 a digit, 扫描 the
      // literal integer 值 in
      if (isdigit(印名)) {
	牌名->整型值 = 扫描整型(印名);
	牌名->牌 = 牌名_整型字面;
	break;
      } else if (isalpha(印名) || '_' == 印名) {
	// Read in a 关键字 or 标识符
	扫描标识(印名, 文本, 文本长);

	// If it'串名 a recognised 关键字, return that 牌
	if ((牌类型 = 关键字(文本)) != 0) {
	  牌名->牌 = 牌类型;
	  break;
	}

	// Not a recognised 关键字, so it must be an 标识符
	牌名->牌 = 牌名_标识;
	break;
      }

      // The 印刻 isn't part of any recognised 牌, error
      致命印名("Unrecognised 印刻", 印名);
  }

  // We found a 牌
  牌名->牌串 = 牌名串[牌名->牌];
  return (1);
}

/*------------------------------------------------
--------- 冗杂.印名 -----------
------------------------------------------------*/

// Miscellaneous functions
//   (印名)   

// Ensure that the current 牌 is 牌名,
// and fetch the 下一个 牌. Otherwise
// throw an error 
void 匹配(int 牌名, char *what) {
  if (牌_.牌 == 牌名) {
    扫描(&牌_);
  } else {
    致命符("Expected", what);
  }
}

// Match a semicolon and fetch the 下一个 牌
void 分号(void) {
  匹配(牌名_分号, ";");
}

// Match a 左 brace and fetch the 下一个 牌
void 左大括号(void) {
  匹配(牌名_左大括号, "{");
}

// Match a 右 brace and fetch the 下一个 牌
void 右大括号(void) {
  匹配(牌名_右大括号, "}");
}

// Match a 左 parenthesis and fetch the 下一个 牌
void 左小括号(void) {
  匹配(牌名_左小括号, "(");
}

// Match a 右 parenthesis and fetch the 下一个 牌
void 右小括号(void) {
  匹配(牌名_右小括号, ")");
}

// Match an 标识符 and fetch the 下一个 牌
void 标识(void) {
  匹配(牌名_标识, "标识符");
}

// Match a 逗号 and fetch the 下一个 牌
void 逗号(void) {
  匹配(牌名_逗号, "逗号");
}

// Print out 致命 messages
void 致命(char *串名) {
  fprintf(stderr, "%s on 行_ %d of %s\n", 串名, 行, 入文件名);
  fclose(出文件);
  不链接(出文件名);
  exit(1);
}

void 致命符(char *符1, char *符2) {
  fprintf(stderr, "%s:%s on 行_ %d of %s\n", 符1, 符2, 行, 入文件名);
  fclose(出文件);
  不链接(出文件名);
  exit(1);
}

void 致命数(char *串名, int 数) {
  fprintf(stderr, "%s:%d on 行_ %d of %s\n", 串名, 数, 行, 入文件名);
  fclose(出文件);
  不链接(出文件名);
  exit(1);
}

void 致命印名(char *串名, int 印名) {
  fprintf(stderr, "%s:%c on 行_ %d of %s\n", 串名, 印名, 行, 入文件名);
  fclose(出文件);
  不链接(出文件名);
  exit(1);
}

/*------------------------------------------------
--------- 符号.印名 -----------
------------------------------------------------*/

// Symbol table functions
//   (印名)   

// Append a 尾部 to the singly-linked 列表 pointed to by 头部 or 尾部
void 追加符号(struct 符号表 **头部, struct 符号表 **尾部,
	       struct 符号表 *尾部) {

  // Check for valid pointers
  if (头部 == NULL || 尾部 == NULL || 尾部 == NULL)
    致命("Either 头部, 尾部 or 尾部 is NULL in 追加符号");

  // Append to the 列表
  if (*尾部) {
    (*尾部)->下一个 = 尾部;
    *尾部 = 尾部;
  } else
    *头部 = *尾部 = 尾部;
  尾部->下一个 = NULL;
}

// Create a symbol 尾部 to be added to a symbol table 列表.
// Set up the 尾部'串名:
// + 类型: char, int etc.
// + 复合类型: composite 类型 pointer for struct/union
// + structural 类型: var, function, array etc.
// + 大小: number of elements, or 标号终: end 标号 for a function
// + 本地符号位置信息: Position information for local symbols
// Return a pointer to the new 节点
struct 符号表 *新符号(char *名字, int 类型, struct 符号表 *复合类型,
			int 结构化类型, int 类别, int 元素个数, int 本地符号位置信息) {

  // Get a new 节点
  struct 符号表 *节点 = (struct 符号表 *) malloc(sizeof(struct 符号表));
  if (节点 == NULL)
    致命("Unable to malloc a symbol table 节点 in 新符号");

  // Fill in the values
  if (名字 == NULL)
    节点->名字 = NULL;
  else
    节点->名字 = strdup(名字);
  节点->类型 = 类型;
  节点->复合类型 = 复合类型;
  节点->结构化类型 = 结构化类型;
  节点->类别 = 类别;
  节点->元素个数 = 元素个数;

  // For pointers and integer types, set the 大小
  // of the symbol. structs and union declarations
  // manually set this up themselves.
  if (指针类型(类型) || 整型类型(类型))
    节点->大小 = 元素个数 * 类型大小(类型, 复合类型);

  节点->符号表_本地符号位置信息 = 本地符号位置信息;
  节点->下一个 = NULL;
  节点->成员 = NULL;
  节点->初始列表 = NULL;
  return (节点);
}

// Add a symbol to the global symbol 列表
struct 符号表 *添加全局(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 类别, int 元素个数, int 本地符号位置信息) {
  struct 符号表 *符号 =
    新符号(名字, 类型, 复合类型, 结构化类型, 类别, 元素个数, 本地符号位置信息);
  // For structs and unions, copy the 大小 from the 类型 节点
  if (类型 == 原语_结构 || 类型 == 原语_联合)
    符号->大小 = 复合类型->大小;
  追加符号(&全局头, &全局尾, 符号);
  return (符号);
}

// Add a symbol to the local symbol 列表
struct 符号表 *添加本地(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 元素个数) {
  struct 符号表 *符号 = 新符号(名字, 类型, 复合类型, 结构化类型, 类别_本地, 元素个数, 0);

  // For structs and unions, copy the 大小 from the 类型 节点
  if (类型 == 原语_结构 || 类型 == 原语_联合)
    符号->大小 = 复合类型->大小;
  追加符号(&本地头, &本地尾, 符号);
  return (符号);
}

// Add a symbol to the parameter 列表
struct 符号表 *添加形参(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型) {
  struct 符号表 *符号 = 新符号(名字, 类型, 复合类型, 结构化类型, 类别_形参, 1, 0);
  追加符号(&形参头, &形参尾, 符号);
  return (符号);
}

// Add a symbol to the temporary 成员 列表
struct 符号表 *添加成员(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 元素个数) {
  struct 符号表 *符号 =
    新符号(名字, 类型, 复合类型, 结构化类型, 类别_成员, 元素个数, 0);

  // For structs and unions, copy the 大小 from the 类型 节点
  if (类型 == 原语_结构 || 类型 == 原语_联合)
    符号->大小 = 复合类型->大小;
  追加符号(&成员头, &成员尾, 符号);
  return (符号);
}

// Add a struct to the struct 列表
struct 符号表 *添加结构(char *名字) {
  struct 符号表 *符号 = 新符号(名字, 原语_结构, NULL, 0, 类别_结构, 0, 0);
  追加符号(&结构头, &结构尾, 符号);
  return (符号);
}

// Add a struct to the union 列表
struct 符号表 *添加联合(char *名字) {
  struct 符号表 *符号 = 新符号(名字, 原语_联合, NULL, 0, 类别_联合, 0, 0);
  追加符号(&联合头, &联合尾, 符号);
  return (符号);
}

// Add an enum 类型 or 值 to the enum 列表.
// Class is 类别_枚举类型 or 类别_枚举值.
// Use 本地符号位置信息 to store the int 值.
struct 符号表 *添加枚举(char *名字, int 类别, int 值) {
  struct 符号表 *符号 = 新符号(名字, 原语_整型, NULL, 0, 类别, 0, 值);
  追加符号(&枚举头, &枚举尾, 符号);
  return (符号);
}

// Add a typedef to the typedef 列表
struct 符号表 *添加类型定义(char *名字, int 类型, struct 符号表 *复合类型) {
  struct 符号表 *符号 = 新符号(名字, 类型, 复合类型, 0, 类别_类型定义, 0, 0);
  追加符号(&类型定义头, &类型定义尾, 符号);
  return (符号);
}

// Search for a symbol in a specific 列表.
// Return a pointer to the found 节点 or NULL if not found.
// If 类别 is not zero, also 匹配 on the given 类别
static struct 符号表 *列表内找符号(char *串名, struct 符号表 *列表,
				      int 类别) {
  for (; 列表 != NULL; 列表 = 列表->下一个)
    if ((列表->名字 != NULL) && !strcmp(串名, 列表->名字))
      if (类别 == 0 || 类别 == 列表->类别)
	return (列表);
  return (NULL);
}

// Determine if the symbol 串名 is in the global symbol table.
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找全局(char *串名) {
  return (列表内找符号(串名, 全局头, 0));
}

// Determine if the symbol 串名 is in the local symbol table.
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找本地(char *串名) {
  struct 符号表 *节点;

  // Look for a parameter if we are in a function'串名 身体
  if (函数身份) {
    节点 = 列表内找符号(串名, 函数身份->成员, 0);
    if (节点)
      return (节点);
  }
  return (列表内找符号(串名, 本地头, 0));
}

// Determine if the symbol 串名 is in the symbol table.
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找符号(char *串名) {
  struct 符号表 *节点;

  // Look for a parameter if we are in a function'串名 身体
  if (函数身份) {
    节点 = 列表内找符号(串名, 函数身份->成员, 0);
    if (节点)
      return (节点);
  }
  // Otherwise, try the local and global symbol lists
  节点 = 列表内找符号(串名, 本地头, 0);
  if (节点)
    return (节点);
  return (列表内找符号(串名, 全局头, 0));
}

// Find a 成员 in the 成员 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找成员(char *串名) {
  return (列表内找符号(串名, 成员头, 0));
}

// Find a struct in the struct 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找结构(char *串名) {
  return (列表内找符号(串名, 结构头, 0));
}

// Find a struct in the union 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找联合(char *串名) {
  return (列表内找符号(串名, 联合头, 0));
}

// Find an enum 类型 in the enum 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找枚举类型(char *串名) {
  return (列表内找符号(串名, 枚举头, 类别_枚举类型));
}

// Find an enum 值 in the enum 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找枚举值(char *串名) {
  return (列表内找符号(串名, 枚举头, 类别_枚举值));
}

// Find a 类型 in the tyedef 列表
// Return a pointer to the found 节点 or NULL if not found.
struct 符号表 *找类型定义(char *串名) {
  return (列表内找符号(串名, 类型定义头, 0));
}

// Reset the contents of the symbol table
void 清除_符号表(void) {
  全局头 = 全局尾 = NULL;
  本地头 = 本地尾 = NULL;
  形参头 = 形参尾 = NULL;
  成员头 = 成员尾 = NULL;
  结构头 = 结构尾 = NULL;
  联合头 = 联合尾 = NULL;
  枚举头 = 枚举尾 = NULL;
  类型定义头 = 类型定义尾 = NULL;
}

// Clear all the entries in the local symbol table
void 自由本地符号(void) {
  本地头 = 本地尾 = NULL;
  形参头 = 形参尾 = NULL;
  函数身份 = NULL;
}

// Remove all static symbols from the global symbol table
void 自由静态符号(void) {
  // 小全局 points at current 节点, 上一个 at the previous one
  struct 符号表 *小全局, *上一个 = NULL;

  // Walk the global table looking for static entries
  for (小全局 = 全局头; 小全局 != NULL; 小全局 = 小全局->下一个) {
    if (小全局->类别 == 类别_静态) {

      // If there'串名 a previous 节点, rearrange the 上一个 pointer
      // to 跳过 over the current 节点. If not, 小全局 is the 头部,
      // so do the same to 全局头
      if (上一个 != NULL)
	上一个->下一个 = 小全局->下一个;
      else
	全局头->下一个 = 小全局->下一个;

      // If 小全局 is the 节点, point 全局尾 at the previous 节点
      // (if there is one), or 全局头
      if (小全局 == 全局尾) {
	if (上一个 != NULL)
	  全局尾 = 上一个;
	else
	  全局尾 = 全局头;
      }
    }
  }

  // Point 上一个 at 小全局 before we move up to the 下一个 节点
  上一个 = 小全局;
}

// Dump a single symbol
static void 转储单个符号(struct 符号表 *符号, int 缩进) {
  int 甲;

  for (甲 = 0; 甲 < 缩进; 甲++)
    printf(" ");
  switch (符号->类型 & (~0xf)) {
    case 原语_空的:
      printf("空 ");
      break;
    case 原语_印刻:
      printf("印刻 ");
      break;
    case 原语_整型:
      printf("整型 ");
      break;
    case 原语_长型:
      printf("长型 ");
      break;
    case 原语_结构:
      if (符号->复合类型 != NULL)
	printf("结构 %s ", 符号->复合类型->名字);
      else
	printf("结构 %s ", 符号->名字);
      break;
    case 原语_联合:
      if (符号->复合类型 != NULL)
	printf("联合 %s ", 符号->复合类型->名字);
      else
	printf("联合 %s ", 符号->名字);
      break;
    default:
      printf("未知 类型 ");
  }

  for (甲 = 0; 甲 < (符号->类型 & 0xf); 甲++)
    printf("*");
  printf("%s", 符号->名字);

  switch (符号->结构化类型) {
    case 结构化_可变的:
      break;
    case 结构化_函数:
      printf("()");
      break;
    case 结构化_数组:
      printf("[]");
      break;
    default:
      printf(" unknown 结构化类型");
  }

  switch (符号->类别) {
    case 类别_全局:
      printf(": 全局");
      break;
    case 类别_本地:
      printf(": 本地");
      break;
    case 类别_形参:
      printf(": 形参");
      break;
    case 类别_外部:
      printf(": 外部");
      break;
    case 类别_静态:
      printf(": 静态");
      break;
    case 类别_结构:
      printf(": 结构");
      break;
    case 类别_联合:
      printf(": 联合");
      break;
    case 类别_成员:
      printf(": 成员");
      break;
    case 类别_枚举类型:
      printf(": 枚举类型");
      break;
    case 类别_枚举值:
      printf(": 枚举值");
      break;
    case 类别_类型定义:
      printf(": 类型定义");
      break;
    default:
      printf(": 未知 类别");
  }

  switch (符号->结构化类型) {
    case 结构化_可变的:
      if (符号->类别 == 类别_枚举值)
	printf(", 值 %d\n", 符号->符号表_本地符号位置信息);
      else
	printf(", 大小 %d\n", 符号->大小);
      break;
    case 结构化_函数:
      printf(", %d params\n", 符号->元素个数);
      break;
    case 结构化_数组:
      printf(", %d elems, 大小 %d\n", 符号->元素个数, 符号->大小);
      break;
  }

  switch (符号->类型 & (~0xf)) {
    case 原语_结构:
    case 原语_联合:
      转储表(符号->成员, NULL, 4);
  }

  switch (符号->结构化类型) {
    case 结构化_函数:
      转储表(符号->成员, NULL, 4);
  }
}

// Dump one symbol table
void 转储表(struct 符号表 *头部, char *名字, int 缩进) {
  struct 符号表 *符号;

  if (头部 != NULL && 名字 != NULL)
    printf("%s\n--------\n", 名字);
  for (符号 = 头部; 符号 != NULL; 符号 = 符号->下一个)
    转储单个符号(符号, 缩进);
}

void 转储符号表(void) {
  转储表(全局头, "全局", 0);
  printf("\n");
  转储表(枚举头, "枚举", 0);
  printf("\n");
  转储表(类型定义头, "类型定义", 0);
}

/*------------------------------------------------
--------- 声明.印名-----------
------------------------------------------------*/

// Parsing of declarations
//   (印名)   

static struct 符号表 *复合_声明(int 类型);
static int 类型定义_声明(struct 符号表 **复合类型);
static int 类型定义_的_类型(char *名字, struct 符号表 **复合类型);
static void 枚举_声明(void);

// Parse the current 牌 and return a primitive 类型 enum 值,
// a pointer to any composite 类型 and possibly modify
// the 类别 of the 类型.
int 解析_类型(struct 符号表 **复合类型, int *类别) {
  int 类型, 外部或静态 = 1;

  // See if the 类别 has been changed to extern or static
  while (外部或静态) {
    switch (牌_.牌) {
      case 牌名_外部:
	if (*类别 == 类别_静态)
	  致命("Illegal to have extern and static at the same time");
	*类别 = 类别_外部;
	扫描(&牌_);
	break;
      case 牌名_静态:
	if (*类别 == 类别_本地)
	  致命("编译器不支持静态本地声明");
	if (*类别 == 类别_外部)
	  致命("Illegal to have extern and static at the same time");
	*类别 = 类别_静态;
	扫描(&牌_);
	break;
      default:
	外部或静态 = 0;
    }
  }

  // Now work on the actual 类型 关键字
  switch (牌_.牌) {
    case 牌名_空的:
      类型 = 原语_空的;
      扫描(&牌_);
      break;
    case 牌名_印刻:
      类型 = 原语_印刻;
      扫描(&牌_);
      break;
    case 牌名_整型:
      类型 = 原语_整型;
      扫描(&牌_);
      break;
    case 牌名_长型:
      类型 = 原语_长型;
      扫描(&牌_);
      break;

      // For the following, if we have a ';' after the
      // parsing then there is no 类型, so return -1.
      // Example: struct x {int y; int z};
    case 牌名_结构:
      类型 = 原语_结构;
      *复合类型 = 复合_声明(原语_结构);
      if (牌_.牌 == 牌名_分号)
	类型 = -1;
      break;
    case 牌名_联合:
      类型 = 原语_联合;
      *复合类型 = 复合_声明(原语_联合);
      if (牌_.牌 == 牌名_分号)
	类型 = -1;
      break;
    case 牌名_枚举:
      类型 = 原语_整型;		// Enums are really ints
      枚举_声明();
      if (牌_.牌 == 牌名_分号)
	类型 = -1;
      break;
    case 牌名_类型定义:
      类型 = 类型定义_声明(复合类型);
      if (牌_.牌 == 牌名_分号)
	类型 = -1;
      break;
    case 牌名_标识:
      类型 = 类型定义_的_类型(文本, 复合类型);
      break;
    default:
      致命符("Illegal 类型, 牌", 牌_.牌串);
  }
  return (类型);
}

// Given a 类型 parsed by 解析_类型(), 扫描 in any following
// '*' tokens and return the new 类型
int 解析_星号(int 类型) {

  while (1) {
    if (牌_.牌 != 牌名_星号)
      break;
    类型 = 指针_到(类型);
    扫描(&牌_);
  }
  return (类型);
}

// Parse a 类型 which appears inside a cast
int 解析_投射(struct 符号表 **复合类型) {
  int 类型, 类别 = 0;

  // Get the 类型 inside the parentheses
  类型 = 解析_星号(解析_类型(复合类型, &类别));

  // Do some error checking. I'm sure more can be done
  if (类型 == 原语_结构 || 类型 == 原语_联合 || 类型 == 原语_空的)
    致命("不能投射到一个结构, 联合或空类型");
  return (类型);
}

// Given a 类型, parse an expression of literals and ensure
// that the 类型 of this expression matches the given 类型.
// Parse any 类型 cast that precedes the expression.
// If an integer literal, return this 值.
// If a 串 literal, return the 标号 number of the 串.
int 解析_字面(int 类型) {
  struct 抽象句法树节点 *树;

  // Parse the expression and 优化 the resulting AST 树
  树 = 优化(二元表达式(0));

  // If there'串名 a cast, get the 孩子 and
  // mark it as having the 类型 from the cast
  if (树->操作 == 抽象_投射) {
    树->左->类型 = 树->类型;
    树 = 树->左;
  }

  // The 树 must now have an integer or 串 literal
  if (树->操作 != 抽象_整型字面 && 树->操作 != 抽象_串字面)
    致命("Cannot initialise globals with a general expression");

  // If the 类型 is char * and
  if (类型 == 指针_到(原语_印刻)) {
    // We have a 串 literal, return the 标号 number
    if (树->操作 == 抽象_串字面)
      return (树->小抽象_整型值);
    // We have a zero int literal, so that'串名 a NULL
    if (树->操作 == 抽象_整型字面 && 树->小抽象_整型值 == 0)
      return (0);
  }

  // We only get here with an integer literal. The input 类型
  // is an integer 类型 and is wide enough to hold the literal 值
  if (整型类型(类型) && 类型大小(类型, NULL) >= 类型大小(树->类型, NULL))
    return (树->小抽象_整型值);

  致命("Type mismatch: literal vs. variable");
  return (0);			// Keep -Wall happy
}

// Given a pointer to a symbol that may already exist
// return true if this symbol doesn't exist. We use
// this function to convert externs into globals
static int 是否_新_符号(struct 符号表 *符号, int 类别, 
		  int 类型, struct 符号表 *复合类型) {

  // There is no existing symbol, thus is new
  if (符号==NULL) return(1);

  // global versus extern: if they 匹配 that it'串名 not new
  // and we can convert the 类别 to global
  if ((符号->类别== 类别_全局 && 类别== 类别_外部)
      || (符号->类别== 类别_外部 && 类别== 类别_全局)) {

      // If the types don't 匹配, there'串名 a problem
      if (类型 != 符号->类型)
        致命符("Type mismatch between global/extern", 符号->名字);

      // Struct/unions, also compare the 复合类型
      if (类型 >= 原语_结构 && 复合类型 != 符号->复合类型)
        致命符("Type mismatch between global/extern", 符号->名字);

      // If we get to here, the types 匹配, so mark the symbol
      // as global
      符号->类别= 类别_全局;
      // Return that symbol is not new
      return(0);
  }

  // It must be a duplicate symbol if we get here
  致命符("Duplicate global variable declaration", 符号->名字);
  return(-1);	// Keep -Wall happy
}

// Given the 类型, 名字 and 类别 of a scalar variable,
// parse any initialisation 值 and allocate storage for it.
// Return the variable'串名 symbol table entry.
static struct 符号表 *标量_声明(char *变量名字, int 类型,
					   struct 符号表 *复合类型,
					   int 类别, struct 抽象句法树节点 **树) {
  struct 符号表 *符号 = NULL;
  struct 抽象句法树节点 *变量节点, *表达式节点;
  *树 = NULL;

  // Add this as a known scalar
  switch (类别) {
    case 类别_静态:
    case 类别_外部:
    case 类别_全局:
      // See if this variable is new or already exists
      符号= 找全局(变量名字);
      if (是否_新_符号(符号, 类别, 类型, 复合类型))
        符号 = 添加全局(变量名字, 类型, 复合类型, 结构化_可变的, 类别, 1, 0);
      break;
    case 类别_本地:
      符号 = 添加本地(变量名字, 类型, 复合类型, 结构化_可变的, 1);
      break;
    case 类别_形参:
      符号 = 添加形参(变量名字, 类型, 复合类型, 结构化_可变的);
      break;
    case 类别_成员:
      符号 = 添加成员(变量名字, 类型, 复合类型, 结构化_可变的, 1);
      break;
  }

  // The variable is being initialised
  if (牌_.牌 == 牌名_赋值) {
    // Only possible for a global or local
    if (类别 != 类别_全局 && 类别 != 类别_本地 && 类别 != 类别_静态)
      致命符("Variable can not be initialised", 变量名字);
    扫描(&牌_);

    // Globals must be assigned a literal 值
    if (类别 == 类别_全局 || 类别 == 类别_静态) {
      // Create one initial 值 for the variable and
      // parse this 值
      符号->初始列表 = (int *) malloc(sizeof(int));
      符号->初始列表[0] = 解析_字面(类型);
    }
    if (类别 == 类别_本地) {
      // Make an 抽象_标识 AST 节点 with the variable
      变量节点 = 制作抽象句法树叶(抽象_标识, 符号->类型, 符号->复合类型, 符号, 0);

      // Get the expression for the assignment, make into a 右值
      表达式节点 = 二元表达式(0);
      表达式节点->右值 = 1;

      // Ensure the expression'串名 类型 matches the variable
      表达式节点 = 修改_类型(表达式节点, 变量节点->类型, 变量节点->复合类型, 0);
      if (表达式节点 == NULL)
	致命("Incompatible expression in assignment");

      // Make an assignment AST 树
      *树 = 制作抽象句法树节点(抽象_赋值, 表达式节点->类型, 表达式节点->复合类型, 表达式节点,
			NULL, 变量节点, NULL, 0);
    }
  }

  // Generate any global space
  if (类别 == 类别_全局 || 类别 == 类别_静态)
    生成全局符号(符号);

  return (符号);
}

// Given the 类型, 名字 and 类别 of an array variable, parse
// the 大小 of the array, if any. Then parse any initialisation
// 值 and allocate storage for it.
// Return the variable'串名 symbol table entry.
static struct 符号表 *数组_声明(char *变量名字, int 类型,
					  struct 符号表 *复合类型, int 类别) {

  struct 符号表 *符号;	// New symbol table entry
  int 元素个数 = -1;	// Assume the number of elements won't be given
  int 最大元素;		// The maximum number of elements in the 初始 列表
  int *初始列表;	// The 列表 of initial elements 
  int 甲 = 0, 乙;

  // Skip past the '['
  扫描(&牌_);

  // See if we have an array 大小
  if (牌_.牌 != 牌名_右中括号) {
    元素个数 = 解析_字面(原语_整型);
    if (元素个数 <= 0)
      致命数("Array 大小 is illegal", 元素个数);
  }

  // Ensure we have a following ']'
  匹配(牌名_右中括号, "]");

  // Add this as a known array. We treat the
  // array as a pointer to its elements' 类型
  switch (类别) {
    case 类别_静态:
    case 类别_外部:
    case 类别_全局:
      // See if this variable is new or already exists
      符号= 找全局(变量名字);
      if (是否_新_符号(符号, 类别, 指针_到(类型), 复合类型))
        符号 = 添加全局(变量名字, 指针_到(类型), 复合类型, 结构化_数组, 类别, 0, 0);
      break;
    case 类别_本地:
      符号 = 添加本地(变量名字, 指针_到(类型), 复合类型, 结构化_数组, 0);
      break;
    default:
      致命("Declaration of array parameters is not implemented");
  }

  // Array initialisation
  if (牌_.牌 == 牌名_赋值) {
    if (类别 != 类别_全局 && 类别 != 类别_静态)
      致命符("Variable can not be initialised", 变量名字);
    扫描(&牌_);

    // Get the following 左 curly bracket
    匹配(牌名_左大括号, "{");

#define 表_自增 10

    // If the array already has 元素个数, allocate that many elements
    // in the 列表. Otherwise, start with 表_自增.
    if (元素个数 != -1)
      最大元素 = 元素个数;
    else
      最大元素 = 表_自增;
    初始列表 = (int *) malloc(最大元素 * sizeof(int));

    // Loop getting a new literal 值 from the 列表
    while (1) {

      // Check we can add the 下一个 值, then parse and add it
      if (元素个数 != -1 && 甲 == 最大元素)
	致命("Too many values in initialisation 列表");

      初始列表[甲++] = 解析_字面(类型);

      // Increase the 列表 大小 if the original 大小 was
      // not set and we have hit the end of the current 列表
      if (元素个数 == -1 && 甲 == 最大元素) {
	最大元素 += 表_自增;
	初始列表 = (int *) realloc(初始列表, 最大元素 * sizeof(int));
      }

      // Leave when we hit the 右 curly bracket
      if (牌_.牌 == 牌名_右大括号) {
	扫描(&牌_);
	break;
      }

      // Next 牌 must be a 逗号, then
      逗号();
    }

    // Zero any 未用的 elements in the 初始列表.
    // Attach the 列表 to the symbol table entry
    for (乙 = 甲; 乙 < 符号->元素个数; 乙++)
      初始列表[乙] = 0;

    if (甲 > 元素个数)
      元素个数 = 甲;
    符号->初始列表 = 初始列表;
  }

  // Set the 大小 of the array and the number of elements
  // Only externs can have no elements.
  if (类别 != 类别_外部 && 元素个数<=0)
    致命符("Array must have non-zero elements", 符号->名字);

  符号->元素个数 = 元素个数;
  符号->大小 = 符号->元素个数 * 类型大小(类型, 复合类型);
  // Generate any global space
  if (类别 == 类别_全局 || 类别 == 类别_静态)
    生成全局符号(符号);
  return (符号);
}

// Given a pointer to the new function being declared and
// a possibly NULL pointer to the function'串名 previous declaration,
// parse a 列表 of parameters and cross-check them against the
// previous declaration. Return the count of parameters
static int 形参_声明_列表(struct 符号表 *旧函数符号,
				  struct 符号表 *新函数符号) {
  int 类型, 形参计数 = 0;
  struct 符号表 *复合类型;
  struct 符号表 *原型指针 = NULL;
  struct 抽象句法树节点 *未用的;

  // Get the pointer to the first prototype parameter
  if (旧函数符号 != NULL)
    原型指针 = 旧函数符号->成员;

  // Loop getting any parameters
  while (牌_.牌 != 牌名_右小括号) {

    // If the first 牌 is 'void'
    if (牌_.牌 == 牌名_空的) {
      // Peek at the 下一个 牌. If a ')', the function
      // has no parameters, so leave the loop.
      扫描(&窥牌);
      if (窥牌.牌 == 牌名_右小括号) {
	// Move the 窥牌 into the 牌_
	形参计数 = 0;
	扫描(&牌_);
	break;
      }
    }

    // Get the 类型 of the 下一个 parameter
    类型 = 声明_列表(&复合类型, 类别_形参, 牌名_逗号, 牌名_右小括号, &未用的);
    if (类型 == -1)
      致命("Bad 类型 in parameter 列表");

    // Ensure the 类型 of this parameter matches the prototype
    if (原型指针 != NULL) {
      if (类型 != 原型指针->类型)
	致命数("Type doesn't 匹配 prototype for parameter", 形参计数 + 1);
      原型指针 = 原型指针->下一个;
    }
    形参计数++;

    // Stop when we hit the 右 parenthesis
    if (牌_.牌 == 牌名_右小括号)
      break;
    // We need a 逗号 as separator
    逗号();
  }

  if (旧函数符号 != NULL && 形参计数 != 旧函数符号->元素个数)
    致命符("Parameter count mismatch for function", 旧函数符号->名字);

  // Return the count of parameters
  return (形参计数);
}

//
// 函数_声明: 类型 标识符 '(' parameter_list ')' ;
//      | 类型 标识符 '(' parameter_list ')' 复合_语句   ;
//
// Parse the declaration of function.
static struct 符号表 *函数_声明(char *函数名字, int 类型,
					     struct 符号表 *复合类型,
					     int 类别) {
  struct 抽象句法树节点 *树, *最终语句;
  struct 符号表 *旧函数符号, *新函数符号 = NULL;
  int 标号终, 形参计数;
  int 行号= 行;

  // 文本 has the 标识符'串名 名字. If this exists and is a
  // function, get the 身份. Otherwise, set 旧函数符号 to NULL.
  if ((旧函数符号 = 找符号(函数名字)) != NULL)
    if (旧函数符号->结构化类型 != 结构化_函数)
      旧函数符号 = NULL;

  // If this is a new function declaration, get a
  // 标号-身份 for the end 标号, and add the function
  // to the symbol table,
  if (旧函数符号 == NULL) {
    标号终 = 生成标号();
    // Assumption: functions only return scalar types, so NULL below
    新函数符号 =
      添加全局(函数名字, 类型, NULL, 结构化_函数, 类别, 0, 标号终);
  }

  // Scan in the '(', any parameters and the ')'.
  // Pass in any existing function prototype pointer
  左小括号();
  形参计数 = 形参_声明_列表(旧函数符号, 新函数符号);
  右小括号();

  // If this is a new function declaration, update the
  // function symbol entry with the number of parameters.
  // Also copy the parameter 列表 into the function'串名 节点.
  if (新函数符号) {
    新函数符号->元素个数 = 形参计数;
    新函数符号->成员 = 形参头;
    旧函数符号 = 新函数符号;
  }

  // Clear out the parameter 列表
  形参头 = 形参尾 = NULL;

  // If the declaration ends in a semicolon, only a prototype.
  if (牌_.牌 == 牌名_分号)
    return (旧函数符号);

  // This is not just a prototype.
  // Set the 函数身份 global to the function'串名 symbol pointer
  函数身份 = 旧函数符号;

  // Get the AST 树 for the compound 语句 and mark
  // that we have parsed no loops or switches yet
  环级 = 0;
  切换级 = 0;
  左大括号();
  树 = 复合_语句(0);
  右大括号();

  // If the function 类型 isn't 原语_空的 ...
  if (类型 != 原语_空的) {

    // Error if no 语句_复 in the function
    if (树 == NULL)
      致命("No 语句_复 in function with non-void 类型");

    // Check that the last AST operation in the
    // compound 语句 was a return 语句
    最终语句 = (树->操作 == 抽象_胶水) ? 树->右 : 树;
    if (最终语句 == NULL || 最终语句->操作 != 抽象_返回)
      致命("No return for function with non-void 类型");
  }

  // Build the 抽象_函数 节点 which has the function'串名 symbol pointer
  // and the compound 语句 sub-树
  树 = 制作抽象句法树一元(抽象_函数, 类型, 复合类型, 树, 旧函数符号, 标号终);
  树->行号= 行号;

  // Do optimisations on the AST 树
  树 = 优化(树);

  // Dump the AST 树 if requested
  if (对象_转储抽象句法树) {
    转储抽象句法树(树, 无标号, 0);
    fprintf(stdout, "\n\n");
  }

  // Generate the assembly code for it
  生成抽象句法树(树, 无标号, 无标号, 无标号, 0);

  // Now free the symbols associated with this function
  自由本地符号();
  return (旧函数符号);
}

// Parse composite 类型 declarations: structs or unions.
// Either find an existing struct/union declaration, or build
// a struct/union symbol table entry and return its pointer.
static struct 符号表 *复合_声明(int 类型) {
  struct 符号表 *复合类型 = NULL;
  struct 符号表 *小成员;
  struct 抽象句法树节点 *未用的;
  int 偏移;
  int 牌名;

  // Skip the struct/union 关键字
  扫描(&牌_);

  // See if there is a following struct/union 名字
  if (牌_.牌 == 牌名_标识) {
    // Find any matching composite 类型
    if (类型 == 原语_结构)
      复合类型 = 找结构(文本);
    else
      复合类型 = 找联合(文本);
    扫描(&牌_);
  }

  // If the 下一个 牌 isn't an LBRACE , this is
  // the 用法 of an existing struct/union 类型.
  // Return the pointer to the 类型.
  if (牌_.牌 != 牌名_左大括号) {
    if (复合类型 == NULL)
      致命符("unknown struct/union 类型", 文本);
    return (复合类型);
  }

  // Ensure this struct/union 类型 hasn't been
  // previously defined
  if (复合类型)
    致命符("previously defined struct/union", 文本);

  // Build the composite 类型 and 跳过 the 左 brace
  if (类型 == 原语_结构)
    复合类型 = 添加结构(文本);
  else
    复合类型 = 添加联合(文本);
  扫描(&牌_);

  // Scan in the 列表 of members
  while (1) {
    // Get the 下一个 成员. 小成员 is used as a dummy
    牌名 = 声明_列表(&小成员, 类别_成员, 牌名_分号, 牌名_右大括号, &未用的);
    if (牌名 == -1)
      致命("Bad 类型 in 成员 列表");
    if (牌_.牌 == 牌名_分号)
      扫描(&牌_);
    if (牌_.牌 == 牌名_右大括号)
      break;
  }

  // Attach to the struct 类型'串名 节点
  右大括号();
  if (成员头 == NULL)
    致命符("No members in struct", 复合类型->名字);
  复合类型->成员 = 成员头;
  成员头 = 成员尾 = NULL;

  // Set the 偏移 of the initial 成员
  // and find the first free byte after it
  小成员 = 复合类型->成员;
  小成员->符号表_本地符号位置信息 = 0;
  偏移 = 类型大小(小成员->类型, 小成员->复合类型);

  // Set the position of each successive 成员 in the composite 类型
  // Unions are easy. For structs, align the 成员 and find the 下一个 free byte
  for (小成员 = 小成员->下一个; 小成员 != NULL; 小成员 = 小成员->下一个) {
    // Set the 偏移 for this 成员
    if (类型 == 原语_结构)
      小成员->符号表_本地符号位置信息 = 生成对齐(小成员->类型, 偏移, 1);
    else
      小成员->符号表_本地符号位置信息 = 0;

    // Get the 偏移 of the 下一个 free byte after this 成员
    偏移 += 类型大小(小成员->类型, 小成员->复合类型);
  }

  // Set the overall 大小 of the composite 类型
  复合类型->大小 = 偏移;
  return (复合类型);
}

// Parse an enum declaration
static void 枚举_声明(void) {
  struct 符号表 *小枚举类型 = NULL;
  char *名字 = NULL;
  int 小整型值 = 0;

  // Skip the enum 关键字.
  扫描(&牌_);

  // If there'串名 a following enum 类型 名字, get a
  // pointer to any existing enum 类型 节点.
  if (牌_.牌 == 牌名_标识) {
    小枚举类型 = 找枚举类型(文本);
    名字 = strdup(文本);	// As it gets tromped soon
    扫描(&牌_);
  }

  // If the 下一个 牌 isn't a LBRACE, check
  // that we have an enum 类型 名字, then return
  if (牌_.牌 != 牌名_左大括号) {
    if (小枚举类型 == NULL)
      致命符("undeclared enum 类型:", 名字);
    return;
  }

  // We do have an LBRACE. Skip it
  扫描(&牌_);

  // If we have an enum 类型 名字, ensure that it
  // hasn't been declared before.
  if (小枚举类型 != NULL)
    致命符("enum 类型 redeclared:", 小枚举类型->名字);
  else
    // Build an enum 类型 节点 for this 标识符
    小枚举类型 = 添加枚举(名字, 类别_枚举类型, 0);

  // Loop to get all the enum values
  while (1) {
    // Ensure we have an 标识符
    // Copy it in case there'串名 an int literal coming up
    标识();
    名字 = strdup(文本);

    // Ensure this enum 值 hasn't been declared before
    小枚举类型 = 找枚举值(名字);
    if (小枚举类型 != NULL)
      致命符("enum 值 redeclared:", 文本);

    // If the 下一个 牌 is an '=', 跳过 it and
    // get the following int literal
    if (牌_.牌 == 牌名_赋值) {
      扫描(&牌_);
      if (牌_.牌 != 牌名_整型字面)
	致命("Expected int literal after '='");
      小整型值 = 牌_.整型值;
      扫描(&牌_);
    }

    // Build an enum 值 节点 for this 标识符.
    // Increment the 值 for the 下一个 enum 标识符.
    小枚举类型 = 添加枚举(名字, 类别_枚举值, 小整型值++);

    // Bail out on a 右 curly bracket, else get a 逗号
    if (牌_.牌 == 牌名_右大括号)
      break;
    逗号();
  }
  扫描(&牌_);			// Skip over the 右 curly bracket
}

// Parse a typedef declaration and return the 类型
// and 复合类型 that it represents
static int 类型定义_声明(struct 符号表 **复合类型) {
  int 类型, 类别 = 0;

  // Skip the typedef 关键字.
  扫描(&牌_);

  // Get the actual 类型 following the 关键字
  类型 = 解析_类型(复合类型, &类别);
  if (类别 != 0)
    致命("Can't have static/extern in a typedef declaration");

  // See if the typedef 标识符 already exists
  if (找类型定义(文本) != NULL)
    致命符("redefinition of typedef", 文本);

  // Get any following '*' tokens
  类型 = 解析_星号(类型);

  // It doesn't exist so add it to the typedef 列表
  添加类型定义(文本, 类型, *复合类型);
  扫描(&牌_);
  return (类型);
}

// Given a typedef 名字, return the 类型 it represents
static int 类型定义_的_类型(char *名字, struct 符号表 **复合类型) {
  struct 符号表 *牌名;

  // Look up the typedef in the 列表
  牌名 = 找类型定义(名字);
  if (牌名 == NULL)
    致命符("unknown 类型", 名字);
  扫描(&牌_);
  *复合类型 = 牌名->复合类型;
  return (牌名->类型);
}

// Parse the declaration of a variable or function.
// The 类型 and any following '*'串名 have been scanned, and we
// have the 标识符 in the 牌_ variable.
// The 类别 argument is the symbol'串名 类别.
// Return a pointer to the symbol'串名 entry in the symbol table
static struct 符号表 *符号_声明(int 类型, struct 符号表 *复合类型,
					   int 类别, struct 抽象句法树节点 **树) {
  struct 符号表 *符号 = NULL;
  char *变量名字 = strdup(文本);

  // Ensure that we have an 标识符. 
  // We copied it above so we can 扫描 more tokens in, e.g.
  // an assignment expression for a local variable.
  标识();

  // Deal with function declarations
  if (牌_.牌 == 牌名_左小括号) {
    return (函数_声明(变量名字, 类型, 复合类型, 类别));
  }

  // See if this array or scalar variable has already been declared
  switch (类别) {
    case 类别_外部:
    case 类别_静态:
    case 类别_全局:
    case 类别_本地:
    case 类别_形参:
      if (找本地(变量名字) != NULL)
	致命符("Duplicate local variable declaration", 变量名字);
    case 类别_成员:
      if (找成员(变量名字) != NULL)
	致命符("Duplicate struct/union 成员 declaration", 变量名字);
  }

  // Add the array or scalar variable to the symbol table
  if (牌_.牌 == 牌名_左中括号) {
    符号 = 数组_声明(变量名字, 类型, 复合类型, 类别);
    *树= NULL;	// Local arrays are not initialised
  } else
    符号 = 标量_声明(变量名字, 类型, 复合类型, 类别, 树);
  return (符号);
}

// Parse a 列表 of symbols where there is an initial 类型.
// Return the 类型 of the symbols. et1 and et2 are end tokens.
int 声明_列表(struct 符号表 **复合类型, int 类别, int et1, int et2,
		     struct 抽象句法树节点 **胶水树) {
  int 初始类型, 类型;
  struct 符号表 *符号;
  struct 抽象句法树节点 *树;
  *胶水树 = NULL;

  // Get the initial 类型. If -1, it was
  // a composite 类型 definition, return this
  if ((初始类型 = 解析_类型(复合类型, &类别)) == -1)
    return (初始类型);

  // Now parse the 列表 of symbols
  while (1) {
    // See if this symbol is a pointer
    类型 = 解析_星号(初始类型);

    // Parse this symbol
    符号 = 符号_声明(类型, *复合类型, 类别, &树);

    // We parsed a function, there is no 列表 so leave
    if (符号->结构化类型 == 结构化_函数) {
      if (类别 != 类别_全局 && 类别 != 类别_静态)
	致命("Function definition not at global 等级");
      return (类型);
    }

    // Glue any AST 树 from a local declaration
    // to build a sequence of assignments to perform
    if (*胶水树 == NULL)
      *胶水树 = 树;
    else
      *胶水树 =
	制作抽象句法树节点(抽象_胶水, 原语_全无, NULL, *胶水树, NULL, 树, NULL, 0);

    // We are at the end of the 列表, leave
    if (牌_.牌 == et1 || 牌_.牌 == et2)
      return (类型);

    // Otherwise, we need a 逗号 as separator
    逗号();
  }

  return(0);	// Keep -Wall happy
}

// Parse one or more global declarations,
// either variables, functions or structs
void 全局_声明(void) {
  struct 符号表 *复合类型= NULL;
  struct 抽象句法树节点 *未用的;

  // Loop parsing one declaration 列表 until the end of file
  while (牌_.牌 != 牌名_文件终) {
    声明_列表(&复合类型, 类别_全局, 牌名_分号, 牌名_文件终, &未用的);

    // Skip any separating semicolons
    if (牌_.牌 == 牌名_分号)
      扫描(&牌_);
  }
}
