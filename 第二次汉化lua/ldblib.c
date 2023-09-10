/*
** $Id: ldblib.c $
** Interface from Lua 到_变量 its debug API
** See Copyright Notice in lua.h
*/

#define ldblib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/*
** The 钩子_变量 table at 注册表_变量[钩子键_变量] maps threads 到_变量 their 当前_圆
** 钩子_变量 function.
*/
static const char *const 钩子键_变量 = "_HOOKKEY";


/*
** If L1 != L, L1 can be in any 状态机_变量, and therefore there are no
** guarantees about its 栈_圆小 空间_圆; any push in L1 must be
** checked.
*/
static void 月调试库_检查栈_函 (炉_状态机结 *L, 炉_状态机结 *L1, int n) {
  if (配置_l_可能性低_宏名(L != L1 && !月亮_检查栈空间_函(L1, n)))
    月亮状态_错误_函(L, "栈_圆小 overflow");
}


static int 月调试库_db_获取注册表_函 (炉_状态机结 *L) {
  月亮_推入值_函(L, 月头_月亮_注册表索引_宏名);
  return 1;
}


static int 月调试库_db_获取元表_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 1);
  if (!月亮获取元表_函(L, 1)) {
    月亮推入空值_函(L);  /* no 元表_小写 */
  }
  return 1;
}


static int 月调试库_db_设置元表_函 (炉_状态机结 *L) {
  int t = 月亮_类型_函(L, 2);
  辅助库_月亮l_实参期望_宏名(L, t == 月头_月亮_T空值_宏名 || t == 月头_月亮_T表_宏名, 2, "nil or table");
  月亮_设置顶_函(L, 2);
  月亮_设置元表_函(L, 1);
  return 1;  /* return 1st argument */
}


static int 月调试库_db_获取用户值_函 (炉_状态机结 *L) {
  int n = (int)月亮状态_可选整数_函(L, 2, 1);
  if (月亮_类型_函(L, 1) != 月头_月亮_T用户数据_宏名)
    辅助库_月亮l_推失败_宏名(L);
  else if (月亮获取索引用户值_函(L, 1, n) != 月头_月亮_T没有_宏名) {
    月亮推入布尔值_函(L, 1);
    return 2;
  }
  return 1;
}


static int 月调试库_db_设置用户值_函 (炉_状态机结 *L) {
  int n = (int)月亮状态_可选整数_函(L, 3, 1);
  月亮状态_检查类型_函(L, 1, 月头_月亮_T用户数据_宏名);
  月亮状态_检查任意_函(L, 2);
  月亮_设置顶_函(L, 2);
  if (!月亮_设置索引用户值_函(L, 1, n))
    辅助库_月亮l_推失败_宏名(L);
  return 1;
}


/*
** Auxiliary function used by several library functions: 月解析器_检查_函 for
** an optional 线程_变量 as function's 首先_变量 argument and set '实参_短变量' with
** 1 if this argument is present (so that functions can skip it 到_变量
** access their other arguments)
*/
static 炉_状态机结 *月调试库_获取线程_函 (炉_状态机结 *L, int *实参_短变量) {
  if (月头_月亮_是否线程_宏名(L, 1)) {
    *实参_短变量 = 1;
    return 月亮到线程_函(L, 1);
  }
  else {
    *实参_短变量 = 0;
    return L;  /* function will operate over 当前_圆 线程_变量 */
  }
}


/*
** Variations of '月亮设置表_函', used by '月调试库_db_获取信息_函' 到_变量 put results
** from '月亮_获取信息_函' into 结果_变量 table. Key is always a string;
** 值_圆 can be a string, an int, or a boolean.
*/
static void 月调试库_设置表串键_函 (炉_状态机结 *L, const char *k, const char *v) {
  月亮推入字符串_函(L, v);
  月亮设置字段_函(L, -2, k);
}

static void 月调试库_设置表整数键_函 (炉_状态机结 *L, const char *k, int v) {
  月亮推入整数_函(L, v);
  月亮设置字段_函(L, -2, k);
}

