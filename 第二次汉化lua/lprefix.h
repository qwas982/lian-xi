/*
** $Id: lprefix.h $
** Definitions for Lua 代码_变量 that must come before any other header file
** See Copyright Notice in lua.h
*/

#ifndef lprefix_h
#define lprefix_h


/*
** Allows POSIX/XSI stuff
*/
#if !defined(配置_月亮_用_C89_宏名)	/* { */

#if !defined(前缀__X打开_源码_宏名)
#define 前缀__X打开_源码_宏名           600
#elif 前缀__X打开_源码_宏名 == 0
#undef 前缀__X打开_源码_宏名  /* use -D_XOPEN_SOURCE=0 到_变量 undefine it */
#endif

/*
** Allows manipulation of large files in gcc and some other compilers
*/
#if !defined(配置_月亮_32位_宏名) && !defined(前缀__文件_偏移_位_宏名)
#define 前缀__大文件_源码_宏名       1
#define 前缀__文件_偏移_位_宏名       64
#endif

#endif				/* } */


/*
** Windows stuff
*/
#if defined(_WIN32)	/* { */

#if !defined(前缀__C运行时_安全_不_警告_宏名)
#define 前缀__C运行时_安全_不_警告_宏名  /* avoid warnings about ISO C functions */
#endif

#endif			/* } */

#endif

