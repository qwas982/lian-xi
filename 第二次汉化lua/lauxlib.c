/*
** $Id: lauxlib.c $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/

#define lauxlib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
** This file uses only the official API of Lua.
** Any function declared here could be written as an application function.
*/

#include "lua.h"

#include "lauxlib.h"


#if !defined(串库_最大_大小T_宏名)
/* maximum 值_圆 for size_t */
#define 串库_最大_大小T_宏名	((size_t)(~(size_t)0))
#endif


/*
** {======================================================
** Traceback
** =======================================================
*/


#define 辅助库_层级1_宏名	10	/* 大小_变量 of the 首先_变量 part of the 栈_圆小 */
#define 辅助库_层级2_宏名	11	/* 大小_变量 of the second part of the 栈_圆小 */



/*
** Search for 'objidx' in table at index -1. ('objidx' must be an
** absolute index.) Return 1 + string at 顶部_变量 if it found a good 名称_变量.
*/
static int 月辅助库_找字段_函 (炉_状态机结 *L, int objidx, int 层级_变量) {
  if (层级_变量 == 0 || !月头_月亮_是否表_宏名(L, -1))
    return 0;  /* not found */
  月亮推入空值_函(L);  /* 起始_变量 '下一个_变量' 环_变量 */
  while (月亮_下一个_函(L, -2)) {  /* for each pair in table */
    if (月亮_类型_函(L, -2) == 月头_月亮_T字符串_宏名) {  /* ignore non-string keys */
      if (月亮原始相等_函(L, objidx, -1)) {  /* found object? */
        月头_月亮_弹出_宏名(L, 1);  /* remove 值_圆 (but keep 名称_变量) */
        return 1;
      }
      else if (月辅助库_找字段_函(L, objidx, 层级_变量 - 1)) {  /* try recursively */
        /* 栈_圆小: lib_name, lib_table, field_name (顶部_变量) */
        月头_月亮_推字面_宏名(L, ".");  /* place '.' between the two names */
        月头_月亮_替换_宏名(L, -3);  /* (in the 插槽_变量 occupied by table) */
        月亮_拼接_函(L, 3);  /* lib_name.field_name */
        return 1;
      }
    }
    月头_月亮_弹出_宏名(L, 1);  /* remove 值_圆 */
  }
  return 0;  /* not found */
}


/*
** Search for a 名称_变量 for a function in all loaded modules
*/
static int 月辅助库_推全局函名_函 (炉_状态机结 *L, 炉_调试结 *活记_缩变量) {
  int 顶部_变量 = 月亮_获取顶_函(L);
  月亮_获取信息_函(L, "f", 活记_缩变量);  /* push function */
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_已载入_表_宏名);
  if (月辅助库_找字段_函(L, 顶部_变量 + 1, 2)) {
    const char *名称_变量 = 月头_月亮_到字符串_宏名(L, -1);
    if (strncmp(名称_变量, 辅助库_月亮_全局名称_宏名 ".", 3) == 0) {  /* 名称_变量 起始_变量 with '_G.'? */
      月亮推入字符串_函(L, 名称_变量 + 3);  /* push 名称_变量 without prefix */
      月头_月亮_移除_宏名(L, -2);  /* remove original 名称_变量 */
    }
    月亮_复制_函(L, -1, 顶部_变量 + 1);  /* copy 名称_变量 到_变量 proper place */
    月亮_设置顶_函(L, 顶部_变量 + 1);  /* remove table "loaded" and 名称_变量 copy */
    return 1;
  }
  else {
    月亮_设置顶_函(L, 顶部_变量);  /* remove function and global table */
    return 0;
  }
}


static void 月辅助库_推函名_函 (炉_状态机结 *L, 炉_调试结 *活记_缩变量) {
  if (月辅助库_推全局函名_函(L, 活记_缩变量)) {  /* try 首先_变量 a global 名称_变量 */
    月亮推入格式化字符串_函(L, "function '%s'", 月头_月亮_到字符串_宏名(L, -1));
    月头_月亮_移除_宏名(L, -2);  /* remove 名称_变量 */
  }
  else if (*活记_缩变量->什么名_圆 != '\0')  /* is there a 名称_变量 from 代码_变量? */
    月亮推入格式化字符串_函(L, "%s '%s'", 活记_缩变量->什么名_圆, 活记_缩变量->名称_变量);  /* use it */
  else if (*活记_缩变量->什么_变量 == 'm')  /* main? */
      月头_月亮_推字面_宏名(L, "main chunk");
  else if (*活记_缩变量->什么_变量 != 'C')  /* for Lua functions, use <file:行_变量> */
    月亮推入格式化字符串_函(L, "function <%s:%d>", 活记_缩变量->短的_源小写, 活记_缩变量->已定义行_小写);
  else  /* nothing 左_圆... */
    月头_月亮_推字面_宏名(L, "?");
}


static int 月辅助库_最后层级_函 (炉_状态机结 *L) {
  炉_调试结 活记_缩变量;
  int 行信_缩变量 = 1, 行扩_缩变量 = 1;
  /* find an upper bound */
  while (月亮_获取栈_函(L, 行扩_缩变量, &活记_缩变量)) { 行信_缩变量 = 行扩_缩变量; 行扩_缩变量 *= 2; }
  /* do a binary search */
  while (行信_缩变量 < 行扩_缩变量) {
    int m = (行信_缩变量 + 行扩_缩变量)/2;
    if (月亮_获取栈_函(L, m, &活记_缩变量)) 行信_缩变量 = m + 1;
    else 行扩_缩变量 = m;
  }
  return 行扩_缩变量 - 1;
}


配置_月亮库_应程接_宏名 void 月亮状态_跟踪回溯_函 (炉_状态机结 *L, 炉_状态机结 *L1,
                                const char *消息_缩变量, int 层级_变量) {
  炉L_缓冲区结 b;
  炉_调试结 活记_缩变量;
  int 最后_变量 = 月辅助库_最后层级_函(L1);
  int 限制到秀_变量 = (最后_变量 - 层级_变量 > 辅助库_层级1_宏名 + 辅助库_层级2_宏名) ? 辅助库_层级1_宏名 : -1;
  月亮状态_缓冲初始的_函(L, &b);
  if (消息_缩变量) {
    月亮状态_添加字符串_函(&b, 消息_缩变量);
    luaL_addchar(&b, '\n');
  }
  月亮状态_添加字符串_函(&b, "栈_圆小 traceback:");
  while (月亮_获取栈_函(L1, 层级_变量++, &活记_缩变量)) {
    if (限制到秀_变量-- == 0) {  /* too many levels? */
      int n = 最后_变量 - 层级_变量 - 辅助库_层级2_宏名 + 1;  /* number of levels 到_变量 skip */
      月亮推入格式化字符串_函(L, "\n\t...\t(skipping %d levels)", n);
      月亮状态_添加值_函(&b);  /* add warning about skip */
      层级_变量 += n;  /* and skip 到_变量 最后_变量 levels */
    }
    else {
      月亮_获取信息_函(L1, "Slnt", &活记_缩变量);
      if (活记_缩变量.当前行_变量 <= 0)
        月亮推入格式化字符串_函(L, "\n\t%s: in ", 活记_缩变量.短的_源小写);
      else
        月亮推入格式化字符串_函(L, "\n\t%s:%d: in ", 活记_缩变量.短的_源小写, 活记_缩变量.当前行_变量);
      月亮状态_添加值_函(&b);
      月辅助库_推函名_函(L, &活记_缩变量);
      月亮状态_添加值_函(&b);
      if (活记_缩变量.是尾调用吗_圆)
        月亮状态_添加字符串_函(&b, "\n\t(...tail calls...)");
    }
  }
  月亮状态_推入结果_函(&b);
}

