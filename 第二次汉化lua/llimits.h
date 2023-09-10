/*
** $Id: llimits.h $
** Limits, basic types, and some other 'installation-dependent' definitions
** See Copyright Notice in lua.h
*/

#ifndef llimits_h
#define llimits_h


#include <limits.h>
#include <stddef.h>


#include "lua.h"


/*
** 'lu_mem' and 'l_内存缩' are unsigned/signed integers big enough 到_变量 计数_变量
** the 总数_变量 memory used by Lua (in bytes). Usually, 'size_t' and
** 'ptrdiff_t' should 工作_变量, but we use 'long' for 16-bit machines.
*/
#if defined(LUAI_MEM)		/* { external definitions? */
typedef LUAI_UMEM lu_mem;
typedef LUAI_MEM l_内存缩;
#elif 配置_月亮I_是否32位整型_宏名	/* }{ */
typedef size_t lu_mem;
typedef ptrdiff_t l_内存缩;
#else  /* 16-bit ints */	/* }{ */
typedef unsigned long lu_mem;
typedef long l_内存缩;
#endif				/* } */


/* chars used as small naturals (so that 'char' is reserved for characters) */
typedef unsigned char 路_字节型;
typedef signed char ls_byte;


/* maximum 值_圆 for size_t */
#define 串库_最大_大小T_宏名	((size_t)(~(size_t)0))

/* maximum 大小_变量 visible for Lua (must be representable in a 炉_整数型) */
#define 限制_最大_大小_宏名	(sizeof(size_t) < sizeof(炉_整数型) ? 串库_最大_大小T_宏名 \
                          : (size_t)(配置_月亮_最大整数_宏名))


#define 限制_最大_LU内存_宏名	((lu_mem)(~(lu_mem)0))

#define 限制_最大_L内存_宏名	((l_内存缩)(限制_最大_LU内存_宏名 >> 1))


#define 限制_最大_整型_宏名		INT_MAX  /* maximum 值_圆 of an int */


/*
** floor of the log2 of the maximum signed 值_圆 for integral type 't'.
** (That is, maximum 'n' such that '2^n' fits in the given signed type.)
*/
#define 限制_2为底对数最大值_宏名(t)	(sizeof(t) * 8 - 2)


/*
** test whether an unsigned 值_圆 is a power of 2 (or zero)
*/
#define 限制_是否2的幂次方_宏名(x)	(((x) & ((x) - 1)) == 0)


/* number of chars of a literal string without the ending \0 */
#define LL(x)   (sizeof(x)/sizeof(char) - 1)


/*
** conversion of pointer 到_变量 unsigned integer: this is for hashing only;
** there is no problem if the integer 月编译器_不能_函 hold the whole pointer
** 值_圆. (In strict ISO C this may cause undefined behavior, but no
** 实际上_变量 machine seems 到_变量 bother.)
*/
#if !defined(配置_月亮_用_C89_宏名) && defined(__STDC_VERSION__) && \
    __STDC_VERSION__ >= 199901L
#include <stdint.h>
#if defined(UINTPTR_MAX)  /* even in C99 this type is optional */
#define 限制_L_指针到整数_宏名	uintptr_t
#else  /* no 'intptr'? */
#define 限制_L_指针到整数_宏名	uintmax_t  /* use the largest available integer */
#endif
#else  /* C89 选项_变量 */
#define 限制_L_指针到整数_宏名	size_t
#endif

#define 限制_指针到无符整型_宏名(p)	((unsigned int)((限制_L_指针到整数_宏名)(p) & UINT_MAX))



/* types of 'usual argument conversions' for 炉_数目型 and 炉_整数型 */
typedef 配置_月亮I_UAC数目_宏名 l_uacNumber;
typedef 配置_月亮I_UAC整型_宏名 l_uacInt;


/*
** Internal assertions for in-house debugging
*/
#if defined LUAI_ASSERT
#undef NDEBUG
#include <assert.h>
#define 限制_月亮_断言_宏名(c)           assert(c)
#endif

#if defined(限制_月亮_断言_宏名)
#define 限制_检查_表达式_宏名(c,e)		(限制_月亮_断言_宏名(c), (e))
/* 到_变量 avoid problems with conditions too long */
#define 限制_月亮_长断言_宏名(c)	((c) ? (void)0 : 限制_月亮_断言_宏名(0))
#else
#define 限制_月亮_断言_宏名(c)		((void)0)
#define 限制_检查_表达式_宏名(c,e)		(e)
#define 限制_月亮_长断言_宏名(c)	((void)0)
#endif

/*
** assertion for checking API calls
*/
#if !defined(配置_月亮i_应程接检查_宏名)
#define 配置_月亮i_应程接检查_宏名(l,e)	((void)l, 限制_月亮_断言_宏名(e))
#endif

#define 限制_应程接_检查_宏名(l,e,消息_缩变量)	配置_月亮i_应程接检查_宏名(l,(e) && 消息_缩变量)


/* macro 到_变量 avoid warnings about unused variables */
#if !defined(限制_未使用_宏名)
#define 限制_未使用_宏名(x)	((void)(x))
#endif


