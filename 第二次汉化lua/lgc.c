/*
** $Id: lgc.c $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#define lgc_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"

#include <stdio.h>
#include <string.h>


#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "ltm.h"


/*
** Maximum number of elements 到_变量 sweep in each single 步进_变量.
** (Large enough 到_变量 dissipate fixed overheads but small enough
** 到_变量 allow small steps for the collector.)
*/
#define 垃圾回收_GC扫除最大_宏名	100

/*
** Maximum number of finalizers 到_变量 call in each single 步进_变量.
*/
#define 垃圾回收_GC终结最大_宏名	10


/*
** Cost of calling one finalizer.
*/
#define 垃圾回收_GC最终执行成本_宏名	50


/*
** The equivalent, in bytes, of one unit of "工作_变量" (visiting a 插槽_变量,
** sweeping an object, etc.)
*/
#define 垃圾回收_工作到内存_宏名	sizeof(标签值_结)


/*
** macro 到_变量 adjust '暂停_变量': '暂停_变量' is actually used like
** '暂停_变量 / 垃圾回收_暂停调整_宏名' (值_圆 chosen by tests)
*/
#define 垃圾回收_暂停调整_宏名		100


/* 掩码_变量 with all color bits */
#define 垃圾回收_颜色们掩码_宏名	(垃圾回收_位掩码_宏名(垃圾回收_黑色位_宏名) | 垃圾回收_白色位_宏名)

/* 掩码_变量 with all GC bits */
#define 垃圾回收_GC位们掩码_宏名      (垃圾回收_颜色们掩码_宏名 | 垃圾回收_年龄位们_宏名)


/* macro 到_变量 erase all color bits then set only the 当前_圆 白色_变量 bit */
#define 垃圾回收_制作白色_宏名(g,x)	\
  (x->已记号_变量 = 限制_类型转换_字节_宏名((x->已记号_变量 & ~垃圾回收_颜色们掩码_宏名) | 垃圾回收_月亮C_白色_宏名(g)))

/* make an object 灰的色_圆 (neither 白色_变量 nor black) */
#define 垃圾回收_设置为灰色_宏名(x)	垃圾回收_重置位们_宏名(x->已记号_变量, 垃圾回收_颜色们掩码_宏名)


/* make an object black (coming from any color) */
#define 垃圾回收_设置为黑色_宏名(x)  \
  (x->已记号_变量 = 限制_类型转换_字节_宏名((x->已记号_变量 & ~垃圾回收_白色位_宏名) | 垃圾回收_位掩码_宏名(垃圾回收_黑色位_宏名)))


#define 垃圾回收_值为白色_宏名(x)   (对象_是否可收集_宏名(x) && 垃圾回收_是否白色_宏名(对象_垃圾回收值_宏名(x)))

#define 垃圾回收_键为白色_宏名(n)   (对象_键是否可回收_宏名(n) && 垃圾回收_是否白色_宏名(对象_垃圾回收键_宏名(n)))


/*
** Protected access 到_变量 objects in values
*/
#define 垃圾回收_gc值N_宏名(o)     (对象_是否可收集_宏名(o) ? 对象_垃圾回收值_宏名(o) : NULL)


#define 垃圾回收_记号值_宏名(g,o) { 对象_检查存活性_宏名(g->主线程_圆,o); \
  if (垃圾回收_值为白色_宏名(o)) 月垃圾回收_真实记号对象_函(g,对象_垃圾回收值_宏名(o)); }

#define 垃圾回收_记号键_宏名(g, n)	{ if 垃圾回收_键为白色_宏名(n) 月垃圾回收_真实记号对象_函(g,对象_垃圾回收键_宏名(n)); }

#define 垃圾回收_记号对象_宏名(g,t)	{ if (垃圾回收_是否白色_宏名(t)) 月垃圾回收_真实记号对象_函(g, 状态机_对象到垃圾回收对象_宏名(t)); }

/*
** 记号_变量 an object that can be NULL (either because it is really optional,
** or it was stripped as debug 信息_短变量, or inside an uncompleted structure)
*/
#define 垃圾回收_记号对象N_宏名(g,t)	{ if (t) 垃圾回收_记号对象_宏名(g,t); }

static void 月垃圾回收_真实记号对象_函 (全局_状态机结 *g, 垃回对象_结 *o);
static lu_mem 月垃圾回收_原子_函 (炉_状态机结 *L);
static void 月垃圾回收_进入扫除_函 (炉_状态机结 *L);


/*
** {======================================================
** Generic functions
** =======================================================
*/


/*
** one after 最后_变量 element in a 哈希_小写 数组_圆
*/
#define 垃圾回收_g节点最后_宏名(h)	表_全局节点_宏名(h, 限制_类型转换_大小t_宏名(对象_节点大小_宏名(h)))


static 垃回对象_结 **月垃圾回收_获取GC列表_函 (垃回对象_结 *o) {
  switch (o->类标_缩变量) {
    case 对象_月亮_V表_宏名: return &状态机_垃圾回收对象到t_宏名(o)->垃回列表_缩;
    case 对象_月亮_VL闭包L_宏名: return &状态机_垃圾回收对象到l闭包_宏名(o)->垃回列表_缩;
    case 对象_月亮_VC闭包L_宏名: return &状态机_垃圾回收对象到c闭包_宏名(o)->垃回列表_缩;
    case 对象_月亮_V线程_宏名: return &状态机_垃圾回收对象到th_宏名(o)->垃回列表_缩;
    case 对象_月亮_V原型_宏名: return &状态机_垃圾回收对象到P_宏名(o)->垃回列表_缩;
    case 对象_月亮_V用户数据_宏名: {
      用户数据_结 *u = 状态机_垃圾回收对象到u_宏名(o);
      限制_月亮_断言_宏名(u->用户值数_缩 > 0);
      return &u->垃回列表_缩;
    }
    default: 限制_月亮_断言_宏名(0); return 0;
  }
}


/*
** Link a collectable object 'o' with a known type into the 列表_变量 'p'.
** (Must be a macro 到_变量 access the '垃回列表_缩' 月解析器_字段_函 in different types.)
*/
#define 垃圾回收_链接GC列表_宏名(o,p)	月垃圾回收_链接GC列表_函(状态机_对象到垃圾回收对象_宏名(o), &(o)->垃回列表_缩, &(p))

static void 月垃圾回收_链接GC列表_函 (垃回对象_结 *o, 垃回对象_结 **pnext, 垃回对象_结 **列表_变量) {
  限制_月亮_断言_宏名(!垃圾回收_是否灰色_宏名(o));  /* 月编译器_不能_函 be in a 灰的色_圆 列表_变量 */
  *pnext = *列表_变量;
  *列表_变量 = o;
  垃圾回收_设置为灰色_宏名(o);  /* now it is */
}


/*
** Link a generic collectable object 'o' into the 列表_变量 'p'.
*/
#define 垃圾回收_链接对象GC列表_宏名(o,p) 月垃圾回收_链接GC列表_函(状态机_对象到垃圾回收对象_宏名(o), 月垃圾回收_获取GC列表_函(o), &(p))



/*
** Clear keys for 空容器_变量 entries in tables. If entry is 空容器_变量, 记号_变量 its
** entry as dead. This allows the collection of the 键_小变量, but keeps its
** entry in the table: its removal could break a chain and could break
** a table traversal.  Other places never manipulate dead keys, because
** its associated 空容器_变量 值_圆 is enough 到_变量 signal that the entry is
** logically 空容器_变量.
*/
static void 月垃圾回收_清理键_函 (节点_联 *n) {
  限制_月亮_断言_宏名(对象_是否空容器_宏名(表_全局值_宏名(n)));
  if (对象_键是否可回收_宏名(n))
    对象_设置死键_宏名(n);  /* unused 键_小变量; remove it */
}


/*
** tells whether a 键_小变量 or 值_圆 can be cleared from a 弱的_圆
** table. Non-collectable objects are never removed from 弱的_圆
** tables. Strings behave as 'values', so are never removed too. for
** other objects: if really collected, 月编译器_不能_函 keep them; for objects
** being finalized, keep them in keys, but not in values
*/
static int 月垃圾回收_是否已清理_函 (全局_状态机结 *g, const 垃回对象_结 *o) {
  if (o == NULL) return 0;  /* non-collectable 值_圆 */
  else if (对象_无变体_宏名(o->类标_缩变量) == 月头_月亮_T字符串_宏名) {
    垃圾回收_记号对象_宏名(g, o);  /* strings are 'values', so are never 弱的_圆 */
    return 0;
  }
  else return 垃圾回收_是否白色_宏名(o);
}


/*
** Barrier that moves collector forward, that is, marks the 白色_变量 object
** 'v' being pointed by the black object 'o'.  In the generational
** 模块_变量, 'v' must also become 旧_变量, if 'o' is 旧_变量; however, it 月编译器_不能_函
** be 已更改_变量 directly 到_变量 OLD, because it may still 针点_变量 到_变量 non-旧_变量
** objects. So, it is 已记号_变量 as OLD0. In the 下一个_变量 cycle it will become
** OLD1, and in the 下一个_变量 it will finally become OLD (regular 旧_变量). By
** then, any object it points 到_变量 will also be 旧_变量.  If called in the
** incremental sweep phase, it clears the black object 到_变量 白色_变量 (sweep
** it) 到_变量 avoid other barrier calls for this same object. (That 月编译器_不能_函
** be done is generational 模块_变量, as its sweep does not distinguish
** whites from deads.)
*/
void 月亮编译_屏障_函 (炉_状态机结 *L, 垃回对象_结 *o, 垃回对象_结 *v) {
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名(垃圾回收_是否黑色_宏名(o) && 垃圾回收_是否白色_宏名(v) && !垃圾回收_是否死亡_宏名(g, v) && !垃圾回收_是否死亡_宏名(g, o));
  if (垃圾回收_保持不变_宏名(g)) {  /* must keep invariant? */
    月垃圾回收_真实记号对象_函(g, v);  /* restore invariant */
    if (垃圾回收_是否旧_宏名(o)) {
      限制_月亮_断言_宏名(!垃圾回收_是否旧_宏名(v));  /* 白色_变量 object could not be 旧_变量 */
      垃圾回收_设置年龄_宏名(v, 垃圾回收_G_旧0_宏名);  /* restore generational invariant */
    }
  }
  else {  /* sweep phase */
    限制_月亮_断言_宏名(垃圾回收_是否扫除阶段_宏名(g));
    if (g->垃回种类_缩小写 == 状态机_K垃圾回收_递增_宏名)  /* incremental 模块_变量? */
      垃圾回收_制作白色_宏名(g, o);  /* 记号_变量 'o' as 白色_变量 到_变量 avoid other barriers */
  }
}


/*
** barrier that moves collector backward, that is, 记号_变量 the black object
** pointing 到_变量 a 白色_变量 object as 灰的色_圆 again.
*/
void 月亮编译_屏障回退_函 (炉_状态机结 *L, 垃回对象_结 *o) {
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名(垃圾回收_是否黑色_宏名(o) && !垃圾回收_是否死亡_宏名(g, o));
  限制_月亮_断言_宏名((g->垃回种类_缩小写 == 状态机_K垃圾回收_生成_宏名) == (垃圾回收_是否旧_宏名(o) && 垃圾回收_获取年龄_宏名(o) != 垃圾回收_G_已摸1_宏名));
  if (垃圾回收_获取年龄_宏名(o) == 垃圾回收_G_已摸2_宏名)  /* already in 灰的色_圆 列表_变量? */
    垃圾回收_设置为灰色_宏名(o);  /* make it 灰的色_圆 到_变量 become touched1 */
  else  /* link it in '再次灰色_变量' and paint it 灰的色_圆 */
    垃圾回收_链接对象GC列表_宏名(o, g->再次灰色_变量);
  if (垃圾回收_是否旧_宏名(o))  /* generational 模块_变量? */
    垃圾回收_设置年龄_宏名(o, 垃圾回收_G_已摸1_宏名);  /* touched in 当前_圆 cycle */
}


void 月亮编译_修复_函 (炉_状态机结 *L, 垃回对象_结 *o) {
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名(g->全部垃回_圆缩 == o);  /* object must be 1st in '全部垃回_圆缩' 列表_变量! */
  垃圾回收_设置为灰色_宏名(o);  /* they will be 灰的色_圆 forever */
  垃圾回收_设置年龄_宏名(o, 垃圾回收_G_旧_宏名);  /* and 旧_变量 forever */
  g->全部垃回_圆缩 = o->下一个_变量;  /* remove object from '全部垃回_圆缩' 列表_变量 */
  o->下一个_变量 = g->固定垃回_圆缩;  /* link it 到_变量 '固定垃回_圆缩' 列表_变量 */
  g->固定垃回_圆缩 = o;
}