/* }====================================================== */


/*
** {======================================================
** Error-月解释器_报告_函 functions
** =======================================================
*/

配置_月亮库_应程接_宏名 int 月亮状态_实参错误_函 (炉_状态机结 *L, int 实参_短变量, const char *extramsg) {
  炉_调试结 活记_缩变量;
  if (!月亮_获取栈_函(L, 0, &活记_缩变量))  /* no 栈_圆小 frame? */
    return 月亮状态_错误_函(L, "bad argument #%d (%s)", 实参_短变量, extramsg);
  月亮_获取信息_函(L, "n", &活记_缩变量);
  if (strcmp(活记_缩变量.什么名_圆, "method") == 0) {
    实参_短变量--;  /* do not 计数_变量 'self' */
    if (实参_短变量 == 0)  /* 错误_小变量 is in the self argument itself? */
      return 月亮状态_错误_函(L, "calling '%s' on bad self (%s)",
                           活记_缩变量.名称_变量, extramsg);
  }
  if (活记_缩变量.名称_变量 == NULL)
    活记_缩变量.名称_变量 = (月辅助库_推全局函名_函(L, &活记_缩变量)) ? 月头_月亮_到字符串_宏名(L, -1) : "?";
  return 月亮状态_错误_函(L, "bad argument #%d 到_变量 '%s' (%s)",
                        实参_短变量, 活记_缩变量.名称_变量, extramsg);
}


配置_月亮库_应程接_宏名 int 月亮状态_类型错误_函 (炉_状态机结 *L, int 实参_短变量, const char *tname) {
  const char *消息_缩变量;
  const char *类型实参_变量;  /* 名称_变量 for the type of the 实际上_变量 argument */
  if (月亮状态_获取元字段_函(L, 实参_短变量, "__name") == 月头_月亮_T字符串_宏名)
    类型实参_变量 = 月头_月亮_到字符串_宏名(L, -1);  /* use the given type 名称_变量 */
  else if (月亮_类型_函(L, 实参_短变量) == 月头_月亮_T轻量用户数据_宏名)
    类型实参_变量 = "light userdata";  /* special 名称_变量 for messages */
  else
    类型实参_变量 = 辅助库_月亮l_类型名称_宏名(L, 实参_短变量);  /* standard 名称_变量 */
  消息_缩变量 = 月亮推入格式化字符串_函(L, "%s expected, got %s", tname, 类型实参_变量);
  return 月亮状态_实参错误_函(L, 实参_短变量, 消息_缩变量);
}


static void 月辅助库_标签_错误_函 (炉_状态机结 *L, int 实参_短变量, int tag) {
  月亮状态_类型错误_函(L, 实参_短变量, 月亮_类型名称_函(L, tag));
}


/*
** The use of '月亮推入格式化字符串_函' ensures this function does not
** need reserved 栈_圆小 空间_圆 when called.
*/
配置_月亮库_应程接_宏名 void 月亮状态_哪里_函 (炉_状态机结 *L, int 层级_变量) {
  炉_调试结 活记_缩变量;
  if (月亮_获取栈_函(L, 层级_变量, &活记_缩变量)) {  /* 月解析器_检查_函 function at 层级_变量 */
    月亮_获取信息_函(L, "Sl", &活记_缩变量);  /* get 信息_短变量 about it */
    if (活记_缩变量.当前行_变量 > 0) {  /* is there 信息_短变量? */
      月亮推入格式化字符串_函(L, "%s:%d: ", 活记_缩变量.短的_源小写, 活记_缩变量.当前行_变量);
      return;
    }
  }
  月亮推入格式化字符串_函(L, "");  /* else, no information available... */
}


/*
** Again, the use of '月亮推入可变格式字符串_函' ensures this function does
** not need reserved 栈_圆小 空间_圆 when called. (At worst, it generates
** an 错误_小变量 with "栈_圆小 overflow" instead of the given message.)
*/
配置_月亮库_应程接_宏名 int 月亮状态_错误_函 (炉_状态机结 *L, const char *格式_短变量, ...) {
  va_list argp;
  va_start(argp, 格式_短变量);
  月亮状态_哪里_函(L, 1);
  月亮推入可变格式字符串_函(L, 格式_短变量, argp);
  va_end(argp);
  月亮_拼接_函(L, 2);
  return 月亮_错误_函(L);
}


配置_月亮库_应程接_宏名 int 月亮状态_文件结果_函 (炉_状态机结 *L, int 状态_短变量, const char *函名_缩变量) {
  int 错误号_缩变量 = errno;  /* calls 到_变量 Lua API may 改变_变量 this 值_圆 */
  if (状态_短变量) {
    月亮推入布尔值_函(L, 1);
    return 1;
  }
  else {
    辅助库_月亮l_推失败_宏名(L);
    if (函名_缩变量)
      月亮推入格式化字符串_函(L, "%s: %s", 函名_缩变量, strerror(错误号_缩变量));
    else
      月亮推入字符串_函(L, strerror(错误号_缩变量));
    月亮推入整数_函(L, 错误号_缩变量);
    return 3;
  }
}


#if !defined(辅助库_l_检验状态_宏名)	/* { */

#if defined(配置_月亮_用_POSIX_宏名)

#include <sys/wait.h>

/*
** use appropriate macros 到_变量 interpret 'pclose' return 状态码_变量
*/
#define 辅助库_l_检验状态_宏名(状态_短变量,什么_变量)  \
   if (WIFEXITED(状态_短变量)) { 状态_短变量 = WEXITSTATUS(状态_短变量); } \
   else if (WIFSIGNALED(状态_短变量)) { 状态_短变量 = WTERMSIG(状态_短变量); 什么_变量 = "signal"; }

#else

#define 辅助库_l_检验状态_宏名(状态_短变量,什么_变量)  /* no 操作_短变量 */

#endif

#endif				/* } */


配置_月亮库_应程接_宏名 int 月亮状态_执行结果_函 (炉_状态机结 *L, int 状态_短变量) {
  if (状态_短变量 != 0 && errno != 0)  /* 错误_小变量 with an 'errno'? */
    return 月亮状态_文件结果_函(L, 0, NULL);
  else {
    const char *什么_变量 = "出口_变量";  /* type of termination */
    辅助库_l_检验状态_宏名(状态_短变量, 什么_变量);  /* interpret 结果_变量 */
    if (*什么_变量 == 'e' && 状态_短变量 == 0)  /* successful termination? */
      月亮推入布尔值_函(L, 1);
    else
      辅助库_月亮l_推失败_宏名(L);
    月亮推入字符串_函(L, 什么_变量);
    月亮推入整数_函(L, 状态_短变量);
    return 3;  /* return true/fail,什么_变量,代码_变量 */
  }
}

