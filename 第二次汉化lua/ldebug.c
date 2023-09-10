/*
** $Id: ldebug.c $
** Debug Interface
** See Copyright Notice in lua.h
*/

#define ldebug_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#include "lua.h"

#include "lapi.h"
#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lvm.h"



#define 调试_无月亮闭包_宏名(f)		((f) == NULL || (f)->c.类标_缩变量 == 对象_月亮_VC闭包L_宏名)


static const char *月调试_函名来自调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量,
                                                   const char **名称_变量);


static int 月调试_当前程序计数器_函 (调用信息_结 *调信_缩变量) {
  限制_月亮_断言_宏名(状态机_是否月亮_宏名(调信_缩变量));
  return 调试_相对指令计数器_宏名(调信_缩变量->u.l.已保存程计_缩, 调试_ci_函_宏名(调信_缩变量)->p);
}


/*
** Get a "基本_变量 行_变量" 到_变量 find the 行_变量 corresponding 到_变量 an instruction.
** Base lines are regularly placed at 调试_最大I宽度绝对值_宏名 intervals, so usually
** an integer division gets the 右_圆 place. When the 源_圆 file has
** large sequences of 空容器_变量/comment lines, it may need 额外_变量 entries,
** so the original 估计_变量 needs a correction.
** If the original 估计_变量 is -1, the initial 'if' ensures that the
** 'while' will run at least once.
** The assertion that the 估计_变量 is a lower bound for the correct 基本_变量
** is 有效_变量 as long as the debug 信息_短变量 has been generated with the same
** 值_圆 for 调试_最大I宽度绝对值_宏名 or smaller. (Previous releases use a little
** smaller 值_圆.)
*/
static int 月调试_获取基本行号_函 (const 原型_结 *f, int 程序计数_变量, int *基本程计_变量) {
  if (f->绝对行信息大小_小写 == 0 || 程序计数_变量 < f->绝对行信息_小写[0].程序计数_变量) {
    *基本程计_变量 = -1;  /* 起始_变量 from the beginning */
    return f->已定义行_小写;
  }
  else {
    int i = 限制_类型转换_无符整型_宏名(程序计数_变量) / 调试_最大I宽度绝对值_宏名 - 1;  /* get an 估计_变量 */
    /* 估计_变量 must be a lower bound of the correct 基本_变量 */
    限制_月亮_断言_宏名(i < 0 ||
              (i < f->绝对行信息大小_小写 && f->绝对行信息_小写[i].程序计数_变量 <= 程序计数_变量));
    while (i + 1 < f->绝对行信息大小_小写 && 程序计数_变量 >= f->绝对行信息_小写[i + 1].程序计数_变量)
      i++;  /* 低_变量 估计_变量; adjust it */
    *基本程计_变量 = f->绝对行信息_小写[i].程序计数_变量;
    return f->绝对行信息_小写[i].行_变量;
  }
}


/*
** Get the 行_变量 corresponding 到_变量 instruction '程序计数_变量' in function 'f';
** 首先_变量 gets a 基本_变量 行_变量 and from there does the increments until
** the desired instruction.
*/
int 月亮全局_获取函数行号_函 (const 原型_结 *f, int 程序计数_变量) {
  if (f->行信息_变量 == NULL)  /* no debug information? */
    return -1;
  else {
    int 基本程计_变量;
    int 基行_变量 = 月调试_获取基本行号_函(f, 程序计数_变量, &基本程计_变量);
    while (基本程计_变量++ < 程序计数_变量) {  /* walk until given instruction */
      限制_月亮_断言_宏名(f->行信息_变量[基本程计_变量] != 调试_绝对行信息_宏名);
      基行_变量 += f->行信息_变量[基本程计_变量];  /* correct 行_变量 */
    }
    return 基行_变量;
  }
}


static int 月调试_获取当前行号_函 (调用信息_结 *调信_缩变量) {
  return 月亮全局_获取函数行号_函(调试_ci_函_宏名(调信_缩变量)->p, 月调试_当前程序计数器_函(调信_缩变量));
}


/*
** Set '陷阱_变量' for all active Lua frames.
** This function can be called during a signal, under "reasonable"
** assumptions. A new '调信_缩变量' is completely linked in the 列表_变量 before it
** becomes part of the "active" 列表_变量, and we assume that pointers are
** 月垃圾回收_原子_函; see comment in 下一个_变量 function.
** (A compiler doing interprocedural optimizations could, theoretically,
** reorder memory writes in such a way that the 列表_变量 could be
** temporarily broken while inserting a new element. We simply assume it
** has no good reasons 到_变量 do that.)
*/
static void 月调试_设置陷阱_函 (调用信息_结 *调信_缩变量) {
  for (; 调信_缩变量 != NULL; 调信_缩变量 = 调信_缩变量->前一个_变量)
    if (状态机_是否月亮_宏名(调信_缩变量))
      调信_缩变量->u.l.陷阱_变量 = 1;
}


/*
** This function can be called during a signal, under "reasonable"
** assumptions.
** Fields '基本钩子计数_圆' and '钩子计数_圆' (set by '调试_重置钩子计数_宏名')
** are for debug only, and it is no problem if they get arbitrary
** values (causes at most one wrong 钩子_变量 call). '钩子掩码_圆' is an 月垃圾回收_原子_函
** 值_圆. We assume that pointers are 月垃圾回收_原子_函 too (e.g., gcc ensures that
** for all platforms 哪儿_变量 it runs). Moreover, '钩子_变量' is always checked
** before being called (see '月亮调度_钩子_函').
*/
配置_月亮_应程接_宏名 void 月亮_设置钩子_函 (炉_状态机结 *L, 炉_钩子型 函_短变量, int 掩码_变量, int 计数_变量) {
  if (函_短变量 == NULL || 掩码_变量 == 0) {  /* turn off hooks? */
    掩码_变量 = 0;
    函_短变量 = NULL;
  }
  L->钩子_变量 = 函_短变量;
  L->基本钩子计数_圆 = 计数_变量;
  调试_重置钩子计数_宏名(L);
  L->钩子掩码_圆 = 限制_类型转换_字节_宏名(掩码_变量);
  if (掩码_变量)
    月调试_设置陷阱_函(L->调信_缩变量);  /* 到_变量 trace inside '月亮虚拟机_执行_函' */
}