static void 月调试库_设置表布尔键_函 (炉_状态机结 *L, const char *k, int v) {
  月亮推入布尔值_函(L, v);
  月亮设置字段_函(L, -2, k);
}


/*
** In function '月调试库_db_获取信息_函', the call 到_变量 '月亮_获取信息_函' may push
** results on the 栈_圆小; later it creates the 结果_变量 table 到_变量 put
** these objects. Function '月调试库_对待栈选项_函' puts the 结果_变量 from
** '月亮_获取信息_函' on 顶部_变量 of the 结果_变量 table so that it can call
** '月亮设置字段_函'.
*/
static void 月调试库_对待栈选项_函 (炉_状态机结 *L, 炉_状态机结 *L1, const char *函名_缩变量) {
  if (L == L1)
    月亮_旋转_函(L, -2, 1);  /* exchange object and table */
  else
    月亮_传递数据_函(L1, L, 1);  /* move object 到_变量 the "main" 栈_圆小 */
  月亮设置字段_函(L, -2, 函名_缩变量);  /* put object into table */
}


/*
** Calls '月亮_获取信息_函' and collects all results in a new table.
** L1 needs 栈_圆小 空间_圆 for an optional input (function) plus
** two optional outputs (function and 行_变量 table) from function
** '月亮_获取信息_函'.
*/
static int 月调试库_db_获取信息_函 (炉_状态机结 *L) {
  炉_调试结 活记_缩变量;
  int 实参_短变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  const char *选项们_变量 = 辅助库_月亮l_可选字符串_宏名(L, 实参_短变量+2, "flnSrtu");
  月调试库_检查栈_函(L, L1, 3);
  辅助库_月亮l_实参检查_宏名(L, 选项们_变量[0] != '>', 实参_短变量 + 2, "invalid 选项_变量 '>'");
  if (月头_月亮_是否函数_宏名(L, 实参_短变量 + 1)) {  /* 信息_短变量 about a function? */
    选项们_变量 = 月亮推入格式化字符串_函(L, ">%s", 选项们_变量);  /* add '>' 到_变量 '选项们_变量' */
    月亮_推入值_函(L, 实参_短变量 + 1);  /* move function 到_变量 'L1' 栈_圆小 */
    月亮_传递数据_函(L, L1, 1);
  }
  else {  /* 栈_圆小 层级_变量 */
    if (!月亮_获取栈_函(L1, (int)月亮状态_检查整数_函(L, 实参_短变量 + 1), &活记_缩变量)) {
      辅助库_月亮l_推失败_宏名(L);  /* 层级_变量 out of range */
      return 1;
    }
  }
  if (!月亮_获取信息_函(L1, 选项们_变量, &活记_缩变量))
    return 月亮状态_实参错误_函(L, 实参_短变量+2, "invalid 选项_变量");
  月头_月亮_新表_宏名(L);  /* table 到_变量 collect results */
  if (strchr(选项们_变量, 'S')) {
    月亮推入长字符串_函(L, 活记_缩变量.源_圆, 活记_缩变量.源的长_短);
    月亮设置字段_函(L, -2, "源_圆");
    月调试库_设置表串键_函(L, "短的_源小写", 活记_缩变量.短的_源小写);
    月调试库_设置表整数键_函(L, "已定义行_小写", 活记_缩变量.已定义行_小写);
    月调试库_设置表整数键_函(L, "最后已定义行_小写", 活记_缩变量.最后已定义行_小写);
    月调试库_设置表串键_函(L, "什么_变量", 活记_缩变量.什么_变量);
  }
  if (strchr(选项们_变量, 'l'))
    月调试库_设置表整数键_函(L, "当前行_变量", 活记_缩变量.当前行_变量);
  if (strchr(选项们_变量, 'u')) {
    月调试库_设置表整数键_函(L, "上值数_短缩", 活记_缩变量.上值数_短缩);
    月调试库_设置表整数键_函(L, "形参数量_变量", 活记_缩变量.形参数量_变量);
    月调试库_设置表布尔键_函(L, "是变量实参吗_变量", 活记_缩变量.是变量实参吗_变量);
  }
  if (strchr(选项们_变量, 'n')) {
    月调试库_设置表串键_函(L, "名称_变量", 活记_缩变量.名称_变量);
    月调试库_设置表串键_函(L, "什么名_圆", 活记_缩变量.什么名_圆);
  }
  if (strchr(选项们_变量, 'r')) {
    月调试库_设置表整数键_函(L, "函传输_变量", 活记_缩变量.函传输_变量);
    月调试库_设置表整数键_函(L, "转移数_缩", 活记_缩变量.转移数_缩);
  }
  if (strchr(选项们_变量, 't'))
    月调试库_设置表布尔键_函(L, "是尾调用吗_圆", 活记_缩变量.是尾调用吗_圆);
  if (strchr(选项们_变量, 'L'))
    月调试库_对待栈选项_函(L, L1, "activelines");
  if (strchr(选项们_变量, 'f'))
    月调试库_对待栈选项_函(L, L1, "函_短变量");
  return 1;  /* return table */
}


