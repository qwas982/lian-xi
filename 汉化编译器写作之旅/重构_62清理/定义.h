/*------------------------------------------------
---------------------所有.h头文件------------------
------------------------------------------------*/
#ifndef _定义_H_
#define _定义_H_


//unistd.h
void _exit(int status);
int 不链接(char *pathname);

//复合类型.h
int isalnum(int 印名);
int isalpha(int 印名);
int iscntrl(int 印名);
int isdigit(int 印名);
int isgraph(int 印名);
int islower(int 印名);
int isprint(int 印名);
int ispunct(int 印名);
int isspace(int 印名);
int isupper(int 印名);
int isxdigit(int 印名);
int isascii(int 印名);
int isblank(int 印名);

int toupper(int 印名);
int tolower(int 印名);

//errno.h
int * __errno_location(void);

#define errno (* __errno_location())

//fcntl.h
#define O_RDONLY 00
#define O_WRONLY 01
#define O_RDWR   02

int open(char *pathname, int flags);

//stddef.h
#ifndef NULL
#define NULL (void *)0
#endif

typedef long size_t;

//stdio.h
#ifndef NULL
#define NULL (void *)0
#endif

#ifndef EOF
# define EOF (-1)
#endif

// This FILE definition will do for now
typedef char * FILE;

FILE *fopen(char *pathname, char *mode);
size_t fread(void *ptr, size_t 大小, size_t nmemb, FILE *stream);
size_t fwrite(void *ptr, size_t 大小, size_t nmemb, FILE *stream);
int fclose(FILE *stream);
int printf(char *format);
int fprintf(FILE *stream, char *format);
int sprintf(char *小串, char *format);
int snprintf(char *小串, size_t 大小, char *format);
int fgetc(FILE *stream);
int fputc(int 印名, FILE *stream);
int fputs(char *串名, FILE *stream);
int putc(int 印名, FILE *stream);
int putchar(int 印名);
int puts(char *串名);
FILE *popen(char *command, char *类型);
int pclose(FILE *stream);

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

//stdlib.h
void exit(int status);
void _Exit(int status);

void *malloc(int 大小);
void free(void *ptr);
void *calloc(int nmemb, int 大小);
void *realloc(void *ptr, int 大小);
int system(char *command);

//串.h
char *strdup(char *串名);
char *strchr(char *串名, int 印名);
char *strrchr(char *串名, int 印名);
int strcmp(char *s1, char *s2);
int strncmp(char *s1, char *s2, size_t 小节点);
char *strerror(int errnum);

/*------------------------------------------------
----------------此子语言内的.h头-------------------
---------------数据.h 声明.h 定义_.h--------------
------------------------------------------------*/

//----------------数据.h----------------
#ifndef 外部_
#define 外部_ extern
#endif

// Global variables
//   (印名)   

外部_ int 行;		     	// Current 行_ number
外部_ int 行开始;		     	// True if at start of a 行_
外部_ int 放回去;		     	// Character put back by scanner
外部_ struct 符号表 *函数身份; 	// Symbol ptr of the current function
外部_ FILE *入文件;		     	// Input and output files
外部_ FILE *出文件;
外部_ char *入文件名;		// Name of file we are parsing
外部_ char *出文件名;		// Name of file we opened as 出文件
外部_ struct 牌 牌_;		// Last 牌 scanned
外部_ struct 牌 窥牌;		// A look-ahead 牌
外部_ char 文本[文本长 + 1];		// Last 标识符 scanned
外部_ int 环级;			// Depth of nested loops
外部_ int 切换级;		// Depth of nested switches
extern char *牌名串[];			// List of 牌 strings

// 符号表列表
外部_ struct 符号表 *全局头, *全局尾;	  // Global variables and functions
外部_ struct 符号表 *本地头, *本地尾;	  // Local variables
外部_ struct 符号表 *形参头, *形参尾;	  // Local parameters
外部_ struct 符号表 *成员头, *成员尾;	  // Temp 列表 of struct/union members
外部_ struct 符号表 *结构头, *结构尾; // List of struct types
外部_ struct 符号表 *联合头, *联合尾;   // List of union types
外部_ struct 符号表 *枚举头,  *枚举尾;    // List of enum types and values
外部_ struct 符号表 *类型定义头,  *类型定义尾;    // List of typedefs

// Command-行_ flags
外部_ int 对象_转储抽象句法树;		// If true, dump the AST trees
外部_ int 对象_转储符号表;		// If true, dump the symbol table
外部_ int 对象_保留汇编;		// If true, keep any assembly files
外部_ int 对象_汇编;		// If true, assemble the assembly files
外部_ int 对象_做链接;		// If true, link the object files
外部_ int 对象_啰嗦;		// If true, print info on compilation stages

