/*
** $Id: lstate.h $
** Global State
** See Copyright Notice in lua.h
*/

#ifndef lstate_h
#define lstate_h

#include "lua.h"


/* Some header files included here need this definition */
typedef struct 调用信息_结 调用信息_结;


#include "lobject.h"
#include "ltm.h"
#include "lzio.h"


/*
** Some notes about garbage-collected objects: All objects in Lua must
** be kept somehow accessible until being freed, so all objects always
** belong 到_变量 one (and only one) of these lists, using 月解析器_字段_函 '下一个_变量' of
** the '共用头_驼峰' for the link:
**
** '全部垃回_圆缩': all objects not 已记号_变量 for finalization;
** '最终对象_缩': all objects 已记号_变量 for finalization;
** '待fnz_缩半': all objects ready 到_变量 be finalized;
** '固定垃回_圆缩': all objects that are not 到_变量 be collected (currently
** only small strings, such as reserved words).
**
** For the generational collector, some of these lists have marks for
** generations. Each 记号_变量 points 到_变量 the 首先_变量 element in the 列表_变量 for
** that particular generation; that generation goes until the 下一个_变量 记号_变量.
**
** '全部垃回_圆缩' -> '生存_圆': new objects;
** '生存_圆' -> '旧_变量': objects that survived one collection;
** '旧的1_小写' -> '真的旧_圆': objects that became 旧_变量 in 最后_变量 collection;
** '真的旧_圆' -> NULL: objects 旧_变量 for 更多_变量 than one cycle.
**
** '最终对象_缩' -> '最终对象生存_短小写': new objects 已记号_变量 for finalization;
** '最终对象生存_短小写' -> '最终对象旧1_缩': survived   """";
** '最终对象旧1_缩' -> '最终对象真的旧_缩': just 旧_变量  """";
** '最终对象真的旧_缩' -> NULL: really 旧_变量       """".
**
** All lists can contain elements older than their main ages, due
** 到_变量 '月亮编译_检查终结器_函' and '月垃圾回收_用户数据到完善_函', which move
** objects between the normal lists and the "已记号_变量 for finalization"
** lists. Moreover, barriers can age young objects in young lists as
** OLD0, which then become OLD1. However, a 列表_变量 never contains
** elements younger than their main ages.
**
** The generational collector also uses a pointer '首先旧1_圆', which
** points 到_变量 the 首先_变量 OLD1 object in the 列表_变量. It is used 到_变量 optimize
** '月垃圾回收_记号旧_函'. (Potentially OLD1 objects can be anywhere between '全部垃回_圆缩'
** and '真的旧_圆', but often the 列表_变量 has no OLD1 objects or they are
** after '旧的1_小写'.) Note the difference between it and '旧的1_小写':
** '首先旧1_圆': no OLD1 objects before this 针点_变量; there can be all
**   ages after it.
** '旧的1_小写': no objects younger than OLD1 after this 针点_变量.
*/

/*
** Moreover, there is another set of lists that control 灰的色_圆 objects.
** These lists are linked by fields '垃回列表_缩'. (All objects that
** can become 灰的色_圆 have such a 月解析器_字段_函. The 月解析器_字段_函 is not the same
** in all objects, but it always has this 名称_变量.)  Any 灰的色_圆 object
** must belong 到_变量 one of these lists, and all objects in these lists
** must be 灰的色_圆 (with two exceptions explained below):
**
** '灰的色_圆': regular 灰的色_圆 objects, still waiting 到_变量 be visited.
** '再次灰色_变量': objects that must be revisited at the 月垃圾回收_原子_函 phase.
**   That includes
**   - black objects got in a write barrier;
**   - all kinds of 弱的_圆 tables during propagation phase;
**   - all threads.
** '弱的_圆': tables with 弱的_圆 values 到_变量 be cleared;
** '短命的_圆': 短命的_圆 tables with 白色_变量->白色_变量 entries;
** '全部弱_圆': tables with 弱的_圆 keys and/or 弱的_圆 values 到_变量 be cleared.
**
** The exceptions 到_变量 that "灰的色_圆 rule" are:
** - TOUCHED2 objects in generational 模块_变量 stay in a 灰的色_圆 列表_变量 (because
** they must be visited again at the 终_变量 of the cycle), but they are
** 已记号_变量 black because assignments 到_变量 them must activate barriers (到_变量
** move them back 到_变量 TOUCHED1).
** - Open upvales are kept 灰的色_圆 到_变量 avoid barriers, but they stay out
** of 灰的色_圆 lists. (They don't even have a '垃回列表_缩' 月解析器_字段_函.)
*/