static int 月调试库_db_获取局部变量_函 (炉_状态机结 *L) {
  int 实参_短变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  int 变量数目_缩变量 = (int)月亮状态_检查整数_函(L, 实参_短变量 + 2);  /* local-variable index */
  if (月头_月亮_是否函数_宏名(L, 实参_短变量 + 1)) {  /* function argument? */
    月亮_推入值_函(L, 实参_短变量 + 1);  /* push function */
    月亮推入字符串_函(L, 月亮_获取本地变量_函(L, NULL, 变量数目_缩变量));  /* push local 名称_变量 */
    return 1;  /* return only 名称_变量 (there is no 值_圆) */
  }
  else {  /* 栈_圆小-层级_变量 argument */
    炉_调试结 活记_缩变量;
    const char *名称_变量;
    int 层级_变量 = (int)月亮状态_检查整数_函(L, 实参_短变量 + 1);
    if (配置_l_可能性低_宏名(!月亮_获取栈_函(L1, 层级_变量, &活记_缩变量)))  /* out of range? */
      return 月亮状态_实参错误_函(L, 实参_短变量+1, "层级_变量 out of range");
    月调试库_检查栈_函(L, L1, 1);
    名称_变量 = 月亮_获取本地变量_函(L1, &活记_缩变量, 变量数目_缩变量);
    if (名称_变量) {
      月亮_传递数据_函(L1, L, 1);  /* move local 值_圆 */
      月亮推入字符串_函(L, 名称_变量);  /* push 名称_变量 */
      月亮_旋转_函(L, -2, 1);  /* re-order */
      return 2;
    }
    else {
      辅助库_月亮l_推失败_宏名(L);  /* no 名称_变量 (nor 值_圆) */
      return 1;
    }
  }
}


static int 月调试库_db_设置局部变量_函 (炉_状态机结 *L) {
  int 实参_短变量;
  const char *名称_变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  炉_调试结 活记_缩变量;
  int 层级_变量 = (int)月亮状态_检查整数_函(L, 实参_短变量 + 1);
  int 变量数目_缩变量 = (int)月亮状态_检查整数_函(L, 实参_短变量 + 2);
  if (配置_l_可能性低_宏名(!月亮_获取栈_函(L1, 层级_变量, &活记_缩变量)))  /* out of range? */
    return 月亮状态_实参错误_函(L, 实参_短变量+1, "层级_变量 out of range");
  月亮状态_检查任意_函(L, 实参_短变量+3);
  月亮_设置顶_函(L, 实参_短变量+3);
  月调试库_检查栈_函(L, L1, 1);
  月亮_传递数据_函(L, L1, 1);
  名称_变量 = 月亮_设置本地变量_函(L1, &活记_缩变量, 变量数目_缩变量);
  if (名称_变量 == NULL)
    月头_月亮_弹出_宏名(L1, 1);  /* pop 值_圆 (if not popped by '月亮_设置本地变量_函') */
  月亮推入字符串_函(L, 名称_变量);
  return 1;
}