配置_月亮_应程接_宏名 炉_钩子型 月亮_获取钩子_函 (炉_状态机结 *L) {
  return L->钩子_变量;
}


配置_月亮_应程接_宏名 int 月亮_获取钩子掩码_函 (炉_状态机结 *L) {
  return L->钩子掩码_圆;
}


配置_月亮_应程接_宏名 int 月亮_获取钩子计数_函 (炉_状态机结 *L) {
  return L->基本钩子计数_圆;
}


配置_月亮_应程接_宏名 int 月亮_获取栈_函 (炉_状态机结 *L, int 层级_变量, 炉_调试结 *活记_缩变量) {
  int 状态码_变量;
  调用信息_结 *调信_缩变量;
  if (层级_变量 < 0) return 0;  /* invalid (negative) 层级_变量 */
  限制_月亮_锁_宏名(L);
  for (调信_缩变量 = L->调信_缩变量; 层级_变量 > 0 && 调信_缩变量 != &L->基本_调信圆缩; 调信_缩变量 = 调信_缩变量->前一个_变量)
    层级_变量--;
  if (层级_变量 == 0 && 调信_缩变量 != &L->基本_调信圆缩) {  /* 层级_变量 found? */
    状态码_变量 = 1;
    活记_缩变量->i_调信缩 = 调信_缩变量;
  }
  else 状态码_变量 = 0;  /* no such 层级_变量 */
  限制_月亮_解锁_宏名(L);
  return 状态码_变量;
}


static const char *月调试_上值名称_函 (const 原型_结 *p, int 上值_缩变量) {
  标签字符串_结 *s = 限制_检查_表达式_宏名(上值_缩变量 < p->上值大小_小写, p->上值们_小写[上值_缩变量].名称_变量);
  if (s == NULL) return "?";
  else return 对象_获取串_宏名(s);
}


static const char *月调试_找变量实参_函 (调用信息_结 *调信_缩变量, int n, 栈身份_型 *位置_缩变量) {
  if (对象_闭包L值_宏名(对象_s到v_宏名(调信_缩变量->函_短变量.p))->p->是否_变量实参短) {
    int 额外数目_变量 = 调信_缩变量->u.l.额外实参数_缩;
    if (n >= -额外数目_变量) {  /* 'n' is negative */
      *位置_缩变量 = 调信_缩变量->函_短变量.p - 额外数目_变量 - (n + 1);
      return "(vararg)";  /* generic 名称_变量 for any vararg */
    }
  }
  return NULL;  /* no such vararg */
}


const char *月亮全局_查找本地变量_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, int n, 栈身份_型 *位置_缩变量) {
  栈身份_型 基本_变量 = 调信_缩变量->函_短变量.p + 1;
  const char *名称_变量 = NULL;
  if (状态机_是否月亮_宏名(调信_缩变量)) {
    if (n < 0)  /* access 到_变量 vararg values? */
      return 月调试_找变量实参_函(调信_缩变量, n, 位置_缩变量);
    else
      名称_变量 = 月亮函数_获取本地名_函(调试_ci_函_宏名(调信_缩变量)->p, n, 月调试_当前程序计数器_函(调信_缩变量));
  }
  if (名称_变量 == NULL) {  /* no 'standard' 名称_变量? */
    栈身份_型 限制_变量 = (调信_缩变量 == L->调信_缩变量) ? L->顶部_变量.p : 调信_缩变量->下一个_变量->函_短变量.p;
    if (限制_变量 - 基本_变量 >= n && n > 0) {  /* is 'n' inside '调信_缩变量' 栈_圆小? */
      /* generic 名称_变量 for any 有效_变量 插槽_变量 */
      名称_变量 = 状态机_是否月亮_宏名(调信_缩变量) ? "(temporary)" : "(C temporary)";
    }
    else
      return NULL;  /* no 名称_变量 */
  }
  if (位置_缩变量)
    *位置_缩变量 = 基本_变量 + (n - 1);
  return 名称_变量;
}


配置_月亮_应程接_宏名 const char *月亮_获取本地变量_函 (炉_状态机结 *L, const 炉_调试结 *活记_缩变量, int n) {
  const char *名称_变量;
  限制_月亮_锁_宏名(L);
  if (活记_缩变量 == NULL) {  /* information about non-active function? */
    if (!对象_是否L函数_宏名(对象_s到v_宏名(L->顶部_变量.p - 1)))  /* not a Lua function? */
      名称_变量 = NULL;
    else  /* consider live variables at function 起始_变量 (parameters) */
      名称_变量 = 月亮函数_获取本地名_函(对象_闭包L值_宏名(对象_s到v_宏名(L->顶部_变量.p - 1))->p, n, 0);
  }
  else {  /* active function; get information through '活记_缩变量' */
    栈身份_型 位置_缩变量 = NULL;  /* 到_变量 avoid warnings */
    名称_变量 = 月亮全局_查找本地变量_函(L, 活记_缩变量->i_调信缩, n, &位置_缩变量);
    if (名称_变量) {
      对象_设置对象s到s_宏名(L, L->顶部_变量.p, 位置_缩变量);
      应程接_API_递增_顶部_宏名(L);
    }
  }
  限制_月亮_解锁_宏名(L);
  return 名称_变量;
}


配置_月亮_应程接_宏名 const char *月亮_设置本地变量_函 (炉_状态机结 *L, const 炉_调试结 *活记_缩变量, int n) {
  栈身份_型 位置_缩变量 = NULL;  /* 到_变量 avoid warnings */
  const char *名称_变量;
  限制_月亮_锁_宏名(L);
  名称_变量 = 月亮全局_查找本地变量_函(L, 活记_缩变量->i_调信缩, n, &位置_缩变量);
  if (名称_变量) {
    对象_设置对象s到s_宏名(L, 位置_缩变量, L->顶部_变量.p - 1);
    L->顶部_变量.p--;  /* pop 值_圆 */
  }
  限制_月亮_解锁_宏名(L);
  return 名称_变量;
}


