/*
** $Id: lvm.c $
** Lua virtual machine
** See Copyright Notice in lua.h
*/

#define lvm_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"


/*
** By default, use jump tables in the main interpreter 环_变量 on gcc
** and compatible compilers.
*/
#if !defined(虚拟机_月亮_用_跳转表_宏名)
#if defined(__GNUC__)
#define 虚拟机_月亮_用_跳转表_宏名	1
#else
#define 虚拟机_月亮_用_跳转表_宏名	0
#endif
#endif



/* 限制_变量 for table tag-method chains (到_变量 avoid infinite loops) */
#define 虚拟机_最大标签环_宏名	2000


/*
** '虚拟机_l_整型单浮点范围_宏名' checks whether a given integer is in the range that
** can be converted 到_变量 a float without rounding. Used in comparisons.
*/

/* number of bits in the mantissa of a float */
#define NBM		(配置_l_浮点属性_宏名(MANT_DIG))

/*
** Check whether some integers may not fit in a float, testing whether
** (maxinteger >> NBM) > 0. (That implies (1 << NBM) <= maxinteger.)
** (The shifts are done in parts, 到_变量 avoid shifting by 更多_变量 than the 大小_变量
** of an integer. In a worst case, NBM == 113 for long double and
** sizeof(long) == 32.)
*/
#if ((((配置_月亮_最大整数_宏名 >> (NBM / 4)) >> (NBM / 4)) >> (NBM / 4)) \
	>> (NBM - (3 * (NBM / 4))))  >  0

/* 限制_变量 for integers that fit in a float */
#define 虚拟机_最大整型单浮点范围_宏名	((lua_Unsigned)1 << NBM)

/* 月解析器_检查_函 whether 'i' is in the interval [-虚拟机_最大整型单浮点范围_宏名, 虚拟机_最大整型单浮点范围_宏名] */
#define 虚拟机_l_整型单浮点范围_宏名(i)	((虚拟机_最大整型单浮点范围_宏名 + 限制_l_类型转换符到无符_宏名(i)) <= (2 * 虚拟机_最大整型单浮点范围_宏名))

#else  /* all integers fit in a float precisely */

#define 虚拟机_l_整型单浮点范围_宏名(i)	1

#endif


/*
** Try 到_变量 convert a 值_圆 from string 到_变量 a number 值_圆.
** If the 值_圆 is not a string or is a string not representing
** a 有效_变量 numeral (or if coercions from strings 到_变量 numbers
** are disabled via macro '虚机头_转化到数目_宏名'), do not modify '结果_变量'
** and return 0.
*/
static int 月虚拟机_l_串到n_函 (const 标签值_结 *对象_变量, 标签值_结 *结果_变量) {
  限制_月亮_断言_宏名(对象_变量 != 结果_变量);
  if (!虚机头_转化到数目_宏名(对象_变量))  /* is object not a string? */
    return 0;
  else
    return (月亮对象_字符串到数目_函(对象_s值_宏名(对象_变量), 结果_变量) == 对象_vs长度_宏名(对象_变量) + 1);
}


/*
** Try 到_变量 convert a 值_圆 到_变量 a float. The float case is already handled
** by the macro '虚机头_到数目_宏名'.
*/
int 月亮虚拟机_到数目__函 (const 标签值_结 *对象_变量, 炉_数目型 *n) {
  标签值_结 v;
  if (对象_tt是否整数_宏名(对象_变量)) {
    *n = 限制_类型转换_数目_宏名(对象_整数值_宏名(对象_变量));
    return 1;
  }
  else if (月虚拟机_l_串到n_函(对象_变量, &v)) {  /* string coercible 到_变量 number? */
    *n = 对象_数目值_宏名(&v);  /* convert 结果_变量 of '月亮对象_字符串到数目_函' 到_变量 a float */
    return 1;
  }
  else
    return 0;  /* conversion failed */
}


/*
** try 到_变量 convert a float 到_变量 an integer, rounding according 到_变量 '模块_变量'.
*/
int 月亮虚拟机_浮点数到整数_函 (炉_数目型 n, 炉_整数型 *p, 浮到整模式_枚举 模块_变量) {
  炉_数目型 f = 配置_l_向下取整_宏名(n);
  if (n != f) {  /* not an integral 值_圆? */
    if (模块_变量 == 浮到整相等_) return 0;  /* fails if 模块_变量 demands integral 值_圆 */
    else if (模块_变量 == 浮到整向上取整_)  /* needs ceil? */
      f += 1;  /* convert floor 到_变量 ceil (remember: n != f) */
  }
  return 配置_月亮_数目到整数_宏名(f, p);
}


/*
** try 到_变量 convert a 值_圆 到_变量 an integer, rounding according 到_变量 '模块_变量',
** without string coercion.
** ("Fast track" handled by macro '虚机头_到整数非安全_宏名'.)
*/
int 月亮虚拟机_非安全到整数_函 (const 标签值_结 *对象_变量, 炉_整数型 *p, 浮到整模式_枚举 模块_变量) {
  if (对象_tt是否浮点_宏名(对象_变量))
    return 月亮虚拟机_浮点数到整数_函(对象_浮点值_宏名(对象_变量), p, 模块_变量);
  else if (对象_tt是否整数_宏名(对象_变量)) {
    *p = 对象_整数值_宏名(对象_变量);
    return 1;
  }
  else
    return 0;
}


/*
** try 到_变量 convert a 值_圆 到_变量 an integer.
*/
int 月亮虚拟机_到整数_函 (const 标签值_结 *对象_变量, 炉_整数型 *p, 浮到整模式_枚举 模块_变量) {
  标签值_结 v;
  if (月虚拟机_l_串到n_函(对象_变量, &v))  /* does '对象_变量' 针点_变量 到_变量 a numerical string? */
    对象_变量 = &v;  /* 改变_变量 it 到_变量 针点_变量 到_变量 its corresponding number */
  return 月亮虚拟机_非安全到整数_函(对象_变量, p, 模块_变量);
}


/*
** Try 到_变量 convert a 'for' 限制_变量 到_变量 an integer, preserving the semantics
** of the 环_变量. Return true if the 环_变量 must not run; otherwise, '*p'
** gets the integer 限制_变量.
** (The following explanation assumes a positive 步进_变量; it is 有效_变量 for
** negative steps mutatis mutandis.)
** If the 限制_变量 is an integer or can be converted 到_变量 an integer,
** rounding down, that is the 限制_变量.
** Otherwise, 月解析器_检查_函 whether the 限制_变量 can be converted 到_变量 a float. If
** the float is too large, clip it 到_变量 配置_月亮_最大整数_宏名.  If the float
** is too negative, the 环_变量 should not run, because any initial
** integer 值_圆 is greater than such 限制_变量; so, the function returns
** true 到_变量 signal that. (For this latter case, no integer 限制_变量 would be
** correct; even a 限制_变量 of 配置_月亮_最小整数_宏名 would run the 环_变量 once for
** an initial 值_圆 equal 到_变量 配置_月亮_最小整数_宏名.)
*/
static int 月虚拟机_为限制_函 (炉_状态机结 *L, 炉_整数型 初始的_变量, const 标签值_结 *限_短变量,
                                   炉_整数型 *p, 炉_整数型 步进_变量) {
  if (!月亮虚拟机_到整数_函(限_短变量, p, (步进_变量 < 0 ? 浮到整向上取整_ : 浮到整向下取整_))) {
    /* not coercible 到_变量 in integer */
    炉_数目型 浮点限_变量;  /* try 到_变量 convert 到_变量 float */
    if (!虚机头_到数目_宏名(限_短变量, &浮点限_变量)) /* 月编译器_不能_函 convert 到_变量 float? */
      月亮全局_for循环错误_函(L, 限_短变量, "限制_变量");
    /* else '浮点限_变量' is a float out of integer bounds */
    if (限制_月亮i_数目小于_宏名(0, 浮点限_变量)) {  /* if it is positive, it is too large */
      if (步进_变量 < 0) return 1;  /* initial 值_圆 must be less than it */
      *p = 配置_月亮_最大整数_宏名;  /* truncate */
    }
    else {  /* it is less than min integer */
      if (步进_变量 > 0) return 1;  /* initial 值_圆 must be greater than it */
      *p = 配置_月亮_最小整数_宏名;  /* truncate */
    }
  }
  return (步进_变量 > 0 ? 初始的_变量 > *p : 初始的_变量 < *p);  /* not 到_变量 run? */
}


/*
** Prepare a numerical for 环_变量 (opcode 操作_为预备).
** Return true 到_变量 skip the 环_变量. Otherwise,
** after preparation, 栈_圆小 will be as follows:
**   奖励_变量 : internal index (safe copy of the control variable)
**   奖励_变量 + 1 : 环_变量 counter (integer loops) or 限制_变量 (float loops)
**   奖励_变量 + 2 : 步进_变量
**   奖励_变量 + 3 : control variable
*/
static int 为预备_变量 (炉_状态机结 *L, 栈身份_型 奖励_变量) {
  标签值_结 *形参初始_变量 = 对象_s到v_宏名(奖励_变量);
  标签值_结 *形参限_变量 = 对象_s到v_宏名(奖励_变量 + 1);
  标签值_结 *形参步进_变量 = 对象_s到v_宏名(奖励_变量 + 2);
  if (对象_tt是否整数_宏名(形参初始_变量) && 对象_tt是否整数_宏名(形参步进_变量)) { /* integer 环_变量? */
    炉_整数型 初始的_变量 = 对象_整数值_宏名(形参初始_变量);
    炉_整数型 步进_变量 = 对象_整数值_宏名(形参步进_变量);
    炉_整数型 限制_变量;
    if (步进_变量 == 0)
      月亮全局_跑错误_函(L, "'for' 步进_变量 is zero");
    对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量 + 3), 初始的_变量);  /* control variable */
    if (月虚拟机_为限制_函(L, 初始的_变量, 形参限_变量, &限制_变量, 步进_变量))
      return 1;  /* skip the 环_变量 */
    else {  /* prepare 环_变量 counter */
      lua_Unsigned 计数_变量;
      if (步进_变量 > 0) {  /* ascending 环_变量? */
        计数_变量 = 限制_l_类型转换符到无符_宏名(限制_变量) - 限制_l_类型转换符到无符_宏名(初始的_变量);
        if (步进_变量 != 1)  /* avoid division in the too common case */
          计数_变量 /= 限制_l_类型转换符到无符_宏名(步进_变量);
      }
      else {  /* 步进_变量 < 0; descending 环_变量 */
        计数_变量 = 限制_l_类型转换符到无符_宏名(初始的_变量) - 限制_l_类型转换符到无符_宏名(限制_变量);
        /* '步进_变量+1' avoids negating 'mininteger' */
        计数_变量 /= 限制_l_类型转换符到无符_宏名(-(步进_变量 + 1)) + 1u;
      }
      /* store the counter in place of the 限制_变量 (which won't be
         已需要_变量 anymore) */
      对象_设置整数值_宏名(形参限_变量, 限制_l_类型转换无符到符_宏名(计数_变量));
    }
  }
  else {  /* try making all values floats */
    炉_数目型 初始的_变量; 炉_数目型 限制_变量; 炉_数目型 步进_变量;
    if (配置_l_可能性低_宏名(!虚机头_到数目_宏名(形参限_变量, &限制_变量)))
      月亮全局_for循环错误_函(L, 形参限_变量, "限制_变量");
    if (配置_l_可能性低_宏名(!虚机头_到数目_宏名(形参步进_变量, &步进_变量)))
      月亮全局_for循环错误_函(L, 形参步进_变量, "步进_变量");
    if (配置_l_可能性低_宏名(!虚机头_到数目_宏名(形参初始_变量, &初始的_变量)))
      月亮全局_for循环错误_函(L, 形参初始_变量, "initial 值_圆");
    if (步进_变量 == 0)
      月亮全局_跑错误_函(L, "'for' 步进_变量 is zero");
    if (限制_月亮i_数目小于_宏名(0, 步进_变量) ? 限制_月亮i_数目小于_宏名(限制_变量, 初始的_变量)
                            : 限制_月亮i_数目小于_宏名(初始的_变量, 限制_变量))
      return 1;  /* skip the 环_变量 */
    else {
      /* make sure internal values are all floats */
      对象_设置浮点值_宏名(形参限_变量, 限制_变量);
      对象_设置浮点值_宏名(形参步进_变量, 步进_变量);
      对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量), 初始的_变量);  /* internal index */
      对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量 + 3), 初始的_变量);  /* control variable */
    }
  }
  return 0;
}


