/*
** $Id: lfunc.h $
** Auxiliary functions 到_变量 manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#ifndef lfunc_h
#define lfunc_h


#include "lobject.h"


#define 函_C闭包大小_宏名(n)	(限制_类型转换_整型_宏名(offsetof(C闭包_结, 上值_圆)) + \
                         限制_类型转换_整型_宏名(sizeof(标签值_结)) * (n))

#define 函_L闭包大小_宏名(n)	(限制_类型转换_整型_宏名(offsetof(L闭包_结, 上值们_短)) + \
                         限制_类型转换_整型_宏名(sizeof(标签值_结 *)) * (n))


/* test whether 线程_变量 is in '线程与上值_短缩' 列表_变量 */
#define 函_是否具有内部的上值_宏名(L)	(L->线程与上值_短缩 != L)


/*
** maximum number of 上值们_小写 in a closure (both C and Lua). (值_联
** must fit in a VM register.)
*/
#define 函_最大上值_宏名	255


#define 函_上是否打开_宏名(上_小变量)	((上_小变量)->v.p != &(上_小变量)->u.值_圆)


#define 函_上层级_宏名(上_小变量)	限制_检查_表达式_宏名(函_上是否打开_宏名(上_小变量), 限制_类型转换_宏名(栈身份_型, (上_小变量)->v.p))


/*
** maximum number of misses before giving 上_小变量 the cache of closures
** in prototypes
*/
#define 函_最大丢失_宏名		10



/* special 状态码_变量 到_变量 关闭_圆 上值们_小写 preserving the 顶部_变量 of the 栈_圆小 */
#define 函_关闭栈顶_宏名	(-1)


配置_月亮I_函_宏名 原型_结 *月亮函数_新原型_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 C闭包_结 *月亮函数_新C闭包_函 (炉_状态机结 *L, int nupvals);
配置_月亮I_函_宏名 L闭包_结 *月亮函数_新L闭包_函 (炉_状态机结 *L, int nupvals);
配置_月亮I_函_宏名 void 月亮函数_初始的上值_函 (炉_状态机结 *L, L闭包_结 *闭包_短变量);
配置_月亮I_函_宏名 上值_结 *月亮函数_查找上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量);
配置_月亮I_函_宏名 void 月亮函数_新待关闭上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量);
配置_月亮I_函_宏名 void 月亮函数_关闭上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量);
配置_月亮I_函_宏名 栈身份_型 月亮函数_关闭_函 (炉_状态机结 *L, 栈身份_型 层级_变量, int 状态码_变量, int yy);
配置_月亮I_函_宏名 void 月亮函数_解除链上值_函 (上值_结 *上值_缩变量);
配置_月亮I_函_宏名 void 月亮函数_释放原型_函 (炉_状态机结 *L, 原型_结 *f);
配置_月亮I_函_宏名 const char *月亮函数_获取本地名_函 (const 原型_结 *函_短变量, int local_number,
                                         int 程序计数_变量);


#endif
