/*
** $Id: linit.c $
** Initialization of libraries for lua.c and other clients
** See Copyright Notice in lua.h
*/


#define linit_c
#define 辅助库_月亮_库_宏名

/*
** If you embed Lua in your program and need 到_变量 打开_圆 the standard
** libraries, call 月亮库打开库们_函 in your program. If you need a
** different set of libraries, copy this file 到_变量 your 月数学库_投影_函 and edit
** it 到_变量 suit your needs.
**
** You can also *preload* libraries, so that a later 'require' can
** 打开_圆 the library, which is already linked 到_变量 the application.
** For that, do the following 代码_变量:
**
**  月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_预载入_表_宏名);
**  月头_月亮_推C函数_宏名(L, luaopen_modname);
**  月亮设置字段_函(L, -2, 模名_变量);
**  月头_月亮_弹出_宏名(L, 1);  // remove PRELOAD table
*/

#include "lprefix.h"


#include <stddef.h>

#include "lua.h"

#include "lualib.h"
#include "lauxlib.h"


/*
** these libs are loaded by lua.c and are readily available 到_变量 any Lua
** program
*/
static const 炉L_寄结 已载入库_变量[] = {
  {辅助库_月亮_全局名称_宏名, 月亮打开基本_函},
  {库_月亮_载入库名_宏名, 月亮打开包_函},
  {库_月亮_协程库名_宏名, 月亮打开协程_函},
  {库_月亮_表库名_宏名, 月亮打开表_函},
  {库_月亮_入出库名_宏名, 月亮打开输入输出_函},
  {库_月亮_系统库名_宏名, 月亮打开操作系统_函},
  {库_月亮_串库名_宏名, 月亮打开字符串_函},
  {库_月亮_数学库名_宏名, 月亮打开数学_函},
  {库_月亮_UTF8库名_宏名, 月亮打开UTF8_函},
  {库_月亮_调试库名_宏名, 月亮打开调试_函},
  {NULL, NULL}
};


配置_月亮库_应程接_宏名 void 月亮库打开库们_函 (炉_状态机结 *L) {
  const 炉L_寄结 *库_短变量;
  /* "require" functions from '已载入库_变量' and set results 到_变量 global table */
  for (库_短变量 = 已载入库_变量; 库_短变量->函_短变量; 库_短变量++) {
    月亮状态_要求引用_函(L, 库_短变量->名称_变量, 库_短变量->函_短变量, 1);
    月头_月亮_弹出_宏名(L, 1);  /* remove 库_短变量 */
  }
}

