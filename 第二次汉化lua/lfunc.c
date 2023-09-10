/*
** $Id: lfunc.c $
** Auxiliary functions 到_变量 manipulate prototypes and closures
** See Copyright Notice in lua.h
*/

#define lfunc_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <stddef.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



C闭包_结 *月亮函数_新C闭包_函 (炉_状态机结 *L, int nupvals) {
  垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_VC闭包L_宏名, 函_C闭包大小_宏名(nupvals));
  C闭包_结 *c = 状态机_垃圾回收对象到c闭包_宏名(o);
  c->nupvalues = 限制_类型转换_字节_宏名(nupvals);
  return c;
}


L闭包_结 *月亮函数_新L闭包_函 (炉_状态机结 *L, int nupvals) {
  垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_VL闭包L_宏名, 函_L闭包大小_宏名(nupvals));
  L闭包_结 *c = 状态机_垃圾回收对象到l闭包_宏名(o);
  c->p = NULL;
  c->nupvalues = 限制_类型转换_字节_宏名(nupvals);
  while (nupvals--) c->上值们_短[nupvals] = NULL;
  return c;
}


/*
** fill a closure with new closed 上值们_小写
*/
void 月亮函数_初始的上值_函 (炉_状态机结 *L, L闭包_结 *闭包_短变量) {
  int i;
  for (i = 0; i < 闭包_短变量->nupvalues; i++) {
    垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_V上值_宏名, sizeof(上值_结));
    上值_结 *上值_缩变量 = 状态机_垃圾回收对象到上值_宏名(o);
    上值_缩变量->v.p = &上值_缩变量->u.值_圆;  /* make it closed */
    对象_设置空值的值_宏名(上值_缩变量->v.p);
    闭包_短变量->上值们_短[i] = 上值_缩变量;
    垃圾回收_月亮C_对象屏障_宏名(L, 闭包_短变量, 上值_缩变量);
  }
}


/*
** Create a new 上值_圆 at the given 层级_变量, and link it 到_变量 the 列表_变量 of
** 打开_圆 上值们_小写 of 'L' after entry '前一_短变量'.
**/
static 上值_结 *月函_新上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量, 上值_结 **前一_短变量) {
  垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_V上值_宏名, sizeof(上值_结));
  上值_结 *上值_缩变量 = 状态机_垃圾回收对象到上值_宏名(o);
  上值_结 *下一个_变量 = *前一_短变量;
  上值_缩变量->v.p = 对象_s到v_宏名(层级_变量);  /* 当前_圆 值_圆 lives in the 栈_圆小 */
  上值_缩变量->u.打开_圆.下一个_变量 = 下一个_变量;  /* link it 到_变量 列表_变量 of 打开_圆 上值们_小写 */
  上值_缩变量->u.打开_圆.前一个_变量 = 前一_短变量;
  if (下一个_变量)
    下一个_变量->u.打开_圆.前一个_变量 = &上值_缩变量->u.打开_圆.下一个_变量;
  *前一_短变量 = 上值_缩变量;
  if (!函_是否具有内部的上值_宏名(L)) {  /* 线程_变量 not in 列表_变量 of threads with 上值们_小写? */
    L->线程与上值_短缩 = G(L)->线程与上值_短缩;  /* link it 到_变量 the 列表_变量 */
    G(L)->线程与上值_短缩 = L;
  }
  return 上值_缩变量;
}


/*
** Find and reuse, or create if it does not exist, an 上值_圆
** at the given 层级_变量.
*/
上值_结 *月亮函数_查找上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量) {
  上值_结 **pp = &L->打开上值_圆小;
  上值_结 *p;
  限制_月亮_断言_宏名(函_是否具有内部的上值_宏名(L) || L->打开上值_圆小 == NULL);
  while ((p = *pp) != NULL && 函_上层级_宏名(p) >= 层级_变量) {  /* search for it */
    限制_月亮_断言_宏名(!垃圾回收_是否死亡_宏名(G(L), p));
    if (函_上层级_宏名(p) == 层级_变量)  /* corresponding 上值_圆? */
      return p;  /* return it */
    pp = &p->u.打开_圆.下一个_变量;
  }
  /* not found: create a new 上值_圆 after 'pp' */
  return 月函_新上值_函(L, 层级_变量, pp);
}


