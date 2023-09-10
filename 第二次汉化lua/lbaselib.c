/*
** $Id: lbaselib.c $
** Basic library
** See Copyright Notice in lua.h
*/

#define lbaselib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


static int 月基本库_月亮B_打印_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  int i;
  for (i = 1; i <= n; i++) {  /* for each argument */
    size_t l;
    const char *s = 月亮状态_到字符串_函(L, i, &l);  /* convert it 到_变量 string */
    if (i > 1)  /* not the 首先_变量 element? */
      辅助库_月亮_写字符串_宏名("\t", 1);  /* add a 表_短_变量 before it */
    辅助库_月亮_写字符串_宏名(s, l);  /* print it */
    月头_月亮_弹出_宏名(L, 1);  /* pop 结果_变量 */
  }
  辅助库_月亮_写行_宏名();
  return 0;
}


/*
** Creates a warning with all given arguments.
** Check 首先_变量 for errors; otherwise an 错误_小变量 may interrupt
** the composition of a warning, leaving it unfinished.
*/
static int 月基本库_月亮B_警告_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);  /* number of arguments */
  int i;
  辅助库_月亮l_检查字符串_宏名(L, 1);  /* at least one argument */
  for (i = 2; i <= n; i++)
    辅助库_月亮l_检查字符串_宏名(L, i);  /* make sure all arguments are strings */
  for (i = 1; i < n; i++)  /* compose warning */
    月亮_警告_函(L, 月头_月亮_到字符串_宏名(L, i), 1);
  月亮_警告_函(L, 月头_月亮_到字符串_宏名(L, n), 0);  /* 关闭_圆 warning */
  return 0;
}


#define 基本库_空格印刻_宏名	" \f\n\r\t\v"

static const char *月基本库_b_串到整型_函 (const char *s, int 基本_变量, 炉_整数型 *pn) {
  lua_Unsigned n = 0;
  int 负号_短变量 = 0;
  s += strspn(s, 基本库_空格印刻_宏名);  /* skip initial spaces */
  if (*s == '-') { s++; 负号_短变量 = 1; }  /* handle sign */
  else if (*s == '+') s++;
  if (!isalnum((unsigned char)*s))  /* no 数字_变量? */
    return NULL;
  do {
    int 数字_变量 = (isdigit((unsigned char)*s)) ? *s - '0'
                   : (toupper((unsigned char)*s) - 'A') + 10;
    if (数字_变量 >= 基本_变量) return NULL;  /* invalid numeral */
    n = n * 基本_变量 + 数字_变量;
    s++;
  } while (isalnum((unsigned char)*s));
  s += strspn(s, 基本库_空格印刻_宏名);  /* skip trailing spaces */
  *pn = (炉_整数型)((负号_短变量) ? (0u - n) : n);
  return s;
}


static int 月基本库_月亮B_到数目_函 (炉_状态机结 *L) {
  if (月头_月亮_是否没有或空值_宏名(L, 2)) {  /* standard conversion? */
    if (月亮_类型_函(L, 1) == 月头_月亮_T数目_宏名) {  /* already a number? */
      月亮_设置顶_函(L, 1);  /* yes; return it */
      return 1;
    }
    else {
      size_t l;
      const char *s = 月亮_到长字符串_函(L, 1, &l);
      if (s != NULL && 月亮_字符串到数目_函(L, s) == l + 1)
        return 1;  /* successful conversion 到_变量 number */
      /* else not a number */
      月亮状态_检查任意_函(L, 1);  /* (but there must be some parameter) */
    }
  }
  else {
    size_t l;
    const char *s;
    炉_整数型 n = 0;  /* 到_变量 avoid warnings */
    炉_整数型 基本_变量 = 月亮状态_检查整数_函(L, 2);
    月亮状态_检查类型_函(L, 1, 月头_月亮_T字符串_宏名);  /* no numbers as strings */
    s = 月亮_到长字符串_函(L, 1, &l);
    辅助库_月亮l_实参检查_宏名(L, 2 <= 基本_变量 && 基本_变量 <= 36, 2, "基本_变量 out of range");
    if (月基本库_b_串到整型_函(s, (int)基本_变量, &n) == s + l) {
      月亮推入整数_函(L, n);
      return 1;
    }  /* else not a number */
  }  /* else not a number */
  辅助库_月亮l_推失败_宏名(L);  /* not a number */
  return 1;
}


