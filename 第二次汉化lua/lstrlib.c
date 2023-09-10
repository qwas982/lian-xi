/*
** $Id: lstrlib.c $
** Standard library for string operations and pattern-matching
** See Copyright Notice in lua.h
*/

#define lstrlib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/*
** maximum number of captures that a pattern can do during
** pattern-matching. This 限制_变量 is arbitrary, but must fit in
** an unsigned char.
*/
#if !defined(炉_最大捕获们)
#define 炉_最大捕获们		32
#endif


/* macro 到_变量 'unsign' a character */
#define uchar(c)	((unsigned char)(c))


/*
** Some sizes are better limited 到_变量 fit in 'int', but must also fit in
** 'size_t'. (We assume that '炉_整数型' 月编译器_不能_函 be smaller than 'int'.)
*/
#define 串库_最大_大小T_宏名	((size_t)(~(size_t)0))

#define 串库_最大大小_宏名  \
	(sizeof(size_t) < sizeof(int) ? 串库_最大_大小T_宏名 : (size_t)(INT_MAX))




static int 月串库_串_长度_函 (炉_状态机结 *L) {
  size_t l;
  月亮状态_检查状态字符串_函(L, 1, &l);
  月亮推入整数_函(L, (炉_整数型)l);
  return 1;
}


/*
** translate a relative initial string position
** (negative means back from 终_变量): clip 结果_变量 到_变量 [1, inf).
** The length of any string in Lua must fit in a 炉_整数型,
** so there are no overflows in the casts.
** The inverted comparison avoids a possible overflow
** computing '-位置_缩变量'.
*/
static size_t 月串库_相对位置索引_函 (炉_整数型 位置_缩变量, size_t 长度_短变量) {
  if (位置_缩变量 > 0)
    return (size_t)位置_缩变量;
  else if (位置_缩变量 == 0)
    return 1;
  else if (位置_缩变量 < -(炉_整数型)长度_短变量)  /* inverted comparison */
    return 1;  /* clip 到_变量 1 */
  else return 长度_短变量 + (size_t)位置_缩变量 + 1;
}


/*
** Gets an optional ending string position from argument '实参_短变量',
** with default 值_圆 'def'.
** Negative means back from 终_变量: clip 结果_变量 到_变量 [0, 长度_短变量]
*/
static size_t 月串库_获取终位置_函 (炉_状态机结 *L, int 实参_短变量, 炉_整数型 def,
                         size_t 长度_短变量) {
  炉_整数型 位置_缩变量 = 月亮状态_可选整数_函(L, 实参_短变量, def);
  if (位置_缩变量 > (炉_整数型)长度_短变量)
    return 长度_短变量;
  else if (位置_缩变量 >= 0)
    return (size_t)位置_缩变量;
  else if (位置_缩变量 < -(炉_整数型)长度_短变量)
    return 0;
  else return 长度_短变量 + (size_t)位置_缩变量 + 1;
}


static int 月串库_串_取代_函 (炉_状态机结 *L) {
  size_t l;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  size_t 起始_变量 = 月串库_相对位置索引_函(月亮状态_检查整数_函(L, 2), l);
  size_t 终_变量 = 月串库_获取终位置_函(L, 3, -1, l);
  if (起始_变量 <= 终_变量)
    月亮推入长字符串_函(L, s + 起始_变量 - 1, (终_变量 - 起始_变量) + 1);
  else 月头_月亮_推字面_宏名(L, "");
  return 1;
}


static int 月串库_串_逆向_函 (炉_状态机结 *L) {
  size_t l, i;
  炉L_缓冲区结 b;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  char *p = 月亮状态_缓冲初始的大小_函(L, &b, l);
  for (i = 0; i < l; i++)
    p[i] = s[l - i - 1];
  月亮状态_推入结果大小_函(&b, l);
  return 1;
}


static int 月串库_串_小写_函 (炉_状态机结 *L) {
  size_t l;
  size_t i;
  炉L_缓冲区结 b;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  char *p = 月亮状态_缓冲初始的大小_函(L, &b, l);
  for (i=0; i<l; i++)
    p[i] = tolower(uchar(s[i]));
  月亮状态_推入结果大小_函(&b, l);
  return 1;
}


static int 月串库_串_大写_函 (炉_状态机结 *L) {
  size_t l;
  size_t i;
  炉L_缓冲区结 b;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  char *p = 月亮状态_缓冲初始的大小_函(L, &b, l);
  for (i=0; i<l; i++)
    p[i] = toupper(uchar(s[i]));
  月亮状态_推入结果大小_函(&b, l);
  return 1;
}


static int 月串库_串_替换_函 (炉_状态机结 *L) {
  size_t l, 列分隔_变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  炉_整数型 n = 月亮状态_检查整数_函(L, 2);
  const char *分隔_变量 = 月亮状态_可选状态字符串_函(L, 3, "", &列分隔_变量);
  if (n <= 0)
    月头_月亮_推字面_宏名(L, "");
  else if (配置_l_可能性低_宏名(l + 列分隔_变量 < l || l + 列分隔_变量 > 串库_最大大小_宏名 / n))
    return 月亮状态_错误_函(L, "resulting string too large");
  else {
    size_t 总长_变量 = (size_t)n * l + (size_t)(n - 1) * 列分隔_变量;
    炉L_缓冲区结 b;
    char *p = 月亮状态_缓冲初始的大小_函(L, &b, 总长_变量);
    while (n-- > 1) {  /* 首先_变量 n-1 copies (followed by separator) */
      memcpy(p, s, l * sizeof(char)); p += l;
      if (列分隔_变量 > 0) {  /* 空容器_变量 'memcpy' is not that cheap */
        memcpy(p, 分隔_变量, 列分隔_变量 * sizeof(char));
        p += 列分隔_变量;
      }
    }
    memcpy(p, s, l * sizeof(char));  /* 最后_变量 copy (not followed by separator) */
    月亮状态_推入结果大小_函(&b, 总长_变量);
  }
  return 1;
}


static int 月串库_串_字节_函 (炉_状态机结 *L) {
  size_t l;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &l);
  炉_整数型 圆周率_变量 = 月亮状态_可选整数_函(L, 2, 1);
  size_t 位置i_变量 = 月串库_相对位置索引_函(圆周率_变量, l);
  size_t 位置e_变量 = 月串库_获取终位置_函(L, 3, 圆周率_变量, l);
  int n, i;
  if (位置i_变量 > 位置e_变量) return 0;  /* 空容器_变量 interval; return no values */
  if (配置_l_可能性低_宏名(位置e_变量 - 位置i_变量 >= (size_t)INT_MAX))  /* arithmetic overflow? */
    return 月亮状态_错误_函(L, "string slice too long");
  n = (int)(位置e_变量 -  位置i_变量) + 1;
  月亮状态_检查栈_函(L, n, "string slice too long");
  for (i=0; i<n; i++)
    月亮推入整数_函(L, uchar(s[位置i_变量+i-1]));
  return n;
}


static int 月串库_串_印刻_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  int i;
  炉L_缓冲区结 b;
  char *p = 月亮状态_缓冲初始的大小_函(L, &b, n);
  for (i=1; i<=n; i++) {
    lua_Unsigned c = (lua_Unsigned)月亮状态_检查整数_函(L, i);
    辅助库_月亮l_实参检查_宏名(L, c <= (lua_Unsigned)UCHAR_MAX, i, "值_圆 out of range");
    p[i - 1] = uchar(c);
  }
  月亮状态_推入结果大小_函(&b, n);
  return 1;
}


/*
** Buffer 到_变量 store the 结果_变量 of 'string.dump'. It must be initialized
** after the call 到_变量 '月亮_转储_函', 到_变量 ensure that the function is on the
** 顶部_变量 of the 栈_圆小 when '月亮_转储_函' is called. ('月亮状态_缓冲初始的_函' might
** push stuff.)
*/
struct 串_写器结 {
  int 初始的_变量;  /* true iff 缓冲区_变量 has been initialized */
  炉L_缓冲区结 B;
};


static int 写器_小写 (炉_状态机结 *L, const void *b, size_t 大小_变量, void *用数_缩变量) {
  struct 串_写器结 *状态机_变量 = (struct 串_写器结 *)用数_缩变量;
  if (!状态机_变量->初始的_变量) {
    状态机_变量->初始的_变量 = 1;
    月亮状态_缓冲初始的_函(L, &状态机_变量->B);
  }
  月亮状态_添加状态字符串_函(&状态机_变量->B, (const char *)b, 大小_变量);
  return 0;
}


static int 月串库_串_转储_函 (炉_状态机结 *L) {
  struct 串_写器结 状态机_变量;
  int 剥离_变量 = 月亮_到布尔值_函(L, 2);
  月亮状态_检查类型_函(L, 1, 月头_月亮_T函数_宏名);
  月亮_设置顶_函(L, 1);  /* ensure function is on the 顶部_变量 of the 栈_圆小 */
  状态机_变量.初始的_变量 = 0;
  if (配置_l_可能性低_宏名(月亮_转储_函(L, 写器_小写, &状态机_变量, 剥离_变量) != 0))
    return 月亮状态_错误_函(L, "unable 到_变量 dump given function");
  月亮状态_推入结果_函(&状态机_变量.B);
  return 1;
}



/*
** {======================================================
** METAMETHODS
** =======================================================
*/

#if defined(配置_月亮_不转化串到数目_宏名)	/* { */

/* no coercion from strings 到_变量 numbers */

static const 炉L_寄结 字符串元方法_变量[] = {
  {"__index", NULL},  /* placeholder */
  {NULL, NULL}
};

#else		/* }{ */

static int 月串库_到数目_函 (炉_状态机结 *L, int 实参_短变量) {
  if (月亮_类型_函(L, 实参_短变量) == 月头_月亮_T数目_宏名) {  /* already a number? */
    月亮_推入值_函(L, 实参_短变量);
    return 1;
  }
  else {  /* 月解析器_检查_函 whether it is a numerical string */
    size_t 长度_短变量;
    const char *s = 月亮_到长字符串_函(L, 实参_短变量, &长度_短变量);
    return (s != NULL && 月亮_字符串到数目_函(L, s) == 长度_短变量 + 1);
  }
}


static void 月串库_尝试元表_函 (炉_状态机结 *L, const char *mtname) {
  月亮_设置顶_函(L, 2);  /* back 到_变量 the original arguments */
  if (配置_l_可能性低_宏名(月亮_类型_函(L, 2) == 月头_月亮_T字符串_宏名 ||
                 !月亮状态_获取元字段_函(L, 2, mtname)))
    月亮状态_错误_函(L, "attempt 到_变量 %s a '%s' with a '%s'", mtname + 2,
                  辅助库_月亮l_类型名称_宏名(L, -2), 辅助库_月亮l_类型名称_宏名(L, -1));
  月头_月亮_插入_宏名(L, -3);  /* put metamethod before arguments */
  月头_月亮_调用_宏名(L, 2, 1);  /* call metamethod */
}


static int 月串库_算术_函 (炉_状态机结 *L, int 操作_短变量, const char *mtname) {
  if (月串库_到数目_函(L, 1) && 月串库_到数目_函(L, 2))
    月亮算术_函(L, 操作_短变量);  /* 结果_变量 will be on the 顶部_变量 */
  else
    月串库_尝试元表_函(L, mtname);
  return 1;
}


