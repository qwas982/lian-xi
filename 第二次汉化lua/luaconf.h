/*
** $Id: luaconf.h $
** Configuration file for Lua
** See Copyright Notice in lua.h
*/


#ifndef luaconf_h
#define luaconf_h

#include <limits.h>
#include <stddef.h>


/*
** ===================================================================
** General Configuration File for Lua
**
** Some definitions here can be 已更改_变量 externally, through the compiler
** (e.g., with '-D' 选项们_变量): They are commented out or protected
** by '#if !defined' guards. However, several other definitions
** should be 已更改_变量 directly here, either because they affect the
** Lua ABI (by making the changes here, you ensure that all software
** connected 到_变量 Lua, such as C libraries, will be compiled with the same
** configuration); or because they are seldom 已更改_变量.
**
** Search for "@@" 到_变量 find all configurable definitions.
** ===================================================================
*/


/*
** {====================================================================
** System Configuration: macros 到_变量 adapt (if 已需要_变量) Lua 到_变量 some
** particular platform, for instance restricting it 到_变量 C89.
** =====================================================================
*/

/*
@@ 配置_月亮_用_C89_宏名 controls the use of non-ISO-C89 features.
** Define it if you want Lua 到_变量 avoid the use of a few C99 features
** or Windows-specific features on Windows.
*/
/* #define 配置_月亮_用_C89_宏名 */


/*
** By default, Lua on Windows use (some) specific Windows features
*/
#if !defined(配置_月亮_用_C89_宏名) && defined(_WIN32) && !defined(_WIN32_WCE)
#define 配置_月亮_用_WINDOWS_宏名  /* enable goodies for regular Windows */
#endif


#if defined(配置_月亮_用_WINDOWS_宏名)
#define 配置_月亮_动链_动态链接库_宏名	/* enable support for DLL */
#define 配置_月亮_用_C89_宏名	/* broadly, Windows is C89 */
#endif


#if defined(LUA_USE_LINUX)
#define 配置_月亮_用_POSIX_宏名
#define 配置_月亮_用_动链打开_宏名		/* needs an 额外_变量 library: -ldl */
#endif


#if defined(LUA_USE_MACOSX)
#define 配置_月亮_用_POSIX_宏名
#define 配置_月亮_用_动链打开_宏名		/* MacOS does not need -ldl */
#endif


#if defined(LUA_USE_IOS)
#define 配置_月亮_用_POSIX_宏名
#define 配置_月亮_用_动链打开_宏名
#endif


/*
@@ 配置_月亮I_是否32位整型_宏名 is true iff 'int' has (at least) 32 bits.
*/
#define 配置_月亮I_是否32位整型_宏名	((UINT_MAX >> 30) >= 3)

/* }================================================================== */



/*
** {==================================================================
** Configuration for Number types. These 选项们_变量 should not be
** set externally, because any other 代码_变量 connected 到_变量 Lua must
** use the same configuration.
** ===================================================================
*/

/*
@@ 配置_月亮_整型_类型_宏名 defines the type for Lua integers.
@@ 配置_月亮_浮点_类型_宏名 defines the type for Lua floats.
** Lua should 工作_变量 fine with any mix of these 选项们_变量 supported
** by your C compiler. The usual configurations are 64-bit integers
** and 'double' (the default), 32-bit integers and 'float' (for
** restricted platforms), and 'long'/'double' (for C compilers not
** compliant with C99, which may not have support for 'long long').
*/

/* predefined 选项们_变量 for 配置_月亮_整型_类型_宏名 */
#define 配置_月亮_整型_整型_宏名		1
#define 配置_月亮_整型_长型_宏名		2
#define 配置_月亮_整型_长型长型_宏名	3

/* predefined 选项们_变量 for 配置_月亮_浮点_类型_宏名 */
#define 配置_月亮_浮点_浮点_宏名		1
#define 配置_月亮_浮点_双浮_宏名	2
#define 配置_月亮_浮点_长型双浮_宏名	3