/*
** create a new collectable object (with given type, 大小_变量, and 偏移_变量)
** and link it 到_变量 '全部垃回_圆缩' 列表_变量.
*/
垃回对象_结 *月亮编译_新对象dt_函 (炉_状态机结 *L, int 类标_缩变量, size_t 大小_短变量, size_t 偏移_变量) {
  全局_状态机结 *g = G(L);
  char *p = 限制_类型转换_印刻指针_宏名(内存_月亮M_新对象_宏名(L, 对象_无变体_宏名(类标_缩变量), 大小_短变量));
  垃回对象_结 *o = 限制_类型转换_宏名(垃回对象_结 *, p + 偏移_变量);
  o->已记号_变量 = 垃圾回收_月亮C_白色_宏名(g);
  o->类标_缩变量 = 类标_缩变量;
  o->下一个_变量 = g->全部垃回_圆缩;
  g->全部垃回_圆缩 = o;
  return o;
}


垃回对象_结 *月亮编译_新对象_函 (炉_状态机结 *L, int 类标_缩变量, size_t 大小_短变量) {
  return 月亮编译_新对象dt_函(L, 类标_缩变量, 大小_短变量, 0);
}

/* }====================================================== */



/*
** {======================================================
** Mark functions
** =======================================================
*/


/*
** Mark an object.  Userdata with no user values, strings, and closed
** 上值们_小写 are visited and turned black here.  Open 上值们_小写 are
** already indirectly linked through their respective threads in the
** '线程与上值_短缩' 列表_变量, so they don't go 到_变量 the 灰的色_圆 列表_变量; nevertheless, they
** are kept 灰的色_圆 到_变量 avoid barriers, as their values will be revisited
** by the 线程_变量 or by '月垃圾回收_重新记号上值们_函'.  Other objects are added 到_变量 the
** 灰的色_圆 列表_变量 到_变量 be visited (and turned black) later.  Both userdata and
** 上值们_小写 can call this function recursively, but this recursion goes
** for at most two levels: An 上值_圆 月编译器_不能_函 refer 到_变量 another 上值_圆
** (only closures can), and a userdata's 元表_小写 must be a table.
*/
static void 月垃圾回收_真实记号对象_函 (全局_状态机结 *g, 垃回对象_结 *o) {
  switch (o->类标_缩变量) {
    case 对象_月亮_V短型串_宏名:
    case 对象_月亮_V长型串_宏名: {
      垃圾回收_设置为黑色_宏名(o);  /* nothing 到_变量 visit */
      break;
    }
    case 对象_月亮_V上值_宏名: {
      上值_结 *上值_缩变量 = 状态机_垃圾回收对象到上值_宏名(o);
      if (函_上是否打开_宏名(上值_缩变量))
        垃圾回收_设置为灰色_宏名(上值_缩变量);  /* 打开_圆 上值们_小写 are kept 灰的色_圆 */
      else
        垃圾回收_设置为黑色_宏名(上值_缩变量);  /* closed 上值们_小写 are visited here */
      垃圾回收_记号值_宏名(g, 上值_缩变量->v.p);  /* 记号_变量 its content */
      break;
    }
    case 对象_月亮_V用户数据_宏名: {
      用户数据_结 *u = 状态机_垃圾回收对象到u_宏名(o);
      if (u->用户值数_缩 == 0) {  /* no user values? */
        垃圾回收_记号对象N_宏名(g, u->元表_小写);  /* 记号_变量 its 元表_小写 */
        垃圾回收_设置为黑色_宏名(u);  /* nothing else 到_变量 记号_变量 */
        break;
      }
      /* else... */
    }  /* FALLTHROUGH */
    case 对象_月亮_VL闭包L_宏名: case 对象_月亮_VC闭包L_宏名: case 对象_月亮_V表_宏名:
    case 对象_月亮_V线程_宏名: case 对象_月亮_V原型_宏名: {
      垃圾回收_链接对象GC列表_宏名(o, g->灰的色_圆);  /* 到_变量 be visited later */
      break;
    }
    default: 限制_月亮_断言_宏名(0); break;
  }
}


/*
** 记号_变量 metamethods for basic types
*/
static void 月垃圾回收_记号元表_函 (全局_状态机结 *g) {
  int i;
  for (i=0; i < 月头_月亮_标签数目_宏名; i++)
    垃圾回收_记号对象N_宏名(g, g->元表_缩变量[i]);
}


/*
** 记号_变量 all objects in 列表_变量 of being-finalized
*/
static lu_mem 月垃圾回收_记号存在的fnz_函 (全局_状态机结 *g) {
  垃回对象_结 *o;
  lu_mem 计数_变量 = 0;
  for (o = g->待fnz_缩半; o != NULL; o = o->下一个_变量) {
    计数_变量++;
    垃圾回收_记号对象_宏名(g, o);
  }
  return 计数_变量;
}


/*
** For each non-已记号_变量 线程_变量, simulates a barrier between each 打开_圆
** 上值_圆 and its 值_圆. (If the 线程_变量 is collected, the 值_圆 will be
** assigned 到_变量 the 上值_圆, but then it can be too late for the barrier
** 到_变量 act. The "barrier" does not need 到_变量 月解析器_检查_函 colors: A non-已记号_变量
** 线程_变量 must be young; 上值们_小写 月编译器_不能_函 be older than their threads; so
** any visited 上值_圆 must be young too.) Also removes the 线程_变量 from
** the 列表_变量, as it was already visited. Removes also threads with no
** 上值们_小写, as they have nothing 到_变量 be checked. (If the 线程_变量 gets an
** 上值_圆 later, it will be linked in the 列表_变量 again.)
*/
static int 月垃圾回收_重新记号上值们_函 (全局_状态机结 *g) {
  炉_状态机结 *线程_变量;
  炉_状态机结 **p = &g->线程与上值_短缩;
  int 工作_变量 = 0;  /* 估计_变量 of how much 工作_变量 was done here */
  while ((线程_变量 = *p) != NULL) {
    工作_变量++;
    if (!垃圾回收_是否白色_宏名(线程_变量) && 线程_变量->打开上值_圆小 != NULL)
      p = &线程_变量->线程与上值_短缩;  /* keep 已记号_变量 线程_变量 with 上值们_小写 in the 列表_变量 */
    else {  /* 线程_变量 is not 已记号_变量 or without 上值们_小写 */
      上值_结 *上值_缩变量;
      限制_月亮_断言_宏名(!垃圾回收_是否旧_宏名(线程_变量) || 线程_变量->打开上值_圆小 == NULL);
      *p = 线程_变量->线程与上值_短缩;  /* remove 线程_变量 from the 列表_变量 */
      线程_变量->线程与上值_短缩 = 线程_变量;  /* 记号_变量 that it is out of 列表_变量 */
      for (上值_缩变量 = 线程_变量->打开上值_圆小; 上值_缩变量 != NULL; 上值_缩变量 = 上值_缩变量->u.打开_圆.下一个_变量) {
        限制_月亮_断言_宏名(垃圾回收_获取年龄_宏名(上值_缩变量) <= 垃圾回收_获取年龄_宏名(线程_变量));
        工作_变量++;
        if (!垃圾回收_是否白色_宏名(上值_缩变量)) {  /* 上值_圆 already visited? */
          限制_月亮_断言_宏名(函_上是否打开_宏名(上值_缩变量) && 垃圾回收_是否灰色_宏名(上值_缩变量));
          垃圾回收_记号值_宏名(g, 上值_缩变量->v.p);  /* 记号_变量 its 值_圆 */
        }
      }
    }
  }
  return 工作_变量;
}


static void 月垃圾回收_清理灰色列表们_函 (全局_状态机结 *g) {
  g->灰的色_圆 = g->再次灰色_变量 = NULL;
  g->弱的_圆 = g->全部弱_圆 = g->短命的_圆 = NULL;
}


/*
** 记号_变量 root set and reset all 灰的色_圆 lists, 到_变量 起始_变量 a new collection
*/
static void 月垃圾回收_重新开始回收_函 (全局_状态机结 *g) {
  月垃圾回收_清理灰色列表们_函(g);
  垃圾回收_记号对象_宏名(g, g->主线程_圆);
  垃圾回收_记号值_宏名(g, &g->l_注册表半);
  月垃圾回收_记号元表_函(g);
  月垃圾回收_记号存在的fnz_函(g);  /* 记号_变量 any finalizing object 左_圆 from 前一个_变量 cycle */
}

/* }====================================================== */


/*
** {======================================================
** Traverse functions
** =======================================================
*/


/*
** Check whether object 'o' should be kept in the '再次灰色_变量' 列表_变量 for
** post-processing by '月垃圾回收_纠正灰列表_函'. (It could put all 旧_变量 objects
** in the 列表_变量 and leave all the 工作_变量 到_变量 '月垃圾回收_纠正灰列表_函', but it is
** 更多_变量 efficient 到_变量 avoid adding elements that will be removed.) Only
** TOUCHED1 objects need 到_变量 be in the 列表_变量. TOUCHED2 doesn't need 到_变量 go
** back 到_变量 a 灰的色_圆 列表_变量, but then it must become OLD. (That is 什么_变量
** '月垃圾回收_纠正灰列表_函' does when it finds a TOUCHED2 object.)
*/
static void 月垃圾回收_生成链接_函 (全局_状态机结 *g, 垃回对象_结 *o) {
  限制_月亮_断言_宏名(垃圾回收_是否黑色_宏名(o));
  if (垃圾回收_获取年龄_宏名(o) == 垃圾回收_G_已摸1_宏名) {  /* touched in this cycle? */
    垃圾回收_链接对象GC列表_宏名(o, g->再次灰色_变量);  /* link it back in '再次灰色_变量' */
  }  /* everything else do not need 到_变量 be linked back */
  else if (垃圾回收_获取年龄_宏名(o) == 垃圾回收_G_已摸2_宏名)
    垃圾回收_改变年龄_宏名(o, 垃圾回收_G_已摸2_宏名, 垃圾回收_G_旧_宏名);  /* advance age */
}


/*
** Traverse a table with 弱的_圆 values and link it 到_变量 proper 列表_变量. During
** propagate phase, keep it in '再次灰色_变量' 列表_变量, 到_变量 be revisited in the
** 月垃圾回收_原子_函 phase. In the 月垃圾回收_原子_函 phase, if table has any 白色_变量 值_圆,
** put it in '弱的_圆' 列表_变量, 到_变量 be cleared.
*/
static void 月垃圾回收_遍历弱值_函 (全局_状态机结 *g, 表_结 *h) {
  节点_联 *n, *限制_变量 = 垃圾回收_g节点最后_宏名(h);
  /* if there is 数组_圆 part, assume it may have 白色_变量 values (it is not
     worth traversing it now just 到_变量 月解析器_检查_函) */
  int 有清除_变量 = (h->数组限制_缩 > 0);
  for (n = 表_全局节点_宏名(h, 0); n < 限制_变量; n++) {  /* traverse 哈希_小写 part */
    if (对象_是否空容器_宏名(表_全局值_宏名(n)))  /* entry is 空容器_变量? */
      月垃圾回收_清理键_函(n);  /* clear its 键_小变量 */
    else {
      限制_月亮_断言_宏名(!对象_键是否空值_宏名(n));
      垃圾回收_记号键_宏名(g, n);
      if (!有清除_变量 && 月垃圾回收_是否已清理_函(g, 垃圾回收_gc值N_宏名(表_全局值_宏名(n))))  /* a 白色_变量 值_圆? */
        有清除_变量 = 1;  /* table will have 到_变量 be cleared */
    }
  }
  if (g->垃回状态机_缩小写 == 垃圾回收_GCS原子态_宏名 && 有清除_变量)
    垃圾回收_链接GC列表_宏名(h, g->弱的_圆);  /* has 到_变量 be cleared later */
  else
    垃圾回收_链接GC列表_宏名(h, g->再次灰色_变量);  /* must retraverse it in 月垃圾回收_原子_函 phase */
}