/* type casts (a macro highlights casts in the 代码_变量) */
#define 限制_类型转换_宏名(t, exp)	((t)(exp))

#define 限制_类型转换_空的_宏名(i)	限制_类型转换_宏名(void, (i))
#define 限制_类型转换_空的指针_宏名(i)	限制_类型转换_宏名(void *, (i))
#define 限制_类型转换_数目_宏名(i)	限制_类型转换_宏名(炉_数目型, (i))
#define 限制_类型转换_整型_宏名(i)	限制_类型转换_宏名(int, (i))
#define 限制_类型转换_无符整型_宏名(i)	限制_类型转换_宏名(unsigned int, (i))
#define 限制_类型转换_字节_宏名(i)	限制_类型转换_宏名(路_字节型, (i))
#define 限制_类型转换_无符印刻_宏名(i)	限制_类型转换_宏名(unsigned char, (i))
#define 限制_类型转换_印刻_宏名(i)	限制_类型转换_宏名(char, (i))
#define 限制_类型转换_印刻指针_宏名(i)	限制_类型转换_宏名(char *, (i))
#define 限制_类型转换_大小t_宏名(i)	限制_类型转换_宏名(size_t, (i))


/* 限制_类型转换_宏名 a signed 炉_整数型 到_变量 lua_Unsigned */
#if !defined(限制_l_类型转换符到无符_宏名)
#define 限制_l_类型转换符到无符_宏名(i)	((lua_Unsigned)(i))
#endif

/*
** 限制_类型转换_宏名 a lua_Unsigned 到_变量 a signed 炉_整数型; this 限制_类型转换_宏名 is
** not strict ISO C, but two-complement architectures should
** 工作_变量 fine.
*/
#if !defined(限制_l_类型转换无符到符_宏名)
#define 限制_l_类型转换无符到符_宏名(i)	((炉_整数型)(i))
#endif


/*
** non-return type
*/
#if !defined(限制_l_无返回值_宏名)

#if defined(__GNUC__)
#define 限制_l_无返回值_宏名		void __attribute__((noreturn))
#elif defined(_MSC_VER) && _MSC_VER >= 1200
#define 限制_l_无返回值_宏名		void __declspec(noreturn)
#else
#define 限制_l_无返回值_宏名		void
#endif

#endif


/*
** Inline functions
*/
#if !defined(配置_月亮_用_C89_宏名)
#define 限制_l_内联_宏名	inline
#elif defined(__GNUC__)
#define 限制_l_内联_宏名	__inline__
#else
#define 限制_l_内联_宏名	/* 空容器_变量 */
#endif

#define l_sinline	static 限制_l_内联_宏名


/*
** type for virtual-machine instructions;
** must be an unsigned with (at least) 4 bytes (see details in lopcodes.h)
*/
#if 配置_月亮I_是否32位整型_宏名
typedef unsigned int l_无符整32型;
#else
typedef unsigned long l_无符整32型;
#endif

typedef l_无符整32型 Instruction;



/*
** Maximum length for short strings, that is, strings that are
** internalized. (Cannot be smaller than reserved words or tags for
** metamethods, as these strings must be internalized;
** #("function") = 8, #("__newindex") = 10.)
*/
#if !defined(限制_月亮I_最大短型长度_宏名)
#define 限制_月亮I_最大短型长度_宏名	40
#endif


/*
** Initial 大小_变量 for the string table (must be power of 2).
** The Lua core alone registers ~50 strings (reserved words +
** metaevent keys + a few others). Libraries would typically add
** a few dozens 更多_变量.
*/
#if !defined(限制_最小串表大小_宏名)
#define 限制_最小串表大小_宏名	128
#endif


/*
** Size of cache for strings in the API. 'N' is the number of
** sets (better be a prime) and "M" is the 大小_变量 of each set (M == 1
** makes a direct cache.)
*/
#if !defined(串缓存_N大写)
#define 串缓存_N大写		53
#define 串缓存_M大写		2
#endif


/* minimum 大小_变量 for string 缓冲区_变量 */
#if !defined(限制_月亮_最小缓冲区_宏名)
#define 限制_月亮_最小缓冲区_宏名	32
#endif


/*
** Maximum depth for nested C calls, syntactical nested non-terminals,
** and other features implemented through recursion in C. (值_联 must
** fit in a 16-bit unsigned integer. It must also be compatible with
** the 大小_变量 of the C 栈_圆小.)
*/
#if !defined(限制_月亮I_最大C调用们_宏名)
#define 限制_月亮I_最大C调用们_宏名		200
#endif


/*
** macros that are executed whenever program enters the Lua core
** ('限制_月亮_锁_宏名') and leaves the core ('限制_月亮_解锁_宏名')
*/
#if !defined(限制_月亮_锁_宏名)
#define 限制_月亮_锁_宏名(L)	((void) 0)
#define 限制_月亮_解锁_宏名(L)	((void) 0)
#endif

/*
** macro executed during Lua functions at points 哪儿_变量 the
** function can yield.
*/
#if !defined(限制_月亮i_线程让步_宏名)
#define 限制_月亮i_线程让步_宏名(L)	{限制_月亮_解锁_宏名(L); 限制_月亮_锁_宏名(L);}
#endif