/* Default configuration ('long long' and 'double', for 64-bit Lua) */
#define 配置_月亮_整型_默认_宏名		配置_月亮_整型_长型长型_宏名
#define 配置_月亮_浮点_默认_宏名	配置_月亮_浮点_双浮_宏名


/*
@@ 配置_月亮_32位_宏名 enables Lua with 32-bit integers and 32-bit floats.
*/
#define 配置_月亮_32位_宏名	0


/*
@@ 配置_月亮_C89_数目_宏名 ensures that Lua uses the largest types available for
** C89 ('long' and 'double'); Windows always has '__int64', so it does
** not need 到_变量 use this case.
*/
#if defined(配置_月亮_用_C89_宏名) && !defined(配置_月亮_用_WINDOWS_宏名)
#define 配置_月亮_C89_数目_宏名		1
#else
#define 配置_月亮_C89_数目_宏名		0
#endif


#if 配置_月亮_32位_宏名		/* { */
/*
** 32-bit integers and 'float'
*/
#if 配置_月亮I_是否32位整型_宏名  /* use 'int' if big enough */
#define 配置_月亮_整型_类型_宏名	配置_月亮_整型_整型_宏名
#else  /* otherwise use 'long' */
#define 配置_月亮_整型_类型_宏名	配置_月亮_整型_长型_宏名
#endif
#define 配置_月亮_浮点_类型_宏名	配置_月亮_浮点_浮点_宏名

#elif 配置_月亮_C89_数目_宏名	/* }{ */
/*
** largest types available for C89 ('long' and 'double')
*/
#define 配置_月亮_整型_类型_宏名	配置_月亮_整型_长型_宏名
#define 配置_月亮_浮点_类型_宏名	配置_月亮_浮点_双浮_宏名

#else		/* }{ */
/* use defaults */

#define 配置_月亮_整型_类型_宏名	配置_月亮_整型_默认_宏名
#define 配置_月亮_浮点_类型_宏名	配置_月亮_浮点_默认_宏名

#endif				/* } */


/* }================================================================== */



/*
** {==================================================================
** Configuration for Paths.
** ===================================================================
*/

/*
** 配置_月亮_路径_分隔符_宏名 is the character that separates templates in a 路径_变量.
** 配置_月亮_路径_记号_宏名 is the string that marks the substitution points in a
** template.
** 配置_月亮_执行_目录_宏名 in a Windows 路径_变量 is replaced by the executable's
** directory.
*/
#define 配置_月亮_路径_分隔符_宏名            ";"
#define 配置_月亮_路径_记号_宏名           "?"
#define 配置_月亮_执行_目录_宏名            "!"


/*
@@ 配置_月亮_路径_默认_宏名 is the default 路径_变量 that Lua uses 到_变量 look for
** Lua libraries.
@@ 配置_月亮_C路径_默认_宏名 is the default 路径_变量 that Lua uses 到_变量 look for
** C libraries.
** CHANGE them if your machine has a non-conventional directory
** hierarchy or if you want 到_变量 install your libraries in
** non-conventional directories.
*/

#define 配置_月亮_V目录_宏名	月头_月亮_版本_主版_宏名 "." 月头_月亮_版本_副版_宏名
#if defined(_WIN32)	/* { */
/*
** In Windows, any exclamation 记号_变量 ('!') in the 路径_变量 is replaced by the
** 路径_变量 of the directory of the executable file of the 当前_圆 process.
*/
#define 配置_月亮_L目录_宏名	"!\\lua\\"
#define 配置_月亮_C目录_宏名	"!\\"
#define 配置_月亮_共享目录_宏名	"!\\..\\share\\lua\\" 配置_月亮_V目录_宏名 "\\"