/*
** Traverse an 短命的_圆 table and link it 到_变量 proper 列表_变量. Returns true
** iff any object was 已记号_变量 during this traversal (which implies that
** convergence has 到_变量 continue). During propagation phase, keep table
** in '再次灰色_变量' 列表_变量, 到_变量 be visited again in the 月垃圾回收_原子_函 phase. In
** the 月垃圾回收_原子_函 phase, if table has any 白色_变量->白色_变量 entry, it has 到_变量
** be revisited during 短命的_圆 convergence (as that 键_小变量 may turn
** black). Otherwise, if it has any 白色_变量 键_小变量, table has 到_变量 be cleared
** (in the 月垃圾回收_原子_函 phase). In generational 模块_变量, some tables
** must be kept in some 灰的色_圆 列表_变量 for post-processing; this is done
** by '月垃圾回收_生成链接_函'.
*/
static int 月垃圾回收_遍历短暂_函 (全局_状态机结 *g, 表_结 *h, int inv) {
  int 已记号_变量 = 0;  /* true if an object is 已记号_变量 in this traversal */
  int 有清除_变量 = 0;  /* true if table has 白色_变量 keys */
  int 有ww_变量 = 0;  /* true if table has entry "白色_变量-键_小变量 -> 白色_变量-值_圆" */
  unsigned int i;
  unsigned int 数组大小_缩变量 = 月亮哈希表_真实a大小_函(h);
  unsigned int 新大小_变量 = 对象_节点大小_宏名(h);
  /* traverse 数组_圆 part */
  for (i = 0; i < 数组大小_缩变量; i++) {
    if (垃圾回收_值为白色_宏名(&h->数组_圆[i])) {
      已记号_变量 = 1;
      月垃圾回收_真实记号对象_函(g, 对象_垃圾回收值_宏名(&h->数组_圆[i]));
    }
  }
  /* traverse 哈希_小写 part; if 'inv', traverse descending
     (see '月垃圾回收_收敛短暂_函') */
  for (i = 0; i < 新大小_变量; i++) {
    节点_联 *n = inv ? 表_全局节点_宏名(h, 新大小_变量 - 1 - i) : 表_全局节点_宏名(h, i);
    if (对象_是否空容器_宏名(表_全局值_宏名(n)))  /* entry is 空容器_变量? */
      月垃圾回收_清理键_函(n);  /* clear its 键_小变量 */
    else if (月垃圾回收_是否已清理_函(g, 对象_垃圾回收键N_宏名(n))) {  /* 键_小变量 is not 已记号_变量 (yet)? */
      有清除_变量 = 1;  /* table must be cleared */
      if (垃圾回收_值为白色_宏名(表_全局值_宏名(n)))  /* 值_圆 not 已记号_变量 yet? */
        有ww_变量 = 1;  /* 白色_变量-白色_变量 entry */
    }
    else if (垃圾回收_值为白色_宏名(表_全局值_宏名(n))) {  /* 值_圆 not 已记号_变量 yet? */
      已记号_变量 = 1;
      月垃圾回收_真实记号对象_函(g, 对象_垃圾回收值_宏名(表_全局值_宏名(n)));  /* 记号_变量 it now */
    }
  }
  /* link table into proper 列表_变量 */
  if (g->垃回状态机_缩小写 == 垃圾回收_GCS传播_宏名)
    垃圾回收_链接GC列表_宏名(h, g->再次灰色_变量);  /* must retraverse it in 月垃圾回收_原子_函 phase */
  else if (有ww_变量)  /* table has 白色_变量->白色_变量 entries? */
    垃圾回收_链接GC列表_宏名(h, g->短命的_圆);  /* have 到_变量 propagate again */
  else if (有清除_变量)  /* table has 白色_变量 keys? */
    垃圾回收_链接GC列表_宏名(h, g->全部弱_圆);  /* may have 到_变量 clean 白色_变量 keys */
  else
    月垃圾回收_生成链接_函(g, 状态机_对象到垃圾回收对象_宏名(h));  /* 月解析器_检查_函 whether collector still needs 到_变量 see it */
  return 已记号_变量;
}


static void 月垃圾回收_遍历强表_函 (全局_状态机结 *g, 表_结 *h) {
  节点_联 *n, *限制_变量 = 垃圾回收_g节点最后_宏名(h);
  unsigned int i;
  unsigned int 数组大小_缩变量 = 月亮哈希表_真实a大小_函(h);
  for (i = 0; i < 数组大小_缩变量; i++)  /* traverse 数组_圆 part */
    垃圾回收_记号值_宏名(g, &h->数组_圆[i]);
  for (n = 表_全局节点_宏名(h, 0); n < 限制_变量; n++) {  /* traverse 哈希_小写 part */
    if (对象_是否空容器_宏名(表_全局值_宏名(n)))  /* entry is 空容器_变量? */
      月垃圾回收_清理键_函(n);  /* clear its 键_小变量 */
    else {
      限制_月亮_断言_宏名(!对象_键是否空值_宏名(n));
      垃圾回收_记号键_宏名(g, n);
      垃圾回收_记号值_宏名(g, 表_全局值_宏名(n));
    }
  }
  月垃圾回收_生成链接_函(g, 状态机_对象到垃圾回收对象_宏名(h));
}


static lu_mem 月垃圾回收_遍历表_函 (全局_状态机结 *g, 表_结 *h) {
  const char *弱键_变量, *弱值_变量;
  const 标签值_结 *模块_变量 = 标签方法_全局快速标方_宏名(g, h->元表_小写, 标方_模式大写);
  垃圾回收_记号对象N_宏名(g, h->元表_小写);
  if (模块_变量 && 对象_tt是否字符串_宏名(模块_变量) &&  /* is there a 弱的_圆 模块_变量? */
      (限制_类型转换_空的_宏名(弱键_变量 = strchr(对象_s值_宏名(模块_变量), 'k')),
       限制_类型转换_空的_宏名(弱值_变量 = strchr(对象_s值_宏名(模块_变量), 'v')),
       (弱键_变量 || 弱值_变量))) {  /* is really 弱的_圆? */
    if (!弱键_变量)  /* strong keys? */
      月垃圾回收_遍历弱值_函(g, h);
    else if (!弱值_变量)  /* strong values? */
      月垃圾回收_遍历短暂_函(g, h, 0);
    else  /* all 弱的_圆 */
      垃圾回收_链接GC列表_宏名(h, g->全部弱_圆);  /* nothing 到_变量 traverse now */
  }
  else  /* not 弱的_圆 */
    月垃圾回收_遍历强表_函(g, h);
  return 1 + h->数组限制_缩 + 2 * 表_分配节点大小_宏名(h);
}


static int 月垃圾回收_遍历用户数据_函 (全局_状态机结 *g, 用户数据_结 *u) {
  int i;
  垃圾回收_记号对象N_宏名(g, u->元表_小写);  /* 记号_变量 its 元表_小写 */
  for (i = 0; i < u->用户值数_缩; i++)
    垃圾回收_记号值_宏名(g, &u->上值_缩变量[i].上值_缩变量);
  月垃圾回收_生成链接_函(g, 状态机_对象到垃圾回收对象_宏名(u));
  return 1 + u->用户值数_缩;
}


/*
** Traverse a prototype. (While a prototype is being build, its
** arrays can be larger than 已需要_变量; the 额外_变量 slots are filled with
** NULL, so the use of '垃圾回收_记号对象N_宏名')
*/
static int 月垃圾回收_遍历原型_函 (全局_状态机结 *g, 原型_结 *f) {
  int i;
  垃圾回收_记号对象N_宏名(g, f->源_圆);
  for (i = 0; i < f->k大小_缩; i++)  /* 记号_变量 literals */
    垃圾回收_记号值_宏名(g, &f->k[i]);
  for (i = 0; i < f->上值大小_小写; i++)  /* 记号_变量 上值_圆 names */
    垃圾回收_记号对象N_宏名(g, f->上值们_小写[i].名称_变量);
  for (i = 0; i < f->p大小_缩; i++)  /* 记号_变量 nested protos */
    垃圾回收_记号对象N_宏名(g, f->p[i]);
  for (i = 0; i < f->本地变量大小_短; i++)  /* 记号_变量 local-variable names */
    垃圾回收_记号对象N_宏名(g, f->本地变量们_短[i].变量名称_变量);
  return 1 + f->k大小_缩 + f->上值大小_小写 + f->p大小_缩 + f->本地变量大小_短;
}


static int 月垃圾回收_遍历C闭包_函 (全局_状态机结 *g, C闭包_结 *闭包_短变量) {
  int i;
  for (i = 0; i < 闭包_短变量->nupvalues; i++)  /* 记号_变量 its 上值们_小写 */
    垃圾回收_记号值_宏名(g, &闭包_短变量->上值_圆[i]);
  return 1 + 闭包_短变量->nupvalues;
}

/*
** Traverse a Lua closure, marking its prototype and its 上值们_小写.
** (Both can be NULL while closure is being created.)
*/
static int 月垃圾回收_遍历L闭包_函 (全局_状态机结 *g, L闭包_结 *闭包_短变量) {
  int i;
  垃圾回收_记号对象N_宏名(g, 闭包_短变量->p);  /* 记号_变量 its prototype */
  for (i = 0; i < 闭包_短变量->nupvalues; i++) {  /* visit its 上值们_小写 */
    上值_结 *上值_缩变量 = 闭包_短变量->上值们_短[i];
    垃圾回收_记号对象N_宏名(g, 上值_缩变量);  /* 记号_变量 上值_圆 */
  }
  return 1 + 闭包_短变量->nupvalues;
}


/*
** Traverse a 线程_变量, marking the elements in the 栈_圆小 上_小变量 到_变量 its 顶部_变量
** and cleaning the rest of the 栈_圆小 in the 最终_变量 traversal. That
** ensures that the entire 栈_圆小 have 有效_变量 (non-dead) objects.
** Threads have no barriers. In gen. 模块_变量, 旧_变量 threads must be visited
** at every cycle, because they might 针点_变量 到_变量 young objects.  In inc.
** 模块_变量, the 线程_变量 can still be modified before the 终_变量 of the cycle,
** and therefore it must be visited again in the 月垃圾回收_原子_函 phase. To ensure
** these visits, threads must return 到_变量 a 灰的色_圆 列表_变量 if they are not new
** (which can only happen in generational 模块_变量) or if the traverse is in
** the propagate phase (which can only happen in incremental 模块_变量).
*/
static int 月垃圾回收_遍历线程_函 (全局_状态机结 *g, 炉_状态机结 *到钩_变量) {
  上值_结 *上值_缩变量;
  栈身份_型 o = 到钩_变量->栈_圆小.p;
  if (垃圾回收_是否旧_宏名(到钩_变量) || g->垃回状态机_缩小写 == 垃圾回收_GCS传播_宏名)
    垃圾回收_链接GC列表_宏名(到钩_变量, g->再次灰色_变量);  /* insert into '再次灰色_变量' 列表_变量 */
  if (o == NULL)
    return 1;  /* 栈_圆小 not completely built yet */
  限制_月亮_断言_宏名(g->垃回状态机_缩小写 == 垃圾回收_GCS原子态_宏名 ||
             到钩_变量->打开上值_圆小 == NULL || 函_是否具有内部的上值_宏名(到钩_变量));
  for (; o < 到钩_变量->顶部_变量.p; o++)  /* 记号_变量 live elements in the 栈_圆小 */
    垃圾回收_记号值_宏名(g, 对象_s到v_宏名(o));
  for (上值_缩变量 = 到钩_变量->打开上值_圆小; 上值_缩变量 != NULL; 上值_缩变量 = 上值_缩变量->u.打开_圆.下一个_变量)
    垃圾回收_记号对象_宏名(g, 上值_缩变量);  /* 打开_圆 上值们_小写 月编译器_不能_函 be collected */
  if (g->垃回状态机_缩小写 == 垃圾回收_GCS原子态_宏名) {  /* 最终_变量 traversal? */
    for (; o < 到钩_变量->栈_最后圆.p + 状态机_额外_栈_宏名; o++)
      对象_设置空值的值_宏名(对象_s到v_宏名(o));  /* clear dead 栈_圆小 slice */
    /* '月垃圾回收_重新记号上值们_函' may have removed 线程_变量 from '线程与上值_短缩' 列表_变量 */
    if (!函_是否具有内部的上值_宏名(到钩_变量) && 到钩_变量->打开上值_圆小 != NULL) {
      到钩_变量->线程与上值_短缩 = g->线程与上值_短缩;  /* link it back 到_变量 the 列表_变量 */
      g->线程与上值_短缩 = 到钩_变量;
    }
  }
  else if (!g->垃回紧急_缩圆)
    月亮调度_收缩栈_函(到钩_变量); /* do not 改变_变量 栈_圆小 in emergency cycle */
  return 1 + 状态机_栈大小_宏名(到钩_变量);
}