/*
** Call closing method for object '对象_变量' with 错误_小变量 message '错_短变量'. The
** boolean 'yy' controls whether the call is 状态机_可让步_宏名.
** (This function assumes 状态机_额外_栈_宏名.)
*/
static void 月函_调用关闭方法_函 (炉_状态机结 *L, 标签值_结 *对象_变量, 标签值_结 *错_短变量, int yy) {
  栈身份_型 顶部_变量 = L->顶部_变量.p;
  const 标签值_结 *标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, 对象_变量, 标方_关闭大写);
  对象_设置对象到s_宏名(L, 顶部_变量, 标方_缩变量);  /* will call metamethod... */
  对象_设置对象到s_宏名(L, 顶部_变量 + 1, 对象_变量);  /* with 'self' as the 1st argument */
  对象_设置对象到s_宏名(L, 顶部_变量 + 2, 错_短变量);  /* and 错误_小变量 消息_缩变量. as 2nd argument */
  L->顶部_变量.p = 顶部_变量 + 3;  /* add function and arguments */
  if (yy)
    月亮调度_调用_函(L, 顶部_变量, 0);
  else
    月亮调度_调用无产出_函(L, 顶部_变量, 0);
}


/*
** Check whether object at given 层级_变量 has a 关闭_圆 metamethod and raise
** an 错误_小变量 if not.
*/
static void 月函_检查关闭方法_函 (炉_状态机结 *L, 栈身份_型 层级_变量) {
  const 标签值_结 *标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, 对象_s到v_宏名(层级_变量), 标方_关闭大写);
  if (对象_tt是否空值_宏名(标方_缩变量)) {  /* no metamethod? */
    int 索引_缩变量 = 限制_类型转换_整型_宏名(层级_变量 - L->调信_缩变量->函_短变量.p);  /* variable index */
    const char *变名_变量 = 月亮全局_查找本地变量_函(L, L->调信_缩变量, 索引_缩变量, NULL);
    if (变名_变量 == NULL) 变名_变量 = "?";
    月亮全局_跑错误_函(L, "variable '%s' got a non-closable 值_圆", 变名_变量);
  }
}


/*
** Prepare and call a closing method.
** If 状态码_变量 is 函_关闭栈顶_宏名, the call 到_变量 the closing method will be 已推入_小写
** at the 顶部_变量 of the 栈_圆小. Otherwise, values can be 已推入_小写 右_圆 after
** the '层级_变量' of the 上值_圆 being closed, as everything after that
** won't be used again.
*/
static void 月函_预备保护调用关闭方法_函 (炉_状态机结 *L, 栈身份_型 层级_变量, int 状态码_变量, int yy) {
  标签值_结 *上值_缩变量 = 对象_s到v_宏名(层级_变量);  /* 值_圆 being closed */
  标签值_结 *错误对象_变量;
  if (状态码_变量 == 函_关闭栈顶_宏名)
    错误对象_变量 = &G(L)->空值的值_圆;  /* 错误_小变量 object is nil */
  else {  /* '月亮调度_设置错误对象_函' will set 顶部_变量 到_变量 层级_变量 + 2 */
    错误对象_变量 = 对象_s到v_宏名(层级_变量 + 1);  /* 错误_小变量 object goes after '上值_缩变量' */
    月亮调度_设置错误对象_函(L, 状态码_变量, 层级_变量 + 1);  /* set 错误_小变量 object */
  }
  月函_调用关闭方法_函(L, 上值_缩变量, 错误对象_变量, yy);
}


/*
** Maximum 值_圆 for deltas in '待关闭列表_结', dependent on the type
** of 德尔塔_变量. (This macro assumes that an 'L' is in scope 哪儿_变量 it
** is used.)
*/
#define 函_最大德尔塔_宏名  \
	((256ul << ((sizeof(L->栈_圆小.p->待关闭列表_结.德尔塔_变量) - 1) * 8)) - 1)