static int 月串库_算术相加_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作加法_宏名, "__add");
}

static int 月串库_算术相减_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作减法_宏名, "__sub");
}

static int 月串库_算术相乘_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作乘法_宏名, "__mul");
}

static int 月串库_算术取模_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作取模_宏名, "__mod");
}

static int 月串库_算术乘方_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作幂运算_宏名, "__pow");
}

static int 月串库_算术相除_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作除法_宏名, "__div");
}

static int 月串库_算术整数除_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作整数除_宏名, "__idiv");
}

static int 月串库_算术一元负号_函 (炉_状态机结 *L) {
  return 月串库_算术_函(L, 月头_月亮_操作负号_宏名, "__unm");
}


static const 炉L_寄结 字符串元方法_变量[] = {
  {"__add", 月串库_算术相加_函},
  {"__sub", 月串库_算术相减_函},
  {"__mul", 月串库_算术相乘_函},
  {"__mod", 月串库_算术取模_函},
  {"__pow", 月串库_算术乘方_函},
  {"__div", 月串库_算术相除_函},
  {"__idiv", 月串库_算术整数除_函},
  {"__unm", 月串库_算术一元负号_函},
  {"__index", NULL},  /* placeholder */
  {NULL, NULL}
};

#endif		/* } */

/* }====================================================== */

/*
** {======================================================
** PATTERN MATCHING
** =======================================================
*/


#define 串库_捕_未结束_宏名	(-1)
#define 串库_捕_位置_宏名	(-2)


typedef struct 匹配状态机_结 {
  const char *源_初始的短;  /* 初始的_变量 of 源_圆 string */
  const char *源_终短;  /* 终_变量 ('\0') of 源_圆 string */
  const char *模式_终缩;  /* 终_变量 ('\0') of pattern */
  炉_状态机结 *L;
  int 匹配深度_圆;  /* control for recursive depth (到_变量 avoid C 栈_圆小 overflow) */
  unsigned char 层级_变量;  /* 总数_变量 number of captures (finished or unfinished) */
  struct {
    const char *初始的_变量;
    ptrdiff_t 长度_短变量;
  } 捕获_结[炉_最大捕获们];
} 匹配状态机_结;


/* recursive function */
static const char *月串库_匹配_函 (匹配状态机_结 *毫秒_缩变量, const char *s, const char *p);


/* maximum recursion depth for '月串库_匹配_函' */
#if !defined(串库_最大C调用_宏名)
#define 串库_最大C调用_宏名	200
#endif


#define 串库_月_转义_宏名		'%'
#define 串库_特殊的_宏名	"^$*+?.([%-"


static int 月串库_检查_捕获_函 (匹配状态机_结 *毫秒_缩变量, int l) {
  l -= '1';
  if (配置_l_可能性低_宏名(l < 0 || l >= 毫秒_缩变量->层级_变量 ||
                 毫秒_缩变量->捕获_结[l].长度_短变量 == 串库_捕_未结束_宏名))
    return 月亮状态_错误_函(毫秒_缩变量->L, "invalid 捕获_结 index %%%d", l + 1);
  return l;
}


static int 月串库_捕获_到_关闭_函 (匹配状态机_结 *毫秒_缩变量) {
  int 层级_变量 = 毫秒_缩变量->层级_变量;
  for (层级_变量--; 层级_变量>=0; 层级_变量--)
    if (毫秒_缩变量->捕获_结[层级_变量].长度_短变量 == 串库_捕_未结束_宏名) return 层级_变量;
  return 月亮状态_错误_函(毫秒_缩变量->L, "invalid pattern 捕获_结");
}


static const char *月串库_类终_函 (匹配状态机_结 *毫秒_缩变量, const char *p) {
  switch (*p++) {
    case 串库_月_转义_宏名: {
      if (配置_l_可能性低_宏名(p == 毫秒_缩变量->模式_终缩))
        月亮状态_错误_函(毫秒_缩变量->L, "malformed pattern (ends with '%%')");
      return p+1;
    }
    case '[': {
      if (*p == '^') p++;
      do {  /* look for a ']' */
        if (配置_l_可能性低_宏名(p == 毫秒_缩变量->模式_终缩))
          月亮状态_错误_函(毫秒_缩变量->L, "malformed pattern (missing ']')");
        if (*(p++) == 串库_月_转义_宏名 && p < 毫秒_缩变量->模式_终缩)
          p++;  /* skip escapes (e.g. '%]') */
      } while (*p != ']');
      return p+1;
    }
    default: {
      return p;
    }
  }
}


static int 月串库_匹配_类_函 (int c, int 闭包_短变量) {
  int 结果_短变量;
  switch (tolower(闭包_短变量)) {
    case 'a' : 结果_短变量 = isalpha(c); break;
    case 'c' : 结果_短变量 = iscntrl(c); break;
    case 'd' : 结果_短变量 = isdigit(c); break;
    case 'g' : 结果_短变量 = isgraph(c); break;
    case 'l' : 结果_短变量 = islower(c); break;
    case 'p' : 结果_短变量 = ispunct(c); break;
    case 's' : 结果_短变量 = isspace(c); break;
    case 'u' : 结果_短变量 = isupper(c); break;
    case 'w' : 结果_短变量 = isalnum(c); break;
    case 'x' : 结果_短变量 = isxdigit(c); break;
    case 'z' : 结果_短变量 = (c == 0); break;  /* deprecated 选项_变量 */
    default: return (闭包_短变量 == c);
  }
  return (islower(闭包_短变量) ? 结果_短变量 : !结果_短变量);
}


static int 月串库_匹配括号类_函 (int c, const char *p, const char *ec) {
  int 信号_短变量 = 1;
  if (*(p+1) == '^') {
    信号_短变量 = 0;
    p++;  /* skip the '^' */
  }
  while (++p < ec) {
    if (*p == 串库_月_转义_宏名) {
      p++;
      if (月串库_匹配_类_函(c, uchar(*p)))
        return 信号_短变量;
    }
    else if ((*(p+1) == '-') && (p+2 < ec)) {
      p+=2;
      if (uchar(*(p-2)) <= c && c <= uchar(*p))
        return 信号_短变量;
    }
    else if (uchar(*p) == c) return 信号_短变量;
  }
  return !信号_短变量;
}


static int 月串库_单个匹配_函 (匹配状态机_结 *毫秒_缩变量, const char *s, const char *p,
                        const char *事件点_缩变量) {
  if (s >= 毫秒_缩变量->源_终短)
    return 0;
  else {
    int c = uchar(*s);
    switch (*p) {
      case '.': return 1;  /* matches any char */
      case 串库_月_转义_宏名: return 月串库_匹配_类_函(c, uchar(*(p+1)));
      case '[': return 月串库_匹配括号类_函(c, p, 事件点_缩变量-1);
      default:  return (uchar(*p) == c);
    }
  }
}


static const char *月串库_匹配平衡_函 (匹配状态机_结 *毫秒_缩变量, const char *s,
                                   const char *p) {
  if (配置_l_可能性低_宏名(p >= 毫秒_缩变量->模式_终缩 - 1))
    月亮状态_错误_函(毫秒_缩变量->L, "malformed pattern (missing arguments 到_变量 '%%b')");
  if (*s != *p) return NULL;
  else {
    int b = *p;
    int e = *(p+1);
    int 继续_短变量 = 1;
    while (++s < 毫秒_缩变量->源_终短) {
      if (*s == e) {
        if (--继续_短变量 == 0) return s+1;
      }
      else if (*s == b) 继续_短变量++;
    }
  }
  return NULL;  /* string ends out of balance */
}


static const char *月串库_最大_扩展_函 (匹配状态机_结 *毫秒_缩变量, const char *s,
                                 const char *p, const char *事件点_缩变量) {
  ptrdiff_t i = 0;  /* counts maximum expand for item */
  while (月串库_单个匹配_函(毫秒_缩变量, s + i, p, 事件点_缩变量))
    i++;
  /* keeps trying 到_变量 月串库_匹配_函 with the maximum repetitions */
  while (i>=0) {
    const char *结果_短变量 = 月串库_匹配_函(毫秒_缩变量, (s+i), 事件点_缩变量+1);
    if (结果_短变量) return 结果_短变量;
    i--;  /* else didn't 月串库_匹配_函; reduce 1 repetition 到_变量 try again */
  }
  return NULL;
}


static const char *月串库_最小_扩展_函 (匹配状态机_结 *毫秒_缩变量, const char *s,
                                 const char *p, const char *事件点_缩变量) {
  for (;;) {
    const char *结果_短变量 = 月串库_匹配_函(毫秒_缩变量, s, 事件点_缩变量+1);
    if (结果_短变量 != NULL)
      return 结果_短变量;
    else if (月串库_单个匹配_函(毫秒_缩变量, s, p, 事件点_缩变量))
      s++;  /* try with one 更多_变量 repetition */
    else return NULL;
  }
}


static const char *月串库_开始_捕获_函 (匹配状态机_结 *毫秒_缩变量, const char *s,
                                    const char *p, int 什么_变量) {
  const char *结果_短变量;
  int 层级_变量 = 毫秒_缩变量->层级_变量;
  if (层级_变量 >= 炉_最大捕获们) 月亮状态_错误_函(毫秒_缩变量->L, "too many captures");
  毫秒_缩变量->捕获_结[层级_变量].初始的_变量 = s;
  毫秒_缩变量->捕获_结[层级_变量].长度_短变量 = 什么_变量;
  毫秒_缩变量->层级_变量 = 层级_变量+1;
  if ((结果_短变量=月串库_匹配_函(毫秒_缩变量, s, p)) == NULL)  /* 月串库_匹配_函 failed? */
    毫秒_缩变量->层级_变量--;  /* undo 捕获_结 */
  return 结果_短变量;
}


static const char *月串库_终_捕获_函 (匹配状态机_结 *毫秒_缩变量, const char *s,
                                  const char *p) {
  int l = 月串库_捕获_到_关闭_函(毫秒_缩变量);
  const char *结果_短变量;
  毫秒_缩变量->捕获_结[l].长度_短变量 = s - 毫秒_缩变量->捕获_结[l].初始的_变量;  /* 关闭_圆 捕获_结 */
  if ((结果_短变量 = 月串库_匹配_函(毫秒_缩变量, s, p)) == NULL)  /* 月串库_匹配_函 failed? */
    毫秒_缩变量->捕获_结[l].长度_短变量 = 串库_捕_未结束_宏名;  /* undo 捕获_结 */
  return 结果_短变量;
}


static const char *月串库_匹配_捕获_函 (匹配状态机_结 *毫秒_缩变量, const char *s, int l) {
  size_t 长度_短变量;
  l = 月串库_检查_捕获_函(毫秒_缩变量, l);
  长度_短变量 = 毫秒_缩变量->捕获_结[l].长度_短变量;
  if ((size_t)(毫秒_缩变量->源_终短-s) >= 长度_短变量 &&
      memcmp(毫秒_缩变量->捕获_结[l].初始的_变量, s, 长度_短变量) == 0)
    return s+长度_短变量;
  else return NULL;
}


