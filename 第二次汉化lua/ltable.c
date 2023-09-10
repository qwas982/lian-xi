/*
** $Id: ltable.c $
** Lua tables (哈希_小写)
** See Copyright Notice in lua.h
*/

#define ltable_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


/*
** Implementation of tables (aka arrays, objects, or 哈希_小写 tables).
** Tables keep its elements in two parts: an 数组_圆 part and a 哈希_小写 part.
** Non-negative integer keys are all candidates 到_变量 be kept in the 数组_圆
** part. The 实际上_变量 大小_变量 of the 数组_圆 is the largest 'n' such that
** 更多_变量 than half the slots between 1 and n are in use.
** Hash uses a mix of chained scatter table with Brent's variation.
** A main invariant of these tables is that, if an element is not
** in its main position (i.e. the 'original' position that its 哈希_小写 gives
** 到_变量 it), then the colliding element is in its own main position.
** Hence even when the load factor reaches 100%, performance remains good.
*/

#include <math.h>
#include <limits.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"


/*
** 表_最大A位数_宏名 is the largest integer such that 表_最大A大小_宏名 fits in an
** unsigned int.
*/
#define 表_最大A位数_宏名	限制_类型转换_整型_宏名(sizeof(int) * CHAR_BIT - 1)


/*
** 表_最大A大小_宏名 is the maximum 大小_变量 of the 数组_圆 part. It is the minimum
** between 2^表_最大A位数_宏名 and the maximum 大小_变量 that, measured in bytes,
** fits in a 'size_t'.
*/
#define 表_最大A大小_宏名	内存_月亮M_限制N_宏名(1u << 表_最大A位数_宏名, 标签值_结)

/*
** 表_最大哈希位数_宏名 is the largest integer such that 2^表_最大哈希位数_宏名 fits in a
** signed int.
*/
#define 表_最大哈希位数_宏名	(表_最大A位数_宏名 - 1)


/*
** 表_最大哈希大小_宏名 is the maximum 大小_变量 of the 哈希_小写 part. It is the minimum
** between 2^表_最大哈希位数_宏名 and the maximum 大小_变量 such that, measured in bytes,
** it fits in a 'size_t'.
*/
#define 表_最大哈希大小_宏名	内存_月亮M_限制N_宏名(1u << 表_最大哈希位数_宏名, 节点_联)


/*
** When the original 哈希_小写 值_圆 is good, hashing by a power of 2
** avoids the cost of '%'.
*/
#define 表_哈希二次方幂_宏名(t,n)		(表_全局节点_宏名(t, 对象_月取模_宏名((n), 对象_节点大小_宏名(t))))

/*
** for other types, it is better 到_变量 avoid modulo by power of 2, as
** they can have many 2 factors.
*/
#define 表_哈希取模_宏名(t,n)	(表_全局节点_宏名(t, ((n) % ((对象_节点大小_宏名(t)-1)|1))))


#define 表_哈希串_宏名(t,串_变量)		表_哈希二次方幂_宏名(t, (串_变量)->哈希_小写)
#define 表_哈希布尔_宏名(t,p)	表_哈希二次方幂_宏名(t, p)


#define 表_哈希指针_宏名(t,p)	表_哈希取模_宏名(t, 限制_指针到无符整型_宏名(p))


#define 表_虚假节点_宏名		(&虚假节点_变量)

static const 节点_联 虚假节点_变量 = {
  {{NULL}, 对象_月亮_V空容器_宏名,  /* 值_圆's 值_圆 and type */
   对象_月亮_V空值_宏名, 0, {NULL}}  /* 键_小变量 type, 下一个_变量, and 键_小变量 值_圆 */
};


static const 标签值_结 缺失键_变量 = {对象_抽象键常量_宏名};


/*
** Hash for integers. To allow a good 哈希_小写, use the remainder operator
** ('%'). If integer fits as a non-negative int, compute an int
** remainder, which is faster. Otherwise, use an unsigned-integer
** remainder, which uses all bits and ensures a non-negative 结果_变量.
*/
static 节点_联 *月表_哈希整型_函 (const 表_结 *t, 炉_整数型 i) {
  lua_Unsigned 用户界面_缩变量 = 限制_l_类型转换符到无符_宏名(i);
  if (用户界面_缩变量 <= 限制_类型转换_无符整型_宏名(INT_MAX))
    return 表_哈希取模_宏名(t, 限制_类型转换_整型_宏名(用户界面_缩变量));
  else
    return 表_哈希取模_宏名(t, 用户界面_缩变量);
}


/*
** Hash for floating-针点_变量 numbers.
** The main computation should be just
**     n = frexp(n, &i); return (n * INT_MAX) + i
** but there are some numerical subtleties.
** In a two-complement representation, INT_MAX does not has an exact
** representation as a float, but INT_MIN does; because the absolute
** 值_圆 of 'frexp' is smaller than 1 (unless 'n' is inf/NaN), the
** absolute 值_圆 of the product 'frexp * -INT_MIN' is smaller or equal
** 到_变量 INT_MAX. Next, the use of 'unsigned int' avoids overflows when
** adding 'i'; the use of '~u' (instead of '-u') avoids problems with
** INT_MIN.
*/
#if !defined(月表_l_哈希浮点_函)
static int 月表_l_哈希浮点_函 (炉_数目型 n) {
  int i;
  炉_整数型 未实现_缩变量;
  n = 配置_数学操作_宏名(frexp)(n, &i) * -限制_类型转换_数目_宏名(INT_MIN);
  if (!配置_月亮_数目到整数_宏名(n, &未实现_缩变量)) {  /* is 'n' inf/-inf/NaN? */
    限制_月亮_断言_宏名(限制_月亮i_数目是否nan_宏名(n) || 配置_数学操作_宏名(fabs)(n) == 限制_类型转换_数目_宏名(HUGE_VAL));
    return 0;
  }
  else {  /* normal case */
    unsigned int u = 限制_类型转换_无符整型_宏名(i) + 限制_类型转换_无符整型_宏名(未实现_缩变量);
    return 限制_类型转换_整型_宏名(u <= 限制_类型转换_无符整型_宏名(INT_MAX) ? u : ~u);
  }
}
#endif