/*
** get (if 'get' is true) or set an 上值_圆 from a closure
*/
static int 月调试库_辅助上值_函 (炉_状态机结 *L, int get) {
  const char *名称_变量;
  int n = (int)月亮状态_检查整数_函(L, 2);  /* 上值_圆 index */
  月亮状态_检查类型_函(L, 1, 月头_月亮_T函数_宏名);  /* closure */
  名称_变量 = get ? 月亮_获取上值_函(L, 1, n) : 月亮_设置上值_函(L, 1, n);
  if (名称_变量 == NULL) return 0;
  月亮推入字符串_函(L, 名称_变量);
  月头_月亮_插入_宏名(L, -(get+1));  /* no-操作_短变量 if get is false */
  return get + 1;
}


static int 月调试库_db_获取上值_函 (炉_状态机结 *L) {
  return 月调试库_辅助上值_函(L, 1);
}


static int 月调试库_db_设置上值_函 (炉_状态机结 *L) {
  月亮状态_检查任意_函(L, 3);
  return 月调试库_辅助上值_函(L, 0);
}


/*
** Check whether a given 上值_圆 from a given closure exists and
** returns its index
*/
static void *月调试库_检查上值_函 (炉_状态机结 *L, int argf, int argnup, int *pnup) {
  void *身份_缩变量;
  int 数量上_短变量 = (int)月亮状态_检查整数_函(L, argnup);  /* 上值_圆 index */
  月亮状态_检查类型_函(L, argf, 月头_月亮_T函数_宏名);  /* closure */
  身份_缩变量 = 月亮_上值ID_函(L, argf, 数量上_短变量);
  if (pnup) {
    辅助库_月亮l_实参检查_宏名(L, 身份_缩变量 != NULL, argnup, "invalid 上值_圆 index");
    *pnup = 数量上_短变量;
  }
  return 身份_缩变量;
}


static int 月调试库_db_上值ID_函 (炉_状态机结 *L) {
  void *身份_缩变量 = 月调试库_检查上值_函(L, 1, 2, NULL);
  if (身份_缩变量 != NULL)
    月亮推入轻量用户数据_函(L, 身份_缩变量);
  else
    辅助库_月亮l_推失败_宏名(L);
  return 1;
}


static int 月调试库_db_上值加入_函 (炉_状态机结 *L) {
  int 数1_缩变量, 数2_缩变量;
  月调试库_检查上值_函(L, 1, 2, &数1_缩变量);
  月调试库_检查上值_函(L, 3, 4, &数2_缩变量);
  辅助库_月亮l_实参检查_宏名(L, !月亮_是否C函数_函(L, 1), 1, "Lua function expected");
  辅助库_月亮l_实参检查_宏名(L, !月亮_是否C函数_函(L, 3), 3, "Lua function expected");
  月亮_上值加入_函(L, 1, 数1_缩变量, 3, 数2_缩变量);
  return 0;
}


/*
** Call 钩子_变量 function registered at 钩子_变量 table for the 当前_圆
** 线程_变量 (if there is one)
*/
static void 月调试库_钩子函_函 (炉_状态机结 *L, 炉_调试结 *活记_缩变量) {
  static const char *const 钩子名_变量[] =
    {"call", "return", "行_变量", "计数_变量", "tail call"};
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 钩子键_变量);
  月亮推入线程_函(L);
  if (月亮获取原始_函(L, -2) == 月头_月亮_T函数_宏名) {  /* is there a 钩子_变量 function? */
    月亮推入字符串_函(L, 钩子名_变量[(int)活记_缩变量->事件_变量]);  /* push 事件_变量 名称_变量 */
    if (活记_缩变量->当前行_变量 >= 0)
      月亮推入整数_函(L, 活记_缩变量->当前行_变量);  /* push 当前_圆 行_变量 */
    else 月亮推入空值_函(L);
    限制_月亮_断言_宏名(月亮_获取信息_函(L, "lS", 活记_缩变量));
    月头_月亮_调用_宏名(L, 2, 0);  /* call 钩子_变量 function */
  }
}