static const char *月串库_匹配_函 (匹配状态机_结 *毫秒_缩变量, const char *s, const char *p) {
  if (配置_l_可能性低_宏名(毫秒_缩变量->匹配深度_圆-- == 0))
    月亮状态_错误_函(毫秒_缩变量->L, "pattern too complex");
  初始的_变量: /* using goto 到_变量 optimize tail recursion */
  if (p != 毫秒_缩变量->模式_终缩) {  /* 终_变量 of pattern? */
    switch (*p) {
      case '(': {  /* 起始_变量 捕获_结 */
        if (*(p + 1) == ')')  /* position 捕获_结? */
          s = 月串库_开始_捕获_函(毫秒_缩变量, s, p + 2, 串库_捕_位置_宏名);
        else
          s = 月串库_开始_捕获_函(毫秒_缩变量, s, p + 1, 串库_捕_未结束_宏名);
        break;
      }
      case ')': {  /* 终_变量 捕获_结 */
        s = 月串库_终_捕获_函(毫秒_缩变量, s, p + 1);
        break;
      }
      case '$': {
        if ((p + 1) != 毫秒_缩变量->模式_终缩)  /* is the '$' the 最后_变量 char in pattern? */
          goto dflt;  /* no; go 到_变量 default */
        s = (s == 毫秒_缩变量->源_终短) ? s : NULL;  /* 月解析器_检查_函 终_变量 of string */
        break;
      }
      case 串库_月_转义_宏名: {  /* escaped sequences not in the 格式_变量 class[*+?-]? */
        switch (*(p + 1)) {
          case 'b': {  /* balanced string? */
            s = 月串库_匹配平衡_函(毫秒_缩变量, s, p + 2);
            if (s != NULL) {
              p += 4; goto 初始的_变量;  /* return 月串库_匹配_函(毫秒_缩变量, s, p + 4); */
            }  /* else fail (s == NULL) */
            break;
          }
          case 'f': {  /* frontier? */
            const char *事件点_缩变量; char 前一个_变量;
            p += 2;
            if (配置_l_可能性低_宏名(*p != '['))
              月亮状态_错误_函(毫秒_缩变量->L, "missing '[' after '%%f' in pattern");
            事件点_缩变量 = 月串库_类终_函(毫秒_缩变量, p);  /* points 到_变量 什么_变量 is 下一个_变量 */
            前一个_变量 = (s == 毫秒_缩变量->源_初始的短) ? '\0' : *(s - 1);
            if (!月串库_匹配括号类_函(uchar(前一个_变量), p, 事件点_缩变量 - 1) &&
               月串库_匹配括号类_函(uchar(*s), p, 事件点_缩变量 - 1)) {
              p = 事件点_缩变量; goto 初始的_变量;  /* return 月串库_匹配_函(毫秒_缩变量, s, 事件点_缩变量); */
            }
            s = NULL;  /* 月串库_匹配_函 failed */
            break;
          }
          case '0': case '1': case '2': case '3':
          case '4': case '5': case '6': case '7':
          case '8': case '9': {  /* 捕获_结 results (%0-%9)? */
            s = 月串库_匹配_捕获_函(毫秒_缩变量, s, uchar(*(p + 1)));
            if (s != NULL) {
              p += 2; goto 初始的_变量;  /* return 月串库_匹配_函(毫秒_缩变量, s, p + 2) */
            }
            break;
          }
          default: goto dflt;
        }
        break;
      }
      default: dflt: {  /* pattern class plus optional suffix */
        const char *事件点_缩变量 = 月串库_类终_函(毫秒_缩变量, p);  /* points 到_变量 optional suffix */
        /* does not 月串库_匹配_函 at least once? */
        if (!月串库_单个匹配_函(毫秒_缩变量, s, p, 事件点_缩变量)) {
          if (*事件点_缩变量 == '*' || *事件点_缩变量 == '?' || *事件点_缩变量 == '-') {  /* accept 空容器_变量? */
            p = 事件点_缩变量 + 1; goto 初始的_变量;  /* return 月串库_匹配_函(毫秒_缩变量, s, 事件点_缩变量 + 1); */
          }
          else  /* '+' or no suffix */
            s = NULL;  /* fail */
        }
        else {  /* matched once */
          switch (*事件点_缩变量) {  /* handle optional suffix */
            case '?': {  /* optional */
              const char *结果_短变量;
              if ((结果_短变量 = 月串库_匹配_函(毫秒_缩变量, s + 1, 事件点_缩变量 + 1)) != NULL)
                s = 结果_短变量;
              else {
                p = 事件点_缩变量 + 1; goto 初始的_变量;  /* else return 月串库_匹配_函(毫秒_缩变量, s, 事件点_缩变量 + 1); */
              }
              break;
            }
            case '+':  /* 1 or 更多_变量 repetitions */
              s++;  /* 1 月串库_匹配_函 already done */
              /* FALLTHROUGH */
            case '*':  /* 0 or 更多_变量 repetitions */
              s = 月串库_最大_扩展_函(毫秒_缩变量, s, p, 事件点_缩变量);
              break;
            case '-':  /* 0 or 更多_变量 repetitions (minimum) */
              s = 月串库_最小_扩展_函(毫秒_缩变量, s, p, 事件点_缩变量);
              break;
            default:  /* no suffix */
              s++; p = 事件点_缩变量; goto 初始的_变量;  /* return 月串库_匹配_函(毫秒_缩变量, s + 1, 事件点_缩变量); */
          }
        }
        break;
      }
    }
  }
  毫秒_缩变量->匹配深度_圆++;
  return s;
}



static const char *月串库_l内存找_函 (const char *s1, size_t l1,
                               const char *s2, size_t l2) {
  if (l2 == 0) return s1;  /* 空容器_变量 strings are everywhere */
  else if (l2 > l1) return NULL;  /* avoids a negative 'l1' */
  else {
    const char *初始的_变量;  /* 到_变量 search for a '*s2' inside 's1' */
    l2--;  /* 1st char will be checked by 'memchr' */
    l1 = l1-l2;  /* 's2' 月编译器_不能_函 be found after that */
    while (l1 > 0 && (初始的_变量 = (const char *)memchr(s1, *s2, l1)) != NULL) {
      初始的_变量++;   /* 1st char is already checked */
      if (memcmp(初始的_变量, s2+1, l2) == 0)
        return 初始的_变量-1;
      else {  /* correct 'l1' and 's1' 到_变量 try again */
        l1 -= 初始的_变量-s1;
        s1 = 初始的_变量;
      }
    }
    return NULL;  /* not found */
  }
}


/*
** get information about the i-到钩_变量 捕获_结. If there are no captures
** and 'i==0', return information about the whole 月串库_匹配_函, which
** is the range 's'..'e'. If the 捕获_结 is a string, return
** its length and put its address in '*容量_短变量'. If it is an integer
** (a position), push it on the 栈_圆小 and return 串库_捕_位置_宏名.
*/
static size_t 月串库_获取_一个捕获_函 (匹配状态机_结 *毫秒_缩变量, int i, const char *s,
                              const char *e, const char **容量_短变量) {
  if (i >= 毫秒_缩变量->层级_变量) {
    if (配置_l_可能性低_宏名(i != 0))
      月亮状态_错误_函(毫秒_缩变量->L, "invalid 捕获_结 index %%%d", i + 1);
    *容量_短变量 = s;
    return e - s;
  }
  else {
    ptrdiff_t capl = 毫秒_缩变量->捕获_结[i].长度_短变量;
    *容量_短变量 = 毫秒_缩变量->捕获_结[i].初始的_变量;
    if (配置_l_可能性低_宏名(capl == 串库_捕_未结束_宏名))
      月亮状态_错误_函(毫秒_缩变量->L, "unfinished 捕获_结");
    else if (capl == 串库_捕_位置_宏名)
      月亮推入整数_函(毫秒_缩变量->L, (毫秒_缩变量->捕获_结[i].初始的_变量 - 毫秒_缩变量->源_初始的短) + 1);
    return capl;
  }
}


/*
** Push the i-到钩_变量 捕获_结 on the 栈_圆小.
*/
static void 月串库_推_一个捕获_函 (匹配状态机_结 *毫秒_缩变量, int i, const char *s,
                                                    const char *e) {
  const char *容量_短变量;
  ptrdiff_t l = 月串库_获取_一个捕获_函(毫秒_缩变量, i, s, e, &容量_短变量);
  if (l != 串库_捕_位置_宏名)
    月亮推入长字符串_函(毫秒_缩变量->L, 容量_短变量, l);
  /* else position was already 已推入_小写 */
}


static int 月串库_推_捕获们_函 (匹配状态机_结 *毫秒_缩变量, const char *s, const char *e) {
  int i;
  int 层级数_变量 = (毫秒_缩变量->层级_变量 == 0 && s) ? 1 : 毫秒_缩变量->层级_变量;
  月亮状态_检查栈_函(毫秒_缩变量->L, 层级数_变量, "too many captures");
  for (i = 0; i < 层级数_变量; i++)
    月串库_推_一个捕获_函(毫秒_缩变量, i, s, e);
  return 层级数_变量;  /* number of strings 已推入_小写 */
}


/* 月解析器_检查_函 whether pattern has no special characters */
static int 月串库_无特殊_函 (const char *p, size_t l) {
  size_t 上到_变量 = 0;
  do {
    if (strpbrk(p + 上到_变量, 串库_特殊的_宏名))
      return 0;  /* pattern has a special character */
    上到_变量 += strlen(p + 上到_变量) + 1;  /* may have 更多_变量 after \0 */
  } while (上到_变量 <= l);
  return 1;  /* no special chars found */
}


static void 月串库_预备状态机_函 (匹配状态机_结 *毫秒_缩变量, 炉_状态机结 *L,
                       const char *s, size_t 状列_缩变量, const char *p, size_t 长针_缩变量) {
  毫秒_缩变量->L = L;
  毫秒_缩变量->匹配深度_圆 = 串库_最大C调用_宏名;
  毫秒_缩变量->源_初始的短 = s;
  毫秒_缩变量->源_终短 = s + 状列_缩变量;
  毫秒_缩变量->模式_终缩 = p + 长针_缩变量;
}


static void 月串库_重新预备状态机_函 (匹配状态机_结 *毫秒_缩变量) {
  毫秒_缩变量->层级_变量 = 0;
  限制_月亮_断言_宏名(毫秒_缩变量->匹配深度_圆 == 串库_最大C调用_宏名);
}