/*
** About 'C调用们数_缩圆':  This 计数_变量 has two parts: the lower 16 bits counts
** the number of recursive invocations in the C 栈_圆小; the higher
** 16 bits counts the number of non-状态机_可让步_宏名 calls in the 栈_圆小.
** (They are together so that we can 改变_变量 and 月词法_保存_函 both with one
** instruction.)
*/


/* true if this 线程_变量 does not have non-状态机_可让步_宏名 calls in the 栈_圆小 */
#define 状态机_可让步_宏名(L)		(((L)->C调用们数_缩圆 & 0xffff0000) == 0)

/* real number of C calls */
#define 状态机_获取C调用_宏名(L)	((L)->C调用们数_缩圆 & 0xffff)


/* Increment the number of non-状态机_可让步_宏名 calls */
#define 状态机_递增n不让步_宏名(L)	((L)->C调用们数_缩圆 += 0x10000)

/* Decrement the number of non-状态机_可让步_宏名 calls */
#define 状态机_递减n不让步_宏名(L)	((L)->C调用们数_缩圆 -= 0x10000)

/* Non-状态机_可让步_宏名 call increment */
#define 状态机_不让步调用信息_宏名	(0x10000 | 1)




struct 炉_长跳结;  /* defined in ldo.c */


/*
** Atomic type (relative 到_变量 signals) 到_变量 better ensure that '月亮_设置钩子_函'
** is 线程_变量 safe
*/
#if !defined(l_型号T型)
#include <signal.h>
#define l_型号T型	sig_atomic_t
#endif


/*
** Extra 栈_圆小 空间_圆 到_变量 handle TM calls and some other extras. This
** 空间_圆 is not included in '栈_最后圆'. It is used only 到_变量 avoid 栈_圆小
** checks, either because the element will be promptly popped or because
** there will be a 栈_圆小 月解析器_检查_函 soon after the push. Function frames
** never use this 额外_变量 空间_圆, so it does not need 到_变量 be kept clean.
*/
#define 状态机_额外_栈_宏名   5


#define 状态机_基本_栈_大小_宏名        (2*月头_月亮_最小栈_宏名)

#define 状态机_栈大小_宏名(到钩_变量)	限制_类型转换_整型_宏名((到钩_变量)->栈_最后圆.p - (到钩_变量)->栈_圆小.p)


/* kinds of Garbage Collection */
#define 状态机_K垃圾回收_递增_宏名		0	/* incremental 垃回_小写缩 */
#define 状态机_K垃圾回收_生成_宏名		1	/* generational 垃回_小写缩 */


typedef struct 字符串表_结 {
  标签字符串_结 **哈希_小写;
  int 使用数_缩;  /* number of elements */
  int 大小_变量;
} 字符串表_结;