static void 月调试_函信息_函 (炉_调试结 *活记_缩变量, 闭包_联 *闭包_短变量) {
  if (调试_无月亮闭包_宏名(闭包_短变量)) {
    活记_缩变量->源_圆 = "=[C]";
    活记_缩变量->源的长_短 = LL("=[C]");
    活记_缩变量->已定义行_小写 = -1;
    活记_缩变量->最后已定义行_小写 = -1;
    活记_缩变量->什么_变量 = "C";
  }
  else {
    const 原型_结 *p = 闭包_短变量->l.p;
    if (p->源_圆) {
      活记_缩变量->源_圆 = 对象_获取串_宏名(p->源_圆);
      活记_缩变量->源的长_短 = 对象_tss长度_宏名(p->源_圆);
    }
    else {
      活记_缩变量->源_圆 = "=?";
      活记_缩变量->源的长_短 = LL("=?");
    }
    活记_缩变量->已定义行_小写 = p->已定义行_小写;
    活记_缩变量->最后已定义行_小写 = p->最后已定义行_小写;
    活记_缩变量->什么_变量 = (活记_缩变量->已定义行_小写 == 0) ? "main" : "Lua";
  }
  月亮对象_大块id_函(活记_缩变量->短的_源小写, 活记_缩变量->源_圆, 活记_缩变量->源的长_短);
}


static int 月调试_下一行号_函 (const 原型_结 *p, int 当前行_变量, int 程序计数_变量) {
  if (p->行信息_变量[程序计数_变量] != 调试_绝对行信息_宏名)
    return 当前行_变量 + p->行信息_变量[程序计数_变量];
  else
    return 月亮全局_获取函数行号_函(p, 程序计数_变量);
}


static void 月调试_收集有效行号_函 (炉_状态机结 *L, 闭包_联 *f) {
  if (调试_无月亮闭包_宏名(f)) {
    对象_设置空值的值_宏名(对象_s到v_宏名(L->顶部_变量.p));
    应程接_API_递增_顶部_宏名(L);
  }
  else {
    int i;
    标签值_结 v;
    const 原型_结 *p = f->l.p;
    int 当前行_变量 = p->已定义行_小写;
    表_结 *t = 月亮哈希表_新_函(L);  /* new table 到_变量 store active lines */
    对象_设置哈希值到s_宏名(L, L->顶部_变量.p, t);  /* push it on 栈_圆小 */
    应程接_API_递增_顶部_宏名(L);
    对象_设置布尔真值_宏名(&v);  /* boolean 'true' 到_变量 be the 值_圆 of all indices */
    if (!p->是否_变量实参短)  /* regular function? */
      i = 0;  /* consider all instructions */
    else {  /* vararg function */
      限制_月亮_断言_宏名(操作码_获取_操作码_宏名(p->代码_变量[0]) == 操作_变量实参预备);
      当前行_变量 = 月调试_下一行号_函(p, 当前行_变量, 0);
      i = 1;  /* skip 首先_变量 instruction (操作_变量实参预备) */
    }
    for (; i < p->行信息大小_小写; i++) {  /* for each instruction */
      当前行_变量 = 月调试_下一行号_函(p, 当前行_变量, i);  /* get its 行_变量 */
      月亮哈希表_设置整型键_函(L, t, 当前行_变量, &v);  /* table[行_变量] = true */
    }
  }
}


static const char *月调试_获取函名_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, const char **名称_变量) {
  /* calling function is a known function? */
  if (调信_缩变量 != NULL && !(调信_缩变量->调用状态码_圆 & 状态机_调信状型_尾部_宏名))
    return 月调试_函名来自调用_函(L, 调信_缩变量->前一个_变量, 名称_变量);
  else return NULL;  /* no way 到_变量 find a 名称_变量 */
}


static int 月调试_辅助获取信息_函 (炉_状态机结 *L, const char *什么_变量, 炉_调试结 *活记_缩变量,
                       闭包_联 *f, 调用信息_结 *调信_缩变量) {
  int 状态码_变量 = 1;
  for (; *什么_变量; 什么_变量++) {
    switch (*什么_变量) {
      case 'S': {
        月调试_函信息_函(活记_缩变量, f);
        break;
      }
      case 'l': {
        活记_缩变量->当前行_变量 = (调信_缩变量 && 状态机_是否月亮_宏名(调信_缩变量)) ? 月调试_获取当前行号_函(调信_缩变量) : -1;
        break;
      }
      case 'u': {
        活记_缩变量->上值数_短缩 = (f == NULL) ? 0 : f->c.nupvalues;
        if (调试_无月亮闭包_宏名(f)) {
          活记_缩变量->是变量实参吗_变量 = 1;
          活记_缩变量->形参数量_变量 = 0;
        }
        else {
          活记_缩变量->是变量实参吗_变量 = f->l.p->是否_变量实参短;
          活记_缩变量->形参数量_变量 = f->l.p->形参数_小写;
        }
        break;
      }
      case 't': {
        活记_缩变量->是尾调用吗_圆 = (调信_缩变量) ? 调信_缩变量->调用状态码_圆 & 状态机_调信状型_尾部_宏名 : 0;
        break;
      }
      case 'n': {
        活记_缩变量->什么名_圆 = 月调试_获取函名_函(L, 调信_缩变量, &活记_缩变量->名称_变量);
        if (活记_缩变量->什么名_圆 == NULL) {
          活记_缩变量->什么名_圆 = "";  /* not found */
          活记_缩变量->名称_变量 = NULL;
        }
        break;
      }
      case 'r': {
        if (调信_缩变量 == NULL || !(调信_缩变量->调用状态码_圆 & 状态机_调信状型_传输_宏名))
          活记_缩变量->函传输_变量 = 活记_缩变量->转移数_缩 = 0;
        else {
          活记_缩变量->函传输_变量 = 调信_缩变量->u2.转移信息_结.函传输_变量;
          活记_缩变量->转移数_缩 = 调信_缩变量->u2.转移信息_结.转移数_缩;
        }
        break;
      }
      case 'L':
      case 'f':  /* handled by 月亮_获取信息_函 */
        break;
      default: 状态码_变量 = 0;  /* invalid 选项_变量 */
    }
  }
  return 状态码_变量;
}