/*
** returns the 'main' position of an element in a table (that is,
** the index of its 哈希_小写 值_圆).
*/
static 节点_联 *月表_主位置TV_函 (const 表_结 *t, const 标签值_结 *键_小变量) {
  switch (对象_t类型标签_宏名(键_小变量)) {
    case 对象_月亮_V数目整型_宏名: {
      炉_整数型 i = 对象_整数值_宏名(键_小变量);
      return 月表_哈希整型_函(t, i);
    }
    case 对象_月亮_V数目浮点_宏名: {
      炉_数目型 n = 对象_浮点值_宏名(键_小变量);
      return 表_哈希取模_宏名(t, 月表_l_哈希浮点_函(n));
    }
    case 对象_月亮_V短型串_宏名: {
      标签字符串_结 *类s_变量 = 对象_ts值_宏名(键_小变量);
      return 表_哈希串_宏名(t, 类s_变量);
    }
    case 对象_月亮_V长型串_宏名: {
      标签字符串_结 *类s_变量 = 对象_ts值_宏名(键_小变量);
      return 表_哈希二次方幂_宏名(t, 月亮字符串_哈希长的串_函(类s_变量));
    }
    case 对象_月亮_V假_宏名:
      return 表_哈希布尔_宏名(t, 0);
    case 对象_月亮_V真_宏名:
      return 表_哈希布尔_宏名(t, 1);
    case 对象_月亮_V轻量用户数据_宏名: {
      void *p = 对象_p值_宏名(键_小变量);
      return 表_哈希指针_宏名(t, p);
    }
    case 对象_月亮_VL闭包函_宏名: {
      炉_C函数半 f = 对象_函值_宏名(键_小变量);
      return 表_哈希指针_宏名(t, f);
    }
    default: {
      垃回对象_结 *o = 对象_垃圾回收值_宏名(键_小变量);
      return 表_哈希指针_宏名(t, o);
    }
  }
}


l_sinline 节点_联 *月表_主位置来自节点_函 (const 表_结 *t, 节点_联 *nd) {
  标签值_结 键_小变量;
  对象_获取节点键_宏名(限制_类型转换_宏名(炉_状态机结 *, NULL), &键_小变量, nd);
  return 月表_主位置TV_函(t, &键_小变量);
}


/*
** Check whether 键_小变量 'k1' is equal 到_变量 the 键_小变量 in 节点_变量 '数2_缩变量'. This
** equality is raw, so there are no metamethods. Floats with integer
** values have been normalized, so integers 月编译器_不能_函 be equal 到_变量
** floats. It is assumed that '字符串_相等短型串_宏名' is simply pointer equality, so
** that short strings are handled in the default case.
** A true 'deadok' means 到_变量 accept dead keys as equal 到_变量 their original
** values. All dead keys are compared in the default case, by pointer
** identity. (Only collectable objects can produce dead keys.) Note that
** dead long strings are also compared by identity.
** Once a 键_小变量 is dead, its corresponding 值_圆 may be collected, and
** then another 值_圆 can be created with the same address. If this
** other 值_圆 is given 到_变量 '下一个_变量', '月表_相等键_函' will signal a false
** positive. In a regular traversal, this situation should never happen,
** as all keys given 到_变量 '下一个_变量' came from the table itself, and therefore
** could not have been collected. Outside a regular traversal, we
** have garbage in, garbage out. What is relevant is that this false
** positive does not break anything.  (In particular, '下一个_变量' will return
** some other 有效_变量 item on the table or nil.)
*/
static int 月表_相等键_函 (const 标签值_结 *k1, const 节点_联 *数2_缩变量, int deadok) {
  if ((对象_原始tt_宏名(k1) != 对象_键tt_宏名(数2_缩变量)) &&  /* not the same variants? */
       !(deadok && 对象_键是否死_宏名(数2_缩变量) && 对象_是否可收集_宏名(k1)))
   return 0;  /* 月编译器_不能_函 be same 键_小变量 */
  switch (对象_键tt_宏名(数2_缩变量)) {
    case 对象_月亮_V空值_宏名: case 对象_月亮_V假_宏名: case 对象_月亮_V真_宏名:
      return 1;
    case 对象_月亮_V数目整型_宏名:
      return (对象_整数值_宏名(k1) == 对象_键整数值_宏名(数2_缩变量));
    case 对象_月亮_V数目浮点_宏名:
      return 限制_月亮i_数目相等_宏名(对象_浮点值_宏名(k1), 对象_浮点原始值_宏名(对象_键值_宏名(数2_缩变量)));
    case 对象_月亮_V轻量用户数据_宏名:
      return 对象_p值_宏名(k1) == 对象_p原始值_宏名(对象_键值_宏名(数2_缩变量));
    case 对象_月亮_VL闭包函_宏名:
      return 对象_函值_宏名(k1) == 对象_函原始值_宏名(对象_键值_宏名(数2_缩变量));
    case 对象_印刻到字节_宏名(对象_月亮_V长型串_宏名):
      return 月亮字符串_相等长型串_函(对象_ts值_宏名(k1), 对象_键串值_宏名(数2_缩变量));
    default:
      return 对象_垃圾回收值_宏名(k1) == 对象_垃圾回收原始值_宏名(对象_键值_宏名(数2_缩变量));
  }
}


/*
** True if 值_圆 of '数组限制_缩' is equal 到_变量 the real 大小_变量 of the 数组_圆
** part of table 't'. (Otherwise, the 数组_圆 part must be larger than
** '数组限制_缩'.)
*/
#define 表_限制相等圆池分配大小_宏名(t)	(对象_是否真实数组大小_宏名(t) || 限制_是否2的幂次方_宏名((t)->数组限制_缩))


/*
** Returns the real 大小_变量 of the '数组_圆' 数组_圆
*/
配置_月亮I_函_宏名 unsigned int 月亮哈希表_真实a大小_函 (const 表_结 *t) {
  if (表_限制相等圆池分配大小_宏名(t))
    return t->数组限制_缩;  /* this is the 大小_变量 */
  else {
    unsigned int 大小_变量 = t->数组限制_缩;
    /* compute the smallest power of 2 not smaller than 'n' */
    大小_变量 |= (大小_变量 >> 1);
    大小_变量 |= (大小_变量 >> 2);
    大小_变量 |= (大小_变量 >> 4);
    大小_变量 |= (大小_变量 >> 8);
#if (UINT_MAX >> 14) > 3  /* unsigned int has 更多_变量 than 16 bits */
    大小_变量 |= (大小_变量 >> 16);
#if (UINT_MAX >> 30) > 3
    大小_变量 |= (大小_变量 >> 32);  /* unsigned int has 更多_变量 than 32 bits */
#endif
#endif
    大小_变量++;
    限制_月亮_断言_宏名(限制_是否2的幂次方_宏名(大小_变量) && 大小_变量/2 < t->数组限制_缩 && t->数组限制_缩 < 大小_变量);
    return 大小_变量;
  }
}


