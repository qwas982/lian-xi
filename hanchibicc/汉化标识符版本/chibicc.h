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

#define 最大_大写(x, y) ((x) < (y) ? (y) : (x))
#define 最小_大写(x, y) ((x) < (y) ? (x) : (y))

#ifndef __GNUC__
# define __attribute__(x)
#endif

typedef struct 类型 类型;
typedef struct 节点 节点;
typedef struct 成员 成员;
typedef struct 重定位 重定位;
typedef struct 隐藏集 隐藏集;

//
// strings.c
//

typedef struct {
  char **数据_小写;
  int 容量;
  int 长度_短;
} 字符串数组;

void 串数组_推(字符串数组 *数组_短, char *s);
char *格式化_函(char *格式_缩写, ...) __attribute__((格式化_函(printf, 1, 2)));

//
// 化为牌_小写.c
//

// 牌
typedef enum {
  牌_标识,   // Identifiers
  牌_标点符号,   // Punctuators
  牌_关键字, // Keywords
  牌_串字面,     // String literals
  牌_数值字面,     // Numeric literals
  牌_预处理器_数目,  // Preprocessing numbers
  牌_文件终,     // End-of-文件_小写 markers
} 牌种类;

typedef struct {
  char *名称_小写;
  int 文件_数目;
  char *内容;

  // For #行号_小写 directive
  char *显示_名称;
  int 行号_德尔塔;
} 文件_大写;

// 牌 type
typedef struct 牌 牌;
struct 牌 {
  牌种类 种类_小写;   // 牌 种类_小写
  牌 *下一个_小写;      // Next token
  int64_t 值_小写_短;      // If 种类_小写 is 牌_数值字面, its value
  long double 浮点值_小写_短; // If 种类_小写 is 牌_数值字面, its value
  char *定位_小写_短;        // 牌 location
  int 长度_短;          // 牌 length
  类型 *类型_小写_短;         // Used if 牌_数值字面 or 牌_串字面
  char *串_小写_短;        // String literal 内容 including terminating '\0'

  文件_大写 *文件_小写;       // Source location
  char *文件名_小写;   // Filename
  int 行号_数目;      // Line number
  int 行号_德尔塔;   // Line number
  bool 在_行开头;      // True if this token is at beginning of 行号_小写
  bool 有_空格;   // True if this token follows a space character
  隐藏集 *隐藏集_小写; // For macro expansion
  牌 *起源;    // If this is expanded from a macro, the original token
};

noreturn void 错误_小写(char *格式_缩写, ...) __attribute__((格式化_函(printf, 1, 2)));
noreturn void 错误_在_小写(char *定位_小写_短, char *格式_缩写, ...) __attribute__((格式化_函(printf, 2, 3)));
noreturn void 错误_牌_小写(牌 *牌_短_小写, char *格式_缩写, ...) __attribute__((格式化_函(printf, 2, 3)));
void 警告_牌_短(牌 *牌_短_小写, char *格式_缩写, ...) __attribute__((格式化_函(printf, 2, 3)));
bool 相等吗_小写(牌 *牌_短_小写, char *op);
牌 *跳过_小写(牌 *牌_短_小写, char *op);
bool 消耗吗_小写(牌 **其余的, 牌 *牌_短_小写, char *串_小写_短);
void 转换_预处理器_牌们(牌 *牌_短_小写);
文件_大写 **获取_输入_文件们(void);
文件_大写 *新_文件(char *名称_小写, int 文件_数目, char *内容);
牌 *化为牌_字符串_字面(牌 *牌_短_小写, 类型 *基本类型_短);
牌 *化为牌_小写(文件_大写 *文件_小写);
牌 *化为牌_文件(char *文件名_小写);

#define 无法到达() \
  错误_小写("internal 错误_小写 at %s:%d", __FILE__, __LINE__)

//
// 预处理_小写.c
//

char *搜索_包含_路径们(char *文件名_小写);
void 初始_宏们(void);
void 定义_宏(char *名称_小写, char *buf);
void 未定义_宏(char *名称_小写);
牌 *预处理_小写(牌 *牌_短_小写);

//
// 解析_小写.c
//

// Variable or 函数_全_小写
typedef struct 对象_缩_大写 对象_缩_大写;
struct 对象_缩_大写 {
  对象_缩_大写 *下一个_小写;
  char *名称_小写;    // Variable 名称_小写
  类型 *类型_小写_短;      // 类型
  牌 *牌_短_小写;    // representative token
  bool 是否_本地; // local or global/函数_全_小写
  int 对齐_短_小写;     // alignment

  // Local variable
  int 偏移_小写;

  // Global variable or 函数_全_小写
  bool 是否_函数;
  bool 是否_定义;
  bool 是否_静态;