#if !defined(配置_月亮_路径_默认_宏名)
#define 配置_月亮_路径_默认_宏名  \
		配置_月亮_L目录_宏名"?.lua;"  配置_月亮_L目录_宏名"?\\初始的_变量.lua;" \
		配置_月亮_C目录_宏名"?.lua;"  配置_月亮_C目录_宏名"?\\初始的_变量.lua;" \
		配置_月亮_共享目录_宏名"?.lua;" 配置_月亮_共享目录_宏名"?\\初始的_变量.lua;" \
		".\\?.lua;" ".\\?\\初始的_变量.lua"
#endif

#if !defined(配置_月亮_C路径_默认_宏名)
#define 配置_月亮_C路径_默认_宏名 \
		配置_月亮_C目录_宏名"?.dll;" \
		配置_月亮_C目录_宏名"..\\库_短变量\\lua\\" 配置_月亮_V目录_宏名 "\\?.dll;" \
		配置_月亮_C目录_宏名"loadall.dll;" ".\\?.dll"
#endif

#else			/* }{ */

#define 配置_月亮_根_宏名	"/usr/local/"
#define 配置_月亮_L目录_宏名	配置_月亮_根_宏名 "share/lua/" 配置_月亮_V目录_宏名 "/"
#define 配置_月亮_C目录_宏名	配置_月亮_根_宏名 "库_短变量/lua/" 配置_月亮_V目录_宏名 "/"

#if !defined(配置_月亮_路径_默认_宏名)
#define 配置_月亮_路径_默认_宏名  \
		配置_月亮_L目录_宏名"?.lua;"  配置_月亮_L目录_宏名"?/初始的_变量.lua;" \
		配置_月亮_C目录_宏名"?.lua;"  配置_月亮_C目录_宏名"?/初始的_变量.lua;" \
		"./?.lua;" "./?/初始的_变量.lua"
#endif

#if !defined(配置_月亮_C路径_默认_宏名)
#define 配置_月亮_C路径_默认_宏名 \
		配置_月亮_C目录_宏名"?.so;" 配置_月亮_C目录_宏名"loadall.so;" "./?.so"
#endif

#endif			/* } */


/*
@@ 配置_月亮_目录分隔符_宏名 is the directory separator (for submodules).
** CHANGE it if your machine does not use "/" as the directory separator
** and is not Windows. (On Windows Lua automatically uses "\".)
*/
#if !defined(配置_月亮_目录分隔符_宏名)

#if defined(_WIN32)
#define 配置_月亮_目录分隔符_宏名	"\\"
#else
#define 配置_月亮_目录分隔符_宏名	"/"
#endif

#endif

/* }================================================================== */


/*
** {==================================================================
** Marks for exported symbols in the C 代码_变量
** ===================================================================
*/

/*
@@ 配置_月亮_应程接_宏名 is a 记号_变量 for all core API functions.
@@ 配置_月亮库_应程接_宏名 is a 记号_变量 for all auxiliary library functions.
@@ 配置_月亮模块_应程接_宏名 is a 记号_变量 for all standard library opening functions.
** CHANGE them if you need 到_变量 define those functions in some special way.
** For instance, if you want 到_变量 create one Windows DLL with the core and
** the libraries, you may want 到_变量 use the following definition (define
** LUA_BUILD_AS_DLL 到_变量 get it).
*/
#if defined(LUA_BUILD_AS_DLL)	/* { */

#if defined(应程接_月亮_内核_宏名) || defined(辅助库_月亮_库_宏名)	/* { */
#define 配置_月亮_应程接_宏名 __declspec(dllexport)
#else						/* }{ */
#define 配置_月亮_应程接_宏名 __declspec(dllimport)
#endif						/* } */

#else				/* }{ */

#define 配置_月亮_应程接_宏名		extern

#endif				/* } */


/*
** More often than not the libs go together with the core.
*/
#define 配置_月亮库_应程接_宏名	配置_月亮_应程接_宏名
#define 配置_月亮模块_应程接_宏名	配置_月亮_应程接_宏名


