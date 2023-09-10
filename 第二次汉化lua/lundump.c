/*
** $Id: lundump.c $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#define lundump_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <limits.h>
#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lundump.h"
#include "lzio.h"


#if !defined(反转储_月亮i_校验码_宏名)
#define 反转储_月亮i_校验码_宏名(L,f)  /* 空容器_变量 */
#endif


typedef struct {
  炉_状态机结 *L;
  入出流_结 *Z;
  const char *名称_变量;
} 载入状态机_结;


static 限制_l_无返回值_宏名 错误_小变量 (载入状态机_结 *S, const char *why) {
  月亮对象_推入格式化字符串_函(S->L, "%s: bad binary 格式_变量 (%s)", S->名称_变量, why);
  月亮调度_抛出_函(S->L, 月头_月亮_句法错误_宏名);
}


/*
** All high-层级_变量 loads go through 反转储_载入向量_宏名; you can 改变_变量 it 到_变量
** adapt 到_变量 the endianness of the input
*/
#define 反转储_载入向量_宏名(S,b,n)	月反转储_载入块_函(S,b,(n)*sizeof((b)[0]))

static void 月反转储_载入块_函 (载入状态机_结 *S, void *b, size_t 大小_变量) {
  if (月亮流_读取_函(S->Z, b, 大小_变量) != 0)
    错误_小变量(S, "truncated chunk");
}


#define 反转储_载入变量_宏名(S,x)		反转储_载入向量_宏名(S,&x,1)


static 路_字节型 月反转储_载入字节_函 (载入状态机_结 *S) {
  int b = 入出流_流获取字符_宏名(S->Z);
  if (b == EOZ)
    错误_小变量(S, "truncated chunk");
  return 限制_类型转换_字节_宏名(b);
}


static size_t 月反转储_载入无符号数_函 (载入状态机_结 *S, size_t 限制_变量) {
  size_t x = 0;
  int b;
  限制_变量 >>= 7;
  do {
    b = 月反转储_载入字节_函(S);
    if (x >= 限制_变量)
      错误_小变量(S, "integer overflow");
    x = (x << 7) | (b & 0x7f);
  } while ((b & 0x80) == 0);
  return x;
}


static size_t 月反转储_载入大小_函 (载入状态机_结 *S) {
  return 月反转储_载入无符号数_函(S, ~(size_t)0);
}


static int 月反转储_载入整型_函 (载入状态机_结 *S) {
  return 限制_类型转换_整型_宏名(月反转储_载入无符号数_函(S, INT_MAX));
}


static 炉_数目型 月反转储_载入数目_函 (载入状态机_结 *S) {
  炉_数目型 x;
  反转储_载入变量_宏名(S, x);
  return x;
}


static 炉_整数型 月反转储_载入整数_函 (载入状态机_结 *S) {
  炉_整数型 x;
  反转储_载入变量_宏名(S, x);
  return x;
}


/*
** Load a nullable string into prototype 'p'.
*/
static 标签字符串_结 *月反转储_载入字符串N_函 (载入状态机_结 *S, 原型_结 *p) {
  炉_状态机结 *L = S->L;
  标签字符串_结 *类s_变量;
  size_t 大小_变量 = 月反转储_载入大小_函(S);
  if (大小_变量 == 0)  /* no string? */
    return NULL;
  else if (--大小_变量 <= 限制_月亮I_最大短型长度_宏名) {  /* short string? */
    char 缓冲_变量[限制_月亮I_最大短型长度_宏名];
    反转储_载入向量_宏名(S, 缓冲_变量, 大小_变量);  /* load string into 缓冲区_变量 */
    类s_变量 = 月亮字符串_新长串_函(L, 缓冲_变量, 大小_变量);  /* create string */
  }
  else {  /* long string */
    类s_变量 = 月亮字符串_创建长型串对象_函(L, 大小_变量);  /* create string */
    对象_设置ts值到s_宏名(L, L->顶部_变量.p, 类s_变量);  /* 锚点_变量 it ('反转储_载入向量_宏名' can GC) */
    月亮调度_递增顶_函(L);
    反转储_载入向量_宏名(S, 对象_获取串_宏名(类s_变量), 大小_变量);  /* load directly in 最终_变量 place */
    L->顶部_变量.p--;  /* pop string */
  }
  垃圾回收_月亮C_对象屏障_宏名(L, p, 类s_变量);
  return 类s_变量;
}


/*
** Load a non-nullable string into prototype 'p'.
*/
static 标签字符串_结 *月反转储_载入字符串_函 (载入状态机_结 *S, 原型_结 *p) {
  标签字符串_结 *状_变量 = 月反转储_载入字符串N_函(S, p);
  if (状_变量 == NULL)
    错误_小变量(S, "bad 格式_变量 for constant string");
  return 状_变量;
}


