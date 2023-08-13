#define _POSIX_C_SOURCE 200809L  // 宏定义：指定POSIX C标准版本为200809L

#include <assert.h>  // 断言
#include <ctype.h>  // 字符处理
#include <errno.h>  // 错误码
#include <glob.h>  // 文件模式匹配
#include <libgen.h>  // 路径操作
#include <stdarg.h>  // 可变参数
#include <stdbool.h>  // 布尔类型
#include <stdint.h>  // 整数类型
#include <stdio.h>  // 标准输入输出
#include <stdlib.h>  // 标准库函数
#include <stdnoreturn.h>  // 不返回函数
#include <string.h>  // 字符串操作
#include <strings.h>  // 字符串操作（忽略大小写）
#include <sys/stat.h>  // 文件状态
#include <sys/types.h>  // 系统数据类型
#include <sys/wait.h>  // 进程等待
#include <time.h>  // 时间和日期
#include <unistd.h>  // 标准符号常量和类型

#define MAX(x, y) ((x) < (y) ? (y) : (x))  // 宏定义：取x和y中的较大值
#define MIN(x, y) ((x) < (y) ? (x) : (y))  // 宏定义：取x和y中的较小值

#ifndef __GNUC__
# define __attribute__(x)  // 如果不是GNU编译器，则将__attribute__宏定义为空
#endif


typedef struct Type Type;  // 类型
typedef struct Node Node;  // 节点
typedef struct Member Member;  // 成员
typedef struct Relocation Relocation;  // 重定位
typedef struct Hideset Hideset;  // 隐藏集

//
// strings.c
//

typedef struct {
  char **data;  // 数据
  int capacity;  // 容量
  int len;  // 长度
} StringArray;  // 字符串数组

void strarray_push(StringArray *arr, char *s);  // 将字符串推入数组
char *format(char *fmt, ...) __attribute__((format(printf, 1, 2)));  // 格式化字符串

//
// tokenize.c
//

// Token
typedef enum {
  TK_IDENT,   // 标识符
  TK_PUNCT,   // 标点符号
  TK_KEYWORD, // 关键字
  TK_STR,     // 字符串字面量
  TK_NUM,     // 数字字面量
  TK_PP_NUM,  // 预处理数字
  TK_EOF,     // 文件结束标记
} TokenKind;  // Token类型

typedef struct {
  char *name;  // 名称
  int file_no;  // 文件编号
  char *contents;  // 内容

  // For #line directive
  char *display_name;  // 显示名称
  int line_delta;  // 行数增量
} File;  // 文件

// Token类型
typedef struct Token Token;
struct Token {
  TokenKind kind;  // Token类型
  Token *next;  // 下一个Token
  int64_t val;  // 如果kind是TK_NUM，则为其值
  long double fval;  // 如果kind是TK_NUM，则为其值
  char *loc;  // Token位置
  int len;  // Token长度
  Type *ty;  // 如果kind是TK_NUM或TK_STR，则为其类型
  char *str;  // 字符串字面量内容，包括终止的'\0'

  File *file;  // 源位置
  char *filename;  // 文件名
  int line_no;  // 行号
  int line_delta;  // 行号增量
  bool at_bol;  // 如果该Token位于行首，则为True
  bool has_space;  // 如果该Token之前有空格字符，则为True
  Hideset *hideset;  // 用于宏展开
  Token *origin;  // 如果该Token是宏展开的结果，则为原始Token
};

noreturn void error(char *fmt, ...) __attribute__((format(printf, 1, 2)));  // 报告错误并退出
noreturn void error_at(char *loc, char *fmt, ...) __attribute__((format(printf, 2, 3)));  // 报告指定位置的错误并退出
noreturn void error_tok(Token *tok, char *fmt, ...) __attribute__((format(printf, 2, 3)));  // 报告指定Token的错误并退出
void warn_tok(Token *tok, char *fmt, ...) __attribute__((format(printf, 2, 3)));  // 报告指定Token的警告
bool equal(Token *tok, char *op);  // 判断Token是否与指定操作符相等
Token *skip(Token *tok, char *op);  // 跳过指定操作符，返回下一个Token
bool consume(Token **rest, Token *tok, char *str);  // 如果Token与指定字符串相等，则将下一个Token赋给rest，并返回True
void convert_pp_tokens(Token *tok);  // 转换预处理Token
File **get_input_files(void);  // 获取输入文件列表
File *new_file(char *name, int file_no, char *contents);  // 创建新文件
Token *tokenize_string_literal(Token *tok, Type *basety);  // 对字符串字面量进行Tokenize
Token *tokenize(File *file);  // 对文件进行Tokenize
Token *tokenize_file(char *filename);  // 对指定文件进行Tokenize