配置_月亮_应程接_宏名 int 月亮_获取信息_函 (炉_状态机结 *L, const char *什么_变量, 炉_调试结 *活记_缩变量) {
  int 状态码_变量;
  闭包_联 *闭包_短变量;
  调用信息_结 *调信_缩变量;
  标签值_结 *函_短变量;
  限制_月亮_锁_宏名(L);
  if (*什么_变量 == '>') {
    调信_缩变量 = NULL;
    函_短变量 = 对象_s到v_宏名(L->顶部_变量.p - 1);
    限制_应程接_检查_宏名(L, 对象_tt是否函数_宏名(函_短变量), "function expected");
    什么_变量++;  /* skip the '>' */
    L->顶部_变量.p--;  /* pop function */
  }
  else {
    调信_缩变量 = 活记_缩变量->i_调信缩;
    函_短变量 = 对象_s到v_宏名(调信_缩变量->函_短变量.p);
    限制_月亮_断言_宏名(对象_tt是否函数_宏名(函_短变量));
  }
  闭包_短变量 = 对象_tt是否闭包_宏名(函_短变量) ? 对象_闭包值_宏名(函_短变量) : NULL;
  状态码_变量 = 月调试_辅助获取信息_函(L, 什么_变量, 活记_缩变量, 闭包_短变量, 调信_缩变量);
  if (strchr(什么_变量, 'f')) {
    对象_设置对象到s_宏名(L, L->顶部_变量.p, 函_短变量);
    应程接_API_递增_顶部_宏名(L);
  }
  if (strchr(什么_变量, 'L'))
    月调试_收集有效行号_函(L, 闭包_短变量);
  限制_月亮_解锁_宏名(L);
  return 状态码_变量;
}


/*
** {======================================================
** Symbolic Execution
** =======================================================
*/

static const char *月调试_获取对象名称_函 (const 原型_结 *p, int lastpc, int 寄存_短变量,
                               const char **名称_变量);


/*
** Find a "名称_变量" for the constant 'c'.
*/
static void 月调试_k名称_函 (const 原型_结 *p, int c, const char **名称_变量) {
  标签值_结 *键值_缩变量 = &p->k[c];
  *名称_变量 = (对象_tt是否字符串_宏名(键值_缩变量)) ? 对象_s值_宏名(键值_缩变量) : "?";
}


/*
** Find a "名称_变量" for the register 'c'.
*/
static void 月调试_r名称_函 (const 原型_结 *p, int 程序计数_变量, int c, const char **名称_变量) {
  const char *什么_变量 = 月调试_获取对象名称_函(p, 程序计数_变量, c, 名称_变量); /* search for 'c' */
  if (!(什么_变量 && *什么_变量 == 'c'))  /* did not find a constant 名称_变量? */
    *名称_变量 = "?";
}


/*
** Find a "名称_变量" for a 'C' 值_圆 in an RK instruction.
*/
static void 月调试_rk名称_函 (const 原型_结 *p, int 程序计数_变量, Instruction i, const char **名称_变量) {
  int c = 操作码_获取实参_C_宏名(i);  /* 键_小变量 index */
  if (操作码_获取实参_k_宏名(i))  /* is 'c' a constant? */
    月调试_k名称_函(p, c, 名称_变量);
  else  /* 'c' is a register */
    月调试_r名称_函(p, 程序计数_变量, c, 名称_变量);
}


static int 月调试_过滤程序计数器_函 (int 程序计数_变量, int 跳目标_变量) {
  if (程序计数_变量 < 跳目标_变量)  /* is 代码_变量 conditional (inside a jump)? */
    return -1;  /* 月编译器_不能_函 know who sets that register */
  else return 程序计数_变量;  /* 当前_圆 position sets that register */
}


/*
** Try 到_变量 find 最后_变量 instruction before 'lastpc' that modified register '寄存_短变量'.
*/
static int 月调试_找设置寄存器_函 (const 原型_结 *p, int lastpc, int 寄存_短变量) {
  int 程序计数_变量;
  int 设置寄存_变量 = -1;  /* keep 最后_变量 instruction that 已更改_变量 '寄存_短变量' */
  int 跳目标_变量 = 0;  /* any 代码_变量 before this address is conditional */
  if (操作码_测试MM模式_宏名(操作码_获取_操作码_宏名(p->代码_变量[lastpc])))
    lastpc--;  /* 前一个_变量 instruction was not actually executed */
  for (程序计数_变量 = 0; 程序计数_变量 < lastpc; 程序计数_变量++) {
    Instruction i = p->代码_变量[程序计数_变量];
    操作码_枚举 操作_短变量 = 操作码_获取_操作码_宏名(i);
    int a = 操作码_获取实参_A_宏名(i);
    int 改变_变量;  /* true if 当前_圆 instruction 已更改_变量 '寄存_短变量' */
    switch (操作_短变量) {
      case 操作_载入空值: {  /* set registers from 'a' 到_变量 'a+b' */
        int b = 操作码_获取实参_B_宏名(i);
        改变_变量 = (a <= 寄存_短变量 && 寄存_短变量 <= a + b);
        break;
      }
      case 操作_泛型为调用: {  /* affect all regs above its 基本_变量 */
        改变_变量 = (寄存_短变量 >= a + 2);
        break;
      }
      case 操作_调用:
      case 操作_尾调用: {  /* affect all registers above 基本_变量 */
        改变_变量 = (寄存_短变量 >= a);
        break;
      }
      case 操作_跳转: {  /* doesn't 改变_变量 registers, but changes '跳目标_变量' */
        int b = 操作码_获取实参_sJ_宏名(i);
        int 目的地_变量 = 程序计数_变量 + 1 + b;
        /* jump does not skip 'lastpc' and is larger than 当前_圆 one? */
        if (目的地_变量 <= lastpc && 目的地_变量 > 跳目标_变量)
          跳目标_变量 = 目的地_变量;  /* update '跳目标_变量' */
        改变_变量 = 0;
        break;
      }
      default:  /* any instruction that sets A */
        改变_变量 = (操作码_测试A模式_宏名(操作_短变量) && 寄存_短变量 == a);
        break;
    }
    if (改变_变量)
      设置寄存_变量 = 月调试_过滤程序计数器_函(程序计数_变量, 跳目标_变量);
  }
  return 设置寄存_变量;
}