/*
** Execute a 步进_变量 of a float numerical for 环_变量, returning
** true iff the 环_变量 must continue. (The integer case is
** written online with opcode 操作_为环, for performance.)
*/
static int 月虚拟机_浮点为环_函 (栈身份_型 奖励_变量) {
  炉_数目型 步进_变量 = 对象_浮点值_宏名(对象_s到v_宏名(奖励_变量 + 2));
  炉_数目型 限制_变量 = 对象_浮点值_宏名(对象_s到v_宏名(奖励_变量 + 1));
  炉_数目型 索引_缩变量 = 对象_浮点值_宏名(对象_s到v_宏名(奖励_变量));  /* internal index */
  索引_缩变量 = 限制_月亮i_数目加法_宏名(L, 索引_缩变量, 步进_变量);  /* increment index */
  if (限制_月亮i_数目小于_宏名(0, 步进_变量) ? 限制_月亮i_数目小等_宏名(索引_缩变量, 限制_变量)
                          : 限制_月亮i_数目小等_宏名(限制_变量, 索引_缩变量)) {
    对象_改变浮点值_宏名(对象_s到v_宏名(奖励_变量), 索引_缩变量);  /* update internal index */
    对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量 + 3), 索引_缩变量);  /* and control variable */
    return 1;  /* jump back */
  }
  else
    return 0;  /* finish the 环_变量 */
}


/*
** Finish the table access '值_变量 = t[键_小变量]'.
** if '插槽_变量' is NULL, 't' is not a table; otherwise, '插槽_变量' points 到_变量
** t[k] entry (which must be 空容器_变量).
*/
void 月亮虚拟机_完成获取_函 (炉_状态机结 *L, const 标签值_结 *t, 标签值_结 *键_小变量, 栈身份_型 值_变量,
                      const 标签值_结 *插槽_变量) {
  int 环_变量;  /* counter 到_变量 avoid infinite loops */
  const 标签值_结 *标方_缩变量;  /* metamethod */
  for (环_变量 = 0; 环_变量 < 虚拟机_最大标签环_宏名; 环_变量++) {
    if (插槽_变量 == NULL) {  /* 't' is not a table? */
      限制_月亮_断言_宏名(!对象_tt是否表_宏名(t));
      标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, t, 标方_索引大写);
      if (配置_l_可能性低_宏名(标签方法_无标方_宏名(标方_缩变量)))
        月亮全局_类型错误_函(L, t, "index");  /* no metamethod */
      /* else will try the metamethod */
    }
    else {  /* 't' is a table */
      限制_月亮_断言_宏名(对象_是否空容器_宏名(插槽_变量));
      标方_缩变量 = 标签方法_快速标方_宏名(L, 对象_哈希值_宏名(t)->元表_小写, 标方_索引大写);  /* table's metamethod */
      if (标方_缩变量 == NULL) {  /* no metamethod? */
        对象_设置空值的值_宏名(对象_s到v_宏名(值_变量));  /* 结果_变量 is nil */
        return;
      }
      /* else will try the metamethod */
    }
    if (对象_tt是否函数_宏名(标方_缩变量)) {  /* is metamethod a function? */
      月亮类型_调用标签方法并返回结果_函(L, 标方_缩变量, t, 键_小变量, 值_变量);  /* call it */
      return;
    }
    t = 标方_缩变量;  /* else try 到_变量 access '标方_缩变量[键_小变量]' */
    if (虚机头_月亮V_快速获取_宏名(L, t, 键_小变量, 插槽_变量, 月亮哈希表_获取键_函)) {  /* fast track? */
      对象_设置对象到s_宏名(L, 值_变量, 插槽_变量);  /* done */
      return;
    }
    /* else repeat (tail call '月亮虚拟机_完成获取_函') */
  }
  月亮全局_跑错误_函(L, "'__index' chain too long; possible 环_变量");
}


/*
** Finish a table assignment 't[键_小变量] = 值_变量'.
** If '插槽_变量' is NULL, 't' is not a table.  Otherwise, '插槽_变量' points
** 到_变量 the entry 't[键_小变量]', or 到_变量 a 值_圆 with an absent 键_小变量 if there
** is no such entry.  (The 值_圆 at '插槽_变量' must be 空容器_变量, otherwise
** '虚机头_月亮V_快速获取_宏名' would have done the job.)
*/
void 月亮虚拟机_完成设置_函 (炉_状态机结 *L, const 标签值_结 *t, 标签值_结 *键_小变量,
                     标签值_结 *值_变量, const 标签值_结 *插槽_变量) {
  int 环_变量;  /* counter 到_变量 avoid infinite loops */
  for (环_变量 = 0; 环_变量 < 虚拟机_最大标签环_宏名; 环_变量++) {
    const 标签值_结 *标方_缩变量;  /* '__newindex' metamethod */
    if (插槽_变量 != NULL) {  /* is 't' a table? */
      表_结 *h = 对象_哈希值_宏名(t);  /* 月词法_保存_函 't' table */
      限制_月亮_断言_宏名(对象_是否空容器_宏名(插槽_变量));  /* 插槽_变量 must be 空容器_变量 */
      标方_缩变量 = 标签方法_快速标方_宏名(L, h->元表_小写, 标方_新索引大写);  /* get metamethod */
      if (标方_缩变量 == NULL) {  /* no metamethod? */
        月亮哈希表_完成设置_函(L, h, 键_小变量, 插槽_变量, 值_变量);  /* set new 值_圆 */
        表_废弃标签方法缓存_宏名(h);
        垃圾回收_月亮C_屏障后退_宏名(L, 状态机_对象到垃圾回收对象_宏名(h), 值_变量);
        return;
      }
      /* else will try the metamethod */
    }
    else {  /* not a table; 月解析器_检查_函 metamethod */
      标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, t, 标方_新索引大写);
      if (配置_l_可能性低_宏名(标签方法_无标方_宏名(标方_缩变量)))
        月亮全局_类型错误_函(L, t, "index");
    }
    /* try the metamethod */
    if (对象_tt是否函数_宏名(标方_缩变量)) {
      月亮类型_调用标签方法_函(L, 标方_缩变量, t, 键_小变量, 值_变量);
      return;
    }
    t = 标方_缩变量;  /* else repeat assignment over '标方_缩变量' */
    if (虚机头_月亮V_快速获取_宏名(L, t, 键_小变量, 插槽_变量, 月亮哈希表_获取键_函)) {
      虚机头_月亮V_结束快速设置_宏名(L, t, 插槽_变量, 值_变量);
      return;  /* done */
    }
    /* else 'return 月亮虚拟机_完成设置_函(L, t, 键_小变量, 值_变量, 插槽_变量)' (环_变量) */
  }
  月亮全局_跑错误_函(L, "'__newindex' chain too long; possible 环_变量");
}


/*
** Compare two strings '状列_缩变量' x 'rs', returning an integer less-equal-
** -greater than zero if '状列_缩变量' is less-equal-greater than 'rs'.
** The 代码_变量 is a little tricky because it allows '\0' in the strings
** and it uses 'strcoll' (到_变量 respect locales) for each segments
** of the strings.
*/
static int 月虚拟机_l_串对比_函 (const 标签字符串_结 *状列_缩变量, const 标签字符串_结 *rs) {
  const char *l = 对象_获取串_宏名(状列_缩变量);
  size_t ll = 对象_tss长度_宏名(状列_缩变量);
  const char *r = 对象_获取串_宏名(rs);
  size_t 学习率_变量 = 对象_tss长度_宏名(rs);
  for (;;) {  /* for each segment */
    int 临时_变量 = strcoll(l, r);
    if (临时_变量 != 0)  /* not equal? */
      return 临时_变量;  /* done */
    else {  /* strings are equal 上_小变量 到_变量 a '\0' */
      size_t 长度_短变量 = strlen(l);  /* index of 首先_变量 '\0' in both strings */
      if (长度_短变量 == 学习率_变量)  /* 'rs' is finished? */
        return (长度_短变量 == ll) ? 0 : 1;  /* 月解析器_检查_函 '状列_缩变量' */
      else if (长度_短变量 == ll)  /* '状列_缩变量' is finished? */
        return -1;  /* '状列_缩变量' is less than 'rs' ('rs' is not finished) */
      /* both strings longer than '长度_短变量'; go on comparing after the '\0' */
      长度_短变量++;
      l += 长度_短变量; ll -= 长度_短变量; r += 长度_短变量; 学习率_变量 -= 长度_短变量;
    }
  }
}


/*
** Check whether integer 'i' is less than float 'f'. If 'i' has an
** exact representation as a float ('虚拟机_l_整型单浮点范围_宏名'), compare numbers as
** floats. Otherwise, use the equivalence 'i < f <=> i < ceil(f)'.
** If 'ceil(f)' is out of integer range, either 'f' is greater than
** all integers or less than all integers.
** (The test with '虚拟机_l_整型单浮点范围_宏名' is only for performance; the else
** case is correct for all values, but it is slow due 到_变量 the conversion
** from float 到_变量 int.)
** When 'f' is NaN, comparisons must 结果_变量 in false.
*/
l_sinline int 月虚拟机_整型小于浮点_函 (炉_整数型 i, 炉_数目型 f) {
  if (虚拟机_l_整型单浮点范围_宏名(i))
    return 限制_月亮i_数目小于_宏名(限制_类型转换_数目_宏名(i), f);  /* compare them as floats */
  else {  /* i < f <=> i < ceil(f) */
    炉_整数型 文信_缩变量;
    if (月亮虚拟机_浮点数到整数_函(f, &文信_缩变量, 浮到整向上取整_))  /* 文信_缩变量 = ceil(f) */
      return i < 文信_缩变量;   /* compare them as integers */
    else  /* 'f' is either greater or less than all integers */
      return f > 0;  /* greater? */
  }
}


/*
** Check whether integer 'i' is less than or equal 到_变量 float 'f'.
** See comments on 前一个_变量 function.
*/
l_sinline int 月虚拟机_整型小等浮点_函 (炉_整数型 i, 炉_数目型 f) {
  if (虚拟机_l_整型单浮点范围_宏名(i))
    return 限制_月亮i_数目小等_宏名(限制_类型转换_数目_宏名(i), f);  /* compare them as floats */
  else {  /* i <= f <=> i <= floor(f) */
    炉_整数型 文信_缩变量;
    if (月亮虚拟机_浮点数到整数_函(f, &文信_缩变量, 浮到整向下取整_))  /* 文信_缩变量 = floor(f) */
      return i <= 文信_缩变量;   /* compare them as integers */
    else  /* 'f' is either greater or less than all integers */
      return f > 0;  /* greater? */
  }
}


/*
** Check whether float 'f' is less than integer 'i'.
** See comments on 前一个_变量 function.
*/
l_sinline int 月虚拟机_浮点小于整型_函 (炉_数目型 f, 炉_整数型 i) {
  if (虚拟机_l_整型单浮点范围_宏名(i))
    return 限制_月亮i_数目小于_宏名(f, 限制_类型转换_数目_宏名(i));  /* compare them as floats */
  else {  /* f < i <=> floor(f) < i */
    炉_整数型 文信_缩变量;
    if (月亮虚拟机_浮点数到整数_函(f, &文信_缩变量, 浮到整向下取整_))  /* 文信_缩变量 = floor(f) */
      return 文信_缩变量 < i;   /* compare them as integers */
    else  /* 'f' is either greater or less than all integers */
      return f < 0;  /* less? */
  }
}


/*
** Check whether float 'f' is less than or equal 到_变量 integer 'i'.
** See comments on 前一个_变量 function.
*/
l_sinline int 月虚拟机_浮点小等整型_函 (炉_数目型 f, 炉_整数型 i) {
  if (虚拟机_l_整型单浮点范围_宏名(i))
    return 限制_月亮i_数目小等_宏名(f, 限制_类型转换_数目_宏名(i));  /* compare them as floats */
  else {  /* f <= i <=> ceil(f) <= i */
    炉_整数型 文信_缩变量;
    if (月亮虚拟机_浮点数到整数_函(f, &文信_缩变量, 浮到整向上取整_))  /* 文信_缩变量 = ceil(f) */
      return 文信_缩变量 <= i;   /* compare them as integers */
    else  /* 'f' is either greater or less than all integers */
      return f < 0;  /* less? */
  }
}


/*
** Return 'l < r', for numbers.
*/
l_sinline int 月虚拟机_数目小于_函 (const 标签值_结 *l, const 标签值_结 *r) {
  限制_月亮_断言_宏名(对象_tt是否数目_宏名(l) && 对象_tt是否数目_宏名(r));
  if (对象_tt是否整数_宏名(l)) {
    炉_整数型 行信_缩变量 = 对象_整数值_宏名(l);
    if (对象_tt是否整数_宏名(r))
      return 行信_缩变量 < 对象_整数值_宏名(r);  /* both are integers */
    else  /* 'l' is int and 'r' is float */
      return 月虚拟机_整型小于浮点_函(行信_缩变量, 对象_浮点值_宏名(r));  /* l < r ? */
  }
  else {
    炉_数目型 行函_缩变量 = 对象_浮点值_宏名(l);  /* 'l' must be float */
    if (对象_tt是否浮点_宏名(r))
      return 限制_月亮i_数目小于_宏名(行函_缩变量, 对象_浮点值_宏名(r));  /* both are float */
    else  /* 'l' is float and 'r' is int */
      return 月虚拟机_浮点小于整型_函(行函_缩变量, 对象_整数值_宏名(r));
  }
}


