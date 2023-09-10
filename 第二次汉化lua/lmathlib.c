/*
** $Id: lmathlib.c $
** Standard mathematical library
** See Copyright Notice in lua.h
*/

#define lmathlib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI	(配置_数学操作_宏名(3.141592653589793238462643383279502884))


static int 月数学库_数学_绝对值_函 (炉_状态机结 *L) {
  if (月亮_是否整数_函(L, 1)) {
    炉_整数型 n = 月头_月亮_到整数_宏名(L, 1);
    if (n < 0) n = (炉_整数型)(0u - (lua_Unsigned)n);
    月亮推入整数_函(L, n);
  }
  else
    月亮推入数目_函(L, 配置_数学操作_宏名(fabs)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_正弦_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(sin)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_余弦_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(cos)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_正切_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(tan)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_反正弦_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(asin)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_反余弦_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(acos)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_反正切_函 (炉_状态机结 *L) {
  炉_数目型 y = 月亮状态_检查数目_函(L, 1);
  炉_数目型 x = 月亮状态_可选数目_函(L, 2, 1);
  月亮推入数目_函(L, 配置_数学操作_宏名(atan2)(y, x));
  return 1;
}


static int 月数学库_数学_到整型_函 (炉_状态机结 *L) {
  int 有效_变量;
  炉_整数型 n = 月亮_到整数x_函(L, 1, &有效_变量);
  if (配置_l_可能性高_宏名(有效_变量))
    月亮推入整数_函(L, n);
  else {
    月亮状态_检查任意_函(L, 1);
    辅助库_月亮l_推失败_宏名(L);  /* 值_圆 is not convertible 到_变量 integer */
  }
  return 1;
}


static void 月数学库_推数整数值_函 (炉_状态机结 *L, 炉_数目型 d) {
  炉_整数型 n;
  if (配置_月亮_数目到整数_宏名(d, &n))  /* does 'd' fit in an integer? */
    月亮推入整数_函(L, n);  /* 结果_变量 is integer */
  else
    月亮推入数目_函(L, d);  /* 结果_变量 is float */
}


static int 月数学库_数学_向下取整_函 (炉_状态机结 *L) {
  if (月亮_是否整数_函(L, 1))
    月亮_设置顶_函(L, 1);  /* integer is its own floor */
  else {
    炉_数目型 d = 配置_数学操作_宏名(floor)(月亮状态_检查数目_函(L, 1));
    月数学库_推数整数值_函(L, d);
  }
  return 1;
}


static int 月数学库_数学_向上取整_函 (炉_状态机结 *L) {
  if (月亮_是否整数_函(L, 1))
    月亮_设置顶_函(L, 1);  /* integer is its own ceil */
  else {
    炉_数目型 d = 配置_数学操作_宏名(ceil)(月亮状态_检查数目_函(L, 1));
    月数学库_推数整数值_函(L, d);
  }
  return 1;
}


static int 月数学库_数学_浮点取模_函 (炉_状态机结 *L) {
  if (月亮_是否整数_函(L, 1) && 月亮_是否整数_函(L, 2)) {
    炉_整数型 d = 月头_月亮_到整数_宏名(L, 2);
    if ((lua_Unsigned)d + 1u <= 1u) {  /* special cases: -1 or 0 */
      辅助库_月亮l_实参检查_宏名(L, d != 0, 2, "zero");
      月亮推入整数_函(L, 0);  /* avoid overflow with 0x80000... / -1 */
    }
    else
      月亮推入整数_函(L, 月头_月亮_到整数_宏名(L, 1) % d);
  }
  else
    月亮推入数目_函(L, 配置_数学操作_宏名(fmod)(月亮状态_检查数目_函(L, 1),
                                     月亮状态_检查数目_函(L, 2)));
  return 1;
}


/*
** 下一个_变量 function does not use 'modf', avoiding problems with 'double*'
** (which is not compatible with 'float*') when 炉_数目型 is not
** 'double'.
*/
static int 月数学库_数学_浮点拆分_函 (炉_状态机结 *L) {
  if (月亮_是否整数_函(L ,1)) {
    月亮_设置顶_函(L, 1);  /* number is its own integer part */
    月亮推入数目_函(L, 0);  /* no fractional part */
  }
  else {
    炉_数目型 n = 月亮状态_检查数目_函(L, 1);
    /* integer part (rounds toward zero) */
    炉_数目型 指令指针_缩变量 = (n < 0) ? 配置_数学操作_宏名(ceil)(n) : 配置_数学操作_宏名(floor)(n);
    月数学库_推数整数值_函(L, 指令指针_缩变量);
    /* fractional part (test 已需要_变量 for inf/-inf) */
    月亮推入数目_函(L, (n == 指令指针_缩变量) ? 配置_数学操作_宏名(0.0) : (n - 指令指针_缩变量));
  }
  return 2;
}


static int 月数学库_数学_平方根_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(sqrt)(月亮状态_检查数目_函(L, 1)));
  return 1;
}