#define unreachable() \
  error("在文件 %s 的第 %d 行发生内部错误", __FILE__, __LINE__)


//
// preprocess.c
//

char *search_include_paths(char *filename);  // 搜索包含文件的路径
void init_macros(void);  // 初始化宏
void define_macro(char *name, char *buf);  // 定义宏
void undef_macro(char *name);  // 取消定义宏
Token *preprocess(Token *tok);  // 预处理Token

//
// parse.c
//

// 变量或函数
typedef struct Obj Obj;  // 对象
struct Obj {
  Obj *next;  // 下一个对象
  char *name;  // 变量名
  Type *ty;  // 类型
  Token *tok;  // 代表性Token
  bool is_local;  // 局部变量或全局/函数变量
  int align;  // 对齐方式

  // 局部变量
  int offset;

  // 全局变量或函数
  bool is_function;  // 是否为函数
  bool is_definition;  // 是否为定义
  bool is_static;  // 是否为静态变量或函数

  // 全局变量
  bool is_tentative;  // 是否为不完全定义
  bool is_tls;  // 是否为线程局部存储

  char *init_data;  // 初始化数据
  Relocation *rel;  // 重定位

  // 函数
  bool is_inline;  // 是否为内联函数
  Obj *params;  // 参数列表
  Node *body;  // 函数体
  Obj *locals;  // 局部变量
  Obj *va_area;  // 可变参数区域
  Obj *alloca_bottom;  // alloca的底部
  int stack_size;  // 栈大小

  // 静态内联函数
  bool is_live;  // 是否为活跃函数
  bool is_root;  // 是否为根函数
  StringArray refs;  // 引用列表
};

// 全局变量可以通过常量表达式或指向另一个全局变量的指针进行初始化。该结构体表示后者。
typedef struct Relocation Relocation;  // 重定位
struct Relocation {
  Relocation *next;  // 下一个重定位
  int offset;  // 偏移量
  char **label;  // 标签
  long addend;  // 加数
};

// AST节点
typedef enum {
  ND_NULL_EXPR, // 无操作
  ND_ADD,       // +
  ND_SUB,       // -
  ND_MUL,       // *
  ND_DIV,       // /
  ND_NEG,       // 一元负号
  ND_MOD,       // %
  ND_BITAND,    // &
  ND_BITOR,     // |
  ND_BITXOR,    // ^
  ND_SHL,       // <<
  ND_SHR,       // >>
  ND_EQ,        // ==
  ND_NE,        // !=
  ND_LT,        // <
  ND_LE,        // <=
  ND_ASSIGN,    // =
  ND_COND,      // 三元运算符
  ND_COMMA,     // ,
  ND_MEMBER,    // . (结构体成员访问)
  ND_ADDR,      // 一元&
  ND_DEREF,     // 一元*
  ND_NOT,       // !
  ND_BITNOT,    // ~
  ND_LOGAND,    // &&
  ND_LOGOR,     // ||
  ND_RETURN,    // "return"
  ND_IF,        // "if"
  ND_FOR,       // "for" 或 "while"
  ND_DO,        // "do"
  ND_SWITCH,    // "switch"
  ND_CASE,      // "case"
  ND_BLOCK,     // { ... }
  ND_GOTO,      // "goto"
  ND_GOTO_EXPR, // "goto" 标签作为值
  ND_LABEL,     // 标记语句
  ND_LABEL_VAL, // [GNU] 标签作为值
  ND_FUNCALL,   // 函数调用
  ND_EXPR_STMT, // 表达式语句
  ND_STMT_EXPR, // 语句表达式
  ND_VAR,       // 变量
  ND_VLA_PTR,   // 可变长度数组指针
  ND_NUM,       // 整数
  ND_CAST,      // 类型转换
  ND_MEMZERO,   // 清零栈变量
  ND_ASM,       // "asm"
  ND_CAS,       // 原子比较并交换
  ND_EXCH,      // 原子交换
} NodeKind;

