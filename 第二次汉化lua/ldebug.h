/*
** $Id: ldebug.h $
** Auxiliary functions from Debug Interface module
** See Copyright Notice in lua.h
*/

#ifndef ldebug_h
#define ldebug_h


#include "lstate.h"


#define 调试_相对指令计数器_宏名(程序计数_变量, p)	(限制_类型转换_整型_宏名((程序计数_变量) - (p)->代码_变量) - 1)


/* Active Lua function (given call 信息_短变量) */
#define 调试_ci_函_宏名(调信_缩变量)		(对象_闭包L值_宏名(对象_s到v_宏名((调信_缩变量)->函_短变量.p)))


#define 调试_重置钩子计数_宏名(L)	(L->钩子计数_圆 = L->基本钩子计数_圆)

/*
** 记号_变量 for entries in '行信息_变量' 数组_圆 that has absolute information in
** '绝对行信息_小写' 数组_圆
*/
#define 调试_绝对行信息_宏名	(-0x80)


/*
** MAXimum number of successive Instructions WiTHout ABSolute 行_变量
** information. (A power of two allows fast divisions.)
*/
#if !defined(调试_最大I宽度绝对值_宏名)
#define 调试_最大I宽度绝对值_宏名	128
#endif


配置_月亮I_函_宏名 int 月亮全局_获取函数行号_函 (const 原型_结 *f, int 程序计数_变量);
配置_月亮I_函_宏名 const char *月亮全局_查找本地变量_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, int n,
                                                    栈身份_型 *位置_缩变量);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_类型错误_函 (炉_状态机结 *L, const 标签值_结 *o,
                                                const char *opname);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_调用错误_函 (炉_状态机结 *L, const 标签值_结 *o);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_for循环错误_函 (炉_状态机结 *L, const 标签值_结 *o,
                                               const char *什么_变量);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_拼接错误_函 (炉_状态机结 *L, const 标签值_结 *p1,
                                                  const 标签值_结 *p2);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_操作整型错误_函 (炉_状态机结 *L, const 标签值_结 *p1,
                                                 const 标签值_结 *p2,
                                                 const char *消息_缩变量);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_到整型错误_函 (炉_状态机结 *L, const 标签值_结 *p1,
                                                 const 标签值_结 *p2);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_秩序错误_函 (炉_状态机结 *L, const 标签值_结 *p1,
                                                 const 标签值_结 *p2);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_跑错误_函 (炉_状态机结 *L, const char *格式_短变量, ...);
配置_月亮I_函_宏名 const char *月亮全局_添加信息_函 (炉_状态机结 *L, const char *消息_缩变量,
                                                  标签字符串_结 *源_短变量, int 行_变量);
配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮全局_错误消息_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 int 月亮全局_跟踪执行_函 (炉_状态机结 *L, const Instruction *程序计数_变量);


#endif
