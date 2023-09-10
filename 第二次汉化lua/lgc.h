/*
** $Id: lgc.h $
** Garbage Collector
** See Copyright Notice in lua.h
*/

#ifndef lgc_h
#define lgc_h


#include "lobject.h"
#include "lstate.h"

/*
** Collectable objects may have one of three colors: 白色_变量, which means
** the object is not 已记号_变量; 灰的色_圆, which means the object is 已记号_变量, but
** its references may be not 已记号_变量; and black, which means that the
** object and all its references are 已记号_变量.  The main invariant of the
** garbage collector, while marking objects, is that a black object can
** never 针点_变量 到_变量 a 白色_变量 one. Moreover, any 灰的色_圆 object must be in a
** "灰的色_圆 列表_变量" (灰的色_圆, 再次灰色_变量, 弱的_圆, 全部弱_圆, 短命的_圆) so that it
** can be visited again before finishing the collection cycle. (Open
** 上值们_小写 are an exception 到_变量 this rule.)  These lists have no meaning
** when the invariant is not being enforced (e.g., sweep phase).
*/


/*
** Possible states of the Garbage Collector
*/
#define 垃圾回收_GCS传播_宏名	0
#define 垃圾回收_GCS进入原子态_宏名	1
#define 垃圾回收_GCS原子态_宏名	2
#define 垃圾回收_GCS扫除所有GC_宏名	3
#define 垃圾回收_GCS扫除终结对象_宏名	4
#define 垃圾回收_GCS扫除待fnz_宏名	5
#define 垃圾回收_GCS扫除终_宏名	6
#define 垃圾回收_GCS调用终结器_宏名	7
#define 垃圾回收_GCS暂停_宏名	8


#define 垃圾回收_是否扫除阶段_宏名(g)  \
	(垃圾回收_GCS扫除所有GC_宏名 <= (g)->垃回状态机_缩小写 && (g)->垃回状态机_缩小写 <= 垃圾回收_GCS扫除终_宏名)


/*
** macro 到_变量 tell when main invariant (白色_变量 objects 月编译器_不能_函 针点_变量 到_变量 black
** ones) must be kept. During a collection, the sweep
** phase may break the invariant, as objects turned 白色_变量 may 针点_变量 到_变量
** still-black objects. The invariant is restored when sweep ends and
** all objects are 白色_变量 again.
*/

#define 垃圾回收_保持不变_宏名(g)	((g)->垃回状态机_缩小写 <= 垃圾回收_GCS原子态_宏名)


/*
** some useful bit tricks
*/
#define 垃圾回收_重置位们_宏名(x,m)		((x) &= 限制_类型转换_字节_宏名(~(m)))
#define 垃圾回收_设置位们_宏名(x,m)		((x) |= (m))
#define 垃圾回收_测试位们_宏名(x,m)		((x) & (m))
#define 垃圾回收_位掩码_宏名(b)		(1<<(b))
#define 垃圾回收_位到掩码_宏名(b1,b2)		(垃圾回收_位掩码_宏名(b1) | 垃圾回收_位掩码_宏名(b2))
#define 垃圾回收_l_设置位_宏名(x,b)		垃圾回收_设置位们_宏名(x, 垃圾回收_位掩码_宏名(b))
#define 垃圾回收_重置位_宏名(x,b)		垃圾回收_重置位们_宏名(x, 垃圾回收_位掩码_宏名(b))
#define 垃圾回收_测试位_宏名(x,b)		垃圾回收_测试位们_宏名(x, 垃圾回收_位掩码_宏名(b))


/*
** Layout for bit use in '已记号_变量' 月解析器_字段_函. First three bits are
** used for object "age" in generational 模块_变量. Last bit is used
** by tests.
*/
#define 垃圾回收_白色0位_宏名	3  /* object is 白色_变量 (type 0) */
#define 垃圾回收_白色1位_宏名	4  /* object is 白色_变量 (type 1) */
#define 垃圾回收_黑色位_宏名	5  /* object is black */
#define 垃圾回收_终结位_宏名	6  /* object has been 已记号_变量 for finalization */

#define 垃圾回收_测试位_宏名		7



#define 垃圾回收_白色位_宏名	垃圾回收_位到掩码_宏名(垃圾回收_白色0位_宏名, 垃圾回收_白色1位_宏名)