static int 月数学库_数学_无符号小于_函 (炉_状态机结 *L) {
  炉_整数型 a = 月亮状态_检查整数_函(L, 1);
  炉_整数型 b = 月亮状态_检查整数_函(L, 2);
  月亮推入布尔值_函(L, (lua_Unsigned)a < (lua_Unsigned)b);
  return 1;
}

static int 月数学库_数学_对数_函 (炉_状态机结 *L) {
  炉_数目型 x = 月亮状态_检查数目_函(L, 1);
  炉_数目型 结果_短变量;
  if (月头_月亮_是否没有或空值_宏名(L, 2))
    结果_短变量 = 配置_数学操作_宏名(log)(x);
  else {
    炉_数目型 基本_变量 = 月亮状态_检查数目_函(L, 2);
#if !defined(配置_月亮_用_C89_宏名)
    if (基本_变量 == 配置_数学操作_宏名(2.0))
      结果_短变量 = 配置_数学操作_宏名(log2)(x);
    else
#endif
    if (基本_变量 == 配置_数学操作_宏名(10.0))
      结果_短变量 = 配置_数学操作_宏名(log10)(x);
    else
      结果_短变量 = 配置_数学操作_宏名(log)(x)/配置_数学操作_宏名(log)(基本_变量);
  }
  月亮推入数目_函(L, 结果_短变量);
  return 1;
}

static int 月数学库_数学_指数_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(exp)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int 月数学库_数学_角度转弧度_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 月亮状态_检查数目_函(L, 1) * (配置_数学操作_宏名(180.0) / PI));
  return 1;
}

static int 月数学库_数学_弧度转角度_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, 月亮状态_检查数目_函(L, 1) * (PI / 配置_数学操作_宏名(180.0)));
  return 1;
}


static int 月数学库_数学_最小值_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  int 指令最小_变量 = 1;  /* index of 当前_圆 minimum 值_圆 */
  int i;
  辅助库_月亮l_实参检查_宏名(L, n >= 1, 1, "值_圆 expected");
  for (i = 2; i <= n; i++) {
    if (月亮对比_函(L, i, 指令最小_变量, 月头_月亮_操作小于_宏名))
      指令最小_变量 = i;
  }
  月亮_推入值_函(L, 指令最小_变量);
  return 1;
}


static int 月数学库_数学_最大值_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  int 指令最大_变量 = 1;  /* index of 当前_圆 maximum 值_圆 */
  int i;
  辅助库_月亮l_实参检查_宏名(L, n >= 1, 1, "值_圆 expected");
  for (i = 2; i <= n; i++) {
    if (月亮对比_函(L, 指令最大_变量, i, 月头_月亮_操作小于_宏名))
      指令最大_变量 = i;
  }
  月亮_推入值_函(L, 指令最大_变量);
  return 1;
}


