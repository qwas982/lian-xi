/*
** $Id: ltablib.c $
** Library for 表_结 Manipulation
** See Copyright Notice in lua.h
*/

#define ltablib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <limits.h>
#include <stddef.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/*
** Operations that an object must define 到_变量 mimic a table
** (some functions only need some of them)
*/
#define 表_表_读_宏名	1			/* read */
#define 表_表_写_宏名	2			/* write */
#define 表_表_长度_宏名	4			/* length */
#define 表_表_读写_宏名	(表_表_读_宏名 | 表_表_写_宏名)		/* read/write */


#define 表_辅助_获取n_宏名(L,n,w)	(月表库_检查表_函(L, n, (w) | 表_表_长度_宏名), 月亮状态_长度_函(L, n))


static int 月表库_检查字段_函 (炉_状态机结 *L, const char *键_小变量, int n) {
  月亮推入字符串_函(L, 键_小变量);
  return (月亮获取原始_函(L, -n) != 月头_月亮_T空值_宏名);
}


/*
** Check that '实参_短变量' either is a table or can behave like one (that is,
** has a 元表_小写 with the required metamethods)
*/
static void 月表库_检查表_函 (炉_状态机结 *L, int 实参_短变量, int 什么_变量) {
  if (月亮_类型_函(L, 实参_短变量) != 月头_月亮_T表_宏名) {  /* is it not a table? */
    int n = 1;  /* number of elements 到_变量 pop */
    if (月亮获取元表_函(L, 实参_短变量) &&  /* must have 元表_小写 */
        (!(什么_变量 & 表_表_读_宏名) || 月表库_检查字段_函(L, "__index", ++n)) &&
        (!(什么_变量 & 表_表_写_宏名) || 月表库_检查字段_函(L, "__newindex", ++n)) &&
        (!(什么_变量 & 表_表_长度_宏名) || 月表库_检查字段_函(L, "__len", ++n))) {
      月头_月亮_弹出_宏名(L, n);  /* pop 元表_小写 and tested metamethods */
    }
    else
      月亮状态_检查类型_函(L, 实参_短变量, 月头_月亮_T表_宏名);  /* force an 错误_小变量 */
  }
}


static int 月表库_t插入_函 (炉_状态机结 *L) {
  炉_整数型 位置_缩变量;  /* 哪儿_变量 到_变量 insert new element */
  炉_整数型 e = 表_辅助_获取n_宏名(L, 1, 表_表_读写_宏名);
  e = 辅助库_月亮l_整型操作_宏名(+, e, 1);  /* 首先_变量 空容器_变量 element */
  switch (月亮_获取顶_函(L)) {
    case 2: {  /* called with only 2 arguments */
      位置_缩变量 = e;  /* insert new element at the 终_变量 */
      break;
    }
    case 3: {
      炉_整数型 i;
      位置_缩变量 = 月亮状态_检查整数_函(L, 2);  /* 2nd argument is the position */
      /* 月解析器_检查_函 whether '位置_缩变量' is in [1, e] */
      辅助库_月亮l_实参检查_宏名(L, (lua_Unsigned)位置_缩变量 - 1u < (lua_Unsigned)e, 2,
                       "position out of bounds");
      for (i = e; i > 位置_缩变量; i--) {  /* move 上_小变量 elements */
        月亮获取索引_函(L, 1, i - 1);
        月亮_设置索引_函(L, 1, i);  /* t[i] = t[i - 1] */
      }
      break;
    }
    default: {
      return 月亮状态_错误_函(L, "wrong number of arguments 到_变量 'insert'");
    }
  }
  月亮_设置索引_函(L, 1, 位置_缩变量);  /* t[位置_缩变量] = v */
  return 0;
}


static int 月表库_t移除_函 (炉_状态机结 *L) {
  炉_整数型 大小_变量 = 表_辅助_获取n_宏名(L, 1, 表_表_读写_宏名);
  炉_整数型 位置_缩变量 = 月亮状态_可选整数_函(L, 2, 大小_变量);
  if (位置_缩变量 != 大小_变量)  /* validate '位置_缩变量' if given */
    /* 月解析器_检查_函 whether '位置_缩变量' is in [1, 大小_变量 + 1] */
    辅助库_月亮l_实参检查_宏名(L, (lua_Unsigned)位置_缩变量 - 1u <= (lua_Unsigned)大小_变量, 2,
                     "position out of bounds");
  月亮获取索引_函(L, 1, 位置_缩变量);  /* 结果_变量 = t[位置_缩变量] */
  for ( ; 位置_缩变量 < 大小_变量; 位置_缩变量++) {
    月亮获取索引_函(L, 1, 位置_缩变量 + 1);
    月亮_设置索引_函(L, 1, 位置_缩变量);  /* t[位置_缩变量] = t[位置_缩变量 + 1] */
  }
  月亮推入空值_函(L);
  月亮_设置索引_函(L, 1, 位置_缩变量);  /* remove entry t[位置_缩变量] */
  return 1;
}


