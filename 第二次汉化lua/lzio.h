/*
** $Id: lzio.h $
** Buffered streams
** See Copyright Notice in lua.h
*/


#ifndef lzio_h
#define lzio_h

#include "lua.h"

#include "lmem.h"


#define EOZ	(-1)			/* 终_变量 of stream */

typedef struct 流入出_结 入出流_结;

#define 入出流_流获取字符_宏名(z)  (((z)->n--)>0 ?  限制_类型转换_无符印刻_宏名(*(z)->p++) : 月亮流_填充_函(z))


typedef struct M缓冲区_结 {
  char *缓冲区_变量;
  size_t n;
  size_t 缓冲大小_小写;
} M缓冲区_结;

#define 入出流_月亮Z_初始的缓冲区_宏名(L, 缓冲_变量) ((缓冲_变量)->缓冲区_变量 = NULL, (缓冲_变量)->缓冲大小_小写 = 0)

#define 入出流_月亮Z_缓冲区_宏名(缓冲_变量)	((缓冲_变量)->缓冲区_变量)
#define 入出流_月亮Z_缓冲区大小_宏名(缓冲_变量)	((缓冲_变量)->缓冲大小_小写)
#define 入出流_月亮Z_缓冲长度_宏名(缓冲_变量)	((缓冲_变量)->n)

#define 入出流_月亮Z_缓冲移除_宏名(缓冲_变量,i)	((缓冲_变量)->n -= (i))
#define 入出流_月亮Z_重置缓冲区_宏名(缓冲_变量) ((缓冲_变量)->n = 0)


#define 入出流_月亮Z_调整缓冲区大小_宏名(L, 缓冲_变量, 大小_变量) \
	((缓冲_变量)->缓冲区_变量 = 内存_月亮M_重新分配v印刻_宏名(L, (缓冲_变量)->缓冲区_变量, \
				(缓冲_变量)->缓冲大小_小写, 大小_变量), \
	(缓冲_变量)->缓冲大小_小写 = 大小_变量)

#define 入出流_月亮Z_释放缓冲区_宏名(L, 缓冲_变量)	入出流_月亮Z_调整缓冲区大小_宏名(L, 缓冲_变量, 0)


配置_月亮I_函_宏名 void 月亮流_初始的_函 (炉_状态机结 *L, 入出流_结 *z, 炉_读器型 读器_圆,
                                        void *数据_变量);
配置_月亮I_函_宏名 size_t 月亮流_读取_函 (入出流_结* z, void *b, size_t n);	/* read 下一个_变量 n bytes */



/* --------- Private Part ------------------ */

struct 流入出_结 {
  size_t n;			/* bytes still unread */
  const char *p;		/* 当前_圆 position in 缓冲区_变量 */
  炉_读器型 读器_圆;		/* 读器_圆 function */
  void *数据_变量;			/* additional 数据_变量 */
  炉_状态机结 *L;			/* Lua 状态机_变量 (for 读器_圆) */
};


配置_月亮I_函_宏名 int 月亮流_填充_函 (入出流_结 *z);

#endif