// AST节点类型
struct Node {
  NodeKind kind; // 节点类型
  Node *next;    // 下一个节点
  Type *ty;      // 类型，例如int或指向int的指针
  Token *tok;    // 代表性的标记

  Node *lhs;     // 左操作数
  Node *rhs;     // 右操作数

  // "if"或"for"语句
  Node *cond;       // 条件表达式
  Node *then;       // if语句的主体
  Node *els;        // else语句的主体
  Node *init;       // for循环的初始化语句
  Node *inc;        // for循环的增量语句

  // "break"和"continue"标签
  char *brk_label;  // break标签
  char *cont_label; // continue标签

  // 块或语句表达式
  Node *body;       // 块或语句表达式的主体

  // 结构体成员访问
  Member *member;   // 结构体成员访问

  // 函数调用
  Type *func_ty;    // 函数类型
  Node *args;       // 函数调用的参数
  bool pass_by_stack; // 是否通过栈传递参数
  Obj *ret_buffer;  // 返回值缓冲区对象


  // Goto语句、带标签的语句或标签作为值
  char *label;  // 标签指针
  char *unique_label;  // 唯一标签指针
  Node *goto_next;  // 下一个Goto节点指针

  // Switch语句
  Node *case_next;  // 下一个Case节点指针
  Node *default_case;  // 默认Case节点指针

  // Case语句
  long begin;  // 开始位置
  long end;  // 结束位置

  // "asm"字符串字面量
  char *asm_str;  // "asm"字符串字面量指针

  // 原子比较并交换
  Node *cas_addr;  // 比较交换地址指针
  Node *cas_old;  // 比较值指针
  Node *cas_new;  // 新值指针

  // 原子op=运算符
  Obj *atomic_addr;  // 原子地址指针
  Node *atomic_expr;  // 原子表达式指针

  // 变量
  Obj *var;  // 变量指针

  // 数字字面量
  int64_t val;  // 64位整数值
  long double fval;  // 长双精度浮点数值
};

Node *new_cast(Node *expr, Type *ty);  // 创建一个类型转换节点，参数为表达式节点和目标类型指针，返回新的类型转换节点指针
int64_t const_expr(Token **rest, Token *tok);  // 解析常量表达式，参数为指向当前Token指针的指针和当前Token指针，返回解析得到的64位整数值
Obj *parse(Token *tok);  // 解析C代码，参数为当前Token指针，返回解析得到的对象指针

//
// type.c
//

typedef enum {
  TY_VOID,  // void类型
  TY_BOOL,  // bool类型
  TY_CHAR,  // char类型
  TY_SHORT,  // short类型
  TY_INT,  // int类型
  TY_LONG,  // long类型
  TY_FLOAT,  // float类型
  TY_DOUBLE,  // double类型
  TY_LDOUBLE,  // long double类型
  TY_ENUM,  // 枚举类型
  TY_PTR,  // 指针类型
  TY_FUNC,  // 函数类型
  TY_ARRAY,  // 数组类型
  TY_VLA,  // 可变长度数组类型
  TY_STRUCT,  // 结构体类型
  TY_UNION,  // 联合体类型
} TypeKind;  // 类型种类枚举

struct Type {
  TypeKind kind;  // 类型种类
  int size;  // sizeof()值
  int align;  // 对齐方式
  bool is_unsigned;  // 是否为无符号类型
  bool is_atomic;  // 是否为_Atomic类型
  Type *origin;  // 用于类型兼容性检查

  // 指针类型或数组类型。我们故意使用相同的成员来表示C中指针/数组的二义性。
  //
  // 在许多期望指针的上下文中，我们检查这个成员而不是"kind"成员来确定一个类型是否为指针。这意味着在许多上下文中，"数组 of T"自然地被处理为"pointer to T"，正如C规范所要求的那样。
  Type *base;  // 基类型

  // 声明
  Token *name;  // 类型名称
  Token *name_pos;  // 类型名称位置

  // 数组
  int array_len;  // 数组长度

  // 可变长度数组
  Node *vla_len;  // 元素个数
  Obj *vla_size;  // sizeof()值