/*
** Check whether table being indexed by instruction 'i' is the
** environment '_ENV'
*/
static const char *月调试_全局扩展函_函 (const 原型_结 *p, int 程序计数_变量, Instruction i, int isup) {
  int t = 操作码_获取实参_B_宏名(i);  /* table index */
  const char *名称_变量;  /* 名称_变量 of indexed variable */
  if (isup)  /* is an 上值_圆? */
    名称_变量 = 月调试_上值名称_函(p, t);
  else
    月调试_获取对象名称_函(p, 程序计数_变量, t, &名称_变量);
  return (名称_变量 && strcmp(名称_变量, 词法_月亮_环境_宏名) == 0) ? "global" : "月解析器_字段_函";
}


static const char *月调试_获取对象名称_函 (const 原型_结 *p, int lastpc, int 寄存_短变量,
                               const char **名称_变量) {
  int 程序计数_变量;
  *名称_变量 = 月亮函数_获取本地名_函(p, 寄存_短变量 + 1, lastpc);
  if (*名称_变量)  /* is a local? */
    return "local";
  /* else try symbolic execution */
  程序计数_变量 = 月调试_找设置寄存器_函(p, lastpc, 寄存_短变量);
  if (程序计数_变量 != -1) {  /* could find instruction? */
    Instruction i = p->代码_变量[程序计数_变量];
    操作码_枚举 操作_短变量 = 操作码_获取_操作码_宏名(i);
    switch (操作_短变量) {
      case 操作_移: {
        int b = 操作码_获取实参_B_宏名(i);  /* move from 'b' 到_变量 'a' */
        if (b < 操作码_获取实参_A_宏名(i))
          return 月调试_获取对象名称_函(p, 程序计数_变量, b, 名称_变量);  /* get 名称_变量 for 'b' */
        break;
      }
      case 操作_获取表上值内: {
        int k = 操作码_获取实参_C_宏名(i);  /* 键_小变量 index */
        月调试_k名称_函(p, k, 名称_变量);
        return 月调试_全局扩展函_函(p, 程序计数_变量, i, 1);
      }
      case 操作_获取表: {
        int k = 操作码_获取实参_C_宏名(i);  /* 键_小变量 index */
        月调试_r名称_函(p, 程序计数_变量, k, 名称_变量);
        return 月调试_全局扩展函_函(p, 程序计数_变量, i, 0);
      }
      case 操作_获取整: {
        *名称_变量 = "integer index";
        return "月解析器_字段_函";
      }
      case 操作_获取字段: {
        int k = 操作码_获取实参_C_宏名(i);  /* 键_小变量 index */
        月调试_k名称_函(p, k, 名称_变量);
        return 月调试_全局扩展函_函(p, 程序计数_变量, i, 0);
      }
      case 操作_获取上值: {
        *名称_变量 = 月调试_上值名称_函(p, 操作码_获取实参_B_宏名(i));
        return "上值_圆";
      }
      case 操作_载入常:
      case 操作_载入常额外: {
        int b = (操作_短变量 == 操作_载入常) ? 操作码_获取实参_Bx_宏名(i)
                                 : 操作码_获取实参_Ax_宏名(p->代码_变量[程序计数_变量 + 1]);
        if (对象_tt是否字符串_宏名(&p->k[b])) {
          *名称_变量 = 对象_s值_宏名(&p->k[b]);
          return "constant";
        }
        break;
      }
      case 操作_自身: {
        月调试_rk名称_函(p, 程序计数_变量, i, 名称_变量);
        return "method";
      }
      default: break;  /* go through 到_变量 return NULL */
    }
  }
  return NULL;  /* could not find reasonable 名称_变量 */
}


/*
** Try 到_变量 find a 名称_变量 for a function based on the 代码_变量 that called it.
** (Only works when function was called by a Lua function.)
** Returns 什么_变量 the 名称_变量 is (e.g., "for iterator", "method",
** "metamethod") and sets '*名称_变量' 到_变量 针点_变量 到_变量 the 名称_变量.
*/
static const char *月调试_函名来自代码_函 (炉_状态机结 *L, const 原型_结 *p,
                                     int 程序计数_变量, const char **名称_变量) {
  标方符_枚举 标方_缩变量 = (标方符_枚举)0;  /* (initial 值_圆 avoids warnings) */
  Instruction i = p->代码_变量[程序计数_变量];  /* calling instruction */
  switch (操作码_获取_操作码_宏名(i)) {
    case 操作_调用:
    case 操作_尾调用:
      return 月调试_获取对象名称_函(p, 程序计数_变量, 操作码_获取实参_A_宏名(i), 名称_变量);  /* get function 名称_变量 */
    case 操作_泛型为调用: {  /* for iterator */
      *名称_变量 = "for iterator";
       return "for iterator";
    }
    /* other instructions can do calls through metamethods */
    case 操作_自身: case 操作_获取表上值内: case 操作_获取表:
    case 操作_获取整: case 操作_获取字段:
      标方_缩变量 = 标方_索引大写;
      break;
    case 操作_设置表上值内: case 操作_设置表: case 操作_设置整: case 操作_设置字段:
      标方_缩变量 = 标方_新索引大写;
      break;
    case 操作_元方法二元: case 操作_元方法二元整: case 操作_元方法二元常: {
      标方_缩变量 = 限制_类型转换_宏名(标方符_枚举, 操作码_获取实参_C_宏名(i));
      break;
    }
    case 操作_一元减号: 标方_缩变量 = 标方_一元减号大写; break;
    case 操作_位非: 标方_缩变量 = 标方_位非大写; break;
    case 操作_长度: 标方_缩变量 = 标方_长度大写; break;
    case 操作_拼接: 标方_缩变量 = 标方_拼接大写; break;
    case 操作_相等: 标方_缩变量 = 标方_相等大写; break;
    /* no cases for 操作_相等整 and 操作_相等常, as they don't call metamethods */
    case 操作_小于: case 操作_小于整: case 操作_大于整: 标方_缩变量 = 标方_小于大写; break;
    case 操作_小等: case 操作_小等整: case 操作_大等整: 标方_缩变量 = 标方_小等大写; break;
    case 操作_关闭: case 操作_返回: 标方_缩变量 = 标方_关闭大写; break;
    default:
      return NULL;  /* 月编译器_不能_函 find a reasonable 名称_变量 */
  }
  *名称_变量 = 对象_获取串_宏名(G(L)->标方名_变量[标方_缩变量]) + 2;
  return "metamethod";
}