static int 月数学库_数学_类型_函 (炉_状态机结 *L) {
  if (月亮_类型_函(L, 1) == 月头_月亮_T数目_宏名)
    月亮推入字符串_函(L, (月亮_是否整数_函(L, 1)) ? "integer" : "float");
  else {
    月亮状态_检查任意_函(L, 1);
    辅助库_月亮l_推失败_宏名(L);
  }
  return 1;
}



/*
** {==================================================================
** Pseudo-Random Number Generator based on 'xoshiro256**'.
** ===================================================================
*/

/* number of binary digits in the mantissa of a float */
#define 数学库_浮点数位数_宏名	配置_l_浮点属性_宏名(MANT_DIG)

#if 数学库_浮点数位数_宏名 > 64
/* there are only 64 random bits; use them all */
#undef 数学库_浮点数位数_宏名
#define 数学库_浮点数位数_宏名	64
#endif


/*
** LUA_RAND32 forces the use of 32-bit integers in the implementation
** of the PRN generator (mainly for testing).
*/
#if !defined(LUA_RAND32) && !defined(范围64_结)

/* try 到_变量 find an integer type with at least 64 bits */

#if ((ULONG_MAX >> 31) >> 31) >= 3

/* 'long' has at least 64 bits */
#define 范围64_结		unsigned long

#elif !defined(配置_月亮_用_C89_宏名) && defined(LLONG_MAX)

/* there is a 'long long' type (which must have at least 64 bits) */
#define 范围64_结		unsigned long long

#elif ((配置_月亮_最大无符号_宏名 >> 31) >> 31) >= 3

/* 'lua_Unsigned' has at least 64 bits */
#define 范围64_结		lua_Unsigned

#endif

#endif


#if defined(范围64_结)  /* { */

/*
** Standard implementation, using 64-bit integers.
** If '范围64_结' has 更多_变量 than 64 bits, the 额外_变量 bits do not interfere
** with the 64 initial bits, except in a 右_圆 shift. Moreover, the
** 最终_变量 结果_变量 has 到_变量 discard the 额外_变量 bits.
*/

/* avoid using 额外_变量 bits when 已需要_变量 */
#define 数学库_截取64位_宏名(x)	((x) & 0xffffffffffffffffu)


/* rotate 左_圆 'x' by 'n' bits */
static 范围64_结 月数学库_旋转左_函 (范围64_结 x, int n) {
  return (x << n) | (数学库_截取64位_宏名(x) >> (64 - n));
}

static 范围64_结 月数学库_下一个随机_函 (范围64_结 *状态机_变量) {
  范围64_结 state0 = 状态机_变量[0];
  范围64_结 state1 = 状态机_变量[1];
  范围64_结 state2 = 状态机_变量[2] ^ state0;
  范围64_结 state3 = 状态机_变量[3] ^ state1;
  范围64_结 结果_短变量 = 月数学库_旋转左_函(state1 * 5, 7) * 9;
  状态机_变量[0] = state0 ^ state3;
  状态机_变量[1] = state1 ^ state2;
  状态机_变量[2] = state2 ^ (state1 << 17);
  状态机_变量[3] = 月数学库_旋转左_函(state3, 45);
  return 结果_短变量;
}


/* must take care 到_变量 not shift stuff by 更多_变量 than 63 slots */


/*
** Convert bits from a random integer into a float in the
** interval [0,1), getting the higher FIG bits from the
** random unsigned integer and converting that 到_变量 a float.
*/

/* must throw out the 额外_变量 (64 - 数学库_浮点数位数_宏名) bits */
#define 数学库_移位64位_浮点数位数_宏名	(64 - 数学库_浮点数位数_宏名)

/* 到_变量 scale 到_变量 [0, 1), multiply by 数学库_比例浮点数位数_宏名 = 2^(-数学库_浮点数位数_宏名) */
#define 数学库_比例浮点数位数_宏名	(配置_数学操作_宏名(0.5) / ((范围64_结)1 << (数学库_浮点数位数_宏名 - 1)))