#define 垃圾回收_是否白色_宏名(x)      垃圾回收_测试位们_宏名((x)->已记号_变量, 垃圾回收_白色位_宏名)
#define 垃圾回收_是否黑色_宏名(x)      垃圾回收_测试位_宏名((x)->已记号_变量, 垃圾回收_黑色位_宏名)
#define 垃圾回收_是否灰色_宏名(x)  /* neither 白色_变量 nor black */  \
	(!垃圾回收_测试位们_宏名((x)->已记号_变量, 垃圾回收_白色位_宏名 | 垃圾回收_位掩码_宏名(垃圾回收_黑色位_宏名)))

#define 垃圾回收_到终结_宏名(x)	垃圾回收_测试位_宏名((x)->已记号_变量, 垃圾回收_终结位_宏名)

#define 垃圾回收_其他白色_宏名(g)	((g)->当前白色_圆 ^ 垃圾回收_白色位_宏名)
#define 垃圾回收_是否死亡m_宏名(拥者_缩变量,m)	((m) & (拥者_缩变量))
#define 垃圾回收_是否死亡_宏名(g,v)	垃圾回收_是否死亡m_宏名(垃圾回收_其他白色_宏名(g), (v)->已记号_变量)

#define 垃圾回收_改变为白色_宏名(x)	((x)->已记号_变量 ^= 垃圾回收_白色位_宏名)
#define 垃圾回收_新白到黑_宏名(x)  \
	限制_检查_表达式_宏名(!垃圾回收_是否白色_宏名(x), 垃圾回收_l_设置位_宏名((x)->已记号_变量, 垃圾回收_黑色位_宏名))

#define 垃圾回收_月亮C_白色_宏名(g)	限制_类型转换_字节_宏名((g)->当前白色_圆 & 垃圾回收_白色位_宏名)


/* object age in generational 模块_变量 */
#define 垃圾回收_G_新_宏名		0	/* created in 当前_圆 cycle */
#define 垃圾回收_G_存活_宏名	1	/* created in 前一个_变量 cycle */
#define 垃圾回收_G_旧0_宏名		2	/* 已记号_变量 旧_变量 by frw. barrier in this cycle */
#define 垃圾回收_G_旧1_宏名		3	/* 首先_变量 full cycle as 旧_变量 */
#define 垃圾回收_G_旧_宏名		4	/* really 旧_变量 object (not 到_变量 be visited) */
#define 垃圾回收_G_已摸1_宏名	5	/* 旧_变量 object touched this cycle */
#define 垃圾回收_G_已摸2_宏名	6	/* 旧_变量 object touched in 前一个_变量 cycle */

#define 垃圾回收_年龄位们_宏名		7  /* all age bits (111) */

#define 垃圾回收_获取年龄_宏名(o)	((o)->已记号_变量 & 垃圾回收_年龄位们_宏名)
#define 垃圾回收_设置年龄_宏名(o,a)  ((o)->已记号_变量 = 限制_类型转换_字节_宏名(((o)->已记号_变量 & (~垃圾回收_年龄位们_宏名)) | a))
#define 垃圾回收_是否旧_宏名(o)	(垃圾回收_获取年龄_宏名(o) > 垃圾回收_G_存活_宏名)

#define 垃圾回收_改变年龄_宏名(o,f,t)  \
	限制_检查_表达式_宏名(垃圾回收_获取年龄_宏名(o) == (f), (o)->已记号_变量 ^= ((f)^(t)))


/* Default Values for GC parameters */
#define 垃圾回收_月亮I_生成主要乘数_宏名         100
#define 垃圾回收_月亮I_生成次要乘数_宏名         20

/* wait memory 到_变量 double before starting new cycle */
#define 垃圾回收_月亮I_GC暂停_宏名    200

/*
** some 垃回_小写缩 parameters are stored divided by 4 到_变量 allow a maximum 值_圆
** 上_小变量 到_变量 1023 in a '路_字节型'.
*/
#define 垃圾回收_获取gc形参_宏名(p)	((p) * 4)
#define 垃圾回收_设置gc形参_宏名(p,v)	((p) = (v) / 4)

#define 垃圾回收_月亮I_GC乘数_宏名      100

/* how much 到_变量 allocate before 下一个_变量 GC 步进_变量 (log2) */
#define 垃圾回收_月亮I_GC步大小_宏名 13      /* 8 KB */


