/*
** $Id: ldo.c $
** Stack and Call structure of Lua
** See Copyright Notice in lua.h
*/

#define ldo_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <setjmp.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lapi.h"
#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"
#include "lundump.h"
#include "lvm.h"
#include "lzio.h"



#define 做_错误状态码_宏名(s)	((s) > 月头_月亮_让步_宏名)


/*
** {======================================================
** Error-recovery functions
** =======================================================
*/

/*
** 做_月亮I_抛出_宏名/做_月亮I_尝试_宏名 define how Lua does exception handling. By
** default, Lua handles errors with exceptions when compiling as
** C++ 代码_变量, with _longjmp/_setjmp when asked 到_变量 use them, and with
** longjmp/setjmp otherwise.
*/
#if !defined(做_月亮I_抛出_宏名)				/* { */

#if defined(__cplusplus) && !defined(LUA_USE_LONGJMP)	/* { */

/* C++ exceptions */
#define 做_月亮I_抛出_宏名(L,c)		throw(c)
#define 做_月亮I_尝试_宏名(L,c,a) \
	try { a } catch(...) { if ((c)->状态码_变量 == 0) (c)->状态码_变量 = -1; }
#define 炉i_跳缓缩		int  /* 虚假_变量 variable */

#elif defined(配置_月亮_用_POSIX_宏名)				/* }{ */

/* in POSIX, try _longjmp/_setjmp (更多_变量 efficient) */
#define 做_月亮I_抛出_宏名(L,c)		_longjmp((c)->b, 1)
#define 做_月亮I_尝试_宏名(L,c,a)		if (_setjmp((c)->b) == 0) { a }
#define 炉i_跳缓缩		jmp_buf

#else							/* }{ */

/* ISO C handling with long jumps */
#define 做_月亮I_抛出_宏名(L,c)		longjmp((c)->b, 1)
#define 做_月亮I_尝试_宏名(L,c,a)		if (setjmp((c)->b) == 0) { a }
#define 炉i_跳缓缩		jmp_buf

#endif							/* } */

#endif							/* } */



/* chain 列表_变量 of long jump buffers */
struct 炉_长跳结 {
  struct 炉_长跳结 *前一个_变量;
  炉i_跳缓缩 b;
  volatile int 状态码_变量;  /* 错误_小变量 代码_变量 */
};


void 月亮调度_设置错误对象_函 (炉_状态机结 *L, int errcode, 栈身份_型 oldtop) {
  switch (errcode) {
    case 月头_月亮_内存错误_宏名: {  /* memory 错误_小变量? */
      对象_设置ts值到s_宏名(L, oldtop, G(L)->内存错误消息_短); /* reuse preregistered 消息_缩变量. */
      break;
    }
    case 月头_月亮_错误中的错误_宏名: {
      对象_设置ts值到s_宏名(L, oldtop, 字符串_月亮S_新字面_宏名(L, "错误_小变量 in 错误_小变量 handling"));
      break;
    }
    case LUA_OK: {  /* special case only for closing 上值们_小写 */
      对象_设置空值的值_宏名(对象_s到v_宏名(oldtop));  /* no 错误_小变量 message */
      break;
    }
    default: {
      限制_月亮_断言_宏名(做_错误状态码_宏名(errcode));  /* real 错误_小变量 */
      对象_设置对象s到s_宏名(L, oldtop, L->顶部_变量.p - 1);  /* 错误_小变量 message on 当前_圆 顶部_变量 */
      break;
    }
  }
  L->顶部_变量.p = oldtop + 1;
}


限制_l_无返回值_宏名 月亮调度_抛出_函 (炉_状态机结 *L, int errcode) {
  if (L->错误跳_圆缩) {  /* 线程_变量 has an 错误_小变量 handler? */
    L->错误跳_圆缩->状态码_变量 = errcode;  /* set 状态码_变量 */
    做_月亮I_抛出_宏名(L, L->错误跳_圆缩);  /* jump 到_变量 it */
  }
  else {  /* 线程_变量 has no 错误_小变量 handler */
    全局_状态机结 *g = G(L);
    errcode = 月亮错误_重置线程_函(L, errcode);  /* 关闭_圆 all 上值们_小写 */
    if (g->主线程_圆->错误跳_圆缩) {  /* main 线程_变量 has a handler? */
      对象_设置对象s到s_宏名(L, g->主线程_圆->顶部_变量.p++, L->顶部_变量.p - 1);  /* copy 错误_小变量 对象_变量. */
      月亮调度_抛出_函(g->主线程_圆, errcode);  /* re-throw in main 线程_变量 */
    }
    else {  /* no handler at all; abort */
      if (g->恐慌_圆) {  /* 恐慌_圆 function? */
        限制_月亮_解锁_宏名(L);
        g->恐慌_圆(L);  /* call 恐慌_圆 function (最后_变量 chance 到_变量 jump out) */
      }
      abort();
    }
  }
}


int 月亮调度_原始跑受保护_函 (炉_状态机结 *L, Pfunc f, void *用数_缩变量) {
  l_无符整32型 旧数C调用_变量 = L->C调用们数_缩圆;
  struct 炉_长跳结 轻跳_缩变量;
  轻跳_缩变量.状态码_变量 = LUA_OK;
  轻跳_缩变量.前一个_变量 = L->错误跳_圆缩;  /* chain new 错误_小变量 handler */
  L->错误跳_圆缩 = &轻跳_缩变量;
  做_月亮I_尝试_宏名(L, &轻跳_缩变量,
    (*f)(L, 用数_缩变量);
  );
  L->错误跳_圆缩 = 轻跳_缩变量.前一个_变量;  /* restore 旧_变量 错误_小变量 handler */
  L->C调用们数_缩圆 = 旧数C调用_变量;
  return 轻跳_缩变量.状态码_变量;
}

/* }====================================================== */


/*
** {==================================================================
** Stack reallocation
** ===================================================================
*/


/*
** Change all pointers 到_变量 the 栈_圆小 into offsets.
*/
static void 月做_相对栈_函 (炉_状态机结 *L) {
  调用信息_结 *调信_缩变量;
  上值_结 *上_小变量;
  L->顶部_变量.偏移_变量 = 做_保存栈_宏名(L, L->顶部_变量.p);
  L->待关闭列表_结.偏移_变量 = 做_保存栈_宏名(L, L->待关闭列表_结.p);
  for (上_小变量 = L->打开上值_圆小; 上_小变量 != NULL; 上_小变量 = 上_小变量->u.打开_圆.下一个_变量)
    上_小变量->v.偏移_变量 = 做_保存栈_宏名(L, 函_上层级_宏名(上_小变量));
  for (调信_缩变量 = L->调信_缩变量; 调信_缩变量 != NULL; 调信_缩变量 = 调信_缩变量->前一个_变量) {
    调信_缩变量->顶部_变量.偏移_变量 = 做_保存栈_宏名(L, 调信_缩变量->顶部_变量.p);
    调信_缩变量->函_短变量.偏移_变量 = 做_保存栈_宏名(L, 调信_缩变量->函_短变量.p);
  }
}


