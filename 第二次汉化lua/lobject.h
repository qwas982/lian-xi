/*
** $Id: lobject.h $
** Type definitions for Lua objects
** See Copyright Notice in lua.h
*/


#ifndef lobject_h
#define lobject_h


#include <stdarg.h>


#include "llimits.h"
#include "lua.h"


/*
** Extra types for collectable non-values
*/
#define 对象_月亮_T上值_宏名	炉_类型数们  /* 上值们_小写 */
#define 对象_月亮_T原型_宏名	(炉_类型数们+1)  /* function prototypes */
#define 对象_月亮_T死键_宏名	(炉_类型数们+2)  /* removed keys in tables */



/*
** number of all possible types (including 月头_月亮_T没有_宏名 but excluding DEADKEY)
*/
#define 对象_月亮_总共类型们_宏名		(对象_月亮_T原型_宏名 + 2)


/*
** tags for Tagged Values have the following use of bits:
** bits 0-3: 实际上_变量 tag (a LUA_T* constant)
** bits 4-5: variant bits
** bit 6: whether 值_圆 is collectable
*/

/* add variant bits 到_变量 a type */
#define 对象_制作变体_宏名(t,v)	((t) | ((v) << 4))



/*
** Union of all Lua values
*/
typedef union 值_联 {
  struct 垃回对象_结 *垃回_小写缩;    /* collectable objects */
  void *p;         /* light userdata */
  炉_C函数半 f; /* light C functions */
  炉_整数型 i;   /* integer numbers */
  炉_数目型 n;    /* float numbers */
  /* not used, but may avoid warnings for uninitialized 值_圆 */
  路_字节型 ub;
} 值_联;


/*
** Tagged Values. This is the basic representation of values in Lua:
** an 实际上_变量 值_圆 plus a tag with its type.
*/

#define 标签值字段	值_联 value_; 路_字节型 tt_

typedef struct 标签值_结 {
  标签值字段;
} 标签值_结;


#define 对象_值__宏名(o)		((o)->value_)
#define 对象_原始值_宏名(o)	(对象_值__宏名(o))


/* raw type tag of a 标签值_结 */
#define 对象_原始tt_宏名(o)	((o)->tt_)

/* tag with no variants (bits 0-3) */
#define 对象_无变体_宏名(t)	((t) & 0x0F)

/* type tag of a 标签值_结 (bits 0-3 for tags + variant bits 4-5) */
#define 对象_带有变体_宏名(t)	((t) & 0x3F)
#define 对象_t类型标签_宏名(o)	对象_带有变体_宏名(对象_原始tt_宏名(o))

/* type of a 标签值_结 */
#define 对象_t类型_宏名(o)	(对象_无变体_宏名(对象_原始tt_宏名(o)))


/* Macros 到_变量 test type */
#define 对象_检查标签_宏名(o,t)		(对象_原始tt_宏名(o) == (t))
#define 对象_检查类型_宏名(o,t)		(对象_t类型_宏名(o) == (t))


/* Macros for internal tests */

/* collectable object has the same tag as the original 值_圆 */
#define 对象_正确tt_宏名(对象_变量)		(对象_t类型标签_宏名(对象_变量) == 对象_垃圾回收值_宏名(对象_变量)->类标_缩变量)

/*
** Any 值_圆 being manipulated by the program either is non
** collectable, or the collectable object has the 右_圆 tag
** and it is not dead. The 选项_变量 'L == NULL' allows other
** macros using this one 到_变量 be used 哪儿_变量 L is not available.
*/
#define 对象_检查存活性_宏名(L,对象_变量) \
	((void)L, 限制_月亮_长断言_宏名(!对象_是否可收集_宏名(对象_变量) || \
		(对象_正确tt_宏名(对象_变量) && (L == NULL || !垃圾回收_是否死亡_宏名(G(L),对象_垃圾回收值_宏名(对象_变量))))))


/* Macros 到_变量 set values */

/* set a 值_圆's tag */
#define 对象_设置tt__宏名(o,t)	((o)->tt_=(t))


/* main macro 到_变量 copy values (from 'obj2' 到_变量 'obj1') */
#define 对象_设置对象_宏名(L,obj1,obj2) \
	{ 标签值_结 *入出1_变量=(obj1); const 标签值_结 *入出2_变量=(obj2); \
          入出1_变量->value_ = 入出2_变量->value_; 对象_设置tt__宏名(入出1_变量, 入出2_变量->tt_); \
	  对象_检查存活性_宏名(L,入出1_变量); 限制_月亮_断言_宏名(!对象_是否无严格空值_宏名(入出1_变量)); }

/*
** Different types of assignments, according 到_变量 源_圆 and destination.
** (They are mostly equal now, but may be different in the future.)
*/

