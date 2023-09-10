/*
** $Id: lmem.c $
** Interface 到_变量 Memory Manager
** See Copyright Notice in lua.h
*/

#define lmem_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <stddef.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"



/*
** About the realloc function:
** void *函重新分配_缩 (void *用数_缩变量, void *ptr, size_t 原始大小_变量, size_t 新大小_变量);
** ('原始大小_变量' is the 旧_变量 大小_变量, '新大小_变量' is the new 大小_变量)
**
** - 函重新分配_缩(用数_缩变量, p, x, 0) frees the 月解析器_块_函 'p' and returns NULL.
** Particularly, 函重新分配_缩(用数_缩变量, NULL, 0, 0) does nothing,
** which is equivalent 到_变量 free(NULL) in ISO C.
**
** - 函重新分配_缩(用数_缩变量, NULL, x, s) creates a new 月解析器_块_函 of 大小_变量 's'
** (no matter 'x'). Returns NULL if it 月编译器_不能_函 create the new 月解析器_块_函.
**
** - otherwise, 函重新分配_缩(用数_缩变量, b, x, y) reallocates the 月解析器_块_函 'b' from
** 大小_变量 'x' 到_变量 大小_变量 'y'. Returns NULL if it 月编译器_不能_函 reallocate the
** 月解析器_块_函 到_变量 the new 大小_变量.
*/


/*
** Macro 到_变量 call the allocation function.
*/
#define 内存_调用释放分配_宏名(g,月解析器_块_函,os,ns)    ((*g->函重新分配_缩)(g->用数_缩变量, 月解析器_块_函, os, ns))


/*
** When an allocation fails, it will try again after an emergency
** collection, except when it 月编译器_不能_函 run a collection.  The GC should
** not be called while the 状态机_变量 is not fully built, as the collector
** is not yet fully initialized. Also, it should not be called when
** '垃回紧急停_缩' is true, because then the interpreter is in the middle of
** a collection 步进_变量.
*/
#define 内存_能再次尝试_宏名(g)	(状态机_完成状态_宏名(g) && !g->垃回紧急停_缩)




#if defined(EMERGENCYGCTESTS)
/*
** First allocation will fail except when freeing a 月解析器_块_函 (frees never
** fail) and when it 月编译器_不能_函 try again; this fail will trigger '月内存_再次尝试_函'
** and a full GC cycle at every allocation.
*/
static void *内存_首次尝试_宏名 (全局_状态机结 *g, void *月解析器_块_函, size_t os, size_t ns) {
  if (ns > 0 && 内存_能再次尝试_宏名(g))
    return NULL;  /* fail */
  else  /* normal allocation */
    return 内存_调用释放分配_宏名(g, 月解析器_块_函, os, ns);
}
#else
#define 内存_首次尝试_宏名(g,月解析器_块_函,os,ns)    内存_调用释放分配_宏名(g, 月解析器_块_函, os, ns)
#endif





/*
** {==================================================================
** Functions 到_变量 allocate/deallocate arrays for the Parser
** ===================================================================
*/

/*
** Minimum 大小_变量 for arrays during parsing, 到_变量 avoid overhead of
** reallocating 到_变量 大小_变量 1, then 2, and then 4. All these arrays
** will be reallocated 到_变量 exact sizes or erased when parsing ends.
*/
#define 内存_最小数组大小_宏名	4


void *月亮内存_增长辅助_函 (炉_状态机结 *L, void *月解析器_块_函, int nelems, int *psize,
                     int size_elems, int 限制_变量, const char *什么_变量) {
  void *新块_变量;
  int 大小_变量 = *psize;
  if (nelems + 1 <= 大小_变量)  /* does one 额外_变量 element still fit? */
    return 月解析器_块_函;  /* nothing 到_变量 be done */
  if (大小_变量 >= 限制_变量 / 2) {  /* 月编译器_不能_函 double it? */
    if (配置_l_可能性低_宏名(大小_变量 >= 限制_变量))  /* 月编译器_不能_函 grow even a little? */
      月亮全局_跑错误_函(L, "too many %s (限制_变量 is %d)", 什么_变量, 限制_变量);
    大小_变量 = 限制_变量;  /* still have at least one free place */
  }
  else {
    大小_变量 *= 2;
    if (大小_变量 < 内存_最小数组大小_宏名)
      大小_变量 = 内存_最小数组大小_宏名;  /* minimum 大小_变量 */
  }
  限制_月亮_断言_宏名(nelems + 1 <= 大小_变量 && 大小_变量 <= 限制_变量);
  /* '限制_变量' ensures that multiplication will not overflow */
  新块_变量 = 月亮内存_安全重新分配_函(L, 月解析器_块_函, 限制_类型转换_大小t_宏名(*psize) * size_elems,
                                         限制_类型转换_大小t_宏名(大小_变量) * size_elems);
  *psize = 大小_变量;  /* update only when everything else is OK */
  return 新块_变量;
}