static int 月基本库_月亮B_错误_函 (炉_状态机结 *L) {
  int 层级_变量 = (int)月亮状态_可选整数_函(L, 2, 1);
  月亮_设置顶_函(L, 1);
  if (月亮_类型_函(L, 1) == 月头_月亮_T字符串_宏名 && 层级_变量 > 0) {
    月亮状态_哪里_函(L, 层级_变量);   /* add 额外_变量 information */
    月亮_推入值_函(L, 1);
    月亮_拼接_函(L, 2);
  }
  return 月亮_错误_函(L);
}


static int 月基本库_月亮B_获取元表_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  if (!月亮获取元表_函(L, 1)) {
    月亮推入空值_函(L);
    return 1;  /* no 元表_小写 */
  }
  月亮状态_获取元字段_函(L, 1, "__metatable");
  return 1;  /* returns either __metatable 月解析器_字段_函 (if present) or 元表_小写 */
}


static int 月基本库_月亮B_设置元表_函 (炉_状态机结 *L) {
  int t = 月亮_类型_函(L, 2);
  月亮状态_检查类型_函(L, 1, 月头_月亮_T表_宏名);
  辅助库_月亮l_实参期望_宏名(L, t == 月头_月亮_T空值_宏名 || t == 月头_月亮_T表_宏名, 2, "nil or table");
  if (配置_l_可能性低_宏名(月亮状态_获取元字段_函(L, 1, "__metatable") != 月头_月亮_T空值_宏名))
    return 月亮状态_错误_函(L, "月编译器_不能_函 改变_变量 a protected 元表_小写");
  月亮_设置顶_函(L, 2);
  月亮_设置元表_函(L, 1);
  return 1;
}


static int 月基本库_月亮B_原始相等_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  月亮状态_检查任意_函(L, 2);
  月亮推入布尔值_函(L, 月亮原始相等_函(L, 1, 2));
  return 1;
}


static int 月基本库_月亮B_原始长度_函 (炉_状态机结 *L) {
  int t = 月亮_类型_函(L, 1);
  辅助库_月亮l_实参期望_宏名(L, t == 月头_月亮_T表_宏名 || t == 月头_月亮_T字符串_宏名, 1,
                      "table or string");
  月亮推入整数_函(L, 月亮_原始长度_函(L, 1));
  return 1;
}


static int 月基本库_月亮B_原始获取_函 (炉_状态机结 *L) {
  月亮状态_检查类型_函(L, 1, 月头_月亮_T表_宏名);
  月亮状态_检查任意_函(L, 2);
  月亮_设置顶_函(L, 2);
  月亮获取原始_函(L, 1);
  return 1;
}

static int 月基本库_月亮B_原始设置_函 (炉_状态机结 *L) {
  月亮状态_检查类型_函(L, 1, 月头_月亮_T表_宏名);
  月亮状态_检查任意_函(L, 2);
  月亮状态_检查任意_函(L, 3);
  月亮_设置顶_函(L, 3);
  月亮_设置原始_函(L, 1);
  return 1;
}


static int 月基本库_推模式_函 (炉_状态机结 *L, int oldmode) {
  if (oldmode == -1)
    辅助库_月亮l_推失败_宏名(L);  /* invalid call 到_变量 '月亮_垃圾回收_函' */
  else
    月亮推入字符串_函(L, (oldmode == 月头_月亮_垃圾回收递增_宏名) ? "incremental"
                                             : "generational");
  return 1;
}


/*
** 月解析器_检查_函 whether call 到_变量 '月亮_垃圾回收_函' was 有效_变量 (not inside a finalizer)
*/
#define 基本库_检查值结果_宏名(结果_短变量) { if (结果_短变量 == -1) break; }