/* from 栈_圆小 到_变量 栈_圆小 */
#define 对象_设置对象s到s_宏名(L,操1_变量,操2_变量)	对象_设置对象_宏名(L,对象_s到v_宏名(操1_变量),对象_s到v_宏名(操2_变量))
/* 到_变量 栈_圆小 (not from same 栈_圆小) */
#define 对象_设置对象到s_宏名(L,操1_变量,操2_变量)	对象_设置对象_宏名(L,对象_s到v_宏名(操1_变量),操2_变量)
/* from table 到_变量 same table */
#define 对象_设置对象t到t_宏名	对象_设置对象_宏名
/* 到_变量 new object */
#define 对象_设置对象到n_宏名	对象_设置对象_宏名
/* 到_变量 table */
#define 对象_设置对象到t_宏名	对象_设置对象_宏名


/*
** Entries in a Lua 栈_圆小. Field '待关闭列表_结' forms a 列表_变量 of all
** 到_变量-be-closed variables active in this 栈_圆小. Dummy entries are
** used when the distance between two 待关闭_缩变量 variables does not fit
** in an unsigned short. They are represented by 德尔塔_变量==0, and
** their real 德尔塔_变量 is always the maximum 值_圆 that fits in
** that 月解析器_字段_函.
*/
typedef union 栈值_联 {
  标签值_结 值_变量;
  struct {
    标签值字段;
    unsigned short 德尔塔_变量;
  } 待关闭列表_结;
} 栈值_联;


/* index 到_变量 栈_圆小 elements */
typedef 栈值_联 *栈身份_型;


/*
** When reallocating the 栈_圆小, 改变_变量 all pointers 到_变量 the 栈_圆小 into
** proper offsets.
*/
typedef union {
  栈身份_型 p;  /* 实际上_变量 pointer */
  ptrdiff_t 偏移_变量;  /* used while the 栈_圆小 is being reallocated */
} 相对栈身份_联;


/* convert a '栈值_联' 到_变量 a '标签值_结' */
#define 对象_s到v_宏名(o)	(&(o)->值_变量)



/*
** {==================================================================
** Nil
** ===================================================================
*/

/* Standard nil */
#define 对象_月亮_V空值_宏名	对象_制作变体_宏名(月头_月亮_T空值_宏名, 0)

/* Empty 插槽_变量 (which might be different from a 插槽_变量 containing nil) */
#define 对象_月亮_V空容器_宏名	对象_制作变体_宏名(月头_月亮_T空值_宏名, 1)

/* 值_联 returned for a 键_小变量 not found in a table (absent 键_小变量) */
#define 对象_月亮_V抽象键_宏名	对象_制作变体_宏名(月头_月亮_T空值_宏名, 2)


/* macro 到_变量 test for (any 种类_变量 of) nil */
#define 对象_tt是否空值_宏名(v)		对象_检查类型_宏名((v), 月头_月亮_T空值_宏名)


/* macro 到_变量 test for a standard nil */
#define 对象_tt是否严格空值_宏名(o)	对象_检查标签_宏名((o), 对象_月亮_V空值_宏名)


#define 对象_设置空值的值_宏名(对象_变量) 对象_设置tt__宏名(对象_变量, 对象_月亮_V空值_宏名)


#define 对象_是否抽象键_宏名(v)		对象_检查标签_宏名((v), 对象_月亮_V抽象键_宏名)


/*
** macro 到_变量 detect non-standard nils (used only in assertions)
*/
#define 对象_是否无严格空值_宏名(v)	(对象_tt是否空值_宏名(v) && !对象_tt是否严格空值_宏名(v))


/*
** By default, entries with any 种类_变量 of nil are considered 空容器_变量.
** (In any definition, values associated with absent keys must also
** be accepted as 空容器_变量.)
*/
#define 对象_是否空容器_宏名(v)		对象_tt是否空值_宏名(v)


/* macro defining a 值_圆 corresponding 到_变量 an absent 键_小变量 */
#define 对象_抽象键常量_宏名		{NULL}, 对象_月亮_V抽象键_宏名


/* 记号_变量 an entry as 空容器_变量 */
#define 对象_设置空容器_宏名(v)		对象_设置tt__宏名(v, 对象_月亮_V空容器_宏名)



/* }================================================================== */


/*
** {==================================================================
** Booleans
** ===================================================================
*/


#define 对象_月亮_V假_宏名	对象_制作变体_宏名(月头_月亮_T布尔_宏名, 0)
#define 对象_月亮_V真_宏名	对象_制作变体_宏名(月头_月亮_T布尔_宏名, 1)