/*
** In prototypes, the 大小_变量 of the 数组_圆 is also its number of
** elements (到_变量 月词法_保存_函 memory). So, if it 月编译器_不能_函 shrink an 数组_圆
** 到_变量 its number of elements, the only 选项_变量 is 到_变量 raise an
** 错误_小变量.
*/
void *月亮内存_收缩向量_函 (炉_状态机结 *L, void *月解析器_块_函, int *大小_变量,
                          int final_n, int size_elem) {
  void *新块_变量;
  size_t 旧大小_变量 = 限制_类型转换_大小t_宏名((*大小_变量) * size_elem);
  size_t 新大小_变量 = 限制_类型转换_大小t_宏名(final_n * size_elem);
  限制_月亮_断言_宏名(新大小_变量 <= 旧大小_变量);
  新块_变量 = 月亮内存_安全重新分配_函(L, 月解析器_块_函, 旧大小_变量, 新大小_变量);
  *大小_变量 = final_n;
  return 新块_变量;
}

/* }================================================================== */


限制_l_无返回值_宏名 月亮内存_太大_函 (炉_状态机结 *L) {
  月亮全局_跑错误_函(L, "memory allocation 错误_小变量: 月解析器_块_函 too big");
}


/*
** Free memory
*/
void 月亮内存_释放_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 原始大小_变量) {
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名((原始大小_变量 == 0) == (月解析器_块_函 == NULL));
  内存_调用释放分配_宏名(g, 月解析器_块_函, 原始大小_变量, 0);
  g->垃回债_缩 -= 原始大小_变量;
}


/*
** In case of allocation fail, this function will do an emergency
** collection 到_变量 free some memory and then try the allocation again.
*/
static void *月内存_再次尝试_函 (炉_状态机结 *L, void *月解析器_块_函,
                       size_t 原始大小_变量, size_t 新大小_变量) {
  全局_状态机结 *g = G(L);
  if (内存_能再次尝试_宏名(g)) {
    月亮编译_完全垃圾回收_函(L, 1);  /* try 到_变量 free some memory... */
    return 内存_调用释放分配_宏名(g, 月解析器_块_函, 原始大小_变量, 新大小_变量);  /* try again */
  }
  else return NULL;  /* 月编译器_不能_函 run an emergency collection */
}


/*
** Generic allocation routine.
*/
void *月亮内存_重新分配_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 原始大小_变量, size_t 新大小_变量) {
  void *新块_变量;
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名((原始大小_变量 == 0) == (月解析器_块_函 == NULL));
  新块_变量 = 内存_首次尝试_宏名(g, 月解析器_块_函, 原始大小_变量, 新大小_变量);
  if (配置_l_可能性低_宏名(新块_变量 == NULL && 新大小_变量 > 0)) {
    新块_变量 = 月内存_再次尝试_函(L, 月解析器_块_函, 原始大小_变量, 新大小_变量);
    if (新块_变量 == NULL)  /* still no memory? */
      return NULL;  /* do not update '垃回债_缩' */
  }
  限制_月亮_断言_宏名((新大小_变量 == 0) == (新块_变量 == NULL));
  g->垃回债_缩 = (g->垃回债_缩 + 新大小_变量) - 原始大小_变量;
  return 新块_变量;
}


void *月亮内存_安全重新分配_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 原始大小_变量,
                                                    size_t 新大小_变量) {
  void *新块_变量 = 月亮内存_重新分配_函(L, 月解析器_块_函, 原始大小_变量, 新大小_变量);
  if (配置_l_可能性低_宏名(新块_变量 == NULL && 新大小_变量 > 0))  /* allocation failed? */
    内存_月亮M_错误_宏名(L);
  return 新块_变量;
}


void *月亮内存_内存分配_函 (炉_状态机结 *L, size_t 大小_变量, int tag) {
  if (大小_变量 == 0)
    return NULL;  /* that's all */
  else {
    全局_状态机结 *g = G(L);
    void *新块_变量 = 内存_首次尝试_宏名(g, NULL, tag, 大小_变量);
    if (配置_l_可能性低_宏名(新块_变量 == NULL)) {
      新块_变量 = 月内存_再次尝试_函(L, NULL, tag, 大小_变量);
      if (新块_变量 == NULL)
        内存_月亮M_错误_宏名(L);
    }
    g->垃回债_缩 += 大小_变量;
    return 新块_变量;
  }
}