static void 月反转储_载入代码_函 (载入状态机_结 *S, 原型_结 *f) {
  int n = 月反转储_载入整型_函(S);
  f->代码_变量 = 内存_月亮M_新向量已检查_宏名(S->L, n, Instruction);
  f->代码大小_小写 = n;
  反转储_载入向量_宏名(S, f->代码_变量, n);
}


static void 月反转储_载入函数_函(载入状态机_结 *S, 原型_结 *f, 标签字符串_结 *psource);


static void 月反转储_载入常量_函 (载入状态机_结 *S, 原型_结 *f) {
  int i;
  int n = 月反转储_载入整型_函(S);
  f->k = 内存_月亮M_新向量已检查_宏名(S->L, n, 标签值_结);
  f->k大小_缩 = n;
  for (i = 0; i < n; i++)
    对象_设置空值的值_宏名(&f->k[i]);
  for (i = 0; i < n; i++) {
    标签值_结 *o = &f->k[i];
    int t = 月反转储_载入字节_函(S);
    switch (t) {
      case 对象_月亮_V空值_宏名:
        对象_设置空值的值_宏名(o);
        break;
      case 对象_月亮_V假_宏名:
        对象_设置布尔假值_宏名(o);
        break;
      case 对象_月亮_V真_宏名:
        对象_设置布尔真值_宏名(o);
        break;
      case 对象_月亮_V数目浮点_宏名:
        对象_设置浮点值_宏名(o, 月反转储_载入数目_函(S));
        break;
      case 对象_月亮_V数目整型_宏名:
        对象_设置整数值_宏名(o, 月反转储_载入整数_函(S));
        break;
      case 对象_月亮_V短型串_宏名:
      case 对象_月亮_V长型串_宏名:
        对象_设置ts值到n_宏名(S->L, o, 月反转储_载入字符串_函(S, f));
        break;
      default: 限制_月亮_断言_宏名(0);
    }
  }
}


static void 月反转储_载入原型_函 (载入状态机_结 *S, 原型_结 *f) {
  int i;
  int n = 月反转储_载入整型_函(S);
  f->p = 内存_月亮M_新向量已检查_宏名(S->L, n, 原型_结 *);
  f->p大小_缩 = n;
  for (i = 0; i < n; i++)
    f->p[i] = NULL;
  for (i = 0; i < n; i++) {
    f->p[i] = 月亮函数_新原型_函(S->L);
    垃圾回收_月亮C_对象屏障_宏名(S->L, f, f->p[i]);
    月反转储_载入函数_函(S, f->p[i], f->源_圆);
  }
}


/*
** Load the 上值们_小写 for a function. The names must be filled 首先_变量,
** because the filling of the other fields can raise read errors and
** the creation of the 错误_小变量 message can call an emergency collection;
** in that case all prototypes must be consistent for the GC.
*/
static void 月反转储_载入上值_函 (载入状态机_结 *S, 原型_结 *f) {
  int i, n;
  n = 月反转储_载入整型_函(S);
  f->上值们_小写 = 内存_月亮M_新向量已检查_宏名(S->L, n, 上值描述_结);
  f->上值大小_小写 = n;
  for (i = 0; i < n; i++)  /* make 数组_圆 有效_变量 for GC */
    f->上值们_小写[i].名称_变量 = NULL;
  for (i = 0; i < n; i++) {  /* following calls can raise errors */
    f->上值们_小写[i].栈内_小写 = 月反转储_载入字节_函(S);
    f->上值们_小写[i].索引_缩变量 = 月反转储_载入字节_函(S);
    f->上值们_小写[i].种类_变量 = 月反转储_载入字节_函(S);
  }
}


static void 月反转储_载入调试_函 (载入状态机_结 *S, 原型_结 *f) {
  int i, n;
  n = 月反转储_载入整型_函(S);
  f->行信息_变量 = 内存_月亮M_新向量已检查_宏名(S->L, n, ls_byte);
  f->行信息大小_小写 = n;
  反转储_载入向量_宏名(S, f->行信息_变量, n);
  n = 月反转储_载入整型_函(S);
  f->绝对行信息_小写 = 内存_月亮M_新向量已检查_宏名(S->L, n, 绝对行信息_结);
  f->绝对行信息大小_小写 = n;
  for (i = 0; i < n; i++) {
    f->绝对行信息_小写[i].程序计数_变量 = 月反转储_载入整型_函(S);
    f->绝对行信息_小写[i].行_变量 = 月反转储_载入整型_函(S);
  }
  n = 月反转储_载入整型_函(S);
  f->本地变量们_短 = 内存_月亮M_新向量已检查_宏名(S->L, n, 本地变量_结);
  f->本地变量大小_短 = n;
  for (i = 0; i < n; i++)
    f->本地变量们_短[i].变量名称_变量 = NULL;
  for (i = 0; i < n; i++) {
    f->本地变量们_短[i].变量名称_变量 = 月反转储_载入字符串N_函(S, f);
    f->本地变量们_短[i].始程计_缩 = 月反转储_载入整型_函(S);
    f->本地变量们_短[i].终程计_缩 = 月反转储_载入整型_函(S);
  }
  n = 月反转储_载入整型_函(S);
  if (n != 0)  /* does it have debug information? */
    n = f->上值大小_小写;  /* must be this many */
  for (i = 0; i < n; i++)
    f->上值们_小写[i].名称_变量 = 月反转储_载入字符串N_函(S, f);
}


