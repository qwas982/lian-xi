/*
** $Id: lctype.c $
** 'ctype' functions for Lua
** See Copyright Notice in lua.h
*/

#define lctype_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include "lctype.h"

#if !c类型_月亮_用_c类型_宏名	/* { */

#include <limits.h>


#if defined (LUA_UCID)		/* accept UniCode IDentifiers? */
/* consider all non-ascii codepoints 到_变量 be alphabetic */
#define NONA		0x01
#else
#define NONA		0x00	/* default */
#endif


配置_月亮I_D定义_宏名 const 路_字节型 月亮i_c类型_变量[UCHAR_MAX + 2] = {
  0x00,  /* EOZ */
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	/* 0. */
  0x00,  0x08,  0x08,  0x08,  0x08,  0x08,  0x00,  0x00,
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,	/* 1. */
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,
  0x0c,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,	/* 2. */
  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,
  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,  0x16,	/* 3. */
  0x16,  0x16,  0x04,  0x04,  0x04,  0x04,  0x04,  0x04,
  0x04,  0x15,  0x15,  0x15,  0x15,  0x15,  0x15,  0x05,	/* 4. */
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,	/* 5. */
  0x05,  0x05,  0x05,  0x04,  0x04,  0x04,  0x04,  0x05,
  0x04,  0x15,  0x15,  0x15,  0x15,  0x15,  0x15,  0x05,	/* 6. */
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,
  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,  0x05,	/* 7. */
  0x05,  0x05,  0x05,  0x04,  0x04,  0x04,  0x04,  0x00,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* 8. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* 9. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* a. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* b. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  0x00,  0x00,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* c. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* d. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,	/* e. */
  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,  NONA,
  NONA,  NONA,  NONA,  NONA,  NONA,  0x00,  0x00,  0x00,	/* f. */
  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00
};

#endif			/* } */
