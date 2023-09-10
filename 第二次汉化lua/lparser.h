/*
** $Id: lparser.h $
** Lua Parser
** See Copyright Notice in lua.h
*/

#ifndef lparser_h
#define lparser_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/*
** Expression and variable descriptor.
** Code generation for variables and expressions can be delayed 到_变量 allow
** optimizations; An '表达式描述_结' structure describes a potentially-delayed
** variable/expression. It has a description of its "main" 值_圆 plus a
** 列表_变量 of conditional jumps that can also produce its 值_圆 (generated
** by short-circuit operators 'and'/'or').
*/

/* kinds of variables/expressions */
typedef enum {
  虚空的_种类,  /* when '表达式描述_结' describes the 最后_变量 expression of a 列表_变量,
             this 种类_变量 means an 空容器_变量 列表_变量 (so, no expression) */
  虚空值_种类,  /* constant nil */
  虚真_种类,  /* constant true */
  虚假_种类,  /* constant false */
  虚常_种类,  /* constant in 'k'; 信息_短变量 = index of constant in 'k' */
  虚常浮点_种类,  /* floating constant; n值_缩 = numerical float 值_圆 */
  虚常整型_种类,  /* integer constant; i值_缩 = numerical integer 值_圆 */
  虚常串_种类,  /* string constant; 串值_短 = 标签字符串_结 address;
             (string is fixed by the lexer) */
  虚没有重定位_种类,  /* expression has its 值_圆 in a fixed register;
                 信息_短变量 = 结果_变量 register */
  虚本地_种类,  /* local variable; 变_短变量.寄索引_缩短 = register index;
              变_短变量.变索引_变量 = relative index in '活动变量_结.arr'  */
  虚上值_种类,  /* 上值_圆 variable; 信息_短变量 = index of 上值_圆 in '上值们_小写' */
  虚常字面_种类,  /* compile-time <const> variable;
              信息_短变量 = absolute index in '活动变量_结.arr'  */
  虚已索引_种类,  /* indexed variable;
                ind.t = table register;
                ind.索引_缩变量 = 键_小变量's R index */
  虚索引上值_种类,  /* indexed 上值_圆;
                ind.t = table 上值_圆;
                ind.索引_缩变量 = 键_小变量's K index */
  虚索引整_种类, /* indexed variable with constant integer;
                ind.t = table register;
                ind.索引_缩变量 = 键_小变量's 值_圆 */
  虚索引串_种类, /* indexed variable with literal string;
                ind.t = table register;
                ind.索引_缩变量 = 键_小变量's K index */
  虚跳转_种类,  /* expression is a test/comparison;
            信息_短变量 = 程序计数_变量 of corresponding jump instruction */
  虚重定位_种类,  /* expression can put 结果_变量 in any register;
              信息_短变量 = instruction 程序计数_变量 */
  虚调用_种类,  /* expression is a function call; 信息_短变量 = instruction 程序计数_变量 */
  虚变量实参_种类  /* vararg expression; 信息_短变量 = instruction 程序计数_变量 */
} 表达式种类_枚举;


#define 解析器_变量种类是否变量_宏名(k)	(虚本地_种类 <= (k) && (k) <= 虚索引串_种类)
#define 解析器_变量种类是否已索引_宏名(k)	(虚已索引_种类 <= (k) && (k) <= 虚索引串_种类)


typedef struct 表达式描述_结 {
  表达式种类_枚举 k;
  union {
    炉_整数型 i值_缩;    /* for 虚常整型_种类 */
    炉_数目型 n值_缩;  /* for 虚常浮点_种类 */
    标签字符串_结 *串值_短;  /* for 虚常串_种类 */
    int 信息_短变量;  /* for generic use */
    struct {  /* for indexed variables */
      short 索引_缩变量;  /* index (R or "long" K) */
      路_字节型 t;  /* table (register or 上值_圆) */
    } ind;
    struct {  /* for local variables */
      路_字节型 寄索引_缩短;  /* register holding the variable */
      unsigned short 变索引_变量;  /* compiler index (in '活动变量_结.arr')  */
    } 变_短变量;
  } u;
  int t;  /* patch 列表_变量 of '出口_变量 when true' */
  int f;  /* patch 列表_变量 of '出口_变量 when false' */
} 表达式描述_结;