/*
** Return 'l <= r', for numbers.
*/
l_sinline int 月虚拟机_数目小等_函 (const 标签值_结 *l, const 标签值_结 *r) {
  限制_月亮_断言_宏名(对象_tt是否数目_宏名(l) && 对象_tt是否数目_宏名(r));
  if (对象_tt是否整数_宏名(l)) {
    炉_整数型 行信_缩变量 = 对象_整数值_宏名(l);
    if (对象_tt是否整数_宏名(r))
      return 行信_缩变量 <= 对象_整数值_宏名(r);  /* both are integers */
    else  /* 'l' is int and 'r' is float */
      return 月虚拟机_整型小等浮点_函(行信_缩变量, 对象_浮点值_宏名(r));  /* l <= r ? */
  }
  else {
    炉_数目型 行函_缩变量 = 对象_浮点值_宏名(l);  /* 'l' must be float */
    if (对象_tt是否浮点_宏名(r))
      return 限制_月亮i_数目小等_宏名(行函_缩变量, 对象_浮点值_宏名(r));  /* both are float */
    else  /* 'l' is float and 'r' is int */
      return 月虚拟机_浮点小等整型_函(行函_缩变量, 对象_整数值_宏名(r));
  }
}


/*
** return 'l < r' for non-numbers.
*/
static int 月虚拟机_小于其他_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r) {
  限制_月亮_断言_宏名(!对象_tt是否数目_宏名(l) || !对象_tt是否数目_宏名(r));
  if (对象_tt是否字符串_宏名(l) && 对象_tt是否字符串_宏名(r))  /* both are strings? */
    return 月虚拟机_l_串对比_函(对象_ts值_宏名(l), 对象_ts值_宏名(r)) < 0;
  else
    return 月亮类型_调用秩序标签方法_函(L, l, r, 标方_小于大写);
}


/*
** Main operation less than; return 'l < r'.
*/
int 月亮虚拟机_小于_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r) {
  if (对象_tt是否数目_宏名(l) && 对象_tt是否数目_宏名(r))  /* both operands are numbers? */
    return 月虚拟机_数目小于_函(l, r);
  else return 月虚拟机_小于其他_函(L, l, r);
}


/*
** return 'l <= r' for non-numbers.
*/
static int 月虚拟机_小等其他_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r) {
  限制_月亮_断言_宏名(!对象_tt是否数目_宏名(l) || !对象_tt是否数目_宏名(r));
  if (对象_tt是否字符串_宏名(l) && 对象_tt是否字符串_宏名(r))  /* both are strings? */
    return 月虚拟机_l_串对比_函(对象_ts值_宏名(l), 对象_ts值_宏名(r)) <= 0;
  else
    return 月亮类型_调用秩序标签方法_函(L, l, r, 标方_小等大写);
}


/*
** Main operation less than or equal 到_变量; return 'l <= r'.
*/
int 月亮虚拟机_小于等于_函 (炉_状态机结 *L, const 标签值_结 *l, const 标签值_结 *r) {
  if (对象_tt是否数目_宏名(l) && 对象_tt是否数目_宏名(r))  /* both operands are numbers? */
    return 月虚拟机_数目小等_函(l, r);
  else return 月虚拟机_小等其他_函(L, l, r);
}


/*
** Main operation for equality of Lua values; return '临1_短变量 == 临2_短变量'.
** L == NULL means raw equality (no metamethods)
*/
int 月亮虚拟机_相等对象_函 (炉_状态机结 *L, const 标签值_结 *临1_短变量, const 标签值_结 *临2_短变量) {
  const 标签值_结 *标方_缩变量;
  if (对象_t类型标签_宏名(临1_短变量) != 对象_t类型标签_宏名(临2_短变量)) {  /* not the same variant? */
    if (对象_t类型_宏名(临1_短变量) != 对象_t类型_宏名(临2_短变量) || 对象_t类型_宏名(临1_短变量) != 月头_月亮_T数目_宏名)
      return 0;  /* only numbers can be equal with different variants */
    else {  /* two numbers with different variants */
      /* One of them is an integer. If the other does not have an
         integer 值_圆, they 月编译器_不能_函 be equal; otherwise, compare their
         integer values. */
      炉_整数型 整1_缩变量, 索2_缩变量;
      return (月亮虚拟机_非安全到整数_函(临1_短变量, &整1_缩变量, 浮到整相等_) &&
              月亮虚拟机_非安全到整数_函(临2_短变量, &索2_缩变量, 浮到整相等_) &&
              整1_缩变量 == 索2_缩变量);
    }
  }
  /* values have same type and same variant */
  switch (对象_t类型标签_宏名(临1_短变量)) {
    case 对象_月亮_V空值_宏名: case 对象_月亮_V假_宏名: case 对象_月亮_V真_宏名: return 1;
    case 对象_月亮_V数目整型_宏名: return (对象_整数值_宏名(临1_短变量) == 对象_整数值_宏名(临2_短变量));
    case 对象_月亮_V数目浮点_宏名: return 限制_月亮i_数目相等_宏名(对象_浮点值_宏名(临1_短变量), 对象_浮点值_宏名(临2_短变量));
    case 对象_月亮_V轻量用户数据_宏名: return 对象_p值_宏名(临1_短变量) == 对象_p值_宏名(临2_短变量);
    case 对象_月亮_VL闭包函_宏名: return 对象_函值_宏名(临1_短变量) == 对象_函值_宏名(临2_短变量);
    case 对象_月亮_V短型串_宏名: return 字符串_相等短型串_宏名(对象_ts值_宏名(临1_短变量), 对象_ts值_宏名(临2_短变量));
    case 对象_月亮_V长型串_宏名: return 月亮字符串_相等长型串_函(对象_ts值_宏名(临1_短变量), 对象_ts值_宏名(临2_短变量));
    case 对象_月亮_V用户数据_宏名: {
      if (对象_u值_宏名(临1_短变量) == 对象_u值_宏名(临2_短变量)) return 1;
      else if (L == NULL) return 0;
      标方_缩变量 = 标签方法_快速标方_宏名(L, 对象_u值_宏名(临1_短变量)->元表_小写, 标方_相等大写);
      if (标方_缩变量 == NULL)
        标方_缩变量 = 标签方法_快速标方_宏名(L, 对象_u值_宏名(临2_短变量)->元表_小写, 标方_相等大写);
      break;  /* will try TM */
    }
    case 对象_月亮_V表_宏名: {
      if (对象_哈希值_宏名(临1_短变量) == 对象_哈希值_宏名(临2_短变量)) return 1;
      else if (L == NULL) return 0;
      标方_缩变量 = 标签方法_快速标方_宏名(L, 对象_哈希值_宏名(临1_短变量)->元表_小写, 标方_相等大写);
      if (标方_缩变量 == NULL)
        标方_缩变量 = 标签方法_快速标方_宏名(L, 对象_哈希值_宏名(临2_短变量)->元表_小写, 标方_相等大写);
      break;  /* will try TM */
    }
    default:
      return 对象_垃圾回收值_宏名(临1_短变量) == 对象_垃圾回收值_宏名(临2_短变量);
  }
  if (标方_缩变量 == NULL)  /* no TM? */
    return 0;  /* objects are different */
  else {
    月亮类型_调用标签方法并返回结果_函(L, 标方_缩变量, 临1_短变量, 临2_短变量, L->顶部_变量.p);  /* call TM */
    return !对象_l_是否假_宏名(对象_s到v_宏名(L->顶部_变量.p));
  }
}


/* macro used by '月亮虚拟机_拼接_函' 到_变量 ensure that element at 'o' is a string */
#define 虚拟机_到字符串_宏名(L,o)  \
	(对象_tt是否字符串_宏名(o) || (虚机头_转化到串_宏名(o) && (月亮对象_到字符串_函(L, o), 1)))

#define 虚拟机_是否空串_宏名(o)	(对象_tt是否短型字符串_宏名(o) && 对象_ts值_宏名(o)->短串长_小写 == 0)

/* copy strings in 栈_圆小 from 顶部_变量 - n 上_小变量 到_变量 顶部_变量 - 1 到_变量 缓冲区_变量 */
static void 月虚拟机_复制到缓冲_函 (栈身份_型 顶部_变量, int n, char *缓冲_变量) {
  size_t 时间限_变量 = 0;  /* 大小_变量 already copied */
  do {
    size_t l = 对象_vs长度_宏名(对象_s到v_宏名(顶部_变量 - n));  /* length of string being copied */
    memcpy(缓冲_变量 + 时间限_变量, 对象_s值_宏名(对象_s到v_宏名(顶部_变量 - n)), l * sizeof(char));
    时间限_变量 += l;
  } while (--n > 0);
}


/*
** Main operation for concatenation: concat '总数_变量' values in the 栈_圆小,
** from 'L->顶部_变量.p - 总数_变量' 上_小变量 到_变量 'L->顶部_变量.p - 1'.
*/
void 月亮虚拟机_拼接_函 (炉_状态机结 *L, int 总数_变量) {
  if (总数_变量 == 1)
    return;  /* "all" values already concatenated */
  do {
    栈身份_型 顶部_变量 = L->顶部_变量.p;
    int n = 2;  /* number of elements handled in this pass (at least 2) */
    if (!(对象_tt是否字符串_宏名(对象_s到v_宏名(顶部_变量 - 2)) || 虚机头_转化到串_宏名(对象_s到v_宏名(顶部_变量 - 2))) ||
        !虚拟机_到字符串_宏名(L, 对象_s到v_宏名(顶部_变量 - 1)))
      月亮类型_尝试拼接标签方法_函(L);  /* may invalidate '顶部_变量' */
    else if (虚拟机_是否空串_宏名(对象_s到v_宏名(顶部_变量 - 1)))  /* second operand is 空容器_变量? */
      限制_类型转换_空的_宏名(虚拟机_到字符串_宏名(L, 对象_s到v_宏名(顶部_变量 - 2)));  /* 结果_变量 is 首先_变量 operand */
    else if (虚拟机_是否空串_宏名(对象_s到v_宏名(顶部_变量 - 2))) {  /* 首先_变量 operand is 空容器_变量 string? */
      对象_设置对象s到s_宏名(L, 顶部_变量 - 2, 顶部_变量 - 1);  /* 结果_变量 is second 操作_短变量. */
    }
    else {
      /* at least two non-空容器_变量 string values; get as many as possible */
      size_t 时间限_变量 = 对象_vs长度_宏名(对象_s到v_宏名(顶部_变量 - 1));
      标签字符串_结 *类s_变量;
      /* collect 总数_变量 length and number of strings */
      for (n = 1; n < 总数_变量 && 虚拟机_到字符串_宏名(L, 对象_s到v_宏名(顶部_变量 - n - 1)); n++) {
        size_t l = 对象_vs长度_宏名(对象_s到v_宏名(顶部_变量 - n - 1));
        if (配置_l_可能性低_宏名(l >= (限制_最大_大小_宏名/sizeof(char)) - 时间限_变量)) {
          L->顶部_变量.p = 顶部_变量 - 总数_变量;  /* pop strings 到_变量 avoid wasting 栈_圆小 */
          月亮全局_跑错误_函(L, "string length overflow");
        }
        时间限_变量 += l;
      }
      if (时间限_变量 <= 限制_月亮I_最大短型长度_宏名) {  /* is 结果_变量 a short string? */
        char 缓冲_变量[限制_月亮I_最大短型长度_宏名];
        月虚拟机_复制到缓冲_函(顶部_变量, n, 缓冲_变量);  /* copy strings 到_变量 缓冲区_变量 */
        类s_变量 = 月亮字符串_新长串_函(L, 缓冲_变量, 时间限_变量);
      }
      else {  /* long string; copy strings directly 到_变量 最终_变量 结果_变量 */
        类s_变量 = 月亮字符串_创建长型串对象_函(L, 时间限_变量);
        月虚拟机_复制到缓冲_函(顶部_变量, n, 对象_获取串_宏名(类s_变量));
      }
      对象_设置ts值到s_宏名(L, 顶部_变量 - n, 类s_变量);  /* create 结果_变量 */
    }
    总数_变量 -= n - 1;  /* got 'n' strings 到_变量 create one new */
    L->顶部_变量.p -= n - 1;  /* popped 'n' strings and 已推入_小写 one */
  } while (总数_变量 > 1);  /* repeat until only 1 结果_变量 左_圆 */
}


