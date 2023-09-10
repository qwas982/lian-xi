/*
** $Id: 月词法_月的词法_函.h $
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#ifndef llex_h
#define llex_h

#include <limits.h>

#include "lobject.h"
#include "lzio.h"


/*
** Single-char tokens (terminal symbols) are represented by their own
** numeric 代码_变量. Other tokens 起始_变量 at the following 值_圆.
*/
#define 首个_保留	(UCHAR_MAX + 1)


#if !defined(词法_月亮_环境_宏名)
#define 词法_月亮_环境_宏名		"_ENV"
#endif


/*
* WARNING: if you 改变_变量 the order of this enumeration,
* grep "ORDER 保留_枚举"
*/
enum 保留_枚举 {
  /* terminal symbols denoted by reserved words */
  终结牌_与 = 首个_保留, 终结牌_破断,
  终结牌_做, 终结牌_否则, 终结牌_否若, 终结牌_终, 终结牌_假, 终结牌_为, 终结牌_函数,
  终结牌_去到, 终结牌_若, 终结牌_在内, 终结牌_本地, 终结牌_空值, 终结牌_非, 终结牌_或, 终结牌_重复,
  终结牌_返回, 终结牌_那么, 终结牌_真, 终结牌_直到, 终结牌_一会儿,
  /* other terminal symbols */
  终结牌_整数除, 终结牌_拼接, 终结牌_省略, 终结牌_相等, 终结牌_大于, 终结牌_小于, 终结牌_不等,
  终结牌_左移, 终结牌_右移,
  终结牌_双冒号, 终结牌_流终,
  终结牌_浮点, 终结牌_整型, 终结牌_名称, 终结牌_字符串
};

/* number of reserved words */
#define 词法_数目_保留字_宏名	(限制_类型转换_整型_宏名(终结牌_一会儿-首个_保留 + 1))


typedef union {
  炉_数目型 r;
  炉_整数型 i;
  标签字符串_结 *类s_变量;
} 语义信息_联;  /* semantics information */


typedef struct 牌_结 {
  int 牌_小写;
  语义信息_联 语义信息_小写;
} 牌_结;


/* 状态机_变量 of the lexer plus 状态机_变量 of the parser when shared by all
   functions */
typedef struct 词法状态机_结 {
  int 当前_圆;  /* 当前_圆 character (charint) */
  int 行数目_小写;  /* input 行_变量 counter */
  int 最后行_小写;  /* 行_变量 of 最后_变量 牌_小写 'consumed' */
  牌_结 t;  /* 当前_圆 牌_小写 */
  牌_结 看向前_小写;  /* look ahead 牌_小写 */
  struct 函状态机_结 *字段静态_变量;  /* 当前_圆 function (parser) */
  struct 炉_状态机结 *L;
  入出流_结 *z;  /* input stream */
  M缓冲区_结 *缓冲_变量;  /* 缓冲区_变量 for tokens */
  表_结 *h;  /* 到_变量 avoid collection/reuse strings */
  struct 动态数据_结 *定你数_缩变量;  /* dynamic structures used by the parser */
  标签字符串_结 *源_圆;  /* 当前_圆 源_圆 名称_变量 */
  标签字符串_结 *环境名_短;  /* environment variable 名称_变量 */
} 词法状态机_结;


配置_月亮I_函_宏名 void 月亮分析_初始的_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮分析_设置输入_函 (炉_状态机结 *L, 词法状态机_结 *状列_缩变量, 入出流_结 *z,
                              标签字符串_结 *源_圆, int firstchar);
配置_月亮I_函_宏名 标签字符串_结 *月亮分析_新字符串_函 (词法状态机_结 *状列_缩变量, const char *串_变量, size_t l);
配置_月亮I_函_宏名 void 月亮分析_下一个_函 (词法状态机_结 *状列_缩变量);
配置_月亮I_函_宏名 int 月亮分析_看向前_函 (词法状态机_结 *状列_缩变量);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮分析_句法错误_函 (词法状态机_结 *状列_缩变量, const char *s);
配置_月亮I_函_宏名 const char *月亮分析_牌到串_函 (词法状态机_结 *状列_缩变量, int 牌_小写);


#endif
