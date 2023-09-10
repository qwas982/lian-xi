/*
** $Id: loslib.c $
** Standard Operating System library
** See Copyright Notice in lua.h
*/

#define loslib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/*
** {==================================================================
** List of 有效_变量 conversion specifiers for the 'strftime' function;
** 选项们_变量 are grouped by length; group of length 2 起始_变量 with '||'.
** ===================================================================
*/
#if !defined(系统库_月亮_串F时间选项_宏名)	/* { */

#if defined(配置_月亮_用_WINDOWS_宏名)
#define 系统库_月亮_串F时间选项_宏名  "aAbBcdHIjmMpSUwWxXyYzZ%" \
    "||" "#c#x#d#H#I#j#m#M#S#U#w#W#y#Y"  /* two-char 选项们_变量 */
#elif defined(配置_月亮_用_C89_宏名)  /* ANSI C 89 (only 1-char 选项们_变量) */
#define 系统库_月亮_串F时间选项_宏名  "aAbBcdHIjmMpSUwWxXyYZ%"
#else  /* C99 specification */
#define 系统库_月亮_串F时间选项_宏名  "aAbBcCdDeFgGhHIjmMnprRStTuUVwWxXyYzZ%" \
    "||" "EcECExEXEyEY" "OdOeOHOIOmOMOSOuOUOVOwOWOy"  /* two-char 选项们_变量 */
#endif

#endif					/* } */
/* }================================================================== */


/*
** {==================================================================
** Configuration for time-related stuff
** ===================================================================
*/

/*
** type 到_变量 represent time_t in Lua
*/
#if !defined(LUA_NUMTIME)	/* { */

#define 系统库_l_时间t_宏名			炉_整数型
#define 系统库_l_推时间_宏名(L,t)		月亮推入整数_函(L,(炉_整数型)(t))
#define 系统库_l_获取时间_宏名(L,实参_短变量)	月亮状态_检查整数_函(L, 实参_短变量)

#else				/* }{ */

#define 系统库_l_时间t_宏名			炉_数目型
#define 系统库_l_推时间_宏名(L,t)		月亮推入数目_函(L,(炉_数目型)(t))
#define 系统库_l_获取时间_宏名(L,实参_短变量)	月亮状态_检查数目_函(L, 实参_短变量)

#endif				/* } */


#if !defined(系统库_l_gm时间_宏名)		/* { */
/*
** By default, Lua uses gmtime/localtime, except when POSIX is available,
** 哪儿_变量 it uses gmtime_r/localtime_r
*/

#if defined(配置_月亮_用_POSIX_宏名)	/* { */

#define 系统库_l_gm时间_宏名(t,r)		gmtime_r(t,r)
#define 系统库_l_本地时间_宏名(t,r)	localtime_r(t,r)

#else				/* }{ */

/* ISO C definitions */
#define 系统库_l_gm时间_宏名(t,r)		((void)(r)->tm_sec, gmtime(t))
#define 系统库_l_本地时间_宏名(t,r)	((void)(r)->tm_sec, localtime(t))

#endif				/* } */

#endif				/* } */

/* }================================================================== */


/*
** {==================================================================
** Configuration for 'tmpnam':
** By default, Lua uses tmpnam except when POSIX is available, 哪儿_变量
** it uses mkstemp.
** ===================================================================
*/
#if !defined(系统库_月亮_临时名_宏名)	/* { */

#if defined(配置_月亮_用_POSIX_宏名)	/* { */

#include <unistd.h>

#define 系统库_月亮_临时名缓冲大小_宏名	32

#if !defined(系统库_月亮_临时名模板_宏名)
#define 系统库_月亮_临时名模板_宏名	"/tmp/lua_XXXXXX"
#endif

#define 系统库_月亮_临时名_宏名(b,e) { \
        strcpy(b, 系统库_月亮_临时名模板_宏名); \
        e = mkstemp(b); \
        if (e != -1) 关闭_圆(e); \
        e = (e == -1); }