/*
** Main operation '奖励_变量 = #寄b_变量'.
*/
void 月亮虚拟机_对象长度_函 (炉_状态机结 *L, 栈身份_型 奖励_变量, const 标签值_结 *寄b_变量) {
  const 标签值_结 *标方_缩变量;
  switch (对象_t类型标签_宏名(寄b_变量)) {
    case 对象_月亮_V表_宏名: {
      表_结 *h = 对象_哈希值_宏名(寄b_变量);
      标方_缩变量 = 标签方法_快速标方_宏名(L, h->元表_小写, 标方_长度大写);
      if (标方_缩变量) break;  /* metamethod? break switch 到_变量 call it */
      对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 月亮哈希表_获取n_函(h));  /* else primitive 长度_短变量 */
      return;
    }
    case 对象_月亮_V短型串_宏名: {
      对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 对象_ts值_宏名(寄b_变量)->短串长_小写);
      return;
    }
    case 对象_月亮_V长型串_宏名: {
      对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 对象_ts值_宏名(寄b_变量)->u.长串长_短);
      return;
    }
    default: {  /* try metamethod */
      标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, 寄b_变量, 标方_长度大写);
      if (配置_l_可能性低_宏名(标签方法_无标方_宏名(标方_缩变量)))  /* no metamethod? */
        月亮全局_类型错误_函(L, 寄b_变量, "get length of");
      break;
    }
  }
  月亮类型_调用标签方法并返回结果_函(L, 标方_缩变量, 寄b_变量, 寄b_变量, 奖励_变量);
}


/*
** Integer division; return 'm // n', that is, floor(m/n).
** C division truncates its 结果_变量 (rounds towards zero).
** 'floor(q) == trunc(q)' when 'q >= 0' or when 'q' is integer,
** otherwise 'floor(q) == trunc(q) - 1'.
*/
炉_整数型 月亮虚拟机_整数除法_函 (炉_状态机结 *L, 炉_整数型 m, 炉_整数型 n) {
  if (配置_l_可能性低_宏名(限制_l_类型转换符到无符_宏名(n) + 1u <= 1u)) {  /* special cases: -1 or 0 */
    if (n == 0)
      月亮全局_跑错误_函(L, "attempt 到_变量 divide by zero");
    return 虚机头_整型操作_宏名(-, 0, m);   /* n==-1; avoid overflow with 0x80000...//-1 */
  }
  else {
    炉_整数型 q = m / n;  /* perform C division */
    if ((m ^ n) < 0 && m % n != 0)  /* 'm/n' would be negative non-integer? */
      q -= 1;  /* correct 结果_变量 for different rounding */
    return q;
  }
}


/*
** Integer modulus; return 'm % n'. (Assume that C '%' with
** negative operands follows C99 behavior. See 前一个_变量 comment
** about 月亮虚拟机_整数除法_函.)
*/
炉_整数型 月亮虚拟机_取模_函 (炉_状态机结 *L, 炉_整数型 m, 炉_整数型 n) {
  if (配置_l_可能性低_宏名(限制_l_类型转换符到无符_宏名(n) + 1u <= 1u)) {  /* special cases: -1 or 0 */
    if (n == 0)
      月亮全局_跑错误_函(L, "attempt 到_变量 perform 'n%%0'");
    return 0;   /* m % -1 == 0; avoid overflow with 0x80000...%-1 */
  }
  else {
    炉_整数型 r = m % n;
    if (r != 0 && (r ^ n) < 0)  /* 'm/n' would be non-integer negative? */
      r += n;  /* correct 结果_变量 for different rounding */
    return r;
  }
}


/*
** Float modulus
*/
炉_数目型 月亮虚拟机_浮点数取整_函 (炉_状态机结 *L, 炉_数目型 m, 炉_数目型 n) {
  炉_数目型 r;
  限制_月亮i_数目取模_宏名(L, m, n, r);
  return r;
}


/* number of bits in an integer */
#define 虚拟机_位数_宏名	限制_类型转换_整型_宏名(sizeof(炉_整数型) * CHAR_BIT)


/*
** Shift 左_圆 operation. (Shift 右_圆 just negates 'y'.)
*/
炉_整数型 月亮虚拟机_左移_函 (炉_整数型 x, 炉_整数型 y) {
  if (y < 0) {  /* shift 右_圆? */
    if (y <= -虚拟机_位数_宏名) return 0;
    else return 虚机头_整型操作_宏名(>>, x, -y);
  }
  else {  /* shift 左_圆 */
    if (y >= 虚拟机_位数_宏名) return 0;
    else return 虚机头_整型操作_宏名(<<, x, y);
  }
}


/*
** create a new Lua closure, push it in the 栈_圆小, and initialize
** its 上值们_小写.
*/
static void 月虚拟机_推闭包_函 (炉_状态机结 *L, 原型_结 *p, 上值_结 **encup, 栈身份_型 基本_变量,
                         栈身份_型 奖励_变量) {
  int 数量上_短变量 = p->上值大小_小写;
  上值描述_结 *上值_缩变量 = p->上值们_小写;
  int i;
  L闭包_结 *集群数_变量 = 月亮函数_新L闭包_函(L, 数量上_短变量);
  集群数_变量->p = p;
  对象_设置闭包L值到s_宏名(L, 奖励_变量, 集群数_变量);  /* 锚点_变量 new closure in 栈_圆小 */
  for (i = 0; i < 数量上_短变量; i++) {  /* fill in its 上值们_小写 */
    if (上值_缩变量[i].栈内_小写)  /* 上值_圆 refers 到_变量 local variable? */
      集群数_变量->上值们_短[i] = 月亮函数_查找上值_函(L, 基本_变量 + 上值_缩变量[i].索引_缩变量);
    else  /* get 上值_圆 from enclosing function */
      集群数_变量->上值们_短[i] = encup[上值_缩变量[i].索引_缩变量];
    垃圾回收_月亮C_对象屏障_宏名(L, 集群数_变量, 集群数_变量->上值们_短[i]);
  }
}


/*
** finish execution of an opcode interrupted by a yield
*/
void 月亮虚拟机_完成操作_函 (炉_状态机结 *L) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  栈身份_型 基本_变量 = 调信_缩变量->函_短变量.p + 1;
  Instruction 指令_短变量 = *(调信_缩变量->u.l.已保存程计_缩 - 1);  /* interrupted instruction */
  操作码_枚举 操作_短变量 = 操作码_获取_操作码_宏名(指令_短变量);
  switch (操作_短变量) {  /* finish its execution */
    case 操作_元方法二元: case 操作_元方法二元整: case 操作_元方法二元常: {
      对象_设置对象s到s_宏名(L, 基本_变量 + 操作码_获取实参_A_宏名(*(调信_缩变量->u.l.已保存程计_缩 - 2)), --L->顶部_变量.p);
      break;
    }
    case 操作_一元减号: case 操作_位非: case 操作_长度:
    case 操作_获取表上值内: case 操作_获取表: case 操作_获取整:
    case 操作_获取字段: case 操作_自身: {
      对象_设置对象s到s_宏名(L, 基本_变量 + 操作码_获取实参_A_宏名(指令_短变量), --L->顶部_变量.p);
      break;
    }
    case 操作_小于: case 操作_小等:
    case 操作_小于整: case 操作_小等整:
    case 操作_大于整: case 操作_大等整:
    case 操作_相等: {  /* note that '操作_相等整'/'操作_相等常' 月编译器_不能_函 yield */
      int 结果_短变量 = !对象_l_是否假_宏名(对象_s到v_宏名(L->顶部_变量.p - 1));
      L->顶部_变量.p--;
#if defined(配置_月亮_兼容_小于_小等_宏名)
      if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_限制相等_宏名) {  /* "<=" using "<" instead? */
        调信_缩变量->调用状态码_圆 ^= 状态机_调信状型_限制相等_宏名;  /* clear 记号_变量 */
        结果_短变量 = !结果_短变量;  /* negate 结果_变量 */
      }
#endif
      限制_月亮_断言_宏名(操作码_获取_操作码_宏名(*调信_缩变量->u.l.已保存程计_缩) == 操作_跳转);
      if (结果_短变量 != 操作码_获取实参_k_宏名(指令_短变量))  /* condition failed? */
        调信_缩变量->u.l.已保存程计_缩++;  /* skip jump instruction */
      break;
    }
    case 操作_拼接: {
      栈身份_型 顶部_变量 = L->顶部_变量.p - 1;  /* 顶部_变量 when '月亮类型_尝试拼接标签方法_函' was called */
      int a = 操作码_获取实参_A_宏名(指令_短变量);      /* 首先_变量 element 到_变量 concatenate */
      int 总数_变量 = 限制_类型转换_整型_宏名(顶部_变量 - 1 - (基本_变量 + a));  /* yet 到_变量 concatenate */
      对象_设置对象s到s_宏名(L, 顶部_变量 - 2, 顶部_变量);  /* put TM 结果_变量 in proper position */
      L->顶部_变量.p = 顶部_变量 - 1;  /* 顶部_变量 is one after 最后_变量 element (at 顶部_变量-2) */
      月亮虚拟机_拼接_函(L, 总数_变量);  /* concat them (may yield again) */
      break;
    }
    case 操作_关闭: {  /* yielded closing variables */
      调信_缩变量->u.l.已保存程计_缩--;  /* repeat instruction 到_变量 关闭_圆 other vars. */
      break;
    }
    case 操作_返回: {  /* yielded closing variables */
      栈身份_型 奖励_变量 = 基本_变量 + 操作码_获取实参_A_宏名(指令_短变量);
      /* adjust 顶部_变量 到_变量 signal correct number of returns, in case the
         return is "上_小变量 到_变量 顶部_变量" ('操作码_是否IT_宏名') */
      L->顶部_变量.p = 奖励_变量 + 调信_缩变量->u2.结果数目_变量;
      /* repeat instruction 到_变量 关闭_圆 other vars. and complete the return */
      调信_缩变量->u.l.已保存程计_缩--;
      break;
    }
    default: {
      /* only these other opcodes can yield */
      限制_月亮_断言_宏名(操作_短变量 == 操作_泛型为调用 || 操作_短变量 == 操作_调用 ||
           操作_短变量 == 操作_尾调用 || 操作_短变量 == 操作_设置表上值内 || 操作_短变量 == 操作_设置表 ||
           操作_短变量 == 操作_设置整 || 操作_短变量 == 操作_设置字段);
      break;
    }
  }
}




/*
** {==================================================================
** Macros for arithmetic/bitwise/comparison opcodes in '月亮虚拟机_执行_函'
** ===================================================================
*/

#define 虚拟机_l_整数加法_宏名(L,a,b)	虚机头_整型操作_宏名(+, a, b)
#define 虚拟机_l_整数减法_宏名(L,a,b)	虚机头_整型操作_宏名(-, a, b)
#define 虚拟机_l_整数乘法_宏名(L,a,b)	虚机头_整型操作_宏名(*, a, b)
#define 虚拟机_l_按位与_宏名(a,b)	虚机头_整型操作_宏名(&, a, b)
#define 虚拟机_l_按位或_宏名(a,b)	虚机头_整型操作_宏名(|, a, b)
#define 虚拟机_l_按位异或_宏名(a,b)	虚机头_整型操作_宏名(^, a, b)

#define 虚拟机_l_整数小于_宏名(a,b)	(a < b)
#define 虚拟机_l_整数小等_宏名(a,b)	(a <= b)
#define 虚拟机_l_整数大于_宏名(a,b)	(a > b)
#define 虚拟机_l_整数大等_宏名(a,b)	(a >= b)


/*
** Arithmetic operations with immediate operands. 'iop' is the integer
** operation, 'fop' is the float operation.
*/
#define 虚拟机_操作_整数算术_宏名(L,iop,fop) {  \
  栈身份_型 奖励_变量 = RA(i); \
  标签值_结 *变1_缩变量 = vRB(i);  \
  int 立即_变量 = 操作码_获取实参_sC_宏名(i);  \
  if (对象_tt是否整数_宏名(变1_缩变量)) {  \
    炉_整数型 初始值1_变量 = 对象_整数值_宏名(变1_缩变量);  \
    程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), iop(L, 初始值1_变量, 立即_变量));  \
  }  \
  else if (对象_tt是否浮点_宏名(变1_缩变量)) {  \
    炉_数目型 注意基_变量 = 对象_浮点值_宏名(变1_缩变量);  \
    炉_数目型 浮点立即_变量 = 限制_类型转换_数目_宏名(立即_变量);  \
    程序计数_变量++; 对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量), fop(L, 注意基_变量, 浮点立即_变量)); \
  }}


/*
** Auxiliary function for arithmetic operations over floats and others
** with two register operands.
*/
#define 虚拟机_操作_浮点算术_辅助_宏名(L,变1_缩变量,变2_缩变量,fop) {  \
  炉_数目型 数1_缩变量; 炉_数目型 数2_缩变量;  \
  if (虚机头_到数目非安全_宏名(变1_缩变量, 数1_缩变量) && 虚机头_到数目非安全_宏名(变2_缩变量, 数2_缩变量)) {  \
    程序计数_变量++; 对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量), fop(L, 数1_缩变量, 数2_缩变量));  \
  }}


