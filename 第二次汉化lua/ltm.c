/*
** $Id: ltm.c $
** Tag methods
** See Copyright Notice in lua.h
*/

#define ltm_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"


static const char u数据类型名_变量[] = "userdata";

配置_月亮I_D定义_宏名 const char *const 月亮T_类型名_变量[对象_月亮_总共类型们_宏名] = {
  "no 值_圆",
  "nil", "boolean", u数据类型名_变量, "number",
  "string", "table", "function", u数据类型名_变量, "线程_变量",
  "上值_圆", "proto" /* these 最后_变量 cases are used for tests only */
};


void 月亮类型_初始化_函 (炉_状态机结 *L) {
  static const char *const 月亮T_事件名_变量[] = {  /* ORDER TM */
    "__index", "__newindex",
    "__gc", "__mode", "__len", "__eq",
    "__add", "__sub", "__mul", "__mod", "__pow",
    "__div", "__idiv",
    "__band", "__bor", "__bxor", "__shl", "__shr",
    "__unm", "__bnot", "__lt", "__le",
    "__concat", "__call", "__close"
  };
  int i;
  for (i=0; i<标方_数目大写; i++) {
    G(L)->标方名_变量[i] = 月亮字符串_新_函(L, 月亮T_事件名_变量[i]);
    月亮编译_修复_函(L, 状态机_对象到垃圾回收对象_宏名(G(L)->标方名_变量[i]));  /* never collect these names */
  }
}


/*
** function 到_变量 be used with macro "标签方法_快速标方_宏名": optimized for absence of
** tag methods
*/
const 标签值_结 *月亮类型_获取标签方法_函 (表_结 *events, 标方符_枚举 事件_变量, 标签字符串_结 *ename) {
  const 标签值_结 *标方_缩变量 = 月亮哈希表_获取短串键_函(events, ename);
  限制_月亮_断言_宏名(事件_变量 <= 标方_相等大写);
  if (标签方法_无标方_宏名(标方_缩变量)) {  /* no tag method? */
    events->标志们_变量 |= 限制_类型转换_字节_宏名(1u<<事件_变量);  /* cache this fact */
    return NULL;
  }
  else return 标方_缩变量;
}


const 标签值_结 *月亮类型_通过对象获取标签方法_函 (炉_状态机结 *L, const 标签值_结 *o, 标方符_枚举 事件_变量) {
  表_结 *元表_缩变量;
  switch (对象_t类型_宏名(o)) {
    case 月头_月亮_T表_宏名:
      元表_缩变量 = 对象_哈希值_宏名(o)->元表_小写;
      break;
    case 月头_月亮_T用户数据_宏名:
      元表_缩变量 = 对象_u值_宏名(o)->元表_小写;
      break;
    default:
      元表_缩变量 = G(L)->元表_缩变量[对象_t类型_宏名(o)];
  }
  return (元表_缩变量 ? 月亮哈希表_获取短串键_函(元表_缩变量, G(L)->标方名_变量[事件_变量]) : &G(L)->空值的值_圆);
}


/*
** Return the 名称_变量 of the type of an object. For tables and userdata
** with 元表_小写, use their '__name' metafield, if present.
*/
const char *月亮类型_对象类型名_函 (炉_状态机结 *L, const 标签值_结 *o) {
  表_结 *元表_缩变量;
  if ((对象_tt是否表_宏名(o) && (元表_缩变量 = 对象_哈希值_宏名(o)->元表_小写) != NULL) ||
      (对象_tt是否完全用户数据_宏名(o) && (元表_缩变量 = 对象_u值_宏名(o)->元表_小写) != NULL)) {
    const 标签值_结 *名称_变量 = 月亮哈希表_获取短串键_函(元表_缩变量, 月亮字符串_新_函(L, "__name"));
    if (对象_tt是否字符串_宏名(名称_变量))  /* is '__name' a string? */
      return 对象_获取串_宏名(对象_ts值_宏名(名称_变量));  /* use it as type 名称_变量 */
  }
  return 标签方法_t类型名称_宏名(对象_t类型_宏名(o));  /* else use standard type 名称_变量 */
}


void 月亮类型_调用标签方法_函 (炉_状态机结 *L, const 标签值_结 *f, const 标签值_结 *p1,
                  const 标签值_结 *p2, const 标签值_结 *p3) {
  栈身份_型 函_短变量 = L->顶部_变量.p;
  对象_设置对象到s_宏名(L, 函_短变量, f);  /* push function (assume 状态机_额外_栈_宏名) */
  对象_设置对象到s_宏名(L, 函_短变量 + 1, p1);  /* 1st argument */
  对象_设置对象到s_宏名(L, 函_短变量 + 2, p2);  /* 2nd argument */
  对象_设置对象到s_宏名(L, 函_短变量 + 3, p3);  /* 3rd argument */
  L->顶部_变量.p = 函_短变量 + 4;
  /* metamethod may yield only when called from Lua 代码_变量 */
  if (状态机_是否月亮代码_宏名(L->调信_缩变量))
    月亮调度_调用_函(L, 函_短变量, 0);
  else
    月亮调度_调用无产出_函(L, 函_短变量, 0);
}


