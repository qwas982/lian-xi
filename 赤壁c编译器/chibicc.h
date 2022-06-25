#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <glob.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define 最大_缩(x, y) ((x) < (y) ? (y) : (x))
#define 最小_缩(x, y) ((x) < (y) ? (x) : (y))

#ifndef __GNUC__
# define __attribute__(x)
#endif

typedef struct 类型 类型;
typedef struct 节点 节点;
typedef struct 成员 成员;
typedef struct 重定位 重定位;
typedef struct 隐藏集 隐藏集;

//
// 字符串.c
//

typedef struct {
  char **数据;
  int 容量;
  int 长度_短;
} 字符串数组;

void 串数组_推(字符串数组 *数组_短, char *s);
char *格式化(char *fmt, ...) __attribute__((格式化(printf, 1, 2)));

//
// 化为牌.c
//

// 牌
typedef enum {
  牌_缩_标识符,   // Identifiers
  牌_缩_标点符号,   // Punctuators
  牌_缩_关键字, // Keywords
  牌_缩_串字面,     // String literals
  牌_缩_数值字面,     // Numeric literals
  牌_缩_预处理号码,  // Preprocessing numbers
  牌_缩_文件终,     // End-of-源文件位置 markers
} 牌种类;

typedef struct {
  char *名字;
  int 文件_号;
  char *内容;

  // For #行 directive
  char *显示_名字;
  int 行_德尔塔;
} 文件_大写;

// 牌 type
typedef struct 牌 牌;
struct 牌 {
  牌种类 种类;   // 牌 种类
  牌 *下一个;      // Next token
  int64_t 值_短;      // If 种类 is 牌_缩_数值字面, its value
  long double 浮点值_短; // If 种类 is 牌_缩_数值字面, its value
  char *位置_短;        // 牌 location
  int 长度_短;          // 牌 length
  类型 *类型_缩;         // Used if 牌_缩_数值字面 or 牌_缩_串字面
  char *串_短;        // String literal 内容 including terminating '\0'

  文件_大写 *源文件位置;       // Source location
  char *文件名;   // Filename
  int 行_号;      // Line number
  int 行_德尔塔;   // Line number
  bool 位置_行开始;      // True if this token is at beginning of 行
  bool 有_空格字符;   // True if this token follows a space character
  隐藏集 *隐藏集_小; // For macro expansion
  牌 *原创;    // If this is expanded from a macro, the original token
};

noreturn void 错误(char *fmt, ...) __attribute__((格式化(printf, 1, 2)));
noreturn void 错误_位置(char *位置_短, char *fmt, ...) __attribute__((格式化(printf, 2, 3)));
noreturn void 错误_牌_短(牌 *牌_短, char *fmt, ...) __attribute__((格式化(printf, 2, 3)));
void 警告_牌_短(牌 *牌_短, char *fmt, ...) __attribute__((格式化(printf, 2, 3)));
bool 等于(牌 *牌_短, char *op);
牌 *跳过(牌 *牌_短, char *op);
bool 消耗(牌 **rest, 牌 *牌_短, char *串_短);
void 转换_预处理_牌(牌 *牌_短);
文件_大写 **取_输入_文件(void);
文件_大写 *新_文件(char *名字, int 文件_号, char *内容);
牌 *化为牌_字符串_字面(牌 *牌_短, 类型 *基本类型);
牌 *化为牌(文件_大写 *源文件位置);
牌 *化为牌_文件(char *文件名);

#define 不可达() \
  错误("内部的 错误 at %s:%d", __FILE__, __LINE__)

//
// 预处理.c
//

char *搜索_包括_路径(char *文件名);
void 初始_宏(void);
void 定义_宏(char *名字, char *缓冲_短);
void 未定义_宏(char *名字);
牌 *预处理(牌 *牌_短);

//
// 解析.c
//

// Variable or 正函数_
typedef struct 对象_短 对象_短;
struct 对象_短 {
  对象_短 *下一个;
  char *名字;    // Variable 名字
  类型 *类型_缩;      // 类型
  牌 *牌_短;    // representative token
  bool 是否_本地; // local or global/正函数_
  int 对齐_短;     // alignment

  // Local variable
  int 偏移;

  // Global variable or 正函数_
  bool 是否_函数;
  bool 是否_定义了;
  bool 是否_静态;

  // Global variable
  bool 是否_试探性的;
  bool is_tls;
  char *初始_数据;
  重定位 *重定位_短;