/*
** Arithmetic operations over floats and others with register operands.
*/
#define 虚拟机_操作_浮点算术_宏名(L,fop) {  \
  栈身份_型 奖励_变量 = RA(i); \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = vRC(i);  \
  虚拟机_操作_浮点算术_辅助_宏名(L, 变1_缩变量, 变2_缩变量, fop); }


/*
** Arithmetic operations with K operands for floats.
*/
#define 虚拟机_操作_浮点常量算术_宏名(L,fop) {  \
  栈身份_型 奖励_变量 = RA(i); \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = KC(i); 限制_月亮_断言_宏名(对象_tt是否数目_宏名(变2_缩变量));  \
  虚拟机_操作_浮点算术_辅助_宏名(L, 变1_缩变量, 变2_缩变量, fop); }


/*
** Arithmetic operations over integers and floats.
*/
#define 虚拟机_操作_算术_辅助_宏名(L,变1_缩变量,变2_缩变量,iop,fop) {  \
  栈身份_型 奖励_变量 = RA(i); \
  if (对象_tt是否整数_宏名(变1_缩变量) && 对象_tt是否整数_宏名(变2_缩变量)) {  \
    炉_整数型 整1_缩变量 = 对象_整数值_宏名(变1_缩变量); 炉_整数型 索2_缩变量 = 对象_整数值_宏名(变2_缩变量);  \
    程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), iop(L, 整1_缩变量, 索2_缩变量));  \
  }  \
  else 虚拟机_操作_浮点算术_辅助_宏名(L, 变1_缩变量, 变2_缩变量, fop); }


/*
** Arithmetic operations with register operands.
*/
#define 虚拟机_操作_算术_宏名(L,iop,fop) {  \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = vRC(i);  \
  虚拟机_操作_算术_辅助_宏名(L, 变1_缩变量, 变2_缩变量, iop, fop); }


/*
** Arithmetic operations with K operands.
*/
#define 虚拟机_操作_常量算术_宏名(L,iop,fop) {  \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = KC(i); 限制_月亮_断言_宏名(对象_tt是否数目_宏名(变2_缩变量));  \
  虚拟机_操作_算术_辅助_宏名(L, 变1_缩变量, 变2_缩变量, iop, fop); }


/*
** Bitwise operations with constant operand.
*/
#define 虚拟机_操作_常量位运算_宏名(L,操作_短变量) {  \
  栈身份_型 奖励_变量 = RA(i); \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = KC(i);  \
  炉_整数型 整1_缩变量;  \
  炉_整数型 索2_缩变量 = 对象_整数值_宏名(变2_缩变量);  \
  if (虚机头_到整数非安全_宏名(变1_缩变量, &整1_缩变量)) {  \
    程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 操作_短变量(整1_缩变量, 索2_缩变量));  \
  }}


/*
** Bitwise operations with register operands.
*/
#define 虚拟机_操作_位运算_宏名(L,操作_短变量) {  \
  栈身份_型 奖励_变量 = RA(i); \
  标签值_结 *变1_缩变量 = vRB(i);  \
  标签值_结 *变2_缩变量 = vRC(i);  \
  炉_整数型 整1_缩变量; 炉_整数型 索2_缩变量;  \
  if (虚机头_到整数非安全_宏名(变1_缩变量, &整1_缩变量) && 虚机头_到整数非安全_宏名(变2_缩变量, &索2_缩变量)) {  \
    程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 操作_短变量(整1_缩变量, 索2_缩变量));  \
  }}


/*
** Order operations with register operands. 'opn' actually works
** for all numbers, but the fast track improves performance for
** integers.
*/
#define 虚拟机_操作_秩序_宏名(L,opi,opn,other) {  \
  栈身份_型 奖励_变量 = RA(i); \
  int 条件_变量;  \
  标签值_结 *寄b_变量 = vRB(i);  \
  if (对象_tt是否整数_宏名(对象_s到v_宏名(奖励_变量)) && 对象_tt是否整数_宏名(寄b_变量)) {  \
    炉_整数型 索引累加_变量 = 对象_整数值_宏名(对象_s到v_宏名(奖励_变量));  \
    炉_整数型 初始布尔_变量 = 对象_整数值_宏名(寄b_变量);  \
    条件_变量 = opi(索引累加_变量, 初始布尔_变量);  \
  }  \
  else if (对象_tt是否数目_宏名(对象_s到v_宏名(奖励_变量)) && 对象_tt是否数目_宏名(寄b_变量))  \
    条件_变量 = opn(对象_s到v_宏名(奖励_变量), 寄b_变量);  \
  else  \
    虚拟机_保护_宏名(条件_变量 = other(L, 对象_s到v_宏名(奖励_变量), 寄b_变量));  \
  虚拟机_做条件跳转_宏名(); }


/*
** Order operations with immediate operand. (Immediate operand is
** always small enough 到_变量 have an exact representation as a float.)
*/
#define 虚拟机_操作_整数秩序_宏名(L,opi,opf,inv,标方_缩变量) {  \
  栈身份_型 奖励_变量 = RA(i); \
  int 条件_变量;  \
  int 图像_缩变量 = 操作码_获取实参_sB_宏名(i);  \
  if (对象_tt是否整数_宏名(对象_s到v_宏名(奖励_变量)))  \
    条件_变量 = opi(对象_整数值_宏名(对象_s到v_宏名(奖励_变量)), 图像_缩变量);  \
  else if (对象_tt是否浮点_宏名(对象_s到v_宏名(奖励_变量))) {  \
    炉_数目型 标志累加_变量 = 对象_浮点值_宏名(对象_s到v_宏名(奖励_变量));  \
    炉_数目型 最终不变_变量 = 限制_类型转换_数目_宏名(图像_缩变量);  \
    条件_变量 = opf(标志累加_变量, 最终不变_变量);  \
  }  \
  else {  \
    int 是最终吗_变量 = 操作码_获取实参_C_宏名(i);  \
    虚拟机_保护_宏名(条件_变量 = 月亮类型_调用秩序索引标签方法_函(L, 对象_s到v_宏名(奖励_变量), 图像_缩变量, inv, 是最终吗_变量, 标方_缩变量));  \
  }  \
  虚拟机_做条件跳转_宏名(); }

/* }================================================================== */


/*
** {==================================================================
** Function '月亮虚拟机_执行_函': main interpreter 环_变量
** ===================================================================
*/

/*
** some macros for common tasks in '月亮虚拟机_执行_函'
*/


#define RA(i)	(基本_变量+操作码_获取实参_A_宏名(i))
#define RB(i)	(基本_变量+操作码_获取实参_B_宏名(i))
#define vRB(i)	对象_s到v_宏名(RB(i))
#define KB(i)	(k+操作码_获取实参_B_宏名(i))
#define RC(i)	(基本_变量+操作码_获取实参_C_宏名(i))
#define vRC(i)	对象_s到v_宏名(RC(i))
#define KC(i)	(k+操作码_获取实参_C_宏名(i))
#define RKC(i)	((操作码_测试实参_k_宏名(i)) ? k + 操作码_获取实参_C_宏名(i) : 对象_s到v_宏名(基本_变量 + 操作码_获取实参_C_宏名(i)))



#define 虚拟机_更新陷阱_宏名(调信_缩变量)  (陷阱_变量 = 调信_缩变量->u.l.陷阱_变量)

#define 虚拟机_更新基址_宏名(调信_缩变量)	(基本_变量 = 调信_缩变量->函_短变量.p + 1)


#define 虚拟机_更新栈_宏名(调信_缩变量)  \
	{ if (配置_l_可能性低_宏名(陷阱_变量)) { 虚拟机_更新基址_宏名(调信_缩变量); 奖励_变量 = RA(i); } }


/*
** Execute a jump instruction. The '虚拟机_更新陷阱_宏名' allows signals 到_变量 stop
** tight loops. (Without it, the local copy of '陷阱_变量' could never 改变_变量.)
*/
#define 虚拟机_做跳转_宏名(调信_缩变量,i,e)	{ 程序计数_变量 += 操作码_获取实参_sJ_宏名(i) + e; 虚拟机_更新陷阱_宏名(调信_缩变量); }


/* for test instructions, execute the jump instruction that follows it */
#define 虚拟机_做下一个跳转_宏名(调信_缩变量)	{ Instruction 未实现_缩变量 = *程序计数_变量; 虚拟机_做跳转_宏名(调信_缩变量, 未实现_缩变量, 1); }

/*
** do a conditional jump: skip 下一个_变量 instruction if '条件_变量' is not 什么_变量
** was expected (parameter 'k'), else do 下一个_变量 instruction, which must
** be a jump.
*/
#define 虚拟机_做条件跳转_宏名()	if (条件_变量 != 操作码_获取实参_k_宏名(i)) 程序计数_变量++; else 虚拟机_做下一个跳转_宏名(调信_缩变量);


/*
** Correct global '程序计数_变量'.
*/
#define 虚拟机_保存程序计数器_宏名(L)	(调信_缩变量->u.l.已保存程计_缩 = 程序计数_变量)


/*
** Whenever 代码_变量 can raise errors, the global '程序计数_变量' and the global
** '顶部_变量' must be correct 到_变量 月解释器_报告_函 occasional errors.
*/
#define 虚拟机_保存状态_宏名(L,调信_缩变量)		(虚拟机_保存程序计数器_宏名(L), L->顶部_变量.p = 调信_缩变量->顶部_变量.p)


/*
** 虚拟机_保护_宏名 代码_变量 that, in general, can raise errors, reallocate the
** 栈_圆小, and 改变_变量 the hooks.
*/
#define 虚拟机_保护_宏名(exp)  (虚拟机_保存状态_宏名(L,调信_缩变量), (exp), 虚拟机_更新陷阱_宏名(调信_缩变量))

/* special 版本_变量 that does not 改变_变量 the 顶部_变量 */
#define 虚拟机_非线性保护_宏名(exp)  (虚拟机_保存程序计数器_宏名(L), (exp), 虚拟机_更新陷阱_宏名(调信_缩变量))

/*
** 虚拟机_保护_宏名 代码_变量 that can only raise errors. (That is, it 月编译器_不能_函 改变_变量
** the 栈_圆小 or hooks.)
*/
#define 虚拟机_半保护_宏名(exp)  (虚拟机_保存状态_宏名(L,调信_缩变量), (exp))

/* 'c' is the 限制_变量 of live values in the 栈_圆小 */
#define 虚拟机_检查垃圾回收_宏名(L,c)  \
	{ 垃圾回收_月亮C_条件GC_宏名(L, (虚拟机_保存程序计数器_宏名(L), L->顶部_变量.p = (c)), \
                         虚拟机_更新陷阱_宏名(调信_缩变量)); \
           限制_月亮i_线程让步_宏名(L); }


/* fetch an instruction and prepare its execution */
#define 虚拟机_虚机拾取_宏名()	{ \
  if (配置_l_可能性低_宏名(陷阱_变量)) {  /* 栈_圆小 reallocation or hooks? */ \
    陷阱_变量 = 月亮全局_跟踪执行_函(L, 程序计数_变量);  /* handle hooks */ \
    虚拟机_更新基址_宏名(调信_缩变量);  /* correct 栈_圆小 */ \
  } \
  i = *(程序计数_变量++); \
}

#define 虚拟机_虚机调度_宏名(o)	switch(o)
#define 虚拟机_虚机情况_宏名(l)	case l:
#define 虚拟机_虚机破断_宏名		break