static int 月串库_串_找_辅助_函 (炉_状态机结 *L, int find) {
  size_t 状列_缩变量, 长针_缩变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &状列_缩变量);
  const char *p = 月亮状态_检查状态字符串_函(L, 2, &长针_缩变量);
  size_t 初始的_变量 = 月串库_相对位置索引_函(月亮状态_可选整数_函(L, 3, 1), 状列_缩变量) - 1;
  if (初始的_变量 > 状列_缩变量) {  /* 起始_变量 after string's 终_变量? */
    辅助库_月亮l_推失败_宏名(L);  /* 月编译器_不能_函 find anything */
    return 1;
  }
  /* explicit request or no special characters? */
  if (find && (月亮_到布尔值_函(L, 4) || 月串库_无特殊_函(p, 长针_缩变量))) {
    /* do a plain search */
    const char *s2 = 月串库_l内存找_函(s + 初始的_变量, 状列_缩变量 - 初始的_变量, p, 长针_缩变量);
    if (s2) {
      月亮推入整数_函(L, (s2 - s) + 1);
      月亮推入整数_函(L, (s2 - s) + 长针_缩变量);
      return 2;
    }
  }
  else {
    匹配状态机_结 毫秒_缩变量;
    const char *s1 = s + 初始的_变量;
    int 锚点_变量 = (*p == '^');
    if (锚点_变量) {
      p++; 长针_缩变量--;  /* skip 锚点_变量 character */
    }
    月串库_预备状态机_函(&毫秒_缩变量, L, s, 状列_缩变量, p, 长针_缩变量);
    do {
      const char *结果_短变量;
      月串库_重新预备状态机_函(&毫秒_缩变量);
      if ((结果_短变量=月串库_匹配_函(&毫秒_缩变量, s1, p)) != NULL) {
        if (find) {
          月亮推入整数_函(L, (s1 - s) + 1);  /* 起始_变量 */
          月亮推入整数_函(L, 结果_短变量 - s);   /* 终_变量 */
          return 月串库_推_捕获们_函(&毫秒_缩变量, NULL, 0) + 2;
        }
        else
          return 月串库_推_捕获们_函(&毫秒_缩变量, s1, 结果_短变量);
      }
    } while (s1++ < 毫秒_缩变量.源_终短 && !锚点_变量);
  }
  辅助库_月亮l_推失败_宏名(L);  /* not found */
  return 1;
}


static int 月串库_串_找_函 (炉_状态机结 *L) {
  return 月串库_串_找_辅助_函(L, 1);
}


static int 月串库_串_匹配_函 (炉_状态机结 *L) {
  return 月串库_串_找_辅助_函(L, 0);
}


/* 状态机_变量 for '月串库_全局匹配_函' */
typedef struct 全局匹配状态机_结 {
  const char *源_短变量;  /* 当前_圆 position */
  const char *p;  /* pattern */
  const char *最后匹配_变量;  /* 终_变量 of 最后_变量 月串库_匹配_函 */
  匹配状态机_结 毫秒_缩变量;  /* 月串库_匹配_函 状态机_变量 */
} 全局匹配状态机_结;


static int 月串库_全局匹配_辅助_函 (炉_状态机结 *L) {
  全局匹配状态机_结 *全局管理_变量 = (全局匹配状态机_结 *)月亮_到用户数据_函(L, 月头_月亮_上值索引_宏名(3));
  const char *源_短变量;
  全局管理_变量->毫秒_缩变量.L = L;
  for (源_短变量 = 全局管理_变量->源_短变量; 源_短变量 <= 全局管理_变量->毫秒_缩变量.源_终短; 源_短变量++) {
    const char *e;
    月串库_重新预备状态机_函(&全局管理_变量->毫秒_缩变量);
    if ((e = 月串库_匹配_函(&全局管理_变量->毫秒_缩变量, 源_短变量, 全局管理_变量->p)) != NULL && e != 全局管理_变量->最后匹配_变量) {
      全局管理_变量->源_短变量 = 全局管理_变量->最后匹配_变量 = e;
      return 月串库_推_捕获们_函(&全局管理_变量->毫秒_缩变量, 源_短变量, e);
    }
  }
  return 0;  /* not found */
}


static int 月串库_全局匹配_函 (炉_状态机结 *L) {
  size_t 状列_缩变量, 长针_缩变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &状列_缩变量);
  const char *p = 月亮状态_检查状态字符串_函(L, 2, &长针_缩变量);
  size_t 初始的_变量 = 月串库_相对位置索引_函(月亮状态_可选整数_函(L, 3, 1), 状列_缩变量) - 1;
  全局匹配状态机_结 *全局管理_变量;
  月亮_设置顶_函(L, 2);  /* keep strings on closure 到_变量 avoid being collected */
  全局管理_变量 = (全局匹配状态机_结 *)月亮创建新的用户数据uv_函(L, sizeof(全局匹配状态机_结), 0);
  if (初始的_变量 > 状列_缩变量)  /* 起始_变量 after string's 终_变量? */
    初始的_变量 = 状列_缩变量 + 1;  /* avoid overflows in 's + 初始的_变量' */
  月串库_预备状态机_函(&全局管理_变量->毫秒_缩变量, L, s, 状列_缩变量, p, 长针_缩变量);
  全局管理_变量->源_短变量 = s + 初始的_变量; 全局管理_变量->p = p; 全局管理_变量->最后匹配_变量 = NULL;
  月亮推入C闭包_函(L, 月串库_全局匹配_辅助_函, 3);
  return 1;
}


static void 月串库_添加_s_函 (匹配状态机_结 *毫秒_缩变量, 炉L_缓冲区结 *b, const char *s,
                                                   const char *e) {
  size_t l;
  炉_状态机结 *L = 毫秒_缩变量->L;
  const char *新s_变量 = 月亮_到长字符串_函(L, 3, &l);
  const char *p;
  while ((p = (char *)memchr(新s_变量, 串库_月_转义_宏名, l)) != NULL) {
    月亮状态_添加状态字符串_函(b, 新s_变量, p - 新s_变量);
    p++;  /* skip ESC */
    if (*p == 串库_月_转义_宏名)  /* '%%' */
      luaL_addchar(b, *p);
    else if (*p == '0')  /* '%0' */
        月亮状态_添加状态字符串_函(b, s, e - s);
    else if (isdigit(uchar(*p))) {  /* '%n' */
      const char *容量_短变量;
      ptrdiff_t resl = 月串库_获取_一个捕获_函(毫秒_缩变量, *p - '1', s, e, &容量_短变量);
      if (resl == 串库_捕_位置_宏名)
        月亮状态_添加值_函(b);  /* add position 到_变量 accumulated 结果_变量 */
      else
        月亮状态_添加状态字符串_函(b, 容量_短变量, resl);
    }
    else
      月亮状态_错误_函(L, "invalid use of '%c' in replacement string", 串库_月_转义_宏名);
    l -= p + 1 - 新s_变量;
    新s_变量 = p + 1;
  }
  月亮状态_添加状态字符串_函(b, 新s_变量, l);
}


/*
** Add the replacement 值_圆 到_变量 the string 缓冲区_变量 'b'.
** Return true if the original string was 已更改_变量. (Function calls and
** table indexing resulting in nil or false do not 改变_变量 the subject.)
*/
static int 月串库_添加_值_函 (匹配状态机_结 *毫秒_缩变量, 炉L_缓冲区结 *b, const char *s,
                                      const char *e, int 转过来_变量) {
  炉_状态机结 *L = 毫秒_缩变量->L;
  switch (转过来_变量) {
    case 月头_月亮_T函数_宏名: {  /* call the function */
      int n;
      月亮_推入值_函(L, 3);  /* push the function */
      n = 月串库_推_捕获们_函(毫秒_缩变量, s, e);  /* all captures as arguments */
      月头_月亮_调用_宏名(L, n, 1);  /* call it */
      break;
    }
    case 月头_月亮_T表_宏名: {  /* index the table */
      月串库_推_一个捕获_函(毫秒_缩变量, 0, s, e);  /* 首先_变量 捕获_结 is the index */
      月亮获取表_函(L, 3);
      break;
    }
    default: {  /* 月头_月亮_T数目_宏名 or 月头_月亮_T字符串_宏名 */
      月串库_添加_s_函(毫秒_缩变量, b, s, e);  /* add 值_圆 到_变量 the 缓冲区_变量 */
      return 1;  /* something 已更改_变量 */
    }
  }
  if (!月亮_到布尔值_函(L, -1)) {  /* nil or false? */
    月头_月亮_弹出_宏名(L, 1);  /* remove 值_圆 */
    月亮状态_添加状态字符串_函(b, s, e - s);  /* keep original text */
    return 0;  /* no changes */
  }
  else if (配置_l_可能性低_宏名(!月亮_是否字符串_函(L, -1)))
    return 月亮状态_错误_函(L, "invalid replacement 值_圆 (a %s)",
                         辅助库_月亮l_类型名称_宏名(L, -1));
  else {
    月亮状态_添加值_函(b);  /* add 结果_变量 到_变量 accumulator */
    return 1;  /* something 已更改_变量 */
  }
}


static int 月串库_串_全局取代_函 (炉_状态机结 *L) {
  size_t 源长_变量, 长针_缩变量;
  const char *源_短变量 = 月亮状态_检查状态字符串_函(L, 1, &源长_变量);  /* subject */
  const char *p = 月亮状态_检查状态字符串_函(L, 2, &长针_缩变量);  /* pattern */
  const char *最后匹配_变量 = NULL;  /* 终_变量 of 最后_变量 月串库_匹配_函 */
  int 转过来_变量 = 月亮_类型_函(L, 3);  /* replacement type */
  炉_整数型 最大_s_变量 = 月亮状态_可选整数_函(L, 4, 源长_变量 + 1);  /* 最大_小变量 replacements */
  int 锚点_变量 = (*p == '^');
  炉_整数型 n = 0;  /* replacement 计数_变量 */
  int 已更改_变量 = 0;  /* 改变_变量 flag */
  匹配状态机_结 毫秒_缩变量;
  炉L_缓冲区结 b;
  辅助库_月亮l_实参期望_宏名(L, 转过来_变量 == 月头_月亮_T数目_宏名 || 转过来_变量 == 月头_月亮_T字符串_宏名 ||
                   转过来_变量 == 月头_月亮_T函数_宏名 || 转过来_变量 == 月头_月亮_T表_宏名, 3,
                      "string/function/table");
  月亮状态_缓冲初始的_函(L, &b);
  if (锚点_变量) {
    p++; 长针_缩变量--;  /* skip 锚点_变量 character */
  }
  月串库_预备状态机_函(&毫秒_缩变量, L, 源_短变量, 源长_变量, p, 长针_缩变量);
  while (n < 最大_s_变量) {
    const char *e;
    月串库_重新预备状态机_函(&毫秒_缩变量);  /* (re)prepare 状态机_变量 for new 月串库_匹配_函 */
    if ((e = 月串库_匹配_函(&毫秒_缩变量, 源_短变量, p)) != NULL && e != 最后匹配_变量) {  /* 月串库_匹配_函? */
      n++;
      已更改_变量 = 月串库_添加_值_函(&毫秒_缩变量, &b, 源_短变量, e, 转过来_变量) | 已更改_变量;
      源_短变量 = 最后匹配_变量 = e;
    }
    else if (源_短变量 < 毫秒_缩变量.源_终短)  /* otherwise, skip one character */
      luaL_addchar(&b, *源_短变量++);
    else break;  /* 终_变量 of subject */
    if (锚点_变量) break;
  }
  if (!已更改_变量)  /* no changes? */
    月亮_推入值_函(L, 1);  /* return original string */
  else {  /* something 已更改_变量 */
    月亮状态_添加状态字符串_函(&b, 源_短变量, 毫秒_缩变量.源_终短-源_短变量);
    月亮状态_推入结果_函(&b);  /* create and return new string */
  }
  月亮推入整数_函(L, n);  /* number of substitutions */
  return 2;
}

/* }====================================================== */



/*
** {======================================================
** STRING FORMAT
** =======================================================
*/

#if !defined(月串库_月亮_数目到串x_函)	/* { */

/*
** Hexadecimal floating-针点_变量 formatter
*/