  // Function
  bool 是否_内联;
  对象_短 *形参_短;
  节点 *体;
  对象_短 *本地;
  对象_短 *va_区域;
  对象_短 *动态分配_底部;
  int 栈_大小;

  // Static inline 正函数_
  bool 是否_活着;
  bool 是否_根;
  字符串数组 引用_短;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.
typedef struct 重定位 重定位;
struct 重定位 {
  重定位 *下一个;
  int 偏移;
  char **标号;
  long 附加物;
};

// AST 节点_小
typedef enum {
  抽象节点_空指针_表达式_短, // Do nothing
  抽象节点_加,       // +
  抽象节点_减,       // -
  抽象节点_乘,       // *
  抽象节点_除,       // /
  抽象节点_负,       // 一元_ -
  抽象节点_余数,       // %
  抽象节点_位与,    // &
  抽象节点_位或,     // |
  抽象节点_位异或,    // ^
  抽象节点_左移,       // <<
  抽象节点_右移,       // >>
  抽象节点_等于,        // ==
  抽象节点_不等,        // !=
  抽象节点_小于,        // <
  抽象节点_小等,        // <=
  抽象节点_赋值,    // =
  抽象节点_条件,      // ?:
  抽象节点_逗号,     // ,
  抽象节点_成员,    // . (struct 成员_小 access)
  抽象节点_地址,      // 一元_ &
  抽象节点_解引用,     // 一元_ *
  抽象节点_非,       // !
  抽象节点_位非,    // ~
  抽象节点_逻辑与,    // &&
  抽象节点_逻辑或,     // ||
  抽象节点_返回,    // "return"
  抽象节点_若,        // "if"
  抽象节点_为,       // "for" or "while"
  抽象节点_做,        // "do"
  抽象节点_切换,    // "switch"
  抽象节点_情况,      // "case"
  抽象节点_块,     // { ... }
  抽象节点_跳转,      // "goto"
  抽象节点_跳转_表达式, // "goto" 标号的-as-values
  抽象节点_标号,     // Labeled statement
  抽象节点_标号_值, // [GNU] Labels-as-values
  抽象节点_函数调用,   // Function call
  抽象节点_表达式_语句, // Expression statement
  抽象节点_语句_表达式, // Statement expression
  抽象节点_变量,       // Variable
  抽象节点_变长数组_指针,   // VLA designator
  抽象节点_整数,       // Integer
  抽象节点_类型转换,      // 类型 类型转换_
  抽象节点_内存清零,   // Zero-clear a 栈_小写 variable
  抽象节点_汇编,       // "asm"
  抽象节点_对比与交换,       // Atomic compare-and-swap
  抽象节点_互换,      // Atomic exchange
} 节点种类;

// AST 节点_小 type
struct 节点 {
  节点种类 种类; // 节点 种类
  节点 *下一个;    // Next 节点_小
  类型 *类型_缩;      // 类型, e.g. int or pointer to int
  牌 *牌_短;    // Representative token

  节点 *左手边;     // Left-hand side
  节点 *右手边;     // Right-hand side

  // "if" or "for" statement
  节点 *条件_短;
  节点 *那么;
  节点 *否则;
  节点 *初始;
  节点 *增_短;

  // "break" and "continue" 标号的
  char *断_标号;
  char *继续_标号;

  // Block or statement expression
  节点 *体;

  // Struct 成员_小 access
  成员 *成员_小;

  // Function call
  类型 *函数_类型;
  节点 *实参_短;
  bool 按栈传递;
  对象_短 *返回_缓冲区;

  // Goto or labeled statement, or 标号的-as-values
  char *标号;
  char *独特性_标号;
  节点 *跳转_下一个;

  // Switch
  节点 *情况_下一个;
  节点 *默认_情况;

  // Case
  long 开始;
  long 终;

  // "asm" string literal
  char *汇编_字符串;

  // Atomic compare-and-swap
  节点 *对比与交换_地址;
  节点 *对比与交换_旧;
  节点 *对比与交换_新;

  // Atomic op= operators
  对象_短 *原子_地址;
  节点 *原子_表达式;

  // Variable
  对象_短 *变量_短;