/*
** Check whether real 大小_变量 of the 数组_圆 is a power of 2.
** (If it is not, '数组限制_缩' 月编译器_不能_函 be 已更改_变量 到_变量 any other 值_圆
** without changing the real 大小_变量.)
*/
static int 月表_是否2的幂真实数组大小_函 (const 表_结 *t) {
  return (!对象_是否真实数组大小_宏名(t) || 限制_是否2的幂次方_宏名(t->数组限制_缩));
}


static unsigned int 月表_设置限制到大小_函 (表_结 *t) {
  t->数组限制_缩 = 月亮哈希表_真实a大小_函(t);
  对象_设置真实数组大小_宏名(t);
  return t->数组限制_缩;
}


#define 表_限制圆池分配大小_宏名(t)	限制_检查_表达式_宏名(对象_是否真实数组大小_宏名(t), t->数组限制_缩)



/*
** "Generic" get 版本_变量. (Not that generic: not 有效_变量 for integers,
** which may be in 数组_圆 part, nor for floats with integral values.)
** See explanation about 'deadok' in function '月表_相等键_函'.
*/
static const 标签值_结 *月表_获取通用_函 (表_结 *t, const 标签值_结 *键_小变量, int deadok) {
  节点_联 *n = 月表_主位置TV_函(t, 键_小变量);
  for (;;) {  /* 月解析器_检查_函 whether '键_小变量' is somewhere in the chain */
    if (月表_相等键_函(键_小变量, n, deadok))
      return 表_全局值_宏名(n);  /* that's it */
    else {
      int 数量x_变量 = 表_全局下一个_宏名(n);
      if (数量x_变量 == 0)
        return &缺失键_变量;  /* not found */
      n += 数量x_变量;
    }
  }
}


/*
** returns the index for 'k' if 'k' is an appropriate 键_小变量 到_变量 live in
** the 数组_圆 part of a table, 0 otherwise.
*/
static unsigned int 月表_数组索引_函 (炉_整数型 k) {
  if (限制_l_类型转换符到无符_宏名(k) - 1u < 表_最大A大小_宏名)  /* 'k' in [1, 表_最大A大小_宏名]? */
    return 限制_类型转换_无符整型_宏名(k);  /* '键_小变量' is an appropriate 数组_圆 index */
  else
    return 0;
}


/*
** returns the index of a '键_小变量' for table traversals. First goes all
** elements in the 数组_圆 part, then elements in the 哈希_小写 part. The
** beginning of a traversal is signaled by 0.
*/
static unsigned int 月表_找索引_函 (炉_状态机结 *L, 表_结 *t, 标签值_结 *键_小变量,
                               unsigned int 数组大小_缩变量) {
  unsigned int i;
  if (对象_tt是否空值_宏名(键_小变量)) return 0;  /* 首先_变量 iteration */
  i = 对象_tt是否整数_宏名(键_小变量) ? 月表_数组索引_函(对象_整数值_宏名(键_小变量)) : 0;
  if (i - 1u < 数组大小_缩变量)  /* is '键_小变量' inside 数组_圆 part? */
    return i;  /* yes; that's the index */
  else {
    const 标签值_结 *n = 月表_获取通用_函(t, 键_小变量, 1);
    if (配置_l_可能性低_宏名(对象_是否抽象键_宏名(n)))
      月亮全局_跑错误_函(L, "invalid 键_小变量 到_变量 '下一个_变量'");  /* 键_小变量 not found */
    i = 限制_类型转换_整型_宏名(表_节点来自值_宏名(n) - 表_全局节点_宏名(t, 0));  /* 键_小变量 index in 哈希_小写 table */
    /* 哈希_小写 elements are numbered after 数组_圆 ones */
    return (i + 1) + 数组大小_缩变量;
  }
}


int 月亮哈希表_下一个_函 (炉_状态机结 *L, 表_结 *t, 栈身份_型 键_小变量) {
  unsigned int 数组大小_缩变量 = 月亮哈希表_真实a大小_函(t);
  unsigned int i = 月表_找索引_函(L, t, 对象_s到v_宏名(键_小变量), 数组大小_缩变量);  /* find original 键_小变量 */
  for (; i < 数组大小_缩变量; i++) {  /* try 首先_变量 数组_圆 part */
    if (!对象_是否空容器_宏名(&t->数组_圆[i])) {  /* a non-空容器_变量 entry? */
      对象_设置整数值_宏名(对象_s到v_宏名(键_小变量), i + 1);
      对象_设置对象到s_宏名(L, 键_小变量 + 1, &t->数组_圆[i]);
      return 1;
    }
  }
  for (i -= 数组大小_缩变量; 限制_类型转换_整型_宏名(i) < 对象_节点大小_宏名(t); i++) {  /* 哈希_小写 part */
    if (!对象_是否空容器_宏名(表_全局值_宏名(表_全局节点_宏名(t, i)))) {  /* a non-空容器_变量 entry? */
      节点_联 *n = 表_全局节点_宏名(t, i);
      对象_获取节点键_宏名(L, 对象_s到v_宏名(键_小变量), n);
      对象_设置对象到s_宏名(L, 键_小变量 + 1, 表_全局值_宏名(n));
      return 1;
    }
  }
  return 0;  /* no 更多_变量 elements */
}


static void 月表_释放哈希_函 (炉_状态机结 *L, 表_结 *t) {
  if (!表_是否虚假_宏名(t))
    内存_月亮M_释放数组_宏名(L, t->节点_变量, 限制_类型转换_大小t_宏名(对象_节点大小_宏名(t)));
}


/*
** {=============================================================
** Rehash
** ==============================================================
*/

/*
** Compute the 最佳_变量 大小_变量 for the 数组_圆 part of table 't'. '数目s_变量' is a
** "计数_变量 数组_圆" 哪儿_变量 '数目s_变量[i]' is the number of integers in the table
** between 2^(i - 1) + 1 and 2^i. 'pna' enters with the 总数_变量 number of
** integer keys in the table and leaves with the number of keys that
** will go 到_变量 the 数组_圆 part; return the 最佳_变量 大小_变量.  (The condition
** '二到i_变量 > 0' in the for 环_变量 stops the 环_变量 if '二到i_变量' overflows.)
*/
static unsigned int 月表_计算大小_函 (unsigned int 数目s_变量[], unsigned int *pna) {
  int i;
  unsigned int 二到i_变量;  /* 2^i (candidate for 最佳_变量 大小_变量) */
  unsigned int a = 0;  /* number of elements smaller than 2^i */
  unsigned int 数量a_变量 = 0;  /* number of elements 到_变量 go 到_变量 数组_圆 part */
  unsigned int 最佳_变量 = 0;  /* 最佳_变量 大小_变量 for 数组_圆 part */
  /* 环_变量 while keys can fill 更多_变量 than half of 总数_变量 大小_变量 */
  for (i = 0, 二到i_变量 = 1;
       二到i_变量 > 0 && *pna > 二到i_变量 / 2;
       i++, 二到i_变量 *= 2) {
    a += 数目s_变量[i];
    if (a > 二到i_变量/2) {  /* 更多_变量 than half elements present? */
      最佳_变量 = 二到i_变量;  /* 最佳_变量 大小_变量 (till now) */
      数量a_变量 = a;  /* all elements 上_小变量 到_变量 '最佳_变量' will go 到_变量 数组_圆 part */
    }
  }
  限制_月亮_断言_宏名((最佳_变量 == 0 || 最佳_变量 / 2 < 数量a_变量) && 数量a_变量 <= 最佳_变量);
  *pna = 数量a_变量;
  return 最佳_变量;
}