/* }====================================================== */



/*
** {======================================================
** Userdata's 元表_小写 manipulation
** =======================================================
*/

配置_月亮库_应程接_宏名 int 月亮状态_新元表_函 (炉_状态机结 *L, const char *tname) {
  if (辅助库_月亮l_获取元表_宏名(L, tname) != 月头_月亮_T空值_宏名)  /* 名称_变量 already in use? */
    return 0;  /* leave 前一个_变量 值_圆 on 顶部_变量, but return 0 */
  月头_月亮_弹出_宏名(L, 1);
  月亮创建表_函(L, 0, 2);  /* create 元表_小写 */
  月亮推入字符串_函(L, tname);
  月亮设置字段_函(L, -2, "__name");  /* 元表_小写.__name = tname */
  月亮_推入值_函(L, -1);
  月亮设置字段_函(L, 月头_月亮_注册表索引_宏名, tname);  /* 注册表_变量.名称_变量 = 元表_小写 */
  return 1;
}


配置_月亮库_应程接_宏名 void 月亮状态_设置元表_函 (炉_状态机结 *L, const char *tname) {
  辅助库_月亮l_获取元表_宏名(L, tname);
  月亮_设置元表_函(L, -2);
}


配置_月亮库_应程接_宏名 void *月亮状态_测试用户数据_函 (炉_状态机结 *L, int 用数_缩变量, const char *tname) {
  void *p = 月亮_到用户数据_函(L, 用数_缩变量);
  if (p != NULL) {  /* 值_圆 is a userdata? */
    if (月亮获取元表_函(L, 用数_缩变量)) {  /* does it have a 元表_小写? */
      辅助库_月亮l_获取元表_宏名(L, tname);  /* get correct 元表_小写 */
      if (!月亮原始相等_函(L, -1, -2))  /* not the same? */
        p = NULL;  /* 值_圆 is a userdata with wrong 元表_小写 */
      月头_月亮_弹出_宏名(L, 2);  /* remove both metatables */
      return p;
    }
  }
  return NULL;  /* 值_圆 is not a userdata with a 元表_小写 */
}


配置_月亮库_应程接_宏名 void *月亮状态_检查用户数据_函 (炉_状态机结 *L, int 用数_缩变量, const char *tname) {
  void *p = 月亮状态_测试用户数据_函(L, 用数_缩变量, tname);
  辅助库_月亮l_实参期望_宏名(L, p != NULL, 用数_缩变量, tname);
  return p;
}

/* }====================================================== */


/*
** {======================================================
** Argument 月解析器_检查_函 functions
** =======================================================
*/

配置_月亮库_应程接_宏名 int 月亮状态_检查选项_函 (炉_状态机结 *L, int 实参_短变量, const char *def,
                                 const char *const lst[]) {
  const char *名称_变量 = (def) ? 辅助库_月亮l_可选字符串_宏名(L, 实参_短变量, def) :
                             辅助库_月亮l_检查字符串_宏名(L, 实参_短变量);
  int i;
  for (i=0; lst[i]; i++)
    if (strcmp(lst[i], 名称_变量) == 0)
      return i;
  return 月亮状态_实参错误_函(L, 实参_短变量,
                       月亮推入格式化字符串_函(L, "invalid 选项_变量 '%s'", 名称_变量));
}


/*
** Ensures the 栈_圆小 has at least '空间_圆' 额外_变量 slots, raising an 错误_小变量
** if it 月编译器_不能_函 fulfill the request. (The 错误_小变量 handling needs a few
** 额外_变量 slots 到_变量 格式_变量 the 错误_小变量 message. In case of an 错误_小变量 without
** this 额外_变量 空间_圆, Lua will generate the same '栈_圆小 overflow' 错误_小变量,
** but without '消息_缩变量'.)
*/
配置_月亮库_应程接_宏名 void 月亮状态_检查栈_函 (炉_状态机结 *L, int 空间_圆, const char *消息_缩变量) {
  if (配置_l_可能性低_宏名(!月亮_检查栈空间_函(L, 空间_圆))) {
    if (消息_缩变量)
      月亮状态_错误_函(L, "栈_圆小 overflow (%s)", 消息_缩变量);
    else
      月亮状态_错误_函(L, "栈_圆小 overflow");
  }
}


配置_月亮库_应程接_宏名 void 月亮状态_检查类型_函 (炉_状态机结 *L, int 实参_短变量, int t) {
  if (配置_l_可能性低_宏名(月亮_类型_函(L, 实参_短变量) != t))
    月辅助库_标签_错误_函(L, 实参_短变量, t);
}


配置_月亮库_应程接_宏名 void 月亮状态_检查任意_函 (炉_状态机结 *L, int 实参_短变量) {
  if (配置_l_可能性低_宏名(月亮_类型_函(L, 实参_短变量) == 月头_月亮_T没有_宏名))
    月亮状态_实参错误_函(L, 实参_短变量, "值_圆 expected");
}


配置_月亮库_应程接_宏名 const char *月亮状态_检查状态字符串_函 (炉_状态机结 *L, int 实参_短变量, size_t *长度_短变量) {
  const char *s = 月亮_到长字符串_函(L, 实参_短变量, 长度_短变量);
  if (配置_l_可能性低_宏名(!s)) 月辅助库_标签_错误_函(L, 实参_短变量, 月头_月亮_T字符串_宏名);
  return s;
}


配置_月亮库_应程接_宏名 const char *月亮状态_可选状态字符串_函 (炉_状态机结 *L, int 实参_短变量,
                                        const char *def, size_t *长度_短变量) {
  if (月头_月亮_是否没有或空值_宏名(L, 实参_短变量)) {
    if (长度_短变量)
      *长度_短变量 = (def ? strlen(def) : 0);
    return def;
  }
  else return 月亮状态_检查状态字符串_函(L, 实参_短变量, 长度_短变量);
}


配置_月亮库_应程接_宏名 炉_数目型 月亮状态_检查数目_函 (炉_状态机结 *L, int 实参_短变量) {
  int 是否数目_变量;
  炉_数目型 d = 月亮_到数目x_函(L, 实参_短变量, &是否数目_变量);
  if (配置_l_可能性低_宏名(!是否数目_变量))
    月辅助库_标签_错误_函(L, 实参_短变量, 月头_月亮_T数目_宏名);
  return d;
}


配置_月亮库_应程接_宏名 炉_数目型 月亮状态_可选数目_函 (炉_状态机结 *L, int 实参_短变量, 炉_数目型 def) {
  return 辅助库_月亮l_可选_宏名(L, 月亮状态_检查数目_函, 实参_短变量, def);
}