/*
** traverse one 灰的色_圆 object, turning it 到_变量 black.
*/
static lu_mem 月垃圾回收_扩散记号_函 (全局_状态机结 *g) {
  垃回对象_结 *o = g->灰的色_圆;
  垃圾回收_新白到黑_宏名(o);
  g->灰的色_圆 = *月垃圾回收_获取GC列表_函(o);  /* remove from '灰的色_圆' 列表_变量 */
  switch (o->类标_缩变量) {
    case 对象_月亮_V表_宏名: return 月垃圾回收_遍历表_函(g, 状态机_垃圾回收对象到t_宏名(o));
    case 对象_月亮_V用户数据_宏名: return 月垃圾回收_遍历用户数据_函(g, 状态机_垃圾回收对象到u_宏名(o));
    case 对象_月亮_VL闭包L_宏名: return 月垃圾回收_遍历L闭包_函(g, 状态机_垃圾回收对象到l闭包_宏名(o));
    case 对象_月亮_VC闭包L_宏名: return 月垃圾回收_遍历C闭包_函(g, 状态机_垃圾回收对象到c闭包_宏名(o));
    case 对象_月亮_V原型_宏名: return 月垃圾回收_遍历原型_函(g, 状态机_垃圾回收对象到P_宏名(o));
    case 对象_月亮_V线程_宏名: return 月垃圾回收_遍历线程_函(g, 状态机_垃圾回收对象到th_宏名(o));
    default: 限制_月亮_断言_宏名(0); return 0;
  }
}


static lu_mem 月垃圾回收_扩散全部_函 (全局_状态机结 *g) {
  lu_mem 总类_缩变量 = 0;
  while (g->灰的色_圆)
    总类_缩变量 += 月垃圾回收_扩散记号_函(g);
  return 总类_缩变量;
}


/*
** Traverse all 短命的_圆 tables propagating marks from keys 到_变量 values.
** Repeat until it converges, that is, nothing new is 已记号_变量. '目录_短变量'
** inverts the direction of the traversals, trying 到_变量 speed 上_小变量
** convergence on chains in the same table.
**
*/
static void 月垃圾回收_收敛短暂_函 (全局_状态机结 *g) {
  int 已更改_变量;
  int 目录_短变量 = 0;
  do {
    垃回对象_结 *w;
    垃回对象_结 *下一个_变量 = g->短命的_圆;  /* get 短命的_圆 列表_变量 */
    g->短命的_圆 = NULL;  /* tables may return 到_变量 this 列表_变量 when traversed */
    已更改_变量 = 0;
    while ((w = 下一个_变量) != NULL) {  /* for each 短命的_圆 table */
      表_结 *h = 状态机_垃圾回收对象到t_宏名(w);
      下一个_变量 = h->垃回列表_缩;  /* 列表_变量 is rebuilt during 环_变量 */
      垃圾回收_新白到黑_宏名(h);  /* out of the 列表_变量 (for now) */
      if (月垃圾回收_遍历短暂_函(g, h, 目录_短变量)) {  /* 已记号_变量 some 值_圆? */
        月垃圾回收_扩散全部_函(g);  /* propagate changes */
        已更改_变量 = 1;  /* will have 到_变量 revisit all 短命的_圆 tables */
      }
    }
    目录_短变量 = !目录_短变量;  /* invert direction 下一个_变量 time */
  } while (已更改_变量);  /* repeat until no 更多_变量 changes */
}

/* }====================================================== */


/*
** {======================================================
** Sweep Functions
** =======================================================
*/


/*
** clear entries with unmarked keys from all weaktables in 列表_变量 'l'
*/
static void 月垃圾回收_通过键清理_函 (全局_状态机结 *g, 垃回对象_结 *l) {
  for (; l; l = 状态机_垃圾回收对象到t_宏名(l)->垃回列表_缩) {
    表_结 *h = 状态机_垃圾回收对象到t_宏名(l);
    节点_联 *限制_变量 = 垃圾回收_g节点最后_宏名(h);
    节点_联 *n;
    for (n = 表_全局节点_宏名(h, 0); n < 限制_变量; n++) {
      if (月垃圾回收_是否已清理_函(g, 对象_垃圾回收键N_宏名(n)))  /* unmarked 键_小变量? */
        对象_设置空容器_宏名(表_全局值_宏名(n));  /* remove entry */
      if (对象_是否空容器_宏名(表_全局值_宏名(n)))  /* is entry 空容器_变量? */
        月垃圾回收_清理键_函(n);  /* clear its 键_小变量 */
    }
  }
}


/*
** clear entries with unmarked values from all weaktables in 列表_变量 'l' 上_小变量
** 到_变量 element 'f'
*/
static void 月垃圾回收_通过值清理_函 (全局_状态机结 *g, 垃回对象_结 *l, 垃回对象_结 *f) {
  for (; l != f; l = 状态机_垃圾回收对象到t_宏名(l)->垃回列表_缩) {
    表_结 *h = 状态机_垃圾回收对象到t_宏名(l);
    节点_联 *n, *限制_变量 = 垃圾回收_g节点最后_宏名(h);
    unsigned int i;
    unsigned int 数组大小_缩变量 = 月亮哈希表_真实a大小_函(h);
    for (i = 0; i < 数组大小_缩变量; i++) {
      标签值_结 *o = &h->数组_圆[i];
      if (月垃圾回收_是否已清理_函(g, 垃圾回收_gc值N_宏名(o)))  /* 值_圆 was collected? */
        对象_设置空容器_宏名(o);  /* remove entry */
    }
    for (n = 表_全局节点_宏名(h, 0); n < 限制_变量; n++) {
      if (月垃圾回收_是否已清理_函(g, 垃圾回收_gc值N_宏名(表_全局值_宏名(n))))  /* unmarked 值_圆? */
        对象_设置空容器_宏名(表_全局值_宏名(n));  /* remove entry */
      if (对象_是否空容器_宏名(表_全局值_宏名(n)))  /* is entry 空容器_变量? */
        月垃圾回收_清理键_函(n);  /* clear its 键_小变量 */
    }
  }
}


static void 月垃圾回收_释放上值_函 (炉_状态机结 *L, 上值_结 *上值_缩变量) {
  if (函_上是否打开_宏名(上值_缩变量))
    月亮函数_解除链上值_函(上值_缩变量);
  内存_月亮M_释放_宏名(L, 上值_缩变量);
}


static void 月垃圾回收_释放对象_函 (炉_状态机结 *L, 垃回对象_结 *o) {
  switch (o->类标_缩变量) {
    case 对象_月亮_V原型_宏名:
      月亮函数_释放原型_函(L, 状态机_垃圾回收对象到P_宏名(o));
      break;
    case 对象_月亮_V上值_宏名:
      月垃圾回收_释放上值_函(L, 状态机_垃圾回收对象到上值_宏名(o));
      break;
    case 对象_月亮_VL闭包L_宏名: {
      L闭包_结 *闭包_短变量 = 状态机_垃圾回收对象到l闭包_宏名(o);
      内存_月亮M_释放内存_宏名(L, 闭包_短变量, 函_L闭包大小_宏名(闭包_短变量->nupvalues));
      break;
    }
    case 对象_月亮_VC闭包L_宏名: {
      C闭包_结 *闭包_短变量 = 状态机_垃圾回收对象到c闭包_宏名(o);
      内存_月亮M_释放内存_宏名(L, 闭包_短变量, 函_C闭包大小_宏名(闭包_短变量->nupvalues));
      break;
    }
    case 对象_月亮_V表_宏名:
      月亮哈希表_释放_函(L, 状态机_垃圾回收对象到t_宏名(o));
      break;
    case 对象_月亮_V线程_宏名:
      月亮错误_释放线程_函(L, 状态机_垃圾回收对象到th_宏名(o));
      break;
    case 对象_月亮_V用户数据_宏名: {
      用户数据_结 *u = 状态机_垃圾回收对象到u_宏名(o);
      内存_月亮M_释放内存_宏名(L, o, 对象_用户数据大小_宏名(u->用户值数_缩, u->长度_短变量));
      break;
    }
    case 对象_月亮_V短型串_宏名: {
      标签字符串_结 *类s_变量 = 状态机_垃圾回收对象到t串_宏名(o);
      月亮字符串_移除_函(L, 类s_变量);  /* remove it from 哈希_小写 table */
      内存_月亮M_释放内存_宏名(L, 类s_变量, 字符串_大写l字符串_宏名(类s_变量->短串长_小写));
      break;
    }
    case 对象_月亮_V长型串_宏名: {
      标签字符串_结 *类s_变量 = 状态机_垃圾回收对象到t串_宏名(o);
      内存_月亮M_释放内存_宏名(L, 类s_变量, 字符串_大写l字符串_宏名(类s_变量->u.长串长_短));
      break;
    }
    default: 限制_月亮_断言_宏名(0);
  }
}


/*
** sweep at most 'countin' elements from a 列表_变量 of GCObjects erasing dead
** objects, 哪儿_变量 a dead object is one 已记号_变量 with the 旧_变量 (non 当前_圆)
** 白色_变量; 改变_变量 all non-dead objects back 到_变量 白色_变量, preparing for 下一个_变量
** collection cycle. Return 哪儿_变量 到_变量 continue the traversal or NULL if
** 列表_变量 is finished. ('*countout' gets the number of elements traversed.)
*/
static 垃回对象_结 **月垃圾回收_扫除列表_函 (炉_状态机结 *L, 垃回对象_结 **p, int countin,
                             int *countout) {
  全局_状态机结 *g = G(L);
  int 拥者_缩变量 = 垃圾回收_其他白色_宏名(g);
  int i;
  int 白色_变量 = 垃圾回收_月亮C_白色_宏名(g);  /* 当前_圆 白色_变量 */
  for (i = 0; *p != NULL && i < countin; i++) {
    垃回对象_结 *当前_短变量 = *p;
    int 已记号_变量 = 当前_短变量->已记号_变量;
    if (垃圾回收_是否死亡m_宏名(拥者_缩变量, 已记号_变量)) {  /* is '当前_短变量' dead? */
      *p = 当前_短变量->下一个_变量;  /* remove '当前_短变量' from 列表_变量 */
      月垃圾回收_释放对象_函(L, 当前_短变量);  /* erase '当前_短变量' */
    }
    else {  /* 改变_变量 记号_变量 到_变量 '白色_变量' */
      当前_短变量->已记号_变量 = 限制_类型转换_字节_宏名((已记号_变量 & ~垃圾回收_GC位们掩码_宏名) | 白色_变量);
      p = &当前_短变量->下一个_变量;  /* go 到_变量 下一个_变量 element */
    }
  }
  if (countout)
    *countout = i;  /* number of elements traversed */
  return (*p == NULL) ? NULL : p;
}


/*
** sweep a 列表_变量 until a live object (or 终_变量 of 列表_变量)
*/
static 垃回对象_结 **月垃圾回收_扫除存活_函 (炉_状态机结 *L, 垃回对象_结 **p) {
  垃回对象_结 **旧_变量 = p;
  do {
    p = 月垃圾回收_扫除列表_函(L, p, 1, NULL);
  } while (p == 旧_变量);
  return p;
}

/* }====================================================== */


/*
** {======================================================
** Finalization
** =======================================================
*/

/*
** If possible, shrink string table.
*/
static void 月垃圾回收_检查大小们_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  if (!g->垃回紧急_缩圆) {
    if (g->串的表_缩.使用数_缩 < g->串的表_缩.大小_变量 / 4) {  /* string table too big? */
      l_内存缩 旧债务_变量 = g->垃回债_缩;
      月亮字符串_调整大小_函(L, g->串的表_缩.大小_变量 / 2);
      g->垃回估计_缩 += g->垃回债_缩 - 旧债务_变量;  /* correct 估计_变量 */
    }
  }
}