/*
** Try 到_变量 find a 名称_变量 for a function based on how it was called.
*/
static const char *月调试_函名来自调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量,
                                                   const char **名称_变量) {
  if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_已钩子_宏名) {  /* was it called inside a 钩子_变量? */
    *名称_变量 = "?";
    return "钩子_变量";
  }
  else if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_终结的_宏名) {  /* was it called as a finalizer? */
    *名称_变量 = "__gc";
    return "metamethod";  /* 月解释器_报告_函 it as such */
  }
  else if (状态机_是否月亮_宏名(调信_缩变量))
    return 月调试_函名来自代码_函(L, 调试_ci_函_宏名(调信_缩变量)->p, 月调试_当前程序计数器_函(调信_缩变量), 名称_变量);
  else
    return NULL;
}

/* }====================================================== */



/*
** Check whether pointer 'o' points 到_变量 some 值_圆 in the 栈_圆小 frame of
** the 当前_圆 function and, if so, returns its index.  Because 'o' may
** not 针点_变量 到_变量 a 值_圆 in this 栈_圆小, we 月编译器_不能_函 compare it with the
** region boundaries (undefined behavior in ISO C).
*/
static int 栈内_小写 (调用信息_结 *调信_缩变量, const 标签值_结 *o) {
  int 位置_缩变量;
  栈身份_型 基本_变量 = 调信_缩变量->函_短变量.p + 1;
  for (位置_缩变量 = 0; 基本_变量 + 位置_缩变量 < 调信_缩变量->顶部_变量.p; 位置_缩变量++) {
    if (o == 对象_s到v_宏名(基本_变量 + 位置_缩变量))
      return 位置_缩变量;
  }
  return -1;  /* not found */
}


/*
** Checks whether 值_圆 'o' came from an 上值_圆. (That can only happen
** with instructions 操作_获取表上值内/操作_设置表上值内, which operate directly on
** 上值们_小写.)
*/
static const char *月调试_获取上值名称_函 (调用信息_结 *调信_缩变量, const 标签值_结 *o,
                                 const char **名称_变量) {
  L闭包_结 *c = 调试_ci_函_宏名(调信_缩变量);
  int i;
  for (i = 0; i < c->nupvalues; i++) {
    if (c->上值们_短[i]->v.p == o) {
      *名称_变量 = 月调试_上值名称_函(c->p, i);
      return "上值_圆";
    }
  }
  return NULL;
}


static const char *月调试_格式化变量信息_函 (炉_状态机结 *L, const char *种类_变量,
                                                const char *名称_变量) {
  if (种类_变量 == NULL)
    return "";  /* no information */
  else
    return 月亮对象_推入格式化字符串_函(L, " (%s '%s')", 种类_变量, 名称_变量);
}

/*
** Build a string with a "description" for the 值_圆 'o', such as
** "variable 'x'" or "上值_圆 'y'".
*/
static const char *月调试_变量信息_函 (炉_状态机结 *L, const 标签值_结 *o) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  const char *名称_变量 = NULL;  /* 到_变量 avoid warnings */
  const char *种类_变量 = NULL;
  if (状态机_是否月亮_宏名(调信_缩变量)) {
    种类_变量 = 月调试_获取上值名称_函(调信_缩变量, o, &名称_变量);  /* 月解析器_检查_函 whether 'o' is an 上值_圆 */
    if (!种类_变量) {  /* not an 上值_圆? */
      int 寄存_短变量 = 栈内_小写(调信_缩变量, o);  /* try a register */
      if (寄存_短变量 >= 0)  /* is 'o' a register? */
        种类_变量 = 月调试_获取对象名称_函(调试_ci_函_宏名(调信_缩变量)->p, 月调试_当前程序计数器_函(调信_缩变量), 寄存_短变量, &名称_变量);
    }
  }
  return 月调试_格式化变量信息_函(L, 种类_变量, 名称_变量);
}


/*
** Raise a type 错误_小变量
*/
static 限制_l_无返回值_宏名 月调试_类型错误_函 (炉_状态机结 *L, const 标签值_结 *o, const char *操作_短变量,
                          const char *额外_变量) {
  const char *t = 月亮类型_对象类型名_函(L, o);
  月亮全局_跑错误_函(L, "attempt 到_变量 %s a %s 值_圆%s", 操作_短变量, t, 额外_变量);
}


/*
** Raise a type 错误_小变量 with "standard" information about the faulty
** object 'o' (using '月调试_变量信息_函').
*/
限制_l_无返回值_宏名 月亮全局_类型错误_函 (炉_状态机结 *L, const 标签值_结 *o, const char *操作_短变量) {
  月调试_类型错误_函(L, o, 操作_短变量, 月调试_变量信息_函(L, o));
}