static 炉_数目型 月数学库_I到双精度_函 (范围64_结 x) {
  return (炉_数目型)(数学库_截取64位_宏名(x) >> 数学库_移位64位_浮点数位数_宏名) * 数学库_比例浮点数位数_宏名;
}

/* convert a '范围64_结' 到_变量 a 'lua_Unsigned' */
#define 月数学库_I到无符整型_函(x)	((lua_Unsigned)数学库_截取64位_宏名(x))

/* convert a 'lua_Unsigned' 到_变量 a '范围64_结' */
#define 月数学库_整型到I_函(x)	((范围64_结)(x))


#else	/* no '范围64_结'   }{ */

/* get an integer with at least 32 bits */
#if 配置_月亮I_是否32位整型_宏名
typedef unsigned int lu_int32;
#else
typedef unsigned long lu_int32;
#endif


/*
** Use two 32-bit integers 到_变量 represent a 64-bit quantity.
*/
typedef struct 范围64_结 {
  lu_int32 h;  /* higher half */
  lu_int32 l;  /* lower half */
} 范围64_结;


/*
** If 'lu_int32' has 更多_变量 than 32 bits, the 额外_变量 bits do not interfere
** with the 32 initial bits, except in a 右_圆 shift and comparisons.
** Moreover, the 最终_变量 结果_变量 has 到_变量 discard the 额外_变量 bits.
*/

/* avoid using 额外_变量 bits when 已需要_变量 */
#define 数学库_截取32位_宏名(x)	((x) & 0xffffffffu)


/*
** basic operations on '范围64_结' values
*/

/* build a new 范围64_结 值_圆 */
static 范围64_结 月数学库_打包I_函 (lu_int32 h, lu_int32 l) {
  范围64_结 结果_变量;
  结果_变量.h = h;
  结果_变量.l = l;
  return 结果_变量;
}

/* return i << n */
static 范围64_结 月数学库_I左移_函 (范围64_结 i, int n) {
  限制_月亮_断言_宏名(n > 0 && n < 32);
  return 月数学库_打包I_函((i.h << n) | (数学库_截取32位_宏名(i.l) >> (32 - n)), i.l << n);
}

/* 整1_缩变量 ^= 索2_缩变量 */
static void 月数学库_I异或_函 (范围64_结 *整1_缩变量, 范围64_结 索2_缩变量) {
  整1_缩变量->h ^= 索2_缩变量.h;
  整1_缩变量->l ^= 索2_缩变量.l;
}

/* return 整1_缩变量 + 索2_缩变量 */
static 范围64_结 月数学库_I相加_函 (范围64_结 整1_缩变量, 范围64_结 索2_缩变量) {
  范围64_结 结果_变量 = 月数学库_打包I_函(整1_缩变量.h + 索2_缩变量.h, 整1_缩变量.l + 索2_缩变量.l);
  if (数学库_截取32位_宏名(结果_变量.l) < 数学库_截取32位_宏名(整1_缩变量.l))  /* carry? */
    结果_变量.h++;
  return 结果_变量;
}

/* return i * 5 */
static 范围64_结 月数学库_乘以5_函 (范围64_结 i) {
  return 月数学库_I相加_函(月数学库_I左移_函(i, 2), i);  /* i * 5 == (i << 2) + i */
}

/* return i * 9 */
static 范围64_结 月数学库_乘以9_函 (范围64_结 i) {
  return 月数学库_I相加_函(月数学库_I左移_函(i, 3), i);  /* i * 9 == (i << 3) + i */
}

/* return 'i' rotated 左_圆 'n' bits */
static 范围64_结 月数学库_旋转左_函 (范围64_结 i, int n) {
  限制_月亮_断言_宏名(n > 0 && n < 32);
  return 月数学库_打包I_函((i.h << n) | (数学库_截取32位_宏名(i.l) >> (32 - n)),
               (数学库_截取32位_宏名(i.h) >> (32 - n)) | (i.l << n));
}

