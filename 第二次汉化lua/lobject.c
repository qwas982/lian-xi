/*
** $Id: lobject.c $
** Some generic functions over Lua objects
** See Copyright Notice in lua.h
*/

#define lobject_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <locale.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lctype.h"
#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"
#include "lvm.h"


/*
** Computes ceil(log2(x))
*/
int 月亮对象_向上取整对数2_函 (unsigned int x) {
  static const 路_字节型 对数2_变量[256] = {  /* 对数2_变量[i] = ceil(log2(i - 1)) */
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  int l = 0;
  x--;
  while (x >= 256) { l += 8; x >>= 8; }
  return l + 对数2_变量[x];
}


static 炉_整数型 月对象_整型算术_函 (炉_状态机结 *L, int 操作_短变量, 炉_整数型 变1_缩变量,
                                                   炉_整数型 变2_缩变量) {
  switch (操作_短变量) {
    case 月头_月亮_操作加法_宏名: return 虚机头_整型操作_宏名(+, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作减法_宏名:return 虚机头_整型操作_宏名(-, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作乘法_宏名:return 虚机头_整型操作_宏名(*, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作取模_宏名: return 月亮虚拟机_取模_函(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作整数除_宏名: return 月亮虚拟机_整数除法_函(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作按位与_宏名: return 虚机头_整型操作_宏名(&, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作按位或_宏名: return 虚机头_整型操作_宏名(|, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作按位异或_宏名: return 虚机头_整型操作_宏名(^, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作左移_宏名: return 月亮虚拟机_左移_函(变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作右移_宏名: return 虚机头_月亮V_右移_宏名(变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作负号_宏名: return 虚机头_整型操作_宏名(-, 0, 变1_缩变量);
    case 月头_月亮_操作按位非_宏名: return 虚机头_整型操作_宏名(^, ~限制_l_类型转换符到无符_宏名(0), 变1_缩变量);
    default: 限制_月亮_断言_宏名(0); return 0;
  }
}


static 炉_数目型 月对象_数目算术_函 (炉_状态机结 *L, int 操作_短变量, 炉_数目型 变1_缩变量,
                                                  炉_数目型 变2_缩变量) {
  switch (操作_短变量) {
    case 月头_月亮_操作加法_宏名: return 限制_月亮i_数目加法_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作减法_宏名: return 限制_月亮i_数目减法_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作乘法_宏名: return 限制_月亮i_数目乘法_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作除法_宏名: return 限制_月亮i_数目除法_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作幂运算_宏名: return 限制_月亮i_数目幂_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作整数除_宏名: return 限制_月亮i_数目整数除法_宏名(L, 变1_缩变量, 变2_缩变量);
    case 月头_月亮_操作负号_宏名: return 限制_月亮i_数目取负_宏名(L, 变1_缩变量);
    case 月头_月亮_操作取模_宏名: return 月亮虚拟机_浮点数取整_函(L, 变1_缩变量, 变2_缩变量);
    default: 限制_月亮_断言_宏名(0); return 0;
  }
}


int 月亮对象_原始算术_函 (炉_状态机结 *L, int 操作_短变量, const 标签值_结 *p1, const 标签值_结 *p2,
                   标签值_结 *结果_短变量) {
  switch (操作_短变量) {
    case 月头_月亮_操作按位与_宏名: case 月头_月亮_操作按位或_宏名: case 月头_月亮_操作按位异或_宏名:
    case 月头_月亮_操作左移_宏名: case 月头_月亮_操作右移_宏名:
    case 月头_月亮_操作按位非_宏名: {  /* operate only on integers */
      炉_整数型 整1_缩变量; 炉_整数型 索2_缩变量;
      if (虚机头_到整数非安全_宏名(p1, &整1_缩变量) && 虚机头_到整数非安全_宏名(p2, &索2_缩变量)) {
        对象_设置整数值_宏名(结果_短变量, 月对象_整型算术_函(L, 操作_短变量, 整1_缩变量, 索2_缩变量));
        return 1;
      }
      else return 0;  /* fail */
    }
    case 月头_月亮_操作除法_宏名: case 月头_月亮_操作幂运算_宏名: {  /* operate only on floats */
      炉_数目型 数1_缩变量; 炉_数目型 数2_缩变量;
      if (虚机头_到数目非安全_宏名(p1, 数1_缩变量) && 虚机头_到数目非安全_宏名(p2, 数2_缩变量)) {
        对象_设置浮点值_宏名(结果_短变量, 月对象_数目算术_函(L, 操作_短变量, 数1_缩变量, 数2_缩变量));
        return 1;
      }
      else return 0;  /* fail */
    }
    default: {  /* other operations */
      炉_数目型 数1_缩变量; 炉_数目型 数2_缩变量;
      if (对象_tt是否整数_宏名(p1) && 对象_tt是否整数_宏名(p2)) {
        对象_设置整数值_宏名(结果_短变量, 月对象_整型算术_函(L, 操作_短变量, 对象_整数值_宏名(p1), 对象_整数值_宏名(p2)));
        return 1;
      }
      else if (虚机头_到数目非安全_宏名(p1, 数1_缩变量) && 虚机头_到数目非安全_宏名(p2, 数2_缩变量)) {
        对象_设置浮点值_宏名(结果_短变量, 月对象_数目算术_函(L, 操作_短变量, 数1_缩变量, 数2_缩变量));
        return 1;
      }
      else return 0;  /* fail */
    }
  }
}


void 月亮对象_算术_函 (炉_状态机结 *L, int 操作_短变量, const 标签值_结 *p1, const 标签值_结 *p2,
                 栈身份_型 结果_短变量) {
  if (!月亮对象_原始算术_函(L, 操作_短变量, p1, p2, 对象_s到v_宏名(结果_短变量))) {
    /* could not perform raw operation; try metamethod */
    月亮类型_尝试二元标签方法_函(L, p1, p2, 结果_短变量, 限制_类型转换_宏名(标方符_枚举, (操作_短变量 - 月头_月亮_操作加法_宏名) + 标方_加大写));
  }
}


int 月亮对象_十六进制值_函 (int c) {
  if (c类型_l是否数字_宏名(c)) return c - '0';
  else return (c类型_l到小写_宏名(c) - 'a') + 10;
}


static int 月对象_是否负数_函 (const char **s) {
  if (**s == '-') { (*s)++; return 1; }
  else if (**s == '+') (*s)++;
  return 0;
}



/*
** {==================================================================
** Lua's implementation for '月对象_月亮_串x到数目_函'
** ===================================================================
*/

#if !defined(月对象_月亮_串x到数目_函)

/* maximum number of significant digits 到_变量 read (到_变量 avoid overflows
   even with single floats) */
#define 对象_最大有效数字_宏名	30

/*
** convert a hexadecimal numeric string 到_变量 a number, following
** C99 specification for 'strtod'
*/
static 炉_数目型 月对象_月亮_串x到数目_函 (const char *s, char **终指针_变量) {
  int 点_变量 = 配置_月亮_获取本地化小数点_宏名();
  炉_数目型 r = 配置_数学操作_宏名(0.0);  /* 结果_变量 (accumulator) */
  int 符号数_缩变量 = 0;  /* number of significant digits */
  int 无符号数_缩变量 = 0;  /* number of non-significant digits */
  int e = 0;  /* exponent correction */
  int 负号_短变量;  /* 1 if number is negative */
  int 有点_变量 = 0;  /* true after seen a 点_变量 */
  *终指针_变量 = 限制_类型转换_印刻指针_宏名(s);  /* nothing is 有效_变量 yet */
  while (c类型_l是否空格_宏名(限制_类型转换_无符印刻_宏名(*s))) s++;  /* skip initial spaces */
  负号_短变量 = 月对象_是否负数_函(&s);  /* 月解析器_检查_函 sign */
  if (!(*s == '0' && (*(s + 1) == 'x' || *(s + 1) == 'X')))  /* 月解析器_检查_函 '0x' */
    return 配置_数学操作_宏名(0.0);  /* invalid 格式_变量 (no '0x') */
  for (s += 2; ; s++) {  /* skip '0x' and read numeral */
    if (*s == 点_变量) {
      if (有点_变量) break;  /* second 点_变量? stop 环_变量 */
      else 有点_变量 = 1;
    }
    else if (c类型_l是否十六进制数字_宏名(限制_类型转换_无符印刻_宏名(*s))) {
      if (符号数_缩变量 == 0 && *s == '0')  /* non-significant 数字_变量 (zero)? */
        无符号数_缩变量++;
      else if (++符号数_缩变量 <= 对象_最大有效数字_宏名)  /* can read it without overflow? */
          r = (r * 配置_数学操作_宏名(16.0)) + 月亮对象_十六进制值_函(*s);
      else e++; /* too many digits; ignore, but still 计数_变量 for exponent */
      if (有点_变量) e--;  /* decimal 数字_变量? correct exponent */
    }
    else break;  /* neither a 点_变量 nor a 数字_变量 */
  }
  if (无符号数_缩变量 + 符号数_缩变量 == 0)  /* no digits? */
    return 配置_数学操作_宏名(0.0);  /* invalid 格式_变量 */
  *终指针_变量 = 限制_类型转换_印刻指针_宏名(s);  /* 有效_变量 上_小变量 到_变量 here */
  e *= 4;  /* each 数字_变量 multiplies/divides 值_圆 by 2^4 */
  if (*s == 'p' || *s == 'P') {  /* exponent part? */
    int 表达式1_变量 = 0;  /* exponent 值_圆 */
    int 负号1_变量;  /* exponent sign */
    s++;  /* skip 'p' */
    负号1_变量 = 月对象_是否负数_函(&s);  /* sign */
    if (!c类型_l是否数字_宏名(限制_类型转换_无符印刻_宏名(*s)))
      return 配置_数学操作_宏名(0.0);  /* invalid; must have at least one 数字_变量 */
    while (c类型_l是否数字_宏名(限制_类型转换_无符印刻_宏名(*s)))  /* read exponent */
      表达式1_变量 = 表达式1_变量 * 10 + *(s++) - '0';
    if (负号1_变量) 表达式1_变量 = -表达式1_变量;
    e += 表达式1_变量;
    *终指针_变量 = 限制_类型转换_印刻指针_宏名(s);  /* 有效_变量 上_小变量 到_变量 here */
  }
  if (负号_短变量) r = -r;
  return 配置_数学操作_宏名(ldexp)(r, e);
}

#endif
/* }====================================================== */


/* maximum length of a numeral 到_变量 be converted 到_变量 a number */
#if !defined (L_最大长度数大写)
#define L_最大长度数大写	200
#endif

/*
** Convert string 's' 到_变量 a Lua number (put in '结果_变量'). Return NULL on
** fail or the address of the ending '\0' on 成功_变量. ('模块_变量' == 'x')
** means a hexadecimal numeral.
*/
static const char *月对象_l_串到双浮位置_函 (const char *s, 炉_数目型 *结果_变量, int 模块_变量) {
  char *终指针_变量;
  *结果_变量 = (模块_变量 == 'x') ? 月对象_月亮_串x到数目_函(s, &终指针_变量)  /* try 到_变量 convert */
                          : 配置_月亮_串到数目_宏名(s, &终指针_变量);
  if (终指针_变量 == s) return NULL;  /* nothing recognized? */
  while (c类型_l是否空格_宏名(限制_类型转换_无符印刻_宏名(*终指针_变量))) 终指针_变量++;  /* skip trailing spaces */
  return (*终指针_变量 == '\0') ? 终指针_变量 : NULL;  /* OK iff no trailing chars */
}


/*
** Convert string 's' 到_变量 a Lua number (put in '结果_变量') handling the
** 当前_圆 locale.
** This function accepts both the 当前_圆 locale or a 点_变量 as the radix
** 记号_变量. If the conversion fails, it may mean number has a 点_变量 but
** locale accepts something else. In that case, the 代码_变量 copies 's'
** 到_变量 a 缓冲区_变量 (because 's' is read-only), changes the 点_变量 到_变量 the
** 当前_圆 locale radix 记号_变量, and tries 到_变量 convert again.
** The variable '模块_变量' checks for special characters in the string:
** - 'n' means 'inf' or 'nan' (which should be rejected)
** - 'x' means a hexadecimal numeral
** - '.' just optimizes the search for the common case (no special chars)
*/
static const char *月对象_l_串到双浮_函 (const char *s, 炉_数目型 *结果_变量) {
  const char *终指针_变量;
  const char *保护模式_变量 = strpbrk(s, ".xXnN");  /* look for special chars */
  int 模块_变量 = 保护模式_变量 ? c类型_l到小写_宏名(限制_类型转换_无符印刻_宏名(*保护模式_变量)) : 0;
  if (模块_变量 == 'n')  /* reject 'inf' and 'nan' */
    return NULL;
  终指针_变量 = 月对象_l_串到双浮位置_函(s, 结果_变量, 模块_变量);  /* try 到_变量 convert */
  if (终指针_变量 == NULL) {  /* failed? may be a different locale */
    char 缓冲_变量[L_最大长度数大写 + 1];
    const char *保护点_变量 = strchr(s, '.');
    if (保护点_变量 == NULL || strlen(s) > L_最大长度数大写)
      return NULL;  /* string too long or no 点_变量; fail */
    strcpy(缓冲_变量, s);  /* copy string 到_变量 缓冲区_变量 */
    缓冲_变量[保护点_变量 - s] = 配置_月亮_获取本地化小数点_宏名();  /* correct decimal 针点_变量 */
    终指针_变量 = 月对象_l_串到双浮位置_函(缓冲_变量, 结果_变量, 模块_变量);  /* try again */
    if (终指针_变量 != NULL)
      终指针_变量 = s + (终指针_变量 - 缓冲_变量);  /* make relative 到_变量 's' */
  }
  return 终指针_变量;
}


#define 对象_最大除以十_宏名		限制_类型转换_宏名(lua_Unsigned, 配置_月亮_最大整数_宏名 / 10)
#define 对象_最大最后数字_宏名	限制_类型转换_整型_宏名(配置_月亮_最大整数_宏名 % 10)

static const char *月对象_l_串到整型_函 (const char *s, 炉_整数型 *结果_变量) {
  lua_Unsigned a = 0;
  int 空容器_变量 = 1;
  int 负号_短变量;
  while (c类型_l是否空格_宏名(限制_类型转换_无符印刻_宏名(*s))) s++;  /* skip initial spaces */
  负号_短变量 = 月对象_是否负数_函(&s);
  if (s[0] == '0' &&
      (s[1] == 'x' || s[1] == 'X')) {  /* 十六进制_变量? */
    s += 2;  /* skip '0x' */
    for (; c类型_l是否十六进制数字_宏名(限制_类型转换_无符印刻_宏名(*s)); s++) {
      a = a * 16 + 月亮对象_十六进制值_函(*s);
      空容器_变量 = 0;
    }
  }
  else {  /* decimal */
    for (; c类型_l是否数字_宏名(限制_类型转换_无符印刻_宏名(*s)); s++) {
      int d = *s - '0';
      if (a >= 对象_最大除以十_宏名 && (a > 对象_最大除以十_宏名 || d > 对象_最大最后数字_宏名 + 负号_短变量))  /* overflow? */
        return NULL;  /* do not accept it (as integer) */
      a = a * 10 + d;
      空容器_变量 = 0;
    }
  }
  while (c类型_l是否空格_宏名(限制_类型转换_无符印刻_宏名(*s))) s++;  /* skip trailing spaces */
  if (空容器_变量 || *s != '\0') return NULL;  /* something wrong in the numeral */
  else {
    *结果_变量 = 限制_l_类型转换无符到符_宏名((负号_短变量) ? 0u - a : a);
    return s;
  }
}


size_t 月亮对象_字符串到数目_函 (const char *s, 标签值_结 *o) {
  炉_整数型 i; 炉_数目型 n;
  const char *e;
  if ((e = 月对象_l_串到整型_函(s, &i)) != NULL) {  /* try as an integer */
    对象_设置整数值_宏名(o, i);
  }
  else if ((e = 月对象_l_串到双浮_函(s, &n)) != NULL) {  /* else try as a float */
    对象_设置浮点值_宏名(o, n);
  }
  else
    return 0;  /* conversion failed */
  return (e - s) + 1;  /* 成功_变量; return string 大小_变量 */
}


int 月亮对象_utf8转义_函 (char *缓冲_变量, unsigned long x) {
  int n = 1;  /* number of bytes put in 缓冲区_变量 (backwards) */
  限制_月亮_断言_宏名(x <= 0x7FFFFFFFu);
  if (x < 0x80)  /* ascii? */
    缓冲_变量[对象_UTF8缓冲大小_宏名 - 1] = 限制_类型转换_印刻_宏名(x);
  else {  /* need continuation bytes */
    unsigned int 基本元表_缩变量 = 0x3f;  /* maximum that fits in 首先_变量 byte */
    do {  /* add continuation bytes */
      缓冲_变量[对象_UTF8缓冲大小_宏名 - (n++)] = 限制_类型转换_印刻_宏名(0x80 | (x & 0x3f));
      x >>= 6;  /* remove added bits */
      基本元表_缩变量 >>= 1;  /* now there is one less bit available in 首先_变量 byte */
    } while (x > 基本元表_缩变量);  /* still needs continuation byte? */
    缓冲_变量[对象_UTF8缓冲大小_宏名 - n] = 限制_类型转换_印刻_宏名((~基本元表_缩变量 << 1) | x);  /* add 首先_变量 byte */
  }
  return n;
}


/*
** Maximum length of the conversion of a number 到_变量 a string. Must be
** enough 到_变量 accommodate both 配置_月亮_整数_格式_宏名 and 配置_月亮_数目_格式_宏名.
** (For a long long int, this is 19 digits plus a sign and a 最终_变量 '\0',
** adding 到_变量 21. For a long double, it can go 到_变量 a sign, 33 digits,
** the 点_变量, an exponent letter, an exponent sign, 5 exponent digits,
** and a 最终_变量 '\0', adding 到_变量 43.)
*/
#define 对象_最大数目到串_宏名	44


/*
** Convert a number object 到_变量 a string, adding it 到_变量 a 缓冲区_变量
*/
static int 月对象_到字符串缓冲_函 (标签值_结 *对象_变量, char *缓冲_变量) {
  int 长度_短变量;
  限制_月亮_断言_宏名(对象_tt是否数目_宏名(对象_变量));
  if (对象_tt是否整数_宏名(对象_变量))
    长度_短变量 = 配置_月亮_整数到串_宏名(缓冲_变量, 对象_最大数目到串_宏名, 对象_整数值_宏名(对象_变量));
  else {
    长度_短变量 = 配置_月亮_数目到串_宏名(缓冲_变量, 对象_最大数目到串_宏名, 对象_浮点值_宏名(对象_变量));
    if (缓冲_变量[strspn(缓冲_变量, "-0123456789")] == '\0') {  /* looks like an int? */
      缓冲_变量[长度_短变量++] = 配置_月亮_获取本地化小数点_宏名();
      缓冲_变量[长度_短变量++] = '0';  /* adds '.0' 到_变量 结果_变量 */
    }
  }
  return 长度_短变量;
}


/*
** Convert a number object 到_变量 a Lua string, replacing the 值_圆 at '对象_变量'
*/
void 月亮对象_到字符串_函 (炉_状态机结 *L, 标签值_结 *对象_变量) {
  char 缓冲_变量[对象_最大数目到串_宏名];
  int 长度_短变量 = 月对象_到字符串缓冲_函(对象_变量, 缓冲_变量);
  对象_设置ts值_宏名(L, 对象_变量, 月亮字符串_新长串_函(L, 缓冲_变量, 长度_短变量));
}




/*
** {==================================================================
** '月亮对象_推入可变格式化字符串_函'
** ===================================================================
*/

/*
** Size for 缓冲区_变量 空间_圆 used by '月亮对象_推入可变格式化字符串_函'. It should be
** (炉_身份大小 + 对象_最大数目到串_宏名) + a minimal 空间_圆 for basic messages,
** so that '月亮全局_添加信息_函' can 工作_变量 directly on the 缓冲区_变量.
*/
#define 对象_缓冲VFS_宏名		(炉_身份大小 + 对象_最大数目到串_宏名 + 95)

/* 缓冲区_变量 used by '月亮对象_推入可变格式化字符串_函' */
typedef struct 缓冲格串_结 {
  炉_状态机结 *L;
  int 已推入_小写;  /* true if there is a part of the 结果_变量 on the 栈_圆小 */
  int b长_短;  /* length of partial string in '空间_圆' */
  char 空间_圆[对象_缓冲VFS_宏名];  /* holds 最后_变量 part of the 结果_变量 */
} 缓冲格串_结;


/*
** Push given string 到_变量 the 栈_圆小, as part of the 结果_变量, and
** join it 到_变量 前一个_变量 partial 结果_变量 if there is one.
** It may call '月亮虚拟机_拼接_函' while using one 插槽_变量 from 状态机_额外_栈_宏名.
** This call 月编译器_不能_函 invoke metamethods, as both operands must be
** strings. It can, however, raise an 错误_小变量 if the 结果_变量 is too
** long. In that case, '月亮虚拟机_拼接_函' frees the 额外_变量 插槽_变量 before
** raising the 错误_小变量.
*/
static void 月对象_推串_函 (缓冲格串_结 *缓冲_变量, const char *串_变量, size_t lstr) {
  炉_状态机结 *L = 缓冲_变量->L;
  对象_设置ts值到s_宏名(L, L->顶部_变量.p, 月亮字符串_新长串_函(L, 串_变量, lstr));
  L->顶部_变量.p++;  /* may use one 插槽_变量 from 状态机_额外_栈_宏名 */
  if (!缓冲_变量->已推入_小写)  /* no 前一个_变量 string on the 栈_圆小? */
    缓冲_变量->已推入_小写 = 1;  /* now there is one */
  else  /* join 前一个_变量 string with new one */
    月亮虚拟机_拼接_函(L, 2);
}


/*
** 空容器_变量 the 缓冲区_变量 空间_圆 into the 栈_圆小
*/
static void 月对象_清除缓冲_函 (缓冲格串_结 *缓冲_变量) {
  月对象_推串_函(缓冲_变量, 缓冲_变量->空间_圆, 缓冲_变量->b长_短);  /* push 缓冲区_变量 内容_小写 */
  缓冲_变量->b长_短 = 0;  /* 空间_圆 now is 空容器_变量 */
}


/*
** Get a 空间_圆 of 大小_变量 '大小_短变量' in the 缓冲区_变量. If 缓冲区_变量 has not enough
** 空间_圆, 空容器_变量 it. '大小_短变量' must fit in an 空容器_变量 缓冲区_变量.
*/
static char *月对象_获取缓冲_函 (缓冲格串_结 *缓冲_变量, int 大小_短变量) {
  限制_月亮_断言_宏名(缓冲_变量->b长_短 <= 对象_缓冲VFS_宏名); 限制_月亮_断言_宏名(大小_短变量 <= 对象_缓冲VFS_宏名);
  if (大小_短变量 > 对象_缓冲VFS_宏名 - 缓冲_变量->b长_短)  /* not enough 空间_圆? */
    月对象_清除缓冲_函(缓冲_变量);
  return 缓冲_变量->空间_圆 + 缓冲_变量->b长_短;
}


#define 对象_添加大小_宏名(b,大小_短变量)	((b)->b长_短 += (大小_短变量))


/*
** Add '串_变量' 到_变量 the 缓冲区_变量. If string is larger than the 缓冲区_变量 空间_圆,
** push the string directly 到_变量 the 栈_圆小.
*/
static void 月对象_添加串到缓冲_函 (缓冲格串_结 *缓冲_变量, const char *串_变量, size_t 串长_缩变量) {
  if (串长_缩变量 <= 对象_缓冲VFS_宏名) {  /* does string fit into 缓冲区_变量? */
    char *基函_缩变量 = 月对象_获取缓冲_函(缓冲_变量, 限制_类型转换_整型_宏名(串长_缩变量));
    memcpy(基函_缩变量, 串_变量, 串长_缩变量);  /* add string 到_变量 缓冲区_变量 */
    对象_添加大小_宏名(缓冲_变量, 限制_类型转换_整型_宏名(串长_缩变量));
  }
  else {  /* string larger than 缓冲区_变量 */
    月对象_清除缓冲_函(缓冲_变量);  /* string comes after 缓冲区_变量's content */
    月对象_推串_函(缓冲_变量, 串_变量, 串长_缩变量);  /* push string */
  }
}


/*
** Add a numeral 到_变量 the 缓冲区_变量.
*/
static void 月对象_添加数目到缓冲_函 (缓冲格串_结 *缓冲_变量, 标签值_结 *数目_短变量) {
  char *数目缓冲_短变量 = 月对象_获取缓冲_函(缓冲_变量, 对象_最大数目到串_宏名);
  int 长度_短变量 = 月对象_到字符串缓冲_函(数目_短变量, 数目缓冲_短变量);  /* 格式_变量 number into '数目缓冲_短变量' */
  对象_添加大小_宏名(缓冲_变量, 长度_短变量);
}


/*
** this function handles only '%d', '%c', '%f', '%p', '%s', and '%%'
   conventional formats, plus Lua-specific '%I' and '%U'
*/
const char *月亮对象_推入可变格式化字符串_函 (炉_状态机结 *L, const char *格式_短变量, va_list argp) {
  缓冲格串_结 缓冲_变量;  /* holds 最后_变量 part of the 结果_变量 */
  const char *e;  /* points 到_变量 下一个_变量 '%' */
  缓冲_变量.已推入_小写 = 缓冲_变量.b长_短 = 0;
  缓冲_变量.L = L;
  while ((e = strchr(格式_短变量, '%')) != NULL) {
    月对象_添加串到缓冲_函(&缓冲_变量, 格式_短变量, e - 格式_短变量);  /* add '格式_短变量' 上_小变量 到_变量 '%' */
    switch (*(e + 1)) {  /* conversion specifier */
      case 's': {  /* zero-terminated string */
        const char *s = va_arg(argp, char *);
        if (s == NULL) s = "(null)";
        月对象_添加串到缓冲_函(&缓冲_变量, s, strlen(s));
        break;
      }
      case 'c': {  /* an 'int' as a character */
        char c = 限制_类型转换_无符印刻_宏名(va_arg(argp, int));
        月对象_添加串到缓冲_函(&缓冲_变量, &c, sizeof(char));
        break;
      }
      case 'd': {  /* an 'int' */
        标签值_结 数目_短变量;
        对象_设置整数值_宏名(&数目_短变量, va_arg(argp, int));
        月对象_添加数目到缓冲_函(&缓冲_变量, &数目_短变量);
        break;
      }
      case 'I': {  /* a '炉_整数型' */
        标签值_结 数目_短变量;
        对象_设置整数值_宏名(&数目_短变量, 限制_类型转换_宏名(炉_整数型, va_arg(argp, l_uacInt)));
        月对象_添加数目到缓冲_函(&缓冲_变量, &数目_短变量);
        break;
      }
      case 'f': {  /* a '炉_数目型' */
        标签值_结 数目_短变量;
        对象_设置浮点值_宏名(&数目_短变量, 限制_类型转换_数目_宏名(va_arg(argp, l_uacNumber)));
        月对象_添加数目到缓冲_函(&缓冲_变量, &数目_短变量);
        break;
      }
      case 'p': {  /* a pointer */
        const int 大小_短变量 = 3 * sizeof(void*) + 8; /* enough 空间_圆 for '%p' */
        char *基函_缩变量 = 月对象_获取缓冲_函(&缓冲_变量, 大小_短变量);
        void *p = va_arg(argp, void *);
        int 长度_短变量 = 配置_月亮_指针到串_宏名(基函_缩变量, 大小_短变量, p);
        对象_添加大小_宏名(&缓冲_变量, 长度_短变量);
        break;
      }
      case 'U': {  /* a 'long' as a UTF-8 sequence */
        char 基函_缩变量[对象_UTF8缓冲大小_宏名];
        int 长度_短变量 = 月亮对象_utf8转义_函(基函_缩变量, va_arg(argp, long));
        月对象_添加串到缓冲_函(&缓冲_变量, 基函_缩变量 + 对象_UTF8缓冲大小_宏名 - 长度_短变量, 长度_短变量);
        break;
      }
      case '%': {
        月对象_添加串到缓冲_函(&缓冲_变量, "%", 1);
        break;
      }
      default: {
        月亮全局_跑错误_函(L, "invalid 选项_变量 '%%%c' 到_变量 '月亮推入格式化字符串_函'",
                         *(e + 1));
      }
    }
    格式_短变量 = e + 2;  /* skip '%' and the specifier */
  }
  月对象_添加串到缓冲_函(&缓冲_变量, 格式_短变量, strlen(格式_短变量));  /* rest of '格式_短变量' */
  月对象_清除缓冲_函(&缓冲_变量);  /* 空容器_变量 缓冲区_变量 into the 栈_圆小 */
  限制_月亮_断言_宏名(缓冲_变量.已推入_小写 == 1);
  return 对象_s值_宏名(对象_s到v_宏名(L->顶部_变量.p - 1));
}


const char *月亮对象_推入格式化字符串_函 (炉_状态机结 *L, const char *格式_短变量, ...) {
  const char *消息_缩变量;
  va_list argp;
  va_start(argp, 格式_短变量);
  消息_缩变量 = 月亮对象_推入可变格式化字符串_函(L, 格式_短变量, argp);
  va_end(argp);
  return 消息_缩变量;
}

/* }================================================================== */


#define 对象_返回值_宏名	"..."
#define 对象_前缀_宏名	"[string \""
#define 对象_后缀_宏名	"\"]"

#define addstr(a,b,l)	( memcpy(a,b,(l) * sizeof(char)), a += (l) )

void 月亮对象_大块id_函 (char *out, const char *源_圆, size_t 源的长_短) {
  size_t 缓冲长_变量 = 炉_身份大小;  /* free 空间_圆 in 缓冲区_变量 */
  if (*源_圆 == '=') {  /* 'literal' 源_圆 */
    if (源的长_短 <= 缓冲长_变量)  /* small enough? */
      memcpy(out, 源_圆 + 1, 源的长_短 * sizeof(char));
    else {  /* truncate it */
      addstr(out, 源_圆 + 1, 缓冲长_变量 - 1);
      *out = '\0';
    }
  }
  else if (*源_圆 == '@') {  /* file 名称_变量 */
    if (源的长_短 <= 缓冲长_变量)  /* small enough? */
      memcpy(out, 源_圆 + 1, 源的长_短 * sizeof(char));
    else {  /* add '...' before rest of 名称_变量 */
      addstr(out, 对象_返回值_宏名, LL(对象_返回值_宏名));
      缓冲长_变量 -= LL(对象_返回值_宏名);
      memcpy(out, 源_圆 + 1 + 源的长_短 - 缓冲长_变量, 缓冲长_变量 * sizeof(char));
    }
  }
  else {  /* string; 格式_变量 as [string "源_圆"] */
    const char *换行_数变量 = strchr(源_圆, '\n');  /* find 首先_变量 new 行_变量 (if any) */
    addstr(out, 对象_前缀_宏名, LL(对象_前缀_宏名));  /* add prefix */
    缓冲长_变量 -= LL(对象_前缀_宏名 对象_返回值_宏名 对象_后缀_宏名) + 1;  /* 月词法_保存_函 空间_圆 for prefix+suffix+'\0' */
    if (源的长_短 < 缓冲长_变量 && 换行_数变量 == NULL) {  /* small one-行_变量 源_圆? */
      addstr(out, 源_圆, 源的长_短);  /* keep it */
    }
    else {
      if (换行_数变量 != NULL) 源的长_短 = 换行_数变量 - 源_圆;  /* stop at 首先_变量 新行_小变量 */
      if (源的长_短 > 缓冲长_变量) 源的长_短 = 缓冲长_变量;
      addstr(out, 源_圆, 源的长_短);
      addstr(out, 对象_返回值_宏名, LL(对象_返回值_宏名));
    }
    memcpy(out, 对象_后缀_宏名, (LL(对象_后缀_宏名) + 1) * sizeof(char));
  }
}

