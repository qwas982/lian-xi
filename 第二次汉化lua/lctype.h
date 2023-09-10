/*
** $Id: lctype.h $
** 'ctype' functions for Lua
** See Copyright Notice in lua.h
*/

#ifndef lctype_h
#define lctype_h

#include "lua.h"


/*
** WARNING: the functions defined here do not necessarily correspond
** 到_变量 the similar functions in the standard C ctype.h. They are
** optimized for the specific needs of Lua.
*/

#if !defined(c类型_月亮_用_c类型_宏名)

#if 'A' == 65 && '0' == 48
/* ASCII case: can use its own tables; faster and fixed */
#define c类型_月亮_用_c类型_宏名	0
#else
/* must use standard C ctype */
#define c类型_月亮_用_c类型_宏名	1
#endif

#endif


#if !c类型_月亮_用_c类型_宏名	/* { */

#include <limits.h>

#include "llimits.h"


#define c类型_字母位_宏名	0
#define c类型_数字位_宏名	1
#define c类型_打印位_宏名	2
#define c类型_空格位_宏名	3
#define c类型_十六进制数字位_宏名	4


#define c类型_掩码_宏名(B)		(1 << (B))


/*
** add 1 到_变量 char 到_变量 allow index -1 (EOZ)
*/
#define c类型_测试属性_宏名(c,p)	(月亮i_c类型_变量[(c)+1] & (p))

/*
** 'lalpha' (Lua alphabetic) and 'lalnum' (Lua alphanumeric) both include '_'
*/
#define c类型_l是否l字母_宏名(c)	c类型_测试属性_宏名(c, c类型_掩码_宏名(c类型_字母位_宏名))
#define c类型_l是否l字母或数字_宏名(c)	c类型_测试属性_宏名(c, (c类型_掩码_宏名(c类型_字母位_宏名) | c类型_掩码_宏名(c类型_数字位_宏名)))
#define c类型_l是否数字_宏名(c)	c类型_测试属性_宏名(c, c类型_掩码_宏名(c类型_数字位_宏名))
#define c类型_l是否空格_宏名(c)	c类型_测试属性_宏名(c, c类型_掩码_宏名(c类型_空格位_宏名))
#define c类型_l是否打印_宏名(c)	c类型_测试属性_宏名(c, c类型_掩码_宏名(c类型_打印位_宏名))
#define c类型_l是否十六进制数字_宏名(c)	c类型_测试属性_宏名(c, c类型_掩码_宏名(c类型_十六进制数字位_宏名))


/*
** In ASCII, this 'c类型_l到小写_宏名' is correct for alphabetic characters and
** for '.'. That is enough for Lua needs. ('限制_检查_表达式_宏名' ensures that
** the character either is an upper-case letter or is unchanged by
** the transformation, which holds for lower-case letters and '.'.)
*/
#define c类型_l到小写_宏名(c)  \
  限制_检查_表达式_宏名(('A' <= (c) && (c) <= 'Z') || (c) == ((c) | ('A' ^ 'a')),  \
            (c) | ('A' ^ 'a'))


/* one entry for each character and for -1 (EOZ) */
配置_月亮I_D声明_宏名(const 路_字节型 月亮i_c类型_变量[UCHAR_MAX + 2];)


#else			/* }{ */

/*
** use standard C ctypes
*/

#include <ctype.h>


#define c类型_l是否l字母_宏名(c)	(isalpha(c) || (c) == '_')
#define c类型_l是否l字母或数字_宏名(c)	(isalnum(c) || (c) == '_')
#define c类型_l是否数字_宏名(c)	(isdigit(c))
#define c类型_l是否空格_宏名(c)	(isspace(c))
#define c类型_l是否打印_宏名(c)	(isprint(c))
#define c类型_l是否十六进制数字_宏名(c)	(isxdigit(c))

#define c类型_l到小写_宏名(c)	(tolower(c))

#endif			/* } */

#endif

