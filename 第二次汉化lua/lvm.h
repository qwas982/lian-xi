/*
** $Id: lvm.h $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "lobject.h"
#include "ltm.h"


#if !defined(配置_月亮_不转化数目到串_宏名)
#define 虚机头_转化到串_宏名(o)	对象_tt是否数目_宏名(o)
#else
#define 虚机头_转化到串_宏名(o)	0	/* no conversion from numbers 到_变量 strings */
#endif


#if !defined(配置_月亮_不转化串到数目_宏名)
#define 虚机头_转化到数目_宏名(o)	对象_tt是否字符串_宏名(o)
#else
#define 虚机头_转化到数目_宏名(o)	0	/* no conversion from strings 到_变量 numbers */
#endif


/*
** You can define 虚机头_月亮_向下取整数目到整数_宏名 if you want 到_变量 convert floats 到_变量 integers
** by flooring them (instead of raising an 错误_小变量 if they are not
** integral values)
*/
#if !defined(虚机头_月亮_向下取整数目到整数_宏名)
#define 虚机头_月亮_向下取整数目到整数_宏名		浮到整相等_
#endif


/*
** Rounding modes for float->integer coercion
 */
typedef enum {
  浮到整相等_,     /* no rounding; accepts only integral values */
  浮到整向下取整_,  /* takes the floor of the number */
  浮到整向上取整_    /* takes the ceil of the number */
} 浮到整模式_枚举;


/* convert an object 到_变量 a float (including string coercion) */
#define 虚机头_到数目_宏名(o,n) \
	(对象_tt是否浮点_宏名(o) ? (*(n) = 对象_浮点值_宏名(o), 1) : 月亮虚拟机_到数目__函(o,n))


/* convert an object 到_变量 a float (without string coercion) */
#define 虚机头_到数目非安全_宏名(o,n) \
	(对象_tt是否浮点_宏名(o) ? ((n) = 对象_浮点值_宏名(o), 1) : \
	(对象_tt是否整数_宏名(o) ? ((n) = 限制_类型转换_数目_宏名(对象_整数值_宏名(o)), 1) : 0))


/* convert an object 到_变量 an integer (including string coercion) */
#define 虚机头_到整数_宏名(o,i) \
  (配置_l_可能性高_宏名(对象_tt是否整数_宏名(o)) ? (*(i) = 对象_整数值_宏名(o), 1) \
                          : 月亮虚拟机_到整数_函(o,i,虚机头_月亮_向下取整数目到整数_宏名))


/* convert an object 到_变量 an integer (without string coercion) */
#define 虚机头_到整数非安全_宏名(o,i) \
  (配置_l_可能性高_宏名(对象_tt是否整数_宏名(o)) ? (*(i) = 对象_整数值_宏名(o), 1) \
                          : 月亮虚拟机_非安全到整数_函(o,i,虚机头_月亮_向下取整数目到整数_宏名))


#define 虚机头_整型操作_宏名(操作_短变量,变1_缩变量,变2_缩变量) 限制_l_类型转换无符到符_宏名(限制_l_类型转换符到无符_宏名(变1_缩变量) 操作_短变量 限制_l_类型转换符到无符_宏名(变2_缩变量))

#define 虚机头_月亮V_原始相等对象_宏名(临1_短变量,临2_短变量)		月亮虚拟机_相等对象_函(NULL,临1_短变量,临2_短变量)


/*
** fast track for '月应程接_获取表_函': if 't' is a table and 't[k]' is present,
** return 1 with '插槽_变量' pointing 到_变量 't[k]' (position of 最终_变量 结果_变量).
** Otherwise, return 0 (meaning it will have 到_变量 月解析器_检查_函 metamethod)
** with '插槽_变量' pointing 到_变量 an 空容器_变量 't[k]' (if 't' is a table) or NULL
** (otherwise). 'f' is the raw get function 到_变量 use.
*/
#define 虚机头_月亮V_快速获取_宏名(L,t,k,插槽_变量,f) \
  (!对象_tt是否表_宏名(t)  \
   ? (插槽_变量 = NULL, 0)  /* not a table; '插槽_变量' is NULL and 结果_变量 is 0 */  \
   : (插槽_变量 = f(对象_哈希值_宏名(t), k),  /* else, do raw access */  \
      !对象_是否空容器_宏名(插槽_变量)))  /* 结果_变量 not 空容器_变量? */