/*
@@ 配置_月亮I_函_宏名 is a 记号_变量 for all extern functions that are not 到_变量 be
** exported 到_变量 outside modules.
@@ 配置_月亮I_D定义_宏名 and 配置_月亮I_D声明_宏名 are marks for all extern (const) variables,
** none of which 到_变量 be exported 到_变量 outside modules (配置_月亮I_D定义_宏名 for
** definitions and 配置_月亮I_D声明_宏名 for declarations).
** CHANGE them if you need 到_变量 记号_变量 them in some special way. Elf/gcc
** (versions 3.2 and later) 记号_变量 them as "hidden" 到_变量 optimize access
** when Lua is compiled as a shared library. Not all elf targets support
** this attribute. Unfortunately, gcc does not offer a way 到_变量 月解析器_检查_函
** whether the 目标_变量 offers that support, and those without support
** give a warning about it. To avoid these warnings, 改变_变量 到_变量 the
** default definition.
*/
#if defined(__GNUC__) && ((__GNUC__*100 + __GNUC_MINOR__) >= 302) && \
    defined(__ELF__)		/* { */
#define 配置_月亮I_函_宏名	__attribute__((visibility("internal"))) extern
#else				/* }{ */
#define 配置_月亮I_函_宏名	extern
#endif				/* } */

#define 配置_月亮I_D声明_宏名(dec)	配置_月亮I_函_宏名 dec
#define 配置_月亮I_D定义_宏名	/* 空容器_变量 */

/* }================================================================== */


/*
** {==================================================================
** Compatibility with 前一个_变量 versions
** ===================================================================
*/

/*
@@ LUA_COMPAT_5_3 controls other macros for compatibility with Lua 5.3.
** You can define it 到_变量 get all 选项们_变量, or 改变_变量 specific 选项们_变量
** 到_变量 fit your specific needs.
*/
#if defined(LUA_COMPAT_5_3)	/* { */

/*
@@ 配置_月亮_兼容_数学库_宏名 controls the presence of several deprecated
** functions in the mathematical library.
** (These functions were already officially removed in 5.3;
** nevertheless they are still available here.)
*/
#define 配置_月亮_兼容_数学库_宏名

/*
@@ 配置_月亮_兼容_应程接整型转换_宏名 controls the presence of macros for
** manipulating other integer types (月头_月亮_推无符_宏名, 月头_月亮_到无符_宏名,
** 辅助库_月亮l_检查整型_宏名, 辅助库_月亮l_检查长型_宏名, etc.)
** (These macros were also officially removed in 5.3, but they are still
** available here.)
*/
#define 配置_月亮_兼容_应程接整型转换_宏名


/*
@@ 配置_月亮_兼容_小于_小等_宏名 controls the emulation of the '__le' metamethod
** using '__lt'.
*/
#define 配置_月亮_兼容_小于_小等_宏名


/*
@@ The following macros supply trivial compatibility for some
** changes in the API. The macros themselves document how 到_变量
** 改变_变量 your 代码_变量 到_变量 avoid using them.
** (Once 更多_变量, these macros were officially removed in 5.3, but they are
** still available here.)
*/
#define 配置_月亮_串长度_宏名(L,i)		月亮_原始长度_函(L, (i))

#define 配置_月亮_对象长度_宏名(L,i)		月亮_原始长度_函(L, (i))

#define 配置_月亮_相等_宏名(L,idx1,idx2)		月亮对比_函(L,(idx1),(idx2),月头_月亮_操作等于_宏名)
#define 配置_月亮_小于_宏名(L,idx1,idx2)	月亮对比_函(L,(idx1),(idx2),月头_月亮_操作小于_宏名)

#endif				/* } */

/* }================================================================== */



/*
** {==================================================================
** Configuration for Numbers (低_变量-层级_变量 part).
** Change these definitions if no predefined LUA_FLOAT_* / LUA_INT_*
** satisfy your needs.
** ===================================================================
*/