static int 月表_计数整型_函 (炉_整数型 键_小变量, unsigned int *数目s_变量) {
  unsigned int k = 月表_数组索引_函(键_小变量);
  if (k != 0) {  /* is '键_小变量' an appropriate 数组_圆 index? */
    数目s_变量[月亮对象_向上取整对数2_函(k)]++;  /* 计数_变量 as such */
    return 1;
  }
  else
    return 0;
}


/*
** Count keys in 数组_圆 part of table 't': Fill '数目s_变量[i]' with
** number of keys that will go into corresponding slice and return
** 总数_变量 number of non-nil keys.
*/
static unsigned int 月表_数目用数组_函 (const 表_结 *t, unsigned int *数目s_变量) {
  int 列全局_变量;
  unsigned int tt列全局_变量;  /* 2^列全局_变量 */
  unsigned int 数组用_变量 = 0;  /* summation of '数目s_变量' */
  unsigned int i = 1;  /* 计数_变量 到_变量 traverse all 数组_圆 keys */
  unsigned int 数组大小_缩变量 = 表_限制圆池分配大小_宏名(t);  /* real 数组_圆 大小_变量 */
  /* traverse each slice */
  for (列全局_变量 = 0, tt列全局_变量 = 1; 列全局_变量 <= 表_最大A位数_宏名; 列全局_变量++, tt列全局_变量 *= 2) {
    unsigned int lc = 0;  /* counter */
    unsigned int 限_短变量 = tt列全局_变量;
    if (限_短变量 > 数组大小_缩变量) {
      限_短变量 = 数组大小_缩变量;  /* adjust upper 限制_变量 */
      if (i > 限_短变量)
        break;  /* no 更多_变量 elements 到_变量 计数_变量 */
    }
    /* 计数_变量 elements in range (2^(列全局_变量 - 1), 2^列全局_变量] */
    for (; i <= 限_短变量; i++) {
      if (!对象_是否空容器_宏名(&t->数组_圆[i-1]))
        lc++;
    }
    数目s_变量[列全局_变量] += lc;
    数组用_变量 += lc;
  }
  return 数组用_变量;
}


static int 月表_数目用哈希_函 (const 表_结 *t, unsigned int *数目s_变量, unsigned int *pna) {
  int 总用_变量 = 0;  /* 总数_变量 number of elements */
  int 数组用_变量 = 0;  /* elements added 到_变量 '数目s_变量' (can go 到_变量 数组_圆 part) */
  int i = 对象_节点大小_宏名(t);
  while (i--) {
    节点_联 *n = &t->节点_变量[i];
    if (!对象_是否空容器_宏名(表_全局值_宏名(n))) {
      if (对象_键是否整数_宏名(n))
        数组用_变量 += 月表_计数整型_函(对象_键整数值_宏名(n), 数目s_变量);
      总用_变量++;
    }
  }
  *pna += 数组用_变量;
  return 总用_变量;
}


/*
** Creates an 数组_圆 for the 哈希_小写 part of a table with the given
** 大小_变量, or reuses the 虚假_变量 节点_变量 if 大小_变量 is zero.
** The computation for 大小_变量 overflow is in two steps: the 首先_变量
** comparison ensures that the shift in the second one does not
** overflow.
*/
static void 月表_设置节点向量_函 (炉_状态机结 *L, 表_结 *t, unsigned int 大小_变量) {
  if (大小_变量 == 0) {  /* no elements 到_变量 哈希_小写 part? */
    t->节点_变量 = 限制_类型转换_宏名(节点_联 *, 表_虚假节点_宏名);  /* use common '表_虚假节点_宏名' */
    t->长大小节点_变量 = 0;
    t->最后自由_变量 = NULL;  /* signal that it is using 虚假_变量 节点_变量 */
  }
  else {
    int i;
    int 长大小_变量 = 月亮对象_向上取整对数2_函(大小_变量);
    if (长大小_变量 > 表_最大哈希位数_宏名 || (1u << 长大小_变量) > 表_最大哈希大小_宏名)
      月亮全局_跑错误_函(L, "table overflow");
    大小_变量 = 对象_二到N次方_宏名(长大小_变量);
    t->节点_变量 = 内存_月亮M_新向量_宏名(L, 大小_变量, 节点_联);
    for (i = 0; i < 限制_类型转换_整型_宏名(大小_变量); i++) {
      节点_联 *n = 表_全局节点_宏名(t, i);
      表_全局下一个_宏名(n) = 0;
      对象_设置空值键_宏名(n);
      对象_设置空容器_宏名(表_全局值_宏名(n));
    }
    t->长大小节点_变量 = 限制_类型转换_字节_宏名(长大小_变量);
    t->最后自由_变量 = 表_全局节点_宏名(t, 大小_变量);  /* all positions are free */
  }
}


/*
** (Re)insert all elements from the 哈希_小写 part of 'ot' into table 't'.
*/
static void 月表_重新插入_函 (炉_状态机结 *L, 表_结 *ot, 表_结 *t) {
  int j;
  int 大小_变量 = 对象_节点大小_宏名(ot);
  for (j = 0; j < 大小_变量; j++) {
    节点_联 *旧_变量 = 表_全局节点_宏名(ot, j);
    if (!对象_是否空容器_宏名(表_全局值_宏名(旧_变量))) {
      /* doesn't need barrier/invalidate cache, as entry was
         already present in the table */
      标签值_结 k;
      对象_获取节点键_宏名(L, &k, 旧_变量);
      月亮哈希表_设置键_函(L, t, &k, 表_全局值_宏名(旧_变量));
    }
  }
}