void 月亮类型_调用标签方法并返回结果_函 (炉_状态机结 *L, const 标签值_结 *f, const 标签值_结 *p1,
                     const 标签值_结 *p2, 栈身份_型 结果_短变量) {
  ptrdiff_t 结果_变量 = 做_保存栈_宏名(L, 结果_短变量);
  栈身份_型 函_短变量 = L->顶部_变量.p;
  对象_设置对象到s_宏名(L, 函_短变量, f);  /* push function (assume 状态机_额外_栈_宏名) */
  对象_设置对象到s_宏名(L, 函_短变量 + 1, p1);  /* 1st argument */
  对象_设置对象到s_宏名(L, 函_短变量 + 2, p2);  /* 2nd argument */
  L->顶部_变量.p += 3;
  /* metamethod may yield only when called from Lua 代码_变量 */
  if (状态机_是否月亮代码_宏名(L->调信_缩变量))
    月亮调度_调用_函(L, 函_短变量, 1);
  else
    月亮调度_调用无产出_函(L, 函_短变量, 1);
  结果_短变量 = 做_恢复栈_宏名(L, 结果_变量);
  对象_设置对象s到s_宏名(L, 结果_短变量, --L->顶部_变量.p);  /* move 结果_变量 到_变量 its place */
}


static int 月标签方法_调用二元标方_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2,
                      栈身份_型 结果_短变量, 标方符_枚举 事件_变量) {
  const 标签值_结 *标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, p1, 事件_变量);  /* try 首先_变量 operand */
  if (标签方法_无标方_宏名(标方_缩变量))
    标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, p2, 事件_变量);  /* try second operand */
  if (标签方法_无标方_宏名(标方_缩变量)) return 0;
  月亮类型_调用标签方法并返回结果_函(L, 标方_缩变量, p1, p2, 结果_短变量);
  return 1;
}


void 月亮类型_尝试二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2,
                    栈身份_型 结果_短变量, 标方符_枚举 事件_变量) {
  if (配置_l_可能性低_宏名(!月标签方法_调用二元标方_函(L, p1, p2, 结果_短变量, 事件_变量))) {
    switch (事件_变量) {
      case 标方_位与大写: case 标方_位或大写: case 标方_位异或大写:
      case 标方_左移大写: case 标方_右移大写: case 标方_位非大写: {
        if (对象_tt是否数目_宏名(p1) && 对象_tt是否数目_宏名(p2))
          月亮全局_到整型错误_函(L, p1, p2);
        else
          月亮全局_操作整型错误_函(L, p1, p2, "perform bitwise operation on");
      }
      /* calls never return, but 到_变量 avoid warnings: *//* FALLTHROUGH */
      default:
        月亮全局_操作整型错误_函(L, p1, p2, "perform arithmetic on");
    }
  }
}


void 月亮类型_尝试拼接标签方法_函 (炉_状态机结 *L) {
  栈身份_型 顶部_变量 = L->顶部_变量.p;
  if (配置_l_可能性低_宏名(!月标签方法_调用二元标方_函(L, 对象_s到v_宏名(顶部_变量 - 2), 对象_s到v_宏名(顶部_变量 - 1), 顶部_变量 - 2,
                               标方_拼接大写)))
    月亮全局_拼接错误_函(L, 对象_s到v_宏名(顶部_变量 - 2), 对象_s到v_宏名(顶部_变量 - 1));
}


void 月亮类型_尝试关联二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2,
                                       int 折叠_变量, 栈身份_型 结果_短变量, 标方符_枚举 事件_变量) {
  if (折叠_变量)
    月亮类型_尝试二元标签方法_函(L, p2, p1, 结果_短变量, 事件_变量);
  else
    月亮类型_尝试二元标签方法_函(L, p1, p2, 结果_短变量, 事件_变量);
}


void 月亮类型_尝试索引二元标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, 炉_整数型 索2_缩变量,
                                   int 折叠_变量, 栈身份_型 结果_短变量, 标方符_枚举 事件_变量) {
  标签值_结 辅助_变量;
  对象_设置整数值_宏名(&辅助_变量, 索2_缩变量);
  月亮类型_尝试关联二元标签方法_函(L, p1, &辅助_变量, 折叠_变量, 结果_短变量, 事件_变量);
}


