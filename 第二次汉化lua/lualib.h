/*
** $Id: lualib.h $
** Lua standard libraries
** See Copyright Notice in lua.h
*/


#ifndef lualib_h
#define lualib_h

#include "lua.h"


/* 版本_变量 suffix for environment variable names */
#define 库_月亮_版本后缀_宏名          "_" 月头_月亮_版本_主版_宏名 "_" 月头_月亮_版本_副版_宏名


配置_月亮模块_应程接_宏名 int (月亮打开基本_函) (炉_状态机结 *L);

#define 库_月亮_协程库名_宏名	"coroutine"
配置_月亮模块_应程接_宏名 int (月亮打开协程_函) (炉_状态机结 *L);

#define 库_月亮_表库名_宏名	"table"
配置_月亮模块_应程接_宏名 int (月亮打开表_函) (炉_状态机结 *L);

#define 库_月亮_入出库名_宏名	"入出_变量"
配置_月亮模块_应程接_宏名 int (月亮打开输入输出_函) (炉_状态机结 *L);

#define 库_月亮_系统库名_宏名	"os"
配置_月亮模块_应程接_宏名 int (月亮打开操作系统_函) (炉_状态机结 *L);

#define 库_月亮_串库名_宏名	"string"
配置_月亮模块_应程接_宏名 int (月亮打开字符串_函) (炉_状态机结 *L);

#define 库_月亮_UTF8库名_宏名	"utf8"
配置_月亮模块_应程接_宏名 int (月亮打开UTF8_函) (炉_状态机结 *L);

#define 库_月亮_数学库名_宏名	"math"
配置_月亮模块_应程接_宏名 int (月亮打开数学_函) (炉_状态机结 *L);

#define 库_月亮_调试库名_宏名	"debug"
配置_月亮模块_应程接_宏名 int (月亮打开调试_函) (炉_状态机结 *L);

#define 库_月亮_载入库名_宏名	"package"
配置_月亮模块_应程接_宏名 int (月亮打开包_函) (炉_状态机结 *L);


/* 打开_圆 all 前一个_变量 libraries */
配置_月亮库_应程接_宏名 void (月亮库打开库们_函) (炉_状态机结 *L);


#endif