/*
** Special case of '虚机头_月亮V_快速获取_宏名' for integers, inlining the fast case
** of '月亮哈希表_获取整型键_函'.
*/
#define 虚机头_月亮V_快速获取索引_宏名(L,t,k,插槽_变量) \
  (!对象_tt是否表_宏名(t)  \
   ? (插槽_变量 = NULL, 0)  /* not a table; '插槽_变量' is NULL and 结果_变量 is 0 */  \
   : (插槽_变量 = (限制_l_类型转换符到无符_宏名(k) - 1u < 对象_哈希值_宏名(t)->数组限制_缩) \
              ? &对象_哈希值_宏名(t)->数组_圆[k - 1] : 月亮哈希表_获取整型键_函(对象_哈希值_宏名(t), k), \
      !对象_是否空容器_宏名(插槽_变量)))  /* 结果_变量 not 空容器_变量? */


/*
** Finish a fast set operation (when fast get succeeds). In that case,
** '插槽_变量' points 到_变量 the place 到_变量 put the 值_圆.
*/
#define 虚机头_月亮V_结束快速设置_宏名(L,t,插槽_变量,v) \
    { 对象_设置对象到t_宏名(L, 限制_类型转换_宏名(标签值_结 *,插槽_变量), v); \
      垃圾回收_月亮C_屏障后退_宏名(L, 对象_垃圾回收值_宏名(t), v); }


/*
** Shift 右_圆 is the same as shift 左_圆 with a negative 'y'
*/
#define 虚机头_月亮V_右移_宏名(x,y)	月亮虚拟机_左移_函(x,虚机头_整型操作_宏名(-, 0, y))



配置_月亮I_函_宏名 int 月亮虚拟机_相等对象_函 (炉_状态机结 *L, const 标签值_结 *临1_短变量, const 标签值_结 *临2_短变量);
配置_月亮I_函_宏名 int 月亮虚拟机_小于_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r);
配置_月亮I_函_宏名 int 月亮虚拟机_小于等于_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r);
配置_月亮I_函_宏名 int 月亮虚拟机_到数目__函 (const 标签值_结 *对象_变量, 炉_数目型 *n);
配置_月亮I_函_宏名 int 月亮虚拟机_到整数_函 (const 标签值_结 *对象_变量, 炉_整数型 *p, 浮到整模式_枚举 模块_变量);
配置_月亮I_函_宏名 int 月亮虚拟机_非安全到整数_函 (const 标签值_结 *对象_变量, 炉_整数型 *p,
                                浮到整模式_枚举 模块_变量);
配置_月亮I_函_宏名 int 月亮虚拟机_浮点数到整数_函 (炉_数目型 n, 炉_整数型 *p, 浮到整模式_枚举 模块_变量);
配置_月亮I_函_宏名 void 月亮虚拟机_完成获取_函 (炉_状态机结 *L, const 标签值_结 *t, 标签值_结 *键_小变量,
                               栈身份_型 值_变量, const 标签值_结 *插槽_变量);
配置_月亮I_函_宏名 void 月亮虚拟机_完成设置_函 (炉_状态机结 *L, const 标签值_结 *t, 标签值_结 *键_小变量,
                               标签值_结 *值_变量, const 标签值_结 *插槽_变量);
配置_月亮I_函_宏名 void 月亮虚拟机_完成操作_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮虚拟机_执行_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量);
配置_月亮I_函_宏名 void 月亮虚拟机_拼接_函 (炉_状态机结 *L, int 总数_变量);
配置_月亮I_函_宏名 炉_整数型 月亮虚拟机_整数除法_函 (炉_状态机结 *L, 炉_整数型 x, 炉_整数型 y);
配置_月亮I_函_宏名 炉_整数型 月亮虚拟机_取模_函 (炉_状态机结 *L, 炉_整数型 x, 炉_整数型 y);
配置_月亮I_函_宏名 炉_数目型 月亮虚拟机_浮点数取整_函 (炉_状态机结 *L, 炉_数目型 x, 炉_数目型 y);
配置_月亮I_函_宏名 炉_整数型 月亮虚拟机_左移_函 (炉_整数型 x, 炉_整数型 y);
配置_月亮I_函_宏名 void 月亮虚拟机_对象长度_函 (炉_状态机结 *L, 栈身份_型 奖励_变量, const 标签值_结 *寄b_变量);

#endif