/* for offsets larger than 32, rotate 右_圆 by 64 - 偏移_变量 */
static 范围64_结 月数学库_旋转左1_函 (范围64_结 i, int n) {
  限制_月亮_断言_宏名(n > 32 && n < 64);
  n = 64 - n;
  return 月数学库_打包I_函((数学库_截取32位_宏名(i.h) >> n) | (i.l << (32 - n)),
               (i.h << (32 - n)) | (数学库_截取32位_宏名(i.l) >> n));
}

/*
** implementation of 'xoshiro256**' algorithm on '范围64_结' values
*/
static 范围64_结 月数学库_下一个随机_函 (范围64_结 *状态机_变量) {
  范围64_结 结果_短变量 = 月数学库_乘以9_函(月数学库_旋转左_函(月数学库_乘以5_函(状态机_变量[1]), 7));
  范围64_结 t = 月数学库_I左移_函(状态机_变量[1], 17);
  月数学库_I异或_函(&状态机_变量[2], 状态机_变量[0]);
  月数学库_I异或_函(&状态机_变量[3], 状态机_变量[1]);
  月数学库_I异或_函(&状态机_变量[1], 状态机_变量[2]);
  月数学库_I异或_函(&状态机_变量[0], 状态机_变量[3]);
  月数学库_I异或_函(&状态机_变量[2], t);
  状态机_变量[3] = 月数学库_旋转左1_函(状态机_变量[3], 45);
  return 结果_短变量;
}


/*
** Converts a '范围64_结' into a float.
*/

/* an unsigned 1 with proper type */
#define UONE		((lu_int32)1)


#if 数学库_浮点数位数_宏名 <= 32

/* 2^(-数学库_浮点数位数_宏名) */
#define 数学库_比例浮点数位数_宏名       (配置_数学操作_宏名(0.5) / (UONE << (数学库_浮点数位数_宏名 - 1)))

/*
** get 上_小变量 到_变量 32 bits from higher half, shifting 右_圆 到_变量
** throw out the 额外_变量 bits.
*/
static 炉_数目型 月数学库_I到双精度_函 (范围64_结 x) {
  炉_数目型 h = (炉_数目型)(数学库_截取32位_宏名(x.h) >> (32 - 数学库_浮点数位数_宏名));
  return h * 数学库_比例浮点数位数_宏名;
}

#else	/* 32 < 数学库_浮点数位数_宏名 <= 64 */

/* must take care 到_变量 not shift stuff by 更多_变量 than 31 slots */

/* 2^(-数学库_浮点数位数_宏名) = 1.0 / 2^30 / 2^3 / 2^(数学库_浮点数位数_宏名-33) */
#define 数学库_比例浮点数位数_宏名  \
    (配置_数学操作_宏名(1.0) / (UONE << 30) / 配置_数学操作_宏名(8.0) / (UONE << (数学库_浮点数位数_宏名 - 33)))

/*
** use 数学库_浮点数位数_宏名 - 32 bits from lower half, throwing out the other
** (32 - (数学库_浮点数位数_宏名 - 32)) = (64 - 数学库_浮点数位数_宏名) bits
*/
#define 数学库_移位低位_宏名	(64 - 数学库_浮点数位数_宏名)

/*
** higher 32 bits go after those (数学库_浮点数位数_宏名 - 32) bits: 数学库_移位高位_宏名 = 2^(数学库_浮点数位数_宏名 - 32)
*/
#define 数学库_移位高位_宏名		((炉_数目型)(UONE << (数学库_浮点数位数_宏名 - 33)) * 配置_数学操作_宏名(2.0))


static 炉_数目型 月数学库_I到双精度_函 (范围64_结 x) {
  炉_数目型 h = (炉_数目型)数学库_截取32位_宏名(x.h) * 数学库_移位高位_宏名;
  炉_数目型 l = (炉_数目型)(数学库_截取32位_宏名(x.l) >> 数学库_移位低位_宏名);
  return (h + l) * 数学库_比例浮点数位数_宏名;
}

#endif