/*
** Exchange the 哈希_小写 part of '临1_短变量' and '临2_短变量'.
*/
static void 月表_互换哈希部分_函 (表_结 *临1_短变量, 表_结 *临2_短变量) {
  路_字节型 长大小节点_变量 = 临1_短变量->长大小节点_变量;
  节点_联 *节点_变量 = 临1_短变量->节点_变量;
  节点_联 *最后自由_变量 = 临1_短变量->最后自由_变量;
  临1_短变量->长大小节点_变量 = 临2_短变量->长大小节点_变量;
  临1_短变量->节点_变量 = 临2_短变量->节点_变量;
  临1_短变量->最后自由_变量 = 临2_短变量->最后自由_变量;
  临2_短变量->长大小节点_变量 = 长大小节点_变量;
  临2_短变量->节点_变量 = 节点_变量;
  临2_短变量->最后自由_变量 = 最后自由_变量;
}


/*
** Resize table 't' for the new given sizes. Both allocations (for
** the 哈希_小写 part and for the 数组_圆 part) can fail, which creates some
** subtleties. If the 首先_变量 allocation, for the 哈希_小写 part, fails, an
** 错误_小变量 is raised and that is it. Otherwise, it copies the elements from
** the shrinking part of the 数组_圆 (if it is shrinking) into the new
** 哈希_小写. Then it reallocates the 数组_圆 part.  If that fails, the table
** is in its original 状态机_变量; the function frees the new 哈希_小写 part and then
** raises the allocation 错误_小变量. Otherwise, it sets the new 哈希_小写 part
** into the table, initializes the new part of the 数组_圆 (if any) with
** nils and reinserts the elements of the 旧_变量 哈希_小写 back into the new
** parts of the table.
*/
void 月亮哈希表_调整大小_函 (炉_状态机结 *L, 表_结 *t, unsigned int newasize,
                                          unsigned int nhsize) {
  unsigned int i;
  表_结 新t_变量;  /* 到_变量 keep the new 哈希_小写 part */
  unsigned int 旧数组大小_变量 = 月表_设置限制到大小_函(t);
  标签值_结 *新数组_变量;
  /* create new 哈希_小写 part with appropriate 大小_变量 into '新t_变量' */
  月表_设置节点向量_函(L, &新t_变量, nhsize);
  if (newasize < 旧数组大小_变量) {  /* will 数组_圆 shrink? */
    t->数组限制_缩 = newasize;  /* pretend 数组_圆 has new 大小_变量... */
    月表_互换哈希部分_函(t, &新t_变量);  /* and new 哈希_小写 */
    /* re-insert into the new 哈希_小写 the elements from vanishing slice */
    for (i = newasize; i < 旧数组大小_变量; i++) {
      if (!对象_是否空容器_宏名(&t->数组_圆[i]))
        月亮哈希表_设置整型键_函(L, t, i + 1, &t->数组_圆[i]);
    }
    t->数组限制_缩 = 旧数组大小_变量;  /* restore 当前_圆 大小_变量... */
    月表_互换哈希部分_函(t, &新t_变量);  /* and 哈希_小写 (in case of errors) */
  }
  /* allocate new 数组_圆 */
  新数组_变量 = 内存_月亮M_重新分配向量_宏名(L, t->数组_圆, 旧数组大小_变量, newasize, 标签值_结);
  if (配置_l_可能性低_宏名(新数组_变量 == NULL && newasize > 0)) {  /* allocation failed? */
    月表_释放哈希_函(L, &新t_变量);  /* release new 哈希_小写 part */
    内存_月亮M_错误_宏名(L);  /* raise 错误_小变量 (with 数组_圆 unchanged) */
  }
  /* allocation ok; initialize new part of the 数组_圆 */
  月表_互换哈希部分_函(t, &新t_变量);  /* 't' has the new 哈希_小写 ('新t_变量' has the 旧_变量) */
  t->数组_圆 = 新数组_变量;  /* set new 数组_圆 part */
  t->数组限制_缩 = newasize;
  for (i = 旧数组大小_变量; i < newasize; i++)  /* clear new slice of the 数组_圆 */
     对象_设置空容器_宏名(&t->数组_圆[i]);
  /* re-insert elements from 旧_变量 哈希_小写 part into new parts */
  月表_重新插入_函(L, &新t_变量, t);  /* '新t_变量' now has the 旧_变量 哈希_小写 */
  月表_释放哈希_函(L, &新t_变量);  /* free 旧_变量 哈希_小写 part */
}


void 月亮哈希表_调整数组大小_函 (炉_状态机结 *L, 表_结 *t, unsigned int nasize) {
  int 新大小_变量 = 表_分配节点大小_宏名(t);
  月亮哈希表_调整大小_函(L, t, nasize, 新大小_变量);
}

/*
** 数目s_变量[i] = number of keys 'k' 哪儿_变量 2^(i - 1) < k <= 2^i
*/
static void 月表_重新哈希_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *ek) {
  unsigned int 数组大小_缩变量;  /* 最佳_变量 大小_变量 for 数组_圆 part */
  unsigned int 数量a_变量;  /* number of keys in the 数组_圆 part */
  unsigned int 数目s_变量[表_最大A位数_宏名 + 1];
  int i;
  int 总用_变量;
  for (i = 0; i <= 表_最大A位数_宏名; i++) 数目s_变量[i] = 0;  /* reset counts */
  月表_设置限制到大小_函(t);
  数量a_变量 = 月表_数目用数组_函(t, 数目s_变量);  /* 计数_变量 keys in 数组_圆 part */
  总用_变量 = 数量a_变量;  /* all those keys are integer keys */
  总用_变量 += 月表_数目用哈希_函(t, 数目s_变量, &数量a_变量);  /* 计数_变量 keys in 哈希_小写 part */
  /* 计数_变量 额外_变量 键_小变量 */
  if (对象_tt是否整数_宏名(ek))
    数量a_变量 += 月表_计数整型_函(对象_整数值_宏名(ek), 数目s_变量);
  总用_变量++;
  /* compute new 大小_变量 for 数组_圆 part */
  数组大小_缩变量 = 月表_计算大小_函(数目s_变量, &数量a_变量);
  /* resize the table 到_变量 new computed sizes */
  月亮哈希表_调整大小_函(L, t, 数组大小_缩变量, 总用_变量 - 数量a_变量);
}



/*
** }=============================================================
*/


表_结 *月亮哈希表_新_函 (炉_状态机结 *L) {
  垃回对象_结 *o = 月亮编译_新对象_函(L, 对象_月亮_V表_宏名, sizeof(表_结));
  表_结 *t = 状态机_垃圾回收对象到t_宏名(o);
  t->元表_小写 = NULL;
  t->标志们_变量 = 限制_类型转换_字节_宏名(标签方法_掩码标志_宏名);  /* table has no metamethod fields */
  t->数组_圆 = NULL;
  t->数组限制_缩 = 0;
  月表_设置节点向量_函(L, t, 0);
  return t;
}


