/*
** $Id: lutf8lib.c $
** Standard library for UTF-8 manipulation
** See Copyright Notice in lua.h
*/

#define lutf8lib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#define utf8库_最大Unicode值_宏名	0x10FFFFu

#define utf8库_最大UTF值_宏名		0x7FFFFFFFu


#define utf8库_消息无效_宏名	"invalid UTF-8 代码_变量"

/*
** Integer type for decoded UTF-8 values; utf8库_最大UTF值_宏名 needs 31 bits.
*/
#if (UINT_MAX >> 30) >= 1
typedef unsigned int utfint;
#else
typedef unsigned long utfint;
#endif


#define utf8库_是否继续_宏名(c)	(((c) & 0xC0) == 0x80)
#define utf8库_是否继续指针_宏名(p)	utf8库_是否继续_宏名(*(p))


/* from 串库_变量 */
/* translate a relative string position: negative means back from 终_变量 */
static 炉_整数型 月utf8库_u_相对位置_函 (炉_整数型 位置_缩变量, size_t 长度_短变量) {
  if (位置_缩变量 >= 0) return 位置_缩变量;
  else if (0u - (size_t)位置_缩变量 > 长度_短变量) return 0;
  else return (炉_整数型)长度_短变量 + 位置_缩变量 + 1;
}


/*
** Decode one UTF-8 sequence, returning NULL if byte sequence is
** invalid.  The 数组_圆 '限制们_变量' stores the minimum 值_圆 for each
** sequence length, 到_变量 月解析器_检查_函 for overlong representations. Its 首先_变量
** entry forces an 错误_小变量 for non-ascii bytes with no continuation
** bytes (计数_变量 == 0).
*/
static const char *月utf8库_UTF8_解码_函 (const char *s, utfint *值_变量, int strict) {
  static const utfint 限制们_变量[] =
        {~(utfint)0, 0x80, 0x800, 0x10000u, 0x200000u, 0x4000000u};
  unsigned int c = (unsigned char)s[0];
  utfint 结果_短变量 = 0;  /* 最终_变量 结果_变量 */
  if (c < 0x80)  /* ascii? */
    结果_短变量 = c;
  else {
    int 计数_变量 = 0;  /* 到_变量 计数_变量 number of continuation bytes */
    for (; c & 0x40; c <<= 1) {  /* while it needs continuation bytes... */
      unsigned int 类构造_变量 = (unsigned char)s[++计数_变量];  /* read 下一个_变量 byte */
      if (!utf8库_是否继续_宏名(类构造_变量))  /* not a continuation byte? */
        return NULL;  /* invalid byte sequence */
      结果_短变量 = (结果_短变量 << 6) | (类构造_变量 & 0x3F);  /* add lower 6 bits from 继续_短变量. byte */
    }
    结果_短变量 |= ((utfint)(c & 0x7F) << (计数_变量 * 5));  /* add 首先_变量 byte */
    if (计数_变量 > 5 || 结果_短变量 > utf8库_最大UTF值_宏名 || 结果_短变量 < 限制们_变量[计数_变量])
      return NULL;  /* invalid byte sequence */
    s += 计数_变量;  /* skip continuation bytes read */
  }
  if (strict) {
    /* 月解析器_检查_函 for invalid 代码_变量 points; too large or surrogates */
    if (结果_短变量 > utf8库_最大Unicode值_宏名 || (0xD800u <= 结果_短变量 && 结果_短变量 <= 0xDFFFu))
      return NULL;
  }
  if (值_变量) *值_变量 = 结果_短变量;
  return s + 1;  /* +1 到_变量 include 首先_变量 byte */
}


/*
** utf8len(s [, i [, j [, 局辅_变量]]]) --> number of characters that
** 起始_变量 in the range [i,j], or nil + 当前_圆 position if 's' is not
** well formed in that interval
*/
static int 月utf8库_UTF长度_函 (炉_状态机结 *L) {
  炉_整数型 n = 0;  /* counter for the number of characters */
  size_t 长度_短变量;  /* string length in bytes */
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &长度_短变量);
  炉_整数型 位置i_变量 = 月utf8库_u_相对位置_函(月亮状态_可选整数_函(L, 2, 1), 长度_短变量);
  炉_整数型 位置j_变量 = 月utf8库_u_相对位置_函(月亮状态_可选整数_函(L, 3, -1), 长度_短变量);
  int 局辅_变量 = 月亮_到布尔值_函(L, 4);
  辅助库_月亮l_实参检查_宏名(L, 1 <= 位置i_变量 && --位置i_变量 <= (炉_整数型)长度_短变量, 2,
                   "initial position out of bounds");
  辅助库_月亮l_实参检查_宏名(L, --位置j_变量 < (炉_整数型)长度_短变量, 3,
                   "最终_变量 position out of bounds");
  while (位置i_变量 <= 位置j_变量) {
    const char *s1 = 月utf8库_UTF8_解码_函(s + 位置i_变量, NULL, !局辅_变量);
    if (s1 == NULL) {  /* conversion 错误_小变量? */
      辅助库_月亮l_推失败_宏名(L);  /* return fail ... */
      月亮推入整数_函(L, 位置i_变量 + 1);  /* ... and 当前_圆 position */
      return 2;
    }
    位置i_变量 = s1 - s;
    n++;
  }
  月亮推入整数_函(L, n);
  return 1;
}