/*
** Change back all offsets into pointers.
*/
static void 月做_纠正栈_函 (炉_状态机结 *L) {
  调用信息_结 *调信_缩变量;
  上值_结 *上_小变量;
  L->顶部_变量.p = 做_恢复栈_宏名(L, L->顶部_变量.偏移_变量);
  L->待关闭列表_结.p = 做_恢复栈_宏名(L, L->待关闭列表_结.偏移_变量);
  for (上_小变量 = L->打开上值_圆小; 上_小变量 != NULL; 上_小变量 = 上_小变量->u.打开_圆.下一个_变量)
    上_小变量->v.p = 对象_s到v_宏名(做_恢复栈_宏名(L, 上_小变量->v.偏移_变量));
  for (调信_缩变量 = L->调信_缩变量; 调信_缩变量 != NULL; 调信_缩变量 = 调信_缩变量->前一个_变量) {
    调信_缩变量->顶部_变量.p = 做_恢复栈_宏名(L, 调信_缩变量->顶部_变量.偏移_变量);
    调信_缩变量->函_短变量.p = 做_恢复栈_宏名(L, 调信_缩变量->函_短变量.偏移_变量);
    if (状态机_是否月亮_宏名(调信_缩变量))
      调信_缩变量->u.l.陷阱_变量 = 1;  /* signal 到_变量 update '陷阱_变量' in '月亮虚拟机_执行_函' */
  }
}


/* some 空间_圆 for 错误_小变量 handling */
#define 做_错误栈大小_宏名	(配置_月亮I_最大栈_宏名 + 200)

/*
** Reallocate the 栈_圆小 到_变量 a new 大小_变量, correcting all pointers into it.
** In ISO C, any pointer use after the pointer has been deallocated is
** undefined behavior. So, before the reallocation, all pointers are
** 已更改_变量 到_变量 offsets, and after the reallocation they are 已更改_变量 back
** 到_变量 pointers. As during the reallocation the pointers are invalid, the
** reallocation 月编译器_不能_函 run emergency collections.
**
** In case of allocation 错误_小变量, raise an 错误_小变量 or return false according
** 到_变量 'raiseerror'.
*/
int 月亮调度_重新分配栈_函 (炉_状态机结 *L, int 新大小_变量, int raiseerror) {
  int 旧大小_变量 = 状态机_栈大小_宏名(L);
  int i;
  栈身份_型 新栈_变量;
  int 旧GC停_变量 = G(L)->垃回紧急停_缩;
  限制_月亮_断言_宏名(新大小_变量 <= 配置_月亮I_最大栈_宏名 || 新大小_变量 == 做_错误栈大小_宏名);
  月做_相对栈_函(L);  /* 改变_变量 pointers 到_变量 offsets */
  G(L)->垃回紧急停_缩 = 1;  /* stop emergency collection */
  新栈_变量 = 内存_月亮M_重新分配向量_宏名(L, L->栈_圆小.p, 旧大小_变量 + 状态机_额外_栈_宏名,
                                   新大小_变量 + 状态机_额外_栈_宏名, 栈值_联);
  G(L)->垃回紧急停_缩 = 旧GC停_变量;  /* restore emergency collection */
  if (配置_l_可能性低_宏名(新栈_变量 == NULL)) {  /* reallocation failed? */
    月做_纠正栈_函(L);  /* 改变_变量 offsets back 到_变量 pointers */
    if (raiseerror)
      内存_月亮M_错误_宏名(L);
    else return 0;  /* do not raise an 错误_小变量 */
  }
  L->栈_圆小.p = 新栈_变量;
  月做_纠正栈_函(L);  /* 改变_变量 offsets back 到_变量 pointers */
  L->栈_最后圆.p = L->栈_圆小.p + 新大小_变量;
  for (i = 旧大小_变量 + 状态机_额外_栈_宏名; i < 新大小_变量 + 状态机_额外_栈_宏名; i++)
    对象_设置空值的值_宏名(对象_s到v_宏名(新栈_变量 + i)); /* erase new segment */
  return 1;
}


/*
** Try 到_变量 grow the 栈_圆小 by at least 'n' elements. When 'raiseerror'
** is true, raises any 错误_小变量; otherwise, return 0 in case of errors.
*/
int 月亮调度_增长栈_函 (炉_状态机结 *L, int n, int raiseerror) {
  int 大小_变量 = 状态机_栈大小_宏名(L);
  if (配置_l_可能性低_宏名(大小_变量 > 配置_月亮I_最大栈_宏名)) {
    /* if 栈_圆小 is larger than maximum, 线程_变量 is already using the
       额外_变量 空间_圆 reserved for errors, that is, 线程_变量 is handling
       a 栈_圆小 错误_小变量; 月编译器_不能_函 grow further than that. */
    限制_月亮_断言_宏名(状态机_栈大小_宏名(L) == 做_错误栈大小_宏名);
    if (raiseerror)
      月亮调度_抛出_函(L, 月头_月亮_错误中的错误_宏名);  /* 错误_小变量 inside message handler */
    return 0;  /* if not 'raiseerror', just signal it */
  }
  else if (n < 配置_月亮I_最大栈_宏名) {  /* avoids arithmetic overflows */
    int 新大小_变量 = 2 * 大小_变量;  /* tentative new 大小_变量 */
    int 已需要_变量 = 限制_类型转换_整型_宏名(L->顶部_变量.p - L->栈_圆小.p) + n;
    if (新大小_变量 > 配置_月亮I_最大栈_宏名)  /* 月编译器_不能_函 cross the 限制_变量 */
      新大小_变量 = 配置_月亮I_最大栈_宏名;
    if (新大小_变量 < 已需要_变量)  /* but must respect 什么_变量 was asked for */
      新大小_变量 = 已需要_变量;
    if (配置_l_可能性高_宏名(新大小_变量 <= 配置_月亮I_最大栈_宏名))
      return 月亮调度_重新分配栈_函(L, 新大小_变量, raiseerror);
  }
  /* else 栈_圆小 overflow */
  /* add 额外_变量 大小_变量 到_变量 be able 到_变量 handle the 错误_小变量 message */
  月亮调度_重新分配栈_函(L, 做_错误栈大小_宏名, raiseerror);
  if (raiseerror)
    月亮全局_跑错误_函(L, "栈_圆小 overflow");
  return 0;
}


/*
** Compute how much of the 栈_圆小 is being used, by computing the
** maximum 顶部_变量 of all call frames in the 栈_圆小 and the 当前_圆 顶部_变量.
*/
static int 月做_栈使用中_函 (炉_状态机结 *L) {
  调用信息_结 *调信_缩变量;
  int 结果_短变量;
  栈身份_型 限_短变量 = L->顶部_变量.p;
  for (调信_缩变量 = L->调信_缩变量; 调信_缩变量 != NULL; 调信_缩变量 = 调信_缩变量->前一个_变量) {
    if (限_短变量 < 调信_缩变量->顶部_变量.p) 限_短变量 = 调信_缩变量->顶部_变量.p;
  }
  限制_月亮_断言_宏名(限_短变量 <= L->栈_最后圆.p + 状态机_额外_栈_宏名);
  结果_短变量 = 限制_类型转换_整型_宏名(限_短变量 - L->栈_圆小.p) + 1;  /* part of 栈_圆小 in use */
  if (结果_短变量 < 月头_月亮_最小栈_宏名)
    结果_短变量 = 月头_月亮_最小栈_宏名;  /* ensure a minimum 大小_变量 */
  return 结果_短变量;
}