/*
** Information about a call.
** About union 'u':
** - 月解析器_字段_函 'l' is used only for Lua functions;
** - 月解析器_字段_函 'c' is used only for C functions.
** About union 'u2':
** - 月解析器_字段_函 '函索引_短' is used only by C functions while doing a
** protected call;
** - 月解析器_字段_函 '让步数_缩' is used only while a function is "doing" an
** yield (from the yield until the 下一个_变量 月做_恢复_函);
** - 月解析器_字段_函 '结果数目_变量' is used only while closing 待关闭_缩变量 variables when
** returning from a function;
** - 月解析器_字段_函 '转移信息_结' is used only during call/returnhooks,
** before the function starts or after it ends.
*/
struct 调用信息_结 {
  相对栈身份_联 函_短变量;  /* function index in the 栈_圆小 */
  相对栈身份_联	顶部_变量;  /* 顶部_变量 for this function */
  struct 调用信息_结 *前一个_变量, *下一个_变量;  /* dynamic call link */
  union {
    struct {  /* only for Lua functions */
      const Instruction *已保存程计_缩;
      volatile l_型号T型 陷阱_变量;
      int 额外实参数_缩;  /* # of 额外_变量 arguments in vararg functions */
    } l;
    struct {  /* only for C functions */
      炉_K函数型 k;  /* continuation in case of yields */
      ptrdiff_t 旧_错函短;
      炉_K上下文型 ctx;  /* context 信息_短变量. in case of yields */
    } c;
  } u;
  union {
    int 函索引_短;  /* called-function index */
    int 让步数_缩;  /* number of values yielded */
    int 结果数目_变量;  /* number of values returned */
    struct {  /* 信息_短变量 about transferred values (for call/return hooks) */
      unsigned short 函传输_变量;  /* 偏移_变量 of 首先_变量 值_圆 transferred */
      unsigned short 转移数_缩;  /* number of values transferred */
    } 转移信息_结;
  } u2;
  short 结果数目_变量;  /* expected number of results from this function */
  unsigned short 调用状态码_圆;
};


/*
** Bits in 调用信息_结 状态码_变量
*/
#define 状态机_调信状型_旧活动钩子_宏名	(1<<0)	/* original 值_圆 of '允许钩子_圆' */
#define 状态机_调信状型_C_宏名		(1<<1)	/* call is running a C function */
#define 状态机_调信状型_新鲜_宏名	(1<<2)	/* call is on a fresh "月亮虚拟机_执行_函" frame */
#define 状态机_调信状型_已钩子_宏名	(1<<3)	/* call is running a debug 钩子_变量 */
#define 状态机_调信状型_让步保护调用_宏名	(1<<4)	/* doing a 状态机_可让步_宏名 protected call */
#define 状态机_调信状型_尾部_宏名	(1<<5)	/* call was tail called */
#define 状态机_调信状型_钩子让步_宏名	(1<<6)	/* 最后_变量 钩子_变量 called yielded */
#define 状态机_调信状型_终结的_宏名	(1<<7)	/* function "called" a finalizer */
#define 状态机_调信状型_传输_宏名	(1<<8)	/* '调信_缩变量' has transfer information */
#define 状态机_调信状型_闭包返回_宏名	(1<<9)  /* function is closing 待关闭_缩变量 variables */
/* Bits 10-12 are used for 状态机_调信状型_递归调用状型_宏名 (see below) */
#define 状态机_调信状型_递归调用状型_宏名	10
#if defined(配置_月亮_兼容_小于_小等_宏名)
#define 状态机_调信状型_限制相等_宏名	(1<<13)  /* using __lt for __le */
#endif


/*
** Field 状态机_调信状型_递归调用状型_宏名 stores the "recover 状态码_变量", used 到_变量 keep the 错误_小变量
** 状态码_变量 while closing 到_变量-be-closed variables in coroutines, so that
** Lua can correctly 月做_恢复_函 after an yield from a __close method called
** because of an 错误_小变量.  (Three bits are enough for 错误_小变量 状态码_变量.)
*/
#define 状态机_获取调信状型递归调用状型_宏名(调信_缩变量)     (((调信_缩变量)->调用状态码_圆 >> 状态机_调信状型_递归调用状型_宏名) & 7)
#define 状态机_设置调信状型递归调用状型_宏名(调信_缩变量,状_变量)  \
  限制_检查_表达式_宏名(((状_变量) & 7) == (状_变量),   /* 状态码_变量 must fit in three bits */  \
            ((调信_缩变量)->调用状态码_圆 = ((调信_缩变量)->调用状态码_圆 & ~(7 << 状态机_调信状型_递归调用状型_宏名))  \
                                                  | ((状_变量) << 状态机_调信状型_递归调用状型_宏名)))


/* active function is a Lua function */
#define 状态机_是否月亮_宏名(调信_缩变量)	(!((调信_缩变量)->调用状态码_圆 & 状态机_调信状型_C_宏名))