static int 月基本库_月亮B_回收垃圾_函 (炉_状态机结 *L) {
  static const char *const 选项_短变量[] = {"stop", "restart", "collect",
    "计数_变量", "步进_变量", "月垃圾回收_设置暂停_函", "setstepmul",
    "isrunning", "generational", "incremental", NULL};
  static const int 选项数目_短变量[] = {月头_月亮_垃圾回收停止_宏名, 月头_月亮_垃圾回收重启_宏名, 月头_月亮_垃圾回收收集_宏名,
    月头_月亮_垃圾回收计数_宏名, 月头_月亮_垃圾回收步进_宏名, 月头_月亮_垃圾回收设置暂停_宏名, 月头_月亮_垃圾回收设置步进乘法_宏名,
    月头_月亮_垃圾回收是否正在跑_宏名, 月头_月亮_垃圾回收生成_宏名, 月头_月亮_垃圾回收递增_宏名};
  int o = 选项数目_短变量[月亮状态_检查选项_函(L, 1, "collect", 选项_短变量)];
  switch (o) {
    case 月头_月亮_垃圾回收计数_宏名: {
      int k = 月亮_垃圾回收_函(L, o);
      int b = 月亮_垃圾回收_函(L, 月头_月亮_垃圾回收计数B_宏名);
      基本库_检查值结果_宏名(k);
      月亮推入数目_函(L, (炉_数目型)k + ((炉_数目型)b/1024));
      return 1;
    }
    case 月头_月亮_垃圾回收步进_宏名: {
      int 步进_变量 = (int)月亮状态_可选整数_函(L, 2, 0);
      int 结果_短变量 = 月亮_垃圾回收_函(L, o, 步进_变量);
      基本库_检查值结果_宏名(结果_短变量);
      月亮推入布尔值_函(L, 结果_短变量);
      return 1;
    }
    case 月头_月亮_垃圾回收设置暂停_宏名:
    case 月头_月亮_垃圾回收设置步进乘法_宏名: {
      int p = (int)月亮状态_可选整数_函(L, 2, 0);
      int 前一个_变量 = 月亮_垃圾回收_函(L, o, p);
      基本库_检查值结果_宏名(前一个_变量);
      月亮推入整数_函(L, 前一个_变量);
      return 1;
    }
    case 月头_月亮_垃圾回收是否正在跑_宏名: {
      int 结果_短变量 = 月亮_垃圾回收_函(L, o);
      基本库_检查值结果_宏名(结果_短变量);
      月亮推入布尔值_函(L, 结果_短变量);
      return 1;
    }
    case 月头_月亮_垃圾回收生成_宏名: {
      int 主乘数_变量 = (int)月亮状态_可选整数_函(L, 2, 0);
      int 副乘数_变量 = (int)月亮状态_可选整数_函(L, 3, 0);
      return 月基本库_推模式_函(L, 月亮_垃圾回收_函(L, o, 主乘数_变量, 副乘数_变量));
    }
    case 月头_月亮_垃圾回收递增_宏名: {
      int 暂停_变量 = (int)月亮状态_可选整数_函(L, 2, 0);
      int 步进乘数_变量 = (int)月亮状态_可选整数_函(L, 3, 0);
      int 步进大小_变量 = (int)月亮状态_可选整数_函(L, 4, 0);
      return 月基本库_推模式_函(L, 月亮_垃圾回收_函(L, o, 暂停_变量, 步进乘数_变量, 步进大小_变量));
    }
    default: {
      int 结果_短变量 = 月亮_垃圾回收_函(L, o);
      基本库_检查值结果_宏名(结果_短变量);
      月亮推入整数_函(L, 结果_短变量);
      return 1;
    }
  }
  辅助库_月亮l_推失败_宏名(L);  /* invalid call (inside a finalizer) */
  return 1;
}


static int 月基本库_月亮B_类型_函 (炉_状态机结 *L) {
  int t = 月亮_类型_函(L, 1);
  辅助库_月亮l_实参检查_宏名(L, t != 月头_月亮_T没有_宏名, 1, "值_圆 expected");
  月亮推入字符串_函(L, 月亮_类型名称_函(L, t));
  return 1;
}


static int 月基本库_月亮B_下一个_函 (炉_状态机结 *L) {
  月亮状态_检查类型_函(L, 1, 月头_月亮_T表_宏名);
  月亮_设置顶_函(L, 2);  /* create a 2nd argument if there isn't one */
  if (月亮_下一个_函(L, 1))
    return 2;
  else {
    月亮推入空值_函(L);
    return 1;
  }
}


static int 月基本库_点对们继续_函 (炉_状态机结 *L, int 状态码_变量, 炉_K上下文型 k) {
  (void)L; (void)状态码_变量; (void)k;  /* unused */
  return 3;
}

static int 月基本库_月亮B_点对们_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  if (月亮状态_获取元字段_函(L, 1, "__pairs") == 月头_月亮_T空值_宏名) {  /* no metamethod? */
    月头_月亮_推C函数_宏名(L, 月基本库_月亮B_下一个_函);  /* will return generator, */
    月亮_推入值_函(L, 1);  /* 状态机_变量, */
    月亮推入空值_函(L);  /* and initial 值_圆 */
  }
  else {
    月亮_推入值_函(L, 1);  /* argument 'self' 到_变量 metamethod */
    月亮_调用常量_函(L, 1, 3, 0, 月基本库_点对们继续_函);  /* get 3 values from metamethod */
  }
  return 3;
}