static void 月辅助库_整型错误_函 (炉_状态机结 *L, int 实参_短变量) {
  if (月亮_是否数目_函(L, 实参_短变量))
    月亮状态_实参错误_函(L, 实参_短变量, "number has no integer representation");
  else
    月辅助库_标签_错误_函(L, 实参_短变量, 月头_月亮_T数目_宏名);
}


配置_月亮库_应程接_宏名 炉_整数型 月亮状态_检查整数_函 (炉_状态机结 *L, int 实参_短变量) {
  int 是否数目_变量;
  炉_整数型 d = 月亮_到整数x_函(L, 实参_短变量, &是否数目_变量);
  if (配置_l_可能性低_宏名(!是否数目_变量)) {
    月辅助库_整型错误_函(L, 实参_短变量);
  }
  return d;
}


配置_月亮库_应程接_宏名 炉_整数型 月亮状态_可选整数_函 (炉_状态机结 *L, int 实参_短变量,
                                                      炉_整数型 def) {
  return 辅助库_月亮l_可选_宏名(L, 月亮状态_检查整数_函, 实参_短变量, def);
}

/* }====================================================== */


/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/

/* userdata 到_变量 盒子_变量 arbitrary 数据_变量 */
typedef struct 用户盒子_结 {
  void *盒子_变量;
  size_t b大小_短;
} 用户盒子_结;


static void *月辅助库_调整盒子大小_函 (炉_状态机结 *L, int 索引_缩变量, size_t 新大小_变量) {
  void *用数_缩变量;
  炉_分配半 分配函_短变量 = 月亮_获取分配函数_函(L, &用数_缩变量);
  用户盒子_结 *盒子_变量 = (用户盒子_结 *)月亮_到用户数据_函(L, 索引_缩变量);
  void *临时_变量 = 分配函_短变量(用数_缩变量, 盒子_变量->盒子_变量, 盒子_变量->b大小_短, 新大小_变量);
  if (配置_l_可能性低_宏名(临时_变量 == NULL && 新大小_变量 > 0)) {  /* allocation 错误_小变量? */
    月头_月亮_推字面_宏名(L, "not enough memory");
    月亮_错误_函(L);  /* raise a memory 错误_小变量 */
  }
  盒子_变量->盒子_变量 = 临时_变量;
  盒子_变量->b大小_短 = 新大小_变量;
  return 临时_变量;
}


static int 月辅助库_盒子垃圾回收_函 (炉_状态机结 *L) {
  月辅助库_调整盒子大小_函(L, 1, 0);
  return 0;
}


static const 炉L_寄结 盒子元表_缩变量[] = {  /* 盒子_变量 metamethods */
  {"__gc", 月辅助库_盒子垃圾回收_函},
  {"__close", 月辅助库_盒子垃圾回收_函},
  {NULL, NULL}
};


static void 月辅助库_新盒子_函 (炉_状态机结 *L) {
  用户盒子_结 *盒子_变量 = (用户盒子_结 *)月亮创建新的用户数据uv_函(L, sizeof(用户盒子_结), 0);
  盒子_变量->盒子_变量 = NULL;
  盒子_变量->b大小_短 = 0;
  if (月亮状态_新元表_函(L, "_UBOX*"))  /* creating 元表_小写? */
    月亮状态_设置函们_函(L, 盒子元表_缩变量, 0);  /* set its metamethods */
  月亮_设置元表_函(L, -2);
}


/*
** 月解析器_检查_函 whether 缓冲区_变量 is using a userdata on the 栈_圆小 as a temporary
** 缓冲区_变量
*/
#define 辅助库_在栈上缓冲_宏名(B)	((B)->b != (B)->初始的_变量.b)


/*
** Whenever 缓冲区_变量 is accessed, 插槽_变量 '索引_缩变量' must either be a 盒子_变量 (which
** 月编译器_不能_函 be NULL) or it is a placeholder for the 缓冲区_变量.
*/
#define 辅助库_检查缓冲区层级_宏名(B,索引_缩变量)  \
  限制_月亮_断言_宏名(辅助库_在栈上缓冲_宏名(B) ? 月亮_到用户数据_函(B->L, 索引_缩变量) != NULL  \
                            : 月亮_到用户数据_函(B->L, 索引_缩变量) == (void*)B)


/*
** Compute new 大小_变量 for 缓冲区_变量 'B', enough 到_变量 accommodate 额外_变量 '大小_短变量'
** bytes. (The test for "not big enough" also gets the case when the
** computation of '新大小_变量' overflows.)
*/
static size_t 月辅助库_新缓冲大小_函 (炉L_缓冲区结 *B, size_t 大小_短变量) {
  size_t 新大小_变量 = (B->大小_变量 / 2) * 3;  /* 缓冲区_变量 大小_变量 * 1.5 */
  if (配置_l_可能性低_宏名(串库_最大_大小T_宏名 - 大小_短变量 < B->n))  /* overflow in (B->n + 大小_短变量)? */
    return 月亮状态_错误_函(B->L, "缓冲区_变量 too large");
  if (新大小_变量 < B->n + 大小_短变量)  /* not big enough? */
    新大小_变量 = B->n + 大小_短变量;
  return 新大小_变量;
}


/*
** Returns a pointer 到_变量 a free area with at least '大小_短变量' bytes in 缓冲区_变量
** 'B'. 'boxidx' is the relative position in the 栈_圆小 哪儿_变量 is the
** 缓冲区_变量's 盒子_变量 or its placeholder.
*/
static char *月辅助库_预备缓冲大小_函 (炉L_缓冲区结 *B, size_t 大小_短变量, int boxidx) {
  辅助库_检查缓冲区层级_宏名(B, boxidx);
  if (B->大小_变量 - B->n >= 大小_短变量)  /* enough 空间_圆? */
    return B->b + B->n;
  else {
    炉_状态机结 *L = B->L;
    char *新缓冲_变量;
    size_t 新大小_变量 = 月辅助库_新缓冲大小_函(B, 大小_短变量);
    /* create larger 缓冲区_变量 */
    if (辅助库_在栈上缓冲_宏名(B))  /* 缓冲区_变量 already has a 盒子_变量? */
      新缓冲_变量 = (char *)月辅助库_调整盒子大小_函(L, boxidx, 新大小_变量);  /* resize it */
    else {  /* no 盒子_变量 yet */
      月头_月亮_移除_宏名(L, boxidx);  /* remove placeholder */
      月辅助库_新盒子_函(L);  /* create a new 盒子_变量 */
      月头_月亮_插入_宏名(L, boxidx);  /* move 盒子_变量 到_变量 its intended position */
      月亮_到关闭_函(L, boxidx);
      新缓冲_变量 = (char *)月辅助库_调整盒子大小_函(L, boxidx, 新大小_变量);
      memcpy(新缓冲_变量, B->b, B->n * sizeof(char));  /* copy original content */
    }
    B->b = 新缓冲_变量;
    B->大小_变量 = 新大小_变量;
    return 新缓冲_变量 + B->n;
  }
}