/*
@@ 配置_月亮I_UAC数目_宏名 is the 结果_变量 of a 'default argument promotion'
@@ over a floating number.
@@ 配置_l_浮点属性_宏名(x) corrects float attribute 'x' 到_变量 the proper float type
** by prefixing it with one of FLT/DBL/LDBL.
@@ 配置_月亮_数目_形式长度_宏名 is the length modifier for writing floats.
@@ 配置_月亮_数目_格式_宏名 is the 格式_变量 for writing floats.
@@ 配置_月亮_数目到串_宏名 converts a float 到_变量 a string.
@@ 配置_数学操作_宏名 allows the addition of an 'l' or 'f' 到_变量 all math operations.
@@ 配置_l_向下取整_宏名 takes the floor of a float.
@@ 配置_月亮_串到数目_宏名 converts a decimal numeral 到_变量 a number.
*/


/* The following definitions are good for most cases here */

#define 配置_l_向下取整_宏名(x)		(配置_数学操作_宏名(floor)(x))

#define 配置_月亮_数目到串_宏名(s,大小_短变量,n)  \
	配置_l_s打印f_宏名((s), 大小_短变量, 配置_月亮_数目_格式_宏名, (配置_月亮I_UAC数目_宏名)(n))

/*
@@ 配置_月亮_数目到整数_宏名 converts a float number with an integral 值_圆
** 到_变量 an integer, or returns 0 if float is not within the range of
** a 炉_整数型.  (The range comparisons are tricky because of
** rounding. The tests here assume a two-complement representation,
** 哪儿_变量 MININTEGER always has an exact representation as a float;
** MAXINTEGER may not have one, and therefore its conversion 到_变量 float
** may have an ill-defined 值_圆.)
*/
#define 配置_月亮_数目到整数_宏名(n,p) \
  ((n) >= (配置_月亮_数目_宏名)(配置_月亮_最小整数_宏名) && \
   (n) < -(配置_月亮_数目_宏名)(配置_月亮_最小整数_宏名) && \
      (*(p) = (配置_月亮_整数_宏名)(n), 1))


/* now the variable definitions */

#if 配置_月亮_浮点_类型_宏名 == 配置_月亮_浮点_浮点_宏名		/* { single float */

#define 配置_月亮_数目_宏名	float

#define 配置_l_浮点属性_宏名(n)		(FLT_##n)

#define 配置_月亮I_UAC数目_宏名	double

#define 配置_月亮_数目_形式长度_宏名	""
#define 配置_月亮_数目_格式_宏名		"%.7g"

#define 配置_数学操作_宏名(操作_短变量)		操作_短变量##f

#define 配置_月亮_串到数目_宏名(s,p)	strtof((s), (p))


#elif 配置_月亮_浮点_类型_宏名 == 配置_月亮_浮点_长型双浮_宏名	/* }{ long double */

#define 配置_月亮_数目_宏名	long double

#define 配置_l_浮点属性_宏名(n)		(LDBL_##n)

#define 配置_月亮I_UAC数目_宏名	long double

#define 配置_月亮_数目_形式长度_宏名	"L"
#define 配置_月亮_数目_格式_宏名		"%.19Lg"

#define 配置_数学操作_宏名(操作_短变量)		操作_短变量##l

#define 配置_月亮_串到数目_宏名(s,p)	strtold((s), (p))

#elif 配置_月亮_浮点_类型_宏名 == 配置_月亮_浮点_双浮_宏名	/* }{ double */

#define 配置_月亮_数目_宏名	double

#define 配置_l_浮点属性_宏名(n)		(DBL_##n)

#define 配置_月亮I_UAC数目_宏名	double

#define 配置_月亮_数目_形式长度_宏名	""
#define 配置_月亮_数目_格式_宏名		"%.14g"

#define 配置_数学操作_宏名(操作_短变量)		操作_短变量

#define 配置_月亮_串到数目_宏名(s,p)	strtod((s), (p))

#else						/* }{ */

#错误_小变量 "numeric float type not defined"

#endif					/* } */



