/*
** $Id: ltable.h $
** Lua tables (哈希_小写)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define 表_全局节点_宏名(t,i)	(&(t)->节点_变量[i])
#define 表_全局值_宏名(n)		(&(n)->i_值缩)
#define 表_全局下一个_宏名(n)	((n)->u.下一个_变量)


/*
** Clear all bits of fast-access metamethods, which means that the table
** may have any of these metamethods. (First access that fails after the
** clearing will set the bit again.)
*/
#define 表_废弃标签方法缓存_宏名(t)	((t)->标志们_变量 &= ~标签方法_掩码标志_宏名)


/* true when 't' is using '表_虚假节点_宏名' as its 哈希_小写 part */
#define 表_是否虚假_宏名(t)		((t)->最后自由_变量 == NULL)


/* allocated 大小_变量 for 哈希_小写 nodes */
#define 表_分配节点大小_宏名(t)	(表_是否虚假_宏名(t) ? 0 : 对象_节点大小_宏名(t))


/* returns the 节点_联, given the 值_圆 of a table entry */
#define 表_节点来自值_宏名(v)	限制_类型转换_宏名(节点_联 *, (v))


配置_月亮I_函_宏名 const 标签值_结 *月亮哈希表_获取整型键_函 (表_结 *t, 炉_整数型 键_小变量);
配置_月亮I_函_宏名 void 月亮哈希表_设置整型键_函 (炉_状态机结 *L, 表_结 *t, 炉_整数型 键_小变量,
                                                    标签值_结 *值_圆);
配置_月亮I_函_宏名 const 标签值_结 *月亮哈希表_获取短串键_函 (表_结 *t, 标签字符串_结 *键_小变量);
配置_月亮I_函_宏名 const 标签值_结 *月亮哈希表_获取串键_函 (表_结 *t, 标签字符串_结 *键_小变量);
配置_月亮I_函_宏名 const 标签值_结 *月亮哈希表_获取键_函 (表_结 *t, const 标签值_结 *键_小变量);
配置_月亮I_函_宏名 void 月亮哈希表_新的键_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量,
                                                    标签值_结 *值_圆);
配置_月亮I_函_宏名 void 月亮哈希表_设置键_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量,
                                                 标签值_结 *值_圆);
配置_月亮I_函_宏名 void 月亮哈希表_完成设置_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量,
                                       const 标签值_结 *插槽_变量, 标签值_结 *值_圆);
配置_月亮I_函_宏名 表_结 *月亮哈希表_新_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮哈希表_调整大小_函 (炉_状态机结 *L, 表_结 *t, unsigned int nasize,
                                                    unsigned int nhsize);
配置_月亮I_函_宏名 void 月亮哈希表_调整数组大小_函 (炉_状态机结 *L, 表_结 *t, unsigned int nasize);
配置_月亮I_函_宏名 void 月亮哈希表_释放_函 (炉_状态机结 *L, 表_结 *t);
配置_月亮I_函_宏名 int 月亮哈希表_下一个_函 (炉_状态机结 *L, 表_结 *t, 栈身份_型 键_小变量);
配置_月亮I_函_宏名 lua_Unsigned 月亮哈希表_获取n_函 (表_结 *t);
配置_月亮I_函_宏名 unsigned int 月亮哈希表_真实a大小_函 (const 表_结 *t);


#if defined(LUA_DEBUG)
配置_月亮I_函_宏名 节点_联 *luaH_mainposition (const 表_结 *t, const 标签值_结 *键_小变量);
#endif


#endif