/*
** returns a pointer 到_变量 a free area with at least '大小_短变量' bytes
*/
配置_月亮库_应程接_宏名 char *月亮状态_预备缓冲区大小_函 (炉L_缓冲区结 *B, size_t 大小_短变量) {
  return 月辅助库_预备缓冲大小_函(B, 大小_短变量, -1);
}


配置_月亮库_应程接_宏名 void 月亮状态_添加状态字符串_函 (炉L_缓冲区结 *B, const char *s, size_t l) {
  if (l > 0) {  /* avoid 'memcpy' when 's' can be NULL */
    char *b = 月辅助库_预备缓冲大小_函(B, l, -1);
    memcpy(b, s, l * sizeof(char));
    luaL_addsize(B, l);
  }
}


配置_月亮库_应程接_宏名 void 月亮状态_添加字符串_函 (炉L_缓冲区结 *B, const char *s) {
  月亮状态_添加状态字符串_函(B, s, strlen(s));
}


配置_月亮库_应程接_宏名 void 月亮状态_推入结果_函 (炉L_缓冲区结 *B) {
  炉_状态机结 *L = B->L;
  辅助库_检查缓冲区层级_宏名(B, -1);
  月亮推入长字符串_函(L, B->b, B->n);
  if (辅助库_在栈上缓冲_宏名(B))
    月亮_关闭槽_函(L, -2);  /* 关闭_圆 the 盒子_变量 */
  月头_月亮_移除_宏名(L, -2);  /* remove 盒子_变量 or placeholder from the 栈_圆小 */
}


配置_月亮库_应程接_宏名 void 月亮状态_推入结果大小_函 (炉L_缓冲区结 *B, size_t 大小_短变量) {
  luaL_addsize(B, 大小_短变量);
  月亮状态_推入结果_函(B);
}


/*
** '月亮状态_添加值_函' is the only function in the Buffer system 哪儿_变量 the
** 盒子_变量 (if existent) is not on the 顶部_变量 of the 栈_圆小. So, instead of
** calling '月亮状态_添加状态字符串_函', it replicates the 代码_变量 using -2 as the
** 最后_变量 argument 到_变量 '月辅助库_预备缓冲大小_函', signaling that the 盒子_变量 is (or will
** be) below the string being added 到_变量 the 缓冲区_变量. (Box creation can
** trigger an emergency GC, so we should not remove the string from the
** 栈_圆小 before we have the 空间_圆 guaranteed.)
*/
配置_月亮库_应程接_宏名 void 月亮状态_添加值_函 (炉L_缓冲区结 *B) {
  炉_状态机结 *L = B->L;
  size_t 长度_短变量;
  const char *s = 月亮_到长字符串_函(L, -1, &长度_短变量);
  char *b = 月辅助库_预备缓冲大小_函(B, 长度_短变量, -2);
  memcpy(b, s, 长度_短变量 * sizeof(char));
  luaL_addsize(B, 长度_短变量);
  月头_月亮_弹出_宏名(L, 1);  /* pop string */
}


配置_月亮库_应程接_宏名 void 月亮状态_缓冲初始的_函 (炉_状态机结 *L, 炉L_缓冲区结 *B) {
  B->L = L;
  B->b = B->初始的_变量.b;
  B->n = 0;
  B->大小_变量 = 炉L_缓冲区大小;
  月亮推入轻量用户数据_函(L, (void*)B);  /* push placeholder */
}


配置_月亮库_应程接_宏名 char *月亮状态_缓冲初始的大小_函 (炉_状态机结 *L, 炉L_缓冲区结 *B, size_t 大小_短变量) {
  月亮状态_缓冲初始的_函(L, B);
  return 月辅助库_预备缓冲大小_函(B, 大小_短变量, -1);
}

/* }====================================================== */


/*
** {======================================================
** Reference system
** =======================================================
*/

/* index of free-列表_变量 header (after the predefined values) */
#define 辅助库_自由列表_宏名	(月头_月亮_注册索引_最后的_宏名 + 1)

/*
** The previously freed references 来自_变量 a linked 列表_变量:
** t[辅助库_自由列表_宏名] is the index of a 首先_变量 free index, or zero if 列表_变量 is
** 空容器_变量; t[t[辅助库_自由列表_宏名]] is the index of the second element; etc.
*/
配置_月亮库_应程接_宏名 int 月亮状态_引用_函 (炉_状态机结 *L, int t) {
  int 引用_短变量;
  if (月头_月亮_是否空值_宏名(L, -1)) {
    月头_月亮_弹出_宏名(L, 1);  /* remove from 栈_圆小 */
    return 辅助库_月亮_引用空值_宏名;  /* 'nil' has a unique fixed reference */
  }
  t = 月亮_绝对索引_函(L, t);
  if (月亮获取原始索引_函(L, t, 辅助库_自由列表_宏名) == 月头_月亮_T空值_宏名) {  /* 首先_变量 access? */
    引用_短变量 = 0;  /* 列表_变量 is 空容器_变量 */
    月亮推入整数_函(L, 0);  /* initialize as an 空容器_变量 列表_变量 */
    月亮_设置原始索引_函(L, t, 辅助库_自由列表_宏名);  /* 引用_短变量 = t[辅助库_自由列表_宏名] = 0 */
  }
  else {  /* already initialized */
    限制_月亮_断言_宏名(月亮_是否整数_函(L, -1));
    引用_短变量 = (int)月头_月亮_到整数_宏名(L, -1);  /* 引用_短变量 = t[辅助库_自由列表_宏名] */
  }
  月头_月亮_弹出_宏名(L, 1);  /* remove element from 栈_圆小 */
  if (引用_短变量 != 0) {  /* any free element? */
    月亮获取原始索引_函(L, t, 引用_短变量);  /* remove it from 列表_变量 */
    月亮_设置原始索引_函(L, t, 辅助库_自由列表_宏名);  /* (t[辅助库_自由列表_宏名] = t[引用_短变量]) */
  }
  else  /* no free elements */
    引用_短变量 = (int)月亮_原始长度_函(L, t) + 1;  /* get a new reference */
  月亮_设置原始索引_函(L, t, 引用_短变量);
  return 引用_短变量;
}


配置_月亮库_应程接_宏名 void 月亮状态_取消引用_函 (炉_状态机结 *L, int t, int 引用_短变量) {
  if (引用_短变量 >= 0) {
    t = 月亮_绝对索引_函(L, t);
    月亮获取原始索引_函(L, t, 辅助库_自由列表_宏名);
    限制_月亮_断言_宏名(月亮_是否整数_函(L, -1));
    月亮_设置原始索引_函(L, t, 引用_短变量);  /* t[引用_短变量] = t[辅助库_自由列表_宏名] */
    月亮推入整数_函(L, 引用_短变量);
    月亮_设置原始索引_函(L, t, 辅助库_自由列表_宏名);  /* t[辅助库_自由列表_宏名] = 引用_短变量 */
  }
}

/* }====================================================== */


/*
** {======================================================
** Load functions
** =======================================================
*/

typedef struct 载函_结 {
  int n;  /* number of pre-read characters */
  FILE *f;  /* file being read */
  char 缓冲_变量[BUFSIZ];  /* area for reading file */
} 载函_结;