void 月亮虚拟机_执行_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量) {
  L闭包_结 *闭包_短变量;
  标签值_结 *k;
  栈身份_型 基本_变量;
  const Instruction *程序计数_变量;
  int 陷阱_变量;
#if 虚拟机_月亮_用_跳转表_宏名
#include "ljumptab.h"
#endif
 startfunc:
  陷阱_变量 = L->钩子掩码_圆;
 returning:  /* 陷阱_变量 already set */
  闭包_短变量 = 对象_闭包L值_宏名(对象_s到v_宏名(调信_缩变量->函_短变量.p));
  k = 闭包_短变量->p->k;
  程序计数_变量 = 调信_缩变量->u.l.已保存程计_缩;
  if (配置_l_可能性低_宏名(陷阱_变量)) {
    if (程序计数_变量 == 闭包_短变量->p->代码_变量) {  /* 首先_变量 instruction (not resuming)? */
      if (闭包_短变量->p->是否_变量实参短)
        陷阱_变量 = 0;  /* hooks will 起始_变量 after VARARGPREP instruction */
      else  /* 月解析器_检查_函 'call' 钩子_变量 */
        月亮调度_钩子调用_函(L, 调信_缩变量);
    }
    调信_缩变量->u.l.陷阱_变量 = 1;  /* assume 陷阱_变量 is on, for now */
  }
  基本_变量 = 调信_缩变量->函_短变量.p + 1;
  /* main 环_变量 of interpreter */
  for (;;) {
    Instruction i;  /* instruction being executed */
    虚拟机_虚机拾取_宏名();
    #if 0
      /* 低_变量-层级_变量 行_变量 tracing for debugging Lua */
      printf("行_变量: %d\n", 月亮全局_获取函数行号_函(闭包_短变量->p, 调试_相对指令计数器_宏名(程序计数_变量, 闭包_短变量->p)));
    #endif
    限制_月亮_断言_宏名(基本_变量 == 调信_缩变量->函_短变量.p + 1);
    限制_月亮_断言_宏名(基本_变量 <= L->顶部_变量.p && L->顶部_变量.p <= L->栈_最后圆.p);
    /* invalidate 顶部_变量 for instructions not expecting it */
    限制_月亮_断言_宏名(操作码_是否IT_宏名(i) || (限制_类型转换_空的_宏名(L->顶部_变量.p = 基本_变量), 1));
    虚拟机_虚机调度_宏名 (操作码_获取_操作码_宏名(i)) {
      虚拟机_虚机情况_宏名(操作_移) {
        栈身份_型 奖励_变量 = RA(i);
        对象_设置对象s到s_宏名(L, 奖励_变量, RB(i));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入整) {
        栈身份_型 奖励_变量 = RA(i);
        炉_整数型 b = 操作码_获取实参_sBx_宏名(i);
        对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), b);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入浮) {
        栈身份_型 奖励_变量 = RA(i);
        int b = 操作码_获取实参_sBx_宏名(i);
        对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量), 限制_类型转换_数目_宏名(b));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入常) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = k + 操作码_获取实参_Bx_宏名(i);
        对象_设置对象到s_宏名(L, 奖励_变量, 寄b_变量);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入常额外) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量;
        寄b_变量 = k + 操作码_获取实参_Ax_宏名(*程序计数_变量); 程序计数_变量++;
        对象_设置对象到s_宏名(L, 奖励_变量, 寄b_变量);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入假) {
        栈身份_型 奖励_变量 = RA(i);
        对象_设置布尔假值_宏名(对象_s到v_宏名(奖励_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入假跳过) {
        栈身份_型 奖励_变量 = RA(i);
        对象_设置布尔假值_宏名(对象_s到v_宏名(奖励_变量));
        程序计数_变量++;  /* skip 下一个_变量 instruction */
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入真) {
        栈身份_型 奖励_变量 = RA(i);
        对象_设置布尔真值_宏名(对象_s到v_宏名(奖励_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_载入空值) {
        栈身份_型 奖励_变量 = RA(i);
        int b = 操作码_获取实参_B_宏名(i);
        do {
          对象_设置空值的值_宏名(对象_s到v_宏名(奖励_变量++));
        } while (b--);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_获取上值) {
        栈身份_型 奖励_变量 = RA(i);
        int b = 操作码_获取实参_B_宏名(i);
        对象_设置对象到s_宏名(L, 奖励_变量, 闭包_短变量->上值们_短[b]->v.p);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_设置上值) {
        栈身份_型 奖励_变量 = RA(i);
        上值_结 *上值_缩变量 = 闭包_短变量->上值们_短[操作码_获取实参_B_宏名(i)];
        对象_设置对象_宏名(L, 上值_缩变量->v.p, 对象_s到v_宏名(奖励_变量));
        垃圾回收_月亮C_屏障_宏名(L, 上值_缩变量, 对象_s到v_宏名(奖励_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_获取表上值内) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *上值_变量 = 闭包_短变量->上值们_短[操作码_获取实参_B_宏名(i)]->v.p;
        标签值_结 *寄c_变量 = KC(i);
        标签字符串_结 *键_小变量 = 对象_ts值_宏名(寄c_变量);  /* 键_小变量 must be a string */
        if (虚机头_月亮V_快速获取_宏名(L, 上值_变量, 键_小变量, 插槽_变量, 月亮哈希表_获取短串键_函)) {
          对象_设置对象到s_宏名(L, 奖励_变量, 插槽_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成获取_函(L, 上值_变量, 寄c_变量, 奖励_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_获取表) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = vRB(i);
        标签值_结 *寄c_变量 = vRC(i);
        lua_Unsigned n;
        if (对象_tt是否整数_宏名(寄c_变量)  /* fast track for integers? */
            ? (限制_类型转换_空的_宏名(n = 对象_整数值_宏名(寄c_变量)), 虚机头_月亮V_快速获取索引_宏名(L, 寄b_变量, n, 插槽_变量))
            : 虚机头_月亮V_快速获取_宏名(L, 寄b_变量, 寄c_变量, 插槽_变量, 月亮哈希表_获取键_函)) {
          对象_设置对象到s_宏名(L, 奖励_变量, 插槽_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成获取_函(L, 寄b_变量, 寄c_变量, 奖励_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_获取整) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = vRB(i);
        int c = 操作码_获取实参_C_宏名(i);
        if (虚机头_月亮V_快速获取索引_宏名(L, 寄b_变量, c, 插槽_变量)) {
          对象_设置对象到s_宏名(L, 奖励_变量, 插槽_变量);
        }
        else {
          标签值_结 键_小变量;
          对象_设置整数值_宏名(&键_小变量, c);
          虚拟机_保护_宏名(月亮虚拟机_完成获取_函(L, 寄b_变量, &键_小变量, 奖励_变量, 插槽_变量));
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_获取字段) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = vRB(i);
        标签值_结 *寄c_变量 = KC(i);
        标签字符串_结 *键_小变量 = 对象_ts值_宏名(寄c_变量);  /* 键_小变量 must be a string */
        if (虚机头_月亮V_快速获取_宏名(L, 寄b_变量, 键_小变量, 插槽_变量, 月亮哈希表_获取短串键_函)) {
          对象_设置对象到s_宏名(L, 奖励_变量, 插槽_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成获取_函(L, 寄b_变量, 寄c_变量, 奖励_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_设置表上值内) {
        const 标签值_结 *插槽_变量;
        标签值_结 *上值_变量 = 闭包_短变量->上值们_短[操作码_获取实参_A_宏名(i)]->v.p;
        标签值_结 *寄b_变量 = KB(i);
        标签值_结 *寄c_变量 = RKC(i);
        标签字符串_结 *键_小变量 = 对象_ts值_宏名(寄b_变量);  /* 键_小变量 must be a string */
        if (虚机头_月亮V_快速获取_宏名(L, 上值_变量, 键_小变量, 插槽_变量, 月亮哈希表_获取短串键_函)) {
          虚机头_月亮V_结束快速设置_宏名(L, 上值_变量, 插槽_变量, 寄c_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成设置_函(L, 上值_变量, 寄b_变量, 寄c_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_设置表) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = vRB(i);  /* 键_小变量 (table is in '奖励_变量') */
        标签值_结 *寄c_变量 = RKC(i);  /* 值_圆 */
        lua_Unsigned n;
        if (对象_tt是否整数_宏名(寄b_变量)  /* fast track for integers? */
            ? (限制_类型转换_空的_宏名(n = 对象_整数值_宏名(寄b_变量)), 虚机头_月亮V_快速获取索引_宏名(L, 对象_s到v_宏名(奖励_变量), n, 插槽_变量))
            : 虚机头_月亮V_快速获取_宏名(L, 对象_s到v_宏名(奖励_变量), 寄b_变量, 插槽_变量, 月亮哈希表_获取键_函)) {
          虚机头_月亮V_结束快速设置_宏名(L, 对象_s到v_宏名(奖励_变量), 插槽_变量, 寄c_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成设置_函(L, 对象_s到v_宏名(奖励_变量), 寄b_变量, 寄c_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_设置整) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        int c = 操作码_获取实参_B_宏名(i);
        标签值_结 *寄c_变量 = RKC(i);
        if (虚机头_月亮V_快速获取索引_宏名(L, 对象_s到v_宏名(奖励_变量), c, 插槽_变量)) {
          虚机头_月亮V_结束快速设置_宏名(L, 对象_s到v_宏名(奖励_变量), 插槽_变量, 寄c_变量);
        }
        else {
          标签值_结 键_小变量;
          对象_设置整数值_宏名(&键_小变量, c);
          虚拟机_保护_宏名(月亮虚拟机_完成设置_函(L, 对象_s到v_宏名(奖励_变量), &键_小变量, 寄c_变量, 插槽_变量));
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_设置字段) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = KB(i);
        标签值_结 *寄c_变量 = RKC(i);
        标签字符串_结 *键_小变量 = 对象_ts值_宏名(寄b_变量);  /* 键_小变量 must be a string */
        if (虚机头_月亮V_快速获取_宏名(L, 对象_s到v_宏名(奖励_变量), 键_小变量, 插槽_变量, 月亮哈希表_获取短串键_函)) {
          虚机头_月亮V_结束快速设置_宏名(L, 对象_s到v_宏名(奖励_变量), 插槽_变量, 寄c_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成设置_函(L, 对象_s到v_宏名(奖励_变量), 寄b_变量, 寄c_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_新表) {
        栈身份_型 奖励_变量 = RA(i);
        int b = 操作码_获取实参_B_宏名(i);  /* log2(哈希_小写 大小_变量) + 1 */
        int c = 操作码_获取实参_C_宏名(i);  /* 数组_圆 大小_变量 */
        表_结 *t;
        if (b > 0)
          b = 1 << (b - 1);  /* 大小_变量 is 2^(b - 1) */
        限制_月亮_断言_宏名((!操作码_测试实参_k_宏名(i)) == (操作码_获取实参_Ax_宏名(*程序计数_变量) == 0));
        if (操作码_测试实参_k_宏名(i))  /* non-zero 额外_变量 argument? */
          c += 操作码_获取实参_Ax_宏名(*程序计数_变量) * (操作码_最大实参_C_宏名 + 1);  /* add it 到_变量 大小_变量 */
        程序计数_变量++;  /* skip 额外_变量 argument */
        L->顶部_变量.p = 奖励_变量 + 1;  /* correct 顶部_变量 in case of emergency GC */
        t = 月亮哈希表_新_函(L);  /* memory allocation */
        对象_设置哈希值到s_宏名(L, 奖励_变量, t);
        if (b != 0 || c != 0)
          月亮哈希表_调整大小_函(L, t, c, b);  /* idem */
        虚拟机_检查垃圾回收_宏名(L, 奖励_变量 + 1);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_自身) {
        栈身份_型 奖励_变量 = RA(i);
        const 标签值_结 *插槽_变量;
        标签值_结 *寄b_变量 = vRB(i);
        标签值_结 *寄c_变量 = RKC(i);
        标签字符串_结 *键_小变量 = 对象_ts值_宏名(寄c_变量);  /* 键_小变量 must be a string */
        对象_设置对象到s_宏名(L, 奖励_变量 + 1, 寄b_变量);
        if (虚机头_月亮V_快速获取_宏名(L, 寄b_变量, 键_小变量, 插槽_变量, 月亮哈希表_获取串键_函)) {
          对象_设置对象到s_宏名(L, 奖励_变量, 插槽_变量);
        }
        else
          虚拟机_保护_宏名(月亮虚拟机_完成获取_函(L, 寄b_变量, 寄c_变量, 奖励_变量, 插槽_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_加整) {
        虚拟机_操作_整数算术_宏名(L, 虚拟机_l_整数加法_宏名, 限制_月亮i_数目加法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_加常) {
        虚拟机_操作_常量算术_宏名(L, 虚拟机_l_整数加法_宏名, 限制_月亮i_数目加法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_减常) {
        虚拟机_操作_常量算术_宏名(L, 虚拟机_l_整数减法_宏名, 限制_月亮i_数目减法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_乘常) {
        虚拟机_操作_常量算术_宏名(L, 虚拟机_l_整数乘法_宏名, 限制_月亮i_数目乘法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_取模常) {
        虚拟机_保存状态_宏名(L, 调信_缩变量);  /* in case of division by 0 */
        虚拟机_操作_常量算术_宏名(L, 月亮虚拟机_取模_函, 月亮虚拟机_浮点数取整_函);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_幂运算常) {
        虚拟机_操作_浮点常量算术_宏名(L, 限制_月亮i_数目幂_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_除常) {
        虚拟机_操作_浮点常量算术_宏名(L, 限制_月亮i_数目除法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_整数除常) {
        虚拟机_保存状态_宏名(L, 调信_缩变量);  /* in case of division by 0 */
        虚拟机_操作_常量算术_宏名(L, 月亮虚拟机_整数除法_函, 限制_月亮i_数目整数除法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位与常) {
        虚拟机_操作_常量位运算_宏名(L, 虚拟机_l_按位与_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位或常) {
        虚拟机_操作_常量位运算_宏名(L, 虚拟机_l_按位或_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位异或常) {
        虚拟机_操作_常量位运算_宏名(L, 虚拟机_l_按位异或_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_右移整) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        int 递增计数_变量 = 操作码_获取实参_sC_宏名(i);
        炉_整数型 初始布尔_变量;
        if (虚机头_到整数非安全_宏名(寄b_变量, &初始布尔_变量)) {
          程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 月亮虚拟机_左移_函(初始布尔_变量, -递增计数_变量));
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_左移整) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        int 递增计数_变量 = 操作码_获取实参_sC_宏名(i);
        炉_整数型 初始布尔_变量;
        if (虚机头_到整数非安全_宏名(寄b_变量, &初始布尔_变量)) {
          程序计数_变量++; 对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 月亮虚拟机_左移_函(递增计数_变量, 初始布尔_变量));
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_加) {
        虚拟机_操作_算术_宏名(L, 虚拟机_l_整数加法_宏名, 限制_月亮i_数目加法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_减) {
        虚拟机_操作_算术_宏名(L, 虚拟机_l_整数减法_宏名, 限制_月亮i_数目减法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_乘) {
        虚拟机_操作_算术_宏名(L, 虚拟机_l_整数乘法_宏名, 限制_月亮i_数目乘法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_取模) {
        虚拟机_保存状态_宏名(L, 调信_缩变量);  /* in case of division by 0 */
        虚拟机_操作_算术_宏名(L, 月亮虚拟机_取模_函, 月亮虚拟机_浮点数取整_函);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_幂运算) {
        虚拟机_操作_浮点算术_宏名(L, 限制_月亮i_数目幂_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_除) {  /* float division (always with floats) */
        虚拟机_操作_浮点算术_宏名(L, 限制_月亮i_数目除法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_整数除) {  /* floor division */
        虚拟机_保存状态_宏名(L, 调信_缩变量);  /* in case of division by 0 */
        虚拟机_操作_算术_宏名(L, 月亮虚拟机_整数除法_函, 限制_月亮i_数目整数除法_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位与) {
        虚拟机_操作_位运算_宏名(L, 虚拟机_l_按位与_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位或) {
        虚拟机_操作_位运算_宏名(L, 虚拟机_l_按位或_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位异或) {
        虚拟机_操作_位运算_宏名(L, 虚拟机_l_按位异或_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_右移) {
        虚拟机_操作_位运算_宏名(L, 虚机头_月亮V_右移_宏名);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_左移) {
        虚拟机_操作_位运算_宏名(L, 月亮虚拟机_左移_函);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_元方法二元) {
        栈身份_型 奖励_变量 = RA(i);
        Instruction 圆周率_变量 = *(程序计数_变量 - 2);  /* original 月串库_算术_函. expression */
        标签值_结 *寄b_变量 = vRB(i);
        标方符_枚举 标方_缩变量 = (标方符_枚举)操作码_获取实参_C_宏名(i);
        栈身份_型 结果_变量 = RA(圆周率_变量);
        限制_月亮_断言_宏名(操作_加 <= 操作码_获取_操作码_宏名(圆周率_变量) && 操作码_获取_操作码_宏名(圆周率_变量) <= 操作_右移);
        虚拟机_保护_宏名(月亮类型_尝试二元标签方法_函(L, 对象_s到v_宏名(奖励_变量), 寄b_变量, 结果_变量, 标方_缩变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_元方法二元整) {
        栈身份_型 奖励_变量 = RA(i);
        Instruction 圆周率_变量 = *(程序计数_变量 - 2);  /* original 月串库_算术_函. expression */
        int 立即_变量 = 操作码_获取实参_sB_宏名(i);
        标方符_枚举 标方_缩变量 = (标方符_枚举)操作码_获取实参_C_宏名(i);
        int 折叠_变量 = 操作码_获取实参_k_宏名(i);
        栈身份_型 结果_变量 = RA(圆周率_变量);
        虚拟机_保护_宏名(月亮类型_尝试索引二元标签方法_函(L, 对象_s到v_宏名(奖励_变量), 立即_变量, 折叠_变量, 结果_变量, 标方_缩变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_元方法二元常) {
        栈身份_型 奖励_变量 = RA(i);
        Instruction 圆周率_变量 = *(程序计数_变量 - 2);  /* original 月串库_算术_函. expression */
        标签值_结 *立即_变量 = KB(i);
        标方符_枚举 标方_缩变量 = (标方符_枚举)操作码_获取实参_C_宏名(i);
        int 折叠_变量 = 操作码_获取实参_k_宏名(i);
        栈身份_型 结果_变量 = RA(圆周率_变量);
        虚拟机_保护_宏名(月亮类型_尝试关联二元标签方法_函(L, 对象_s到v_宏名(奖励_变量), 立即_变量, 折叠_变量, 结果_变量, 标方_缩变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_一元减号) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        炉_数目型 注意基_变量;
        if (对象_tt是否整数_宏名(寄b_变量)) {
          炉_整数型 初始布尔_变量 = 对象_整数值_宏名(寄b_变量);
          对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 虚机头_整型操作_宏名(-, 0, 初始布尔_变量));
        }
        else if (虚机头_到数目非安全_宏名(寄b_变量, 注意基_变量)) {
          对象_设置浮点值_宏名(对象_s到v_宏名(奖励_变量), 限制_月亮i_数目取负_宏名(L, 注意基_变量));
        }
        else
          虚拟机_保护_宏名(月亮类型_尝试二元标签方法_函(L, 寄b_变量, 寄b_变量, 奖励_变量, 标方_一元减号大写));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_位非) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        炉_整数型 初始布尔_变量;
        if (虚机头_到整数非安全_宏名(寄b_变量, &初始布尔_变量)) {
          对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量), 虚机头_整型操作_宏名(^, ~限制_l_类型转换符到无符_宏名(0), 初始布尔_变量));
        }
        else
          虚拟机_保护_宏名(月亮类型_尝试二元标签方法_函(L, 寄b_变量, 寄b_变量, 奖励_变量, 标方_位非大写));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_非) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        if (对象_l_是否假_宏名(寄b_变量))
          对象_设置布尔真值_宏名(对象_s到v_宏名(奖励_变量));
        else
          对象_设置布尔假值_宏名(对象_s到v_宏名(奖励_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_长度) {
        栈身份_型 奖励_变量 = RA(i);
        虚拟机_保护_宏名(月亮虚拟机_对象长度_函(L, 奖励_变量, vRB(i)));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_拼接) {
        栈身份_型 奖励_变量 = RA(i);
        int n = 操作码_获取实参_B_宏名(i);  /* number of elements 到_变量 concatenate */
        L->顶部_变量.p = 奖励_变量 + n;  /* 记号_变量 the 终_变量 of concat operands */
        虚拟机_非线性保护_宏名(月亮虚拟机_拼接_函(L, n));
        虚拟机_检查垃圾回收_宏名(L, L->顶部_变量.p); /* '月亮虚拟机_拼接_函' ensures correct 顶部_变量 */
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_关闭) {
        栈身份_型 奖励_变量 = RA(i);
        虚拟机_保护_宏名(月亮函数_关闭_函(L, 奖励_变量, LUA_OK, 1));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_待关闭) {
        栈身份_型 奖励_变量 = RA(i);
        /* create new 到_变量-be-closed 上值_圆 */
        虚拟机_半保护_宏名(月亮函数_新待关闭上值_函(L, 奖励_变量));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_跳转) {
        虚拟机_做跳转_宏名(调信_缩变量, i, 0);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_相等) {
        栈身份_型 奖励_变量 = RA(i);
        int 条件_变量;
        标签值_结 *寄b_变量 = vRB(i);
        虚拟机_保护_宏名(条件_变量 = 月亮虚拟机_相等对象_函(L, 对象_s到v_宏名(奖励_变量), 寄b_变量));
        虚拟机_做条件跳转_宏名();
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_小于) {
        虚拟机_操作_秩序_宏名(L, 虚拟机_l_整数小于_宏名, 月虚拟机_数目小于_函, 月虚拟机_小于其他_函);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_小等) {
        虚拟机_操作_秩序_宏名(L, 虚拟机_l_整数小等_宏名, 月虚拟机_数目小等_函, 月虚拟机_小等其他_函);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_相等常) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = KB(i);
        /* basic types do not use '__eq'; we can use raw equality */
        int 条件_变量 = 虚机头_月亮V_原始相等对象_宏名(对象_s到v_宏名(奖励_变量), 寄b_变量);
        虚拟机_做条件跳转_宏名();
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_相等整) {
        栈身份_型 奖励_变量 = RA(i);
        int 条件_变量;
        int 图像_缩变量 = 操作码_获取实参_sB_宏名(i);
        if (对象_tt是否整数_宏名(对象_s到v_宏名(奖励_变量)))
          条件_变量 = (对象_整数值_宏名(对象_s到v_宏名(奖励_变量)) == 图像_缩变量);
        else if (对象_tt是否浮点_宏名(对象_s到v_宏名(奖励_变量)))
          条件_变量 = 限制_月亮i_数目相等_宏名(对象_浮点值_宏名(对象_s到v_宏名(奖励_变量)), 限制_类型转换_数目_宏名(图像_缩变量));
        else
          条件_变量 = 0;  /* other types 月编译器_不能_函 be equal 到_变量 a number */
        虚拟机_做条件跳转_宏名();
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_小于整) {
        虚拟机_操作_整数秩序_宏名(L, 虚拟机_l_整数小于_宏名, 限制_月亮i_数目小于_宏名, 0, 标方_小于大写);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_小等整) {
        虚拟机_操作_整数秩序_宏名(L, 虚拟机_l_整数小等_宏名, 限制_月亮i_数目小等_宏名, 0, 标方_小等大写);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_大于整) {
        虚拟机_操作_整数秩序_宏名(L, 虚拟机_l_整数大于_宏名, 限制_月亮i_数目大于_宏名, 1, 标方_小于大写);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_大等整) {
        虚拟机_操作_整数秩序_宏名(L, 虚拟机_l_整数大等_宏名, 限制_月亮i_数目大等_宏名, 1, 标方_小等大写);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_测试) {
        栈身份_型 奖励_变量 = RA(i);
        int 条件_变量 = !对象_l_是否假_宏名(对象_s到v_宏名(奖励_变量));
        虚拟机_做条件跳转_宏名();
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_测试设置) {
        栈身份_型 奖励_变量 = RA(i);
        标签值_结 *寄b_变量 = vRB(i);
        if (对象_l_是否假_宏名(寄b_变量) == 操作码_获取实参_k_宏名(i))
          程序计数_变量++;
        else {
          对象_设置对象到s_宏名(L, 奖励_变量, 寄b_变量);
          虚拟机_做下一个跳转_宏名(调信_缩变量);
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_调用) {
        栈身份_型 奖励_变量 = RA(i);
        调用信息_结 *新调信_变量;
        int b = 操作码_获取实参_B_宏名(i);
        int 结果数目_变量 = 操作码_获取实参_C_宏名(i) - 1;
        if (b != 0)  /* fixed number of arguments? */
          L->顶部_变量.p = 奖励_变量 + b;  /* 顶部_变量 signals number of arguments */
        /* else 前一个_变量 instruction set 顶部_变量 */
        虚拟机_保存程序计数器_宏名(L);  /* in case of errors */
        if ((新调信_变量 = 月亮调度_预备调用_函(L, 奖励_变量, 结果数目_变量)) == NULL)
          虚拟机_更新陷阱_宏名(调信_缩变量);  /* C call; nothing else 到_变量 be done */
        else {  /* Lua call: run function in this same C frame */
          调信_缩变量 = 新调信_变量;
          goto startfunc;
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_尾调用) {
        栈身份_型 奖励_变量 = RA(i);
        int b = 操作码_获取实参_B_宏名(i);  /* number of arguments + 1 (function) */
        int n;  /* number of results when calling a C function */
        int 形参数1_变量 = 操作码_获取实参_C_宏名(i);
        /* 德尔塔_变量 is virtual '函_短变量' - real '函_短变量' (vararg functions) */
        int 德尔塔_变量 = (形参数1_变量) ? 调信_缩变量->u.l.额外实参数_缩 + 形参数1_变量 : 0;
        if (b != 0)
          L->顶部_变量.p = 奖励_变量 + b;
        else  /* 前一个_变量 instruction set 顶部_变量 */
          b = 限制_类型转换_整型_宏名(L->顶部_变量.p - 奖励_变量);
        虚拟机_保存程序计数器_宏名(调信_缩变量);  /* several calls here can raise errors */
        if (操作码_测试实参_k_宏名(i)) {
          月亮函数_关闭上值_函(L, 基本_变量);  /* 关闭_圆 上值们_小写 from 当前_圆 call */
          限制_月亮_断言_宏名(L->待关闭列表_结.p < 基本_变量);  /* no pending 待关闭_缩变量 variables */
          限制_月亮_断言_宏名(基本_变量 == 调信_缩变量->函_短变量.p + 1);
        }
        if ((n = 月亮调度_预备尾调用_函(L, 调信_缩变量, 奖励_变量, b, 德尔塔_变量)) < 0)  /* Lua function? */
          goto startfunc;  /* execute the callee */
        else {  /* C function? */
          调信_缩变量->函_短变量.p -= 德尔塔_变量;  /* restore '函_短变量' (if vararg) */
          月亮调度_后处理调用_函(L, 调信_缩变量, n);  /* finish caller */
          虚拟机_更新陷阱_宏名(调信_缩变量);  /* '月亮调度_后处理调用_函' can 改变_变量 hooks */
          goto 返回_短变量;  /* caller returns after the tail call */
        }
      }
      虚拟机_虚机情况_宏名(操作_返回) {
        栈身份_型 奖励_变量 = RA(i);
        int n = 操作码_获取实参_B_宏名(i) - 1;  /* number of results */
        int 形参数1_变量 = 操作码_获取实参_C_宏名(i);
        if (n < 0)  /* not fixed? */
          n = 限制_类型转换_整型_宏名(L->顶部_变量.p - 奖励_变量);  /* get 什么_变量 is available */
        虚拟机_保存程序计数器_宏名(调信_缩变量);
        if (操作码_测试实参_k_宏名(i)) {  /* may there be 打开_圆 上值们_小写? */
          调信_缩变量->u2.结果数目_变量 = n;  /* 月词法_保存_函 number of returns */
          if (L->顶部_变量.p < 调信_缩变量->顶部_变量.p)
            L->顶部_变量.p = 调信_缩变量->顶部_变量.p;
          月亮函数_关闭_函(L, 基本_变量, 函_关闭栈顶_宏名, 1);
          虚拟机_更新陷阱_宏名(调信_缩变量);
          虚拟机_更新栈_宏名(调信_缩变量);
        }
        if (形参数1_变量)  /* vararg function? */
          调信_缩变量->函_短变量.p -= 调信_缩变量->u.l.额外实参数_缩 + 形参数1_变量;
        L->顶部_变量.p = 奖励_变量 + n;  /* set call for '月亮调度_后处理调用_函' */
        月亮调度_后处理调用_函(L, 调信_缩变量, n);
        虚拟机_更新陷阱_宏名(调信_缩变量);  /* '月亮调度_后处理调用_函' can 改变_变量 hooks */
        goto 返回_短变量;
      }
      虚拟机_虚机情况_宏名(操作_返回0) {
        if (配置_l_可能性低_宏名(L->钩子掩码_圆)) {
          栈身份_型 奖励_变量 = RA(i);
          L->顶部_变量.p = 奖励_变量;
          虚拟机_保存程序计数器_宏名(调信_缩变量);
          月亮调度_后处理调用_函(L, 调信_缩变量, 0);  /* no hurry... */
          陷阱_变量 = 1;
        }
        else {  /* do the 'poscall' here */
          int 结果数目_变量;
          L->调信_缩变量 = 调信_缩变量->前一个_变量;  /* back 到_变量 caller */
          L->顶部_变量.p = 基本_变量 - 1;
          for (结果数目_变量 = 调信_缩变量->结果数目_变量; 配置_l_可能性低_宏名(结果数目_变量 > 0); 结果数目_变量--)
            对象_设置空值的值_宏名(对象_s到v_宏名(L->顶部_变量.p++));  /* all results are nil */
        }
        goto 返回_短变量;
      }
      虚拟机_虚机情况_宏名(操作_返回1) {
        if (配置_l_可能性低_宏名(L->钩子掩码_圆)) {
          栈身份_型 奖励_变量 = RA(i);
          L->顶部_变量.p = 奖励_变量 + 1;
          虚拟机_保存程序计数器_宏名(调信_缩变量);
          月亮调度_后处理调用_函(L, 调信_缩变量, 1);  /* no hurry... */
          陷阱_变量 = 1;
        }
        else {  /* do the 'poscall' here */
          int 结果数目_变量 = 调信_缩变量->结果数目_变量;
          L->调信_缩变量 = 调信_缩变量->前一个_变量;  /* back 到_变量 caller */
          if (结果数目_变量 == 0)
            L->顶部_变量.p = 基本_变量 - 1;  /* asked for no results */
          else {
            栈身份_型 奖励_变量 = RA(i);
            对象_设置对象s到s_宏名(L, 基本_变量 - 1, 奖励_变量);  /* at least this 结果_变量 */
            L->顶部_变量.p = 基本_变量;
            for (; 配置_l_可能性低_宏名(结果数目_变量 > 1); 结果数目_变量--)
              对象_设置空值的值_宏名(对象_s到v_宏名(L->顶部_变量.p++));  /* complete missing results */
          }
        }
       返回_短变量:  /* return from a Lua function */
        if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_新鲜_宏名)
          return;  /* 终_变量 this frame */
        else {
          调信_缩变量 = 调信_缩变量->前一个_变量;
          goto returning;  /* continue running caller in this frame */
        }
      }
      虚拟机_虚机情况_宏名(操作_为环) {
        栈身份_型 奖励_变量 = RA(i);
        if (对象_tt是否整数_宏名(对象_s到v_宏名(奖励_变量 + 2))) {  /* integer 环_变量? */
          lua_Unsigned 计数_变量 = 限制_l_类型转换符到无符_宏名(对象_整数值_宏名(对象_s到v_宏名(奖励_变量 + 1)));
          if (计数_变量 > 0) {  /* still 更多_变量 iterations? */
            炉_整数型 步进_变量 = 对象_整数值_宏名(对象_s到v_宏名(奖励_变量 + 2));
            炉_整数型 索引_缩变量 = 对象_整数值_宏名(对象_s到v_宏名(奖励_变量));  /* internal index */
            对象_改变整数值_宏名(对象_s到v_宏名(奖励_变量 + 1), 计数_变量 - 1);  /* update counter */
            索引_缩变量 = 虚机头_整型操作_宏名(+, 索引_缩变量, 步进_变量);  /* add 步进_变量 到_变量 index */
            对象_改变整数值_宏名(对象_s到v_宏名(奖励_变量), 索引_缩变量);  /* update internal index */
            对象_设置整数值_宏名(对象_s到v_宏名(奖励_变量 + 3), 索引_缩变量);  /* and control variable */
            程序计数_变量 -= 操作码_获取实参_Bx_宏名(i);  /* jump back */
          }
        }
        else if (月虚拟机_浮点为环_函(奖励_变量))  /* float 环_变量 */
          程序计数_变量 -= 操作码_获取实参_Bx_宏名(i);  /* jump back */
        虚拟机_更新陷阱_宏名(调信_缩变量);  /* allows a signal 到_变量 break the 环_变量 */
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_为预备) {
        栈身份_型 奖励_变量 = RA(i);
        虚拟机_保存状态_宏名(L, 调信_缩变量);  /* in case of errors */
        if (为预备_变量(L, 奖励_变量))
          程序计数_变量 += 操作码_获取实参_Bx_宏名(i) + 1;  /* skip the 环_变量 */
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_泛型为预备) {
       栈身份_型 奖励_变量 = RA(i);
        /* create 到_变量-be-closed 上值_圆 (if 已需要_变量) */
        虚拟机_半保护_宏名(月亮函数_新待关闭上值_函(L, 奖励_变量 + 3));
        程序计数_变量 += 操作码_获取实参_Bx_宏名(i);
        i = *(程序计数_变量++);  /* go 到_变量 下一个_变量 instruction */
        限制_月亮_断言_宏名(操作码_获取_操作码_宏名(i) == 操作_泛型为调用 && 奖励_变量 == RA(i));
        goto l_tforcall;
      }
      虚拟机_虚机情况_宏名(操作_泛型为调用) {
       l_tforcall: {
        栈身份_型 奖励_变量 = RA(i);
        /* '奖励_变量' has the iterator function, '奖励_变量 + 1' has the 状态机_变量,
           '奖励_变量 + 2' has the control variable, and '奖励_变量 + 3' has the
           到_变量-be-closed variable. The call will use the 栈_圆小 after
           these values (starting at '奖励_变量 + 4')
        */
        /* push function, 状态机_变量, and control variable */
        memcpy(奖励_变量 + 4, 奖励_变量, 3 * sizeof(*奖励_变量));
        L->顶部_变量.p = 奖励_变量 + 4 + 3;
        虚拟机_非线性保护_宏名(月亮调度_调用_函(L, 奖励_变量 + 4, 操作码_获取实参_C_宏名(i)));  /* do the call */
        虚拟机_更新栈_宏名(调信_缩变量);  /* 栈_圆小 may have 已更改_变量 */
        i = *(程序计数_变量++);  /* go 到_变量 下一个_变量 instruction */
        限制_月亮_断言_宏名(操作码_获取_操作码_宏名(i) == 操作_泛型为环 && 奖励_变量 == RA(i));
        goto l_tforloop;
      }}
      虚拟机_虚机情况_宏名(操作_泛型为环) {
       l_tforloop: {
        栈身份_型 奖励_变量 = RA(i);
        if (!对象_tt是否空值_宏名(对象_s到v_宏名(奖励_变量 + 4))) {  /* continue 环_变量? */
          对象_设置对象s到s_宏名(L, 奖励_变量 + 2, 奖励_变量 + 4);  /* 月词法_保存_函 control variable */
          程序计数_变量 -= 操作码_获取实参_Bx_宏名(i);  /* jump back */
        }
        虚拟机_虚机破断_宏名;
      }}
      虚拟机_虚机情况_宏名(操作_设置列表) {
        栈身份_型 奖励_变量 = RA(i);
        int n = 操作码_获取实参_B_宏名(i);
        unsigned int 最后_变量 = 操作码_获取实参_C_宏名(i);
        表_结 *h = 对象_哈希值_宏名(对象_s到v_宏名(奖励_变量));
        if (n == 0)
          n = 限制_类型转换_整型_宏名(L->顶部_变量.p - 奖励_变量) - 1;  /* get 上_小变量 到_变量 the 顶部_变量 */
        else
          L->顶部_变量.p = 调信_缩变量->顶部_变量.p;  /* correct 顶部_变量 in case of emergency GC */
        最后_变量 += n;
        if (操作码_测试实参_k_宏名(i)) {
          最后_变量 += 操作码_获取实参_Ax_宏名(*程序计数_变量) * (操作码_最大实参_C_宏名 + 1);
          程序计数_变量++;
        }
        if (最后_变量 > 月亮哈希表_真实a大小_函(h))  /* needs 更多_变量 空间_圆? */
          月亮哈希表_调整数组大小_函(L, h, 最后_变量);  /* preallocate it at once */
        for (; n > 0; n--) {
          标签值_结 *值_变量 = 对象_s到v_宏名(奖励_变量 + n);
          对象_设置对象到t_宏名(L, &h->数组_圆[最后_变量 - 1], 值_变量);
          最后_变量--;
          垃圾回收_月亮C_屏障后退_宏名(L, 状态机_对象到垃圾回收对象_宏名(h), 值_变量);
        }
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_闭包) {
        栈身份_型 奖励_变量 = RA(i);
        原型_结 *p = 闭包_短变量->p->p[操作码_获取实参_Bx_宏名(i)];
        虚拟机_半保护_宏名(月虚拟机_推闭包_函(L, p, 闭包_短变量->上值们_短, 基本_变量, 奖励_变量));
        虚拟机_检查垃圾回收_宏名(L, 奖励_变量 + 1);
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_变量实参) {
        栈身份_型 奖励_变量 = RA(i);
        int n = 操作码_获取实参_C_宏名(i) - 1;  /* required results */
        虚拟机_保护_宏名(月亮类型_获取可变实参们_函(L, 调信_缩变量, 奖励_变量, n));
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_变量实参预备) {
        虚拟机_非线性保护_宏名(月亮类型_调整可变实参们_函(L, 操作码_获取实参_A_宏名(i), 调信_缩变量, 闭包_短变量->p));
        if (配置_l_可能性低_宏名(陷阱_变量)) {  /* 前一个_变量 "虚拟机_保护_宏名" updated 陷阱_变量 */
          月亮调度_钩子调用_函(L, 调信_缩变量);
          L->旧程计_变量 = 1;  /* 下一个_变量 opcode will be seen as a "new" 行_变量 */
        }
        虚拟机_更新基址_宏名(调信_缩变量);  /* function has new 基本_变量 after adjustment */
        虚拟机_虚机破断_宏名;
      }
      虚拟机_虚机情况_宏名(操作_额外实参) {
        限制_月亮_断言_宏名(0);
        虚拟机_虚机破断_宏名;
      }
    }
  }
}

/* }================================================================== */
