/*
** $Id: lcorolib.c $
** Coroutine Library
** See Copyright Notice in lua.h
*/

#define lcorolib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <stdlib.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


static 炉_状态机结 *月核心库_获取协程_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月亮到线程_函(L, 1);
  辅助库_月亮l_实参期望_宏名(L, 协程_短变量, 1, "线程_变量");
  return 协程_短变量;
}


/*
** Resumes a coroutine. Returns the number of results for non-错误_小变量
** cases or -1 for errors.
*/
static int 月核心库_辅助恢复_函 (炉_状态机结 *L, 炉_状态机结 *协程_短变量, int 实参数_变量) {
  int 状态码_变量, 结果数目_变量;
  if (配置_l_可能性低_宏名(!月亮_检查栈空间_函(协程_短变量, 实参数_变量))) {
    月头_月亮_推字面_宏名(L, "too many arguments 到_变量 月做_恢复_函");
    return -1;  /* 错误_小变量 flag */
  }
  月亮_传递数据_函(L, 协程_短变量, 实参数_变量);
  状态码_变量 = 月亮_恢复_函(协程_短变量, L, 实参数_变量, &结果数目_变量);
  if (配置_l_可能性高_宏名(状态码_变量 == LUA_OK || 状态码_变量 == 月头_月亮_让步_宏名)) {
    if (配置_l_可能性低_宏名(!月亮_检查栈空间_函(L, 结果数目_变量 + 1))) {
      月头_月亮_弹出_宏名(协程_短变量, 结果数目_变量);  /* remove results anyway */
      月头_月亮_推字面_宏名(L, "too many results 到_变量 月做_恢复_函");
      return -1;  /* 错误_小变量 flag */
    }
    月亮_传递数据_函(协程_短变量, L, 结果数目_变量);  /* move yielded values */
    return 结果数目_变量;
  }
  else {
    月亮_传递数据_函(协程_短变量, L, 1);  /* move 错误_小变量 message */
    return -1;  /* 错误_小变量 flag */
  }
}


static int 月核心库_月亮B_协程恢复_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月核心库_获取协程_函(L);
  int r;
  r = 月核心库_辅助恢复_函(L, 协程_短变量, 月亮_获取顶_函(L) - 1);
  if (配置_l_可能性低_宏名(r < 0)) {
    月亮推入布尔值_函(L, 0);
    月头_月亮_插入_宏名(L, -2);
    return 2;  /* return false + 错误_小变量 message */
  }
  else {
    月亮推入布尔值_函(L, 1);
    月头_月亮_插入_宏名(L, -(r + 1));
    return r + 1;  /* return true + '月做_恢复_函' returns */
  }
}


static int 月核心库_月亮B_辅助包装_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月亮到线程_函(L, 月头_月亮_上值索引_宏名(1));
  int r = 月核心库_辅助恢复_函(L, 协程_短变量, 月亮_获取顶_函(L));
  if (配置_l_可能性低_宏名(r < 0)) {  /* 错误_小变量? */
    int 状态_短变量 = 月亮_状态码_函(协程_短变量);
    if (状态_短变量 != LUA_OK && 状态_短变量 != 月头_月亮_让步_宏名) {  /* 错误_小变量 in the coroutine? */
      状态_短变量 = 月亮_关闭线程_函(协程_短变量, L);  /* 关闭_圆 its 待关闭_缩变量 variables */
      限制_月亮_断言_宏名(状态_短变量 != LUA_OK);
      月亮_传递数据_函(协程_短变量, L, 1);  /* move 错误_小变量 message 到_变量 the caller */
    }
    if (状态_短变量 != 月头_月亮_内存错误_宏名 &&  /* not a memory 错误_小变量 and ... */
        月亮_类型_函(L, -1) == 月头_月亮_T字符串_宏名) {  /* ... 错误_小变量 object is a string? */
      月亮状态_哪里_函(L, 1);  /* add 额外_变量 信息_短变量, if available */
      月头_月亮_插入_宏名(L, -2);
      月亮_拼接_函(L, 2);
    }
    return 月亮_错误_函(L);  /* propagate 错误_小变量 */
  }
  return r;
}