/*
** these macros allow user-specific actions when a 线程_变量 is
** created/deleted/resumed/yielded.
*/
#if !defined(限制_月亮i_用户状态打开_宏名)
#define 限制_月亮i_用户状态打开_宏名(L)		((void)L)
#endif

#if !defined(限制_月亮i_用户状态关闭_宏名)
#define 限制_月亮i_用户状态关闭_宏名(L)		((void)L)
#endif

#if !defined(限制_月亮i_用户状态线程_宏名)
#define 限制_月亮i_用户状态线程_宏名(L,L1)	((void)L)
#endif

#if !defined(限制_月亮i_用户状态释放_宏名)
#define 限制_月亮i_用户状态释放_宏名(L,L1)	((void)L)
#endif

#if !defined(限制_月亮i_用户状态恢复_宏名)
#define 限制_月亮i_用户状态恢复_宏名(L,n)	((void)L)
#endif

#if !defined(限制_月亮i_用户状态让步_宏名)
#define 限制_月亮i_用户状态让步_宏名(L,n)	((void)L)
#endif



/*
** The luai_num* macros define the primitive operations over numbers.
*/

/* floor division (defined as 'floor(a/b)') */
#if !defined(限制_月亮i_数目整数除法_宏名)
#define 限制_月亮i_数目整数除法_宏名(L,a,b)     ((void)L, 配置_l_向下取整_宏名(限制_月亮i_数目除法_宏名(L,a,b)))
#endif

/* float division */
#if !defined(限制_月亮i_数目除法_宏名)
#define 限制_月亮i_数目除法_宏名(L,a,b)      ((a)/(b))
#endif

/*
** modulo: defined as 'a - floor(a/b)*b'; the direct computation
** using this definition has several problems with rounding errors,
** so it is better 到_变量 use 'fmod'. 'fmod' gives the 结果_变量 of
** 'a - trunc(a/b)*b', and therefore must be corrected when
** 'trunc(a/b) ~= floor(a/b)'. That happens when the division has a
** non-integer negative 结果_变量: non-integer 结果_变量 is equivalent 到_变量
** a non-zero remainder 'm'; negative 结果_变量 is equivalent 到_变量 'a' and
** 'b' with different signs, or 'm' and 'b' with different signs
** (as the 结果_变量 'm' of 'fmod' has the same sign of 'a').
*/
#if !defined(限制_月亮i_数目取模_宏名)
#define 限制_月亮i_数目取模_宏名(L,a,b,m)  \
  { (void)L; (m) = 配置_数学操作_宏名(fmod)(a,b); \
    if (((m) > 0) ? (b) < 0 : ((m) < 0 && (b) > 0)) (m) += (b); }
#endif

/* exponentiation */
#if !defined(限制_月亮i_数目幂_宏名)
#define 限制_月亮i_数目幂_宏名(L,a,b)  \
  ((void)L, (b == 2) ? (a)*(a) : 配置_数学操作_宏名(pow)(a,b))
#endif

/* the others are quite standard operations */
#if !defined(限制_月亮i_数目加法_宏名)
#define 限制_月亮i_数目加法_宏名(L,a,b)      ((a)+(b))
#define 限制_月亮i_数目减法_宏名(L,a,b)      ((a)-(b))
#define 限制_月亮i_数目乘法_宏名(L,a,b)      ((a)*(b))
#define 限制_月亮i_数目取负_宏名(L,a)        (-(a))
#define 限制_月亮i_数目相等_宏名(a,b)         ((a)==(b))
#define 限制_月亮i_数目小于_宏名(a,b)         ((a)<(b))
#define 限制_月亮i_数目小等_宏名(a,b)         ((a)<=(b))
#define 限制_月亮i_数目大于_宏名(a,b)         ((a)>(b))
#define 限制_月亮i_数目大等_宏名(a,b)         ((a)>=(b))
#define 限制_月亮i_数目是否nan_宏名(a)        (!限制_月亮i_数目相等_宏名((a), (a)))
#endif





/*
** macro 到_变量 control inclusion of some hard tests on 栈_圆小 reallocation
*/
#if !defined(HARDSTACKTESTS)
#define 限制_条件移动栈_宏名(L,pre,位置_缩变量)	((void)0)
#else
/* realloc 栈_圆小 keeping its 大小_变量 */
#define 限制_条件移动栈_宏名(L,pre,位置_缩变量)  \
  { int sz_ = 状态机_栈大小_宏名(L); pre; 月亮调度_重新分配栈_函((L), sz_, 0); 位置_缩变量; }
#endif

#if !defined(HARDMEMTESTS)
#define 限制_条件改变内存_宏名(L,pre,位置_缩变量)	((void)0)
#else
#define 限制_条件改变内存_宏名(L,pre,位置_缩变量)  \
	{ if (垃圾回收_gc正在跑_宏名(G(L))) { pre; 月亮编译_完全垃圾回收_函(L, 0); 位置_缩变量; } }
#endif

#endif