//----------------声明.h----------------
// Function prototypes for all compiler files
//   (印名)   

// 扫描.印名
void 拒绝_牌(struct 牌 *牌名);
int 扫描(struct 牌 *牌名);

// 树.印名
struct 抽象句法树节点 *制作抽象句法树节点(int 操作, int 类型,
			  struct 符号表 *复合类型,
			  struct 抽象句法树节点 *左,
			  struct 抽象句法树节点 *中,
			  struct 抽象句法树节点 *右,
			  struct 符号表 *符号, int 整型值);
struct 抽象句法树节点 *制作抽象句法树叶(int 操作, int 类型,
			  struct 符号表 *复合类型,
			  struct 符号表 *符号, int 整型值);
struct 抽象句法树节点 *制作抽象句法树一元(int 操作, int 类型,
			   struct 符号表 *复合类型,
			   struct 抽象句法树节点 *左,
			   struct 符号表 *符号, int 整型值);
void 转储抽象句法树(struct 抽象句法树节点 *小节点, int 标号, int 等级);

// gen.印名
int 生成标号(void);
int 生成抽象句法树(struct 抽象句法树节点 *小节点, int 若标号, int 环顶部标号,
	   int 环终标号, int 父节点抽象句法树操作);
void 生成前文(char *文件名);
void 生成后文();
void 生成自由寄存(int 保留寄存);
void 生成全局符号(struct 符号表 *节点);
int 生成全局串(char *串值, int 追加);
void 生成全局串终(void);
int 生成基元大小(int 类型);
int 生成对齐(int 类型, int 偏移, int 指向);
void 生成返回(int 寄存, int 身份);

// cg.印名
int 代码生成器基元大小(int 类型);
int 代码生成器对齐(int 类型, int 偏移, int 指向);
void 代码生成器文本段();
void 代码生成器数据段();
int 代码生成器分配寄存(void);
void 代码生成器自由全部寄存(int 保留寄存);
void 代码生成器自由寄存(int 寄存);
void 代码生成器溢出寄存(void);
void 代码生成器前文(char *文件名);
void 代码生成器后文();
void 代码生成器函前文(struct 符号表 *符号);
void 代码生成器后文(struct 符号表 *符号);
int 代码生成器载入整型(int 值, int 类型);
int 代码生成器载入变量(struct 符号表 *符号, int 操作);
int 代码生成器载入全局串(int 标号);
int 代码生成器加(int 寄存1, int 寄存2);
int 代码生成器减(int 寄存1, int 寄存2);
int 代码生成器乘(int 寄存1, int 寄存2);
int 代码生成器除法取余(int 寄存1, int 寄存2, int 操作);
int 代码生成器左移常量(int 小寄存, int 小值);
int 代码生成器调用(struct 符号表 *符号, int 实参数目);
void 代码生成器复制实参(int 小寄存, int 实参本地符号位置信息);
int 代码生成器存全局(int 小寄存, struct 符号表 *符号);
int 代码生成器存本地(int 小寄存, struct 符号表 *符号);
void 代码生成器全局符号(struct 符号表 *节点);
void 代码生成器全局串(int 小标号, char *串值, int 追加);
void 代码生成器全局串终(void);
int 代码生成器对比与设置(int 抽象句法树操作, int 寄存1, int 寄存2, int 类型);
int 代码生成器对比与跳(int 抽象句法树操作, int 寄存1, int 寄存2, int 标号, int 类型);
void 代码生成器标号(int 小标号);
void 代码生成器跳(int 小标号);
int 代码生成器加宽(int 小寄存, int 旧类型, int 新类型);
void 代码生成器返回(int 寄存, struct 符号表 *符号);
int 代码生成器地址(struct 符号表 *符号);
int 代码生成器解引用(int 小寄存, int 类型);
int 代码生成器存解引用(int 寄存1, int 寄存2, int 类型);
int 代码生成器否定(int 小寄存);
int 代码生成器颠倒(int 小寄存);
int 代码生成器逻辑非(int 小寄存);
void 代码生成器载入布尔(int 小寄存, int 小值);
int 代码生成器布尔(int 小寄存, int 操作, int 标号);
int 代码生成器与(int 寄存1, int 寄存2);
int 代码生成器或(int 寄存1, int 寄存2);
int 代码生成器异或(int 寄存1, int 寄存2);
int 代码生成器左移(int 寄存1, int 寄存2);
int 代码生成器右移(int 寄存1, int 寄存2);
void 代码生成器切换(int 寄存, int 情况计数, int 顶部标号,
	      int *情况标号, int *小情况值, int 默认标号);
void 代码生成器移(int 寄存1, int 寄存2);
void 代码生成器行号(int 行_);