/*
** Insert a variable in the 列表_变量 of 到_变量-be-closed variables.
*/
void 月亮函数_新待关闭上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量) {
  限制_月亮_断言_宏名(层级_变量 > L->待关闭列表_结.p);
  if (对象_l_是否假_宏名(对象_s到v_宏名(层级_变量)))
    return;  /* false doesn't need 到_变量 be closed */
  月函_检查关闭方法_函(L, 层级_变量);  /* 值_圆 must have a 关闭_圆 method */
  while (限制_类型转换_无符整型_宏名(层级_变量 - L->待关闭列表_结.p) > 函_最大德尔塔_宏名) {
    L->待关闭列表_结.p += 函_最大德尔塔_宏名;  /* create a 虚假_变量 节点_变量 at maximum 德尔塔_变量 */
    L->待关闭列表_结.p->待关闭列表_结.德尔塔_变量 = 0;
  }
  层级_变量->待关闭列表_结.德尔塔_变量 = 限制_类型转换_宏名(unsigned short, 层级_变量 - L->待关闭列表_结.p);
  L->待关闭列表_结.p = 层级_变量;
}


void 月亮函数_解除链上值_函 (上值_结 *上值_缩变量) {
  限制_月亮_断言_宏名(函_上是否打开_宏名(上值_缩变量));
  *上值_缩变量->u.打开_圆.前一个_变量 = 上值_缩变量->u.打开_圆.下一个_变量;
  if (上值_缩变量->u.打开_圆.下一个_变量)
    上值_缩变量->u.打开_圆.下一个_变量->u.打开_圆.前一个_变量 = 上值_缩变量->u.打开_圆.前一个_变量;
}


/*
** Close all 上值们_小写 上_小变量 到_变量 the given 栈_圆小 层级_变量.
*/
void 月亮函数_关闭上值_函 (炉_状态机结 *L, 栈身份_型 层级_变量) {
  上值_结 *上值_缩变量;
  栈身份_型 上列_缩变量;  /* 栈_圆小 index pointed by '上值_缩变量' */
  while ((上值_缩变量 = L->打开上值_圆小) != NULL && (上列_缩变量 = 函_上层级_宏名(上值_缩变量)) >= 层级_变量) {
    标签值_结 *插槽_变量 = &上值_缩变量->u.值_圆;  /* new position for 值_圆 */
    限制_月亮_断言_宏名(函_上层级_宏名(上值_缩变量) < L->顶部_变量.p);
    月亮函数_解除链上值_函(上值_缩变量);  /* remove 上值_圆 from '打开上值_圆小' 列表_变量 */
    对象_设置对象_宏名(L, 插槽_变量, 上值_缩变量->v.p);  /* move 值_圆 到_变量 上值_圆 插槽_变量 */
    上值_缩变量->v.p = 插槽_变量;  /* now 当前_圆 值_圆 lives here */
    if (!垃圾回收_是否白色_宏名(上值_缩变量)) {  /* neither 白色_变量 nor dead? */
      垃圾回收_新白到黑_宏名(上值_缩变量);  /* closed 上值们_小写 月编译器_不能_函 be 灰的色_圆 */
      垃圾回收_月亮C_屏障_宏名(L, 上值_缩变量, 插槽_变量);
    }
  }
}


/*
** Remove 首先_变量 element from the 待关闭列表_结 plus its 虚假_变量 nodes.
*/
static void 月函_弹出待关闭列表_函 (炉_状态机结 *L) {
  栈身份_型 待关闭_缩变量 = L->待关闭列表_结.p;
  限制_月亮_断言_宏名(待关闭_缩变量->待关闭列表_结.德尔塔_变量 > 0);  /* 首先_变量 element 月编译器_不能_函 be 虚假_变量 */
  待关闭_缩变量 -= 待关闭_缩变量->待关闭列表_结.德尔塔_变量;
  while (待关闭_缩变量 > L->栈_圆小.p && 待关闭_缩变量->待关闭列表_结.德尔塔_变量 == 0)
    待关闭_缩变量 -= 函_最大德尔塔_宏名;  /* remove 虚假_变量 nodes */
  L->待关闭列表_结.p = 待关闭_缩变量;
}