#define 串库_大小与长度修饰符_宏名	(sizeof(配置_月亮_数目_形式长度_宏名)/sizeof(char))


/*
** Number of bits that goes into the 首先_变量 数字_变量. It can be any 值_圆
** between 1 and 4; the following definition tries 到_变量 对齐_变量 the number
** 到_变量 nibble boundaries by making 什么_变量 is 左_圆 after that 首先_变量 数字_变量 a
** multiple of 4.
*/
#define 串库_月_非阻塞文件描述符_宏名		((配置_l_浮点属性_宏名(MANT_DIG) - 1)%4 + 1)


/*
** Add integer part of 'x' 到_变量 缓冲区_变量 and return new 'x'
*/
static 炉_数目型 月串库_添加数字_函 (char *缓冲_变量, int n, 炉_数目型 x) {
  炉_数目型 数定_缩变量 = 配置_数学操作_宏名(floor)(x);  /* get integer part from 'x' */
  int d = (int)数定_缩变量;
  缓冲_变量[n] = (d < 10 ? d + '0' : d - 10 + 'a');  /* add 到_变量 缓冲区_变量 */
  return x - 数定_缩变量;  /* return 什么_变量 is 左_圆 */
}


static int 月串库_数目到串辅助_函 (char *缓冲_变量, int 大小_短变量, 炉_数目型 x) {
  /* if 'inf' or 'NaN', 格式_变量 it like '%g' */
  if (x != x || x == (炉_数目型)HUGE_VAL || x == -(炉_数目型)HUGE_VAL)
    return 配置_l_s打印f_宏名(缓冲_变量, 大小_短变量, 配置_月亮_数目_格式_宏名, (配置_月亮I_UAC数目_宏名)x);
  else if (x == 0) {  /* can be -0... */
    /* create "0" or "-0" followed by exponent */
    return 配置_l_s打印f_宏名(缓冲_变量, 大小_短变量, 配置_月亮_数目_格式_宏名 "x0p+0", (配置_月亮I_UAC数目_宏名)x);
  }
  else {
    int e;
    炉_数目型 m = 配置_数学操作_宏名(frexp)(x, &e);  /* 'x' fraction and exponent */
    int n = 0;  /* character 计数_变量 */
    if (m < 0) {  /* is number negative? */
      缓冲_变量[n++] = '-';  /* add sign */
      m = -m;  /* make it positive */
    }
    缓冲_变量[n++] = '0'; 缓冲_变量[n++] = 'x';  /* add "0x" */
    m = 月串库_添加数字_函(缓冲_变量, n++, m * (1 << 串库_月_非阻塞文件描述符_宏名));  /* add 首先_变量 数字_变量 */
    e -= 串库_月_非阻塞文件描述符_宏名;  /* this 数字_变量 goes before the radix 针点_变量 */
    if (m > 0) {  /* 更多_变量 digits? */
      缓冲_变量[n++] = 配置_月亮_获取本地化小数点_宏名();  /* add radix 针点_变量 */
      do {  /* add as many digits as 已需要_变量 */
        m = 月串库_添加数字_函(缓冲_变量, n++, m * 16);
      } while (m > 0);
    }
    n += 配置_l_s打印f_宏名(缓冲_变量 + n, 大小_短变量 - n, "p%+d", e);  /* add exponent */
    限制_月亮_断言_宏名(n < 大小_短变量);
    return n;
  }
}


static int 月串库_月亮_数目到串x_函 (炉_状态机结 *L, char *缓冲_变量, int 大小_短变量,
                            const char *格式_短变量, 炉_数目型 x) {
  int n = 月串库_数目到串辅助_函(缓冲_变量, 大小_短变量, x);
  if (格式_短变量[串库_大小与长度修饰符_宏名] == 'A') {
    int i;
    for (i = 0; i < n; i++)
      缓冲_变量[i] = toupper(uchar(缓冲_变量[i]));
  }
  else if (配置_l_可能性低_宏名(格式_短变量[串库_大小与长度修饰符_宏名] != 'a'))
    return 月亮状态_错误_函(L, "modifiers for 格式_变量 '%%a'/'%%A' not implemented");
  return n;
}

#endif				/* } */


/*
** Maximum 大小_变量 for items formatted with '%f'. This 大小_变量 is produced
** by 格式_变量('%.99f', -maxfloat), and is equal 到_变量 99 + 3 ('-', '.',
** and '\0') + number of decimal digits 到_变量 represent maxfloat (which
** is maximum exponent + 1). (99+3+1, adding some 额外_变量, 110)
*/
#define 串库_最大_项F_宏名	(110 + 配置_l_浮点属性_宏名(MAX_10_EXP))


/*
** All formats except '%f' do not need that large 限制_变量.  The other
** float formats use exponents, so that they fit in the 99 限制_变量 for
** significant digits; 's' for large strings and 'q' add items directly
** 到_变量 the 缓冲区_变量; all integer formats also fit in the 99 限制_变量.  The
** worst case are floats: they may need 99 significant digits, plus
** '0x', '-', '.', 'e+XXXX', and '\0'. Adding some 额外_变量, 120.
*/
#define 串库_最大_项_宏名	120


/* 有效_变量 标志们_变量 in a 格式_变量 specification */
#if !defined(串库_月_格式化标志F_宏名)

/* 有效_变量 标志们_变量 for a, A, e, E, f, F, g, and G conversions */
#define 串库_月_格式化标志F_宏名	"-+#0 "

/* 有效_变量 标志们_变量 for o, x, and X conversions */
#define 串库_月_格式化标志X_宏名	"-#0"

/* 有效_变量 标志们_变量 for d and i conversions */
#define 串库_月_格式化标志I_宏名	"-+0 "

/* 有效_变量 标志们_变量 for u conversions */
#define 串库_月_格式化标志U_宏名	"-0"

/* 有效_变量 标志们_变量 for c, p, and s conversions */
#define 串库_月_格式化标志C_宏名	"-"

#endif


/*
** Maximum 大小_变量 of each 格式_变量 specification (such as "%-099.99d"):
** Initial '%', 标志们_变量 (上_小变量 到_变量 5), width (2), period, precision (2),
** length modifier (8), conversion specifier, and 最终_变量 '\0', plus some
** 额外_变量.
*/
#define 串库_最大_格式_宏名	32


static void 月串库_添加引号_函 (炉L_缓冲区结 *b, const char *s, size_t 长度_短变量) {
  luaL_addchar(b, '"');
  while (长度_短变量--) {
    if (*s == '"' || *s == '\\' || *s == '\n') {
      luaL_addchar(b, '\\');
      luaL_addchar(b, *s);
    }
    else if (iscntrl(uchar(*s))) {
      char 缓冲_变量[10];
      if (!isdigit(uchar(*(s+1))))
        配置_l_s打印f_宏名(缓冲_变量, sizeof(缓冲_变量), "\\%d", (int)uchar(*s));
      else
        配置_l_s打印f_宏名(缓冲_变量, sizeof(缓冲_变量), "\\%03d", (int)uchar(*s));
      月亮状态_添加字符串_函(b, 缓冲_变量);
    }
    else
      luaL_addchar(b, *s);
    s++;
  }
  luaL_addchar(b, '"');
}


/*
** Serialize a floating-针点_变量 number in such a way that it can be
** scanned back by Lua. Use hexadecimal 格式_变量 for "common" numbers
** (到_变量 preserve precision); inf, -inf, and NaN are handled separately.
** (NaN 月编译器_不能_函 be expressed as a numeral, so we write '(0/0)' for it.)
*/
static int 月串库_引号浮点_函 (炉_状态机结 *L, char *缓冲_变量, 炉_数目型 n) {
  const char *s;  /* for the fixed representations */
  if (n == (炉_数目型)HUGE_VAL)  /* inf? */
    s = "1e9999";
  else if (n == -(炉_数目型)HUGE_VAL)  /* -inf? */
    s = "-1e9999";
  else if (n != n)  /* NaN? */
    s = "(0/0)";
  else {  /* 格式_变量 number as hexadecimal */
    int  注意基_变量 = 月串库_月亮_数目到串x_函(L, 缓冲_变量, 串库_最大_项_宏名,
                                 "%" 配置_月亮_数目_形式长度_宏名 "a", n);
    /* ensures that '缓冲_变量' string uses a 点_变量 as the radix character */
    if (memchr(缓冲_变量, '.', 注意基_变量) == NULL) {  /* no 点_变量? */
      char 针点_变量 = 配置_月亮_获取本地化小数点_宏名();  /* try locale 针点_变量 */
      char *保护针点_变量 = (char *)memchr(缓冲_变量, 针点_变量, 注意基_变量);
      if (保护针点_变量) *保护针点_变量 = '.';  /* 改变_变量 it 到_变量 a 点_变量 */
    }
    return 注意基_变量;
  }
  /* for the fixed representations */
  return 配置_l_s打印f_宏名(缓冲_变量, 串库_最大_项_宏名, "%s", s);
}


static void 月串库_添加字面_函 (炉_状态机结 *L, 炉L_缓冲区结 *b, int 实参_短变量) {
  switch (月亮_类型_函(L, 实参_短变量)) {
    case 月头_月亮_T字符串_宏名: {
      size_t 长度_短变量;
      const char *s = 月亮_到长字符串_函(L, 实参_短变量, &长度_短变量);
      月串库_添加引号_函(b, s, 长度_短变量);
      break;
    }
    case 月头_月亮_T数目_宏名: {
      char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(b, 串库_最大_项_宏名);
      int 注意基_变量;
      if (!月亮_是否整数_函(L, 实参_短变量))  /* float? */
        注意基_变量 = 月串库_引号浮点_函(L, 缓冲_变量, 月头_月亮_到数目_宏名(L, 实参_短变量));
      else {  /* integers */
        炉_整数型 n = 月头_月亮_到整数_宏名(L, 实参_短变量);
        const char *格式_变量 = (n == 配置_月亮_最小整数_宏名)  /* corner case? */
                           ? "0x%" 配置_月亮_整数_形式长度_宏名 "x"  /* use 十六进制_变量 */
                           : 配置_月亮_整数_格式_宏名;  /* else use default 格式_变量 */
        注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 串库_最大_项_宏名, 格式_变量, (配置_月亮I_UAC整型_宏名)n);
      }
      luaL_addsize(b, 注意基_变量);
      break;
    }
    case 月头_月亮_T空值_宏名: case 月头_月亮_T布尔_宏名: {
      月亮状态_到字符串_函(L, 实参_短变量, NULL);
      月亮状态_添加值_函(b);
      break;
    }
    default: {
      月亮状态_实参错误_函(L, 实参_短变量, "值_圆 has no literal 来自_变量");
    }
  }
}


static const char *月串库_获取两位数字_函 (const char *s) {
  if (isdigit(uchar(*s))) {
    s++;
    if (isdigit(uchar(*s))) s++;  /* (2 digits at most) */
  }
  return s;
}