/*
** Traversal function for 'ipairs'
*/
static int 月基本库_整数点对们辅助_函 (炉_状态机结 *L) {
  炉_整数型 i = 月亮状态_检查整数_函(L, 2);
  i = 辅助库_月亮l_整型操作_宏名(+, i, 1);
  月亮推入整数_函(L, i);
  return (月亮获取索引_函(L, 1, i) == 月头_月亮_T空值_宏名) ? 1 : 2;
}


/*
** 'ipairs' function. Returns '月基本库_整数点对们辅助_函', given "table", 0.
** (The given "table" may not be a table.)
*/
static int 月基本库_月亮B_整数点对们_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  月头_月亮_推C函数_宏名(L, 月基本库_整数点对们辅助_函);  /* iteration function */
  月亮_推入值_函(L, 1);  /* 状态机_变量 */
  月亮推入整数_函(L, 0);  /* initial 值_圆 */
  return 3;
}


static int 月基本库_加载_辅助_函 (炉_状态机结 *L, int 状态码_变量, int envidx) {
  if (配置_l_可能性高_宏名(状态码_变量 == LUA_OK)) {
    if (envidx != 0) {  /* '环境_短变量' parameter? */
      月亮_推入值_函(L, envidx);  /* environment for loaded function */
      if (!月亮_设置上值_函(L, -2, 1))  /* set it as 1st 上值_圆 */
        月头_月亮_弹出_宏名(L, 1);  /* remove '环境_短变量' if not used by 前一个_变量 call */
    }
    return 1;
  }
  else {  /* 错误_小变量 (message is on 顶部_变量 of the 栈_圆小) */
    辅助库_月亮l_推失败_宏名(L);
    月头_月亮_插入_宏名(L, -2);  /* put before 错误_小变量 message */
    return 2;  /* return fail plus 错误_小变量 message */
  }
}


static int 月基本库_月亮B_加载文件_函 (炉_状态机结 *L) {
  const char *函名_缩变量 = 辅助库_月亮l_可选字符串_宏名(L, 1, NULL);
  const char *模块_变量 = 辅助库_月亮l_可选字符串_宏名(L, 2, NULL);
  int 环境_短变量 = (!月头_月亮_是否没有_宏名(L, 3) ? 3 : 0);  /* '环境_短变量' index or 0 if no '环境_短变量' */
  int 状态码_变量 = 月亮状态_加载文件x_函(L, 函名_缩变量, 模块_变量);
  return 月基本库_加载_辅助_函(L, 状态码_变量, 环境_短变量);
}


/*
** {======================================================
** Generic Read function
** =======================================================
*/


/*
** reserved 插槽_变量, above all arguments, 到_变量 hold a copy of the returned
** string 到_变量 avoid it being collected while parsed. 'load' has four
** optional arguments (chunk, 源_圆 名称_变量, 模块_变量, and environment).
*/
#define 基本库_保留槽_宏名	5


/*
** Reader for generic 'load' function: '月亮_加载_函' uses the
** 栈_圆小 for internal stuff, so the 读器_圆 月编译器_不能_函 改变_变量 the
** 栈_圆小 顶部_变量. Instead, it keeps its resulting string in a
** reserved 插槽_变量 inside the 栈_圆小.
*/
static const char *月基本库_通用_读取器_函 (炉_状态机结 *L, void *用数_缩变量, size_t *大小_变量) {
  (void)(用数_缩变量);  /* not used */
  月亮状态_检查栈_函(L, 2, "too many nested functions");
  月亮_推入值_函(L, 1);  /* get function */
  月头_月亮_调用_宏名(L, 0, 1);  /* call it */
  if (月头_月亮_是否空值_宏名(L, -1)) {
    月头_月亮_弹出_宏名(L, 1);  /* pop 结果_变量 */
    *大小_变量 = 0;
    return NULL;
  }
  else if (配置_l_可能性低_宏名(!月亮_是否字符串_函(L, -1)))
    月亮状态_错误_函(L, "读器_圆 function must return a string");
  月头_月亮_替换_宏名(L, 基本库_保留槽_宏名);  /* 月词法_保存_函 string in reserved 插槽_变量 */
  return 月亮_到长字符串_函(L, 基本库_保留槽_宏名, 大小_变量);
}