/*
** Raise an 错误_小变量 for calling a non-callable object. Try 到_变量 find a 名称_变量
** for the object based on how it was called ('月调试_函名来自调用_函'); if it
** 月编译器_不能_函 get a 名称_变量 there, try '月调试_变量信息_函'.
*/
限制_l_无返回值_宏名 月亮全局_调用错误_函 (炉_状态机结 *L, const 标签值_结 *o) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  const char *名称_变量 = NULL;  /* 到_变量 avoid warnings */
  const char *种类_变量 = 月调试_函名来自调用_函(L, 调信_缩变量, &名称_变量);
  const char *额外_变量 = 种类_变量 ? 月调试_格式化变量信息_函(L, 种类_变量, 名称_变量) : 月调试_变量信息_函(L, o);
  月调试_类型错误_函(L, o, "call", 额外_变量);
}


限制_l_无返回值_宏名 月亮全局_for循环错误_函 (炉_状态机结 *L, const 标签值_结 *o, const char *什么_变量) {
  月亮全局_跑错误_函(L, "bad 'for' %s (number expected, got %s)",
                   什么_变量, 月亮类型_对象类型名_函(L, o));
}


限制_l_无返回值_宏名 月亮全局_拼接错误_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2) {
  if (对象_tt是否字符串_宏名(p1) || 虚机头_转化到串_宏名(p1)) p1 = p2;
  月亮全局_类型错误_函(L, p1, "concatenate");
}


限制_l_无返回值_宏名 月亮全局_操作整型错误_函 (炉_状态机结 *L, const 标签值_结 *p1,
                         const 标签值_结 *p2, const char *消息_缩变量) {
  if (!对象_tt是否数目_宏名(p1))  /* 首先_变量 operand is wrong? */
    p2 = p1;  /* now second is wrong */
  月亮全局_类型错误_函(L, p2, 消息_缩变量);
}


/*
** Error when both values are convertible 到_变量 numbers, but not 到_变量 integers
*/
限制_l_无返回值_宏名 月亮全局_到整型错误_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2) {
  炉_整数型 临时_变量;
  if (!月亮虚拟机_非安全到整数_函(p1, &临时_变量, 虚机头_月亮_向下取整数目到整数_宏名))
    p2 = p1;
  月亮全局_跑错误_函(L, "number%s has no integer representation", 月调试_变量信息_函(L, p2));
}


限制_l_无返回值_宏名 月亮全局_秩序错误_函 (炉_状态机结 *L, const 标签值_结 *p1, const 标签值_结 *p2) {
  const char *临1_短变量 = 月亮类型_对象类型名_函(L, p1);
  const char *临2_短变量 = 月亮类型_对象类型名_函(L, p2);
  if (strcmp(临1_短变量, 临2_短变量) == 0)
    月亮全局_跑错误_函(L, "attempt 到_变量 compare two %s values", 临1_短变量);
  else
    月亮全局_跑错误_函(L, "attempt 到_变量 compare %s with %s", 临1_短变量, 临2_短变量);
}


/* add 源_短变量:行_变量 information 到_变量 '消息_缩变量' */
const char *月亮全局_添加信息_函 (炉_状态机结 *L, const char *消息_缩变量, 标签字符串_结 *源_短变量,
                                        int 行_变量) {
  char 缓冲_变量[炉_身份大小];
  if (源_短变量)
    月亮对象_大块id_函(缓冲_变量, 对象_获取串_宏名(源_短变量), 对象_tss长度_宏名(源_短变量));
  else {  /* no 源_圆 available; use "?" instead */
    缓冲_变量[0] = '?'; 缓冲_变量[1] = '\0';
  }
  return 月亮对象_推入格式化字符串_函(L, "%s:%d: %s", 缓冲_变量, 行_变量, 消息_缩变量);
}


限制_l_无返回值_宏名 月亮全局_错误消息_函 (炉_状态机结 *L) {
  if (L->错函_短变量 != 0) {  /* is there an 错误_小变量 handling function? */
    栈身份_型 错函_短变量 = 做_恢复栈_宏名(L, L->错函_短变量);
    限制_月亮_断言_宏名(对象_tt是否函数_宏名(对象_s到v_宏名(错函_短变量)));
    对象_设置对象s到s_宏名(L, L->顶部_变量.p, L->顶部_变量.p - 1);  /* move argument */
    对象_设置对象s到s_宏名(L, L->顶部_变量.p - 1, 错函_短变量);  /* push function */
    L->顶部_变量.p++;  /* assume 状态机_额外_栈_宏名 */
    月亮调度_调用无产出_函(L, L->顶部_变量.p - 2, 1);  /* call it */
  }
  月亮调度_抛出_函(L, 月头_月亮_运行错误_宏名);
}


限制_l_无返回值_宏名 月亮全局_跑错误_函 (炉_状态机结 *L, const char *格式_短变量, ...) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  const char *消息_缩变量;
  va_list argp;
  垃圾回收_月亮C_检查GC_宏名(L);  /* 错误_小变量 message uses memory */
  va_start(argp, 格式_短变量);
  消息_缩变量 = 月亮对象_推入可变格式化字符串_函(L, 格式_短变量, argp);  /* 格式_变量 message */
  va_end(argp);
  if (状态机_是否月亮_宏名(调信_缩变量)) {  /* if Lua function, add 源_圆:行_变量 information */
    月亮全局_添加信息_函(L, 消息_缩变量, 调试_ci_函_宏名(调信_缩变量)->p->源_圆, 月调试_获取当前行号_函(调信_缩变量));
    对象_设置对象s到s_宏名(L, L->顶部_变量.p - 2, L->顶部_变量.p - 1);  /* remove '消息_缩变量' */
    L->顶部_变量.p--;
  }
  月亮全局_错误消息_函(L);
}


