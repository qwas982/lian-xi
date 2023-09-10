/*
** $Id: lundump.h $
** load precompiled Lua chunks
** See Copyright Notice in lua.h
*/

#ifndef lundump_h
#define lundump_h

#include "llimits.h"
#include "lobject.h"
#include "lzio.h"


/* 数据_变量 到_变量 catch conversion errors */
#define 反转储_月亮C_数据_宏名	"\x19\x93\r\n\x1a\n"

#define 反转储_月亮C_整型_宏名	0x5678
#define 反转储_月亮C_数目_宏名	限制_类型转换_数目_宏名(370.5)

/*
** Encode major-minor 版本_变量 in one byte, one nibble for each
*/
#define 反转储_我的整型_宏名(s)	(s[0]-'0')  /* assume one-数字_变量 numerals */
#define 反转储_月亮C_版本_宏名	(反转储_我的整型_宏名(月头_月亮_版本_主版_宏名)*16+反转储_我的整型_宏名(月头_月亮_版本_副版_宏名))

#define 反转储_月亮C_格式_宏名	0	/* this is the official 格式_变量 */

/* load one chunk; from lundump.c */
配置_月亮I_函_宏名 L闭包_结* 月亮实用工具_解除转储_函 (炉_状态机结* L, 入出流_结* Z, const char* 名称_变量);

/* dump one chunk; from ldump.c */
配置_月亮I_函_宏名 int 月亮实用工具_转储_函 (炉_状态机结* L, const 原型_结* f, 炉_写器结 w,
                         void* 数据_变量, int 剥离_变量);

#endif