/*
** If 栈_圆小 大小_变量 is 更多_变量 than 3 times the 当前_圆 use, reduce that 大小_变量
** 到_变量 twice the 当前_圆 use. (So, the 最终_变量 栈_圆小 大小_变量 is at most 2/3 the
** 前一个_变量 大小_变量, and half of its entries are 空容器_变量.)
** As a particular case, if 栈_圆小 was handling a 栈_圆小 overflow and now
** it is not, '最大_小变量' (limited by 配置_月亮I_最大栈_宏名) will be smaller than
** 状态机_栈大小_宏名 (equal 到_变量 做_错误栈大小_宏名 in this case), and so the 栈_圆小
** will be reduced 到_变量 a "regular" 大小_变量.
*/
void 月亮调度_收缩栈_函 (炉_状态机结 *L) {
  int 在用_变量 = 月做_栈使用中_函(L);
  int 最大_小变量 = (在用_变量 > 配置_月亮I_最大栈_宏名 / 3) ? 配置_月亮I_最大栈_宏名 : 在用_变量 * 3;
  /* if 线程_变量 is currently not handling a 栈_圆小 overflow and its
     大小_变量 is larger than maximum "reasonable" 大小_变量, shrink it */
  if (在用_变量 <= 配置_月亮I_最大栈_宏名 && 状态机_栈大小_宏名(L) > 最大_小变量) {
    int 新大小_变量 = (在用_变量 > 配置_月亮I_最大栈_宏名 / 2) ? 配置_月亮I_最大栈_宏名 : 在用_变量 * 2;
    月亮调度_重新分配栈_函(L, 新大小_变量, 0);  /* ok if that fails */
  }
  else  /* don't 改变_变量 栈_圆小 */
    限制_条件移动栈_宏名(L,{},{});  /* (改变_变量 only for debugging) */
  月亮错误_收缩调用信息_函(L);  /* shrink CI 列表_变量 */
}


void 月亮调度_递增顶_函 (炉_状态机结 *L) {
  做_月亮D_检查栈_宏名(L, 1);
  L->顶部_变量.p++;
}

/* }================================================================== */


/*
** Call a 钩子_变量 for the given 事件_变量. Make sure there is a 钩子_变量 到_变量 be
** called. (Both 'L->钩子_变量' and 'L->钩子掩码_圆', which trigger this
** function, can be 已更改_变量 asynchronously by signals.)
*/
void 月亮调度_钩子_函 (炉_状态机结 *L, int 事件_变量, int 行_变量,
                              int 函传输_变量, int 转移数_缩) {
  炉_钩子型 钩子_变量 = L->钩子_变量;
  if (钩子_变量 && L->允许钩子_圆) {  /* make sure there is a 钩子_变量 */
    int 掩码_变量 = 状态机_调信状型_已钩子_宏名;
    调用信息_结 *调信_缩变量 = L->调信_缩变量;
    ptrdiff_t 顶部_变量 = 做_保存栈_宏名(L, L->顶部_变量.p);  /* preserve original '顶部_变量' */
    ptrdiff_t ci_top = 做_保存栈_宏名(L, 调信_缩变量->顶部_变量.p);  /* idem for '调信_缩变量->顶部_变量' */
    炉_调试结 活记_缩变量;
    活记_缩变量.事件_变量 = 事件_变量;
    活记_缩变量.当前行_变量 = 行_变量;
    活记_缩变量.i_调信缩 = 调信_缩变量;
    if (转移数_缩 != 0) {
      掩码_变量 |= 状态机_调信状型_传输_宏名;  /* '调信_缩变量' has transfer information */
      调信_缩变量->u2.转移信息_结.函传输_变量 = 函传输_变量;
      调信_缩变量->u2.转移信息_结.转移数_缩 = 转移数_缩;
    }
    if (状态机_是否月亮_宏名(调信_缩变量) && L->顶部_变量.p < 调信_缩变量->顶部_变量.p)
      L->顶部_变量.p = 调信_缩变量->顶部_变量.p;  /* protect entire activation register */
    做_月亮D_检查栈_宏名(L, 月头_月亮_最小栈_宏名);  /* ensure minimum 栈_圆小 大小_变量 */
    if (调信_缩变量->顶部_变量.p < L->顶部_变量.p + 月头_月亮_最小栈_宏名)
      调信_缩变量->顶部_变量.p = L->顶部_变量.p + 月头_月亮_最小栈_宏名;
    L->允许钩子_圆 = 0;  /* 月编译器_不能_函 call hooks inside a 钩子_变量 */
    调信_缩变量->调用状态码_圆 |= 掩码_变量;
    限制_月亮_解锁_宏名(L);
    (*钩子_变量)(L, &活记_缩变量);
    限制_月亮_锁_宏名(L);
    限制_月亮_断言_宏名(!L->允许钩子_圆);
    L->允许钩子_圆 = 1;
    调信_缩变量->顶部_变量.p = 做_恢复栈_宏名(L, ci_top);
    L->顶部_变量.p = 做_恢复栈_宏名(L, 顶部_变量);
    调信_缩变量->调用状态码_圆 &= ~掩码_变量;
  }
}


/*
** Executes a call 钩子_变量 for Lua functions. This function is called
** whenever '钩子掩码_圆' is not zero, so it checks whether call hooks are
** active.
*/
void 月亮调度_钩子调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量) {
  L->旧程计_变量 = 0;  /* set '旧程计_变量' for new function */
  if (L->钩子掩码_圆 & 月头_月亮_掩码调用_宏名) {  /* is call 钩子_变量 on? */
    int 事件_变量 = (调信_缩变量->调用状态码_圆 & 状态机_调信状型_尾部_宏名) ? 月头_月亮_钩子尾调用_宏名
                                             : 月头_月亮_钩子调用_宏名;
    原型_结 *p = 调试_ci_函_宏名(调信_缩变量)->p;
    调信_缩变量->u.l.已保存程计_缩++;  /* hooks assume '程序计数_变量' is already incremented */
    月亮调度_钩子_函(L, 事件_变量, -1, 1, p->形参数_小写);
    调信_缩变量->u.l.已保存程计_缩--;  /* correct '程序计数_变量' */
  }
}


/*
** Executes a return 钩子_变量 for Lua and C functions and sets/corrects
** '旧程计_变量'. (Note that this correction is 已需要_变量 by the 行_变量 钩子_变量, so it
** is done even when return hooks are off.)
*/
static void 月做_返回钩子_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, int 结果数目_变量) {
  if (L->钩子掩码_圆 & 月头_月亮_掩码返回_宏名) {  /* is return 钩子_变量 on? */
    栈身份_型 首个果_变量 = L->顶部_变量.p - 结果数目_变量;  /* index of 首先_变量 结果_变量 */
    int 德尔塔_变量 = 0;  /* correction for vararg functions */
    int 函传输_变量;
    if (状态机_是否月亮_宏名(调信_缩变量)) {
      原型_结 *p = 调试_ci_函_宏名(调信_缩变量)->p;
      if (p->是否_变量实参短)
        德尔塔_变量 = 调信_缩变量->u.l.额外实参数_缩 + p->形参数_小写 + 1;
    }
    调信_缩变量->函_短变量.p += 德尔塔_变量;  /* if vararg, back 到_变量 virtual '函_短变量' */
    函传输_变量 = 限制_类型转换_宏名(unsigned short, 首个果_变量 - 调信_缩变量->函_短变量.p);
    月亮调度_钩子_函(L, 月头_月亮_钩子返回_宏名, -1, 函传输_变量, 结果数目_变量);  /* call it */
    调信_缩变量->函_短变量.p -= 德尔塔_变量;
  }
  if (状态机_是否月亮_宏名(调信_缩变量 = 调信_缩变量->前一个_变量))
    L->旧程计_变量 = 调试_相对指令计数器_宏名(调信_缩变量->u.l.已保存程计_缩, 调试_ci_函_宏名(调信_缩变量)->p);  /* set '旧程计_变量' */
}