/*
** Check whether a conversion specification is 有效_变量. When called,
** 首先_变量 character in '来自_变量' must be '%' and 最后_变量 character must
** be a 有效_变量 conversion specifier. '标志们_变量' are the accepted 标志们_变量;
** 'precision' signals whether 到_变量 accept a precision.
*/
static void 月串库_检查格式_函 (炉_状态机结 *L, const char *来自_变量, const char *标志们_变量,
                                       int precision) {
  const char *规格_短变量 = 来自_变量 + 1;  /* skip '%' */
  规格_短变量 += strspn(规格_短变量, 标志们_变量);  /* skip 标志们_变量 */
  if (*规格_短变量 != '0') {  /* a width 月编译器_不能_函 起始_变量 with '0' */
    规格_短变量 = 月串库_获取两位数字_函(规格_短变量);  /* skip width */
    if (*规格_短变量 == '.' && precision) {
      规格_短变量++;
      规格_短变量 = 月串库_获取两位数字_函(规格_短变量);  /* skip precision */
    }
  }
  if (!isalpha(uchar(*规格_短变量)))  /* did not go 到_变量 the 终_变量? */
    月亮状态_错误_函(L, "invalid conversion specification: '%s'", 来自_变量);
}


/*
** Get a conversion specification and copy it 到_变量 '来自_变量'.
** Return the address of its 最后_变量 character.
*/
static const char *月串库_获取格式_函 (炉_状态机结 *L, const char *串格_变量,
                                            char *来自_变量) {
  /* spans 标志们_变量, width, and precision ('0' is included as a flag) */
  size_t 长度_短变量 = strspn(串格_变量, 串库_月_格式化标志F_宏名 "123456789.");
  长度_短变量++;  /* adds following character (should be the specifier) */
  /* still needs 空间_圆 for '%', '\0', plus a length modifier */
  if (长度_短变量 >= 串库_最大_格式_宏名 - 10)
    月亮状态_错误_函(L, "invalid 格式_变量 (too long)");
  *(来自_变量++) = '%';
  memcpy(来自_变量, 串格_变量, 长度_短变量 * sizeof(char));
  *(来自_变量 + 长度_短变量) = '\0';
  return 串格_变量 + 长度_短变量 - 1;
}


/*
** add length modifier into formats
*/
static void 月串库_添加长度修改器_函 (char *来自_变量, const char *lenmod) {
  size_t l = strlen(来自_变量);
  size_t 本地方法_缩变量 = strlen(lenmod);
  char 规格_短变量 = 来自_变量[l - 1];
  strcpy(来自_变量 + l - 1, lenmod);
  来自_变量[l + 本地方法_缩变量 - 1] = 规格_短变量;
  来自_变量[l + 本地方法_缩变量] = '\0';
}


static int 月串库_串_格式_函 (炉_状态机结 *L) {
  int 顶部_变量 = 月亮_获取顶_函(L);
  int 实参_短变量 = 1;
  size_t sfl_变量;
  const char *串格_变量 = 月亮状态_检查状态字符串_函(L, 实参_短变量, &sfl_变量);
  const char *串格_终_变量 = 串格_变量+sfl_变量;
  const char *标志们_变量;
  炉L_缓冲区结 b;
  月亮状态_缓冲初始的_函(L, &b);
  while (串格_变量 < 串格_终_变量) {
    if (*串格_变量 != 串库_月_转义_宏名)
      luaL_addchar(&b, *串格_变量++);
    else if (*++串格_变量 == 串库_月_转义_宏名)
      luaL_addchar(&b, *串格_变量++);  /* %% */
    else { /* 格式_变量 item */
      char 来自_变量[串库_最大_格式_宏名];  /* 到_变量 store the 格式_变量 ('%...') */
      int 最大项_变量 = 串库_最大_项_宏名;  /* maximum length for the 结果_变量 */
      char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, 最大项_变量);  /* 到_变量 put 结果_变量 */
      int 注意基_变量 = 0;  /* number of bytes in 结果_变量 */
      if (++实参_短变量 > 顶部_变量)
        return 月亮状态_实参错误_函(L, 实参_短变量, "no 值_圆");
      串格_变量 = 月串库_获取格式_函(L, 串格_变量, 来自_变量);
      switch (*串格_变量++) {
        case 'c': {
          月串库_检查格式_函(L, 来自_变量, 串库_月_格式化标志C_宏名, 0);
          注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 最大项_变量, 来自_变量, (int)月亮状态_检查整数_函(L, 实参_短变量));
          break;
        }
        case 'd': case 'i':
          标志们_变量 = 串库_月_格式化标志I_宏名;
          goto intcase;
        case 'u':
          标志们_变量 = 串库_月_格式化标志U_宏名;
          goto intcase;
        case 'o': case 'x': case 'X':
          标志们_变量 = 串库_月_格式化标志X_宏名;
         intcase: {
          炉_整数型 n = 月亮状态_检查整数_函(L, 实参_短变量);
          月串库_检查格式_函(L, 来自_变量, 标志们_变量, 1);
          月串库_添加长度修改器_函(来自_变量, 配置_月亮_整数_形式长度_宏名);
          注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 最大项_变量, 来自_变量, (配置_月亮I_UAC整型_宏名)n);
          break;
        }
        case 'a': case 'A':
          月串库_检查格式_函(L, 来自_变量, 串库_月_格式化标志F_宏名, 1);
          月串库_添加长度修改器_函(来自_变量, 配置_月亮_数目_形式长度_宏名);
          注意基_变量 = 月串库_月亮_数目到串x_函(L, 缓冲_变量, 最大项_变量, 来自_变量,
                                  月亮状态_检查数目_函(L, 实参_短变量));
          break;
        case 'f':
          最大项_变量 = 串库_最大_项F_宏名;  /* 额外_变量 空间_圆 for '%f' */
          缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, 最大项_变量);
          /* FALLTHROUGH */
        case 'e': case 'E': case 'g': case 'G': {
          炉_数目型 n = 月亮状态_检查数目_函(L, 实参_短变量);
          月串库_检查格式_函(L, 来自_变量, 串库_月_格式化标志F_宏名, 1);
          月串库_添加长度修改器_函(来自_变量, 配置_月亮_数目_形式长度_宏名);
          注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 最大项_变量, 来自_变量, (配置_月亮I_UAC数目_宏名)n);
          break;
        }
        case 'p': {
          const void *p = 月亮到指针_函(L, 实参_短变量);
          月串库_检查格式_函(L, 来自_变量, 串库_月_格式化标志C_宏名, 0);
          if (p == NULL) {  /* avoid calling 'printf' with argument NULL */
            p = "(null)";  /* 结果_变量 */
            来自_变量[strlen(来自_变量) - 1] = 's';  /* 格式_变量 it as a string */
          }
          注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 最大项_变量, 来自_变量, p);
          break;
        }
        case 'q': {
          if (来自_变量[2] != '\0')  /* modifiers? */
            return 月亮状态_错误_函(L, "specifier '%%q' 月编译器_不能_函 have modifiers");
          月串库_添加字面_函(L, &b, 实参_短变量);
          break;
        }
        case 's': {
          size_t l;
          const char *s = 月亮状态_到字符串_函(L, 实参_短变量, &l);
          if (来自_变量[2] == '\0')  /* no modifiers? */
            月亮状态_添加值_函(&b);  /* keep entire string */
          else {
            辅助库_月亮l_实参检查_宏名(L, l == strlen(s), 实参_短变量, "string contains zeros");
            月串库_检查格式_函(L, 来自_变量, 串库_月_格式化标志C_宏名, 1);
            if (strchr(来自_变量, '.') == NULL && l >= 100) {
              /* no precision and string is too long 到_变量 be formatted */
              月亮状态_添加值_函(&b);  /* keep entire string */
            }
            else {  /* 格式_变量 the string into '缓冲_变量' */
              注意基_变量 = 配置_l_s打印f_宏名(缓冲_变量, 最大项_变量, 来自_变量, s);
              月头_月亮_弹出_宏名(L, 1);  /* remove 结果_变量 from '月亮状态_到字符串_函' */
            }
          }
          break;
        }
        default: {  /* also treat cases 'pnLlh' */
          return 月亮状态_错误_函(L, "invalid conversion '%s' 到_变量 '格式_变量'", 来自_变量);
        }
      }
      限制_月亮_断言_宏名(注意基_变量 < 最大项_变量);
      luaL_addsize(&b, 注意基_变量);
    }
  }
  月亮状态_推入结果_函(&b);
  return 1;
}

/* }====================================================== */


/*
** {======================================================
** PACK/UNPACK
** =======================================================
*/


/* 值_圆 used for padding */
#if !defined(串库_月亮L_打包衬垫字节_宏名)
#define 串库_月亮L_打包衬垫字节_宏名		0x00
#endif

/* maximum 大小_变量 for the binary representation of an integer */
#define 串库_最大整型大小_宏名	16

/* number of bits in a character */
#define NB	CHAR_BIT

/* 掩码_变量 for one character (NB 1's) */
#define MC	((1 << NB) - 1)

/* 大小_变量 of a 炉_整数型 */
#define 串库_大小整型_宏名	((int)sizeof(炉_整数型))


/* 虚假_变量 union 到_变量 get native endianness */
static const union {
  int 虚假_变量;
  char little;  /* true iff machine is little endian */
} 土著字节序_变量 = {1};


/*
** information 到_变量 pack/unpack stuff
*/
typedef struct 头部_结 {
  炉_状态机结 *L;
  int 是小少的吗_圆;
  int 最大对齐_变量;
} 头部_结;


/*
** 选项们_变量 for pack/unpack
*/
typedef enum 常量选项_枚举 {
  常量整型_,		/* signed integers */
  常量无符整型_,	/* unsigned integers */
  常量浮点_,	/* single-precision floating-针点_变量 numbers */
  常量数目_,	/* Lua "native" floating-针点_变量 numbers */
  常量双浮_,	/* double-precision floating-针点_变量 numbers */
  常量印刻_,	/* fixed-length strings */
  常量字符串_,	/* strings with prefixed length */
  常量零串_,	/* zero-terminated strings */
  常量衬垫_,	/* padding */
  常量衬垫对齐_,	/* padding for alignment */
  常量无操作_		/* no-操作_短变量 (configuration or spaces) */
} 常量选项_枚举;


/*
** Read an integer numeral from string '格式_短变量' or return 'df' if
** there is no numeral
*/
static int 数字_变量 (int c) { return '0' <= c && c <= '9'; }

static int 月串库_获取数目_函 (const char **格式_短变量, int df) {
  if (!数字_变量(**格式_短变量))  /* no number? */
    return df;  /* return default 值_圆 */
  else {
    int a = 0;
    do {
      a = a*10 + (*((*格式_短变量)++) - '0');
    } while (数字_变量(**格式_短变量) && a <= ((int)串库_最大大小_宏名 - 9)/10);
    return a;
  }
}


/*
** Read an integer numeral and raises an 错误_小变量 if it is larger
** than the maximum 大小_变量 for integers.
*/
static int 月串库_获取数目限制_函 (头部_结 *h, const char **格式_短变量, int df) {
  int 大小_短变量 = 月串库_获取数目_函(格式_短变量, df);
  if (配置_l_可能性低_宏名(大小_短变量 > 串库_最大整型大小_宏名 || 大小_短变量 <= 0))
    return 月亮状态_错误_函(h->L, "integral 大小_变量 (%d) out of 限制们_变量 [1,%d]",
                            大小_短变量, 串库_最大整型大小_宏名);
  return 大小_短变量;
}


/*
** Initialize 头部_结
*/
static void 月串库_初始的头部_函 (炉_状态机结 *L, 头部_结 *h) {
  h->L = L;
  h->是小少的吗_圆 = 土著字节序_变量.little;
  h->最大对齐_变量 = 1;
}