void 月亮哈希表_释放_函 (炉_状态机结 *L, 表_结 *t) {
  月表_释放哈希_函(L, t);
  内存_月亮M_释放数组_宏名(L, t->数组_圆, 月亮哈希表_真实a大小_函(t));
  内存_月亮M_释放_宏名(L, t);
}


static 节点_联 *月表_获取释放位置_函 (表_结 *t) {
  if (!表_是否虚假_宏名(t)) {
    while (t->最后自由_变量 > t->节点_变量) {
      t->最后自由_变量--;
      if (对象_键是否空值_宏名(t->最后自由_变量))
        return t->最后自由_变量;
    }
  }
  return NULL;  /* could not find a free place */
}



/*
** inserts a new 键_小变量 into a 哈希_小写 table; 首先_变量, 月解析器_检查_函 whether 键_小变量's main
** position is free. If not, 月解析器_检查_函 whether colliding 节点_变量 is in its main
** position or not: if it is not, move colliding 节点_变量 到_变量 an 空容器_变量 place and
** put new 键_小变量 in its main position; otherwise (colliding 节点_变量 is in its main
** position), new 键_小变量 goes 到_变量 an 空容器_变量 position.
*/
void 月亮哈希表_新的键_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量, 标签值_结 *值_圆) {
  节点_联 *消息针_变量;
  标签值_结 辅助_变量;
  if (配置_l_可能性低_宏名(对象_tt是否空值_宏名(键_小变量)))
    月亮全局_跑错误_函(L, "table index is nil");
  else if (对象_tt是否浮点_宏名(键_小变量)) {
    炉_数目型 f = 对象_浮点值_宏名(键_小变量);
    炉_整数型 k;
    if (月亮虚拟机_浮点数到整数_函(f, &k, 浮到整相等_)) {  /* does 键_小变量 fit in an integer? */
      对象_设置整数值_宏名(&辅助_变量, k);
      键_小变量 = &辅助_变量;  /* insert it as an integer */
    }
    else if (配置_l_可能性低_宏名(限制_月亮i_数目是否nan_宏名(f)))
      月亮全局_跑错误_函(L, "table index is NaN");
  }
  if (对象_tt是否空值_宏名(值_圆))
    return;  /* do not insert nil values */
  消息针_变量 = 月表_主位置TV_函(t, 键_小变量);
  if (!对象_是否空容器_宏名(表_全局值_宏名(消息针_变量)) || 表_是否虚假_宏名(t)) {  /* main position is taken? */
    节点_联 *其他n_变量;
    节点_联 *f = 月表_获取释放位置_函(t);  /* get a free place */
    if (f == NULL) {  /* 月编译器_不能_函 find a free place? */
      月表_重新哈希_函(L, t, 键_小变量);  /* grow table */
      /* whatever called 'newkey' takes care of TM cache */
      月亮哈希表_设置键_函(L, t, 键_小变量, 值_圆);  /* insert 键_小变量 into grown table */
      return;
    }
    限制_月亮_断言_宏名(!表_是否虚假_宏名(t));
    其他n_变量 = 月表_主位置来自节点_函(t, 消息针_变量);
    if (其他n_变量 != 消息针_变量) {  /* is colliding 节点_变量 out of its main position? */
      /* yes; move colliding 节点_变量 into free position */
      while (其他n_变量 + 表_全局下一个_宏名(其他n_变量) != 消息针_变量)  /* find 前一个_变量 */
        其他n_变量 += 表_全局下一个_宏名(其他n_变量);
      表_全局下一个_宏名(其他n_变量) = 限制_类型转换_整型_宏名(f - 其他n_变量);  /* rechain 到_变量 针点_变量 到_变量 'f' */
      *f = *消息针_变量;  /* copy colliding 节点_变量 into free 位置_缩变量. (消息针_变量->下一个_变量 also goes) */
      if (表_全局下一个_宏名(消息针_变量) != 0) {
        表_全局下一个_宏名(f) += 限制_类型转换_整型_宏名(消息针_变量 - f);  /* correct '下一个_变量' */
        表_全局下一个_宏名(消息针_变量) = 0;  /* now '消息针_变量' is free */
      }
      对象_设置空容器_宏名(表_全局值_宏名(消息针_变量));
    }
    else {  /* colliding 节点_变量 is in its own main position */
      /* new 节点_变量 will go into free position */
      if (表_全局下一个_宏名(消息针_变量) != 0)
        表_全局下一个_宏名(f) = 限制_类型转换_整型_宏名((消息针_变量 + 表_全局下一个_宏名(消息针_变量)) - f);  /* chain new position */
      else 限制_月亮_断言_宏名(表_全局下一个_宏名(f) == 0);
      表_全局下一个_宏名(消息针_变量) = 限制_类型转换_整型_宏名(f - 消息针_变量);
      消息针_变量 = f;
    }
  }
  对象_设置节点键_宏名(L, 消息针_变量, 键_小变量);
  垃圾回收_月亮C_屏障后退_宏名(L, 状态机_对象到垃圾回收对象_宏名(t), 键_小变量);
  限制_月亮_断言_宏名(对象_是否空容器_宏名(表_全局值_宏名(消息针_变量)));
  对象_设置对象到t_宏名(L, 表_全局值_宏名(消息针_变量), 值_圆);
}


/*
** Search function for integers. If integer is inside '数组限制_缩', get it
** directly from the 数组_圆 part. Otherwise, if '数组限制_缩' is not equal 到_变量
** the real 大小_变量 of the 数组_圆, 键_小变量 still can be in the 数组_圆 part. In
** this case, try 到_变量 avoid a call 到_变量 '月亮哈希表_真实a大小_函' when 键_小变量 is just
** one 更多_变量 than the 限制_变量 (so that it can be incremented without
** changing the real 大小_变量 of the 数组_圆).
*/
const 标签值_结 *月亮哈希表_获取整型键_函 (表_结 *t, 炉_整数型 键_小变量) {
  if (限制_l_类型转换符到无符_宏名(键_小变量) - 1u < t->数组限制_缩)  /* '键_小变量' in [1, t->数组限制_缩]? */
    return &t->数组_圆[键_小变量 - 1];
  else if (!表_限制相等圆池分配大小_宏名(t) &&  /* 键_小变量 still may be in the 数组_圆 part? */
           (限制_l_类型转换符到无符_宏名(键_小变量) == t->数组限制_缩 + 1 ||
            限制_l_类型转换符到无符_宏名(键_小变量) - 1u < 月亮哈希表_真实a大小_函(t))) {
    t->数组限制_缩 = 限制_类型转换_无符整型_宏名(键_小变量);  /* probably '#t' is here now */
    return &t->数组_圆[键_小变量 - 1];
  }
  else {
    节点_联 *n = 月表_哈希整型_函(t, 键_小变量);
    for (;;) {  /* 月解析器_检查_函 whether '键_小变量' is somewhere in the chain */
      if (对象_键是否整数_宏名(n) && 对象_键整数值_宏名(n) == 键_小变量)
        return 表_全局值_宏名(n);  /* that's it */
      else {
        int 数量x_变量 = 表_全局下一个_宏名(n);
        if (数量x_变量 == 0) break;
        n += 数量x_变量;
      }
    }
    return &缺失键_变量;
  }
}