  // Numeric literal
  int64_t 值_短;
  long double 浮点值_短;
};

节点 *新_类型转换(节点 *表达式_短, 类型 *类型_缩);
int64_t 常量_表达式(牌 **rest, 牌 *牌_短);
对象_短 *解析(牌 *牌_短);

//
// type.c
//

typedef enum {
  类型_空的,
  类型_布尔,
  类型_字符,
  类型_短的,
  类型_整型,
  类型_长的,
  类型_浮点,
  类型_双浮,
  类型_长双浮,
  类型_枚举,
  类型_指针,
  类型_函数,
  类型_数组,
  类型_变长数组, // variable-length array
  类型_结构,
  类型_联合,
} 类型种类;

struct 类型 {
  类型种类 种类;
  int 大小;           // sizeof() value
  int 对齐_短;          // alignment
  bool 是否_无符号;   // unsigned or signed
  bool 是否_原子;     // true if _Atomic
  类型 *原创;       // for type compatibility check

  // Pointer-to or array-of type. We intentionally use the same 成员_小
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // 成员_小 instead of "种类" 成员_小 to determine whether a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  类型 *基础;

  // Declaration
  牌 *名字;
  牌 *名字_位置;

  // Array
  int 数组_长度;

  // Variable-length array
  节点 *变长数组_长度; // # of elements
  对象_短 *变长数组_大小; // sizeof() value

  // Struct
  成员 *成员_复数;
  bool 是否_有弹性;
  bool 是否_已打包;

  // Function type
  类型 *返回_类型;
  类型 *形参_短;
  bool 是否_可变参数;
  类型 *下一个;
};

// Struct 成员_小
struct 成员 {
  成员 *下一个;
  类型 *类型_缩;
  牌 *牌_短; // for 错误 message
  牌 *名字;
  int 索引_缩;
  int 对齐_短;
  int 偏移;

  // Bitfield
  bool 是否_位字段;
  int 位_偏移;
  int 位_宽度;
};

extern 类型 *类型_空的_小;
extern 类型 *类型_布尔_小;

extern 类型 *类型_字符_小;
extern 类型 *类型_短的_小;
extern 类型 *类型_整型_小;
extern 类型 *类型_长的_小;

extern 类型 *类型_无符号字符;
extern 类型 *类型_无符号短的;
extern 类型 *类型_无符号整型;
extern 类型 *类型_无符号长的;

extern 类型 *类型_浮点_小;
extern 类型 *类型_双浮_小;
extern 类型 *类型_长双浮_小;

bool 是否_整数_小(类型 *类型_缩);
bool 是否_浮点数_小(类型 *类型_缩);
bool 是否_数值型_小(类型 *类型_缩);
bool 是否_兼容_小(类型 *t1, 类型 *t2);
类型 *复制_类型(类型 *类型_缩);
类型 *指针_到(类型 *基础);
类型 *函数_类型_小(类型 *返回_类型);
类型 *数组_阵列(类型 *基础, int 大小);
类型 *变长数组_阵列(类型 *基础, 节点 *表达式_短);
类型 *枚举_类型_小(void);
类型 *结构_类型_小(void);
void 添加_类型_小(节点 *节点_小);

//
// 代码生成.c
//

void 代码生成(对象_短 *程序_短, FILE *out);
int 对齐_到(int n, int 对齐_短);

//
// unicode.c
//

int 编码_万国码8(char *缓冲_短, uint32_t c);
uint32_t 解码_万国码8(char **新_位置_短, char *p);
bool 是否_标识1(uint32_t c);
bool 是否_标识2(uint32_t c);
int 显示_宽度(char *p, int 长度_短);

//
// hashmap.c
//

typedef struct {
  char *键;
  int 键长度;
  void *值_短;
} 哈希入口;

typedef struct {
  哈希入口 *桶;
  int 容量;
  int 已用;
} 哈希映射;

void *哈希映射_取(哈希映射 *映射, char *键);
void *哈希映射_取2(哈希映射 *映射, char *键, int 键长度);
void 哈希映射_放置(哈希映射 *映射, char *键, void *值_短);
void 哈希映射_放置2(哈希映射 *映射, char *键, int 键长度, void *值_短);
void 哈希映射_删除(哈希映射 *映射, char *键);
void 哈希映射_删除2(哈希映射 *映射, char *键, int 键长度);
void 哈希映射_测试(void);

//
// main.c
//

bool 文件_存在(char *路径);

extern 字符串数组 包括_路径;
extern bool 选项_文件采摘;
extern bool 选项_文件公共的;
extern char *基础_文件;