/*
** Check whether '函_短变量' has a '__call' metafield. If so, put it in the
** 栈_圆小, below original '函_短变量', so that '月亮调度_预备调用_函' can call it. Raise
** an 错误_小变量 if there is no '__call' metafield.
*/
栈身份_型 月亮调度_尝试函标签方法_函 (炉_状态机结 *L, 栈身份_型 函_短变量) {
  const 标签值_结 *标方_缩变量;
  栈身份_型 p;
  做_检查栈GC指针_宏名(L, 1, 函_短变量);  /* 空间_圆 for metamethod */
  标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, 对象_s到v_宏名(函_短变量), 标方_调用大写);  /* (after 前一个_变量 GC) */
  if (配置_l_可能性低_宏名(对象_tt是否空值_宏名(标方_缩变量)))
    月亮全局_调用错误_函(L, 对象_s到v_宏名(函_短变量));  /* nothing 到_变量 call */
  for (p = L->顶部_变量.p; p > 函_短变量; p--)  /* 打开_圆 空间_圆 for metamethod */
    对象_设置对象s到s_宏名(L, p, p-1);
  L->顶部_变量.p++;  /* 栈_圆小 空间_圆 pre-allocated by the caller */
  对象_设置对象到s_宏名(L, 函_短变量, 标方_缩变量);  /* metamethod is the new function 到_变量 be called */
  return 函_短变量;
}


/*
** Given '结果数目_变量' results at 'firstResult', move '已想要_变量' of them 到_变量 '结果_短变量'.
** Handle most typical cases (zero results for commands, one 结果_变量 for
** expressions, multiple results for tail calls/single parameters)
** separated.
*/
l_sinline void 月做_移动结果_函 (炉_状态机结 *L, 栈身份_型 结果_短变量, int 结果数目_变量, int 已想要_变量) {
  栈身份_型 首个结果_变量;
  int i;
  switch (已想要_变量) {  /* handle typical cases separately */
    case 0:  /* no values 已需要_变量 */
      L->顶部_变量.p = 结果_短变量;
      return;
    case 1:  /* one 值_圆 已需要_变量 */
      if (结果数目_变量 == 0)   /* no results? */
        对象_设置空值的值_宏名(对象_s到v_宏名(结果_短变量));  /* adjust with nil */
      else  /* at least one 结果_变量 */
        对象_设置对象s到s_宏名(L, 结果_短变量, L->顶部_变量.p - 结果数目_变量);  /* move it 到_变量 proper place */
      L->顶部_变量.p = 结果_短变量 + 1;
      return;
    case 月头_月亮_多返回_宏名:
      已想要_变量 = 结果数目_变量;  /* we want all results */
      break;
    default:  /* two/更多_变量 results and/or 到_变量-be-closed variables */
      if (应程接_必须关闭C函数_宏名(已想要_变量)) {  /* 到_变量-be-closed variables? */
        L->调信_缩变量->调用状态码_圆 |= 状态机_调信状型_闭包返回_宏名;  /* in case of yields */
        L->调信_缩变量->u2.结果数目_变量 = 结果数目_变量;
        结果_短变量 = 月亮函数_关闭_函(L, 结果_短变量, 函_关闭栈顶_宏名, 1);
        L->调信_缩变量->调用状态码_圆 &= ~状态机_调信状型_闭包返回_宏名;
        if (L->钩子掩码_圆) {  /* if 已需要_变量, call 钩子_变量 after '__close's */
          ptrdiff_t savedres = 做_保存栈_宏名(L, 结果_短变量);
          月做_返回钩子_函(L, L->调信_缩变量, 结果数目_变量);
          结果_短变量 = 做_恢复栈_宏名(L, savedres);  /* 钩子_变量 can move 栈_圆小 */
        }
        已想要_变量 = 应程接_解码N结果们_宏名(已想要_变量);
        if (已想要_变量 == 月头_月亮_多返回_宏名)
          已想要_变量 = 结果数目_变量;  /* we want all results */
      }
      break;
  }
  /* generic case */
  首个结果_变量 = L->顶部_变量.p - 结果数目_变量;  /* index of 首先_变量 结果_变量 */
  if (结果数目_变量 > 已想要_变量)  /* 额外_变量 results? */
    结果数目_变量 = 已想要_变量;  /* don't need them */
  for (i = 0; i < 结果数目_变量; i++)  /* move all results 到_变量 correct place */
    对象_设置对象s到s_宏名(L, 结果_短变量 + i, 首个结果_变量 + i);
  for (; i < 已想要_变量; i++)  /* complete 已想要_变量 number of results */
    对象_设置空值的值_宏名(对象_s到v_宏名(结果_短变量 + i));
  L->顶部_变量.p = 结果_短变量 + 已想要_变量;  /* 顶部_变量 points after the 最后_变量 结果_变量 */
}


/*
** Finishes a function call: calls 钩子_变量 if necessary, moves 当前_圆
** number of results 到_变量 proper place, and returns 到_变量 前一个_变量 call
** 信息_短变量. If function has 到_变量 关闭_圆 variables, 钩子_变量 must be called after
** that.
*/
void 月亮调度_后处理调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, int 结果数目_变量) {
  int 已想要_变量 = 调信_缩变量->结果数目_变量;
  if (配置_l_可能性低_宏名(L->钩子掩码_圆 && !应程接_必须关闭C函数_宏名(已想要_变量)))
    月做_返回钩子_函(L, 调信_缩变量, 结果数目_变量);
  /* move results 到_变量 proper place */
  月做_移动结果_函(L, 调信_缩变量->函_短变量.p, 结果数目_变量, 已想要_变量);
  /* function 月编译器_不能_函 be in any of these cases when returning */
  限制_月亮_断言_宏名(!(调信_缩变量->调用状态码_圆 &
        (状态机_调信状型_已钩子_宏名 | 状态机_调信状型_让步保护调用_宏名 | 状态机_调信状型_终结的_宏名 | 状态机_调信状型_传输_宏名 | 状态机_调信状型_闭包返回_宏名)));
  L->调信_缩变量 = 调信_缩变量->前一个_变量;  /* back 到_变量 caller (after closing variables) */
}



#define 做_下一个_ci_宏名(L)  (L->调信_缩变量->下一个_变量 ? L->调信_缩变量->下一个_变量 : 月亮错误_扩展调用信息_函(L))


l_sinline 调用信息_结 *月做_预备保护调用信息_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int 返数_缩变量,
                                                int 掩码_变量, 栈身份_型 顶部_变量) {
  调用信息_结 *调信_缩变量 = L->调信_缩变量 = 做_下一个_ci_宏名(L);  /* new frame */
  调信_缩变量->函_短变量.p = 函_短变量;
  调信_缩变量->结果数目_变量 = 返数_缩变量;
  调信_缩变量->调用状态码_圆 = 掩码_变量;
  调信_缩变量->顶部_变量.p = 顶部_变量;
  return 调信_缩变量;
}


/*
** precall for C functions
*/
l_sinline int 月做_预备调用C_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int 结果数目_变量,
                                            炉_C函数半 f) {
  int n;  /* number of returns */
  调用信息_结 *调信_缩变量;
  做_检查栈GC指针_宏名(L, 月头_月亮_最小栈_宏名, 函_短变量);  /* ensure minimum 栈_圆小 大小_变量 */
  L->调信_缩变量 = 调信_缩变量 = 月做_预备保护调用信息_函(L, 函_短变量, 结果数目_变量, 状态机_调信状型_C_宏名,
                               L->顶部_变量.p + 月头_月亮_最小栈_宏名);
  限制_月亮_断言_宏名(调信_缩变量->顶部_变量.p <= L->栈_最后圆.p);
  if (配置_l_可能性低_宏名(L->钩子掩码_圆 & 月头_月亮_掩码调用_宏名)) {
    int 实参数_变量 = 限制_类型转换_整型_宏名(L->顶部_变量.p - 函_短变量) - 1;
    月亮调度_钩子_函(L, 月头_月亮_钩子调用_宏名, -1, 1, 实参数_变量);
  }
  限制_月亮_解锁_宏名(L);
  n = (*f)(L);  /* do the 实际上_变量 call */
  限制_月亮_锁_宏名(L);
  应程接_API检查n元素们_宏名(L, n);
  月亮调度_后处理调用_函(L, 调信_缩变量, n);
  return n;
}