/*
** Check whether new instruction 'newpc' is in a different 行_变量 from
** 前一个_变量 instruction '旧程计_变量'. More often than not, 'newpc' is only
** one or a few instructions after '旧程计_变量' (it must be after, see
** caller), so try 到_变量 avoid calling '月亮全局_获取函数行号_函'. If they are
** too far apart, there is a good chance of a 调试_绝对行信息_宏名 in the way,
** so it goes directly 到_变量 '月亮全局_获取函数行号_函'.
*/
static int 月调试_行号已更改_函 (const 原型_结 *p, int 旧程计_变量, int newpc) {
  if (p->行信息_变量 == NULL)  /* no debug information? */
    return 0;
  if (newpc - 旧程计_变量 < 调试_最大I宽度绝对值_宏名 / 2) {  /* not too far apart? */
    int 德尔塔_变量 = 0;  /* 行_变量 difference */
    int 程序计数_变量 = 旧程计_变量;
    for (;;) {
      int 行信息_变量 = p->行信息_变量[++程序计数_变量];
      if (行信息_变量 == 调试_绝对行信息_宏名)
        break;  /* 月编译器_不能_函 compute 德尔塔_变量; fall through */
      德尔塔_变量 += 行信息_变量;
      if (程序计数_变量 == newpc)
        return (德尔塔_变量 != 0);  /* 德尔塔_变量 computed successfully */
    }
  }
  /* either instructions are too far apart or there is an absolute 行_变量
     信息_短变量 in the way; compute 行_变量 difference explicitly */
  return (月亮全局_获取函数行号_函(p, 旧程计_变量) != 月亮全局_获取函数行号_函(p, newpc));
}


/*
** Traces the execution of a Lua function. Called before the execution
** of each opcode, when debug is on. 'L->旧程计_变量' stores the 最后_变量
** instruction traced, 到_变量 detect 行_变量 changes. When entering a new
** function, '新程计信_变量' will be zero and will test as a new 行_变量 whatever
** the 值_圆 of '旧程计_变量'.  Some exceptional conditions may return 到_变量
** a function without setting '旧程计_变量'. In that case, '旧程计_变量' may be
** invalid; if so, use zero as a 有效_变量 值_圆. (A wrong but 有效_变量 '旧程计_变量'
** at most causes an 额外_变量 call 到_变量 a 行_变量 钩子_变量.)
** This function is not "Protected" when called, so it should correct
** 'L->顶部_变量.p' before calling anything that can run the GC.
*/
int 月亮全局_跟踪执行_函 (炉_状态机结 *L, const Instruction *程序计数_变量) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  路_字节型 掩码_变量 = L->钩子掩码_圆;
  const 原型_结 *p = 调试_ci_函_宏名(调信_缩变量)->p;
  int 计数钩子_变量;
  if (!(掩码_变量 & (月头_月亮_掩码行_宏名 | 月头_月亮_掩码计数_宏名))) {  /* no hooks? */
    调信_缩变量->u.l.陷阱_变量 = 0;  /* don't need 到_变量 stop again */
    return 0;  /* turn off '陷阱_变量' */
  }
  程序计数_变量++;  /* reference is always 下一个_变量 instruction */
  调信_缩变量->u.l.已保存程计_缩 = 程序计数_变量;  /* 月词法_保存_函 '程序计数_变量' */
  计数钩子_变量 = (--L->钩子计数_圆 == 0 && (掩码_变量 & 月头_月亮_掩码计数_宏名));
  if (计数钩子_变量)
    调试_重置钩子计数_宏名(L);  /* reset 计数_变量 */
  else if (!(掩码_变量 & 月头_月亮_掩码行_宏名))
    return 1;  /* no 行_变量 钩子_变量 and 计数_变量 != 0; nothing 到_变量 be done now */
  if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_钩子让步_宏名) {  /* called 钩子_变量 最后_变量 time? */
    调信_缩变量->调用状态码_圆 &= ~状态机_调信状型_钩子让步_宏名;  /* erase 记号_变量 */
    return 1;  /* do not call 钩子_变量 again (VM yielded, so it did not move) */
  }
  if (!操作码_是否IT_宏名(*(调信_缩变量->u.l.已保存程计_缩 - 1)))  /* 顶部_变量 not being used? */
    L->顶部_变量.p = 调信_缩变量->顶部_变量.p;  /* correct 顶部_变量 */
  if (计数钩子_变量)
    月亮调度_钩子_函(L, 月头_月亮_钩子计数_宏名, -1, 0, 0);  /* call 计数_变量 钩子_变量 */
  if (掩码_变量 & 月头_月亮_掩码行_宏名) {
    /* 'L->旧程计_变量' may be invalid; use zero in this case */
    int 旧程计_变量 = (L->旧程计_变量 < p->代码大小_小写) ? L->旧程计_变量 : 0;
    int 新程计信_变量 = 调试_相对指令计数器_宏名(程序计数_变量, p);
    if (新程计信_变量 <= 旧程计_变量 ||  /* call 钩子_变量 when jump back (环_变量), */
        月调试_行号已更改_函(p, 旧程计_变量, 新程计信_变量)) {  /* or when enter new 行_变量 */
      int 新行_小变量 = 月亮全局_获取函数行号_函(p, 新程计信_变量);
      月亮调度_钩子_函(L, 月头_月亮_钩子行_宏名, 新行_小变量, 0, 0);  /* call 行_变量 钩子_变量 */
    }
    L->旧程计_变量 = 新程计信_变量;  /* '程序计数_变量' of 最后_变量 call 到_变量 行_变量 钩子_变量 */
  }
  if (L->状态码_变量 == 月头_月亮_让步_宏名) {  /* did 钩子_变量 yield? */
    if (计数钩子_变量)
      L->钩子计数_圆 = 1;  /* undo decrement 到_变量 zero */
    调信_缩变量->u.l.已保存程计_缩--;  /* undo increment (月做_恢复_函 will increment it again) */
    调信_缩变量->调用状态码_圆 |= 状态机_调信状型_钩子让步_宏名;  /* 记号_变量 that it yielded */
    月亮调度_抛出_函(L, 月头_月亮_让步_宏名);
  }
  return 1;  /* keep '陷阱_变量' on */
}