/* convert a '范围64_结' 到_变量 a 'lua_Unsigned' */
static lua_Unsigned 月数学库_I到无符整型_函 (范围64_结 x) {
  return (((lua_Unsigned)数学库_截取32位_宏名(x.h) << 31) << 1) | (lua_Unsigned)数学库_截取32位_宏名(x.l);
}

/* convert a 'lua_Unsigned' 到_变量 a '范围64_结' */
static 范围64_结 月数学库_整型到I_函 (lua_Unsigned n) {
  return 月数学库_打包I_函((lu_int32)((n >> 31) >> 1), (lu_int32)n);
}

#endif  /* } */


/*
** A 状态机_变量 uses four '范围64_结' values.
*/
typedef struct {
  范围64_结 s[4];
} 范围状态机_结;


/*
** Project the random integer 'ran' into the interval [0, n].
** Because 'ran' has 2^B possible values, the projection can only be
** uniform when the 大小_变量 of the interval is a power of 2 (exact
** division). Otherwise, 到_变量 get a uniform projection into [0, n], we
** 首先_变量 compute '限_短变量', the smallest Mersenne number not smaller than
** 'n'. We then 月数学库_投影_函 'ran' into the interval [0, 限_短变量].  If the 结果_变量
** is inside [0, n], we are done. Otherwise, we try with another 'ran',
** until we have a 结果_变量 inside the interval.
*/
static lua_Unsigned 月数学库_投影_函 (lua_Unsigned ran, lua_Unsigned n,
                             范围状态机_结 *状态机_变量) {
  if ((n & (n + 1)) == 0)  /* is 'n + 1' a power of 2? */
    return ran & n;  /* no bias */
  else {
    lua_Unsigned 限_短变量 = n;
    /* compute the smallest (2^b - 1) not smaller than 'n' */
    限_短变量 |= (限_短变量 >> 1);
    限_短变量 |= (限_短变量 >> 2);
    限_短变量 |= (限_短变量 >> 4);
    限_短变量 |= (限_短变量 >> 8);
    限_短变量 |= (限_短变量 >> 16);
#if (配置_月亮_最大无符号_宏名 >> 31) >= 3
    限_短变量 |= (限_短变量 >> 32);  /* integer type has 更多_变量 than 32 bits */
#endif
    限制_月亮_断言_宏名((限_短变量 & (限_短变量 + 1)) == 0  /* '限_短变量 + 1' is a power of 2, */
      && 限_短变量 >= n  /* not smaller than 'n', */
      && (限_短变量 >> 1) < n);  /* and it is the smallest one */
    while ((ran &= 限_短变量) > n)  /* 月数学库_投影_函 'ran' into [0..限_短变量] */
      ran = 月数学库_I到无符整型_函(月数学库_下一个随机_函(状态机_变量->s));  /* not inside [0..n]? try again */
    return ran;
  }
}


static int 月数学库_数学_随机数_函 (炉_状态机结 *L) {
  炉_整数型 低_变量, 上_小变量;
  lua_Unsigned p;
  范围状态机_结 *状态机_变量 = (范围状态机_结 *)月亮_到用户数据_函(L, 月头_月亮_上值索引_宏名(1));
  范围64_结 寄变_变量 = 月数学库_下一个随机_函(状态机_变量->s);  /* 下一个_变量 pseudo-random 值_圆 */
  switch (月亮_获取顶_函(L)) {  /* 月解析器_检查_函 number of arguments */
    case 0: {  /* no arguments */
      月亮推入数目_函(L, 月数学库_I到双精度_函(寄变_变量));  /* float between 0 and 1 */
      return 1;
    }
    case 1: {  /* only upper 限制_变量 */
      低_变量 = 1;
      上_小变量 = 月亮状态_检查整数_函(L, 1);
      if (上_小变量 == 0) {  /* single 0 as argument? */
        月亮推入整数_函(L, 月数学库_I到无符整型_函(寄变_变量));  /* full random integer */
        return 1;
      }
      break;
    }
    case 2: {  /* lower and upper 限制们_变量 */
      低_变量 = 月亮状态_检查整数_函(L, 1);
      上_小变量 = 月亮状态_检查整数_函(L, 2);
      break;
    }
    default: return 月亮状态_错误_函(L, "wrong number of arguments");
  }
  /* random integer in the interval [低_变量, 上_小变量] */
  辅助库_月亮l_实参检查_宏名(L, 低_变量 <= 上_小变量, 1, "interval is 空容器_变量");
  /* 月数学库_投影_函 random integer into the interval [0, 上_小变量 - 低_变量] */
  p = 月数学库_投影_函(月数学库_I到无符整型_函(寄变_变量), (lua_Unsigned)上_小变量 - (lua_Unsigned)低_变量, 状态机_变量);
  月亮推入整数_函(L, p + (lua_Unsigned)低_变量);
  return 1;
}