/*
** Read and classify 下一个_变量 选项_变量. '大小_变量' is filled with 选项_变量's 大小_变量.
*/
static 常量选项_枚举 月串库_获取选项_函 (头部_结 *h, const char **格式_短变量, int *大小_变量) {
  /* 虚假_变量 structure 到_变量 get native alignment requirements */
  struct cD { char c; union { 炉I_最大对齐; } u; };
  int 输出_缩变量 = *((*格式_短变量)++);
  *大小_变量 = 0;  /* default */
  switch (输出_缩变量) {
    case 'b': *大小_变量 = sizeof(char); return 常量整型_;
    case 'B': *大小_变量 = sizeof(char); return 常量无符整型_;
    case 'h': *大小_变量 = sizeof(short); return 常量整型_;
    case 'H': *大小_变量 = sizeof(short); return 常量无符整型_;
    case 'l': *大小_变量 = sizeof(long); return 常量整型_;
    case 'L': *大小_变量 = sizeof(long); return 常量无符整型_;
    case 'j': *大小_变量 = sizeof(炉_整数型); return 常量整型_;
    case 'J': *大小_变量 = sizeof(炉_整数型); return 常量无符整型_;
    case 'T': *大小_变量 = sizeof(size_t); return 常量无符整型_;
    case 'f': *大小_变量 = sizeof(float); return 常量浮点_;
    case 'n': *大小_变量 = sizeof(炉_数目型); return 常量数目_;
    case 'd': *大小_变量 = sizeof(double); return 常量双浮_;
    case 'i': *大小_变量 = 月串库_获取数目限制_函(h, 格式_短变量, sizeof(int)); return 常量整型_;
    case 'I': *大小_变量 = 月串库_获取数目限制_函(h, 格式_短变量, sizeof(int)); return 常量无符整型_;
    case 's': *大小_变量 = 月串库_获取数目限制_函(h, 格式_短变量, sizeof(size_t)); return 常量字符串_;
    case 'c':
      *大小_变量 = 月串库_获取数目_函(格式_短变量, -1);
      if (配置_l_可能性低_宏名(*大小_变量 == -1))
        月亮状态_错误_函(h->L, "missing 大小_变量 for 格式_变量 选项_变量 'c'");
      return 常量印刻_;
    case 'z': return 常量零串_;
    case 'x': *大小_变量 = 1; return 常量衬垫_;
    case 'X': return 常量衬垫对齐_;
    case ' ': break;
    case '<': h->是小少的吗_圆 = 1; break;
    case '>': h->是小少的吗_圆 = 0; break;
    case '=': h->是小少的吗_圆 = 土著字节序_变量.little; break;
    case '!': {
      const int 最大对齐_变量 = offsetof(struct cD, u);
      h->最大对齐_变量 = 月串库_获取数目限制_函(h, 格式_短变量, 最大对齐_变量);
      break;
    }
    default: 月亮状态_错误_函(h->L, "invalid 格式_变量 选项_变量 '%c'", 输出_缩变量);
  }
  return 常量无操作_;
}


/*
** Read, classify, and fill other details about the 下一个_变量 选项_变量.
** 'psize' is filled with 选项_变量's 大小_变量, 'notoalign' with its
** alignment requirements.
** Local variable '大小_变量' gets the 大小_变量 到_变量 be aligned. (Kpadal 选项_变量
** always gets its full alignment, other 选项们_变量 are limited by
** the maximum alignment ('最大对齐_变量'). 常量印刻_ 选项_变量 needs no alignment
** despite its 大小_变量.
*/
static 常量选项_枚举 月串库_获取细节_函 (头部_结 *h, size_t 总大小_变量,
                           const char **格式_短变量, int *psize, int *到对齐数_变量) {
  常量选项_枚举 输出_缩变量 = 月串库_获取选项_函(h, 格式_短变量, psize);
  int 对齐_变量 = *psize;  /* usually, alignment follows 大小_变量 */
  if (输出_缩变量 == 常量衬垫对齐_) {  /* 'X' gets alignment from following 选项_变量 */
    if (**格式_短变量 == '\0' || 月串库_获取选项_函(h, 格式_短变量, &对齐_变量) == 常量印刻_ || 对齐_变量 == 0)
      月亮状态_实参错误_函(h->L, 1, "invalid 下一个_变量 选项_变量 for 选项_变量 'X'");
  }
  if (对齐_变量 <= 1 || 输出_缩变量 == 常量印刻_)  /* need no alignment? */
    *到对齐数_变量 = 0;
  else {
    if (对齐_变量 > h->最大对齐_变量)  /* enforce maximum alignment */
      对齐_变量 = h->最大对齐_变量;
    if (配置_l_可能性低_宏名((对齐_变量 & (对齐_变量 - 1)) != 0))  /* not a power of 2? */
      月亮状态_实参错误_函(h->L, 1, "格式_变量 asks for alignment not power of 2");
    *到对齐数_变量 = (对齐_变量 - (int)(总大小_变量 & (对齐_变量 - 1))) & (对齐_变量 - 1);
  }
  return 输出_缩变量;
}


/*
** Pack integer 'n' with '大小_变量' bytes and '是小少的吗_圆' endianness.
** The 最终_变量 'if' handles the case when '大小_变量' is larger than
** the 大小_变量 of a Lua integer, correcting the 额外_变量 sign-extension
** bytes if necessary (by default they would be zeros).
*/
static void 月串库_打包整型_函 (炉L_缓冲区结 *b, lua_Unsigned n,
                     int 是小少的吗_圆, int 大小_变量, int 负号_短变量) {
  char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(b, 大小_变量);
  int i;
  缓冲_变量[是小少的吗_圆 ? 0 : 大小_变量 - 1] = (char)(n & MC);  /* 首先_变量 byte */
  for (i = 1; i < 大小_变量; i++) {
    n >>= NB;
    缓冲_变量[是小少的吗_圆 ? i : 大小_变量 - 1 - i] = (char)(n & MC);
  }
  if (负号_短变量 && 大小_变量 > 串库_大小整型_宏名) {  /* negative number need sign extension? */
    for (i = 串库_大小整型_宏名; i < 大小_变量; i++)  /* correct 额外_变量 bytes */
      缓冲_变量[是小少的吗_圆 ? i : 大小_变量 - 1 - i] = (char)MC;
  }
  luaL_addsize(b, 大小_变量);  /* add 结果_变量 到_变量 缓冲区_变量 */
}


/*
** Copy '大小_变量' bytes from '源_短变量' 到_变量 '目的地_变量', correcting endianness if
** given '是小少的吗_圆' is different from native endianness.
*/
static void 月串库_带字节序复制_函 (char *目的地_变量, const char *源_短变量,
                            int 大小_变量, int 是小少的吗_圆) {
  if (是小少的吗_圆 == 土著字节序_变量.little)
    memcpy(目的地_变量, 源_短变量, 大小_变量);
  else {
    目的地_变量 += 大小_变量 - 1;
    while (大小_变量-- != 0)
      *(目的地_变量--) = *(源_短变量++);
  }
}


static int 月串库_串_打包_函 (炉_状态机结 *L) {
  炉L_缓冲区结 b;
  头部_结 h;
  const char *格式_短变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);  /* 格式_变量 string */
  int 实参_短变量 = 1;  /* 当前_圆 argument 到_变量 pack */
  size_t 总大小_变量 = 0;  /* accumulate 总数_变量 大小_变量 of 结果_变量 */
  月串库_初始的头部_函(L, &h);
  月亮推入空值_函(L);  /* 记号_变量 到_变量 separate arguments from string 缓冲区_变量 */
  月亮状态_缓冲初始的_函(L, &b);
  while (*格式_短变量 != '\0') {
    int 大小_变量, 到对齐数_变量;
    常量选项_枚举 输出_缩变量 = 月串库_获取细节_函(&h, 总大小_变量, &格式_短变量, &大小_变量, &到对齐数_变量);
    总大小_变量 += 到对齐数_变量 + 大小_变量;
    while (到对齐数_变量-- > 0)
     luaL_addchar(&b, 串库_月亮L_打包衬垫字节_宏名);  /* fill alignment */
    实参_短变量++;
    switch (输出_缩变量) {
      case 常量整型_: {  /* signed integers */
        炉_整数型 n = 月亮状态_检查整数_函(L, 实参_短变量);
        if (大小_变量 < 串库_大小整型_宏名) {  /* need overflow 月解析器_检查_函? */
          炉_整数型 限_短变量 = (炉_整数型)1 << ((大小_变量 * NB) - 1);
          辅助库_月亮l_实参检查_宏名(L, -限_短变量 <= n && n < 限_短变量, 实参_短变量, "integer overflow");
        }
        月串库_打包整型_函(&b, (lua_Unsigned)n, h.是小少的吗_圆, 大小_变量, (n < 0));
        break;
      }
      case 常量无符整型_: {  /* unsigned integers */
        炉_整数型 n = 月亮状态_检查整数_函(L, 实参_短变量);
        if (大小_变量 < 串库_大小整型_宏名)  /* need overflow 月解析器_检查_函? */
          辅助库_月亮l_实参检查_宏名(L, (lua_Unsigned)n < ((lua_Unsigned)1 << (大小_变量 * NB)),
                           实参_短变量, "unsigned overflow");
        月串库_打包整型_函(&b, (lua_Unsigned)n, h.是小少的吗_圆, 大小_变量, 0);
        break;
      }
      case 常量浮点_: {  /* C float */
        float f = (float)月亮状态_检查数目_函(L, 实参_短变量);  /* get argument */
        char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, sizeof(f));
        /* move 'f' 到_变量 最终_变量 结果_变量, correcting endianness if 已需要_变量 */
        月串库_带字节序复制_函(缓冲_变量, (char *)&f, sizeof(f), h.是小少的吗_圆);
        luaL_addsize(&b, 大小_变量);
        break;
      }
      case 常量数目_: {  /* Lua float */
        炉_数目型 f = 月亮状态_检查数目_函(L, 实参_短变量);  /* get argument */
        char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, sizeof(f));
        /* move 'f' 到_变量 最终_变量 结果_变量, correcting endianness if 已需要_变量 */
        月串库_带字节序复制_函(缓冲_变量, (char *)&f, sizeof(f), h.是小少的吗_圆);
        luaL_addsize(&b, 大小_变量);
        break;
      }
      case 常量双浮_: {  /* C double */
        double f = (double)月亮状态_检查数目_函(L, 实参_短变量);  /* get argument */
        char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, sizeof(f));
        /* move 'f' 到_变量 最终_变量 结果_变量, correcting endianness if 已需要_变量 */
        月串库_带字节序复制_函(缓冲_变量, (char *)&f, sizeof(f), h.是小少的吗_圆);
        luaL_addsize(&b, 大小_变量);
        break;
      }
      case 常量印刻_: {  /* fixed-大小_变量 string */
        size_t 长度_短变量;
        const char *s = 月亮状态_检查状态字符串_函(L, 实参_短变量, &长度_短变量);
        辅助库_月亮l_实参检查_宏名(L, 长度_短变量 <= (size_t)大小_变量, 实参_短变量,
                         "string longer than given 大小_变量");
        月亮状态_添加状态字符串_函(&b, s, 长度_短变量);  /* add string */
        while (长度_短变量++ < (size_t)大小_变量)  /* pad 额外_变量 空间_圆 */
          luaL_addchar(&b, 串库_月亮L_打包衬垫字节_宏名);
        break;
      }
      case 常量字符串_: {  /* strings with length 计数_变量 */
        size_t 长度_短变量;
        const char *s = 月亮状态_检查状态字符串_函(L, 实参_短变量, &长度_短变量);
        辅助库_月亮l_实参检查_宏名(L, 大小_变量 >= (int)sizeof(size_t) ||
                         长度_短变量 < ((size_t)1 << (大小_变量 * NB)),
                         实参_短变量, "string length does not fit in given 大小_变量");
        月串库_打包整型_函(&b, (lua_Unsigned)长度_短变量, h.是小少的吗_圆, 大小_变量, 0);  /* pack length */
        月亮状态_添加状态字符串_函(&b, s, 长度_短变量);
        总大小_变量 += 长度_短变量;
        break;
      }
      case 常量零串_: {  /* zero-terminated string */
        size_t 长度_短变量;
        const char *s = 月亮状态_检查状态字符串_函(L, 实参_短变量, &长度_短变量);
        辅助库_月亮l_实参检查_宏名(L, strlen(s) == 长度_短变量, 实参_短变量, "string contains zeros");
        月亮状态_添加状态字符串_函(&b, s, 长度_短变量);
        luaL_addchar(&b, '\0');  /* add zero at the 终_变量 */
        总大小_变量 += 长度_短变量 + 1;
        break;
      }
      case 常量衬垫_: luaL_addchar(&b, 串库_月亮L_打包衬垫字节_宏名);  /* FALLTHROUGH */
      case 常量衬垫对齐_: case 常量无操作_:
        实参_短变量--;  /* undo increment */
        break;
    }
  }
  月亮状态_推入结果_函(&b);
  return 1;
}