/*
** Convert a string 掩码_变量 (for 'sethook') into a bit 掩码_变量
*/
static int 月调试库_制作掩码_函 (const char *子掩码_变量, int 计数_变量) {
  int 掩码_变量 = 0;
  if (strchr(子掩码_变量, 'c')) 掩码_变量 |= 月头_月亮_掩码调用_宏名;
  if (strchr(子掩码_变量, 'r')) 掩码_变量 |= 月头_月亮_掩码返回_宏名;
  if (strchr(子掩码_变量, 'l')) 掩码_变量 |= 月头_月亮_掩码行_宏名;
  if (计数_变量 > 0) 掩码_变量 |= 月头_月亮_掩码计数_宏名;
  return 掩码_变量;
}


/*
** Convert a bit 掩码_变量 (for 'gethook') into a string 掩码_变量
*/
static char *月调试库_反制作掩码_函 (int 掩码_变量, char *子掩码_变量) {
  int i = 0;
  if (掩码_变量 & 月头_月亮_掩码调用_宏名) 子掩码_变量[i++] = 'c';
  if (掩码_变量 & 月头_月亮_掩码返回_宏名) 子掩码_变量[i++] = 'r';
  if (掩码_变量 & 月头_月亮_掩码行_宏名) 子掩码_变量[i++] = 'l';
  子掩码_变量[i] = '\0';
  return 子掩码_变量;
}


static int 月调试库_db_设置钩子_函 (炉_状态机结 *L) {
  int 实参_短变量, 掩码_变量, 计数_变量;
  炉_钩子型 函_短变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  if (月头_月亮_是否没有或空值_宏名(L, 实参_短变量+1)) {  /* no 钩子_变量? */
    月亮_设置顶_函(L, 实参_短变量+1);
    函_短变量 = NULL; 掩码_变量 = 0; 计数_变量 = 0;  /* turn off hooks */
  }
  else {
    const char *子掩码_变量 = 辅助库_月亮l_检查字符串_宏名(L, 实参_短变量+2);
    月亮状态_检查类型_函(L, 实参_短变量+1, 月头_月亮_T函数_宏名);
    计数_变量 = (int)月亮状态_可选整数_函(L, 实参_短变量 + 3, 0);
    函_短变量 = 月调试库_钩子函_函; 掩码_变量 = 月调试库_制作掩码_函(子掩码_变量, 计数_变量);
  }
  if (!月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, 钩子键_变量)) {
    /* table just created; initialize it */
    月头_月亮_推字面_宏名(L, "k");
    月亮设置字段_函(L, -2, "__mode");  /** hooktable.__mode = "k" */
    月亮_推入值_函(L, -1);
    月亮_设置元表_函(L, -2);  /* 元表_小写(hooktable) = hooktable */
  }
  月调试库_检查栈_函(L, L1, 1);
  月亮推入线程_函(L1); 月亮_传递数据_函(L1, L, 1);  /* 键_小变量 (线程_变量) */
  月亮_推入值_函(L, 实参_短变量 + 1);  /* 值_圆 (钩子_变量 function) */
  月亮_设置原始_函(L, -3);  /* hooktable[L1] = new Lua 钩子_变量 */
  月亮_设置钩子_函(L1, 函_短变量, 掩码_变量, 计数_变量);
  return 0;
}