/*
** Copy elements (1[f], ..., 1[e]) into (类标_缩变量[t], 类标_缩变量[t+1], ...). Whenever
** possible, copy in increasing order, which is better for rehashing.
** "possible" means destination after original range, or smaller
** than origin, or copying 到_变量 another table.
*/
static int 月表库_t移动_函 (炉_状态机结 *L) {
  炉_整数型 f = 月亮状态_检查整数_函(L, 2);
  炉_整数型 e = 月亮状态_检查整数_函(L, 3);
  炉_整数型 t = 月亮状态_检查整数_函(L, 4);
  int 类标_缩变量 = !月头_月亮_是否没有或空值_宏名(L, 5) ? 5 : 1;  /* destination table */
  月表库_检查表_函(L, 1, 表_表_读_宏名);
  月表库_检查表_函(L, 类标_缩变量, 表_表_写_宏名);
  if (e >= f) {  /* otherwise, nothing 到_变量 move */
    炉_整数型 n, i;
    辅助库_月亮l_实参检查_宏名(L, f > 0 || e < 配置_月亮_最大整数_宏名 + f, 3,
                  "too many elements 到_变量 move");
    n = e - f + 1;  /* number of elements 到_变量 move */
    辅助库_月亮l_实参检查_宏名(L, t <= 配置_月亮_最大整数_宏名 - n + 1, 4,
                  "destination wrap around");
    if (t > e || t <= f || (类标_缩变量 != 1 && !月亮对比_函(L, 1, 类标_缩变量, 月头_月亮_操作等于_宏名))) {
      for (i = 0; i < n; i++) {
        月亮获取索引_函(L, 1, f + i);
        月亮_设置索引_函(L, 类标_缩变量, t + i);
      }
    }
    else {
      for (i = n - 1; i >= 0; i--) {
        月亮获取索引_函(L, 1, f + i);
        月亮_设置索引_函(L, 类标_缩变量, t + i);
      }
    }
  }
  月亮_推入值_函(L, 类标_缩变量);  /* return destination table */
  return 1;
}


static void 月表库_添加字段_函 (炉_状态机结 *L, 炉L_缓冲区结 *b, 炉_整数型 i) {
  月亮获取索引_函(L, 1, i);
  if (配置_l_可能性低_宏名(!月亮_是否字符串_函(L, -1)))
    月亮状态_错误_函(L, "invalid 值_圆 (%s) at index %I in table for 'concat'",
                  辅助库_月亮l_类型名称_宏名(L, -1), (配置_月亮I_UAC整型_宏名)i);
  月亮状态_添加值_函(b);
}


static int 月表库_t拼接_函 (炉_状态机结 *L) {
  炉L_缓冲区结 b;
  炉_整数型 最后_变量 = 表_辅助_获取n_宏名(L, 1, 表_表_读_宏名);
  size_t 列分隔_变量;
  const char *分隔_变量 = 月亮状态_可选状态字符串_函(L, 2, "", &列分隔_变量);
  炉_整数型 i = 月亮状态_可选整数_函(L, 3, 1);
  最后_变量 = 月亮状态_可选整数_函(L, 4, 最后_变量);
  月亮状态_缓冲初始的_函(L, &b);
  for (; i < 最后_变量; i++) {
    月表库_添加字段_函(L, &b, i);
    月亮状态_添加状态字符串_函(&b, 分隔_变量, 列分隔_变量);
  }
  if (i == 最后_变量)  /* add 最后_变量 值_圆 (if interval was not 空容器_变量) */
    月表库_添加字段_函(L, &b, i);
  月亮状态_推入结果_函(&b);
  return 1;
}


/*
** {======================================================
** Pack/unpack
** =======================================================
*/