static const char *月辅助库_获取F_函 (炉_状态机结 *L, void *用数_缩变量, size_t *大小_变量) {
  载函_结 *行函_缩变量 = (载函_结 *)用数_缩变量;
  (void)L;  /* not used */
  if (行函_缩变量->n > 0) {  /* are there pre-read characters 到_变量 be read? */
    *大小_变量 = 行函_缩变量->n;  /* return them (chars already in 缓冲区_变量) */
    行函_缩变量->n = 0;  /* no 更多_变量 pre-read characters */
  }
  else {  /* read a 月解析器_块_函 from file */
    /* 'fread' can return > 0 *and* set the EOF flag. If 下一个_变量 call 到_变量
       '月辅助库_获取F_函' called 'fread', it might still wait for user input.
       The 下一个_变量 月解析器_检查_函 avoids this problem. */
    if (feof(行函_缩变量->f)) return NULL;
    *大小_变量 = fread(行函_缩变量->缓冲_变量, 1, sizeof(行函_缩变量->缓冲_变量), 行函_缩变量->f);  /* read 月解析器_块_函 */
  }
  return 行函_缩变量->缓冲_变量;
}


static int 月辅助库_错文件_函 (炉_状态机结 *L, const char *什么_变量, int 函名索引_变量) {
  const char *流错_变量 = strerror(errno);
  const char *文件名_变量 = 月头_月亮_到字符串_宏名(L, 函名索引_变量) + 1;
  月亮推入格式化字符串_函(L, "月编译器_不能_函 %s %s: %s", 什么_变量, 文件名_变量, 流错_变量);
  月头_月亮_移除_宏名(L, 函名索引_变量);
  return 辅助库_月亮_错误文件_宏名;
}


/*
** Skip an optional BOM at the 起始_变量 of a stream. If there is an
** 月解释器_不完整的_函 BOM (the 首先_变量 character is correct but the rest is
** not), returns the 首先_变量 character anyway 到_变量 force an 错误_小变量
** (as no chunk can 起始_变量 with 0xEF).
*/
static int 月辅助库_跳过BOM_函 (FILE *f) {
  int c = getc(f);  /* read 首先_变量 character */
  if (c == 0xEF && getc(f) == 0xBB && getc(f) == 0xBF)  /* correct BOM? */
    return getc(f);  /* ignore BOM and return 下一个_变量 char */
  else  /* no (有效_变量) BOM */
    return c;  /* return 首先_变量 character */
}


/*
** reads the 首先_变量 character of file 'f' and skips an optional BOM 记号_变量
** in its beginning plus its 首先_变量 行_变量 if it starts with '#'. Returns
** true if it skipped the 首先_变量 行_变量.  In any case, '*cp' has the
** 首先_变量 "有效_变量" character of the file (after the optional BOM and
** a 首先_变量-行_变量 comment).
*/
static int 月辅助库_跳过注释_函 (FILE *f, int *cp) {
  int c = *cp = 月辅助库_跳过BOM_函(f);
  if (c == '#') {  /* 首先_变量 行_变量 is a comment (Unix exec. file)? */
    do {  /* skip 首先_变量 行_变量 */
      c = getc(f);
    } while (c != EOF && c != '\n');
    *cp = getc(f);  /* 下一个_变量 character after comment, if present */
    return 1;  /* there was a comment */
  }
  else return 0;  /* no comment */
}


配置_月亮库_应程接_宏名 int 月亮状态_加载文件x_函 (炉_状态机结 *L, const char *文件名_变量,
                                             const char *模块_变量) {
  载函_结 行函_缩变量;
  int 状态码_变量, 读状态码_变量;
  int c;
  int 函名索引_变量 = 月亮_获取顶_函(L) + 1;  /* index of 文件名_变量 on the 栈_圆小 */
  if (文件名_变量 == NULL) {
    月头_月亮_推字面_宏名(L, "=stdin");
    行函_缩变量.f = stdin;
  }
  else {
    月亮推入格式化字符串_函(L, "@%s", 文件名_变量);
    行函_缩变量.f = fopen(文件名_变量, "r");
    if (行函_缩变量.f == NULL) return 月辅助库_错文件_函(L, "打开_圆", 函名索引_变量);
  }
  行函_缩变量.n = 0;
  if (月辅助库_跳过注释_函(行函_缩变量.f, &c))  /* read initial portion */
    行函_缩变量.缓冲_变量[行函_缩变量.n++] = '\n';  /* add 新行_小变量 到_变量 correct 行_变量 numbers */
  if (c == 月头_月亮_签名_宏名[0]) {  /* binary file? */
    行函_缩变量.n = 0;  /* remove possible 新行_小变量 */
    if (文件名_变量) {  /* "real" file? */
      行函_缩变量.f = freopen(文件名_变量, "寄b_变量", 行函_缩变量.f);  /* reopen in binary 模块_变量 */
      if (行函_缩变量.f == NULL) return 月辅助库_错文件_函(L, "reopen", 函名索引_变量);
      月辅助库_跳过注释_函(行函_缩变量.f, &c);  /* re-read initial portion */
    }
  }
  if (c != EOF)
    行函_缩变量.缓冲_变量[行函_缩变量.n++] = c;  /* 'c' is the 首先_变量 character of the stream */
  状态码_变量 = 月亮_加载_函(L, 月辅助库_获取F_函, &行函_缩变量, 月头_月亮_到字符串_宏名(L, -1), 模块_变量);
  读状态码_变量 = ferror(行函_缩变量.f);
  if (文件名_变量) fclose(行函_缩变量.f);  /* 关闭_圆 file (even in case of errors) */
  if (读状态码_变量) {
    月亮_设置顶_函(L, 函名索引_变量);  /* ignore results from '月亮_加载_函' */
    return 月辅助库_错文件_函(L, "read", 函名索引_变量);
  }
  月头_月亮_移除_宏名(L, 函名索引_变量);
  return 状态码_变量;
}


typedef struct 载串_结 {
  const char *s;
  size_t 大小_变量;
} 载串_结;


static const char *月辅助库_获取S_函 (炉_状态机结 *L, void *用数_缩变量, size_t *大小_变量) {
  载串_结 *状列_缩变量 = (载串_结 *)用数_缩变量;
  (void)L;  /* not used */
  if (状列_缩变量->大小_变量 == 0) return NULL;
  *大小_变量 = 状列_缩变量->大小_变量;
  状列_缩变量->大小_变量 = 0;
  return 状列_缩变量->s;
}


配置_月亮库_应程接_宏名 int 月亮状态_加载缓冲区x_函 (炉_状态机结 *L, const char *缓冲_变量, size_t 大小_变量,
                                 const char *名称_变量, const char *模块_变量) {
  载串_结 状列_缩变量;
  状列_缩变量.s = 缓冲_变量;
  状列_缩变量.大小_变量 = 大小_变量;
  return 月亮_加载_函(L, 月辅助库_获取S_函, &状列_缩变量, 名称_变量, 模块_变量);
}