static void 月反转储_载入函数_函 (载入状态机_结 *S, 原型_结 *f, 标签字符串_结 *psource) {
  f->源_圆 = 月反转储_载入字符串N_函(S, f);
  if (f->源_圆 == NULL)  /* no 源_圆 in dump? */
    f->源_圆 = psource;  /* reuse parent's 源_圆 */
  f->已定义行_小写 = 月反转储_载入整型_函(S);
  f->最后已定义行_小写 = 月反转储_载入整型_函(S);
  f->形参数_小写 = 月反转储_载入字节_函(S);
  f->是否_变量实参短 = 月反转储_载入字节_函(S);
  f->最大栈大小_小写 = 月反转储_载入字节_函(S);
  月反转储_载入代码_函(S, f);
  月反转储_载入常量_函(S, f);
  月反转储_载入上值_函(S, f);
  月反转储_载入原型_函(S, f);
  月反转储_载入调试_函(S, f);
}


static void 月反转储_检查字面_函 (载入状态机_结 *S, const char *s, const char *消息_缩变量) {
  char 缓冲_变量[sizeof(月头_月亮_签名_宏名) + sizeof(反转储_月亮C_数据_宏名)]; /* larger than both */
  size_t 长度_短变量 = strlen(s);
  反转储_载入向量_宏名(S, 缓冲_变量, 长度_短变量);
  if (memcmp(s, 缓冲_变量, 长度_短变量) != 0)
    错误_小变量(S, 消息_缩变量);
}


static void 月反转储_强制检查大小_函 (载入状态机_结 *S, size_t 大小_变量, const char *tname) {
  if (月反转储_载入字节_函(S) != 大小_变量)
    错误_小变量(S, 月亮对象_推入格式化字符串_函(S->L, "%s 大小_变量 mismatch", tname));
}


#define 反转储_检查大小_宏名(S,t)	月反转储_强制检查大小_函(S,sizeof(t),#t)

static void 月反转储_检查头部_函 (载入状态机_结 *S) {
  /* skip 1st char (already read and checked) */
  月反转储_检查字面_函(S, &月头_月亮_签名_宏名[1], "not a binary chunk");
  if (月反转储_载入字节_函(S) != 反转储_月亮C_版本_宏名)
    错误_小变量(S, "版本_变量 mismatch");
  if (月反转储_载入字节_函(S) != 反转储_月亮C_格式_宏名)
    错误_小变量(S, "格式_变量 mismatch");
  月反转储_检查字面_函(S, 反转储_月亮C_数据_宏名, "corrupted chunk");
  反转储_检查大小_宏名(S, Instruction);
  反转储_检查大小_宏名(S, 炉_整数型);
  反转储_检查大小_宏名(S, 炉_数目型);
  if (月反转储_载入整数_函(S) != 反转储_月亮C_整型_宏名)
    错误_小变量(S, "integer 格式_变量 mismatch");
  if (月反转储_载入数目_函(S) != 反转储_月亮C_数目_宏名)
    错误_小变量(S, "float 格式_变量 mismatch");
}


/*
** Load precompiled chunk.
*/
L闭包_结 *月亮实用工具_解除转储_函(炉_状态机结 *L, 入出流_结 *Z, const char *名称_变量) {
  载入状态机_结 S;
  L闭包_结 *闭包_短变量;
  if (*名称_变量 == '@' || *名称_变量 == '=')
    S.名称_变量 = 名称_变量 + 1;
  else if (*名称_变量 == 月头_月亮_签名_宏名[0])
    S.名称_变量 = "binary string";
  else
    S.名称_变量 = 名称_变量;
  S.L = L;
  S.Z = Z;
  月反转储_检查头部_函(&S);
  闭包_短变量 = 月亮函数_新L闭包_函(L, 月反转储_载入字节_函(&S));
  对象_设置闭包L值到s_宏名(L, L->顶部_变量.p, 闭包_短变量);
  月亮调度_递增顶_函(L);
  闭包_短变量->p = 月亮函数_新原型_函(L);
  垃圾回收_月亮C_对象屏障_宏名(L, 闭包_短变量, 闭包_短变量->p);
  月反转储_载入函数_函(&S, 闭包_短变量->p, NULL);
  限制_月亮_断言_宏名(闭包_短变量->nupvalues == 闭包_短变量->p->上值大小_小写);
  反转储_月亮i_校验码_宏名(L, 闭包_短变量->p);
  return 闭包_短变量;
}