static int 月表库_t打包_函 (炉_状态机结 *L) {
  int i;
  int n = 月亮_获取顶_函(L);  /* number of elements 到_变量 pack */
  月亮创建表_函(L, n, 1);  /* create 结果_变量 table */
  月头_月亮_插入_宏名(L, 1);  /* put it at index 1 */
  for (i = n; i >= 1; i--)  /* assign elements */
    月亮_设置索引_函(L, 1, i);
  月亮推入整数_函(L, n);
  月亮设置字段_函(L, 1, "n");  /* t.n = number of elements */
  return 1;  /* return table */
}


static int 月表库_t反打包_函 (炉_状态机结 *L) {
  lua_Unsigned n;
  炉_整数型 i = 月亮状态_可选整数_函(L, 2, 1);
  炉_整数型 e = 辅助库_月亮l_可选_宏名(L, 月亮状态_检查整数_函, 3, 月亮状态_长度_函(L, 1));
  if (i > e) return 0;  /* 空容器_变量 range */
  n = (lua_Unsigned)e - i;  /* number of elements minus 1 (avoid overflows) */
  if (配置_l_可能性低_宏名(n >= (unsigned int)INT_MAX  ||
                 !月亮_检查栈空间_函(L, (int)(++n))))
    return 月亮状态_错误_函(L, "too many results 到_变量 unpack");
  for (; i < e; i++) {  /* push 实参_短变量[i..e - 1] (到_变量 avoid overflows) */
    月亮获取索引_函(L, 1, i);
  }
  月亮获取索引_函(L, 1, e);  /* push 最后_变量 element */
  return (int)n;
}

/* }====================================================== */



/*
** {======================================================
** Quicksort
** (based on 'Algorithms in MODULA-3', Robert Sedgewick;
**  Addison-Wesley, 1993.)
** =======================================================
*/


/* type for 数组_圆 indices */
typedef unsigned int IdxT;


/*
** Produce a "random" 'unsigned int' 到_变量 randomize pivot choice. This
** macro is used only when '月表库_排序_函' detects a big imbalance in the 结果_变量
** of a 月表库_分区_函. (If you don't want/need this "randomness", ~0 is a
** good choice.)
*/
#if !defined(月表库_l_随机化基准点_函)		/* { */

#include <time.h>

/* 大小_变量 of 'e' measured in number of 'unsigned int's */
#define 表_栈溢出_宏名(e)		(sizeof(e) / sizeof(unsigned int))

/*
** Use 'time' and 'clock' as sources of "randomness". Because we don't
** know the types 'clock_t' and 'time_t', we 月编译器_不能_函 限制_类型转换_宏名 them 到_变量
** anything without risking overflows. A safe way 到_变量 use their values
** is 到_变量 copy them 到_变量 an 数组_圆 of a known type and use the 数组_圆 values.
*/
static unsigned int 月表库_l_随机化基准点_函 (void) {
  clock_t c = clock();
  time_t t = time(NULL);
  unsigned int 缓冲_变量[表_栈溢出_宏名(c) + 表_栈溢出_宏名(t)];
  unsigned int i, 范围_缩变量 = 0;
  memcpy(缓冲_变量, &c, 表_栈溢出_宏名(c) * sizeof(unsigned int));
  memcpy(缓冲_变量 + 表_栈溢出_宏名(c), &t, 表_栈溢出_宏名(t) * sizeof(unsigned int));
  for (i = 0; i < 表_栈溢出_宏名(缓冲_变量); i++)
    范围_缩变量 += 缓冲_变量[i];
  return 范围_缩变量;
}

#endif					/* } */


/* arrays larger than '表_随机数生成限制_宏名' may use randomized pivots */
#define 表_随机数生成限制_宏名	100u


static void 月表库_设置2_函 (炉_状态机结 *L, IdxT i, IdxT j) {
  月亮_设置索引_函(L, 1, i);
  月亮_设置索引_函(L, 1, j);
}


