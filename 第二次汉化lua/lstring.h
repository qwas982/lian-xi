/*
** $Id: lstring.h $
** String table (keep all strings handled by Lua)
** See Copyright Notice in lua.h
*/

#ifndef lstring_h
#define lstring_h

#include "lgc.h"
#include "lobject.h"
#include "lstate.h"


/*
** Memory-allocation 错误_小变量 message must be preallocated (it 月编译器_不能_函
** be created after memory is exhausted)
*/
#define 字符串_内存错误消息_宏名       "not enough memory"


/*
** Size of a 标签字符串_结: Size of the header plus 空间_圆 for the string
** itself (including 最终_变量 '\0').
*/
#define 字符串_大写l字符串_宏名(l)  (offsetof(标签字符串_结, 内容_小写) + ((l) + 1) * sizeof(char))

#define 字符串_月亮S_新字面_宏名(L, s)	(月亮字符串_新长串_函(L, "" s, \
                                 (sizeof(s)/sizeof(char))-1))


/*
** test whether a string is a reserved word
*/
#define 字符串_是否保留字_宏名(s)	((s)->类标_缩变量 == 对象_月亮_V短型串_宏名 && (s)->额外_变量 > 0)


/*
** equality for short strings, which are always internalized
*/
#define 字符串_相等短型串_宏名(a,b)	限制_检查_表达式_宏名((a)->类标_缩变量 == 对象_月亮_V短型串_宏名, (a) == (b))


配置_月亮I_函_宏名 unsigned int 月亮字符串_哈希_函 (const char *串_变量, size_t l, unsigned int 种籽_圆);
配置_月亮I_函_宏名 unsigned int 月亮字符串_哈希长的串_函 (标签字符串_结 *类s_变量);
配置_月亮I_函_宏名 int 月亮字符串_相等长型串_函 (标签字符串_结 *a, 标签字符串_结 *b);
配置_月亮I_函_宏名 void 月亮字符串_调整大小_函 (炉_状态机结 *L, int 新大小_变量);
配置_月亮I_函_宏名 void 月亮字符串_清除缓存_函 (全局_状态机结 *g);
配置_月亮I_函_宏名 void 月亮字符串_初始的_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮字符串_移除_函 (炉_状态机结 *L, 标签字符串_结 *类s_变量);
配置_月亮I_函_宏名 用户数据_结 *月亮字符串_新用户数据_函 (炉_状态机结 *L, size_t s, int 用户值数_缩);
配置_月亮I_函_宏名 标签字符串_结 *月亮字符串_新长串_函 (炉_状态机结 *L, const char *串_变量, size_t l);
配置_月亮I_函_宏名 标签字符串_结 *月亮字符串_新_函 (炉_状态机结 *L, const char *串_变量);
配置_月亮I_函_宏名 标签字符串_结 *月亮字符串_创建长型串对象_函 (炉_状态机结 *L, size_t l);


#endif