/*
** Prepare a function for a tail call, building its call 信息_短变量 on 顶部_变量
** of the 当前_圆 call 信息_短变量. 'narg1' is the number of arguments plus 1
** (so that it includes the function itself). Return the number of
** results, if it was a C function, or -1 for a Lua function.
*/
int 月亮调度_预备尾调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量, 栈身份_型 函_短变量,
                                    int narg1, int 德尔塔_变量) {
 retry:
  switch (对象_t类型标签_宏名(对象_s到v_宏名(函_短变量))) {
    case 对象_月亮_VC闭包L_宏名:  /* C closure */
      return 月做_预备调用C_函(L, 函_短变量, 月头_月亮_多返回_宏名, 对象_闭包C值_宏名(对象_s到v_宏名(函_短变量))->f);
    case 对象_月亮_VL闭包函_宏名:  /* light C function */
      return 月做_预备调用C_函(L, 函_短变量, 月头_月亮_多返回_宏名, 对象_函值_宏名(对象_s到v_宏名(函_短变量)));
    case 对象_月亮_VL闭包L_宏名: {  /* Lua function */
      原型_结 *p = 对象_闭包L值_宏名(对象_s到v_宏名(函_短变量))->p;
      int 函大小_变量 = p->最大栈大小_小写;  /* frame 大小_变量 */
      int 固定形参数_变量 = p->形参数_小写;
      int i;
      做_检查栈GC指针_宏名(L, 函大小_变量 - 德尔塔_变量, 函_短变量);
      调信_缩变量->函_短变量.p -= 德尔塔_变量;  /* restore '函_短变量' (if vararg) */
      for (i = 0; i < narg1; i++)  /* move down function and arguments */
        对象_设置对象s到s_宏名(L, 调信_缩变量->函_短变量.p + i, 函_短变量 + i);
      函_短变量 = 调信_缩变量->函_短变量.p;  /* moved-down function */
      for (; narg1 <= 固定形参数_变量; narg1++)
        对象_设置空值的值_宏名(对象_s到v_宏名(函_短变量 + narg1));  /* complete missing arguments */
      调信_缩变量->顶部_变量.p = 函_短变量 + 1 + 函大小_变量;  /* 顶部_变量 for new function */
      限制_月亮_断言_宏名(调信_缩变量->顶部_变量.p <= L->栈_最后圆.p);
      调信_缩变量->u.l.已保存程计_缩 = p->代码_变量;  /* starting 针点_变量 */
      调信_缩变量->调用状态码_圆 |= 状态机_调信状型_尾部_宏名;
      L->顶部_变量.p = 函_短变量 + narg1;  /* set 顶部_变量 */
      return -1;
    }
    default: {  /* not a function */
      函_短变量 = 月亮调度_尝试函标签方法_函(L, 函_短变量);  /* try 到_变量 get '__call' metamethod */
      /* return 月亮调度_预备尾调用_函(L, 调信_缩变量, 函_短变量, narg1 + 1, 德尔塔_变量); */
      narg1++;
      goto retry;  /* try again */
    }
  }
}


/*
** Prepares the call 到_变量 a function (C or Lua). For C functions, also do
** the call. The function 到_变量 be called is at '*函_短变量'.  The arguments
** are on the 栈_圆小, 右_圆 after the function.  Returns the 调用信息_结
** 到_变量 be executed, if it was a Lua function. Otherwise (a C function)
** returns NULL, with all the results on the 栈_圆小, starting at the
** original function position.
*/
调用信息_结 *月亮调度_预备调用_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int 结果数目_变量) {
 retry:
  switch (对象_t类型标签_宏名(对象_s到v_宏名(函_短变量))) {
    case 对象_月亮_VC闭包L_宏名:  /* C closure */
      月做_预备调用C_函(L, 函_短变量, 结果数目_变量, 对象_闭包C值_宏名(对象_s到v_宏名(函_短变量))->f);
      return NULL;
    case 对象_月亮_VL闭包函_宏名:  /* light C function */
      月做_预备调用C_函(L, 函_短变量, 结果数目_变量, 对象_函值_宏名(对象_s到v_宏名(函_短变量)));
      return NULL;
    case 对象_月亮_VL闭包L_宏名: {  /* Lua function */
      调用信息_结 *调信_缩变量;
      原型_结 *p = 对象_闭包L值_宏名(对象_s到v_宏名(函_短变量))->p;
      int 实参数_变量 = 限制_类型转换_整型_宏名(L->顶部_变量.p - 函_短变量) - 1;  /* number of real arguments */
      int 固定形参数_变量 = p->形参数_小写;
      int 函大小_变量 = p->最大栈大小_小写;  /* frame 大小_变量 */
      做_检查栈GC指针_宏名(L, 函大小_变量, 函_短变量);
      L->调信_缩变量 = 调信_缩变量 = 月做_预备保护调用信息_函(L, 函_短变量, 结果数目_变量, 0, 函_短变量 + 1 + 函大小_变量);
      调信_缩变量->u.l.已保存程计_缩 = p->代码_变量;  /* starting 针点_变量 */
      for (; 实参数_变量 < 固定形参数_变量; 实参数_变量++)
        对象_设置空值的值_宏名(对象_s到v_宏名(L->顶部_变量.p++));  /* complete missing arguments */
      限制_月亮_断言_宏名(调信_缩变量->顶部_变量.p <= L->栈_最后圆.p);
      return 调信_缩变量;
    }
    default: {  /* not a function */
      函_短变量 = 月亮调度_尝试函标签方法_函(L, 函_短变量);  /* try 到_变量 get '__call' metamethod */
      /* return 月亮调度_预备调用_函(L, 函_短变量, 结果数目_变量); */
      goto retry;  /* try again with metamethod */
    }
  }
}


/*
** Call a function (C or Lua) through C. 'inc' can be 1 (increment
** number of recursive invocations in the C 栈_圆小) or 状态机_不让步调用信息_宏名 (the same
** plus increment number of non-状态机_可让步_宏名 calls).
** This function can be called with some use of 状态机_额外_栈_宏名, so it should
** 月解析器_检查_函 the 栈_圆小 before doing anything else. '月亮调度_预备调用_函' already
** does that.
*/
l_sinline void 月做_C调用_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults, l_无符整32型 inc) {
  调用信息_结 *调信_缩变量;
  L->C调用们数_缩圆 += inc;
  if (配置_l_可能性低_宏名(状态机_获取C调用_宏名(L) >= 限制_月亮I_最大C调用们_宏名)) {
    做_检查栈指针_宏名(L, 0, 函_短变量);  /* free any use of 状态机_额外_栈_宏名 */
    月亮错误_检查C栈_函(L);
  }
  if ((调信_缩变量 = 月亮调度_预备调用_函(L, 函_短变量, nResults)) != NULL) {  /* Lua function? */
    调信_缩变量->调用状态码_圆 = 状态机_调信状型_新鲜_宏名;  /* 记号_变量 that it is a "fresh" execute */
    月亮虚拟机_执行_函(L, 调信_缩变量);  /* call it */
  }
  L->C调用们数_缩圆 -= inc;
}


/*
** External interface for '月做_C调用_函'
*/
void 月亮调度_调用_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults) {
  月做_C调用_函(L, 函_短变量, nResults, 1);
}


/*
** Similar 到_变量 '月亮调度_调用_函', but does not allow yields during the call.
*/
void 月亮调度_调用无产出_函 (炉_状态机结 *L, 栈身份_型 函_短变量, int nResults) {
  月做_C调用_函(L, 函_短变量, nResults, 状态机_不让步调用信息_宏名);
}