/*
** Return true iff 值_圆 at 栈_圆小 index 'a' is less than the 值_圆 at
** index 'b' (according 到_变量 the order of the 月表库_排序_函).
*/
static int 月表库_排序_对比_函 (炉_状态机结 *L, int a, int b) {
  if (月头_月亮_是否空值_宏名(L, 2))  /* no function? */
    return 月亮对比_函(L, a, b, 月头_月亮_操作小于_宏名);  /* a < b */
  else {  /* function */
    int 结果_短变量;
    月亮_推入值_函(L, 2);    /* push function */
    月亮_推入值_函(L, a-1);  /* -1 到_变量 compensate function */
    月亮_推入值_函(L, b-2);  /* -2 到_变量 compensate function and 'a' */
    月头_月亮_调用_宏名(L, 2, 1);      /* call function */
    结果_短变量 = 月亮_到布尔值_函(L, -1);  /* get 结果_变量 */
    月头_月亮_弹出_宏名(L, 1);          /* pop 结果_变量 */
    return 结果_短变量;
  }
}


/*
** Does the 月表库_分区_函: Pivot P is at the 顶部_变量 of the 栈_圆小.
** precondition: a[lo] <= P == a[上_小变量-1] <= a[上_小变量],
** so it only needs 到_变量 do the 月表库_分区_函 from lo + 1 到_变量 上_小变量 - 2.
** Pos-condition: a[lo .. i - 1] <= a[i] == P <= a[i + 1 .. 上_小变量]
** returns 'i'.
*/
static IdxT 月表库_分区_函 (炉_状态机结 *L, IdxT lo, IdxT 上_小变量) {
  IdxT i = lo;  /* will be incremented before 首先_变量 use */
  IdxT j = 上_小变量 - 1;  /* will be decremented before 首先_变量 use */
  /* 环_变量 invariant: a[lo .. i] <= P <= a[j .. 上_小变量] */
  for (;;) {
    /* 下一个_变量 环_变量: repeat ++i while a[i] < P */
    while ((void)月亮获取索引_函(L, 1, ++i), 月表库_排序_对比_函(L, -1, -2)) {
      if (配置_l_可能性低_宏名(i == 上_小变量 - 1))  /* a[i] < P  but a[上_小变量 - 1] == P  ?? */
        月亮状态_错误_函(L, "invalid order function for sorting");
      月头_月亮_弹出_宏名(L, 1);  /* remove a[i] */
    }
    /* after the 环_变量, a[i] >= P and a[lo .. i - 1] < P */
    /* 下一个_变量 环_变量: repeat --j while P < a[j] */
    while ((void)月亮获取索引_函(L, 1, --j), 月表库_排序_对比_函(L, -3, -1)) {
      if (配置_l_可能性低_宏名(j < i))  /* j < i  but  a[j] > P ?? */
        月亮状态_错误_函(L, "invalid order function for sorting");
      月头_月亮_弹出_宏名(L, 1);  /* remove a[j] */
    }
    /* after the 环_变量, a[j] <= P and a[j + 1 .. 上_小变量] >= P */
    if (j < i) {  /* no elements out of place? */
      /* a[lo .. i - 1] <= P <= a[j + 1 .. i .. 上_小变量] */
      月头_月亮_弹出_宏名(L, 1);  /* pop a[j] */
      /* swap pivot (a[上_小变量 - 1]) with a[i] 到_变量 satisfy 位置_缩变量-condition */
      月表库_设置2_函(L, 上_小变量 - 1, i);
      return i;
    }
    /* otherwise, swap a[i] - a[j] 到_变量 restore invariant and repeat */
    月表库_设置2_函(L, i, j);
  }
}


/*
** Choose an element in the middle (2nd-3th quarters) of [lo,上_小变量]
** "randomized" by '范围_缩变量'
*/
static IdxT 月表库_选定基准点_函 (IdxT lo, IdxT 上_小变量, unsigned int 范围_缩变量) {
  IdxT r4 = (上_小变量 - lo) / 4;  /* range/4 */
  IdxT p = 范围_缩变量 % (r4 * 2) + (lo + r4);
  限制_月亮_断言_宏名(lo + r4 <= p && p <= 上_小变量 - r4);
  return p;
}