/* kinds of variables */
#define 解析器_变量D种类寄存器_宏名		0   /* regular */
#define 解析器_寄存器D种类常量_宏名	1   /* constant */
#define 解析器_寄存器D种类待关闭_宏名	2   /* 到_变量-be-closed */
#define 解析器_寄存器D种类代码到常量_宏名		3   /* compile-time constant */

/* description of an active local variable */
typedef union 变量描述_联 {
  struct {
    标签值字段;  /* constant 值_圆 (if it is a compile-time constant) */
    路_字节型 种类_变量;
    路_字节型 寄索引_缩短;  /* register holding the variable */
    short 原型索引_缩短;  /* index of the variable in the 原型_结's '本地变量们_短' 数组_圆 */
    标签字符串_结 *名称_变量;  /* variable 名称_变量 */
  } 有效数据_变量;
  标签值_结 k;  /* constant 值_圆 (if any) */
} 变量描述_联;



/* description of pending goto statements and 标号_圆 statements */
typedef struct 标号描述_结 {
  标签字符串_结 *名称_变量;  /* 标号_圆 identifier */
  int 程序计数_变量;  /* position in 代码_变量 */
  int 行_变量;  /* 行_变量 哪儿_变量 it appeared */
  路_字节型 活动变量数_缩;  /* number of active variables in that position */
  路_字节型 关闭_圆;  /* goto that escapes 上值们_小写 */
} 标号描述_结;


/* 列表_变量 of labels or gotos */
typedef struct 标号列表_结 {
  标号描述_结 *arr;  /* 数组_圆 */
  int n;  /* number of entries in use */
  int 大小_变量;  /* 数组_圆 大小_变量 */
} 标号列表_结;


/* dynamic structures used by the parser */
typedef struct 动态数据_结 {
  struct {  /* 列表_变量 of all active local variables */
    变量描述_联 *arr;
    int n;
    int 大小_变量;
  } 活动变量_结;
  标号列表_结 全局表_缩变量;  /* 列表_变量 of pending gotos */
  标号列表_结 标号_圆;   /* 列表_变量 of active labels */
} 动态数据_结;


/* control of blocks */
struct 块继续_结;  /* defined in lparser.c */


/* 状态机_变量 已需要_变量 到_变量 generate 代码_变量 for a given function */
typedef struct 函状态机_结 {
  原型_结 *f;  /* 当前_圆 function header */
  struct 函状态机_结 *前一_短变量;  /* enclosing function */
  struct 词法状态机_结 *状列_缩变量;  /* lexical 状态机_变量 */
  struct 块继续_结 *基本逻辑_变量;  /* chain of 当前_圆 blocks */
  int 程序计数_变量;  /* 下一个_变量 position 到_变量 代码_变量 (equivalent 到_变量 'ncode') */
  int 最后目标_圆;   /* '标号_圆' of 最后_变量 'jump 标号_圆' */
  int 前个行_圆;  /* 最后_变量 行_变量 that was saved in '行信息_变量' */
  int nk;  /* number of elements in 'k' */
  int np;  /* number of elements in 'p' */
  int 绝对行信息数_;  /* number of elements in '绝对行信息_小写' */
  int 首个本地_圆;  /* index of 首先_变量 local 变_短变量 (in 动态数据_结 数组_圆) */
  int 首个标号_圆;  /* index of 首先_变量 标号_圆 (in '定你数_缩变量->标号_圆->arr') */
  short 调试变量数_缩;  /* number of elements in 'f->本地变量们_短' */
  路_字节型 活动变量数_缩;  /* number of active local variables */
  路_字节型 上值数_短缩;  /* number of 上值们_小写 */
  路_字节型 是否寄存_短;  /* 首先_变量 free register */
  路_字节型 指令与绝对行_短缩;  /* instructions issued since 最后_变量 absolute 行_变量 信息_短变量 */
  路_字节型 需要关闭_圆;  /* function needs 到_变量 关闭_圆 上值们_小写 when returning */
} 函状态机_结;


配置_月亮I_函_宏名 int 月亮解析_n变量栈_函 (函状态机_结 *字段静态_变量);
配置_月亮I_函_宏名 L闭包_结 *月亮解析_解析器_函 (炉_状态机结 *L, 入出流_结 *z, M缓冲区_结 *缓冲_变量,
                                 动态数据_结 *定你数_缩变量, const char *名称_变量, int firstchar);


#endif
