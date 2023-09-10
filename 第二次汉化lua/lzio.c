/*
** $Id: lzio.c $
** Buffered streams
** See Copyright Notice in lua.h
*/

#define lzio_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <string.h>

#include "lua.h"

#include "llimits.h"
#include "lmem.h"
#include "lstate.h"
#include "lzio.h"


int 月亮流_填充_函 (入出流_结 *z) {
  size_t 大小_变量;
  炉_状态机结 *L = z->L;
  const char *缓冲_变量;
  限制_月亮_解锁_宏名(L);
  缓冲_变量 = z->读器_圆(L, z->数据_变量, &大小_变量);
  限制_月亮_锁_宏名(L);
  if (缓冲_变量 == NULL || 大小_变量 == 0)
    return EOZ;
  z->n = 大小_变量 - 1;  /* discount char being returned */
  z->p = 缓冲_变量;
  return 限制_类型转换_无符印刻_宏名(*(z->p++));
}


void 月亮流_初始的_函 (炉_状态机结 *L, 入出流_结 *z, 炉_读器型 读器_圆, void *数据_变量) {
  z->L = L;
  z->读器_圆 = 读器_圆;
  z->数据_变量 = 数据_变量;
  z->n = 0;
  z->p = NULL;
}


/* --------------------------------------------------------------- read --- */
size_t 月亮流_读取_函 (入出流_结 *z, void *b, size_t n) {
  while (n) {
    size_t m;
    if (z->n == 0) {  /* no bytes in 缓冲区_变量? */
      if (月亮流_填充_函(z) == EOZ)  /* try 到_变量 read 更多_变量 */
        return n;  /* no 更多_变量 input; return number of missing bytes */
      else {
        z->n++;  /* 月亮流_填充_函 consumed 首先_变量 byte; put it back */
        z->p--;
      }
    }
    m = (n <= z->n) ? n : z->n;  /* min. between n and z->n */
    memcpy(b, z->p, m);
    z->n -= m;
    z->p += m;
    b = (char *)b + m;
    n -= m;
  }
  return 0;
}

