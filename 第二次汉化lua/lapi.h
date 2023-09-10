/*
** $Id: lapi.h $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include "llimits.h"
#include "lstate.h"


/* Increments 'L->顶部_变量.p', checking for 栈_圆小 overflows */
#define 应程接_API_递增_顶部_宏名(L)	{L->顶部_变量.p++; \
			 限制_应程接_检查_宏名(L, L->顶部_变量.p <= L->调信_缩变量->顶部_变量.p, \
					"栈_圆小 overflow");}


/*
** If a call returns too many multiple returns, the callee may not have
** 栈_圆小 空间_圆 到_变量 accommodate all results. In this case, this macro
** increases its 栈_圆小 空间_圆 ('L->调信_缩变量->顶部_变量.p').
*/
#define 应程接_调整结果们_宏名(L,结果数目_变量) \
    { if ((结果数目_变量) <= 月头_月亮_多返回_宏名 && L->调信_缩变量->顶部_变量.p < L->顶部_变量.p) \
	L->调信_缩变量->顶部_变量.p = L->顶部_变量.p; }


/* Ensure the 栈_圆小 has at least 'n' elements */
#define 应程接_API检查n元素们_宏名(L,n) \
	限制_应程接_检查_宏名(L, (n) < (L->顶部_变量.p - L->调信_缩变量->函_短变量.p), \
			  "not enough elements in the 栈_圆小")


/*
** To reduce the overhead of returning from C functions, the presence of
** 到_变量-be-closed variables in these functions is coded in the 调用信息_结's
** 月解析器_字段_函 '结果数目_变量', in a way that functions with no 到_变量-be-closed variables
** with zero, one, or "all" 已想要_变量 results have no overhead. Functions
** with other number of 已想要_变量 results, as well as functions with
** variables 到_变量 be closed, have an 额外_变量 月解析器_检查_函.
*/

#define 应程接_必须关闭C函数_宏名(n)	((n) < 月头_月亮_多返回_宏名)

/* Map [-1, inf) (range of '结果数目_变量') into (-inf, -2] */
#define 应程接_编码N结果们_宏名(n)		(-(n) - 3)
#define 应程接_解码N结果们_宏名(n)	(-(n) - 3)

#endif