static int 月串库_串_打包大小_函 (炉_状态机结 *L) {
  头部_结 h;
  const char *格式_短变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);  /* 格式_变量 string */
  size_t 总大小_变量 = 0;  /* accumulate 总数_变量 大小_变量 of 结果_变量 */
  月串库_初始的头部_函(L, &h);
  while (*格式_短变量 != '\0') {
    int 大小_变量, 到对齐数_变量;
    常量选项_枚举 输出_缩变量 = 月串库_获取细节_函(&h, 总大小_变量, &格式_短变量, &大小_变量, &到对齐数_变量);
    辅助库_月亮l_实参检查_宏名(L, 输出_缩变量 != 常量字符串_ && 输出_缩变量 != 常量零串_, 1,
                     "variable-length 格式_变量");
    大小_变量 += 到对齐数_变量;  /* 总数_变量 空间_圆 used by 选项_变量 */
    辅助库_月亮l_实参检查_宏名(L, 总大小_变量 <= 串库_最大大小_宏名 - 大小_变量, 1,
                     "格式_变量 结果_变量 too large");
    总大小_变量 += 大小_变量;
  }
  月亮推入整数_函(L, (炉_整数型)总大小_变量);
  return 1;
}


/*
** Unpack an integer with '大小_变量' bytes and '是小少的吗_圆' endianness.
** If 大小_变量 is smaller than the 大小_变量 of a Lua integer and integer
** is signed, must do sign extension (propagating the sign 到_变量 the
** higher bits); if 大小_变量 is larger than the 大小_变量 of a Lua integer,
** it must 月解析器_检查_函 the unread bytes 到_变量 see whether they do not cause an
** overflow.
*/
static 炉_整数型 月串库_反打包整型_函 (炉_状态机结 *L, const char *串_变量,
                              int 是小少的吗_圆, int 大小_变量, int issigned) {
  lua_Unsigned 结果_短变量 = 0;
  int i;
  int 限制_变量 = (大小_变量  <= 串库_大小整型_宏名) ? 大小_变量 : 串库_大小整型_宏名;
  for (i = 限制_变量 - 1; i >= 0; i--) {
    结果_短变量 <<= NB;
    结果_短变量 |= (lua_Unsigned)(unsigned char)串_变量[是小少的吗_圆 ? i : 大小_变量 - 1 - i];
  }
  if (大小_变量 < 串库_大小整型_宏名) {  /* real 大小_变量 smaller than 炉_整数型? */
    if (issigned) {  /* needs sign extension? */
      lua_Unsigned 掩码_变量 = (lua_Unsigned)1 << (大小_变量*NB - 1);
      结果_短变量 = ((结果_短变量 ^ 掩码_变量) - 掩码_变量);  /* do sign extension */
    }
  }
  else if (大小_变量 > 串库_大小整型_宏名) {  /* must 月解析器_检查_函 unread bytes */
    int 掩码_变量 = (!issigned || (炉_整数型)结果_短变量 >= 0) ? 0 : MC;
    for (i = 限制_变量; i < 大小_变量; i++) {
      if (配置_l_可能性低_宏名((unsigned char)串_变量[是小少的吗_圆 ? i : 大小_变量 - 1 - i] != 掩码_变量))
        月亮状态_错误_函(L, "%d-byte integer does not fit into Lua Integer", 大小_变量);
    }
  }
  return (炉_整数型)结果_短变量;
}


static int 月串库_串_反打包_函 (炉_状态机结 *L) {
  头部_结 h;
  const char *格式_短变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  size_t 行目_缩变量;
  const char *数据_变量 = 月亮状态_检查状态字符串_函(L, 2, &行目_缩变量);
  size_t 位置_缩变量 = 月串库_相对位置索引_函(月亮状态_可选整数_函(L, 3, 1), 行目_缩变量) - 1;
  int n = 0;  /* number of results */
  辅助库_月亮l_实参检查_宏名(L, 位置_缩变量 <= 行目_缩变量, 3, "initial position out of string");
  月串库_初始的头部_函(L, &h);
  while (*格式_短变量 != '\0') {
    int 大小_变量, 到对齐数_变量;
    常量选项_枚举 输出_缩变量 = 月串库_获取细节_函(&h, 位置_缩变量, &格式_短变量, &大小_变量, &到对齐数_变量);
    辅助库_月亮l_实参检查_宏名(L, (size_t)到对齐数_变量 + 大小_变量 <= 行目_缩变量 - 位置_缩变量, 2,
                    "数据_变量 string too short");
    位置_缩变量 += 到对齐数_变量;  /* skip alignment */
    /* 栈_圆小 空间_圆 for item + 下一个_变量 position */
    月亮状态_检查栈_函(L, 2, "too many results");
    n++;
    switch (输出_缩变量) {
      case 常量整型_:
      case 常量无符整型_: {
        炉_整数型 结果_短变量 = 月串库_反打包整型_函(L, 数据_变量 + 位置_缩变量, h.是小少的吗_圆, 大小_变量,
                                       (输出_缩变量 == 常量整型_));
        月亮推入整数_函(L, 结果_短变量);
        break;
      }
      case 常量浮点_: {
        float f;
        月串库_带字节序复制_函((char *)&f, 数据_变量 + 位置_缩变量, sizeof(f), h.是小少的吗_圆);
        月亮推入数目_函(L, (炉_数目型)f);
        break;
      }
      case 常量数目_: {
        炉_数目型 f;
        月串库_带字节序复制_函((char *)&f, 数据_变量 + 位置_缩变量, sizeof(f), h.是小少的吗_圆);
        月亮推入数目_函(L, f);
        break;
      }
      case 常量双浮_: {
        double f;
        月串库_带字节序复制_函((char *)&f, 数据_变量 + 位置_缩变量, sizeof(f), h.是小少的吗_圆);
        月亮推入数目_函(L, (炉_数目型)f);
        break;
      }
      case 常量印刻_: {
        月亮推入长字符串_函(L, 数据_变量 + 位置_缩变量, 大小_变量);
        break;
      }
      case 常量字符串_: {
        size_t 长度_短变量 = (size_t)月串库_反打包整型_函(L, 数据_变量 + 位置_缩变量, h.是小少的吗_圆, 大小_变量, 0);
        辅助库_月亮l_实参检查_宏名(L, 长度_短变量 <= 行目_缩变量 - 位置_缩变量 - 大小_变量, 2, "数据_变量 string too short");
        月亮推入长字符串_函(L, 数据_变量 + 位置_缩变量 + 大小_变量, 长度_短变量);
        位置_缩变量 += 长度_短变量;  /* skip string */
        break;
      }
      case 常量零串_: {
        size_t 长度_短变量 = strlen(数据_变量 + 位置_缩变量);
        辅助库_月亮l_实参检查_宏名(L, 位置_缩变量 + 长度_短变量 < 行目_缩变量, 2,
                         "unfinished string for 格式_变量 'z'");
        月亮推入长字符串_函(L, 数据_变量 + 位置_缩变量, 长度_短变量);
        位置_缩变量 += 长度_短变量 + 1;  /* skip string plus 最终_变量 '\0' */
        break;
      }
      case 常量衬垫对齐_: case 常量衬垫_: case 常量无操作_:
        n--;  /* undo increment */
        break;
    }
    位置_缩变量 += 大小_变量;
  }
  月亮推入整数_函(L, 位置_缩变量 + 1);  /* 下一个_变量 position */
  return n + 1;
}

/* }====================================================== */


static const 炉L_寄结 串库_变量[] = {
  {"byte", 月串库_串_字节_函},
  {"char", 月串库_串_印刻_函},
  {"dump", 月串库_串_转储_函},
  {"find", 月串库_串_找_函},
  {"格式_变量", 月串库_串_格式_函},
  {"月串库_全局匹配_函", 月串库_全局匹配_函},
  {"gsub", 月串库_串_全局取代_函},
  {"长度_短变量", 月串库_串_长度_函},
  {"lower", 月串库_串_小写_函},
  {"月串库_匹配_函", 月串库_串_匹配_函},
  {"rep", 月串库_串_替换_函},
  {"月应程接_逆向_函", 月串库_串_逆向_函},
  {"sub", 月串库_串_取代_函},
  {"upper", 月串库_串_大写_函},
  {"pack", 月串库_串_打包_函},
  {"packsize", 月串库_串_打包大小_函},
  {"unpack", 月串库_串_反打包_函},
  {NULL, NULL}
};


static void 月串库_创建元表_函 (炉_状态机结 *L) {
  /* table 到_变量 be 元表_小写 for strings */
  辅助库_月亮l_新库表_宏名(L, 字符串元方法_变量);
  月亮状态_设置函们_函(L, 字符串元方法_变量, 0);
  月头_月亮_推字面_宏名(L, "");  /* 虚假_变量 string */
  月亮_推入值_函(L, -2);  /* copy table */
  月亮_设置元表_函(L, -2);  /* set table as 元表_小写 for strings */
  月头_月亮_弹出_宏名(L, 1);  /* pop 虚假_变量 string */
  月亮_推入值_函(L, -2);  /* get string library */
  月亮设置字段_函(L, -2, "__index");  /* 元表_小写.__index = string */
  月头_月亮_弹出_宏名(L, 1);  /* pop 元表_小写 */
}


/*
** Open string library
*/
配置_月亮模块_应程接_宏名 int 月亮打开字符串_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 串库_变量);
  月串库_创建元表_函(L);
  return 1;
}