/*
** Check whether the declared GC 模块_变量 is generational. While in
** generational 模块_变量, the collector can go temporarily 到_变量 incremental
** 模块_变量 到_变量 improve performance. This is signaled by 'g->最后原子_变量 != 0'.
*/
#define 垃圾回收_是否减少GC模式生成_宏名(g)	(g->垃回种类_缩小写 == 状态机_K垃圾回收_生成_宏名 || g->最后原子_变量 != 0)


/*
** Control when GC is running:
*/
#define 垃圾回收_GC停止用户_宏名	1  /* bit true when GC stopped by user */
#define 垃圾回收_GC停止GC_宏名		2  /* bit true when GC stopped by itself */
#define 垃圾回收_GC停止CLS_宏名	4  /* bit true when closing Lua 状态机_变量 */
#define 垃圾回收_gc正在跑_宏名(g)	((g)->垃回步进_短缩 == 0)


/*
** Does one 步进_变量 of collection when 负债_变量 becomes positive. 'pre'/'位置_缩变量'
** allows some adjustments 到_变量 be done only when 已需要_变量. macro
** '限制_条件改变内存_宏名' is used only for heavy tests (forcing a full
** GC cycle on every opportunity)
*/
#define 垃圾回收_月亮C_条件GC_宏名(L,pre,位置_缩变量) \
	{ if (G(L)->垃回债_缩 > 0) { pre; 月亮编译_步进_函(L); 位置_缩变量;}; \
	  限制_条件改变内存_宏名(L,pre,位置_缩变量); }

/* 更多_变量 often than not, 'pre'/'位置_缩变量' are 空容器_变量 */
#define 垃圾回收_月亮C_检查GC_宏名(L)		垃圾回收_月亮C_条件GC_宏名(L,(void)0,(void)0)


#define 垃圾回收_月亮C_对象屏障_宏名(L,p,o) (  \
	(垃圾回收_是否黑色_宏名(p) && 垃圾回收_是否白色_宏名(o)) ? \
	月亮编译_屏障_函(L,状态机_对象到垃圾回收对象_宏名(p),状态机_对象到垃圾回收对象_宏名(o)) : 限制_类型转换_空的_宏名(0))

#define 垃圾回收_月亮C_屏障_宏名(L,p,v) (  \
	对象_是否可收集_宏名(v) ? 垃圾回收_月亮C_对象屏障_宏名(L,p,对象_垃圾回收值_宏名(v)) : 限制_类型转换_空的_宏名(0))

#define 垃圾回收_月亮C_对象屏障后退_宏名(L,p,o) (  \
	(垃圾回收_是否黑色_宏名(p) && 垃圾回收_是否白色_宏名(o)) ? 月亮编译_屏障回退_函(L,p) : 限制_类型转换_空的_宏名(0))

#define 垃圾回收_月亮C_屏障后退_宏名(L,p,v) (  \
	对象_是否可收集_宏名(v) ? 垃圾回收_月亮C_对象屏障后退_宏名(L, p, 对象_垃圾回收值_宏名(v)) : 限制_类型转换_空的_宏名(0))

配置_月亮I_函_宏名 void 月亮编译_修复_函 (炉_状态机结 *L, 垃回对象_结 *o);
配置_月亮I_函_宏名 void 月亮编译_释放所有对象们_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮编译_步进_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮编译_跑直到状态_函 (炉_状态机结 *L, int statesmask);
配置_月亮I_函_宏名 void 月亮编译_完全垃圾回收_函 (炉_状态机结 *L, int isemergency);
配置_月亮I_函_宏名 垃回对象_结 *月亮编译_新对象_函 (炉_状态机结 *L, int 类标_缩变量, size_t 大小_短变量);
配置_月亮I_函_宏名 垃回对象_结 *月亮编译_新对象dt_函 (炉_状态机结 *L, int 类标_缩变量, size_t 大小_短变量,
                                                 size_t 偏移_变量);
配置_月亮I_函_宏名 void 月亮编译_屏障_函 (炉_状态机结 *L, 垃回对象_结 *o, 垃回对象_结 *v);
配置_月亮I_函_宏名 void 月亮编译_屏障回退_函 (炉_状态机结 *L, 垃回对象_结 *o);
配置_月亮I_函_宏名 void 月亮编译_检查终结器_函 (炉_状态机结 *L, 垃回对象_结 *o, 表_结 *元表_缩变量);
配置_月亮I_函_宏名 void 月亮编译_改变模式_函 (炉_状态机结 *L, int newmode);


#endif