/*
** Finish the job of '月亮_保护调用常量_函' after it was interrupted by an yield.
** (The caller, '月做_完成C调用_函', does the 最终_变量 call 到_变量 '应程接_调整结果们_宏名'.)
** The main job is 到_变量 complete the '月亮调度_预处理调用_函' called by '月亮_保护调用常量_函'.
** If a '__close' method yields here, eventually control will be back
** 到_变量 '月做_完成C调用_函' (when that '__close' method finally returns) and
** '月做_完成保护调用k_函' will run again and 关闭_圆 any still pending '__close'
** methods. Similarly, if a '__close' method errs, '月做_预备覆盖_函' calls
** '月做_反滚动_函' which calls ''月做_完成C调用_函' and we are back here again, 到_变量
** 关闭_圆 any pending '__close' methods.
** Note that, 上_小变量 到_变量 the call 到_变量 '月亮函数_关闭_函', the corresponding
** '调用信息_结' is not modified, so that this repeated run works like the
** 首先_变量 one (except that it has at least one less '__close' 到_变量 do). In
** particular, 月解析器_字段_函 状态机_调信状型_递归调用状型_宏名 preserves the 错误_小变量 状态码_变量 across these
** multiple runs, changing only if there is a new 错误_小变量.
*/
static int 月做_完成保护调用k_函 (炉_状态机结 *L,  调用信息_结 *调信_缩变量) {
  int 状态码_变量 = 状态机_获取调信状型递归调用状型_宏名(调信_缩变量);  /* get original 状态码_变量 */
  if (配置_l_可能性高_宏名(状态码_变量 == LUA_OK))  /* no 错误_小变量? */
    状态码_变量 = 月头_月亮_让步_宏名;  /* was interrupted by an yield */
  else {  /* 错误_小变量 */
    栈身份_型 函_短变量 = 做_恢复栈_宏名(L, 调信_缩变量->u2.函索引_短);
    L->允许钩子_圆 = 状态机_获取旧活动钩子_宏名(调信_缩变量->调用状态码_圆);  /* restore '允许钩子_圆' */
    函_短变量 = 月亮函数_关闭_函(L, 函_短变量, 状态码_变量, 1);  /* can yield or raise an 错误_小变量 */
    月亮调度_设置错误对象_函(L, 状态码_变量, 函_短变量);
    月亮调度_收缩栈_函(L);   /* restore 栈_圆小 大小_变量 in case of overflow */
    状态机_设置调信状型递归调用状型_宏名(调信_缩变量, LUA_OK);  /* clear original 状态码_变量 */
  }
  调信_缩变量->调用状态码_圆 &= ~状态机_调信状型_让步保护调用_宏名;
  L->错函_短变量 = 调信_缩变量->u.c.旧_错函短;
  /* if it is here, there were errors or yields; unlike '月亮_保护调用常量_函',
     do not 改变_变量 状态码_变量 */
  return 状态码_变量;
}


/*
** Completes the execution of a C function interrupted by an yield.
** The interruption must have happened while the function was either
** closing its 待关闭_缩变量 variables in '月做_移动结果_函' or executing
** '月亮_调用常量_函'/'月亮_保护调用常量_函'. In the 首先_变量 case, it just redoes
** '月亮调度_后处理调用_函'. In the second case, the call 到_变量 '月做_完成保护调用k_函'
** finishes the interrupted execution of '月亮_保护调用常量_函'.  After that, it
** calls the continuation of the interrupted function and finally it
** completes the job of the '月亮调度_调用_函' that called the function.  In
** the call 到_变量 '应程接_调整结果们_宏名', we do not know the number of results
** of the function called by '月亮_调用常量_函'/'月亮_保护调用常量_函', so we are
** conservative and use 月头_月亮_多返回_宏名 (always adjust).
*/
static void 月做_完成C调用_函 (炉_状态机结 *L, 调用信息_结 *调信_缩变量) {
  int n;  /* 实际上_变量 number of results from C function */
  if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_闭包返回_宏名) {  /* was returning? */
    限制_月亮_断言_宏名(应程接_必须关闭C函数_宏名(调信_缩变量->结果数目_变量));
    n = 调信_缩变量->u2.结果数目_变量;  /* just redo '月亮调度_后处理调用_函' */
    /* don't need 到_变量 reset 状态机_调信状型_闭包返回_宏名, as it will be set again anyway */
  }
  else {
    int 状态码_变量 = 月头_月亮_让步_宏名;  /* default if there were no errors */
    /* must have a continuation and must be able 到_变量 call it */
    限制_月亮_断言_宏名(调信_缩变量->u.c.k != NULL && 状态机_可让步_宏名(L));
    if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_让步保护调用_宏名)   /* was inside a '月亮_保护调用常量_函'? */
      状态码_变量 = 月做_完成保护调用k_函(L, 调信_缩变量);  /* finish it */
    应程接_调整结果们_宏名(L, 月头_月亮_多返回_宏名);  /* finish '月亮_调用常量_函' */
    限制_月亮_解锁_宏名(L);
    n = (*调信_缩变量->u.c.k)(L, 状态码_变量, 调信_缩变量->u.c.ctx);  /* call continuation */
    限制_月亮_锁_宏名(L);
    应程接_API检查n元素们_宏名(L, n);
  }
  月亮调度_后处理调用_函(L, 调信_缩变量, n);  /* finish '月亮调度_调用_函' */
}


/*
** Executes "full continuation" (everything in the 栈_圆小) of a
** previously interrupted coroutine until the 栈_圆小 is 空容器_变量 (or another
** interruption long-jumps out of the 环_变量).
*/
static void 月做_反滚动_函 (炉_状态机结 *L, void *用数_缩变量) {
  调用信息_结 *调信_缩变量;
  限制_未使用_宏名(用数_缩变量);
  while ((调信_缩变量 = L->调信_缩变量) != &L->基本_调信圆缩) {  /* something in the 栈_圆小 */
    if (!状态机_是否月亮_宏名(调信_缩变量))  /* C function? */
      月做_完成C调用_函(L, 调信_缩变量);  /* complete its execution */
    else {  /* Lua function */
      月亮虚拟机_完成操作_函(L);  /* finish interrupted instruction */
      月亮虚拟机_执行_函(L, 调信_缩变量);  /* execute down 到_变量 higher C '边界_变量' */
    }
  }
}


/*
** Try 到_变量 find a suspended protected call (a "recover 针点_变量") for the
** given 线程_变量.
*/
static 调用信息_结 *月做_找保护调用_函 (炉_状态机结 *L) {
  调用信息_结 *调信_缩变量;
  for (调信_缩变量 = L->调信_缩变量; 调信_缩变量 != NULL; 调信_缩变量 = 调信_缩变量->前一个_变量) {  /* search for a pcall */
    if (调信_缩变量->调用状态码_圆 & 状态机_调信状型_让步保护调用_宏名)
      return 调信_缩变量;
  }
  return NULL;  /* no pending pcall */
}


/*
** Signal an 错误_小变量 in the call 到_变量 '月亮_恢复_函', not in the execution
** of the coroutine itself. (Such errors should not be handled by any
** coroutine 错误_小变量 handler and should not kill the coroutine.)
*/
static int 月做_恢复_错误_函 (炉_状态机结 *L, const char *消息_缩变量, int 实参数_变量) {
  L->顶部_变量.p -= 实参数_变量;  /* remove 实参们_短变量 from the 栈_圆小 */
  对象_设置ts值到s_宏名(L, L->顶部_变量.p, 月亮字符串_新_函(L, 消息_缩变量));  /* push 错误_小变量 message */
  应程接_API_递增_顶部_宏名(L);
  限制_月亮_解锁_宏名(L);
  return 月头_月亮_运行错误_宏名;
}