/*
** Quicksort algorithm (recursive function)
*/
static void 月表库_辅助排序_函 (炉_状态机结 *L, IdxT lo, IdxT 上_小变量,
                                   unsigned int 范围_缩变量) {
  while (lo < 上_小变量) {  /* 环_变量 for tail recursion */
    IdxT p;  /* Pivot index */
    IdxT n;  /* 到_变量 be used later */
    /* 月表库_排序_函 elements 'lo', 'p', and '上_小变量' */
    月亮获取索引_函(L, 1, lo);
    月亮获取索引_函(L, 1, 上_小变量);
    if (月表库_排序_对比_函(L, -1, -2))  /* a[上_小变量] < a[lo]? */
      月表库_设置2_函(L, lo, 上_小变量);  /* swap a[lo] - a[上_小变量] */
    else
      月头_月亮_弹出_宏名(L, 2);  /* remove both values */
    if (上_小变量 - lo == 1)  /* only 2 elements? */
      return;  /* already sorted */
    if (上_小变量 - lo < 表_随机数生成限制_宏名 || 范围_缩变量 == 0)  /* small interval or no randomize? */
      p = (lo + 上_小变量)/2;  /* middle element is a good pivot */
    else  /* for larger intervals, it is worth a random pivot */
      p = 月表库_选定基准点_函(lo, 上_小变量, 范围_缩变量);
    月亮获取索引_函(L, 1, p);
    月亮获取索引_函(L, 1, lo);
    if (月表库_排序_对比_函(L, -2, -1))  /* a[p] < a[lo]? */
      月表库_设置2_函(L, p, lo);  /* swap a[p] - a[lo] */
    else {
      月头_月亮_弹出_宏名(L, 1);  /* remove a[lo] */
      月亮获取索引_函(L, 1, 上_小变量);
      if (月表库_排序_对比_函(L, -1, -2))  /* a[上_小变量] < a[p]? */
        月表库_设置2_函(L, p, 上_小变量);  /* swap a[上_小变量] - a[p] */
      else
        月头_月亮_弹出_宏名(L, 2);
    }
    if (上_小变量 - lo == 2)  /* only 3 elements? */
      return;  /* already sorted */
    月亮获取索引_函(L, 1, p);  /* get middle element (Pivot) */
    月亮_推入值_函(L, -1);  /* push Pivot */
    月亮获取索引_函(L, 1, 上_小变量 - 1);  /* push a[上_小变量 - 1] */
    月表库_设置2_函(L, p, 上_小变量 - 1);  /* swap Pivot (a[p]) with a[上_小变量 - 1] */
    p = 月表库_分区_函(L, lo, 上_小变量);
    /* a[lo .. p - 1] <= a[p] == P <= a[p + 1 .. 上_小变量] */
    if (p - lo < 上_小变量 - p) {  /* lower interval is smaller? */
      月表库_辅助排序_函(L, lo, p - 1, 范围_缩变量);  /* call recursively for lower interval */
      n = p - lo;  /* 大小_变量 of smaller interval */
      lo = p + 1;  /* tail call for [p + 1 .. 上_小变量] (upper interval) */
    }
    else {
      月表库_辅助排序_函(L, p + 1, 上_小变量, 范围_缩变量);  /* call recursively for upper interval */
      n = 上_小变量 - p;  /* 大小_变量 of smaller interval */
      上_小变量 = p - 1;  /* tail call for [lo .. p - 1]  (lower interval) */
    }
    if ((上_小变量 - lo) / 128 > n) /* 月表库_分区_函 too imbalanced? */
      范围_缩变量 = 月表库_l_随机化基准点_函();  /* try a new randomization */
  }  /* tail call 月表库_辅助排序_函(L, lo, 上_小变量, 范围_缩变量) */
}


static int 月表库_排序_函 (炉_状态机结 *L) {
  炉_整数型 n = 表_辅助_获取n_宏名(L, 1, 表_表_读写_宏名);
  if (n > 1) {  /* non-trivial interval? */
    辅助库_月亮l_实参检查_宏名(L, n < INT_MAX, 1, "数组_圆 too big");
    if (!月头_月亮_是否没有或空值_宏名(L, 2))  /* is there a 2nd argument? */
      月亮状态_检查类型_函(L, 2, 月头_月亮_T函数_宏名);  /* must be a function */
    月亮_设置顶_函(L, 2);  /* make sure there are two arguments */
    月表库_辅助排序_函(L, 1, (IdxT)n, 0);
  }
  return 0;
}

/* }====================================================== */


static const 炉L_寄结 表_函_缩变量[] = {
  {"concat", 月表库_t拼接_函},
  {"insert", 月表库_t插入_函},
  {"pack", 月表库_t打包_函},
  {"unpack", 月表库_t反打包_函},
  {"remove", 月表库_t移除_函},
  {"move", 月表库_t移动_函},
  {"月表库_排序_函", 月表库_排序_函},
  {NULL, NULL}
};


配置_月亮模块_应程接_宏名 int 月亮打开表_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 表_函_缩变量);
  return 1;
}