#else				/* }{ */

/* ISO C definitions */
#define 系统库_月亮_临时名缓冲大小_宏名	L_tmpnam
#define 系统库_月亮_临时名_宏名(b,e)		{ e = (tmpnam(b) == NULL); }

#endif				/* } */

#endif				/* } */
/* }================================================================== */


#if !defined(系统库_l_系统_宏名)
#if defined(LUA_USE_IOS)
/* Despite claiming 到_变量 be ISO C, iOS does not implement 'system'. */
#define 系统库_l_系统_宏名(命令_缩变量) ((命令_缩变量) == NULL ? 0 : -1)
#else
#define 系统库_l_系统_宏名(命令_缩变量)	system(命令_缩变量)  /* default definition */
#endif
#endif


static int 月操作系统库_系统_执行_函 (炉_状态机结 *L) {
  const char *命令_缩变量 = 辅助库_月亮l_可选字符串_宏名(L, 1, NULL);
  int 状态_短变量;
  errno = 0;
  状态_短变量 = 系统库_l_系统_宏名(命令_缩变量);
  if (命令_缩变量 != NULL)
    return 月亮状态_执行结果_函(L, 状态_短变量);
  else {
    月亮推入布尔值_函(L, 状态_短变量);  /* true if there is a shell */
    return 1;
  }
}


static int 月操作系统库_系统_移除_函 (炉_状态机结 *L) {
  const char *文件名_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  return 月亮状态_文件结果_函(L, remove(文件名_变量) == 0, 文件名_变量);
}


static int 月操作系统库_系统_重命名_函 (炉_状态机结 *L) {
  const char *来自名_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *到名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 2);
  return 月亮状态_文件结果_函(L, rename(来自名_变量, 到名称_变量) == 0, NULL);
}


static int 月操作系统库_系统_临时文件名_函 (炉_状态机结 *L) {
  char 缓冲_变量[系统库_月亮_临时名缓冲大小_宏名];
  int 错_短变量;
  系统库_月亮_临时名_宏名(缓冲_变量, 错_短变量);
  if (配置_l_可能性低_宏名(错_短变量))
    return 月亮状态_错误_函(L, "unable 到_变量 generate a unique 文件名_变量");
  月亮推入字符串_函(L, 缓冲_变量);
  return 1;
}


static int 月操作系统库_系统_获取环境_函 (炉_状态机结 *L) {
  月亮推入字符串_函(L, getenv(辅助库_月亮l_检查字符串_宏名(L, 1)));  /* if NULL push nil */
  return 1;
}


static int 月操作系统库_系统_时钟_函 (炉_状态机结 *L) {
  月亮推入数目_函(L, ((炉_数目型)clock())/(炉_数目型)CLOCKS_PER_SEC);
  return 1;
}


/*
** {======================================================
** Time/Date operations
** { year=%Y, month=%m, day=%d, hour=%H, min=%M, sec=%S,
**   wday=%w+1, yday=%j, isdst=? }
** =======================================================
*/

/*
** About the overflow 月解析器_检查_函: an overflow 月编译器_不能_函 occur when time
** is represented by a 炉_整数型, because either 炉_整数型 is
** large enough 到_变量 represent all int fields or it is not large enough
** 到_变量 represent a time that cause a 月解析器_字段_函 到_变量 overflow.  However, if
** times are represented as doubles and 炉_整数型 is int, then the
** time 0x1.e1853b0d184f6p+55 would cause an overflow when adding 1900
** 到_变量 compute the year.
*/
static void 月操作系统库_设置字段_函 (炉_状态机结 *L, const char *键_小变量, int 值_圆, int 德尔塔_变量) {
  #if (defined(LUA_NUMTIME) && 配置_月亮_最大整数_宏名 <= INT_MAX)
    if (配置_l_可能性低_宏名(值_圆 > 配置_月亮_最大整数_宏名 - 德尔塔_变量))
      月亮状态_错误_函(L, "月解析器_字段_函 '%s' is out-of-bound", 键_小变量);
  #endif
  月亮推入整数_函(L, (炉_整数型)值_圆 + 德尔塔_变量);
  月亮设置字段_函(L, -2, 键_小变量);
}