配置_月亮库_应程接_宏名 int 月亮状态_加载字符串_函 (炉_状态机结 *L, const char *s) {
  return 辅助库_月亮l_载入缓冲区_宏名(L, s, strlen(s), s);
}

/* }====================================================== */



配置_月亮库_应程接_宏名 int 月亮状态_获取元字段_函 (炉_状态机结 *L, int 对象_变量, const char *事件_变量) {
  if (!月亮获取元表_函(L, 对象_变量))  /* no 元表_小写? */
    return 月头_月亮_T空值_宏名;
  else {
    int 类标_缩变量;
    月亮推入字符串_函(L, 事件_变量);
    类标_缩变量 = 月亮获取原始_函(L, -2);
    if (类标_缩变量 == 月头_月亮_T空值_宏名)  /* is metafield nil? */
      月头_月亮_弹出_宏名(L, 2);  /* remove 元表_小写 and metafield */
    else
      月头_月亮_移除_宏名(L, -2);  /* remove only 元表_小写 */
    return 类标_缩变量;  /* return metafield type */
  }
}


配置_月亮库_应程接_宏名 int 月亮状态_调用元_函 (炉_状态机结 *L, int 对象_变量, const char *事件_变量) {
  对象_变量 = 月亮_绝对索引_函(L, 对象_变量);
  if (月亮状态_获取元字段_函(L, 对象_变量, 事件_变量) == 月头_月亮_T空值_宏名)  /* no metafield? */
    return 0;
  月亮_推入值_函(L, 对象_变量);
  月头_月亮_调用_宏名(L, 1, 1);
  return 1;
}


配置_月亮库_应程接_宏名 炉_整数型 月亮状态_长度_函 (炉_状态机结 *L, int 索引_缩变量) {
  炉_整数型 l;
  int 是否数目_变量;
  月亮_长度_函(L, 索引_缩变量);
  l = 月亮_到整数x_函(L, -1, &是否数目_变量);
  if (配置_l_可能性低_宏名(!是否数目_变量))
    月亮状态_错误_函(L, "object length is not an integer");
  月头_月亮_弹出_宏名(L, 1);  /* remove object */
  return l;
}


配置_月亮库_应程接_宏名 const char *月亮状态_到字符串_函 (炉_状态机结 *L, int 索引_缩变量, size_t *长度_短变量) {
  索引_缩变量 = 月亮_绝对索引_函(L,索引_缩变量);
  if (月亮状态_调用元_函(L, 索引_缩变量, "__tostring")) {  /* metafield? */
    if (!月亮_是否字符串_函(L, -1))
      月亮状态_错误_函(L, "'__tostring' must return a string");
  }
  else {
    switch (月亮_类型_函(L, 索引_缩变量)) {
      case 月头_月亮_T数目_宏名: {
        if (月亮_是否整数_函(L, 索引_缩变量))
          月亮推入格式化字符串_函(L, "%I", (配置_月亮I_UAC整型_宏名)月头_月亮_到整数_宏名(L, 索引_缩变量));
        else
          月亮推入格式化字符串_函(L, "%f", (配置_月亮I_UAC数目_宏名)月头_月亮_到数目_宏名(L, 索引_缩变量));
        break;
      }
      case 月头_月亮_T字符串_宏名:
        月亮_推入值_函(L, 索引_缩变量);
        break;
      case 月头_月亮_T布尔_宏名:
        月亮推入字符串_函(L, (月亮_到布尔值_函(L, 索引_缩变量) ? "true" : "false"));
        break;
      case 月头_月亮_T空值_宏名:
        月头_月亮_推字面_宏名(L, "nil");
        break;
      default: {
        int 类标_缩变量 = 月亮状态_获取元字段_函(L, 索引_缩变量, "__name");  /* try 名称_变量 */
        const char *种类_变量 = (类标_缩变量 == 月头_月亮_T字符串_宏名) ? 月头_月亮_到字符串_宏名(L, -1) :
                                                 辅助库_月亮l_类型名称_宏名(L, 索引_缩变量);
        月亮推入格式化字符串_函(L, "%s: %p", 种类_变量, 月亮到指针_函(L, 索引_缩变量));
        if (类标_缩变量 != 月头_月亮_T空值_宏名)
          月头_月亮_移除_宏名(L, -2);  /* remove '__name' */
        break;
      }
    }
  }
  return 月亮_到长字符串_函(L, -1, 长度_短变量);
}


/*
** set functions from 列表_变量 'l' into table at 顶部_变量 - '数量上_短变量'; each
** function gets the '数量上_短变量' elements at the 顶部_变量 as 上值们_小写.
** Returns with only the table at the 栈_圆小.
*/
配置_月亮库_应程接_宏名 void 月亮状态_设置函们_函 (炉_状态机结 *L, const 炉L_寄结 *l, int 数量上_短变量) {
  月亮状态_检查栈_函(L, 数量上_短变量, "too many 上值们_小写");
  for (; l->名称_变量 != NULL; l++) {  /* fill the table with given functions */
    if (l->函_短变量 == NULL)  /* place holder? */
      月亮推入布尔值_函(L, 0);
    else {
      int i;
      for (i = 0; i < 数量上_短变量; i++)  /* copy 上值们_小写 到_变量 the 顶部_变量 */
        月亮_推入值_函(L, -数量上_短变量);
      月亮推入C闭包_函(L, l->函_短变量, 数量上_短变量);  /* closure with those 上值们_小写 */
    }
    月亮设置字段_函(L, -(数量上_短变量 + 2), l->名称_变量);
  }
  月头_月亮_弹出_宏名(L, 数量上_短变量);  /* remove 上值们_小写 */
}


/*
** ensure that 栈_圆小[索引_缩变量][函名_缩变量] has a table and push that table
** into the 栈_圆小
*/
配置_月亮库_应程接_宏名 int 月亮状态_获取子表_函 (炉_状态机结 *L, int 索引_缩变量, const char *函名_缩变量) {
  if (月亮获取字段_函(L, 索引_缩变量, 函名_缩变量) == 月头_月亮_T表_宏名)
    return 1;  /* table already there */
  else {
    月头_月亮_弹出_宏名(L, 1);  /* remove 前一个_变量 结果_变量 */
    索引_缩变量 = 月亮_绝对索引_函(L, 索引_缩变量);
    月头_月亮_新表_宏名(L);
    月亮_推入值_函(L, -1);  /* copy 到_变量 be 左_圆 at 顶部_变量 */
    月亮设置字段_函(L, 索引_缩变量, 函名_缩变量);  /* assign new table 到_变量 月解析器_字段_函 */
    return 0;  /* false, because did not find table there */
  }
}