/*
@@ 配置_月亮_无符号_宏名 is the unsigned 版本_变量 of 配置_月亮_整数_宏名.
@@ 配置_月亮I_UAC整型_宏名 is the 结果_变量 of a 'default argument promotion'
@@ over a 配置_月亮_整数_宏名.
@@ 配置_月亮_整数_形式长度_宏名 is the length modifier for reading/writing integers.
@@ 配置_月亮_整数_格式_宏名 is the 格式_变量 for writing integers.
@@ 配置_月亮_最大整数_宏名 is the maximum 值_圆 for a 配置_月亮_整数_宏名.
@@ 配置_月亮_最小整数_宏名 is the minimum 值_圆 for a 配置_月亮_整数_宏名.
@@ 配置_月亮_最大无符号_宏名 is the maximum 值_圆 for a 配置_月亮_无符号_宏名.
@@ 配置_月亮_整数到串_宏名 converts an integer 到_变量 a string.
*/


/* The following definitions are good for most cases here */

#define 配置_月亮_整数_格式_宏名		"%" 配置_月亮_整数_形式长度_宏名 "d"

#define 配置_月亮I_UAC整型_宏名		配置_月亮_整数_宏名

#define 配置_月亮_整数到串_宏名(s,大小_短变量,n)  \
	配置_l_s打印f_宏名((s), 大小_短变量, 配置_月亮_整数_格式_宏名, (配置_月亮I_UAC整型_宏名)(n))

/*
** use 配置_月亮I_UAC整型_宏名 here 到_变量 avoid problems with promotions (which
** can turn a comparison between unsigneds into a signed comparison)
*/
#define 配置_月亮_无符号_宏名		unsigned 配置_月亮I_UAC整型_宏名


/* now the variable definitions */

#if 配置_月亮_整型_类型_宏名 == 配置_月亮_整型_整型_宏名		/* { int */

#define 配置_月亮_整数_宏名		int
#define 配置_月亮_整数_形式长度_宏名	""

#define 配置_月亮_最大整数_宏名		INT_MAX
#define 配置_月亮_最小整数_宏名		INT_MIN

#define 配置_月亮_最大无符号_宏名		UINT_MAX

#elif 配置_月亮_整型_类型_宏名 == 配置_月亮_整型_长型_宏名	/* }{ long */

#define 配置_月亮_整数_宏名		long
#define 配置_月亮_整数_形式长度_宏名	"l"

#define 配置_月亮_最大整数_宏名		LONG_MAX
#define 配置_月亮_最小整数_宏名		LONG_MIN

#define 配置_月亮_最大无符号_宏名		ULONG_MAX

#elif 配置_月亮_整型_类型_宏名 == 配置_月亮_整型_长型长型_宏名	/* }{ long long */

/* use presence of macro LLONG_MAX as proxy for C99 compliance */
#if defined(LLONG_MAX)		/* { */
/* use ISO C99 stuff */

#define 配置_月亮_整数_宏名		long long
#define 配置_月亮_整数_形式长度_宏名	"ll"

#define 配置_月亮_最大整数_宏名		LLONG_MAX
#define 配置_月亮_最小整数_宏名		LLONG_MIN

#define 配置_月亮_最大无符号_宏名		ULLONG_MAX

#elif defined(配置_月亮_用_WINDOWS_宏名) /* }{ */
/* in Windows, can use specific Windows types */

#define 配置_月亮_整数_宏名		__int64
#define 配置_月亮_整数_形式长度_宏名	"I64"

#define 配置_月亮_最大整数_宏名		_I64_MAX
#define 配置_月亮_最小整数_宏名		_I64_MIN

#define 配置_月亮_最大无符号_宏名		_UI64_MAX

#else				/* }{ */

#错误_小变量 "Compiler does not support 'long long'. Use 选项_变量 '-DLUA_32BITS' \
  or '-DLUA_C89_NUMBERS' (see file 'luaconf.h' for details)"

#endif				/* } */

#else				/* }{ */

#错误_小变量 "numeric integer type not defined"

#endif				/* } */

/* }================================================================== */


/*
** {==================================================================
** Dependencies with C99 and other C details
** ===================================================================
*/