/*
** Close all 上值们_小写 and 到_变量-be-closed variables 上_小变量 到_变量 the given 栈_圆小
** 层级_变量. Return restored '层级_变量'.
*/
栈身份_型 月亮函数_关闭_函 (炉_状态机结 *L, 栈身份_型 层级_变量, int 状态码_变量, int yy) {
  ptrdiff_t levelrel = 做_保存栈_宏名(L, 层级_变量);
  月亮函数_关闭上值_函(L, 层级_变量);  /* 首先_变量, 关闭_圆 the 上值们_小写 */
  while (L->待关闭列表_结.p >= 层级_变量) {  /* traverse 待关闭_缩变量's down 到_变量 that 层级_变量 */
    栈身份_型 待关闭_缩变量 = L->待关闭列表_结.p;  /* get variable index */
    月函_弹出待关闭列表_函(L);  /* remove it from 列表_变量 */
    月函_预备保护调用关闭方法_函(L, 待关闭_缩变量, 状态码_变量, yy);  /* 关闭_圆 variable */
    层级_变量 = 做_恢复栈_宏名(L, levelrel);
  }
  return 层级_变量;
}


原型_结 *月亮函数_新原型_函 (炉_状态机结 *L) {
  垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_V原型_宏名, sizeof(原型_结));
  原型_结 *f = 状态机_垃圾回收对象到P_宏名(o);
  f->k = NULL;
  f->k大小_缩 = 0;
  f->p = NULL;
  f->p大小_缩 = 0;
  f->代码_变量 = NULL;
  f->代码大小_小写 = 0;
  f->行信息_变量 = NULL;
  f->行信息大小_小写 = 0;
  f->绝对行信息_小写 = NULL;
  f->绝对行信息大小_小写 = 0;
  f->上值们_小写 = NULL;
  f->上值大小_小写 = 0;
  f->形参数_小写 = 0;
  f->是否_变量实参短 = 0;
  f->最大栈大小_小写 = 0;
  f->本地变量们_短 = NULL;
  f->本地变量大小_短 = 0;
  f->已定义行_小写 = 0;
  f->最后已定义行_小写 = 0;
  f->源_圆 = NULL;
  return f;
}


void 月亮函数_释放原型_函 (炉_状态机结 *L, 原型_结 *f) {
  内存_月亮M_释放数组_宏名(L, f->代码_变量, f->代码大小_小写);
  内存_月亮M_释放数组_宏名(L, f->p, f->p大小_缩);
  内存_月亮M_释放数组_宏名(L, f->k, f->k大小_缩);
  内存_月亮M_释放数组_宏名(L, f->行信息_变量, f->行信息大小_小写);
  内存_月亮M_释放数组_宏名(L, f->绝对行信息_小写, f->绝对行信息大小_小写);
  内存_月亮M_释放数组_宏名(L, f->本地变量们_短, f->本地变量大小_短);
  内存_月亮M_释放数组_宏名(L, f->上值们_小写, f->上值大小_小写);
  内存_月亮M_释放_宏名(L, f);
}


/*
** Look for n-到钩_变量 local variable at 行_变量 '行_变量' in function '函_短变量'.
** Returns NULL if not found.
*/
const char *月亮函数_获取本地名_函 (const 原型_结 *f, int local_number, int 程序计数_变量) {
  int i;
  for (i = 0; i<f->本地变量大小_短 && f->本地变量们_短[i].始程计_缩 <= 程序计数_变量; i++) {
    if (程序计数_变量 < f->本地变量们_短[i].终程计_缩) {  /* is variable active? */
      local_number--;
      if (local_number == 0)
        return 对象_获取串_宏名(f->本地变量们_短[i].变量名称_变量);
    }
  }
  return NULL;  /* not found */
}

