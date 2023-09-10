/*
** $Id: ldo.h $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#ifndef ldo_h
#define ldo_h


#include "llimits.h"
#include "lobject.h"
#include "lstate.h"
#include "lzio.h"


/*
** Macro 到_变量 月解析器_检查_函 栈_圆小 大小_变量 and grow 栈_圆小 if 已需要_变量.  Parameters
** 'pre'/'位置_缩变量' allow the macro 到_变量 preserve a pointer into the
** 栈_圆小 across reallocations, doing the 工作_变量 only when 已需要_变量.
** It also allows the running of one GC 步进_变量 when the 栈_圆小 is
** reallocated.
** '限制_条件移动栈_宏名' is used in heavy tests 到_变量 force a 栈_圆小 reallocation
** at every 月解析器_检查_函.
*/
#define 做_月亮D_检查辅助栈_宏名(L,n,pre,位置_缩变量)  \
	if (配置_l_可能性低_宏名(L->栈_最后圆.p - L->顶部_变量.p <= (n))) \
	  { pre; 月亮调度_增长栈_函(L, n, 1); 位置_缩变量; } \
        else { 限制_条件移动栈_宏名(L,pre,位置_缩变量); }

/* In general, 'pre'/'位置_缩变量' are 空容器_变量 (nothing 到_变量 月词法_保存_函) */
#define 做_月亮D_检查栈_宏名(L,n)	做_月亮D_检查辅助栈_宏名(L,n,(void)0,(void)0)



#define 做_保存栈_宏名(L,pt)		(限制_类型转换_印刻指针_宏名(pt) - 限制_类型转换_印刻指针_宏名(L->栈_圆小.p))
#define 做_恢复栈_宏名(L,n)	限制_类型转换_宏名(栈身份_型, 限制_类型转换_印刻指针_宏名(L->栈_圆小.p) + (n))


/* macro 到_变量 月解析器_检查_函 栈_圆小 大小_变量, preserving 'p' */
#define 做_检查栈指针_宏名(L,n,p)  \
  做_月亮D_检查辅助栈_宏名(L, n, \
    ptrdiff_t t__ = 做_保存栈_宏名(L, p),  /* 月词法_保存_函 'p' */ \
    p = 做_恢复栈_宏名(L, t__))  /* '位置_缩变量' part: restore 'p' */


/* macro 到_变量 月解析器_检查_函 栈_圆小 大小_变量 and GC, preserving 'p' */
#define 做_检查栈GC指针_宏名(L,n,p)  \
  做_月亮D_检查辅助栈_宏名(L, n, \
    ptrdiff_t t__ = 做_保存栈_宏名(L, p);  /* 月词法_保存_函 'p' */ \
    垃圾回收_月亮C_检查GC_宏名(L),  /* 栈_圆小 grow uses memory */ \
    p = 做_恢复栈_宏名(L, t__))  /* '位置_缩变量' part: restore 'p' */


/* macro 到_变量 月解析器_检查_函 栈_圆小 大小_变量 and GC */
#define 做_检查栈GC_宏名(L,函大小_变量)  \
	做_月亮D_检查辅助栈_宏名(L, (函大小_变量), 垃圾回收_月亮C_检查GC_宏名(L), (void)0)


/* type of protected functions, 到_变量 be ran by 'runprotected' */
typedef void (*Pfunc) (炉_状态机结 *L, void *用数_缩变量);

配置_月亮I_函_宏名 void 月亮调度_设置错误对象_函 (炉_状态机结 *L, int errcode, 栈身份_型 oldtop);
配置_月亮I_函_宏名 int 月亮调度_受保护解析器_函 (炉_状态机结 *L, 入出流_结 *z, const char *名称_变量,
                                                  const char *模块_变量);
配置_月亮I_函_宏名 void 月亮调度_钩子_函 (炉_状态机结 *L, int 事件_变量, int 行_变量,
                                        int fTransfer, int nTransfer);
配置_月亮I_函_宏名 void 月亮调度_钩子调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量);
配置_月亮I_函_宏名 int 月亮调度_预备尾调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, 栈身份_型 函_短变量,
                                              int narg1, int 德尔塔_变量);
配置_月亮I_函_宏名 调用信息_结 *月亮调度_预备调用_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults);
配置_月亮I_函_宏名 void 月亮调度_调用_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults);
配置_月亮I_函_宏名 void 月亮调度_调用无产出_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults);
配置_月亮I_函_宏名 栈身份_型 月亮调度_尝试函标签方法_函 (炉_状态机结 *L, 栈身份_型 函_短变量);
配置_月亮I_函_宏名 int 月亮调度_关闭受保护_函 (炉_状态机结 *L, ptrdiff_t 层级_变量, int 状态码_变量);
配置_月亮I_函_宏名 int 月亮调度_预处理调用_函 (炉_状态机结 *L, Pfunc 函_短变量, void *u,
                                        ptrdiff_t oldtop, ptrdiff_t 错标_缩变量);
配置_月亮I_函_宏名 void 月亮调度_后处理调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, int 结果数目_变量);
配置_月亮I_函_宏名 int 月亮调度_重新分配栈_函 (炉_状态机结 *L, int 新大小_变量, int raiseerror);
配置_月亮I_函_宏名 int 月亮调度_增长栈_函 (炉_状态机结 *L, int n, int raiseerror);
配置_月亮I_函_宏名 void 月亮调度_收缩栈_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮调度_递增顶_函 (炉_状态机结 *L);

配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮调度_抛出_函 (炉_状态机结 *L, int errcode);
配置_月亮I_函_宏名 int 月亮调度_原始跑受保护_函 (炉_状态机结 *L, Pfunc f, void *用数_缩变量);

#endif