// expr.印名
struct 抽象句法树节点 *表达式_列表(int 终牌);
struct 抽象句法树节点 *二元表达式(int 上牌优);

// 小语句.印名
struct 抽象句法树节点 *复合_语句(int 在切换内);

// misc.印名
void 匹配(int 牌名, char *what);
void 分号(void);
void 左大括号(void);
void 右大括号(void);
void 左小括号(void);
void 右小括号(void);
void 标识(void);
void 逗号(void);
void 致命(char *串名);
void 致命符(char *s1, char *s2);
void 致命数(char *串名, int d);
void 致命印名(char *串名, int 印名);

// 符号.印名
void 追加符号(struct 符号表 **头部, struct 符号表 **尾部,
	       struct 符号表 *尾部);
struct 符号表 *新符号(char *名字, int 类型, struct 符号表 *复合类型,
			int 结构化类型, int 类别, int 元素个数, int 本地符号位置信息);
struct 符号表 *添加全局(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 类别, int 元素个数, int 本地符号位置信息);
struct 符号表 *添加本地(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 元素个数);
struct 符号表 *添加形参(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型);
struct 符号表 *添加结构(char *名字);
struct 符号表 *添加联合(char *名字);
struct 符号表 *添加成员(char *名字, int 类型, struct 符号表 *复合类型,
			 int 结构化类型, int 元素个数);
struct 符号表 *添加枚举(char *名字, int 类别, int 值);
struct 符号表 *添加类型定义(char *名字, int 类型, struct 符号表 *复合类型);
struct 符号表 *找全局(char *串名);
struct 符号表 *找本地(char *串名);
struct 符号表 *找符号(char *串名);
struct 符号表 *找成员(char *串名);
struct 符号表 *找结构(char *串名);
struct 符号表 *找联合(char *串名);
struct 符号表 *找枚举类型(char *串名);
struct 符号表 *找枚举值(char *串名);
struct 符号表 *找类型定义(char *串名);
void 清除_符号表(void);
void 自由本地符号(void);
void 自由静态符号(void);
void 转储表(struct 符号表 *头部, char *名字, int 缩进);
void 转储符号表(void);

// 声明.印名
int 解析_类型(struct 符号表 **复合类型, int *类别);
int 解析_星号(int 类型);
int 解析_投射(struct 符号表 **复合类型);
int 声明_列表(struct 符号表 **复合类型, int 类别, int et1, int et2,
		     struct 抽象句法树节点 **胶水树);
void 全局_声明(void);

// types.印名
int 整型类型(int 类型);
int 指针类型(int 类型);
int 指针_到(int 类型);
int 值_位置(int 类型);
int 类型大小(int 类型, struct 符号表 *复合类型);
struct 抽象句法树节点 *修改_类型(struct 抽象句法树节点 *树, int 右类型,
			    struct 符号表 *右类别类型, int 操作);

// opt.印名
struct 抽象句法树节点 *优化(struct 抽象句法树节点 *小节点);

//----------------定义.h----------------

#include "incdir.h"

// Structure and enum definitions
//   (印名)   

enum {
  文本长 = 512			// Length of identifiers in input
};

// Commands and default filenames
#define 甲出 "a.out"
#define 汇编命令 "as -g -o "
#define 链接命令 "cc -g -o "
#define c预处理器命令 "cpp -nostdinc -isystem "

// 牌_ types
enum {
  牌名_文件终,

  // Binary operators
  牌名_赋值, 牌名_赋值加, 牌名_赋值减,
  牌名_赋值星号, 牌名_赋值斜杠, 牌名_赋值余数,
  牌名_问号, 牌名_逻辑或, 牌名_逻辑与,
  牌名_或, 牌名_异或, 牌名_与,
  牌名_等于, 牌名_不等,
  牌名_小于, 牌名_大于, 牌名_小等, 牌名_大等,
  牌名_左移, 牌名_右移,
  牌名_加, 牌名_减, 牌名_星号, 牌名_斜杠, 牌名_余数,

  // Other operators
  牌名_自增, 牌名_自减, 牌名_颠倒, 牌名_逻辑非,

  // Type keywords
  牌名_空的, 牌名_印刻, 牌名_整型, 牌名_长型,

  // Other keywords
  牌名_若, 牌名_否则, 牌名_一会儿, 牌名_为, 牌名_返回,
  牌名_结构, 牌名_联合, 牌名_枚举, 牌名_类型定义,
  牌名_外部, 牌名_断, 牌名_继续, 牌名_切换,
  牌名_情况, 牌名_默认, 牌名_求大小, 牌名_静态,