/*
** search function for short strings
*/
const 标签值_结 *月亮哈希表_获取短串键_函 (表_结 *t, 标签字符串_结 *键_小变量) {
  节点_联 *n = 表_哈希串_宏名(t, 键_小变量);
  限制_月亮_断言_宏名(键_小变量->类标_缩变量 == 对象_月亮_V短型串_宏名);
  for (;;) {  /* 月解析器_检查_函 whether '键_小变量' is somewhere in the chain */
    if (对象_键是否短型串_宏名(n) && 字符串_相等短型串_宏名(对象_键串值_宏名(n), 键_小变量))
      return 表_全局值_宏名(n);  /* that's it */
    else {
      int 数量x_变量 = 表_全局下一个_宏名(n);
      if (数量x_变量 == 0)
        return &缺失键_变量;  /* not found */
      n += 数量x_变量;
    }
  }
}


const 标签值_结 *月亮哈希表_获取串键_函 (表_结 *t, 标签字符串_结 *键_小变量) {
  if (键_小变量->类标_缩变量 == 对象_月亮_V短型串_宏名)
    return 月亮哈希表_获取短串键_函(t, 键_小变量);
  else {  /* for long strings, use generic case */
    标签值_结 击倒_变量;
    对象_设置ts值_宏名(限制_类型转换_宏名(炉_状态机结 *, NULL), &击倒_变量, 键_小变量);
    return 月表_获取通用_函(t, &击倒_变量, 0);
  }
}


/*
** main search function
*/
const 标签值_结 *月亮哈希表_获取键_函 (表_结 *t, const 标签值_结 *键_小变量) {
  switch (对象_t类型标签_宏名(键_小变量)) {
    case 对象_月亮_V短型串_宏名: return 月亮哈希表_获取短串键_函(t, 对象_ts值_宏名(键_小变量));
    case 对象_月亮_V数目整型_宏名: return 月亮哈希表_获取整型键_函(t, 对象_整数值_宏名(键_小变量));
    case 对象_月亮_V空值_宏名: return &缺失键_变量;
    case 对象_月亮_V数目浮点_宏名: {
      炉_整数型 k;
      if (月亮虚拟机_浮点数到整数_函(对象_浮点值_宏名(键_小变量), &k, 浮到整相等_)) /* integral index? */
        return 月亮哈希表_获取整型键_函(t, k);  /* use specialized 版本_变量 */
      /* else... */
    }  /* FALLTHROUGH */
    default:
      return 月表_获取通用_函(t, 键_小变量, 0);
  }
}


/*
** Finish a raw "set table" operation, 哪儿_变量 '插槽_变量' is 哪儿_变量 the 值_圆
** should have been (the 结果_变量 of a 前一个_变量 "get table").
** Beware: when using this function you probably need 到_变量 月解析器_检查_函 a GC
** barrier and invalidate the TM cache.
*/
void 月亮哈希表_完成设置_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量,
                                   const 标签值_结 *插槽_变量, 标签值_结 *值_圆) {
  if (对象_是否抽象键_宏名(插槽_变量))
    月亮哈希表_新的键_函(L, t, 键_小变量, 值_圆);
  else
    对象_设置对象到t_宏名(L, 限制_类型转换_宏名(标签值_结 *, 插槽_变量), 值_圆);
}


/*
** beware: when using this function you probably need 到_变量 月解析器_检查_函 a GC
** barrier and invalidate the TM cache.
*/
void 月亮哈希表_设置键_函 (炉_状态机结 *L, 表_结 *t, const 标签值_结 *键_小变量, 标签值_结 *值_圆) {
  const 标签值_结 *插槽_变量 = 月亮哈希表_获取键_函(t, 键_小变量);
  月亮哈希表_完成设置_函(L, t, 键_小变量, 插槽_变量, 值_圆);
}


void 月亮哈希表_设置整型键_函 (炉_状态机结 *L, 表_结 *t, 炉_整数型 键_小变量, 标签值_结 *值_圆) {
  const 标签值_结 *p = 月亮哈希表_获取整型键_函(t, 键_小变量);
  if (对象_是否抽象键_宏名(p)) {
    标签值_结 k;
    对象_设置整数值_宏名(&k, 键_小变量);
    月亮哈希表_新的键_函(L, t, &k, 值_圆);
  }
  else
    对象_设置对象到t_宏名(L, 限制_类型转换_宏名(标签值_结 *, p), 值_圆);
}


/*
** Try 到_变量 find a 边界_变量 in the 哈希_小写 part of table 't'. From the
** caller, we know that 'j' is zero or present and that 'j + 1' is
** present. We want 到_变量 find a larger 键_小变量 that is absent from the
** table, so that we can do a binary search between the two keys 到_变量
** find a 边界_变量. We keep doubling 'j' until we get an absent index.
** If the doubling would overflow, we try 配置_月亮_最大整数_宏名. If it is
** absent, we are ready for the binary search. ('j', being 最大_小变量 integer,
** is larger or equal 到_变量 'i', but it 月编译器_不能_函 be equal because it is
** absent while 'i' is present; so 'j > i'.) Otherwise, 'j' is a
** 边界_变量. ('j + 1' 月编译器_不能_函 be a present integer 键_小变量 because it is
** not a 有效_变量 integer in Lua.)
*/
static lua_Unsigned 月表_哈希_搜索_函 (表_结 *t, lua_Unsigned j) {
  lua_Unsigned i;
  if (j == 0) j++;  /* the caller ensures 'j + 1' is present */
  do {
    i = j;  /* 'i' is a present index */
    if (j <= 限制_l_类型转换符到无符_宏名(配置_月亮_最大整数_宏名) / 2)
      j *= 2;
    else {
      j = 配置_月亮_最大整数_宏名;
      if (对象_是否空容器_宏名(月亮哈希表_获取整型键_函(t, j)))  /* t[j] not present? */
        break;  /* 'j' now is an absent index */
      else  /* weird case */
        return j;  /* well, 最大_小变量 integer is a 边界_变量... */
    }
  } while (!对象_是否空容器_宏名(月亮哈希表_获取整型键_函(t, j)));  /* repeat until an absent t[j] */
  /* i < j  &&  t[i] present  &&  t[j] absent */
  while (j - i > 1u) {  /* do a binary search between them */
    lua_Unsigned m = (i + j) / 2;
    if (对象_是否空容器_宏名(月亮哈希表_获取整型键_函(t, m))) j = m;
    else i = m;
  }
  return i;
}