static int 月基本库_月亮B_加载_函 (炉_状态机结 *L) {
  int 状态码_变量;
  size_t l;
  const char *s = 月亮_到长字符串_函(L, 1, &l);
  const char *模块_变量 = 辅助库_月亮l_可选字符串_宏名(L, 3, "bt");
  int 环境_短变量 = (!月头_月亮_是否没有_宏名(L, 4) ? 4 : 0);  /* '环境_短变量' index or 0 if no '环境_短变量' */
  if (s != NULL) {  /* loading a string? */
    const char *大块名_变量 = 辅助库_月亮l_可选字符串_宏名(L, 2, s);
    状态码_变量 = 月亮状态_加载缓冲区x_函(L, s, l, 大块名_变量, 模块_变量);
  }
  else {  /* loading from a 读器_圆 function */
    const char *大块名_变量 = 辅助库_月亮l_可选字符串_宏名(L, 2, "=(load)");
    月亮状态_检查类型_函(L, 1, 月头_月亮_T函数_宏名);
    月亮_设置顶_函(L, 基本库_保留槽_宏名);  /* create reserved 插槽_变量 */
    状态码_变量 = 月亮_加载_函(L, 月基本库_通用_读取器_函, NULL, 大块名_变量, 模块_变量);
  }
  return 月基本库_加载_辅助_函(L, 状态码_变量, 环境_短变量);
}

/* }====================================================== */


static int 月基本库_做文件继续_函 (炉_状态机结 *L, int d1, 炉_K上下文型 d2) {
  (void)d1;  (void)d2;  /* only 到_变量 月串库_匹配_函 'lua_Kfunction' prototype */
  return 月亮_获取顶_函(L) - 1;
}


static int 月基本库_月亮B_做文件_函 (炉_状态机结 *L) {
  const char *函名_缩变量 = 辅助库_月亮l_可选字符串_宏名(L, 1, NULL);
  月亮_设置顶_函(L, 1);
  if (配置_l_可能性低_宏名(辅助库_月亮l_载入文件_宏名(L, 函名_缩变量) != LUA_OK))
    return 月亮_错误_函(L);
  月亮_调用常量_函(L, 0, 月头_月亮_多返回_宏名, 0, 月基本库_做文件继续_函);
  return 月基本库_做文件继续_函(L, 0, 0);
}


static int 月基本库_月亮B_断言_函 (炉_状态机结 *L) {
  if (配置_l_可能性高_宏名(月亮_到布尔值_函(L, 1)))  /* condition is true? */
    return 月亮_获取顶_函(L);  /* return all arguments */
  else {  /* 错误_小变量 */
    月亮状态_检查任意_函(L, 1);  /* there must be a condition */
    月头_月亮_移除_宏名(L, 1);  /* remove it */
    月头_月亮_推字面_宏名(L, "assertion failed!");  /* default message */
    月亮_设置顶_函(L, 1);  /* leave only message (default if no other one) */
    return 月基本库_月亮B_错误_函(L);  /* call '错误_小变量' */
  }
}


static int 月基本库_月亮B_选取_函 (炉_状态机结 *L) {
  int n = 月亮_获取顶_函(L);
  if (月亮_类型_函(L, 1) == 月头_月亮_T字符串_宏名 && *月头_月亮_到字符串_宏名(L, 1) == '#') {
    月亮推入整数_函(L, n-1);
    return 1;
  }
  else {
    炉_整数型 i = 月亮状态_检查整数_函(L, 1);
    if (i < 0) i = n + i;
    else if (i > n) i = n;
    辅助库_月亮l_实参检查_宏名(L, 1 <= i, 1, "index out of range");
    return n - (int)i;
  }
}


/*
** Continuation function for 'pcall' and 'xpcall'. Both functions
** already 已推入_小写 a 'true' before doing the call, so in case of 成功_变量
** '月基本库_完成保护调用_函' only has 到_变量 return everything in the 栈_圆小 minus
** '额外_变量' values (哪儿_变量 '额外_变量' is exactly the number of items 到_变量 be
** ignored).
*/
static int 月基本库_完成保护调用_函 (炉_状态机结 *L, int 状态码_变量, 炉_K上下文型 额外_变量) {
  if (配置_l_可能性低_宏名(状态码_变量 != LUA_OK && 状态码_变量 != 月头_月亮_让步_宏名)) {  /* 错误_小变量? */
    月亮推入布尔值_函(L, 0);  /* 首先_变量 结果_变量 (false) */
    月亮_推入值_函(L, -2);  /* 错误_小变量 message */
    return 2;  /* return false, 消息_缩变量 */
  }
  else
    return 月亮_获取顶_函(L) - (int)额外_变量;  /* return all results */
}