/*
** Do the 工作_变量 for '月亮_恢复_函' in protected 模块_变量. Most of the 工作_变量
** depends on the 状态码_变量 of the coroutine: initial 状态机_变量, suspended
** inside a 钩子_变量, or regularly suspended (optionally with a continuation
** function), plus erroneous cases: non-suspended coroutine or dead
** coroutine.
*/
static void 月做_恢复_函 (炉_状态机结 *L, void *用数_缩变量) {
  int n = *(限制_类型转换_宏名(int*, 用数_缩变量));  /* number of arguments */
  栈身份_型 首个实参_变量 = L->顶部_变量.p - n;  /* 首先_变量 argument */
  调用信息_结 *调信_缩变量 = L->调信_缩变量;
  if (L->状态码_变量 == LUA_OK)  /* starting a coroutine? */
    月做_C调用_函(L, 首个实参_变量 - 1, 月头_月亮_多返回_宏名, 0);  /* just call its 月解析器_身体_函 */
  else {  /* resuming from 前一个_变量 yield */
    限制_月亮_断言_宏名(L->状态码_变量 == 月头_月亮_让步_宏名);
    L->状态码_变量 = LUA_OK;  /* 记号_变量 that it is running (again) */
    if (状态机_是否月亮_宏名(调信_缩变量)) {  /* yielded inside a 钩子_变量? */
      L->顶部_变量.p = 首个实参_变量;  /* discard arguments */
      月亮虚拟机_执行_函(L, 调信_缩变量);  /* just continue running Lua 代码_变量 */
    }
    else {  /* 'common' yield */
      if (调信_缩变量->u.c.k != NULL) {  /* does it have a continuation function? */
        限制_月亮_解锁_宏名(L);
        n = (*调信_缩变量->u.c.k)(L, 月头_月亮_让步_宏名, 调信_缩变量->u.c.ctx); /* call continuation */
        限制_月亮_锁_宏名(L);
        应程接_API检查n元素们_宏名(L, n);
      }
      月亮调度_后处理调用_函(L, 调信_缩变量, n);  /* finish '月亮调度_调用_函' */
    }
    月做_反滚动_函(L, NULL);  /* run continuation */
  }
}


/*
** Unrolls a coroutine in protected 模块_变量 while there are recoverable
** errors, that is, errors inside a protected call. (Any 错误_小变量
** interrupts '月做_反滚动_函', and this 环_变量 protects it again so it can
** continue.) Stops with a normal 终_变量 (状态码_变量 == LUA_OK), an yield
** (状态码_变量 == 月头_月亮_让步_宏名), or an unprotected 错误_小变量 ('月做_找保护调用_函' doesn't
** find a recover 针点_变量).
*/
static int 月做_预备覆盖_函 (炉_状态机结 *L, int 状态码_变量) {
  调用信息_结 *调信_缩变量;
  while (做_错误状态码_宏名(状态码_变量) && (调信_缩变量 = 月做_找保护调用_函(L)) != NULL) {
    L->调信_缩变量 = 调信_缩变量;  /* go down 到_变量 recovery functions */
    状态机_设置调信状型递归调用状型_宏名(调信_缩变量, 状态码_变量);  /* 状态码_变量 到_变量 finish 'pcall' */
    状态码_变量 = 月亮调度_原始跑受保护_函(L, 月做_反滚动_函, NULL);
  }
  return 状态码_变量;
}


配置_月亮_应程接_宏名 int 月亮_恢复_函 (炉_状态机结 *L, 炉_状态机结 *from, int 实参数们_变量,
                                      int *结果数目_变量) {
  int 状态码_变量;
  限制_月亮_锁_宏名(L);
  if (L->状态码_变量 == LUA_OK) {  /* may be starting a coroutine */
    if (L->调信_缩变量 != &L->基本_调信圆缩)  /* not in 基本_变量 层级_变量? */
      return 月做_恢复_错误_函(L, "月编译器_不能_函 月做_恢复_函 non-suspended coroutine", 实参数们_变量);
    else if (L->顶部_变量.p - (L->调信_缩变量->函_短变量.p + 1) == 实参数们_变量)  /* no function? */
      return 月做_恢复_错误_函(L, "月编译器_不能_函 月做_恢复_函 dead coroutine", 实参数们_变量);
  }
  else if (L->状态码_变量 != 月头_月亮_让步_宏名)  /* ended with errors? */
    return 月做_恢复_错误_函(L, "月编译器_不能_函 月做_恢复_函 dead coroutine", 实参数们_变量);
  L->C调用们数_缩圆 = (from) ? 状态机_获取C调用_宏名(from) : 0;
  if (状态机_获取C调用_宏名(L) >= 限制_月亮I_最大C调用们_宏名)
    return 月做_恢复_错误_函(L, "C 栈_圆小 overflow", 实参数们_变量);
  L->C调用们数_缩圆++;
  限制_月亮i_用户状态恢复_宏名(L, 实参数们_变量);
  应程接_API检查n元素们_宏名(L, (L->状态码_变量 == LUA_OK) ? 实参数们_变量 + 1 : 实参数们_变量);
  状态码_变量 = 月亮调度_原始跑受保护_函(L, 月做_恢复_函, &实参数们_变量);
   /* continue running after recoverable errors */
  状态码_变量 = 月做_预备覆盖_函(L, 状态码_变量);
  if (配置_l_可能性高_宏名(!做_错误状态码_宏名(状态码_变量)))
    限制_月亮_断言_宏名(状态码_变量 == L->状态码_变量);  /* normal 终_变量 or yield */
  else {  /* unrecoverable 错误_小变量 */
    L->状态码_变量 = 限制_类型转换_字节_宏名(状态码_变量);  /* 记号_变量 线程_变量 as 'dead' */
    月亮调度_设置错误对象_函(L, 状态码_变量, L->顶部_变量.p);  /* push 错误_小变量 message */
    L->调信_缩变量->顶部_变量.p = L->顶部_变量.p;
  }
  *结果数目_变量 = (状态码_变量 == 月头_月亮_让步_宏名) ? L->调信_缩变量->u2.让步数_缩
                                    : 限制_类型转换_整型_宏名(L->顶部_变量.p - (L->调信_缩变量->函_短变量.p + 1));
  限制_月亮_解锁_宏名(L);
  return 状态码_变量;
}


配置_月亮_应程接_宏名 int 月亮_是否可让出_函 (炉_状态机结 *L) {
  return 状态机_可让步_宏名(L);
}


配置_月亮_应程接_宏名 int 月亮_让出常量_函 (炉_状态机结 *L, int 结果数目_变量, 炉_K上下文型 ctx,
                        炉_K函数型 k) {
  调用信息_结 *调信_缩变量;
  限制_月亮i_用户状态让步_宏名(L, 结果数目_变量);
  限制_月亮_锁_宏名(L);
  调信_缩变量 = L->调信_缩变量;
  应程接_API检查n元素们_宏名(L, 结果数目_变量);
  if (配置_l_可能性低_宏名(!状态机_可让步_宏名(L))) {
    if (L != G(L)->主线程_圆)
      月亮全局_跑错误_函(L, "attempt 到_变量 yield across a C-call 边界_变量");
    else
      月亮全局_跑错误_函(L, "attempt 到_变量 yield from outside a coroutine");
  }
  L->状态码_变量 = 月头_月亮_让步_宏名;
  调信_缩变量->u2.让步数_缩 = 结果数目_变量;  /* 月词法_保存_函 number of results */
  if (状态机_是否月亮_宏名(调信_缩变量)) {  /* inside a 钩子_变量? */
    限制_月亮_断言_宏名(!状态机_是否月亮代码_宏名(调信_缩变量));
    限制_应程接_检查_宏名(L, 结果数目_变量 == 0, "hooks 月编译器_不能_函 yield values");
    限制_应程接_检查_宏名(L, k == NULL, "hooks 月编译器_不能_函 continue after yielding");
  }
  else {
    if ((调信_缩变量->u.c.k = k) != NULL)  /* is there a continuation? */
      调信_缩变量->u.c.ctx = ctx;  /* 月词法_保存_函 context */
    月亮调度_抛出_函(L, 月头_月亮_让步_宏名);
  }
  限制_月亮_断言_宏名(调信_缩变量->调用状态码_圆 & 状态机_调信状型_已钩子_宏名);  /* must be inside a 钩子_变量 */
  限制_月亮_解锁_宏名(L);
  return 0;  /* return 到_变量 '月亮调度_钩子_函' */
}