/*
** 月utf8库_代码点_函(s, [i, [j [, 局辅_变量]]]) -> returns codepoints for all
** characters that 起始_变量 in the range [i,j]
*/
static int 月utf8库_代码点_函 (炉_状态机结 *L) {
  size_t 长度_短变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &长度_短变量);
  炉_整数型 位置i_变量 = 月utf8库_u_相对位置_函(月亮状态_可选整数_函(L, 2, 1), 长度_短变量);
  炉_整数型 位置e_变量 = 月utf8库_u_相对位置_函(月亮状态_可选整数_函(L, 3, 位置i_变量), 长度_短变量);
  int 局辅_变量 = 月亮_到布尔值_函(L, 4);
  int n;
  const char *se;
  辅助库_月亮l_实参检查_宏名(L, 位置i_变量 >= 1, 2, "out of bounds");
  辅助库_月亮l_实参检查_宏名(L, 位置e_变量 <= (炉_整数型)长度_短变量, 3, "out of bounds");
  if (位置i_变量 > 位置e_变量) return 0;  /* 空容器_变量 interval; return no values */
  if (位置e_变量 - 位置i_变量 >= INT_MAX)  /* (炉_整数型 -> int) overflow? */
    return 月亮状态_错误_函(L, "string slice too long");
  n = (int)(位置e_变量 -  位置i_变量) + 1;  /* upper bound for number of returns */
  月亮状态_检查栈_函(L, n, "string slice too long");
  n = 0;  /* 计数_变量 the number of returns */
  se = s + 位置e_变量;  /* string 终_变量 */
  for (s += 位置i_变量 - 1; s < se;) {
    utfint 代码_变量;
    s = 月utf8库_UTF8_解码_函(s, &代码_变量, !局辅_变量);
    if (s == NULL)
      return 月亮状态_错误_函(L, utf8库_消息无效_宏名);
    月亮推入整数_函(L, 代码_变量);
    n++;
  }
  return n;
}


static void 月utf8库_推UTF印刻_函 (炉_状态机结 *L, int 实参_短变量) {
  lua_Unsigned 代码_变量 = (lua_Unsigned)月亮状态_检查整数_函(L, 实参_短变量);
  辅助库_月亮l_实参检查_宏名(L, 代码_变量 <= utf8库_最大UTF值_宏名, 实参_短变量, "值_圆 out of range");
  月亮推入格式化字符串_函(L, "%U", (long)代码_变量);
}


/*
** 月utf8库_UTF印刻_函(数1_缩变量, 数2_缩变量, ...)  -> char(数1_缩变量)..char(数2_缩变量)...
*/
static int 月utf8库_UTF印刻_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  if (n == 1)  /* optimize common case of single char */
    月utf8库_推UTF印刻_函(L, 1);
  else {
    int i;
    炉L_缓冲区结 b;
    月亮状态_缓冲初始的_函(L, &b);
    for (i = 1; i <= n; i++) {
      月utf8库_推UTF印刻_函(L, i);
      月亮状态_添加值_函(&b);
    }
    月亮状态_推入结果_函(&b);
  }
  return 1;
}


