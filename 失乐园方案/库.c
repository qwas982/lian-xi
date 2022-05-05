/*
 * The 失乐园方案 内部库
 
 */

#include <assert.h>

#include "失乐园.h"
#include "入出.h"
#include "状态.h"

/*
 * search for a dynamic binding with the given 标签
 * on the 栈, and pushes the associanted 值 or
 * a default one if the binding was not found
 * the 标签 and a default 值 must have been pushed
 * on the 栈
 */
static int 动态_查找(失乐园_状态_类型* 大失)
{
  int 索引短;

  for(索引短 = 大失->栈指针 - 3; 索引短 >= 0; 索引短--) {
    if(栈短(索引短).类型 == 失乐园_类型_动态_绑定 &&
       失乐园_对象_等号(失乐园_动态_绑定(垃圾回收栈(索引短))->标签, 栈短(大失->栈指针-2))) {
      栈短(大失->栈指针++) = 失乐园_动态_绑定(垃圾回收栈(索引短))->值;
      break;
    }
  }

  return 1;
}

/*
 * search for a dynamic binding with the given 标签
 * on the 栈, sets the associated 值
 * of the binding with the given argument, and
 * returns true if successful, false otherwise
 * the 标签 and the 值 must have been pushed
 * on the 栈
 */
static int 动态_储存(失乐园_状态_类型* 大失)
{
  int 索引短;

  for(索引短 = 大失->栈指针 - 3; 索引短 >= 0; 索引短--) {
    if(栈短(索引短).类型 == 失乐园_类型_动态_绑定 &&
       失乐园_对象_等号(失乐园_动态_绑定(垃圾回收栈(索引短))->标签, 栈短(大失->栈指针-2))) {
      失乐园_动态_绑定(垃圾回收栈(索引短))->值 = 栈短(大失->栈指针-1);
      break;
    }
  }

  栈短(大失->栈指针).类型 = 失乐园_类型_布尔;
  if(索引短 < 0) {
    栈短(大失->栈指针++).值.bool = 0;
  } else {
    栈短(大失->栈指针++).值.bool = 1;
  }

  return 1;
}

/*
 * creates an input 端口 that is the
 * system's standard input
 */
static int 打开_标准入_端口(失乐园_状态_类型* 大失)
{
  垃圾回收栈(大失->栈指针) = 失乐园_入出_创建_标准入(大失);
  栈短(大失->栈指针++).类型 = 失乐园_类型_输入_端口;

  return 1;
}

/*
 * creates an input 端口 that is the
 * system's standard output
 */
static int 打开_标准出_端口(失乐园_状态_类型* 大失)
{
  垃圾回收栈(大失->栈指针) = 失乐园_入出_创建_标准出(大失);
  栈短(大失->栈指针++).类型 = 失乐园_类型_输出_端口;

  return 1;
}

static int 打开_标准错_端口(失乐园_状态_类型* 大失)
{
  垃圾回收栈(大失->栈指针) = 失乐园_入出_创建_标准错(大失);
  栈短(大失->栈指针++).类型 = 失乐园_类型_输出_端口;

  return 1;
}

/*
 * parses input into tokens to be used
 * by the 'read' procedure'
 */
static int 读_牌(失乐园_状态_类型* 大失)
{
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 1) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  assert(栈短(大失->栈指针-1).类型 == 失乐园_类型_输入_端口);

  栈短(大失->栈指针++).类型 = 失乐园_类型_零;
  失乐园_入出_读_牌(大失, &栈短(大失->栈指针-2), &栈短(大失->栈指针-1));

  return 1;
}

/* creates syntactic closures */
static int 制作_句法_闭包(失乐园_状态_类型* 大失)
{
  失乐园_对象_类型 句法闭包;
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 3) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  句法闭包.类型 = 失乐园_类型_句法闭包;
  句法闭包.值.小垃圾回收 = 失乐园_创建_句法_闭包(大失);

  失乐园_句法闭包(句法闭包.值.小垃圾回收)->环境短  = 栈短(大失->栈指针-3);
  失乐园_句法闭包(句法闭包.值.小垃圾回收)->自由_ = 栈短(大失->栈指针-2);
  失乐园_句法闭包(句法闭包.值.小垃圾回收)->表达式短  = 栈短(大失->栈指针-1);

  栈短(大失->栈指针++) = 句法闭包;
  return 1;
}


static int 句法_闭包_问(失乐园_状态_类型* 大失)
{
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 1) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  栈短(大失->栈指针).类型 = 失乐园_类型_布尔;
  if(栈短(大失->栈指针-1).类型 == 失乐园_类型_句法闭包) {
    栈短(大失->栈指针++).值.bool = 1;
  } else {
    栈短(大失->栈指针++).值.bool = 0;
  }

  return 1;
}

static int 句法_闭包_环境(失乐园_状态_类型* 大失)
{
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 1) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  if(栈短(大失->栈指针-1).类型 != 失乐园_类型_句法闭包) {
    失乐园_推_串(大失, "argument must be a syntactic closure");
    失乐园_错误(大失, 1);
  }

  栈短(大失->栈指针++) = 失乐园_句法闭包(栈短(大失->栈指针-1).值.小垃圾回收)->环境短;
  return 1;
}

static int 句法_闭包_自由(失乐园_状态_类型* 大失)
{
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 1) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  if(栈短(大失->栈指针-1).类型 != 失乐园_类型_句法闭包) {
    失乐园_推_串(大失, "argument must be a syntactic closure");
    失乐园_错误(大失, 1);
  }

  栈短(大失->栈指针++) = 失乐园_句法闭包(栈短(大失->栈指针-1).值.小垃圾回收)->自由_;
  return 1;
}

static int 句法_闭包_表达式(失乐园_状态_类型* 大失)
{
  int 实参数号 = 失乐园_取_顶部(大失);

  if(实参数号 != 1) {
    失乐园_推_串(大失, "wrong number of arguments");
    失乐园_错误(大失, 1);
  }

  if(栈短(大失->栈指针-1).类型 != 失乐园_类型_句法闭包) {
    失乐园_推_串(大失, "argument must be a syntactic closure");
    失乐园_错误(大失, 1);
  }

  栈短(大失->栈指针++) = 失乐园_句法闭包(栈短(大失->栈指针-1).值.小垃圾回收)->表达式短;
  return 1;
}

static 失乐园_寄存_类型 库_寄存[] = {
  {"##动态-查找", 动态_查找},
  {"##动态-储存", 动态_储存},
  {"##打开-标准-输入", 打开_标准入_端口},
  {"##打开-标准-输出", 打开_标准出_端口},
  {"##打开-标准-错误", 打开_标准错_端口},
  {"##读-牌", 读_牌},
  {"制作-句法-闭包", 制作_句法_闭包},
  {"句法-闭包?", 句法_闭包_问},
  {"句法-闭包-环境", 句法_闭包_环境},
  {"句法-闭包-自由", 句法_闭包_自由},
  {"句法-闭包-表达式", 句法_闭包_表达式},
  {NULL, NULL}
};

int 失乐园_打开_库(失乐园_状态_类型* 大失)
{
  失乐园_寄存器(大失, 库_寄存);

  return 0;
}