/*
** Auxiliary structure 到_变量 call '月亮函数_关闭_函' in protected 模块_变量.
*/
struct 保护关闭_结 {
  栈身份_型 层级_变量;
  int 状态码_变量;
};


/*
** Auxiliary function 到_变量 call '月亮函数_关闭_函' in protected 模块_变量.
*/
static void 月做_关闭保护辅助_函 (炉_状态机结 *L, void *用数_缩变量) {
  struct 保护关闭_结 *程计位_变量 = 限制_类型转换_宏名(struct 保护关闭_结 *, 用数_缩变量);
  月亮函数_关闭_函(L, 程计位_变量->层级_变量, 程计位_变量->状态码_变量, 0);
}


/*
** Calls '月亮函数_关闭_函' in protected 模块_变量. Return the original 状态码_变量
** or, in case of errors, the new 状态码_变量.
*/
int 月亮调度_关闭受保护_函 (炉_状态机结 *L, ptrdiff_t 层级_变量, int 状态码_变量) {
  调用信息_结 *旧_调信_变量 = L->调信_缩变量;
  路_字节型 旧_允许钩子_变量 = L->允许钩子_圆;
  for (;;) {  /* keep closing 上值们_小写 until no 更多_变量 errors */
    struct 保护关闭_结 程计位_变量;
    程计位_变量.层级_变量 = 做_恢复栈_宏名(L, 层级_变量); 程计位_变量.状态码_变量 = 状态码_变量;
    状态码_变量 = 月亮调度_原始跑受保护_函(L, &月做_关闭保护辅助_函, &程计位_变量);
    if (配置_l_可能性高_宏名(状态码_变量 == LUA_OK))  /* no 更多_变量 errors? */
      return 程计位_变量.状态码_变量;
    else {  /* an 错误_小变量 occurred; restore saved 状态机_变量 and repeat */
      L->调信_缩变量 = 旧_调信_变量;
      L->允许钩子_圆 = 旧_允许钩子_变量;
    }
  }
}


/*
** Call the C function '函_短变量' in protected 模块_变量, restoring basic
** 线程_变量 information ('允许钩子_圆', etc.) and in particular
** its 栈_圆小 层级_变量 in case of errors.
*/
int 月亮调度_预处理调用_函 (炉_状态机结 *L, Pfunc 函_短变量, void *u,
                ptrdiff_t old_top, ptrdiff_t 错标_缩变量) {
  int 状态码_变量;
  调用信息_结 *旧_调信_变量 = L->调信_缩变量;
  路_字节型 旧_允许钩子_变量 = L->允许钩子_圆;
  ptrdiff_t 旧_错函短 = L->错函_短变量;
  L->错函_短变量 = 错标_缩变量;
  状态码_变量 = 月亮调度_原始跑受保护_函(L, 函_短变量, u);
  if (配置_l_可能性低_宏名(状态码_变量 != LUA_OK)) {  /* an 错误_小变量 occurred? */
    L->调信_缩变量 = 旧_调信_变量;
    L->允许钩子_圆 = 旧_允许钩子_变量;
    状态码_变量 = 月亮调度_关闭受保护_函(L, old_top, 状态码_变量);
    月亮调度_设置错误对象_函(L, 状态码_变量, 做_恢复栈_宏名(L, old_top));
    月亮调度_收缩栈_函(L);   /* restore 栈_圆小 大小_变量 in case of overflow */
  }
  L->错函_短变量 = 旧_错函短;
  return 状态码_变量;
}



/*
** Execute a protected parser.
*/
struct 安全解析_结 {  /* 数据_变量 到_变量 '月做_f_解析器_函' */
  入出流_结 *z;
  M缓冲区_结 缓冲_变量;  /* dynamic structure used by the scanner */
  动态数据_结 定你数_缩变量;  /* dynamic structures used by the parser */
  const char *模块_变量;
  const char *名称_变量;
};


static void 月做_检查模式_函 (炉_状态机结 *L, const char *模块_变量, const char *x) {
  if (模块_变量 && strchr(模块_变量, x[0]) == NULL) {
    月亮对象_推入格式化字符串_函(L,
       "attempt 到_变量 load a %s chunk (模块_变量 is '%s')", x, 模块_变量);
    月亮调度_抛出_函(L, 月头_月亮_句法错误_宏名);
  }
}


static void 月做_f_解析器_函 (炉_状态机结 *L, void *用数_缩变量) {
  L闭包_结 *闭包_短变量;
  struct 安全解析_结 *p = 限制_类型转换_宏名(struct 安全解析_结 *, 用数_缩变量);
  int c = 入出流_流获取字符_宏名(p->z);  /* read 首先_变量 character */
  if (c == 月头_月亮_签名_宏名[0]) {
    月做_检查模式_函(L, p->模块_变量, "binary");
    闭包_短变量 = 月亮实用工具_解除转储_函(L, p->z, p->名称_变量);
  }
  else {
    月做_检查模式_函(L, p->模块_变量, "text");
    闭包_短变量 = 月亮解析_解析器_函(L, p->z, &p->缓冲_变量, &p->定你数_缩变量, p->名称_变量, c);
  }
  限制_月亮_断言_宏名(闭包_短变量->nupvalues == 闭包_短变量->p->上值大小_小写);
  月亮函数_初始的上值_函(L, 闭包_短变量);
}


int 月亮调度_受保护解析器_函 (炉_状态机结 *L, 入出流_结 *z, const char *名称_变量,
                                        const char *模块_变量) {
  struct 安全解析_结 p;
  int 状态码_变量;
  状态机_递增n不让步_宏名(L);  /* 月编译器_不能_函 yield during parsing */
  p.z = z; p.名称_变量 = 名称_变量; p.模块_变量 = 模块_变量;
  p.定你数_缩变量.活动变量_结.arr = NULL; p.定你数_缩变量.活动变量_结.大小_变量 = 0;
  p.定你数_缩变量.全局表_缩变量.arr = NULL; p.定你数_缩变量.全局表_缩变量.大小_变量 = 0;
  p.定你数_缩变量.标号_圆.arr = NULL; p.定你数_缩变量.标号_圆.大小_变量 = 0;
  入出流_月亮Z_初始的缓冲区_宏名(L, &p.缓冲_变量);
  状态码_变量 = 月亮调度_预处理调用_函(L, 月做_f_解析器_函, &p, 做_保存栈_宏名(L, L->顶部_变量.p), L->错函_短变量);
  入出流_月亮Z_释放缓冲区_宏名(L, &p.缓冲_变量);
  内存_月亮M_释放数组_宏名(L, p.定你数_缩变量.活动变量_结.arr, p.定你数_缩变量.活动变量_结.大小_变量);
  内存_月亮M_释放数组_宏名(L, p.定你数_缩变量.全局表_缩变量.arr, p.定你数_缩变量.全局表_缩变量.大小_变量);
  内存_月亮M_释放数组_宏名(L, p.定你数_缩变量.标号_圆.arr, p.定你数_缩变量.标号_圆.大小_变量);
  状态机_递减n不让步_宏名(L);
  return 状态码_变量;
}