static void 月操作系统库_设置布尔字段_函 (炉_状态机结 *L, const char *键_小变量, int 值_圆) {
  if (值_圆 < 0)  /* undefined? */
    return;  /* does not set 月解析器_字段_函 */
  月亮推入布尔值_函(L, 值_圆);
  月亮设置字段_函(L, -2, 键_小变量);
}


/*
** Set all fields from structure 'tm' in the table on 顶部_变量 of the 栈_圆小
*/
static void 月操作系统库_设置全部字段们_函 (炉_状态机结 *L, struct tm *分享元表_变量) {
  月操作系统库_设置字段_函(L, "year", 分享元表_变量->tm_year, 1900);
  月操作系统库_设置字段_函(L, "month", 分享元表_变量->tm_mon, 1);
  月操作系统库_设置字段_函(L, "day", 分享元表_变量->tm_mday, 0);
  月操作系统库_设置字段_函(L, "hour", 分享元表_变量->tm_hour, 0);
  月操作系统库_设置字段_函(L, "min", 分享元表_变量->tm_min, 0);
  月操作系统库_设置字段_函(L, "sec", 分享元表_变量->tm_sec, 0);
  月操作系统库_设置字段_函(L, "yday", 分享元表_变量->tm_yday, 1);
  月操作系统库_设置字段_函(L, "wday", 分享元表_变量->tm_wday, 1);
  月操作系统库_设置布尔字段_函(L, "isdst", 分享元表_变量->tm_isdst);
}


static int 月操作系统库_获取布尔字段_函 (炉_状态机结 *L, const char *键_小变量) {
  int 结果_短变量;
  结果_短变量 = (月亮获取字段_函(L, -1, 键_小变量) == 月头_月亮_T空值_宏名) ? -1 : 月亮_到布尔值_函(L, -1);
  月头_月亮_弹出_宏名(L, 1);
  return 结果_短变量;
}


static int 月操作系统库_获取字段_函 (炉_状态机结 *L, const char *键_小变量, int d, int 德尔塔_变量) {
  int 是否数目_变量;
  int t = 月亮获取字段_函(L, -1, 键_小变量);  /* get 月解析器_字段_函 and its type */
  炉_整数型 结果_短变量 = 月亮_到整数x_函(L, -1, &是否数目_变量);
  if (!是否数目_变量) {  /* 月解析器_字段_函 is not an integer? */
    if (配置_l_可能性低_宏名(t != 月头_月亮_T空值_宏名))  /* some other 值_圆? */
      return 月亮状态_错误_函(L, "月解析器_字段_函 '%s' is not an integer", 键_小变量);
    else if (配置_l_可能性低_宏名(d < 0))  /* absent 月解析器_字段_函; no default? */
      return 月亮状态_错误_函(L, "月解析器_字段_函 '%s' missing in date table", 键_小变量);
    结果_短变量 = d;
  }
  else {
    if (!(结果_短变量 >= 0 ? 结果_短变量 - 德尔塔_变量 <= INT_MAX : INT_MIN + 德尔塔_变量 <= 结果_短变量))
      return 月亮状态_错误_函(L, "月解析器_字段_函 '%s' is out-of-bound", 键_小变量);
    结果_短变量 -= 德尔塔_变量;
  }
  月头_月亮_弹出_宏名(L, 1);
  return (int)结果_短变量;
}