/*
** Stripped-down 'require': After checking "loaded" table, calls 'openf'
** 到_变量 打开_圆 a module, registers the 结果_变量 in 'package.loaded' table and,
** if 'glb' is true, also registers the 结果_变量 in the global table.
** Leaves resulting module on the 顶部_变量.
*/
配置_月亮库_应程接_宏名 void 月亮状态_要求引用_函 (炉_状态机结 *L, const char *模名_变量,
                               炉_C函数半 openf, int glb) {
  月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_已载入_表_宏名);
  月亮获取字段_函(L, -1, 模名_变量);  /* LOADED[模名_变量] */
  if (!月亮_到布尔值_函(L, -1)) {  /* package not already loaded? */
    月头_月亮_弹出_宏名(L, 1);  /* remove 月解析器_字段_函 */
    月头_月亮_推C函数_宏名(L, openf);
    月亮推入字符串_函(L, 模名_变量);  /* argument 到_变量 打开_圆 function */
    月头_月亮_调用_宏名(L, 1, 1);  /* call 'openf' 到_变量 打开_圆 module */
    月亮_推入值_函(L, -1);  /* make copy of module (call 结果_变量) */
    月亮设置字段_函(L, -3, 模名_变量);  /* LOADED[模名_变量] = module */
  }
  月头_月亮_移除_宏名(L, -2);  /* remove LOADED table */
  if (glb) {
    月亮_推入值_函(L, -1);  /* copy of module */
    月亮设置全局变量_函(L, 模名_变量);  /* _G[模名_变量] = module */
  }
}


配置_月亮库_应程接_宏名 void 月亮状态_添加全局替换_函 (炉L_缓冲区结 *b, const char *s,
                                     const char *p, const char *r) {
  const char *狂野_变量;
  size_t l = strlen(p);
  while ((狂野_变量 = strstr(s, p)) != NULL) {
    月亮状态_添加状态字符串_函(b, s, 狂野_变量 - s);  /* push prefix */
    月亮状态_添加字符串_函(b, r);  /* push replacement in place of pattern */
    s = 狂野_变量 + l;  /* continue after 'p' */
  }
  月亮状态_添加字符串_函(b, s);  /* push 最后_变量 suffix */
}


配置_月亮库_应程接_宏名 const char *月亮状态_全局替换_函 (炉_状态机结 *L, const char *s,
                                  const char *p, const char *r) {
  炉L_缓冲区结 b;
  月亮状态_缓冲初始的_函(L, &b);
  月亮状态_添加全局替换_函(&b, s, p, r);
  月亮状态_推入结果_函(&b);
  return 月头_月亮_到字符串_宏名(L, -1);
}


static void *月辅助库_l_分配_函 (void *用数_缩变量, void *ptr, size_t 原始大小_变量, size_t 新大小_变量) {
  (void)用数_缩变量; (void)原始大小_变量;  /* not used */
  if (新大小_变量 == 0) {
    free(ptr);
    return NULL;
  }
  else
    return realloc(ptr, 新大小_变量);
}


static int 恐慌_圆 (炉_状态机结 *L) {
  const char *消息_缩变量 = 月头_月亮_到字符串_宏名(L, -1);
  if (消息_缩变量 == NULL) 消息_缩变量 = "错误_小变量 object is not a string";
  辅助库_月亮_写字符串错误_宏名("PANIC: unprotected 错误_小变量 in call 到_变量 Lua API (%s)\n",
                        消息_缩变量);
  return 0;  /* return 到_变量 Lua 到_变量 abort */
}


/*
** Warning functions:
** 月辅助库_关闭警告格式_函: warning system is off
** 月辅助库_打开警告格式_函: ready 到_变量 起始_变量 a new message
** 月辅助库_继续警告格式_函: 前一个_变量 message is 到_变量 be continued
*/
static void 月辅助库_关闭警告格式_函 (void *用数_缩变量, const char *message, int tocont);
static void 月辅助库_打开警告格式_函 (void *用数_缩变量, const char *message, int tocont);
static void 月辅助库_继续警告格式_函 (void *用数_缩变量, const char *message, int tocont);


/*
** Check whether message is a control message. If so, execute the
** control or ignore it if unknown.
*/
static int 月辅助库_检查控制_函 (炉_状态机结 *L, const char *message, int tocont) {
  if (tocont || *(message++) != '@')  /* not a control message? */
    return 0;
  else {
    if (strcmp(message, "off") == 0)
      月亮_设置警告函数_函(L, 月辅助库_关闭警告格式_函, L);  /* turn warnings off */
    else if (strcmp(message, "on") == 0)
      月亮_设置警告函数_函(L, 月辅助库_打开警告格式_函, L);   /* turn warnings on */
    return 1;  /* it was a control message */
  }
}


static void 月辅助库_关闭警告格式_函 (void *用数_缩变量, const char *message, int tocont) {
  月辅助库_检查控制_函((炉_状态机结 *)用数_缩变量, message, tocont);
}


/*
** Writes the message and handle 'tocont', finishing the message
** if 已需要_变量 and setting the 下一个_变量 warn function.
*/
static void 月辅助库_继续警告格式_函 (void *用数_缩变量, const char *message, int tocont) {
  炉_状态机结 *L = (炉_状态机结 *)用数_缩变量;
  辅助库_月亮_写字符串错误_宏名("%s", message);  /* write message */
  if (tocont)  /* not the 最后_变量 part? */
    月亮_设置警告函数_函(L, 月辅助库_继续警告格式_函, L);  /* 到_变量 be continued */
  else {  /* 最后_变量 part */
    辅助库_月亮_写字符串错误_宏名("%s", "\n");  /* finish message with 终_变量-of-行_变量 */
    月亮_设置警告函数_函(L, 月辅助库_打开警告格式_函, L);  /* 下一个_变量 call is a new message */
  }
}


static void 月辅助库_打开警告格式_函 (void *用数_缩变量, const char *message, int tocont) {
  if (月辅助库_检查控制_函((炉_状态机结 *)用数_缩变量, message, tocont))  /* control message? */
    return;  /* nothing else 到_变量 be done */
  辅助库_月亮_写字符串错误_宏名("%s", "Lua warning: ");  /* 起始_变量 a new warning */
  月辅助库_继续警告格式_函(用数_缩变量, message, tocont);  /* finish processing */
}


配置_月亮库_应程接_宏名 炉_状态机结 *月亮状态_新状态_函 (void) {
  炉_状态机结 *L = 月亮_新状态机_函(月辅助库_l_分配_函, NULL);
  if (配置_l_可能性高_宏名(L)) {
    月亮_设置恐慌函数_函(L, &恐慌_圆);
    月亮_设置警告函数_函(L, 月辅助库_关闭警告格式_函, L);  /* default is warnings off */
  }
  return L;
}


配置_月亮库_应程接_宏名 void 月亮状态_检查版本_函 (炉_状态机结 *L, 炉_数目型 ver, size_t 大小_短变量) {
  炉_数目型 v = 月亮_版本_函(L);
  if (大小_短变量 != 辅助库_月亮L_数量大小_宏名)  /* 月解析器_检查_函 numeric types */
    月亮状态_错误_函(L, "core and library have incompatible numeric types");
  else if (v != ver)
    月亮状态_错误_函(L, "版本_变量 mismatch: app. needs %f, Lua core provides %f",
                  (配置_月亮I_UAC数目_宏名)ver, (配置_月亮I_UAC数目_宏名)v);
}