/*
@@ 配置_l_s打印f_宏名 is equivalent 到_变量 'snprintf' or 'sprintf' in C89.
** (All uses in Lua have only one 格式_变量 item.)
*/
#if !defined(配置_月亮_用_C89_宏名)
#define 配置_l_s打印f_宏名(s,大小_短变量,f,i)	snprintf(s,大小_短变量,f,i)
#else
#define 配置_l_s打印f_宏名(s,大小_短变量,f,i)	((void)(大小_短变量), sprintf(s,f,i))
#endif


/*
@@ 月对象_月亮_串x到数目_函 converts a hexadecimal numeral 到_变量 a number.
** In C99, 'strtod' does that conversion. Otherwise, you can
** leave '月对象_月亮_串x到数目_函' undefined and Lua will provide its own
** implementation.
*/
#if !defined(配置_月亮_用_C89_宏名)
#define 月对象_月亮_串x到数目_函(s,p)		配置_月亮_串到数目_宏名(s,p)
#endif


/*
@@ 配置_月亮_指针到串_宏名 converts a pointer 到_变量 a 月载入库_可读性_函 string in a
** non-specified way.
*/
#define 配置_月亮_指针到串_宏名(缓冲_变量,大小_短变量,p)	配置_l_s打印f_宏名(缓冲_变量,大小_短变量,"%p",p)


/*
@@ 月串库_月亮_数目到串x_函 converts a float 到_变量 a hexadecimal numeral.
** In C99, 'sprintf' (with 格式_变量 specifiers '%a'/'%A') does that.
** Otherwise, you can leave '月串库_月亮_数目到串x_函' undefined and Lua will
** provide its own implementation.
*/
#if !defined(配置_月亮_用_C89_宏名)
#define 月串库_月亮_数目到串x_函(L,b,大小_短变量,f,n)  \
	((void)L, 配置_l_s打印f_宏名(b,大小_短变量,f,(配置_月亮I_UAC数目_宏名)(n)))
#endif


/*
** 'strtof' and 'opf' variants for math functions are not 有效_变量 in
** C89. Otherwise, the macro 'HUGE_VALF' is a good proxy for testing the
** availability of these variants. ('math.h' is already included in
** all files that use these macros.)
*/
#if defined(配置_月亮_用_C89_宏名) || (defined(HUGE_VAL) && !defined(HUGE_VALF))
#undef 配置_数学操作_宏名  /* variants not available */
#undef 配置_月亮_串到数目_宏名
#define 配置_数学操作_宏名(操作_短变量)		(炉_数目型)操作_短变量  /* no variant */
#define 配置_月亮_串到数目_宏名(s,p)	((炉_数目型)strtod((s), (p)))
#endif


/*
@@ 配置_月亮_K上下文_宏名 is the type of the context ('ctx') for continuation
** functions.  It must be a numerical type; Lua will use 'intptr_t' if
** available, otherwise it will use 'ptrdiff_t' (the nearest thing 到_变量
** 'intptr_t' in C89)
*/
#define 配置_月亮_K上下文_宏名	ptrdiff_t

#if !defined(配置_月亮_用_C89_宏名) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(INTPTR_MAX)  /* even in C99 this type is optional */
#undef 配置_月亮_K上下文_宏名
#define 配置_月亮_K上下文_宏名	intptr_t
#endif
#endif


/*
@@ 配置_月亮_获取本地化小数点_宏名 gets the locale "radix character" (decimal 针点_变量).
** Change that if you do not want 到_变量 use C locales. (Code using this
** macro must include the header 'locale.h'.)
*/
#if !defined(配置_月亮_获取本地化小数点_宏名)
#define 配置_月亮_获取本地化小数点_宏名()		(localeconv()->decimal_point[0])
#endif


/*
** macros 到_变量 improve jump prediction, used mostly for 错误_小变量 handling
** and debug facilities. (Some macros in the Lua API use these macros.
** Define 配置_月亮_无内置_宏名 if you do not want '__builtin_expect' in your
** 代码_变量.)
*/
#if !defined(配置_月亮i_可能性高_宏名)