#define 对象_tt是否布尔_宏名(o)		对象_检查类型_宏名((o), 月头_月亮_T布尔_宏名)
#define 对象_tt是否假_宏名(o)		对象_检查标签_宏名((o), 对象_月亮_V假_宏名)
#define 对象_tt是否真_宏名(o)		对象_检查标签_宏名((o), 对象_月亮_V真_宏名)


#define 对象_l_是否假_宏名(o)	(对象_tt是否假_宏名(o) || 对象_tt是否空值_宏名(o))


#define 对象_设置布尔假值_宏名(对象_变量)		对象_设置tt__宏名(对象_变量, 对象_月亮_V假_宏名)
#define 对象_设置布尔真值_宏名(对象_变量)		对象_设置tt__宏名(对象_变量, 对象_月亮_V真_宏名)

/* }================================================================== */


/*
** {==================================================================
** Threads
** ===================================================================
*/

#define 对象_月亮_V线程_宏名		对象_制作变体_宏名(月头_月亮_T线程_宏名, 0)

#define 对象_tt是否线程_宏名(o)		对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_V线程_宏名))

#define 对象_线程值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否线程_宏名(o), 状态机_垃圾回收对象到th_宏名(对象_值__宏名(o).垃回_小写缩))

#define 对象_设置线程值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); 炉_状态机结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(对象_月亮_V线程_宏名)); \
    对象_检查存活性_宏名(L,入出_变量); }

#define 对象_设置线程值到s_宏名(L,o,t)	对象_设置线程值_宏名(L,对象_s到v_宏名(o),t)

/* }================================================================== */


/*
** {==================================================================
** Collectable Objects
** ===================================================================
*/

/*
** Common 头部_结 for all collectable objects (in macro 来自_变量, 到_变量 be
** included in other objects)
*/
#define 共用头_驼峰	struct 垃回对象_结 *下一个_变量; 路_字节型 类标_缩变量; 路_字节型 已记号_变量


/* Common type for all collectable objects */
typedef struct 垃回对象_结 {
  共用头_驼峰;
} 垃回对象_结;


/* Bit 记号_变量 for collectable types */
#define 对象_位_是否可收集_宏名	(1 << 6)

#define 对象_是否可收集_宏名(o)	(对象_原始tt_宏名(o) & 对象_位_是否可收集_宏名)

/* 记号_变量 a tag as collectable */
#define 对象_印刻到字节_宏名(t)			((t) | 对象_位_是否可收集_宏名)

#define 对象_垃圾回收值_宏名(o)	限制_检查_表达式_宏名(对象_是否可收集_宏名(o), 对象_值__宏名(o).垃回_小写缩)

#define 对象_垃圾回收原始值_宏名(v)	((v).垃回_小写缩)

#define 对象_设置垃圾回收o值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); 垃回对象_结 *信_全_变量=(x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 信_全_变量; 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(信_全_变量->类标_缩变量)); }

/* }================================================================== */


/*
** {==================================================================
** Numbers
** ===================================================================
*/

/* Variant tags for numbers */
#define 对象_月亮_V数目整型_宏名	对象_制作变体_宏名(月头_月亮_T数目_宏名, 0)  /* integer numbers */
#define 对象_月亮_V数目浮点_宏名	对象_制作变体_宏名(月头_月亮_T数目_宏名, 1)  /* float numbers */

#define 对象_tt是否数目_宏名(o)		对象_检查类型_宏名((o), 月头_月亮_T数目_宏名)
#define 对象_tt是否浮点_宏名(o)		对象_检查标签_宏名((o), 对象_月亮_V数目浮点_宏名)
#define 对象_tt是否整数_宏名(o)		对象_检查标签_宏名((o), 对象_月亮_V数目整型_宏名)

#define 对象_数目值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否数目_宏名(o), \
	(对象_tt是否整数_宏名(o) ? 限制_类型转换_数目_宏名(对象_整数值_宏名(o)) : 对象_浮点值_宏名(o)))
#define 对象_浮点值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否浮点_宏名(o), 对象_值__宏名(o).n)
#define 对象_整数值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否整数_宏名(o), 对象_值__宏名(o).i)

#define 对象_浮点原始值_宏名(v)	((v).n)
#define 对象_整数原始值_宏名(v)	((v).i)

#define 对象_设置浮点值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 对象_值__宏名(入出_变量).n=(x); 对象_设置tt__宏名(入出_变量, 对象_月亮_V数目浮点_宏名); }

#define 对象_改变浮点值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 限制_月亮_断言_宏名(对象_tt是否浮点_宏名(入出_变量)); 对象_值__宏名(入出_变量).n=(x); }