/*
** Calls an order tag method.
** For lessequal, 配置_月亮_兼容_小于_小等_宏名 keeps compatibility with 旧_变量
** behavior: if there is no '__le', try '__lt', based on l <= r iff
** !(r < l) (assuming a 总数_变量 order). If the metamethod yields during
** this substitution, the continuation has 到_变量 know about it (到_变量 negate
** the 结果_变量 of r<l); bit 状态机_调信状型_限制相等_宏名 in the call 状态码_变量 keeps that
** information.
*/
int 月亮类型_调用秩序标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2,
                      标方符_枚举 事件_变量) {
  if (月标签方法_调用二元标方_函(L, p1, p2, L->顶部_变量.p, 事件_变量))  /* try original 事件_变量 */
    return !对象_l_是否假_宏名(对象_s到v_宏名(L->顶部_变量.p));
#if defined(配置_月亮_兼容_小于_小等_宏名)
  else if (事件_变量 == 标方_小等大写) {
      /* try '!(p2 < p1)' for '(p1 <= p2)' */
      L->调信_缩变量->调用状态码_圆 |= 状态机_调信状型_限制相等_宏名;  /* 记号_变量 it is doing 'lt' for '行扩_缩变量' */
      if (月标签方法_调用二元标方_函(L, p2, p1, L->顶部_变量.p, 标方_小于大写)) {
        L->调信_缩变量->调用状态码_圆 ^= 状态机_调信状型_限制相等_宏名;  /* clear 记号_变量 */
        return 对象_l_是否假_宏名(对象_s到v_宏名(L->顶部_变量.p));
      }
      /* else 错误_小变量 will remove this '调信_缩变量'; no need 到_变量 clear 记号_变量 */
  }
#endif
  月亮全局_秩序错误_函(L, p1, p2);  /* no metamethod found */
  return 0;  /* 到_变量 avoid warnings */
}


int 月亮类型_调用秩序索引标签方法_函 (炉_状态机结 *L, const 标签值_结 *p1, int 变2_缩变量,
                       int 折叠_变量, int 是否浮点_变量, 标方符_枚举 事件_变量) {
  标签值_结 辅助_变量; const 标签值_结 *p2;
  if (是否浮点_变量) {
    对象_设置浮点值_宏名(&辅助_变量, 限制_类型转换_数目_宏名(变2_缩变量));
  }
  else
    对象_设置整数值_宏名(&辅助_变量, 变2_缩变量);
  if (折叠_变量) {  /* arguments were exchanged? */
    p2 = p1; p1 = &辅助_变量;  /* correct them */
  }
  else
    p2 = &辅助_变量;
  return 月亮类型_调用秩序标签方法_函(L, p1, p2, 事件_变量);
}


void 月亮类型_调整可变实参们_函 (炉_状态机结 *L, int 固定形参数_变量, 调用信息_结 *调信_缩变量,
                         const 原型_结 *p) {
  int i;
  int 实际上_变量 = 限制_类型转换_整型_宏名(L->顶部_变量.p - 调信_缩变量->函_短变量.p) - 1;  /* number of arguments */
  int 额外数目_变量 = 实际上_变量 - 固定形参数_变量;  /* number of 额外_变量 arguments */
  调信_缩变量->u.l.额外实参数_缩 = 额外数目_变量;
  做_月亮D_检查栈_宏名(L, p->最大栈大小_小写 + 1);
  /* copy function 到_变量 the 顶部_变量 of the 栈_圆小 */
  对象_设置对象s到s_宏名(L, L->顶部_变量.p++, 调信_缩变量->函_短变量.p);
  /* move fixed parameters 到_变量 the 顶部_变量 of the 栈_圆小 */
  for (i = 1; i <= 固定形参数_变量; i++) {
    对象_设置对象s到s_宏名(L, L->顶部_变量.p++, 调信_缩变量->函_短变量.p + i);
    对象_设置空值的值_宏名(对象_s到v_宏名(调信_缩变量->函_短变量.p + i));  /* erase original parameter (for GC) */
  }
  调信_缩变量->函_短变量.p += 实际上_变量 + 1;
  调信_缩变量->顶部_变量.p += 实际上_变量 + 1;
  限制_月亮_断言_宏名(L->顶部_变量.p <= 调信_缩变量->顶部_变量.p && 调信_缩变量->顶部_变量.p <= L->栈_最后圆.p);
}


void 月亮类型_获取可变实参们_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, 栈身份_型 哪儿_变量, int 已想要_变量) {
  int i;
  int 额外数目_变量 = 调信_缩变量->u.l.额外实参数_缩;
  if (已想要_变量 < 0) {
    已想要_变量 = 额外数目_变量;  /* get all 额外_变量 arguments available */
    做_检查栈GC指针_宏名(L, 额外数目_变量, 哪儿_变量);  /* ensure 栈_圆小 空间_圆 */
    L->顶部_变量.p = 哪儿_变量 + 额外数目_变量;  /* 下一个_变量 instruction will need 顶部_变量 */
  }
  for (i = 0; i < 已想要_变量 && i < 额外数目_变量; i++)
    对象_设置对象s到s_宏名(L, 哪儿_变量 + i, 调信_缩变量->函_短变量.p - 额外数目_变量 + i);
  for (; i < 已想要_变量; i++)   /* complete required results with nil */
    对象_设置空值的值_宏名(对象_s到v_宏名(哪儿_变量 + i));
}