/*
** Get the 下一个_变量 udata 到_变量 be finalized from the '待fnz_缩半' 列表_变量, and
** link it back into the '全部垃回_圆缩' 列表_变量.
*/
static 垃回对象_结 *月垃圾回收_用户数据到完善_函 (全局_状态机结 *g) {
  垃回对象_结 *o = g->待fnz_缩半;  /* get 首先_变量 element */
  限制_月亮_断言_宏名(垃圾回收_到终结_宏名(o));
  g->待fnz_缩半 = o->下一个_变量;  /* remove it from '待fnz_缩半' 列表_变量 */
  o->下一个_变量 = g->全部垃回_圆缩;  /* return it 到_变量 '全部垃回_圆缩' 列表_变量 */
  g->全部垃回_圆缩 = o;
  垃圾回收_重置位_宏名(o->已记号_变量, 垃圾回收_终结位_宏名);  /* object is "normal" again */
  if (垃圾回收_是否扫除阶段_宏名(g))
    垃圾回收_制作白色_宏名(g, o);  /* "sweep" object */
  else if (垃圾回收_获取年龄_宏名(o) == 垃圾回收_G_旧1_宏名)
    g->首先旧1_圆 = o;  /* it is the 首先_变量 OLD1 object in the 列表_变量 */
  return o;
}


static void 月垃圾回收_做此调用_函 (炉_状态机结 *L, void *用数_缩变量) {
  限制_未使用_宏名(用数_缩变量);
  月亮调度_调用无产出_函(L, L->顶部_变量.p - 2, 0);
}


static void 月垃圾回收_垃圾回收标签方法_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  const 标签值_结 *标方_缩变量;
  标签值_结 v;
  限制_月亮_断言_宏名(!g->垃回紧急_缩圆);
  对象_设置垃圾回收o值_宏名(L, &v, 月垃圾回收_用户数据到完善_函(g));
  标方_缩变量 = 月亮类型_通过对象获取标签方法_函(L, &v, 标方_垃回大写);
  if (!标签方法_无标方_宏名(标方_缩变量)) {  /* is there a finalizer? */
    int 状态码_变量;
    路_字节型 旧组高_变量 = L->允许钩子_圆;
    int 旧垃回栈顶针_变量  = g->垃回步进_短缩;
    g->垃回步进_短缩 |= 垃圾回收_GC停止GC_宏名;  /* avoid GC steps */
    L->允许钩子_圆 = 0;  /* stop debug hooks during GC metamethod */
    对象_设置对象到s_宏名(L, L->顶部_变量.p++, 标方_缩变量);  /* push finalizer... */
    对象_设置对象到s_宏名(L, L->顶部_变量.p++, &v);  /* ... and its argument */
    L->调信_缩变量->调用状态码_圆 |= 状态机_调信状型_终结的_宏名;  /* will run a finalizer */
    状态码_变量 = 月亮调度_预处理调用_函(L, 月垃圾回收_做此调用_函, NULL, 做_保存栈_宏名(L, L->顶部_变量.p - 2), 0);
    L->调信_缩变量->调用状态码_圆 &= ~状态机_调信状型_终结的_宏名;  /* not running a finalizer anymore */
    L->允许钩子_圆 = 旧组高_变量;  /* restore hooks */
    g->垃回步进_短缩 = 旧垃回栈顶针_变量;  /* restore 状态机_变量 */
    if (配置_l_可能性低_宏名(状态码_变量 != LUA_OK)) {  /* 错误_小变量 while running __gc? */
      月亮错误_警告错误_函(L, "__gc");
      L->顶部_变量.p--;  /* pops 错误_小变量 object */
    }
  }
}


/*
** Call a few finalizers
*/
static int 月垃圾回收_跑几个终结器_函 (炉_状态机结 *L, int n) {
  全局_状态机结 *g = G(L);
  int i;
  for (i = 0; i < n && g->待fnz_缩半; i++)
    月垃圾回收_垃圾回收标签方法_函(L);  /* call one finalizer */
  return i;
}


/*
** call all pending finalizers
*/
static void 月垃圾回收_调用全部没搞定的终结器_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  while (g->待fnz_缩半)
    月垃圾回收_垃圾回收标签方法_函(L);
}


/*
** find 最后_变量 '下一个_变量' 月解析器_字段_函 in 列表_变量 'p' 列表_变量 (到_变量 add elements in its 终_变量)
*/
static 垃回对象_结 **月垃圾回收_找最后_函 (垃回对象_结 **p) {
  while (*p != NULL)
    p = &(*p)->下一个_变量;
  return p;
}


/*
** Move all unreachable objects (or 'all' objects) that need
** finalization from 列表_变量 '最终对象_缩' 到_变量 列表_变量 '待fnz_缩半' (到_变量 be finalized).
** (Note that objects after '最终对象旧1_缩' 月编译器_不能_函 be 白色_变量, so they
** don't need 到_变量 be traversed. In incremental 模块_变量, '最终对象旧1_缩' is NULL,
** so the whole 列表_变量 is traversed.)
*/
static void 月垃圾回收_分离存在的fnz_函 (全局_状态机结 *g, int all) {
  垃回对象_结 *当前_短变量;
  垃回对象_结 **p = &g->最终对象_缩;
  垃回对象_结 **最后下个_变量 = 月垃圾回收_找最后_函(&g->待fnz_缩半);
  while ((当前_短变量 = *p) != g->最终对象旧1_缩) {  /* traverse all finalizable objects */
    限制_月亮_断言_宏名(垃圾回收_到终结_宏名(当前_短变量));
    if (!(垃圾回收_是否白色_宏名(当前_短变量) || all))  /* not being collected? */
      p = &当前_短变量->下一个_变量;  /* don't bother with it */
    else {
      if (当前_短变量 == g->最终对象生存_短小写)  /* removing '最终对象生存_短小写'? */
        g->最终对象生存_短小写 = 当前_短变量->下一个_变量;  /* correct it */
      *p = 当前_短变量->下一个_变量;  /* remove '当前_短变量' from '最终对象_缩' 列表_变量 */
      当前_短变量->下一个_变量 = *最后下个_变量;  /* link at the 终_变量 of '待fnz_缩半' 列表_变量 */
      *最后下个_变量 = 当前_短变量;
      最后下个_变量 = &当前_短变量->下一个_变量;
    }
  }
}


/*
** If pointer 'p' points 到_变量 'o', move it 到_变量 the 下一个_变量 element.
*/
static void 月垃圾回收_检查指针_函 (垃回对象_结 **p, 垃回对象_结 *o) {
  if (o == *p)
    *p = o->下一个_变量;
}


/*
** Correct pointers 到_变量 objects inside '全部垃回_圆缩' 列表_变量 when
** object 'o' is being removed from the 列表_变量.
*/
static void 月垃圾回收_纠正指针们_函 (全局_状态机结 *g, 垃回对象_结 *o) {
  月垃圾回收_检查指针_函(&g->生存_圆, o);
  月垃圾回收_检查指针_函(&g->旧的1_小写, o);
  月垃圾回收_检查指针_函(&g->真的旧_圆, o);
  月垃圾回收_检查指针_函(&g->首先旧1_圆, o);
}


/*
** if object 'o' has a finalizer, remove it from '全部垃回_圆缩' 列表_变量 (must
** search the 列表_变量 到_变量 find it) and link it in '最终对象_缩' 列表_变量.
*/
void 月亮编译_检查终结器_函 (炉_状态机结 *L, 垃回对象_结 *o, 表_结 *元表_缩变量) {
  全局_状态机结 *g = G(L);
  if (垃圾回收_到终结_宏名(o) ||                 /* 对象_变量. is already 已记号_变量... */
      标签方法_全局快速标方_宏名(g, 元表_缩变量, 标方_垃回大写) == NULL ||    /* or has no finalizer... */
      (g->垃回步进_短缩 & 垃圾回收_GC停止CLS_宏名))                   /* or closing 状态机_变量? */
    return;  /* nothing 到_变量 be done */
  else {  /* move 'o' 到_变量 '最终对象_缩' 列表_变量 */
    垃回对象_结 **p;
    if (垃圾回收_是否扫除阶段_宏名(g)) {
      垃圾回收_制作白色_宏名(g, o);  /* "sweep" object 'o' */
      if (g->扫除垃回_圆缩 == &o->下一个_变量)  /* should not remove '扫除垃回_圆缩' object */
        g->扫除垃回_圆缩 = 月垃圾回收_扫除存活_函(L, g->扫除垃回_圆缩);  /* 改变_变量 '扫除垃回_圆缩' */
    }
    else
      月垃圾回收_纠正指针们_函(g, o);
    /* search for pointer pointing 到_变量 'o' */
    for (p = &g->全部垃回_圆缩; *p != o; p = &(*p)->下一个_变量) { /* 空容器_变量 */ }
    *p = o->下一个_变量;  /* remove 'o' from '全部垃回_圆缩' 列表_变量 */
    o->下一个_变量 = g->最终对象_缩;  /* link it in '最终对象_缩' 列表_变量 */
    g->最终对象_缩 = o;
    垃圾回收_l_设置位_宏名(o->已记号_变量, 垃圾回收_终结位_宏名);  /* 记号_变量 it as such */
  }
}

/* }====================================================== */


/*
** {======================================================
** Generational Collector
** =======================================================
*/


/*
** Set the "time" 到_变量 wait before starting a new GC cycle; cycle will
** 起始_变量 when memory use hits the 阈值_变量 of ('估计_变量' * 暂停_变量 /
** 垃圾回收_暂停调整_宏名). (Division by '估计_变量' should be OK: it 月编译器_不能_函 be zero,
** because Lua 月编译器_不能_函 even 起始_变量 with less than 垃圾回收_暂停调整_宏名 bytes).
*/
static void 月垃圾回收_设置暂停_函 (全局_状态机结 *g) {
  l_内存缩 阈值_变量, 负债_变量;
  int 暂停_变量 = 垃圾回收_获取gc形参_宏名(g->垃回暂停_缩圆);
  l_内存缩 估计_变量 = g->垃回估计_缩 / 垃圾回收_暂停调整_宏名;  /* adjust '估计_变量' */
  限制_月亮_断言_宏名(估计_变量 > 0);
  阈值_变量 = (暂停_变量 < 限制_最大_L内存_宏名 / 估计_变量)  /* overflow? */
            ? 估计_变量 * 暂停_变量  /* no overflow */
            : 限制_最大_L内存_宏名;  /* overflow; truncate 到_变量 maximum */
  负债_变量 = 状态机_获取全部字节_宏名(g) - 阈值_变量;
  if (负债_变量 > 0) 负债_变量 = 0;
  月亮错误_设置债务_函(g, 负债_变量);
}


/*
** Sweep a 列表_变量 of objects 到_变量 enter generational 模块_变量.  Deletes dead
** objects and turns the non dead 到_变量 旧_变量. All non-dead threads---which
** are now 旧_变量---must be in a 灰的色_圆 列表_变量. Everything else is not in a
** 灰的色_圆 列表_变量. Open 上值们_小写 are also kept 灰的色_圆.
*/
static void 月垃圾回收_扫除到旧_函 (炉_状态机结 *L, 垃回对象_结 **p) {
  垃回对象_结 *当前_短变量;
  全局_状态机结 *g = G(L);
  while ((当前_短变量 = *p) != NULL) {
    if (垃圾回收_是否白色_宏名(当前_短变量)) {  /* is '当前_短变量' dead? */
      限制_月亮_断言_宏名(垃圾回收_是否死亡_宏名(g, 当前_短变量));
      *p = 当前_短变量->下一个_变量;  /* remove '当前_短变量' from 列表_变量 */
      月垃圾回收_释放对象_函(L, 当前_短变量);  /* erase '当前_短变量' */
    }
    else {  /* all surviving objects become 旧_变量 */
      垃圾回收_设置年龄_宏名(当前_短变量, 垃圾回收_G_旧_宏名);
      if (当前_短变量->类标_缩变量 == 对象_月亮_V线程_宏名) {  /* threads must be watched */
        炉_状态机结 *到钩_变量 = 状态机_垃圾回收对象到th_宏名(当前_短变量);
        垃圾回收_链接GC列表_宏名(到钩_变量, g->再次灰色_变量);  /* insert into '再次灰色_变量' 列表_变量 */
      }
      else if (当前_短变量->类标_缩变量 == 对象_月亮_V上值_宏名 && 函_上是否打开_宏名(状态机_垃圾回收对象到上值_宏名(当前_短变量)))
        垃圾回收_设置为灰色_宏名(当前_短变量);  /* 打开_圆 上值们_小写 are always 灰的色_圆 */
      else  /* everything else is black */
        垃圾回收_新白到黑_宏名(当前_短变量);
      p = &当前_短变量->下一个_变量;  /* go 到_变量 下一个_变量 element */
    }
  }
}