#define 对象_设置整数值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 对象_值__宏名(入出_变量).i=(x); 对象_设置tt__宏名(入出_变量, 对象_月亮_V数目整型_宏名); }

#define 对象_改变整数值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 限制_月亮_断言_宏名(对象_tt是否整数_宏名(入出_变量)); 对象_值__宏名(入出_变量).i=(x); }

/* }================================================================== */


/*
** {==================================================================
** Strings
** ===================================================================
*/

/* Variant tags for strings */
#define 对象_月亮_V短型串_宏名	对象_制作变体_宏名(月头_月亮_T字符串_宏名, 0)  /* short strings */
#define 对象_月亮_V长型串_宏名	对象_制作变体_宏名(月头_月亮_T字符串_宏名, 1)  /* long strings */

#define 对象_tt是否字符串_宏名(o)		对象_检查类型_宏名((o), 月头_月亮_T字符串_宏名)
#define 对象_tt是否短型字符串_宏名(o)	对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_V短型串_宏名))
#define 对象_tt是否长型字符串_宏名(o)	对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_V长型串_宏名))

#define 对象_ts原始值_宏名(v)	(状态机_垃圾回收对象到t串_宏名((v).垃回_小写缩))

#define 对象_ts值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否字符串_宏名(o), 状态机_垃圾回收对象到t串_宏名(对象_值__宏名(o).垃回_小写缩))

#define 对象_设置ts值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); 标签字符串_结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(x_->类标_缩变量)); \
    对象_检查存活性_宏名(L,入出_变量); }

/* set a string 到_变量 the 栈_圆小 */
#define 对象_设置ts值到s_宏名(L,o,s)	对象_设置ts值_宏名(L,对象_s到v_宏名(o),s)

/* set a string 到_变量 a new object */
#define 对象_设置ts值到n_宏名	对象_设置ts值_宏名


/*
** 头部_结 for a string 值_圆.
*/
typedef struct 标签字符串_结 {
  共用头_驼峰;
  路_字节型 额外_变量;  /* reserved words for short strings; "has 哈希_小写" for longs */
  路_字节型 短串长_小写;  /* length for short strings */
  unsigned int 哈希_小写;
  union {
    size_t 长串长_短;  /* length for long strings */
    struct 标签字符串_结 *哈希下个_变量;  /* linked 列表_变量 for 哈希_小写 table */
  } u;
  char 内容_小写[1];
} 标签字符串_结;



/*
** Get the 实际上_变量 string (数组_圆 of bytes) from a '标签字符串_结'.
*/
#define 对象_获取串_宏名(类s_变量)  ((类s_变量)->内容_小写)


/* get the 实际上_变量 string (数组_圆 of bytes) from a Lua 值_圆 */
#define 对象_s值_宏名(o)       对象_获取串_宏名(对象_ts值_宏名(o))

/* get string length from '标签字符串_结 *s' */
#define 对象_tss长度_宏名(s)	((s)->类标_缩变量 == 对象_月亮_V短型串_宏名 ? (s)->短串长_小写 : (s)->u.长串长_短)

/* get string length from '标签值_结 *o' */
#define 对象_vs长度_宏名(o)	对象_tss长度_宏名(对象_ts值_宏名(o))

/* }================================================================== */


/*
** {==================================================================
** Userdata
** ===================================================================
*/


/*
** Light userdata should be a variant of userdata, but for compatibility
** reasons they are also different types.
*/
#define 对象_月亮_V轻量用户数据_宏名	对象_制作变体_宏名(月头_月亮_T轻量用户数据_宏名, 0)

#define 对象_月亮_V用户数据_宏名		对象_制作变体_宏名(月头_月亮_T用户数据_宏名, 0)

#define 对象_tt是否轻量用户数据_宏名(o)	对象_检查标签_宏名((o), 对象_月亮_V轻量用户数据_宏名)
#define 对象_tt是否完全用户数据_宏名(o)	对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_V用户数据_宏名))

#define 对象_p值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否轻量用户数据_宏名(o), 对象_值__宏名(o).p)
#define 对象_u值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否完全用户数据_宏名(o), 状态机_垃圾回收对象到u_宏名(对象_值__宏名(o).垃回_小写缩))

#define 对象_p原始值_宏名(v)	((v).p)

#define 对象_设置p值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 对象_值__宏名(入出_变量).p=(x); 对象_设置tt__宏名(入出_变量, 对象_月亮_V轻量用户数据_宏名); }

#define 对象_设置u值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); 用户数据_结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(对象_月亮_V用户数据_宏名)); \
    对象_检查存活性_宏名(L,入出_变量); }


/* Ensures that addresses after this type are always fully aligned. */
typedef union 联值_联 {
  标签值_结 上值_缩变量;
  炉I_最大对齐;  /* ensures maximum alignment for udata bytes */
} 联值_联;