  // Structural tokens
  牌名_整型字面, 牌名_串字面, 牌名_分号, 牌名_标识,
  牌名_左大括号, 牌名_右大括号, 牌名_左小括号, 牌名_右小括号,
  牌名_左中括号, 牌名_右中括号, 牌名_逗号, 牌名_句号,
  牌名_箭头, 牌名_冒号
};

// 牌_ structure
struct 牌 {
  int 牌;			// 牌_ 类型, from the enum 列表 above
  char *牌串;			// String version of the 牌
  int 整型值;			// For 牌名_整型字面, the integer 值
};

// AST 节点 types. The first few 行_ up
// with the related tokens
enum {
  抽象_赋值 = 1, 抽象_赋值加, 抽象_赋值减, 抽象_赋值星号,			//  1
  抽象_赋值斜杠, 抽象_赋值余数, 抽象_三元运算符, 抽象_逻辑或,			//  5
  抽象_逻辑与, 抽象_或, 抽象_异或, 抽象_与, 抽象_等于, 抽象_不等, 抽象_小于,		//  9
  抽象_大于, 抽象_小等, 抽象_大等, 抽象_左移, 抽象_右移,				// 16
  抽象_加法, 抽象_减法, 抽象_乘法, 抽象_除法, 抽象_余数,		// 21
  抽象_整型字面, 抽象_串字面, 抽象_标识, 抽象_胶水,				// 26
  抽象_若, 抽象_一会儿, 抽象_函数, 抽象_加宽, 抽象_返回,			// 30
  抽象_函数调用, 抽象_解引用, 抽象_地址, 抽象_比例尺,				// 35
  抽象_前置自增, 抽象_前置自减, 抽象_后置自增, 抽象_后置自减,			// 39
  抽象_否定, 抽象_颠倒, 抽象_逻辑非, 抽象_到布尔, 抽象_断,		// 43
  抽象_继续, 抽象_切换, 抽象_情况, 抽象_默认, 抽象_投射		// 48
};

// Primitive types. The bottom 4 bits is an integer
// 值 that represents the 等级 of indirection,
// e.g. 0= no pointer, 1= pointer, 2= pointer pointer etc.
enum {
  原语_全无, 原语_空的 = 16, 原语_印刻 = 32, 原语_整型 = 48, 原语_长型 = 64,
  原语_结构=80, 原语_联合=96
};

// Structural types
enum {
  结构化_可变的, 结构化_函数, 结构化_数组
};

// Storage classes
enum {
  类别_全局 = 1,			// Globally visible symbol
  类别_本地,			// Locally visible symbol
  类别_形参,			// Locally visible function parameter
  类别_外部,			// External globally visible symbol
  类别_静态,			// Static symbol, visible in one file
  类别_结构,			// A struct
  类别_联合,			// A union
  类别_成员,			// Member of a struct or union
  类别_枚举类型,			// A named enumeration 类型
  类别_枚举值,			// A named enumeration 值
  类别_类型定义			// A named typedef
};

// Symbol table structure
struct 符号表 {
  char *名字;			// Name of a symbol
  int 类型;			// Primitive 类型 for the symbol
  struct 符号表 *复合类型;	// If struct/union, ptr to that 类型
  int 结构化类型;			// Structural 类型 for the symbol
  int 类别;			// Storage 类别 for the symbol
  int 大小;			// Total 大小 in bytes of this symbol
  int 元素个数;			// Functions: # params. Arrays: # elements
#define 符号表_终标号 符号表_本地符号位置信息	// For functions, the end 标号
  int 符号表_本地符号位置信息;			// For locals, the negative 偏移
    				// from the stack base pointer
  int *初始列表;		// List of initial values
  struct 符号表 *下一个;	// Next symbol in one 列表
  struct 符号表 *成员;	// First 成员 of a function, struct,
};				// union or enum

// Abstract Syntax Tree structure
struct 抽象句法树节点 {
  int 操作;			// "Operation" to be performed on this 树
  int 类型;			// Type of any expression this 树 generates
  struct 符号表 *复合类型;	// If struct/union, ptr to that 类型
  int 右值;			// True if the 节点 is an 右值
  struct 抽象句法树节点 *左;		// Left, middle and 右 孩子 trees
  struct 抽象句法树节点 *中;
  struct 抽象句法树节点 *右;
  struct 符号表 *符号;		// For many AST nodes, the pointer to
  				// the symbol in the symbol table
#define 小抽象_整型值 小抽象_大小	// For 抽象_整型字面, the integer 值
  int 小抽象_大小;			// For 抽象_比例尺, the 大小 to scale by
  int 行号;			// 行 number from where this 节点 comes
};

enum {
  无寄存 = -1,			// Use 无寄存 when the AST generation
  				// functions have no register to return
  无标号 = 0			// Use 无标号 when we have no 标号 to
    				// pass to 生成抽象句法树()
};

#endif //_定义_H_