/*
** Sweep for generational 模块_变量. Delete dead objects. (Because the
** collection is not incremental, there are no "new 白色_变量" objects
** during the sweep. So, any 白色_变量 object must be dead.) For
** non-dead objects, advance their ages and clear the color of
** new objects. (Old objects keep their colors.)
** The ages of 垃圾回收_G_已摸1_宏名 and 垃圾回收_G_已摸2_宏名 objects 月编译器_不能_函 be advanced
** here, because these 旧_变量-generation objects are usually not swept
** here.  They will all be advanced in '月垃圾回收_纠正灰列表_函'. That function
** will also remove objects turned 白色_变量 here from any 灰的色_圆 列表_变量.
*/
static 垃回对象_结 **月垃圾回收_扫除生成_函 (炉_状态机结 *L, 全局_状态机结 *g, 垃回对象_结 **p,
                            垃回对象_结 *限制_变量, 垃回对象_结 **pfirstold1) {
  static const 路_字节型 下个年龄_变量[] = {
    垃圾回收_G_存活_宏名,  /* from 垃圾回收_G_新_宏名 */
    垃圾回收_G_旧1_宏名,      /* from 垃圾回收_G_存活_宏名 */
    垃圾回收_G_旧1_宏名,      /* from 垃圾回收_G_旧0_宏名 */
    垃圾回收_G_旧_宏名,       /* from 垃圾回收_G_旧1_宏名 */
    垃圾回收_G_旧_宏名,       /* from 垃圾回收_G_旧_宏名 (do not 改变_变量) */
    垃圾回收_G_已摸1_宏名,  /* from 垃圾回收_G_已摸1_宏名 (do not 改变_变量) */
    垃圾回收_G_已摸2_宏名   /* from 垃圾回收_G_已摸2_宏名 (do not 改变_变量) */
  };
  int 白色_变量 = 垃圾回收_月亮C_白色_宏名(g);
  垃回对象_结 *当前_短变量;
  while ((当前_短变量 = *p) != 限制_变量) {
    if (垃圾回收_是否白色_宏名(当前_短变量)) {  /* is '当前_短变量' dead? */
      限制_月亮_断言_宏名(!垃圾回收_是否旧_宏名(当前_短变量) && 垃圾回收_是否死亡_宏名(g, 当前_短变量));
      *p = 当前_短变量->下一个_变量;  /* remove '当前_短变量' from 列表_变量 */
      月垃圾回收_释放对象_函(L, 当前_短变量);  /* erase '当前_短变量' */
    }
    else {  /* correct 记号_变量 and age */
      if (垃圾回收_获取年龄_宏名(当前_短变量) == 垃圾回收_G_新_宏名) {  /* new objects go back 到_变量 白色_变量 */
        int 已记号_变量 = 当前_短变量->已记号_变量 & ~垃圾回收_GC位们掩码_宏名;  /* erase GC bits */
        当前_短变量->已记号_变量 = 限制_类型转换_字节_宏名(已记号_变量 | 垃圾回收_G_存活_宏名 | 白色_变量);
      }
      else {  /* all other objects will be 旧_变量, and so keep their color */
        垃圾回收_设置年龄_宏名(当前_短变量, 下个年龄_变量[垃圾回收_获取年龄_宏名(当前_短变量)]);
        if (垃圾回收_获取年龄_宏名(当前_短变量) == 垃圾回收_G_旧1_宏名 && *pfirstold1 == NULL)
          *pfirstold1 = 当前_短变量;  /* 首先_变量 OLD1 object in the 列表_变量 */
      }
      p = &当前_短变量->下一个_变量;  /* go 到_变量 下一个_变量 element */
    }
  }
  return p;
}


/*
** Traverse a 列表_变量 making all its elements 白色_变量 and clearing their
** age. In incremental 模块_变量, all objects are 'new' all the time,
** except for fixed strings (which are always 旧_变量).
*/
static void 月垃圾回收_白列表_函 (全局_状态机结 *g, 垃回对象_结 *p) {
  int 白色_变量 = 垃圾回收_月亮C_白色_宏名(g);
  for (; p != NULL; p = p->下一个_变量)
    p->已记号_变量 = 限制_类型转换_字节_宏名((p->已记号_变量 & ~垃圾回收_GC位们掩码_宏名) | 白色_变量);
}


/*
** Correct a 列表_变量 of 灰的色_圆 objects. Return pointer 到_变量 哪儿_变量 rest of the
** 列表_变量 should be linked.
** Because this correction is done after sweeping, young objects might
** be turned 白色_变量 and still be in the 列表_变量. They are only removed.
** 'TOUCHED1' objects are advanced 到_变量 'TOUCHED2' and remain on the 列表_变量;
** Non-白色_变量 threads also remain on the 列表_变量; 'TOUCHED2' objects become
** regular 旧_变量; they and anything else are removed from the 列表_变量.
*/
static 垃回对象_结 **月垃圾回收_纠正灰列表_函 (垃回对象_结 **p) {
  垃回对象_结 *当前_短变量;
  while ((当前_短变量 = *p) != NULL) {
    垃回对象_结 **下一个_变量 = 月垃圾回收_获取GC列表_函(当前_短变量);
    if (垃圾回收_是否白色_宏名(当前_短变量))
      goto remove;  /* remove all 白色_变量 objects */
    else if (垃圾回收_获取年龄_宏名(当前_短变量) == 垃圾回收_G_已摸1_宏名) {  /* touched in this cycle? */
      限制_月亮_断言_宏名(垃圾回收_是否灰色_宏名(当前_短变量));
      垃圾回收_新白到黑_宏名(当前_短变量);  /* make it black, for 下一个_变量 barrier */
      垃圾回收_改变年龄_宏名(当前_短变量, 垃圾回收_G_已摸1_宏名, 垃圾回收_G_已摸2_宏名);
      goto remain;  /* keep it in the 列表_变量 and go 到_变量 下一个_变量 element */
    }
    else if (当前_短变量->类标_缩变量 == 对象_月亮_V线程_宏名) {
      限制_月亮_断言_宏名(垃圾回收_是否灰色_宏名(当前_短变量));
      goto remain;  /* keep non-白色_变量 threads on the 列表_变量 */
    }
    else {  /* everything else is removed */
      限制_月亮_断言_宏名(垃圾回收_是否旧_宏名(当前_短变量));  /* young objects should be 白色_变量 here */
      if (垃圾回收_获取年龄_宏名(当前_短变量) == 垃圾回收_G_已摸2_宏名)  /* advance from TOUCHED2... */
        垃圾回收_改变年龄_宏名(当前_短变量, 垃圾回收_G_已摸2_宏名, 垃圾回收_G_旧_宏名);  /* ... 到_变量 OLD */
      垃圾回收_新白到黑_宏名(当前_短变量);  /* make object black (到_变量 be removed) */
      goto remove;
    }
    remove: *p = *下一个_变量; continue;
    remain: p = 下一个_变量; continue;
  }
  return p;
}


/*
** Correct all 灰的色_圆 lists, coalescing them into '再次灰色_变量'.
*/
static void 月垃圾回收_纠正灰列表们_函 (全局_状态机结 *g) {
  垃回对象_结 **列表_变量 = 月垃圾回收_纠正灰列表_函(&g->再次灰色_变量);
  *列表_变量 = g->弱的_圆; g->弱的_圆 = NULL;
  列表_变量 = 月垃圾回收_纠正灰列表_函(列表_变量);
  *列表_变量 = g->全部弱_圆; g->全部弱_圆 = NULL;
  列表_变量 = 月垃圾回收_纠正灰列表_函(列表_变量);
  *列表_变量 = g->短命的_圆; g->短命的_圆 = NULL;
  月垃圾回收_纠正灰列表_函(列表_变量);
}


/*
** Mark black 'OLD1' objects when starting a new young collection.
** Gray objects are already in some 灰的色_圆 列表_变量, and so will be visited
** in the 月垃圾回收_原子_函 步进_变量.
*/
static void 月垃圾回收_记号旧_函 (全局_状态机结 *g, 垃回对象_结 *from, 垃回对象_结 *到_变量) {
  垃回对象_结 *p;
  for (p = from; p != 到_变量; p = p->下一个_变量) {
    if (垃圾回收_获取年龄_宏名(p) == 垃圾回收_G_旧1_宏名) {
      限制_月亮_断言_宏名(!垃圾回收_是否白色_宏名(p));
      垃圾回收_改变年龄_宏名(p, 垃圾回收_G_旧1_宏名, 垃圾回收_G_旧_宏名);  /* now they are 旧_变量 */
      if (垃圾回收_是否黑色_宏名(p))
        月垃圾回收_真实记号对象_函(g, p);
    }
  }
}


/*
** Finish a young-generation collection.
*/
static void 月垃圾回收_完成生成轮_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  月垃圾回收_纠正灰列表们_函(g);
  月垃圾回收_检查大小们_函(L, g);
  g->垃回状态机_缩小写 = 垃圾回收_GCS传播_宏名;  /* skip restart */
  if (!g->垃回紧急_缩圆)
    月垃圾回收_调用全部没搞定的终结器_函(L);
}


/*
** Does a young collection. First, 记号_变量 'OLD1' objects. Then does the
** 月垃圾回收_原子_函 步进_变量. Then, sweep all lists and advance pointers. Finally,
** finish the collection.
*/
static void 月垃圾回收_年轻回收_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  垃回对象_结 **保护存活_缩变量;  /* 到_变量 针点_变量 到_变量 首先_变量 non-dead 生存_圆 object */
  垃回对象_结 *虚假_变量;  /* 虚假_变量 out parameter 到_变量 '月垃圾回收_扫除生成_函' */
  限制_月亮_断言_宏名(g->垃回状态机_缩小写 == 垃圾回收_GCS传播_宏名);
  if (g->首先旧1_圆) {  /* are there regular OLD1 objects? */
    月垃圾回收_记号旧_函(g, g->首先旧1_圆, g->真的旧_圆);  /* 记号_变量 them */
    g->首先旧1_圆 = NULL;  /* no 更多_变量 OLD1 objects (for now) */
  }
  月垃圾回收_记号旧_函(g, g->最终对象_缩, g->最终对象真的旧_缩);
  月垃圾回收_记号旧_函(g, g->待fnz_缩半, NULL);
  月垃圾回收_原子_函(L);

  /* sweep nursery and get a pointer 到_变量 its 最后_变量 live element */
  g->垃回状态机_缩小写 = 垃圾回收_GCS扫除所有GC_宏名;
  保护存活_缩变量 = 月垃圾回收_扫除生成_函(L, g, &g->全部垃回_圆缩, g->生存_圆, &g->首先旧1_圆);
  /* sweep '生存_圆' */
  月垃圾回收_扫除生成_函(L, g, 保护存活_缩变量, g->旧的1_小写, &g->首先旧1_圆);
  g->真的旧_圆 = g->旧的1_小写;
  g->旧的1_小写 = *保护存活_缩变量;  /* '生存_圆' survivals are 旧_变量 now */
  g->生存_圆 = g->全部垃回_圆缩;  /* all 新s_变量 are survivals */

  /* repeat for '最终对象_缩' lists */
  虚假_变量 = NULL;  /* no '首先旧1_圆' optimization for '最终对象_缩' lists */
  保护存活_缩变量 = 月垃圾回收_扫除生成_函(L, g, &g->最终对象_缩, g->最终对象生存_短小写, &虚假_变量);
  /* sweep '生存_圆' */
  月垃圾回收_扫除生成_函(L, g, 保护存活_缩变量, g->最终对象旧1_缩, &虚假_变量);
  g->最终对象真的旧_缩 = g->最终对象旧1_缩;
  g->最终对象旧1_缩 = *保护存活_缩变量;  /* '生存_圆' survivals are 旧_变量 now */
  g->最终对象生存_短小写 = g->最终对象_缩;  /* all 新s_变量 are survivals */

  月垃圾回收_扫除生成_函(L, g, &g->待fnz_缩半, NULL, &虚假_变量);
  月垃圾回收_完成生成轮_函(L, g);
}