/*
** 头部_结 for userdata with user values;
** memory area follows the 终_变量 of this structure.
*/
typedef struct 用户数据_结 {
  共用头_驼峰;
  unsigned short 用户值数_缩;  /* number of user values */
  size_t 长度_短变量;  /* number of bytes */
  struct 表_结 *元表_小写;
  垃回对象_结 *垃回列表_缩;
  联值_联 上值_缩变量[1];  /* user values */
} 用户数据_结;


/*
** 头部_结 for userdata with no user values. These userdata do not need
** 到_变量 be 灰的色_圆 during GC, and therefore do not need a '垃回列表_缩' 月解析器_字段_函.
** To simplify, the 代码_变量 always use '用户数据_结' for both kinds of userdata,
** making sure it never accesses '垃回列表_缩' on userdata with no user values.
** This structure here is used only 到_变量 compute the correct 大小_变量 for
** this representation. (The '二元数据_联' 月解析器_字段_函 in its 终_变量 ensures correct
** alignment for binary 数据_变量 following this header.)
*/
typedef struct 用户数据0_结 {
  共用头_驼峰;
  unsigned short 用户值数_缩;  /* number of user values */
  size_t 长度_短变量;  /* number of bytes */
  struct 表_结 *元表_小写;
  union {炉I_最大对齐;} 二元数据_联;
} 用户数据0_结;


/* compute the 偏移_变量 of the memory area of a userdata */
#define 对象_用户数据内存偏移_宏名(nuv) \
	((nuv) == 0 ? offsetof(用户数据0_结, 二元数据_联)  \
                    : offsetof(用户数据_结, 上值_缩变量) + (sizeof(联值_联) * (nuv)))

/* get the address of the memory 月解析器_块_函 inside '用户数据_结' */
#define 对象_获取用户数据内存_宏名(u)	(限制_类型转换_印刻指针_宏名(u) + 对象_用户数据内存偏移_宏名((u)->用户值数_缩))

/* compute the 大小_变量 of a userdata */
#define 对象_用户数据大小_宏名(nuv,注意基_变量)	(对象_用户数据内存偏移_宏名(nuv) + (注意基_变量))

/* }================================================================== */


/*
** {==================================================================
** Prototypes
** ===================================================================
*/

#define 对象_月亮_V原型_宏名	对象_制作变体_宏名(对象_月亮_T原型_宏名, 0)


/*
** Description of an 上值_圆 for function prototypes
*/
typedef struct 上值描述_结 {
  标签字符串_结 *名称_变量;  /* 上值_圆 名称_变量 (for debug information) */
  路_字节型 栈内_小写;  /* whether it is in 栈_圆小 (register) */
  路_字节型 索引_缩变量;  /* index of 上值_圆 (in 栈_圆小 or in outer function's 列表_变量) */
  路_字节型 种类_变量;  /* 种类_变量 of corresponding variable */
} 上值描述_结;


/*
** Description of a local variable for function prototypes
** (used for debug information)
*/
typedef struct 本地变量_结 {
  标签字符串_结 *变量名称_变量;
  int 始程计_缩;  /* 首先_变量 针点_变量 哪儿_变量 variable is active */
  int 终程计_缩;    /* 首先_变量 针点_变量 哪儿_变量 variable is dead */
} 本地变量_结;


/*
** Associates the absolute 行_变量 源_圆 for a given instruction ('程序计数_变量').
** The 数组_圆 '行信息_变量' gives, for each instruction, the difference in
** lines from the 前一个_变量 instruction. When that difference does not
** fit into a byte, Lua saves the absolute 行_变量 for that instruction.
** (Lua also saves the absolute 行_变量 periodically, 到_变量 speed 上_小变量 the
** computation of a 行_变量 number: we can use binary search in the
** absolute-行_变量 数组_圆, but we must traverse the '行信息_变量' 数组_圆
** linearly 到_变量 compute a 行_变量.)
*/
typedef struct 绝对行信息_结 {
  int 程序计数_变量;
  int 行_变量;
} 绝对行信息_结;