static void 月数学库_设置种子_函 (炉_状态机结 *L, 范围64_结 *状态机_变量,
                     lua_Unsigned 数1_缩变量, lua_Unsigned 数2_缩变量) {
  int i;
  状态机_变量[0] = 月数学库_整型到I_函(数1_缩变量);
  状态机_变量[1] = 月数学库_整型到I_函(0xff);  /* avoid a zero 状态机_变量 */
  状态机_变量[2] = 月数学库_整型到I_函(数2_缩变量);
  状态机_变量[3] = 月数学库_整型到I_函(0);
  for (i = 0; i < 16; i++)
    月数学库_下一个随机_函(状态机_变量);  /* discard initial values 到_变量 "spread" 种籽_圆 */
  月亮推入整数_函(L, 数1_缩变量);
  月亮推入整数_函(L, 数2_缩变量);
}


/*
** Set a "random" 种籽_圆. To get some randomness, use the 当前_圆 time
** and the address of 'L' (in case the machine does address 空间_圆 layout
** randomization).
*/
static void 月数学库_随机种子_函 (炉_状态机结 *L, 范围状态机_结 *状态机_变量) {
  lua_Unsigned 种子1_变量 = (lua_Unsigned)time(NULL);
  lua_Unsigned 种子2_变量 = (lua_Unsigned)(size_t)L;
  月数学库_设置种子_函(L, 状态机_变量->s, 种子1_变量, 种子2_变量);
}


static int 月数学库_数学_随机数种子_函 (炉_状态机结 *L) {
  范围状态机_结 *状态机_变量 = (范围状态机_结 *)月亮_到用户数据_函(L, 月头_月亮_上值索引_宏名(1));
  if (月头_月亮_是否没有_宏名(L, 1)) {
    月数学库_随机种子_函(L, 状态机_变量);
  }
  else {
    炉_整数型 数1_缩变量 = 月亮状态_检查整数_函(L, 1);
    炉_整数型 数2_缩变量 = 月亮状态_可选整数_函(L, 2, 0);
    月数学库_设置种子_函(L, 状态机_变量->s, 数1_缩变量, 数2_缩变量);
  }
  return 2;  /* return seeds */
}


static const 炉L_寄结 随机函_变量[] = {
  {"random", 月数学库_数学_随机数_函},
  {"randomseed", 月数学库_数学_随机数种子_函},
  {NULL, NULL}
};


/*
** Register the random functions and initialize their 状态机_变量.
*/
static void 月数学库_设置随机函_函 (炉_状态机结 *L) {
  范围状态机_结 *状态机_变量 = (范围状态机_结 *)月亮创建新的用户数据uv_函(L, sizeof(范围状态机_结), 0);
  月数学库_随机种子_函(L, 状态机_变量);  /* initialize with a "random" 种籽_圆 */
  月头_月亮_弹出_宏名(L, 2);  /* remove 已推入_小写 seeds */
  月亮状态_设置函们_函(L, 随机函_变量, 1);
}

/* }================================================================== */