/*
** Clears all 灰的色_圆 lists, sweeps objects, and prepare sublists 到_变量 enter
** generational 模块_变量. The sweeps remove dead objects and turn all
** surviving objects 到_变量 旧_变量. Threads go back 到_变量 '再次灰色_变量'; everything
** else is turned black (not in any 灰的色_圆 列表_变量).
*/
static void 月垃圾回收_原子到生成_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  月垃圾回收_清理灰色列表们_函(g);
  /* sweep all elements making them 旧_变量 */
  g->垃回状态机_缩小写 = 垃圾回收_GCS扫除所有GC_宏名;
  月垃圾回收_扫除到旧_函(L, &g->全部垃回_圆缩);
  /* everything alive now is 旧_变量 */
  g->真的旧_圆 = g->旧的1_小写 = g->生存_圆 = g->全部垃回_圆缩;
  g->首先旧1_圆 = NULL;  /* there are no OLD1 objects anywhere */

  /* repeat for '最终对象_缩' lists */
  月垃圾回收_扫除到旧_函(L, &g->最终对象_缩);
  g->最终对象真的旧_缩 = g->最终对象旧1_缩 = g->最终对象生存_短小写 = g->最终对象_缩;

  月垃圾回收_扫除到旧_函(L, &g->待fnz_缩半);

  g->垃回种类_缩小写 = 状态机_K垃圾回收_生成_宏名;
  g->最后原子_变量 = 0;
  g->垃回估计_缩 = 状态机_获取全部字节_宏名(g);  /* 基本_变量 for memory control */
  月垃圾回收_完成生成轮_函(L, g);
}


/*
** Set 负债_变量 for the 下一个_变量 minor collection, which will happen when
** memory grows '生成副乘法_缩'%.
*/
static void 月垃圾回收_设置次要债务_函 (全局_状态机结 *g) {
  月亮错误_设置债务_函(g, -(限制_类型转换_宏名(l_内存缩, (状态机_获取全部字节_宏名(g) / 100)) * g->生成副乘法_缩));
}


/*
** Enter generational 模块_变量. Must go until the 终_变量 of an 月垃圾回收_原子_函 cycle
** 到_变量 ensure that all objects are correctly 已记号_变量 and 弱的_圆 tables
** are cleared. Then, turn all objects into 旧_变量 and finishes the
** collection.
*/
static lu_mem 月垃圾回收_进入生成_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  lu_mem 对象数目_变量;
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS暂停_宏名));  /* prepare 到_变量 起始_变量 a new cycle */
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS传播_宏名));  /* 起始_变量 new cycle */
  对象数目_变量 = 月垃圾回收_原子_函(L);  /* propagates all and then do the 月垃圾回收_原子_函 stuff */
  月垃圾回收_原子到生成_函(L, g);
  月垃圾回收_设置次要债务_函(g);  /* set 负债_变量 assuming 下一个_变量 cycle will be minor */
  return 对象数目_变量;
}


/*
** Enter incremental 模块_变量. Turn all objects 白色_变量, make all
** intermediate lists 针点_变量 到_变量 NULL (到_变量 avoid invalid pointers),
** and go 到_变量 the 暂停_变量 状态机_变量.
*/
static void 月垃圾回收_进入递增_函 (全局_状态机结 *g) {
  月垃圾回收_白列表_函(g, g->全部垃回_圆缩);
  g->真的旧_圆 = g->旧的1_小写 = g->生存_圆 = NULL;
  月垃圾回收_白列表_函(g, g->最终对象_缩);
  月垃圾回收_白列表_函(g, g->待fnz_缩半);
  g->最终对象真的旧_缩 = g->最终对象旧1_缩 = g->最终对象生存_短小写 = NULL;
  g->垃回状态机_缩小写 = 垃圾回收_GCS暂停_宏名;
  g->垃回种类_缩小写 = 状态机_K垃圾回收_递增_宏名;
  g->最后原子_变量 = 0;
}


/*
** Change collector 模块_变量 到_变量 'newmode'.
*/
void 月亮编译_改变模式_函 (炉_状态机结 *L, int newmode) {
  全局_状态机结 *g = G(L);
  if (newmode != g->垃回种类_缩小写) {
    if (newmode == 状态机_K垃圾回收_生成_宏名)  /* entering generational 模块_变量? */
      月垃圾回收_进入生成_函(L, g);
    else
      月垃圾回收_进入递增_函(g);  /* entering incremental 模块_变量 */
  }
  g->最后原子_变量 = 0;
}


/*
** Does a full collection in generational 模块_变量.
*/
static lu_mem 月垃圾回收_完全生成_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  月垃圾回收_进入递增_函(g);
  return 月垃圾回收_进入生成_函(L, g);
}


/*
** Does a major collection after 最后_变量 collection was a "bad collection".
**
** When the program is building a big structure, it allocates lots of
** memory but generates very little garbage. In those scenarios,
** the generational 模块_变量 just wastes time doing small collections, and
** major collections are frequently 什么_变量 we call a "bad collection", a
** collection that frees too few objects. To avoid the cost of switching
** between generational 模块_变量 and the incremental 模块_变量 已需要_变量 for full
** (major) collections, the collector tries 到_变量 stay in incremental 模块_变量
** after a bad collection, and 到_变量 switch back 到_变量 generational 模块_变量 only
** after a "good" collection (one that traverses less than 9/8 objects
** of the 前一个_变量 one).
** The collector must choose whether 到_变量 stay in incremental 模块_变量 or 到_变量
** switch back 到_变量 generational 模块_变量 before sweeping. At this 针点_变量, it
** does not know the real memory in use, so it 月编译器_不能_函 use memory 到_变量
** decide whether 到_变量 return 到_变量 generational 模块_变量. Instead, it uses the
** number of objects traversed (returned by '月垃圾回收_原子_函') as a proxy. The
** 月解析器_字段_函 'g->最后原子_变量' keeps this 计数_变量 from the 最后_变量 collection.
** ('g->最后原子_变量 != 0' also means that the 最后_变量 collection was bad.)
*/
static void 月垃圾回收_步进生成完全_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  lu_mem 新原子_变量;  /* 计数_变量 of traversed objects */
  lu_mem 最后原子_变量 = g->最后原子_变量;  /* 计数_变量 from 最后_变量 collection */
  if (g->垃回种类_缩小写 == 状态机_K垃圾回收_生成_宏名)  /* still in generational 模块_变量? */
    月垃圾回收_进入递增_函(g);  /* enter incremental 模块_变量 */
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS传播_宏名));  /* 起始_变量 new cycle */
  新原子_变量 = 月垃圾回收_原子_函(L);  /* 记号_变量 everybody */
  if (新原子_变量 < 最后原子_变量 + (最后原子_变量 >> 3)) {  /* good collection? */
    月垃圾回收_原子到生成_函(L, g);  /* return 到_变量 generational 模块_变量 */
    月垃圾回收_设置次要债务_函(g);
  }
  else {  /* another bad collection; stay in incremental 模块_变量 */
    g->垃回估计_缩 = 状态机_获取全部字节_宏名(g);  /* 首先_变量 估计_变量 */;
    月垃圾回收_进入扫除_函(L);
    月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS暂停_宏名));  /* finish collection */
    月垃圾回收_设置暂停_函(g);
    g->最后原子_变量 = 新原子_变量;
  }
}


/*
** Does a generational "步进_变量".
** Usually, this means doing a minor collection and setting the 负债_变量 到_变量
** make another collection when memory grows '生成副乘法_缩'% larger.
**
** However, there are exceptions.  If memory grows '生成主乘法_缩'%
** larger than it was at the 终_变量 of the 最后_变量 major collection (kept
** in 'g->垃回估计_缩'), the function does a major collection. At the
** 终_变量, it checks whether the major collection was able 到_变量 free a
** decent amount of memory (at least half the growth in memory since
** 前一个_变量 major collection). If so, the collector keeps its 状态机_变量,
** and the 下一个_变量 collection will probably be minor again. Otherwise,
** we have 什么_变量 we call a "bad collection". In that case, set the 月解析器_字段_函
** 'g->最后原子_变量' 到_变量 signal that fact, so that the 下一个_变量 collection will
** go 到_变量 '月垃圾回收_步进生成完全_函'.
**
** '垃回债_缩 <= 0' means an explicit call 到_变量 GC 步进_变量 with "大小_变量" zero;
** in that case, do a minor collection.
*/
static void 月垃圾回收_生成步进_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  if (g->最后原子_变量 != 0)  /* 最后_变量 collection was a bad one? */
    月垃圾回收_步进生成完全_函(L, g);  /* do a full 步进_变量 */
  else {
    lu_mem 主基本_变量 = g->垃回估计_缩;  /* memory after 最后_变量 major collection */
    lu_mem 主递增_变量 = (主基本_变量 / 100) * 垃圾回收_获取gc形参_宏名(g->生成主乘法_缩);
    if (g->垃回债_缩 > 0 && 状态机_获取全部字节_宏名(g) > 主基本_变量 + 主递增_变量) {
      lu_mem 对象数目_变量 = 月垃圾回收_完全生成_函(L, g);  /* do a major collection */
      if (状态机_获取全部字节_宏名(g) < 主基本_变量 + (主递增_变量 / 2)) {
        /* collected at least half of memory growth since 最后_变量 major
           collection; keep doing minor collections. */
        限制_月亮_断言_宏名(g->最后原子_变量 == 0);
      }
      else {  /* bad collection */
        g->最后原子_变量 = 对象数目_变量;  /* signal that 最后_变量 collection was bad */
        月垃圾回收_设置暂停_函(g);  /* do a long wait for 下一个_变量 (major) collection */
      }
    }
    else {  /* regular case; do a minor collection */
      月垃圾回收_年轻回收_函(L, g);
      月垃圾回收_设置次要债务_函(g);
      g->垃回估计_缩 = 主基本_变量;  /* preserve 基本_变量 值_圆 */
    }
  }
  限制_月亮_断言_宏名(垃圾回收_是否减少GC模式生成_宏名(g));
}

/* }====================================================== */


/*
** {======================================================
** GC control
** =======================================================
*/


/*
** Enter 首先_变量 sweep phase.
** The call 到_变量 '月垃圾回收_扫除存活_函' makes the pointer 针点_变量 到_变量 an object
** inside the 列表_变量 (instead of 到_变量 the header), so that the real sweep do
** not need 到_变量 skip objects created between "now" and the 起始_变量 of the
** real sweep.
*/
static void 月垃圾回收_进入扫除_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  g->垃回状态机_缩小写 = 垃圾回收_GCS扫除所有GC_宏名;
  限制_月亮_断言_宏名(g->扫除垃回_圆缩 == NULL);
  g->扫除垃回_圆缩 = 月垃圾回收_扫除存活_函(L, &g->全部垃回_圆缩);
}


/*
** Delete all objects in 列表_变量 'p' until (but not including) object
** '限制_变量'.
*/
static void 月垃圾回收_删除列表_函 (炉_状态机结 *L, 垃回对象_结 *p, 垃回对象_结 *限制_变量) {
  while (p != 限制_变量) {
    垃回对象_结 *下一个_变量 = p->下一个_变量;
    月垃圾回收_释放对象_函(L, p);
    p = 下一个_变量;
  }
}


/*
** Call all finalizers of the objects in the given Lua 状态机_变量, and
** then free all objects, except for the main 线程_变量.
*/
void 月亮编译_释放所有对象们_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  g->垃回步进_短缩 = 垃圾回收_GC停止CLS_宏名;  /* no 额外_变量 finalizers after here */
  月亮编译_改变模式_函(L, 状态机_K垃圾回收_递增_宏名);
  月垃圾回收_分离存在的fnz_函(g, 1);  /* separate all objects with finalizers */
  限制_月亮_断言_宏名(g->最终对象_缩 == NULL);
  月垃圾回收_调用全部没搞定的终结器_函(L);
  月垃圾回收_删除列表_函(L, g->全部垃回_圆缩, 状态机_对象到垃圾回收对象_宏名(g->主线程_圆));
  限制_月亮_断言_宏名(g->最终对象_缩 == NULL);  /* no new finalizers */
  月垃圾回收_删除列表_函(L, g->固定垃回_圆缩, NULL);  /* collect fixed objects */
  限制_月亮_断言_宏名(g->串的表_缩.使用数_缩 == 0);
}