/*
** Function Prototypes
*/
typedef struct 原型_结 {
  共用头_驼峰;
  路_字节型 形参数_小写;  /* number of fixed (named) parameters */
  路_字节型 是否_变量实参短;
  路_字节型 最大栈大小_小写;  /* number of registers 已需要_变量 by this function */
  int 上值大小_小写;  /* 大小_变量 of '上值们_小写' */
  int k大小_缩;  /* 大小_变量 of 'k' */
  int 代码大小_小写;
  int 行信息大小_小写;
  int p大小_缩;  /* 大小_变量 of 'p' */
  int 本地变量大小_短;
  int 绝对行信息大小_小写;  /* 大小_变量 of '绝对行信息_小写' */
  int 已定义行_小写;  /* debug information  */
  int 最后已定义行_小写;  /* debug information  */
  标签值_结 *k;  /* constants used by the function */
  Instruction *代码_变量;  /* opcodes */
  struct 原型_结 **p;  /* functions defined inside the function */
  上值描述_结 *上值们_小写;  /* 上值_圆 information */
  ls_byte *行信息_变量;  /* information about 源_圆 lines (debug information) */
  绝对行信息_结 *绝对行信息_小写;  /* idem */
  本地变量_结 *本地变量们_短;  /* information about local variables (debug information) */
  标签字符串_结  *源_圆;  /* used for debug information */
  垃回对象_结 *垃回列表_缩;
} 原型_结;

/* }================================================================== */


/*
** {==================================================================
** Functions
** ===================================================================
*/

#define 对象_月亮_V上值_宏名	对象_制作变体_宏名(对象_月亮_T上值_宏名, 0)


/* Variant tags for functions */
#define 对象_月亮_VL闭包L_宏名	对象_制作变体_宏名(月头_月亮_T函数_宏名, 0)  /* Lua closure */
#define 对象_月亮_VL闭包函_宏名	对象_制作变体_宏名(月头_月亮_T函数_宏名, 1)  /* light C function */
#define 对象_月亮_VC闭包L_宏名	对象_制作变体_宏名(月头_月亮_T函数_宏名, 2)  /* C closure */

#define 对象_tt是否函数_宏名(o)		对象_检查类型_宏名(o, 月头_月亮_T函数_宏名)
#define 对象_tt是否L闭包_宏名(o)		对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_VL闭包L_宏名))
#define 对象_tt是否L闭包函_宏名(o)		对象_检查标签_宏名((o), 对象_月亮_VL闭包函_宏名)
#define 对象_tt是否C闭包_宏名(o)		对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_VC闭包L_宏名))
#define 对象_tt是否闭包_宏名(o)         (对象_tt是否L闭包_宏名(o) || 对象_tt是否C闭包_宏名(o))


#define 对象_是否L函数_宏名(o)	对象_tt是否L闭包_宏名(o)

#define 对象_闭包值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否闭包_宏名(o), 状态机_垃圾回收对象到闭包_宏名(对象_值__宏名(o).垃回_小写缩))
#define 对象_闭包L值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否L闭包_宏名(o), 状态机_垃圾回收对象到l闭包_宏名(对象_值__宏名(o).垃回_小写缩))
#define 对象_函值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否L闭包函_宏名(o), 对象_值__宏名(o).f)
#define 对象_闭包C值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否C闭包_宏名(o), 状态机_垃圾回收对象到c闭包_宏名(对象_值__宏名(o).垃回_小写缩))

#define 对象_函原始值_宏名(v)	((v).f)

#define 对象_设置闭包L值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); L闭包_结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(对象_月亮_VL闭包L_宏名)); \
    对象_检查存活性_宏名(L,入出_变量); }

#define 对象_设置闭包L值到s_宏名(L,o,闭包_短变量)	对象_设置闭包L值_宏名(L,对象_s到v_宏名(o),闭包_短变量)

#define 对象_设置函值_宏名(对象_变量,x) \
  { 标签值_结 *入出_变量=(对象_变量); 对象_值__宏名(入出_变量).f=(x); 对象_设置tt__宏名(入出_变量, 对象_月亮_VL闭包函_宏名); }

#define 对象_设置闭包C值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); C闭包_结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(对象_月亮_VC闭包L_宏名)); \
    对象_检查存活性_宏名(L,入出_变量); }


/*
** Upvalues for Lua closures
*/
typedef struct 上值_结 {
  共用头_驼峰;
  union {
    标签值_结 *p;  /* points 到_变量 栈_圆小 or 到_变量 its own 值_圆 */
    ptrdiff_t 偏移_变量;  /* used while the 栈_圆小 is being reallocated */
  } v;
  union {
    struct {  /* (when 打开_圆) */
      struct 上值_结 *下一个_变量;  /* linked 列表_变量 */
      struct 上值_结 **前一个_变量;
    } 打开_圆;
    标签值_结 值_圆;  /* the 值_圆 (when closed) */
  } u;
} 上值_结;



#define 闭包头_驼峰 \
	共用头_驼峰; 路_字节型 nupvalues; 垃回对象_结 *垃回列表_缩

typedef struct C闭包_结 {
  闭包头_驼峰;
  炉_C函数半 f;
  标签值_结 上值_圆[1];  /* 列表_变量 of 上值们_小写 */
} C闭包_结;