/* call is running Lua 代码_变量 (not a 钩子_变量) */
#define 状态机_是否月亮代码_宏名(调信_缩变量)	(!((调信_缩变量)->调用状态码_圆 & (状态机_调信状型_C_宏名 | 状态机_调信状型_已钩子_宏名)))

/* assume that 状态机_调信状型_旧活动钩子_宏名 has 偏移_变量 0 and that 'v' is strictly 0/1 */
#define 状态机_设置旧活动钩子_宏名(状_变量,v)	((状_变量) = ((状_变量) & ~状态机_调信状型_旧活动钩子_宏名) | (v))
#define 状态机_获取旧活动钩子_宏名(状_变量)	((状_变量) & 状态机_调信状型_旧活动钩子_宏名)


/*
** 'global 状态机_变量', shared by all threads of this 状态机_变量
*/
typedef struct 全局_状态机结 {
  炉_分配半 函重新分配_缩;  /* function 到_变量 reallocate memory */
  void *用数_缩变量;         /* auxiliary 数据_变量 到_变量 '函重新分配_缩' */
  l_内存缩 总共字节们_圆;  /* number of bytes currently allocated - 垃回债_缩 */
  l_内存缩 垃回债_缩;  /* bytes allocated not yet compensated by the collector */
  lu_mem 垃回估计_缩;  /* an 估计_变量 of the non-garbage memory in use */
  lu_mem 最后原子_变量;  /* see function '月垃圾回收_生成步进_函' in file 'lgc.c' */
  字符串表_结 串的表_缩;  /* 哈希_小写 table for strings */
  标签值_结 l_注册表半;
  标签值_结 空值的值_圆;  /* a nil 值_圆 */
  unsigned int 种籽_圆;  /* randomized 种籽_圆 for hashes */
  路_字节型 当前白色_圆;
  路_字节型 垃回状态机_缩小写;  /* 状态机_变量 of garbage collector */
  路_字节型 垃回种类_缩小写;  /* 种类_变量 of GC running */
  路_字节型 垃回紧急停_缩;  /* stops emergency collections */
  路_字节型 生成副乘法_缩;  /* control for minor generational collections */
  路_字节型 生成主乘法_缩;  /* control for major generational collections */
  路_字节型 垃回步进_短缩;  /* control whether GC is running */
  路_字节型 垃回紧急_缩圆;  /* true if this is an emergency collection */
  路_字节型 垃回暂停_缩圆;  /* 大小_变量 of 暂停_变量 between successive GCs */
  路_字节型 垃回步进乘法_短缩;  /* GC "speed" */
  路_字节型 垃回步进大小_缩圆;  /* (log2 of) GC granularity */
  垃回对象_结 *全部垃回_圆缩;  /* 列表_变量 of all collectable objects */
  垃回对象_结 **扫除垃回_圆缩;  /* 当前_圆 position of sweep in 列表_变量 */
  垃回对象_结 *最终对象_缩;  /* 列表_变量 of collectable objects with finalizers */
  垃回对象_结 *灰的色_圆;  /* 列表_变量 of 灰的色_圆 objects */
  垃回对象_结 *再次灰色_变量;  /* 列表_变量 of objects 到_变量 be traversed atomically */
  垃回对象_结 *弱的_圆;  /* 列表_变量 of tables with 弱的_圆 values */
  垃回对象_结 *短命的_圆;  /* 列表_变量 of 短命的_圆 tables (弱的_圆 keys) */
  垃回对象_结 *全部弱_圆;  /* 列表_变量 of all-弱的_圆 tables */
  垃回对象_结 *待fnz_缩半;  /* 列表_变量 of userdata 到_变量 be GC */
  垃回对象_结 *固定垃回_圆缩;  /* 列表_变量 of objects not 到_变量 be collected */
  /* fields for generational collector */
  垃回对象_结 *生存_圆;  /* 起始_变量 of objects that survived one GC cycle */
  垃回对象_结 *旧的1_小写;  /* 起始_变量 of 旧的1_小写 objects */
  垃回对象_结 *真的旧_圆;  /* objects 更多_变量 than one cycle 旧_变量 ("really 旧_变量") */
  垃回对象_结 *首先旧1_圆;  /* 首先_变量 OLD1 object in the 列表_变量 (if any) */
  垃回对象_结 *最终对象生存_短小写;  /* 列表_变量 of 生存_圆 objects with finalizers */
  垃回对象_结 *最终对象旧1_缩;  /* 列表_变量 of 旧的1_小写 objects with finalizers */
  垃回对象_结 *最终对象真的旧_缩;  /* 列表_变量 of really 旧_变量 objects with finalizers */
  struct 炉_状态机结 *线程与上值_短缩;  /* 列表_变量 of threads with 打开_圆 上值们_小写 */
  炉_C函数半 恐慌_圆;  /* 到_变量 be called in unprotected errors */
  struct 炉_状态机结 *主线程_圆;
  标签字符串_结 *内存错误消息_短;  /* message for memory-allocation errors */
  标签字符串_结 *标方名_变量[标方_数目大写];  /* 数组_圆 with tag-method names */
  struct 表_结 *元表_缩变量[炉_类型数们];  /* metatables for basic types */
  标签字符串_结 *串缓存_短圆[串缓存_N大写][串缓存_M大写];  /* cache for strings in API */
  炉_警告函数型 警函_短缩;  /* warning function */
  void *用h数j_警告缩;         /* auxiliary 数据_变量 到_变量 '警函_短缩' */
} 全局_状态机结;