static int 月核心库_月亮B_创建协程_函 (炉_状态机结 *L) {
  炉_状态机结 *新行_缩变量;
  月亮状态_检查类型_函(L, 1, 月头_月亮_T函数_宏名);
  新行_缩变量 = 月亮_新线程_函(L);
  月亮_推入值_函(L, 1);  /* move function 到_变量 顶部_变量 */
  月亮_传递数据_函(L, 新行_缩变量, 1);  /* move function from L 到_变量 新行_缩变量 */
  return 1;
}


static int 月核心库_月亮B_协程包装_函 (炉_状态机结 *L) {
  月核心库_月亮B_创建协程_函(L);
  月亮推入C闭包_函(L, 月核心库_月亮B_辅助包装_函, 1);
  return 1;
}


static int 月核心库_月亮B_产出_函 (炉_状态机结 *L) {
  return 月头_月亮_让步_宏名(L, 月亮_获取顶_函(L));
}


#define 内核库_COS_运行_宏名		0
#define 内核库_COS_死亡_宏名	1
#define 内核库_COS_让步_宏名	2
#define 内核库_COS_正常_宏名	3


static const char *const 状态名_变量[] =
  {"running", "dead", "suspended", "normal"};


static int 月核心库_辅助状态码_函 (炉_状态机结 *L, 炉_状态机结 *协程_短变量) {
  if (L == 协程_短变量) return 内核库_COS_运行_宏名;
  else {
    switch (月亮_状态码_函(协程_短变量)) {
      case 月头_月亮_让步_宏名:
        return 内核库_COS_让步_宏名;
      case LUA_OK: {
        炉_调试结 活记_缩变量;
        if (月亮_获取栈_函(协程_短变量, 0, &活记_缩变量))  /* does it have frames? */
          return 内核库_COS_正常_宏名;  /* it is running */
        else if (月亮_获取顶_函(协程_短变量) == 0)
            return 内核库_COS_死亡_宏名;
        else
          return 内核库_COS_让步_宏名;  /* initial 状态机_变量 */
      }
      default:  /* some 错误_小变量 occurred */
        return 内核库_COS_死亡_宏名;
    }
  }
}


static int 月核心库_月亮B_协程状态码_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月核心库_获取协程_函(L);
  月亮推入字符串_函(L, 状态名_变量[月核心库_辅助状态码_函(L, 协程_短变量)]);
  return 1;
}


static int 月核心库_月亮B_可产出_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月头_月亮_是否没有_宏名(L, 1) ? L : 月核心库_获取协程_函(L);
  月亮推入布尔值_函(L, 月亮_是否可让出_函(协程_短变量));
  return 1;
}


static int 月核心库_月亮B_协程正在跑_函 (炉_状态机结 *L) {
  int 是否主_变量 = 月亮推入线程_函(L);
  月亮推入布尔值_函(L, 是否主_变量);
  return 2;
}


static int 月核心库_月亮B_关闭_函 (炉_状态机结 *L) {
  炉_状态机结 *协程_短变量 = 月核心库_获取协程_函(L);
  int 状态码_变量 = 月核心库_辅助状态码_函(L, 协程_短变量);
  switch (状态码_变量) {
    case 内核库_COS_死亡_宏名: case 内核库_COS_让步_宏名: {
      状态码_变量 = 月亮_关闭线程_函(协程_短变量, L);
      if (状态码_变量 == LUA_OK) {
        月亮推入布尔值_函(L, 1);
        return 1;
      }
      else {
        月亮推入布尔值_函(L, 0);
        月亮_传递数据_函(协程_短变量, L, 1);  /* move 错误_小变量 message */
        return 2;
      }
    }
    default:  /* normal or running coroutine */
      return 月亮状态_错误_函(L, "月编译器_不能_函 关闭_圆 a %s coroutine", 状态名_变量[状态码_变量]);
  }
}


static const 炉L_寄结 协程_函_变量[] = {
  {"create", 月核心库_月亮B_创建协程_函},
  {"月做_恢复_函", 月核心库_月亮B_协程恢复_函},
  {"running", 月核心库_月亮B_协程正在跑_函},
  {"状态码_变量", 月核心库_月亮B_协程状态码_函},
  {"wrap", 月核心库_月亮B_协程包装_函},
  {"yield", 月核心库_月亮B_产出_函},
  {"isyieldable", 月核心库_月亮B_可产出_函},
  {"关闭_圆", 月核心库_月亮B_关闭_函},
  {NULL, NULL}
};



配置_月亮模块_应程接_宏名 int 月亮打开协程_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 协程_函_变量);
  return 1;
}