typedef struct L闭包_结 {
  闭包头_驼峰;
  struct 原型_结 *p;
  上值_结 *上值们_短[1];  /* 列表_变量 of 上值们_小写 */
} L闭包_结;


typedef union 闭包_联 {
  C闭包_结 c;
  L闭包_结 l;
} 闭包_联;


#define 对象_获取原型_宏名(o)	(对象_闭包L值_宏名(o)->p)

/* }================================================================== */


/*
** {==================================================================
** Tables
** ===================================================================
*/

#define 对象_月亮_V表_宏名	对象_制作变体_宏名(月头_月亮_T表_宏名, 0)

#define 对象_tt是否表_宏名(o)		对象_检查标签_宏名((o), 对象_印刻到字节_宏名(对象_月亮_V表_宏名))

#define 对象_哈希值_宏名(o)	限制_检查_表达式_宏名(对象_tt是否表_宏名(o), 状态机_垃圾回收对象到t_宏名(对象_值__宏名(o).垃回_小写缩))

#define 对象_设置哈希值_宏名(L,对象_变量,x) \
  { 标签值_结 *入出_变量 = (对象_变量); 表_结 *x_ = (x); \
    对象_值__宏名(入出_变量).垃回_小写缩 = 状态机_对象到垃圾回收对象_宏名(x_); 对象_设置tt__宏名(入出_变量, 对象_印刻到字节_宏名(对象_月亮_V表_宏名)); \
    对象_检查存活性_宏名(L,入出_变量); }

#define 对象_设置哈希值到s_宏名(L,o,h)	对象_设置哈希值_宏名(L,对象_s到v_宏名(o),h)


/*
** Nodes for Hash tables: A pack of two 标签值_结's (键_小变量-值_圆 pairs)
** plus a '下一个_变量' 月解析器_字段_函 到_变量 link colliding entries. The distribution
** of the 键_小变量's fields ('键_tt缩' and '键_值短') not forming a proper
** '标签值_结' allows for a smaller 大小_变量 for '节点_联' both in 4-byte
** and 8-byte alignments.
*/
typedef union 节点_联 {
  struct 节点键_结 {
    标签值字段;  /* fields for 值_圆 */
    路_字节型 键_tt缩;  /* 键_小变量 type */
    int 下一个_变量;  /* for chaining */
    值_联 键_值短;  /* 键_小变量 值_圆 */
  } u;
  标签值_结 i_值缩;  /* direct access 到_变量 节点_变量's 值_圆 as a proper '标签值_结' */
} 节点_联;


/* copy a 值_圆 into a 键_小变量 */
#define 对象_设置节点键_宏名(L,节点_变量,对象_变量) \
	{ 节点_联 *n_=(节点_变量); const 标签值_结 *入出__变量=(对象_变量); \
	  n_->u.键_值短 = 入出__变量->value_; n_->u.键_tt缩 = 入出__变量->tt_; \
	  对象_检查存活性_宏名(L,入出__变量); }


/* copy a 值_圆 from a 键_小变量 */
#define 对象_获取节点键_宏名(L,对象_变量,节点_变量) \
	{ 标签值_结 *入出__变量=(对象_变量); const 节点_联 *n_=(节点_变量); \
	  入出__变量->value_ = n_->u.键_值短; 入出__变量->tt_ = n_->u.键_tt缩; \
	  对象_检查存活性_宏名(L,入出__变量); }


/*
** About '数组限制_缩': if '对象_是否真实数组大小_宏名(t)' is true, then '数组限制_缩' is the
** real 大小_变量 of '数组_圆'. Otherwise, the real 大小_变量 of '数组_圆' is the
** smallest power of two not smaller than '数组限制_缩' (or zero iff '数组限制_缩'
** is zero); '数组限制_缩' is then used as a hint for #t.
*/

#define 对象_位保留已分配密封_宏名		(1 << 7)
#define 对象_是否真实数组大小_宏名(t)		(!((t)->标志们_变量 & 对象_位保留已分配密封_宏名))
#define 对象_设置真实数组大小_宏名(t)		((t)->标志们_变量 &= 限制_类型转换_字节_宏名(~对象_位保留已分配密封_宏名))
#define 对象_设置无真实数组大小_宏名(t)	((t)->标志们_变量 |= 对象_位保留已分配密封_宏名)


typedef struct 表_结 {
  共用头_驼峰;
  路_字节型 标志们_变量;  /* 1<<p means tagmethod(p) is not present */
  路_字节型 长大小节点_变量;  /* log2 of 大小_变量 of '节点_变量' 数组_圆 */
  unsigned int 数组限制_缩;  /* "限制_变量" of '数组_圆' 数组_圆 */
  标签值_结 *数组_圆;  /* 数组_圆 part */
  节点_联 *节点_变量;
  节点_联 *最后自由_变量;  /* any free position is before this position */
  struct 表_结 *元表_小写;
  垃回对象_结 *垃回列表_缩;
} 表_结;