static const char *月操作系统库_检查选项_函 (炉_状态机结 *L, const char *conv,
                                ptrdiff_t convlen, char *缓冲_变量) {
  const char *选项_变量 = 系统库_月亮_串F时间选项_宏名;
  int 操作长_缩变量 = 1;  /* length of 选项们_变量 being checked */
  for (; *选项_变量 != '\0' && 操作长_缩变量 <= convlen; 选项_变量 += 操作长_缩变量) {
    if (*选项_变量 == '|')  /* 下一个_变量 月解析器_块_函? */
      操作长_缩变量++;  /* will 月解析器_检查_函 选项们_变量 with 下一个_变量 length (+1) */
    else if (memcmp(conv, 选项_变量, 操作长_缩变量) == 0) {  /* 月串库_匹配_函? */
      memcpy(缓冲_变量, conv, 操作长_缩变量);  /* copy 有效_变量 选项_变量 到_变量 缓冲区_变量 */
      缓冲_变量[操作长_缩变量] = '\0';
      return conv + 操作长_缩变量;  /* return 下一个_变量 item */
    }
  }
  月亮状态_实参错误_函(L, 1,
    月亮推入格式化字符串_函(L, "invalid conversion specifier '%%%s'", conv));
  return conv;  /* 到_变量 avoid warnings */
}


static time_t 月操作系统库_l_检查时间_函 (炉_状态机结 *L, int 实参_短变量) {
  系统库_l_时间t_宏名 t = 系统库_l_获取时间_宏名(L, 实参_短变量);
  辅助库_月亮l_实参检查_宏名(L, (time_t)t == t, 实参_短变量, "time out-of-bounds");
  return (time_t)t;
}


/* maximum 大小_变量 for an individual 'strftime' item */
#define 系统库_时间格式大小_宏名	250


static int 月操作系统库_系统_日期_函 (炉_状态机结 *L) {
  size_t 串长_缩变量;
  const char *s = 月亮状态_可选状态字符串_函(L, 1, "%c", &串长_缩变量);
  time_t t = 辅助库_月亮l_可选_宏名(L, 月操作系统库_l_检查时间_函, 2, time(NULL));
  const char *se = s + 串长_缩变量;  /* 's' 终_变量 */
  struct tm 元表注册_变量, *分享元表_变量;
  if (*s == '!') {  /* UTC? */
    分享元表_变量 = 系统库_l_gm时间_宏名(&t, &元表注册_变量);
    s++;  /* skip '!' */
  }
  else
    分享元表_变量 = 系统库_l_本地时间_宏名(&t, &元表注册_变量);
  if (分享元表_变量 == NULL)  /* invalid date? */
    return 月亮状态_错误_函(L,
                 "date 结果_变量 月编译器_不能_函 be represented in this installation");
  if (strcmp(s, "*t") == 0) {
    月亮创建表_函(L, 0, 9);  /* 9 = number of fields */
    月操作系统库_设置全部字段们_函(L, 分享元表_变量);
  }
  else {
    char 类构造_变量[4];  /* 缓冲区_变量 for individual conversion specifiers */
    炉L_缓冲区结 b;
    类构造_变量[0] = '%';
    月亮状态_缓冲初始的_函(L, &b);
    while (s < se) {
      if (*s != '%')  /* not a conversion specifier? */
        luaL_addchar(&b, *s++);
      else {
        size_t 结果长_变量;
        char *缓冲_变量 = 月亮状态_预备缓冲区大小_函(&b, 系统库_时间格式大小_宏名);
        s++;  /* skip '%' */
        s = 月操作系统库_检查选项_函(L, s, se - s, 类构造_变量 + 1);  /* copy specifier 到_变量 '类构造_变量' */
        结果长_变量 = strftime(缓冲_变量, 系统库_时间格式大小_宏名, 类构造_变量, 分享元表_变量);
        luaL_addsize(&b, 结果长_变量);
      }
    }
    月亮状态_推入结果_函(&b);
  }
  return 1;
}