static int 月基本库_月亮B_保护调用_函 (炉_状态机结 *L) {
  int 状态码_变量;
  月亮状态_检查任意_函(L, 1);
  月亮推入布尔值_函(L, 1);  /* 首先_变量 结果_变量 if no errors */
  月头_月亮_插入_宏名(L, 1);  /* put it in place */
  状态码_变量 = 月亮_保护调用常量_函(L, 月亮_获取顶_函(L) - 2, 月头_月亮_多返回_宏名, 0, 0, 月基本库_完成保护调用_函);
  return 月基本库_完成保护调用_函(L, 状态码_变量, 0);
}


/*
** Do a protected call with 错误_小变量 handling. After '月亮_旋转_函', the
** 栈_圆小 will have <f, 错_短变量, true, f, [实参们_短变量...]>; so, the function passes
** 2 到_变量 '月基本库_完成保护调用_函' 到_变量 skip the 2 首先_变量 values when returning results.
*/
static int 月基本库_月亮B_x保护调用_函 (炉_状态机结 *L) {
  int 状态码_变量;
  int n = 月亮_获取顶_函(L);
  月亮状态_检查类型_函(L, 2, 月头_月亮_T函数_宏名);  /* 月解析器_检查_函 错误_小变量 function */
  月亮推入布尔值_函(L, 1);  /* 首先_变量 结果_变量 */
  月亮_推入值_函(L, 1);  /* function */
  月亮_旋转_函(L, 3, 2);  /* move them below function's arguments */
  状态码_变量 = 月亮_保护调用常量_函(L, n - 2, 月头_月亮_多返回_宏名, 2, 2, 月基本库_完成保护调用_函);
  return 月基本库_完成保护调用_函(L, 状态码_变量, 2);
}


static int 月基本库_月亮B_到字符串_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  月亮状态_到字符串_函(L, 1, NULL);
  return 1;
}


static const 炉L_寄结 基本_函_变量[] = {
  {"断言", 月基本库_月亮B_断言_函},
  {"回收垃圾", 月基本库_月亮B_回收垃圾_函},
  {"做文件", 月基本库_月亮B_做文件_函},
  {"错误", 月基本库_月亮B_错误_函},
  {"获取元表", 月基本库_月亮B_获取元表_函},
  {"ipairs", 月基本库_月亮B_整数点对们_函},
  {"载入文件", 月基本库_月亮B_加载文件_函},
  {"载入", 月基本库_月亮B_加载_函},
  {"下一个", 月基本库_月亮B_下一个_函},
  {"pairs", 月基本库_月亮B_点对们_函},
  {"pcall", 月基本库_月亮B_保护调用_函},
  {"打印", 月基本库_月亮B_打印_函},
  {"警告", 月基本库_月亮B_警告_函},
  {"rawequal", 月基本库_月亮B_原始相等_函},
  {"rawlen", 月基本库_月亮B_原始长度_函},
  {"rawget", 月基本库_月亮B_原始获取_函},
  {"rawset", 月基本库_月亮B_原始设置_函},
  {"选择", 月基本库_月亮B_选取_函},
  {"setmetatable", 月基本库_月亮B_设置元表_函},
  {"虚机头_到数目_宏名", 月基本库_月亮B_到数目_函},
  {"虚拟机_到字符串_宏名", 月基本库_月亮B_到字符串_函},
  {"类型", 月基本库_月亮B_类型_函},
  {"xpcall", 月基本库_月亮B_x保护调用_函},
  /* placeholders */
  {辅助库_月亮_全局名称_宏名, NULL},
  {"_VERSION", NULL},
  {NULL, NULL}
};


配置_月亮模块_应程接_宏名 int 月亮打开基本_函 (炉_状态机结 *L) {
  /* 打开_圆 库_短变量 into global table */
  月头_月亮_推全局表_宏名(L);
  月亮状态_设置函们_函(L, 基本_函_变量, 0);
  /* set global _G */
  月亮_推入值_函(L, -1);
  月亮设置字段_函(L, -2, 辅助库_月亮_全局名称_宏名);
  /* set global _VERSION */
  月头_月亮_推字面_宏名(L, 月头_月亮_版本_宏名);
  月亮设置字段_函(L, -2, "_VERSION");
  return 1;
}