/*
** 偏移_变量(s, n, [i])  -> index 哪儿_变量 n-到钩_变量 character counting from
**   position 'i' starts; 0 means character at 'i'.
*/
static int 月utf8库_字节偏移_函 (炉_状态机结 *L) {
  size_t 长度_短变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &长度_短变量);
  炉_整数型 n  = 月亮状态_检查整数_函(L, 2);
  炉_整数型 位置i_变量 = (n >= 0) ? 1 : 长度_短变量 + 1;
  位置i_变量 = 月utf8库_u_相对位置_函(月亮状态_可选整数_函(L, 3, 位置i_变量), 长度_短变量);
  辅助库_月亮l_实参检查_宏名(L, 1 <= 位置i_变量 && --位置i_变量 <= (炉_整数型)长度_短变量, 3,
                   "position out of bounds");
  if (n == 0) {
    /* find beginning of 当前_圆 byte sequence */
    while (位置i_变量 > 0 && utf8库_是否继续指针_宏名(s + 位置i_变量)) 位置i_变量--;
  }
  else {
    if (utf8库_是否继续指针_宏名(s + 位置i_变量))
      return 月亮状态_错误_函(L, "initial position is a continuation byte");
    if (n < 0) {
       while (n < 0 && 位置i_变量 > 0) {  /* move back */
         do {  /* find beginning of 前一个_变量 character */
           位置i_变量--;
         } while (位置i_变量 > 0 && utf8库_是否继续指针_宏名(s + 位置i_变量));
         n++;
       }
     }
     else {
       n--;  /* do not move for 1st character */
       while (n > 0 && 位置i_变量 < (炉_整数型)长度_短变量) {
         do {  /* find beginning of 下一个_变量 character */
           位置i_变量++;
         } while (utf8库_是否继续指针_宏名(s + 位置i_变量));  /* (月编译器_不能_函 pass 最终_变量 '\0') */
         n--;
       }
     }
  }
  if (n == 0)  /* did it find given character? */
    月亮推入整数_函(L, 位置i_变量 + 1);
  else  /* no such character */
    辅助库_月亮l_推失败_宏名(L);
  return 1;
}


static int 月utf8库_迭代_辅助_函 (炉_状态机结 *L, int strict) {
  size_t 长度_短变量;
  const char *s = 月亮状态_检查状态字符串_函(L, 1, &长度_短变量);
  lua_Unsigned n = (lua_Unsigned)月头_月亮_到整数_宏名(L, 2);
  if (n < 长度_短变量) {
    while (utf8库_是否继续指针_宏名(s + n)) n++;  /* go 到_变量 下一个_变量 character */
  }
  if (n >= 长度_短变量)  /* (also handles original 'n' being negative) */
    return 0;  /* no 更多_变量 codepoints */
  else {
    utfint 代码_变量;
    const char *下一个_变量 = 月utf8库_UTF8_解码_函(s + n, &代码_变量, strict);
    if (下一个_变量 == NULL || utf8库_是否继续指针_宏名(下一个_变量))
      return 月亮状态_错误_函(L, utf8库_消息无效_宏名);
    月亮推入整数_函(L, n + 1);
    月亮推入整数_函(L, 代码_变量);
    return 2;
  }
}


static int 月utf8库_迭代_辅助严格_函 (炉_状态机结 *L) {
  return 月utf8库_迭代_辅助_函(L, 1);
}

static int 月utf8库_迭代_辅助宽松_函 (炉_状态机结 *L) {
  return 月utf8库_迭代_辅助_函(L, 0);
}


static int 月utf8库_迭代_代码们_函 (炉_状态机结 *L) {
  int 局辅_变量 = 月亮_到布尔值_函(L, 2);
  const char *s = 辅助库_月亮l_检查字符串_宏名(L, 1);
  辅助库_月亮l_实参检查_宏名(L, !utf8库_是否继续指针_宏名(s), 1, utf8库_消息无效_宏名);
  月头_月亮_推C函数_宏名(L, 局辅_变量 ? 月utf8库_迭代_辅助宽松_函 : 月utf8库_迭代_辅助严格_函);
  月亮_推入值_函(L, 1);
  月亮推入整数_函(L, 0);
  return 3;
}


/* pattern 到_变量 月串库_匹配_函 a single UTF-8 character */
#define utf8库_UTF8模式_宏名	"[\0-\x7F\xC2-\xFD][\x80-\xBF]*"


static const 炉L_寄结 函们_变量[] = {
  {"偏移_变量", 月utf8库_字节偏移_函},
  {"月utf8库_代码点_函", 月utf8库_代码点_函},
  {"char", 月utf8库_UTF印刻_函},
  {"长度_短变量", 月utf8库_UTF长度_函},
  {"codes", 月utf8库_迭代_代码们_函},
  /* placeholders */
  {"charpattern", NULL},
  {NULL, NULL}
};


配置_月亮模块_应程接_宏名 int 月亮打开UTF8_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 函们_变量);
  月亮推入长字符串_函(L, utf8库_UTF8模式_宏名, sizeof(utf8库_UTF8模式_宏名)/sizeof(char) - 1);
  月亮设置字段_函(L, -2, "charpattern");
  return 1;
}