  // Global variable
  bool 是否_试探性;
  bool 是否_传输层安全;
  char *初始_数据;
  重定位 *重定位_缩;

  // Function
  bool 是否_内联;
  对象_缩_大写 *形参们_短_小写;
  节点 *函数体_小写;
  对象_缩_大写 *本地们_小写;
  对象_缩_大写 *变量_缩_区域;
  对象_缩_大写 *动态分配_底部;
  int 栈_大小;

  // Static inline 函数_全_小写
  bool 是否_存活;
  bool 是否_根部;
  字符串数组 引用们_缩_小写;
};

// Global variable can be initialized either by a constant expression
// or a pointer to another global variable. This struct represents the
// latter.
typedef struct 重定位 重定位;
struct 重定位 {
  重定位 *下一个_小写;
  int 偏移_小写;
  char **标号_小写;
  long 加终_小写_短;
};

// AST 节点_小写
typedef enum {
  节点_空指针_表达式, // Do nothing
  节点_加法,       // +
  节点_减法,       // -
  节点_乘法,       // *
  节点_除法,       // /
  节点_负号,       // 一元运算_小写 -
  节点_取模,       // %
  节点_位与,    // &
  节点_位或,     // |
  节点_位异或,    // ^
  节点_左移,       // <<
  节点_右移,       // >>
  节点_等于,        // ==
  节点_不等,        // !=
  节点_小于,        // <
  节点_小等,        // <=
  节点_赋值,    // =
  节点_条件,      // ?:
  节点_逗号,     // ,
  节点_成员,    // . (struct 成员_小写 access)
  节点_地址,      // 一元运算_小写 &
  节点_解引用,     // 一元运算_小写 *
  节点_非,       // !
  节点_位非,    // ~
  节点_逻辑与,    // &&
  节点_逻辑或,     // ||
  节点_返回,    // "return"
  节点_若,        // "if"
  节点_为,       // "for" or "while"
  节点_做,        // "do"
  节点_切换,    // "switch"
  节点_情况,      // "case"
  节点_块,     // { ... }
  节点_去到,      // "goto"
  节点_去到_表达式, // "goto" 标号们_小写-as-values
  节点_标号,     // Labeled statement
  节点_标号_值, // [GNU] Labels-as-values
  节点_函调,   // Function call
  节点_表达式_语句, // Expression statement
  节点_语句_表达式, // Statement expression
  节点_变量,       // Variable
  节点_变长数组_指针,   // VLA designator
  节点_数,       // Integer
  节点_类转,      // 类型 类型转换_小写
  节点_内存归零,   // Zero-clear a stack variable
  节点_汇编,       // "asm"
  节点_对比与交换,       // Atomic compare-and-swap
  节点_调换,      // Atomic exchange
} 节点种类;

// AST 节点_小写 type
struct 节点 {
  节点种类 种类_小写; // 节点 种类_小写
  节点 *下一个_小写;    // Next 节点_小写
  类型 *类型_小写_短;      // 类型, e.g. int or pointer to int
  牌 *牌_短_小写;    // Representative token

  节点 *左手塞_缩;     // Left-hand side
  节点 *右手塞_缩;     // Right-hand side

  // "if" or "for" statement
  节点 *条件_小写_短;
  节点 *那么_小写;
  节点 *否则_小写_短;
  节点 *初始_小写_短;
  节点 *递增_小写_短;

  // "break" and "continue" 标号们_小写
  char *破坏_标号_缩;
  char *继续_标号_缩;

  // Block or statement expression
  节点 *函数体_小写;

  // Struct 成员_小写 access
  成员 *成员_小写;

  // Function call
  类型 *函_类型_缩;
  节点 *实参们_短;
  bool 通过栈传递;
  对象_缩_大写 *返回_缓冲区;

  // Goto or labeled statement, or 标号们_小写-as-values
  char *标号_小写;
  char *唯一_标号;
  节点 *去到_下一个;

  // Switch
  节点 *情况_下一个;
  节点 *默认_情况;

  // Case
  long 开头_小写;
  long 终_小写;

  // "asm" string literal
  char *汇编_串_短;

  // Atomic compare-and-swap
  节点 *对比与交换_地址;
  节点 *对比与交换_旧;
  节点 *对比与交换_新;

  // Atomic op= operators
  对象_缩_大写 *原子_地址;
  节点 *原子_表达式;

  // Variable
  对象_缩_大写 *变量_短;

  // Numeric literal
  int64_t 值_小写_短;
  long double 浮点值_小写_短;
};

节点 *新_转换(节点 *表达式_短_小写, 类型 *类型_小写_短);
int64_t 常量_表达式_短(牌 **其余的, 牌 *牌_短_小写);
对象_缩_大写 *解析_小写(牌 *牌_短_小写);

//
// type.c
//