static lu_mem 月垃圾回收_原子_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  lu_mem 工作_变量 = 0;
  垃回对象_结 *原始弱_变量, *原始全部_变量;
  垃回对象_结 *再次灰色_变量 = g->再次灰色_变量;  /* 月词法_保存_函 original 列表_变量 */
  g->再次灰色_变量 = NULL;
  限制_月亮_断言_宏名(g->短命的_圆 == NULL && g->弱的_圆 == NULL);
  限制_月亮_断言_宏名(!垃圾回收_是否白色_宏名(g->主线程_圆));
  g->垃回状态机_缩小写 = 垃圾回收_GCS原子态_宏名;
  垃圾回收_记号对象_宏名(g, L);  /* 记号_变量 running 线程_变量 */
  /* 注册表_变量 and global metatables may be 已更改_变量 by API */
  垃圾回收_记号值_宏名(g, &g->l_注册表半);
  月垃圾回收_记号元表_函(g);  /* 记号_变量 global metatables */
  工作_变量 += 月垃圾回收_扩散全部_函(g);  /* empties '灰的色_圆' 列表_变量 */
  /* remark occasional 上值们_小写 of (maybe) dead threads */
  工作_变量 += 月垃圾回收_重新记号上值们_函(g);
  工作_变量 += 月垃圾回收_扩散全部_函(g);  /* propagate changes */
  g->灰的色_圆 = 再次灰色_变量;
  工作_变量 += 月垃圾回收_扩散全部_函(g);  /* traverse '再次灰色_变量' 列表_变量 */
  月垃圾回收_收敛短暂_函(g);
  /* at this 针点_变量, all strongly accessible objects are 已记号_变量. */
  /* Clear values from 弱的_圆 tables, before checking finalizers */
  月垃圾回收_通过值清理_函(g, g->弱的_圆, NULL);
  月垃圾回收_通过值清理_函(g, g->全部弱_圆, NULL);
  原始弱_变量 = g->弱的_圆; 原始全部_变量 = g->全部弱_圆;
  月垃圾回收_分离存在的fnz_函(g, 0);  /* separate objects 到_变量 be finalized */
  工作_变量 += 月垃圾回收_记号存在的fnz_函(g);  /* 记号_变量 objects that will be finalized */
  工作_变量 += 月垃圾回收_扩散全部_函(g);  /* remark, 到_变量 propagate 'resurrection' */
  月垃圾回收_收敛短暂_函(g);
  /* at this 针点_变量, all resurrected objects are 已记号_变量. */
  /* remove dead objects from 弱的_圆 tables */
  月垃圾回收_通过键清理_函(g, g->短命的_圆);  /* clear keys from all 短命的_圆 tables */
  月垃圾回收_通过键清理_函(g, g->全部弱_圆);  /* clear keys from all '全部弱_圆' tables */
  /* clear values from resurrected 弱的_圆 tables */
  月垃圾回收_通过值清理_函(g, g->弱的_圆, 原始弱_变量);
  月垃圾回收_通过值清理_函(g, g->全部弱_圆, 原始全部_变量);
  月亮字符串_清除缓存_函(g);
  g->当前白色_圆 = 限制_类型转换_字节_宏名(垃圾回收_其他白色_宏名(g));  /* 折叠_变量 当前_圆 白色_变量 */
  限制_月亮_断言_宏名(g->灰的色_圆 == NULL);
  return 工作_变量;  /* 估计_变量 of slots 已记号_变量 by '月垃圾回收_原子_函' */
}


static int 月垃圾回收_扫除步进_函 (炉_状态机结 *L, 全局_状态机结 *g,
                      int nextstate, 垃回对象_结 **nextlist) {
  if (g->扫除垃回_圆缩) {
    l_内存缩 旧债务_变量 = g->垃回债_缩;
    int 计数_变量;
    g->扫除垃回_圆缩 = 月垃圾回收_扫除列表_函(L, g->扫除垃回_圆缩, 垃圾回收_GC扫除最大_宏名, &计数_变量);
    g->垃回估计_缩 += g->垃回债_缩 - 旧债务_变量;  /* update 估计_变量 */
    return 计数_变量;
  }
  else {  /* enter 下一个_变量 状态机_变量 */
    g->垃回状态机_缩小写 = nextstate;
    g->扫除垃回_圆缩 = nextlist;
    return 0;  /* no 工作_变量 done */
  }
}


static lu_mem 月垃圾回收_单步进_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  lu_mem 工作_变量;
  限制_月亮_断言_宏名(!g->垃回紧急停_缩);  /* collector is not reentrant */
  g->垃回紧急停_缩 = 1;  /* no emergency collections while collecting */
  switch (g->垃回状态机_缩小写) {
    case 垃圾回收_GCS暂停_宏名: {
      月垃圾回收_重新开始回收_函(g);
      g->垃回状态机_缩小写 = 垃圾回收_GCS传播_宏名;
      工作_变量 = 1;
      break;
    }
    case 垃圾回收_GCS传播_宏名: {
      if (g->灰的色_圆 == NULL) {  /* no 更多_变量 灰的色_圆 objects? */
        g->垃回状态机_缩小写 = 垃圾回收_GCS进入原子态_宏名;  /* finish propagate phase */
        工作_变量 = 0;
      }
      else
        工作_变量 = 月垃圾回收_扩散记号_函(g);  /* traverse one 灰的色_圆 object */
      break;
    }
    case 垃圾回收_GCS进入原子态_宏名: {
      工作_变量 = 月垃圾回收_原子_函(L);  /* 工作_变量 is 什么_变量 was traversed by '月垃圾回收_原子_函' */
      月垃圾回收_进入扫除_函(L);
      g->垃回估计_缩 = 状态机_获取全部字节_宏名(g);  /* 首先_变量 估计_变量 */;
      break;
    }
    case 垃圾回收_GCS扫除所有GC_宏名: {  /* sweep "regular" objects */
      工作_变量 = 月垃圾回收_扫除步进_函(L, g, 垃圾回收_GCS扫除终结对象_宏名, &g->最终对象_缩);
      break;
    }
    case 垃圾回收_GCS扫除终结对象_宏名: {  /* sweep objects with finalizers */
      工作_变量 = 月垃圾回收_扫除步进_函(L, g, 垃圾回收_GCS扫除待fnz_宏名, &g->待fnz_缩半);
      break;
    }
    case 垃圾回收_GCS扫除待fnz_宏名: {  /* sweep objects 到_变量 be finalized */
      工作_变量 = 月垃圾回收_扫除步进_函(L, g, 垃圾回收_GCS扫除终_宏名, NULL);
      break;
    }
    case 垃圾回收_GCS扫除终_宏名: {  /* finish sweeps */
      月垃圾回收_检查大小们_函(L, g);
      g->垃回状态机_缩小写 = 垃圾回收_GCS调用终结器_宏名;
      工作_变量 = 0;
      break;
    }
    case 垃圾回收_GCS调用终结器_宏名: {  /* call remaining finalizers */
      if (g->待fnz_缩半 && !g->垃回紧急_缩圆) {
        g->垃回紧急停_缩 = 0;  /* ok collections during finalizers */
        工作_变量 = 月垃圾回收_跑几个终结器_函(L, 垃圾回收_GC终结最大_宏名) * 垃圾回收_GC最终执行成本_宏名;
      }
      else {  /* emergency 模块_变量 or no 更多_变量 finalizers */
        g->垃回状态机_缩小写 = 垃圾回收_GCS暂停_宏名;  /* finish collection */
        工作_变量 = 0;
      }
      break;
    }
    default: 限制_月亮_断言_宏名(0); return 0;
  }
  g->垃回紧急停_缩 = 0;
  return 工作_变量;
}


/*
** advances the garbage collector until it reaches a 状态机_变量 allowed
** by 'statemask'
*/
void 月亮编译_跑直到状态_函 (炉_状态机结 *L, int statesmask) {
  全局_状态机结 *g = G(L);
  while (!垃圾回收_测试位_宏名(statesmask, g->垃回状态机_缩小写))
    月垃圾回收_单步进_函(L);
}



/*
** Performs a basic incremental 步进_变量. The 负债_变量 and 步进_变量 大小_变量 are
** converted from bytes 到_变量 "units of 工作_变量"; then the function loops
** running single steps until adding that many units of 工作_变量 or
** finishing a cycle (暂停_变量 状态机_变量). Finally, it sets the 负债_变量 that
** controls when 下一个_变量 步进_变量 will be performed.
*/
static void 月垃圾回收_递增步进_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  int 步进乘数_变量 = (垃圾回收_获取gc形参_宏名(g->垃回步进乘法_短缩) | 1);  /* avoid division by 0 */
  l_内存缩 负债_变量 = (g->垃回债_缩 / 垃圾回收_工作到内存_宏名) * 步进乘数_变量;
  l_内存缩 步进大小_变量 = (g->垃回步进大小_缩圆 <= 限制_2为底对数最大值_宏名(l_内存缩))
                 ? ((限制_类型转换_宏名(l_内存缩, 1) << g->垃回步进大小_缩圆) / 垃圾回收_工作到内存_宏名) * 步进乘数_变量
                 : 限制_最大_L内存_宏名;  /* overflow; keep maximum 值_圆 */
  do {  /* repeat until 暂停_变量 or enough "credit" (negative 负债_变量) */
    lu_mem 工作_变量 = 月垃圾回收_单步进_函(L);  /* perform one single 步进_变量 */
    负债_变量 -= 工作_变量;
  } while (负债_变量 > -步进大小_变量 && g->垃回状态机_缩小写 != 垃圾回收_GCS暂停_宏名);
  if (g->垃回状态机_缩小写 == 垃圾回收_GCS暂停_宏名)
    月垃圾回收_设置暂停_函(g);  /* 暂停_变量 until 下一个_变量 cycle */
  else {
    负债_变量 = (负债_变量 / 步进乘数_变量) * 垃圾回收_工作到内存_宏名;  /* convert '工作_变量 units' 到_变量 bytes */
    月亮错误_设置债务_函(g, 负债_变量);
  }
}

/*
** Performs a basic GC 步进_变量 if collector is running. (If collector is
** not running, set a reasonable 负债_变量 到_变量 avoid it being called at
** every single 月解析器_检查_函.)
*/
void 月亮编译_步进_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  if (!垃圾回收_gc正在跑_宏名(g))  /* not running? */
    月亮错误_设置债务_函(g, -2000);
  else {
    if(垃圾回收_是否减少GC模式生成_宏名(g))
      月垃圾回收_生成步进_函(L, g);
    else
      月垃圾回收_递增步进_函(L, g);
  }
}


/*
** Perform a full collection in incremental 模块_变量.
** Before running the collection, 月解析器_检查_函 '垃圾回收_保持不变_宏名'; if it is true,
** there may be some objects 已记号_变量 as black, so the collector has
** 到_变量 sweep all objects 到_变量 turn them back 到_变量 白色_变量 (as 白色_变量 has not
** 已更改_变量, nothing will be collected).
*/
static void 月垃圾回收_完全递增_函 (炉_状态机结 *L, 全局_状态机结 *g) {
  if (垃圾回收_保持不变_宏名(g))  /* black objects? */
    月垃圾回收_进入扫除_函(L); /* sweep everything 到_变量 turn them back 到_变量 白色_变量 */
  /* finish any pending sweep phase 到_变量 起始_变量 a new cycle */
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS暂停_宏名));
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS调用终结器_宏名));  /* run 上_小变量 到_变量 finalizers */
  /* 估计_变量 must be correct after a full GC cycle */
  限制_月亮_断言_宏名(g->垃回估计_缩 == 状态机_获取全部字节_宏名(g));
  月亮编译_跑直到状态_函(L, 垃圾回收_位掩码_宏名(垃圾回收_GCS暂停_宏名));  /* finish collection */
  月垃圾回收_设置暂停_函(g);
}


/*
** Performs a full GC cycle; if 'isemergency', set a flag 到_变量 avoid
** some operations which could 改变_变量 the interpreter 状态机_变量 in some
** unexpected ways (running finalizers and shrinking some structures).
*/
void 月亮编译_完全垃圾回收_函 (炉_状态机结 *L, int isemergency) {
  全局_状态机结 *g = G(L);
  限制_月亮_断言_宏名(!g->垃回紧急_缩圆);
  g->垃回紧急_缩圆 = isemergency;  /* set flag */
  if (g->垃回种类_缩小写 == 状态机_K垃圾回收_递增_宏名)
    月垃圾回收_完全递增_函(L, g);
  else
    月垃圾回收_完全生成_函(L, g);
  g->垃回紧急_缩圆 = 0;
}

/* }====================================================== */