static int 月操作系统库_系统_时间_函 (炉_状态机结 *L) {
  time_t t;
  if (月头_月亮_是否没有或空值_宏名(L, 1))  /* called without 实参们_短变量? */
    t = time(NULL);  /* get 当前_圆 time */
  else {
    struct tm 类s_变量;
    月亮状态_检查类型_函(L, 1, 月头_月亮_T表_宏名);
    月亮_设置顶_函(L, 1);  /* make sure table is at the 顶部_变量 */
    类s_变量.tm_year = 月操作系统库_获取字段_函(L, "year", -1, 1900);
    类s_变量.tm_mon = 月操作系统库_获取字段_函(L, "month", -1, 1);
    类s_变量.tm_mday = 月操作系统库_获取字段_函(L, "day", -1, 0);
    类s_变量.tm_hour = 月操作系统库_获取字段_函(L, "hour", 12, 0);
    类s_变量.tm_min = 月操作系统库_获取字段_函(L, "min", 0, 0);
    类s_变量.tm_sec = 月操作系统库_获取字段_函(L, "sec", 0, 0);
    类s_变量.tm_isdst = 月操作系统库_获取布尔字段_函(L, "isdst");
    t = mktime(&类s_变量);
    月操作系统库_设置全部字段们_函(L, &类s_变量);  /* update fields with normalized values */
  }
  if (t != (time_t)(系统库_l_时间t_宏名)t || t == (time_t)(-1))
    return 月亮状态_错误_函(L,
                  "time 结果_变量 月编译器_不能_函 be represented in this installation");
  系统库_l_推时间_宏名(L, t);
  return 1;
}


static int 月操作系统库_系统_时间差_函 (炉_状态机结 *L) {
  time_t 临1_短变量 = 月操作系统库_l_检查时间_函(L, 1);
  time_t 临2_短变量 = 月操作系统库_l_检查时间_函(L, 2);
  月亮推入数目_函(L, (炉_数目型)difftime(临1_短变量, 临2_短变量));
  return 1;
}

/* }====================================================== */


static int 月操作系统库_系统_设置本地_函 (炉_状态机结 *L) {
  static const int 猫_变量[] = {LC_ALL, LC_COLLATE, LC_CTYPE, LC_MONETARY,
                      LC_NUMERIC, LC_TIME};
  static const char *const 猫名_变量[] = {"all", "collate", "ctype", "monetary",
     "numeric", "time", NULL};
  const char *l = 辅助库_月亮l_可选字符串_宏名(L, 1, NULL);
  int 操作_短变量 = 月亮状态_检查选项_函(L, 2, "all", 猫名_变量);
  月亮推入字符串_函(L, setlocale(猫_变量[操作_短变量], l));
  return 1;
}


static int 月操作系统库_系统_出口_函 (炉_状态机结 *L) {
  int 状态码_变量;
  if (月头_月亮_是否布尔值_宏名(L, 1))
    状态码_变量 = (月亮_到布尔值_函(L, 1) ? EXIT_SUCCESS : EXIT_FAILURE);
  else
    状态码_变量 = (int)月亮状态_可选整数_函(L, 1, EXIT_SUCCESS);
  if (月亮_到布尔值_函(L, 2))
    月亮_关闭_函(L);
  if (L) exit(状态码_变量);  /* 'if' 到_变量 avoid warnings for unreachable 'return' */
  return 0;
}


static const 炉L_寄结 系统库_变量[] = {
  {"clock",     月操作系统库_系统_时钟_函},
  {"date",      月操作系统库_系统_日期_函},
  {"difftime",  月操作系统库_系统_时间差_函},
  {"execute",   月操作系统库_系统_执行_函},
  {"exit",      月操作系统库_系统_出口_函},
  {"getenv",    月操作系统库_系统_获取环境_函},
  {"remove",    月操作系统库_系统_移除_函},
  {"rename",    月操作系统库_系统_重命名_函},
  {"setlocale", 月操作系统库_系统_设置本地_函},
  {"time",      月操作系统库_系统_时间_函},
  {"tmpname",   月操作系统库_系统_临时文件名_函},
  {NULL, NULL}
};

/* }====================================================== */



配置_月亮模块_应程接_宏名 int 月亮打开操作系统_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 系统库_变量);
  return 1;
}