/*
** {==================================================================
** Deprecated functions (for compatibility only)
** ===================================================================
*/
#if defined(配置_月亮_兼容_数学库_宏名)

static int math_cosh (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(cosh)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int math_sinh (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(sinh)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int math_tanh (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(tanh)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

static int math_pow (炉_状态机结 *L) {
  炉_数目型 x = 月亮状态_检查数目_函(L, 1);
  炉_数目型 y = 月亮状态_检查数目_函(L, 2);
  月亮推入数目_函(L, 配置_数学操作_宏名(pow)(x, y));
  return 1;
}

static int math_frexp (炉_状态机结 *L) {
  int e;
  月亮推入数目_函(L, 配置_数学操作_宏名(frexp)(月亮状态_检查数目_函(L, 1), &e));
  月亮推入整数_函(L, e);
  return 2;
}

static int math_ldexp (炉_状态机结 *L) {
  炉_数目型 x = 月亮状态_检查数目_函(L, 1);
  int 事件点_缩变量 = (int)月亮状态_检查整数_函(L, 2);
  月亮推入数目_函(L, 配置_数学操作_宏名(ldexp)(x, 事件点_缩变量));
  return 1;
}

static int math_log10 (炉_状态机结 *L) {
  月亮推入数目_函(L, 配置_数学操作_宏名(log10)(月亮状态_检查数目_函(L, 1)));
  return 1;
}

#endif
/* }================================================================== */



static const 炉L_寄结 数学库_变量[] = {
  {"abs",   月数学库_数学_绝对值_函},
  {"acos",  月数学库_数学_反余弦_函},
  {"asin",  月数学库_数学_反正弦_函},
  {"atan",  月数学库_数学_反正切_函},
  {"ceil",  月数学库_数学_向上取整_函},
  {"cos",   月数学库_数学_余弦_函},
  {"deg",   月数学库_数学_角度转弧度_函},
  {"exp",   月数学库_数学_指数_函},
  {"虚机头_到整数_宏名", 月数学库_数学_到整型_函},
  {"floor", 月数学库_数学_向下取整_函},
  {"fmod",   月数学库_数学_浮点取模_函},
  {"ult",   月数学库_数学_无符号小于_函},
  {"log",   月数学库_数学_对数_函},
  {"最大_小变量",   月数学库_数学_最大值_函},
  {"min",   月数学库_数学_最小值_函},
  {"modf",   月数学库_数学_浮点拆分_函},
  {"rad",   月数学库_数学_弧度转角度_函},
  {"sin",   月数学库_数学_正弦_函},
  {"sqrt",  月数学库_数学_平方根_函},
  {"tan",   月数学库_数学_正切_函},
  {"type", 月数学库_数学_类型_函},
#if defined(配置_月亮_兼容_数学库_宏名)
  {"atan2", 月数学库_数学_反正切_函},
  {"cosh",   math_cosh},
  {"sinh",   math_sinh},
  {"tanh",   math_tanh},
  {"pow",   math_pow},
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
  {"log10", math_log10},
#endif
  /* placeholders */
  {"random", NULL},
  {"randomseed", NULL},
  {"圆周率_变量", NULL},
  {"huge", NULL},
  {"maxinteger", NULL},
  {"mininteger", NULL},
  {NULL, NULL}
};


/*
** Open math library
*/
配置_月亮模块_应程接_宏名 int 月亮打开数学_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 数学库_变量);
  月亮推入数目_函(L, PI);
  月亮设置字段_函(L, -2, "圆周率_变量");
  月亮推入数目_函(L, (炉_数目型)HUGE_VAL);
  月亮设置字段_函(L, -2, "huge");
  月亮推入整数_函(L, 配置_月亮_最大整数_宏名);
  月亮设置字段_函(L, -2, "maxinteger");
  月亮推入整数_函(L, 配置_月亮_最小整数_宏名);
  月亮设置字段_函(L, -2, "mininteger");
  月数学库_设置随机函_函(L);
  return 1;
}