/*
** 'per 线程_变量' 状态机_变量
*/
struct 炉_状态机结 {
  共用头_驼峰;
  路_字节型 状态码_变量;
  路_字节型 允许钩子_圆;
  unsigned short 调信数_全缩;  /* number of items in '调信_缩变量' 列表_变量 */
  相对栈身份_联 顶部_变量;  /* 首先_变量 free 插槽_变量 in the 栈_圆小 */
  全局_状态机结 *l_G;
  调用信息_结 *调信_缩变量;  /* call 信息_短变量 for 当前_圆 function */
  相对栈身份_联 栈_最后圆;  /* 终_变量 of 栈_圆小 (最后_变量 element + 1) */
  相对栈身份_联 栈_圆小;  /* 栈_圆小 基本_变量 */
  上值_结 *打开上值_圆小;  /* 列表_变量 of 打开_圆 上值们_小写 in this 栈_圆小 */
  相对栈身份_联 待关闭列表_结;  /* 列表_变量 of 到_变量-be-closed variables */
  垃回对象_结 *垃回列表_缩;
  struct 炉_状态机结 *线程与上值_短缩;  /* 列表_变量 of threads with 打开_圆 上值们_小写 */
  struct 炉_长跳结 *错误跳_圆缩;  /* 当前_圆 错误_小变量 recover 针点_变量 */
  调用信息_结 基本_调信圆缩;  /* 调用信息_结 for 首先_变量 层级_变量 (C calling Lua) */
  volatile 炉_钩子型 钩子_变量;
  ptrdiff_t 错函_短变量;  /* 当前_圆 错误_小变量 handling function (栈_圆小 index) */
  l_无符整32型 C调用们数_缩圆;  /* number of nested (non-状态机_可让步_宏名 | C)  calls */
  int 旧程计_变量;  /* 最后_变量 程序计数_变量 traced */
  int 基本钩子计数_圆;
  int 钩子计数_圆;
  volatile l_型号T型 钩子掩码_圆;
};


#define G(L)	(L->l_G)

/*
** 'g->空值的值_圆' being a nil 值_圆 标志们_变量 that the 状态机_变量 was completely
** build.
*/
#define 状态机_完成状态_宏名(g)	对象_tt是否空值_宏名(&g->空值的值_圆)