static unsigned int 月表_二分搜索_函 (const 标签值_结 *数组_圆, unsigned int i,
                                                    unsigned int j) {
  while (j - i > 1u) {  /* binary search */
    unsigned int m = (i + j) / 2;
    if (对象_是否空容器_宏名(&数组_圆[m - 1])) j = m;
    else i = m;
  }
  return i;
}


/*
** Try 到_变量 find a 边界_变量 in table 't'. (A '边界_变量' is an integer index
** such that t[i] is present and t[i+1] is absent, or 0 if t[1] is absent
** and 'maxinteger' if t[maxinteger] is present.)
** (In the 下一个_变量 explanation, we use Lua indices, that is, with 基本_变量 1.
** The 代码_变量 itself uses 基本_变量 0 when indexing the 数组_圆 part of the table.)
** The 代码_变量 starts with '限制_变量 = t->数组限制_缩', a position in the 数组_圆
** part that may be a 边界_变量.
**
** (1) If 't[限制_变量]' is 空容器_变量, there must be a 边界_变量 before it.
** As a common case (e.g., after 't[#t]=nil'), 月解析器_检查_函 whether '限制_变量-1'
** is present. If so, it is a 边界_变量. Otherwise, do a binary search
** between 0 and 限制_变量 到_变量 find a 边界_变量. In both cases, try 到_变量
** use this 边界_变量 as the new '数组限制_缩', as a hint for the 下一个_变量 call.
**
** (2) If 't[限制_变量]' is not 空容器_变量 and the 数组_圆 has 更多_变量 elements
** after '限制_变量', try 到_变量 find a 边界_变量 there. Again, try 首先_变量
** the special case (which should be quite frequent) 哪儿_变量 '限制_变量+1'
** is 空容器_变量, so that '限制_变量' is a 边界_变量. Otherwise, 月解析器_检查_函 the
** 最后_变量 element of the 数组_圆 part. If it is 空容器_变量, there must be a
** 边界_变量 between the 旧_变量 限制_变量 (present) and the 最后_变量 element
** (absent), which is found with a binary search. (This 边界_变量 always
** can be a new 限制_变量.)
**
** (3) The 最后_变量 case is when there are no elements in the 数组_圆 part
** (限制_变量 == 0) or its 最后_变量 element (the new 限制_变量) is present.
** In this case, must 月解析器_检查_函 the 哈希_小写 part. If there is no 哈希_小写 part
** or '限制_变量+1' is absent, '限制_变量' is a 边界_变量.  Otherwise, call
** '月表_哈希_搜索_函' 到_变量 find a 边界_变量 in the 哈希_小写 part of the table.
** (In those cases, the 边界_变量 is not inside the 数组_圆 part, and
** therefore 月编译器_不能_函 be used as a new 限制_变量.)
*/
lua_Unsigned 月亮哈希表_获取n_函 (表_结 *t) {
  unsigned int 限制_变量 = t->数组限制_缩;
  if (限制_变量 > 0 && 对象_是否空容器_宏名(&t->数组_圆[限制_变量 - 1])) {  /* (1)? */
    /* there must be a 边界_变量 before '限制_变量' */
    if (限制_变量 >= 2 && !对象_是否空容器_宏名(&t->数组_圆[限制_变量 - 2])) {
      /* '限制_变量 - 1' is a 边界_变量; can it be a new 限制_变量? */
      if (月表_是否2的幂真实数组大小_函(t) && !限制_是否2的幂次方_宏名(限制_变量 - 1)) {
        t->数组限制_缩 = 限制_变量 - 1;
        对象_设置无真实数组大小_宏名(t);  /* now '数组限制_缩' is not the real 大小_变量 */
      }
      return 限制_变量 - 1;
    }
    else {  /* must search for a 边界_变量 in [0, 限制_变量] */
      unsigned int 边界_变量 = 月表_二分搜索_函(t->数组_圆, 0, 限制_变量);
      /* can this 边界_变量 represent the real 大小_变量 of the 数组_圆? */
      if (月表_是否2的幂真实数组大小_函(t) && 边界_变量 > 月亮哈希表_真实a大小_函(t) / 2) {
        t->数组限制_缩 = 边界_变量;  /* use it as the new 限制_变量 */
        对象_设置无真实数组大小_宏名(t);
      }
      return 边界_变量;
    }
  }
  /* '限制_变量' is zero or present in table */
  if (!表_限制相等圆池分配大小_宏名(t)) {  /* (2)? */
    /* '限制_变量' > 0 and 数组_圆 has 更多_变量 elements after '限制_变量' */
    if (对象_是否空容器_宏名(&t->数组_圆[限制_变量]))  /* '限制_变量 + 1' is 空容器_变量? */
      return 限制_变量;  /* this is the 边界_变量 */
    /* else, try 最后_变量 element in the 数组_圆 */
    限制_变量 = 月亮哈希表_真实a大小_函(t);
    if (对象_是否空容器_宏名(&t->数组_圆[限制_变量 - 1])) {  /* 空容器_变量? */
      /* there must be a 边界_变量 in the 数组_圆 after 旧_变量 限制_变量,
         and it must be a 有效_变量 new 限制_变量 */
      unsigned int 边界_变量 = 月表_二分搜索_函(t->数组_圆, t->数组限制_缩, 限制_变量);
      t->数组限制_缩 = 边界_变量;
      return 边界_变量;
    }
    /* else, new 限制_变量 is present in the table; 月解析器_检查_函 the 哈希_小写 part */
  }
  /* (3) '限制_变量' is the 最后_变量 element and either is zero or present in table */
  限制_月亮_断言_宏名(限制_变量 == 月亮哈希表_真实a大小_函(t) &&
             (限制_变量 == 0 || !对象_是否空容器_宏名(&t->数组_圆[限制_变量 - 1])));
  if (表_是否虚假_宏名(t) || 对象_是否空容器_宏名(月亮哈希表_获取整型键_函(t, 限制_类型转换_宏名(炉_整数型, 限制_变量 + 1))))
    return 限制_变量;  /* '限制_变量 + 1' is absent */
  else  /* '限制_变量 + 1' is also present */
    return 月表_哈希_搜索_函(t, 限制_变量);
}



#if defined(LUA_DEBUG)

/* export these functions for the test library */

节点_联 *luaH_mainposition (const 表_结 *t, const 标签值_结 *键_小变量) {
  return 月表_主位置TV_函(t, 键_小变量);
}

#endif