/*
** Macros 到_变量 manipulate keys inserted in nodes
*/
#define 对象_键tt_宏名(节点_变量)		((节点_变量)->u.键_tt缩)
#define 对象_键值_宏名(节点_变量)		((节点_变量)->u.键_值短)

#define 对象_键是否空值_宏名(节点_变量)		(对象_键tt_宏名(节点_变量) == 月头_月亮_T空值_宏名)
#define 对象_键是否整数_宏名(节点_变量)	(对象_键tt_宏名(节点_变量) == 对象_月亮_V数目整型_宏名)
#define 对象_键整数值_宏名(节点_变量)		(对象_键值_宏名(节点_变量).i)
#define 对象_键是否短型串_宏名(节点_变量)	(对象_键tt_宏名(节点_变量) == 对象_印刻到字节_宏名(对象_月亮_V短型串_宏名))
#define 对象_键串值_宏名(节点_变量)		(状态机_垃圾回收对象到t串_宏名(对象_键值_宏名(节点_变量).垃回_小写缩))

#define 对象_设置空值键_宏名(节点_变量)		(对象_键tt_宏名(节点_变量) = 月头_月亮_T空值_宏名)

#define 对象_键是否可回收_宏名(n)	(对象_键tt_宏名(n) & 对象_位_是否可收集_宏名)

#define 对象_垃圾回收键_宏名(n)	(对象_键值_宏名(n).垃回_小写缩)
#define 对象_垃圾回收键N_宏名(n)	(对象_键是否可回收_宏名(n) ? 对象_垃圾回收键_宏名(n) : NULL)


/*
** Dead keys in tables have the tag DEADKEY but keep their original
** 对象_垃圾回收值_宏名. This distinguishes them from regular keys but allows them 到_变量
** be found when searched in a special way. ('下一个_变量' needs that 到_变量 find
** keys removed from a table during a traversal.)
*/
#define 对象_设置死键_宏名(节点_变量)	(对象_键tt_宏名(节点_变量) = 对象_月亮_T死键_宏名)
#define 对象_键是否死_宏名(节点_变量)		(对象_键tt_宏名(节点_变量) == 对象_月亮_T死键_宏名)

/* }================================================================== */



/*
** 'module' operation for hashing (大小_变量 is always a power of 2)
*/
#define 对象_月取模_宏名(s,大小_变量) \
	(限制_检查_表达式_宏名((大小_变量&(大小_变量-1))==0, (限制_类型转换_整型_宏名((s) & ((大小_变量)-1)))))


#define 对象_二到N次方_宏名(x)	(1<<(x))
#define 对象_节点大小_宏名(t)	(对象_二到N次方_宏名((t)->长大小节点_变量))


/* 大小_变量 of 缓冲区_变量 for '月亮对象_utf8转义_函' function */
#define 对象_UTF8缓冲大小_宏名	8

配置_月亮I_函_宏名 int 月亮对象_utf8转义_函 (char *缓冲_变量, unsigned long x);
配置_月亮I_函_宏名 int 月亮对象_向上取整对数2_函 (unsigned int x);
配置_月亮I_函_宏名 int 月亮对象_原始算术_函 (炉_状态机结 *L, int 操作_短变量, const 标签值_结 *p1,
                             const 标签值_结 *p2, 标签值_结 *结果_短变量);
配置_月亮I_函_宏名 void 月亮对象_算术_函 (炉_状态机结 *L, int 操作_短变量, const 标签值_结 *p1,
                           const 标签值_结 *p2, 栈身份_型 结果_短变量);
配置_月亮I_函_宏名 size_t 月亮对象_字符串到数目_函 (const char *s, 标签值_结 *o);
配置_月亮I_函_宏名 int 月亮对象_十六进制值_函 (int c);
配置_月亮I_函_宏名 void 月亮对象_到字符串_函 (炉_状态机结 *L, 标签值_结 *对象_变量);
配置_月亮I_函_宏名 const char *月亮对象_推入可变格式化字符串_函 (炉_状态机结 *L, const char *格式_短变量,
                                                       va_list argp);
配置_月亮I_函_宏名 const char *月亮对象_推入格式化字符串_函 (炉_状态机结 *L, const char *格式_短变量, ...);
配置_月亮I_函_宏名 void 月亮对象_大块id_函 (char *out, const char *源_圆, size_t 源的长_短);


#endif

