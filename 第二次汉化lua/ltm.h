/*
** $Id: ltm.h $
** Tag methods
** See Copyright Notice in lua.h
*/

#ifndef ltm_h
#define ltm_h


#include "lobject.h"
#include "lstate.h"


/*
* WARNING: if you 改变_变量 the order of this enumeration,
* grep "ORDER TM" and "ORDER OP"
*/
typedef enum {
  标方_索引大写,
  标方_新索引大写,
  标方_垃回大写,
  标方_模式大写,
  标方_长度大写,
  标方_相等大写,  /* 最后_变量 tag method with fast access */
  标方_加大写,
  标方_减大写,
  标方_乘大写,
  标方_取模大写,
  标方_幂运算大写,
  标方_除大写,
  标方_整数除大写,
  标方_位与大写,
  标方_位或大写,
  标方_位异或大写,
  标方_左移大写,
  标方_右移大写,
  标方_一元减号大写,
  标方_位非大写,
  标方_小于大写,
  标方_小等大写,
  标方_拼接大写,
  标方_调用大写,
  标方_关闭大写,
  标方_数目大写		/* number of elements in the enum */
} 标方符_枚举;


/*
** Mask with 1 in all fast-access methods. A 1 in any of these bits
** in the flag of a (meta)table means the 元表_小写 does not have the
** corresponding metamethod 月解析器_字段_函. (Bit 7 of the flag is used for
** '对象_是否真实数组大小_宏名'.)
*/
#define 标签方法_掩码标志_宏名	(~(~0u << (标方_相等大写 + 1)))


/*
** Test whether there is no tagmethod.
** (Because tagmethods use raw accesses, the 结果_变量 may be an "空容器_变量" nil.)
*/
#define 标签方法_无标方_宏名(标方_缩变量)	对象_tt是否空值_宏名(标方_缩变量)


#define 标签方法_全局快速标方_宏名(g,et,e) ((et) == NULL ? NULL : \
  ((et)->标志们_变量 & (1u<<(e))) ? NULL : 月亮类型_获取标签方法_函(et, e, (g)->标方名_变量[e]))

#define 标签方法_快速标方_宏名(l,et,e)	标签方法_全局快速标方_宏名(G(l), et, e)

#define 标签方法_t类型名称_宏名(x)	月亮T_类型名_变量[(x) + 1]

配置_月亮I_D声明_宏名(const char *const 月亮T_类型名_变量[对象_月亮_总共类型们_宏名];)


配置_月亮I_函_宏名 const char *月亮类型_对象类型名_函 (炉_状态机结 *L, const 标签值_结 *o);

配置_月亮I_函_宏名 const 标签值_结 *月亮类型_获取标签方法_函 (表_结 *events, 标方符_枚举 事件_变量, 标签字符串_结 *ename);
配置_月亮I_函_宏名 const 标签值_结 *月亮类型_通过对象获取标签方法_函 (炉_状态机结 *L, const 标签值_结 *o,
                                                       标方符_枚举 事件_变量);
配置_月亮I_函_宏名 void 月亮类型_初始化_函 (炉_状态机结 *L);

配置_月亮I_函_宏名 void 月亮类型_调用标签方法_函 (炉_状态机结 *L, const 标签值_结 *f, const 标签值_结 *p1,
                            const 标签值_结 *p2, const 标签值_结 *p3);
配置_月亮I_函_宏名 void 月亮类型_调用标签方法并返回结果_函 (炉_状态机结 *L, const 标签值_结 *f,
                            const 标签值_结 *p1, const 标签值_结 *p2, 栈身份_型 p3);
配置_月亮I_函_宏名 void 月亮类型_尝试二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2,
                              栈身份_型 结果_短变量, 标方符_枚举 事件_变量);
配置_月亮I_函_宏名 void 月亮类型_尝试拼接标签方法_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮类型_尝试关联二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1,
       const 标签值_结 *p2, int inv, 栈身份_型 结果_短变量, 标方符_枚举 事件_变量);
配置_月亮I_函_宏名 void 月亮类型_尝试索引二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, 炉_整数型 索2_缩变量,
                               int inv, 栈身份_型 结果_短变量, 标方符_枚举 事件_变量);
配置_月亮I_函_宏名 int 月亮类型_调用秩序标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1,
                                const 标签值_结 *p2, 标方符_枚举 事件_变量);
配置_月亮I_函_宏名 int 月亮类型_调用秩序索引标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, int 变2_缩变量,
                                 int inv, int 是否浮点_变量, 标方符_枚举 事件_变量);

配置_月亮I_函_宏名 void 月亮类型_调整可变实参们_函 (炉_状态机结 *L, int 固定形参数_变量,
                                   调用信息_结 *调信_缩变量, const 原型_结 *p);
配置_月亮I_函_宏名 void 月亮类型_获取可变实参们_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量,
                                              栈身份_型 哪儿_变量, int 已想要_变量);


#endif