typedef enum {
  类型_空的,
  类型_布尔,
  类型_印刻,
  类型_短的,
  类型_整型,
  类型_长的,
  类型_浮点,
  类型_双浮,
  类型_长双浮,
  类型_枚举,
  类型_指针,
  类型_函,
  类型_数组,
  类型_变长数组, // variable-length array
  类型_结构,
  类型_联合,
} 类型种类;

struct 类型 {
  类型种类 种类_小写;
  int 大小_小写;           // sizeof() value
  int 对齐_短_小写;          // alignment
  bool 是否_无符号;   // unsigned or signed
  bool 是否_原子;     // true if _Atomic
  类型 *起源;       // for type compatibility check

  // Pointer-to or array-of type. We intentionally use the same 成员_小写
  // to represent pointer/array duality in C.
  //
  // In many contexts in which a pointer is expected, we examine this
  // 成员_小写 instead of "种类_小写" 成员_小写 to determine whether a type is a
  // pointer or not. That means in many contexts "array of T" is
  // naturally handled as if it were "pointer to T", as required by
  // the C spec.
  类型 *基础_小写;

  // Declaration
  牌 *名称_小写;
  牌 *名称_位置;

  // Array
  int 数组_长度;

  // Variable-length array
  节点 *变长数组_长度; // # of elements
  对象_缩_大写 *变长数组_大小; // sizeof() value

  // Struct
  成员 *成员们_小写;
  bool 是否_弹性;
  bool 是否_已打包;

  // Function type
  类型 *返回_类型;
  类型 *形参们_短_小写;
  bool 是否_可变参数;
  类型 *下一个_小写;
};

// Struct 成员_小写
struct 成员 {
  成员 *下一个_小写;
  类型 *类型_小写_短;
  牌 *牌_短_小写; // for 错误_小写 message
  牌 *名称_小写;
  int 索引_缩;
  int 对齐_短_小写;
  int 偏移_小写;

  // Bitfield
  bool 是否_位字段;
  int 位_偏移;
  int 位_宽度;
};

extern 类型 *类型_空的_小写;
extern 类型 *类型_布尔_小写;

extern 类型 *类型_印刻_小写;
extern 类型 *类型_短的_小写;
extern 类型 *类型_整型_小写;
extern 类型 *类型_长的_小写;

extern 类型 *类型_无符印刻_小写;
extern 类型 *类型_无符短的_小写;
extern 类型 *类型_无符整型_小写;
extern 类型 *类型_无符长的_小写;

extern 类型 *类型_浮点_小写;
extern 类型 *类型_双浮_小写;
extern 类型 *类型_长双浮_小写;

bool 是否_整数(类型 *类型_小写_短);
bool 是否_浮点数目(类型 *类型_小写_短);
bool 是否_数值(类型 *类型_小写_短);
bool 是否_兼容(类型 *t1, 类型 *t2);
类型 *复制_类型(类型 *类型_小写_短);
类型 *指针_到(类型 *基础_小写);
类型 *函_类型_小写(类型 *返回_类型);
类型 *数组_的(类型 *基础_小写, int 大小_小写);
类型 *变长数组_的(类型 *基础_小写, 节点 *表达式_短_小写);
类型 *枚举_类型(void);
类型 *结构_类型(void);
void 加_类型(节点 *节点_小写);

//
// 代码生成_短.c
//

void 代码生成_短(对象_缩_大写 *prog, FILE *out);
int 对齐_到(int n, int 对齐_短_小写);

//
// unicode.c
//

int 编码_万国码变形格式8(char *buf, uint32_t c);
uint32_t 解码_万国码变形格式8(char **new_pos, char *p);
bool 是否_标识1(uint32_t c);
bool 是否_标识2(uint32_t c);
int 显示_宽度(char *p, int 长度_短);

//
// hashmap.c
//

typedef struct {
  char *键_小写;
  int 键长度_小写;
  void *值_小写_短;
} 哈希条目_驼峰名;

typedef struct {
  哈希条目_驼峰名 *桶们_小写;
  int 容量;
  int 已用_小写;
} 哈希映射_驼峰名;

void *哈希映射_获取(哈希映射_驼峰名 *映射_小写, char *键_小写);
void *哈希映射_获取2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写);
void 哈希映射_放置(哈希映射_驼峰名 *映射_小写, char *键_小写, void *值_小写_短);
void 哈希映射_放置2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写, void *值_小写_短);
void 哈希映射_删除(哈希映射_驼峰名 *映射_小写, char *键_小写);
void 哈希映射_删除2(哈希映射_驼峰名 *映射_小写, char *键_小写, int 键长度_小写);
void 哈希映射_测试(void);

//
// main.c
//

bool 文件_存在们(char *path);

extern 字符串数组 包含_路径们_小写;
extern bool 优化_标志位置独立代码;
extern bool 优化_标志共同;
extern char *基本_文件;