/*
** Union of all collectable objects (only for conversions)
** ISO C99, 6.5.2.3 p.5:
** "if a union contains several structures that share a common initial
** sequence [...], and if the union object currently contains one
** of these structures, it is permitted 到_变量 inspect the common initial
** part of any of them anywhere that a declaration of the complete type
** of the union is visible."
*/
union 垃回联合_联 {
  垃回对象_结 垃回_小写缩;  /* common header */
  struct 标签字符串_结 类s_变量;
  struct 用户数据_结 u;
  union 闭包_联 闭包_短变量;
  struct 表_结 h;
  struct 原型_结 p;
  struct 炉_状态机结 到钩_变量;  /* 线程_变量 */
  struct 上值_结 upv;
};


/*
** ISO C99, 6.7.2.1 p.14:
** "A pointer 到_变量 a union object, suitably converted, points 到_变量 each of
** its members [...], and vice versa."
*/
#define 状态机_类型转换_u_宏名(o)	限制_类型转换_宏名(union 垃回联合_联 *, (o))

/* macros 到_变量 convert a 垃回对象_结 into a specific 值_圆 */
#define 状态机_垃圾回收对象到t串_宏名(o)  \
	限制_检查_表达式_宏名(对象_无变体_宏名((o)->类标_缩变量) == 月头_月亮_T字符串_宏名, &((状态机_类型转换_u_宏名(o))->类s_变量))
#define 状态机_垃圾回收对象到u_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_V用户数据_宏名, &((状态机_类型转换_u_宏名(o))->u))
#define 状态机_垃圾回收对象到l闭包_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_VL闭包L_宏名, &((状态机_类型转换_u_宏名(o))->闭包_短变量.l))
#define 状态机_垃圾回收对象到c闭包_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_VC闭包L_宏名, &((状态机_类型转换_u_宏名(o))->闭包_短变量.c))
#define 状态机_垃圾回收对象到闭包_宏名(o)  \
	限制_检查_表达式_宏名(对象_无变体_宏名((o)->类标_缩变量) == 月头_月亮_T函数_宏名, &((状态机_类型转换_u_宏名(o))->闭包_短变量))
#define 状态机_垃圾回收对象到t_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_V表_宏名, &((状态机_类型转换_u_宏名(o))->h))
#define 状态机_垃圾回收对象到P_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_V原型_宏名, &((状态机_类型转换_u_宏名(o))->p))
#define 状态机_垃圾回收对象到th_宏名(o)  限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_V线程_宏名, &((状态机_类型转换_u_宏名(o))->到钩_变量))
#define 状态机_垃圾回收对象到上值_宏名(o)	限制_检查_表达式_宏名((o)->类标_缩变量 == 对象_月亮_V上值_宏名, &((状态机_类型转换_u_宏名(o))->upv))


/*
** macro 到_变量 convert a Lua object into a 垃回对象_结
** (The access 到_变量 '类标_缩变量' tries 到_变量 ensure that 'v' is actually a Lua object.)
*/
#define 状态机_对象到垃圾回收对象_宏名(v)	限制_检查_表达式_宏名((v)->类标_缩变量 >= 月头_月亮_T字符串_宏名, &(状态机_类型转换_u_宏名(v)->垃回_小写缩))


/* 实际上_变量 number of 总数_变量 bytes allocated */
#define 状态机_获取全部字节_宏名(g)	限制_类型转换_宏名(lu_mem, (g)->总共字节们_圆 + (g)->垃回债_缩)

配置_月亮I_函_宏名 void 月亮错误_设置债务_函 (全局_状态机结 *g, l_内存缩 负债_变量);
配置_月亮I_函_宏名 void 月亮错误_释放线程_函 (炉_状态机结 *L, 炉_状态机结 *L1);
配置_月亮I_函_宏名 调用信息_结 *月亮错误_扩展调用信息_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮错误_释放调用信息_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮错误_收缩调用信息_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮错误_检查C栈_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮错误_递增C栈_函 (炉_状态机结 *L);
配置_月亮I_函_宏名 void 月亮错误_警告_函 (炉_状态机结 *L, const char *消息_缩变量, int tocont);
配置_月亮I_函_宏名 void 月亮错误_警告错误_函 (炉_状态机结 *L, const char *哪儿_变量);
配置_月亮I_函_宏名 int 月亮错误_重置线程_函 (炉_状态机结 *L, int 状态码_变量);


#endif