  // 结构体
  Member *members;  // 成员列表
  bool is_flexible;  // 是否为柔性数组
  bool is_packed;  // 是否为紧凑结构体

  // 函数类型
  Type *return_ty;  // 返回类型
  Type *params;  // 参数列表
  bool is_variadic;  // 是否为可变参数函数
  Type *next;  // 下一个函数类型
};

// 结构体成员
struct Member {
  Member *next;  // 下一个成员指针
  Type *ty;  // 成员类型指针
  Token *tok;  // 用于错误消息的Token指针
  Token *name;  // 成员名称Token指针
  int idx;  // 成员索引
  int align;  // 对齐方式
  int offset;  // 偏移量

  // 位域 字段
  bool is_bitfield;  // 是否为位域 字段
  int bit_offset;  // 位域偏移量
  int bit_width;  // 位域宽度
};

extern Type *ty_void;  // void类型指针
extern Type *ty_bool;  // bool类型指针

extern Type *ty_char;  // char类型指针
extern Type *ty_short;  // short类型指针
extern Type *ty_int;  // int类型指针
extern Type *ty_long;  // long类型指针

extern Type *ty_uchar;  // unsigned char类型指针
extern Type *ty_ushort;  // unsigned short类型指针
extern Type *ty_uint;  // unsigned int类型指针
extern Type *ty_ulong;  // unsigned long类型指针

extern Type *ty_float;  // float类型指针
extern Type *ty_double;  // double类型指针
extern Type *ty_ldouble;  // long double类型指针

bool is_integer(Type *ty);  // 判断类型是否为整数类型
bool is_flonum(Type *ty);  // 判断类型是否为浮点数类型
bool is_numeric(Type *ty);  // 判断类型是否为数字类型
bool is_compatible(Type *t1, Type *t2);  // 判断两个类型是否兼容
Type *copy_type(Type *ty);  // 复制类型
Type *pointer_to(Type *base);  // 创建指向指定基类型的指针类型
Type *func_type(Type *return_ty);  // 创建函数类型
Type *array_of(Type *base, int size);  // 创建指定基类型和大小的数组类型
Type *vla_of(Type *base, Node *expr);  // 创建指定基类型和可变长度的数组类型
Type *enum_type(void);  // 创建枚举类型
Type *struct_type(void);  // 创建结构体类型
void add_type(Node *node);  // 添加类型到类型列表

//
// codegen.c
//

void codegen(Obj *prog, FILE *out);  // 生成目标代码
int align_to(int n, int align);  // 对齐到指定的边界

//
// unicode.c
//

int encode_utf8(char *buf, uint32_t c);  // 将Unicode字符编码为UTF-8
uint32_t decode_utf8(char **new_pos, char *p);  // 将UTF-8编码解码为Unicode字符
bool is_ident1(uint32_t c);  // 判断字符是否为合法的标识符首字符
bool is_ident2(uint32_t c);  // 判断字符是否为合法的标识符非首字符
int display_width(char *p, int len);  // 计算字符串在显示上的宽度

//
// hashmap.c
//

typedef struct {
  char *key;  // 键
  int keylen;  // 键长度
  void *val;  // 值
} HashEntry;  // 哈希表条目

typedef struct {
  HashEntry *buckets;  // 桶数组
  int capacity;  // 容量
  int used;  // 已使用的数量
} HashMap;  // 哈希表

void *hashmap_get(HashMap *map, char *key);  // 获取哈希表中指定键的值
void *hashmap_get2(HashMap *map, char *key, int keylen);  // 获取哈希表中指定键和键长度的值
void hashmap_put(HashMap *map, char *key, void *val);  // 向哈希表中插入指定键值对
void hashmap_put2(HashMap *map, char *key, int keylen, void *val);  // 向哈希表中插入指定键、键长度和值的键值对
void hashmap_delete(HashMap *map, char *key);  // 从哈希表中删除指定键的键值对
void hashmap_delete2(HashMap *map, char *key, int keylen);  // 从哈希表中删除指定键和键长度的键值对
void hashmap_test(void);  // 哈希表测试函数

//
// main.c
//

bool file_exists(char *path);  // 判断文件是否存在

extern StringArray include_paths;  // 包含路径数组
extern bool opt_fpic;  // -fpic选项
extern bool opt_fcommon;  // -fcommon选项
extern char *base_file;  // 基准文件路径