static int 月调试库_db_获取钩子_函 (炉_状态机结 *L) {
  int 实参_短变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  char 缓冲_变量[5];
  int 掩码_变量 = 月亮_获取钩子掩码_函(L1);
  炉_钩子型 钩子_变量 = 月亮_获取钩子_函(L1);
  if (钩子_变量 == NULL) {  /* no 钩子_变量? */
    辅助库_月亮l_推失败_宏名(L);
    return 1;
  }
  else if (钩子_变量 != 月调试库_钩子函_函)  /* external 钩子_变量? */
    月头_月亮_推字面_宏名(L, "external 钩子_变量");
  else {  /* 钩子_变量 table must exist */
    月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 钩子键_变量);
    月调试库_检查栈_函(L, L1, 1);
    月亮推入线程_函(L1); 月亮_传递数据_函(L1, L, 1);
    月亮获取原始_函(L, -2);   /* 1st 结果_变量 = hooktable[L1] */
    月头_月亮_移除_宏名(L, -2);  /* remove 钩子_变量 table */
  }
  月亮推入字符串_函(L, 月调试库_反制作掩码_函(掩码_变量, 缓冲_变量));  /* 2nd 结果_变量 = 掩码_变量 */
  月亮推入整数_函(L, 月亮_获取钩子计数_函(L1));  /* 3rd 结果_变量 = 计数_变量 */
  return 3;
}


static int 月调试库_db_调试_函 (炉_状态机结 *L) {
  for (;;) {
    char 缓冲区_变量[250];
    辅助库_月亮_写字符串错误_宏名("%s", "lua_debug> ");
    if (fgets(缓冲区_变量, sizeof(缓冲区_变量), stdin) == NULL ||
        strcmp(缓冲区_变量, "继续_短变量\n") == 0)
      return 0;
    if (辅助库_月亮l_载入缓冲区_宏名(L, 缓冲区_变量, strlen(缓冲区_变量), "=(debug command)") ||
        月头_月亮_保护调用_宏名(L, 0, 0, 0))
      辅助库_月亮_写字符串错误_宏名("%s\n", 月亮状态_到字符串_函(L, -1, NULL));
    月亮_设置顶_函(L, 0);  /* remove eventual returns */
  }
}


static int 月调试库_db_追溯_函 (炉_状态机结 *L) {
  int 实参_短变量;
  炉_状态机结 *L1 = 月调试库_获取线程_函(L, &实参_短变量);
  const char *消息_缩变量 = 月头_月亮_到字符串_宏名(L, 实参_短变量 + 1);
  if (消息_缩变量 == NULL && !月头_月亮_是否没有或空值_宏名(L, 实参_短变量 + 1))  /* non-string '消息_缩变量'? */
    月亮_推入值_函(L, 实参_短变量 + 1);  /* return it untouched */
  else {
    int 层级_变量 = (int)月亮状态_可选整数_函(L, 实参_短变量 + 2, (L == L1) ? 1 : 0);
    月亮状态_跟踪回溯_函(L, L1, 消息_缩变量, 层级_变量);
  }
  return 1;
}


static int 月调试库_db_设置C栈限制_函 (炉_状态机结 *L) {
  int 限制_变量 = (int)月亮状态_检查整数_函(L, 1);
  int 结果_短变量 = 月亮_设置C栈限制_函(L, 限制_变量);
  月亮推入整数_函(L, 结果_短变量);
  return 1;
}


static const 炉L_寄结 调试库_变量[] = {
  {"debug", 月调试库_db_调试_函},
  {"getuservalue", 月调试库_db_获取用户值_函},
  {"gethook", 月调试库_db_获取钩子_函},
  {"getinfo", 月调试库_db_获取信息_函},
  {"getlocal", 月调试库_db_获取局部变量_函},
  {"getregistry", 月调试库_db_获取注册表_函},
  {"getmetatable", 月调试库_db_获取元表_函},
  {"getupvalue", 月调试库_db_获取上值_函},
  {"upvaluejoin", 月调试库_db_上值加入_函},
  {"upvalueid", 月调试库_db_上值ID_函},
  {"setuservalue", 月调试库_db_设置用户值_函},
  {"sethook", 月调试库_db_设置钩子_函},
  {"setlocal", 月调试库_db_设置局部变量_函},
  {"setmetatable", 月调试库_db_设置元表_函},
  {"setupvalue", 月调试库_db_设置上值_函},
  {"traceback", 月调试库_db_追溯_函},
  {"setcstacklimit", 月调试库_db_设置C栈限制_函},
  {NULL, NULL}
};


配置_月亮模块_应程接_宏名 int 月亮打开调试_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 调试库_变量);
  return 1;
}