#if defined(__GNUC__) && !defined(配置_月亮_无内置_宏名)
#define 配置_月亮i_可能性高_宏名(x)		(__builtin_expect(((x) != 0), 1))
#define 配置_月亮i_可能性低_宏名(x)	(__builtin_expect(((x) != 0), 0))
#else
#define 配置_月亮i_可能性高_宏名(x)		(x)
#define 配置_月亮i_可能性低_宏名(x)	(x)
#endif

#endif


#if defined(应程接_月亮_内核_宏名) || defined(辅助库_月亮_库_宏名)
/* shorter names for Lua's own use */
#define 配置_l_可能性高_宏名(x)	配置_月亮i_可能性高_宏名(x)
#define 配置_l_可能性低_宏名(x)	配置_月亮i_可能性低_宏名(x)
#endif



/* }================================================================== */


/*
** {==================================================================
** Language Variations
** =====================================================================
*/

/*
@@ 配置_月亮_不转化数目到串_宏名/配置_月亮_不转化串到数目_宏名 control how Lua performs some
** coercions. Define 配置_月亮_不转化数目到串_宏名 到_变量 turn off automatic coercion from
** numbers 到_变量 strings. Define 配置_月亮_不转化串到数目_宏名 到_变量 turn off automatic
** coercion from strings 到_变量 numbers.
*/
/* #define 配置_月亮_不转化数目到串_宏名 */
/* #define 配置_月亮_不转化串到数目_宏名 */


/*
@@ LUA_USE_APICHECK turns on several consistency checks on the C API.
** Define it as a help when debugging C 代码_变量.
*/
#if defined(LUA_USE_APICHECK)
#include <assert.h>
#define 配置_月亮i_应程接检查_宏名(l,e)	assert(e)
#endif

/* }================================================================== */


/*
** {==================================================================
** Macros that affect the API and must be stable (that is, must be the
** same when you compile Lua and when you compile 代码_变量 that links 到_变量
** Lua).
** =====================================================================
*/

/*
@@ 配置_月亮I_最大栈_宏名 限制们_变量 the 大小_变量 of the Lua 栈_圆小.
** CHANGE it if you need a different 限制_变量. This 限制_变量 is arbitrary;
** its only purpose is 到_变量 stop Lua from consuming unlimited 栈_圆小
** 空间_圆 (and 到_变量 reserve some numbers for pseudo-indices).
** (It must fit into 最大_小变量(size_t)/32 and 最大_小变量(int)/2.)
*/
#if 配置_月亮I_是否32位整型_宏名
#define 配置_月亮I_最大栈_宏名		1000000
#else
#define 配置_月亮I_最大栈_宏名		15000
#endif


/*
@@ 炉_额外空间 defines the 大小_变量 of a raw memory area associated with
** a Lua 状态机_变量 with very fast access.
** CHANGE it if you need a different 大小_变量.
*/
#define 炉_额外空间		(sizeof(void *))


/*
@@ 炉_身份大小 gives the maximum 大小_变量 for the description of the 源_圆
** of a function in debug information.
** CHANGE it if you want a different 大小_变量.
*/
#define 炉_身份大小	60


/*
@@ 炉L_缓冲区大小 is the initial 缓冲区_变量 大小_变量 used by the lauxlib
** 缓冲区_变量 system.
*/
#define 炉L_缓冲区大小   ((int)(16 * sizeof(void*) * sizeof(炉_数目型)))


/*
@@ 炉I_最大对齐 defines fields that, when used in a union, ensure
** maximum alignment for the other items in that union.
*/
#define 炉I_最大对齐  炉_数目型 n; double u; void *s; 炉_整数型 i; long l

/* }================================================================== */





/* =================================================================== */

/*
** Local configuration. You can use this 空间_圆 到_变量 add your redefinitions
** without modifying the main part of the file.
*/





#endif

