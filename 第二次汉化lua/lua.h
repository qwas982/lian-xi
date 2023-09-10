/*
** $Id: lua.h $
** Lua - A Scripting Language
** Lua.org, PUC-Rio, Brazil (http://www.lua.org)
** See Copyright Notice at the 终_变量 of this file
*/


#ifndef lua_h
#define lua_h

#include <stdarg.h>
#include <stddef.h>


#include "luaconf.h"


#define 月头_月亮_版本_主版_宏名	"5"
#define 月头_月亮_版本_副版_宏名	"4"
#define 月头_月亮_版本_发布_宏名	"6"

#define 月头_月亮_版本_数目_宏名			504
#define 月头_月亮_版本_发布_数目_宏名		(月头_月亮_版本_数目_宏名 * 100 + 6)

#define 月头_月亮_版本_宏名	"Lua " 月头_月亮_版本_主版_宏名 "." 月头_月亮_版本_副版_宏名
#define 月头_月亮_发布_宏名	月头_月亮_版本_宏名 "." 月头_月亮_版本_发布_宏名
#define 月头_月亮_版权_宏名	月头_月亮_发布_宏名 "  Copyright (C) 1994-2023 Lua.org, PUC-Rio"
#define 月头_月亮_作者_宏名	"R. Ierusalimschy, L. H. de Figueiredo, W. Celes"


/* 记号_变量 for precompiled 代码_变量 ('<esc>Lua') */
#define 月头_月亮_签名_宏名	"\x1bLua"

/* 选项_变量 for multiple returns in '月头_月亮_保护调用_宏名' and '月头_月亮_调用_宏名' */
#define 月头_月亮_多返回_宏名	(-1)


/*
** Pseudo-indices
** (-配置_月亮I_最大栈_宏名 is the minimum 有效_变量 index; we keep some free 空容器_变量
** 空间_圆 after that 到_变量 help overflow detection)
*/
#define 月头_月亮_注册表索引_宏名	(-配置_月亮I_最大栈_宏名 - 1000)
#define 月头_月亮_上值索引_宏名(i)	(月头_月亮_注册表索引_宏名 - (i))


/* 线程_变量 状态码_变量 */
#define LUA_OK		0
#define 月头_月亮_让步_宏名	1
#define 月头_月亮_运行错误_宏名	2
#define 月头_月亮_句法错误_宏名	3
#define 月头_月亮_内存错误_宏名	4
#define 月头_月亮_错误中的错误_宏名	5


typedef struct 炉_状态机结 炉_状态机结;


/*
** basic types
*/
#define 月头_月亮_T没有_宏名		(-1)

#define 月头_月亮_T空值_宏名		0
#define 月头_月亮_T布尔_宏名		1
#define 月头_月亮_T轻量用户数据_宏名	2
#define 月头_月亮_T数目_宏名		3
#define 月头_月亮_T字符串_宏名		4
#define 月头_月亮_T表_宏名		5
#define 月头_月亮_T函数_宏名		6
#define 月头_月亮_T用户数据_宏名		7
#define 月头_月亮_T线程_宏名		8

#define 炉_类型数们		9



/* minimum Lua 栈_圆小 available 到_变量 a C function */
#define 月头_月亮_最小栈_宏名	20


/* predefined values in the 注册表_变量 */
#define 月头_月亮_注册索引_主线程_宏名	1
#define 月头_月亮_注册索引_全局的_宏名	2
#define 月头_月亮_注册索引_最后的_宏名		月头_月亮_注册索引_全局的_宏名


/* type of numbers in Lua */
typedef 配置_月亮_数目_宏名 炉_数目型;


/* type for integer functions */
typedef 配置_月亮_整数_宏名 炉_整数型;

/* unsigned integer type */
typedef 配置_月亮_无符号_宏名 lua_Unsigned;

/* type for continuation-function contexts */
typedef 配置_月亮_K上下文_宏名 炉_K上下文型;


/*
** Type for C functions registered with Lua
*/
typedef int (*炉_C函数半) (炉_状态机结 *L);

/*
** Type for continuation functions
*/
typedef int (*炉_K函数型) (炉_状态机结 *L, int 状态码_变量, 炉_K上下文型 ctx);


/*
** Type for functions that read/write blocks when loading/转储中_变量 Lua chunks
*/
typedef const char * (*炉_读器型) (炉_状态机结 *L, void *用数_缩变量, size_t *大小_短变量);

typedef int (*炉_写器结) (炉_状态机结 *L, const void *p, size_t 大小_短变量, void *用数_缩变量);


/*
** Type for memory-allocation functions
*/
typedef void * (*炉_分配半) (void *用数_缩变量, void *ptr, size_t 原始大小_变量, size_t 新大小_变量);


/*
** Type for warning functions
*/
typedef void (*炉_警告函数型) (void *用数_缩变量, const char *消息_缩变量, int tocont);


/*
** Type used by the debug API 到_变量 collect debug information
*/
typedef struct 炉_调试结 炉_调试结;


/*
** Functions 到_变量 be called by the debugger in specific events
*/
typedef void (*炉_钩子型) (炉_状态机结 *L, 炉_调试结 *活记_缩变量);


/*
** generic 额外_变量 include file
*/
#if defined(LUA_USER_H)
#include LUA_USER_H
#endif


/*
** RCS ident string
*/
extern const char 月亮_标识_变量[];


/*
** 状态机_变量 manipulation
*/
配置_月亮_应程接_宏名 炉_状态机结 *(月亮_新状态机_函) (炉_分配半 f, void *用数_缩变量);
配置_月亮_应程接_宏名 void       (月亮_关闭_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 炉_状态机结 *(月亮_新线程_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 int        (月亮_关闭线程_函) (炉_状态机结 *L, 炉_状态机结 *from);
配置_月亮_应程接_宏名 int        (月亮_重置线程_函) (炉_状态机结 *L);  /* Deprecated! */

配置_月亮_应程接_宏名 炉_C函数半 (月亮_设置恐慌函数_函) (炉_状态机结 *L, 炉_C函数半 panicf);


配置_月亮_应程接_宏名 炉_数目型 (月亮_版本_函) (炉_状态机结 *L);


/*
** basic 栈_圆小 manipulation
*/
配置_月亮_应程接_宏名 int   (月亮_绝对索引_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int   (月亮_获取顶_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 void  (月亮_设置顶_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void  (月亮_推入值_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void  (月亮_旋转_函) (炉_状态机结 *L, int 索引_缩变量, int n);
配置_月亮_应程接_宏名 void  (月亮_复制_函) (炉_状态机结 *L, int fromidx, int toidx);
配置_月亮_应程接_宏名 int   (月亮_检查栈空间_函) (炉_状态机结 *L, int n);

配置_月亮_应程接_宏名 void  (月亮_传递数据_函) (炉_状态机结 *from, 炉_状态机结 *到_变量, int n);


/*
** access functions (栈_圆小 -> C)
*/

配置_月亮_应程接_宏名 int             (月亮_是否数目_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int             (月亮_是否字符串_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int             (月亮_是否C函数_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int             (月亮_是否整数_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int             (月亮_是否用户数据_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int             (月亮_类型_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 const char     *(月亮_类型名称_函) (炉_状态机结 *L, int tp);

配置_月亮_应程接_宏名 炉_数目型      (月亮_到数目x_函) (炉_状态机结 *L, int 索引_缩变量, int *是否数目_变量);
配置_月亮_应程接_宏名 炉_整数型     (月亮_到整数x_函) (炉_状态机结 *L, int 索引_缩变量, int *是否数目_变量);
配置_月亮_应程接_宏名 int             (月亮_到布尔值_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 const char     *(月亮_到长字符串_函) (炉_状态机结 *L, int 索引_缩变量, size_t *长度_短变量);
配置_月亮_应程接_宏名 lua_Unsigned    (月亮_原始长度_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 炉_C函数半   (月亮_到C函数_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void	       *(月亮_到用户数据_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 炉_状态机结      *(月亮到线程_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 const void     *(月亮到指针_函) (炉_状态机结 *L, int 索引_缩变量);


/*
** Comparison and arithmetic functions
*/

#define 月头_月亮_操作加法_宏名	0	/* ORDER TM, ORDER OP */
#define 月头_月亮_操作减法_宏名	1
#define 月头_月亮_操作乘法_宏名	2
#define 月头_月亮_操作取模_宏名	3
#define 月头_月亮_操作幂运算_宏名	4
#define 月头_月亮_操作除法_宏名	5
#define 月头_月亮_操作整数除_宏名	6
#define 月头_月亮_操作按位与_宏名	7
#define 月头_月亮_操作按位或_宏名	8
#define 月头_月亮_操作按位异或_宏名	9
#define 月头_月亮_操作左移_宏名	10
#define 月头_月亮_操作右移_宏名	11
#define 月头_月亮_操作负号_宏名	12
#define 月头_月亮_操作按位非_宏名	13

配置_月亮_应程接_宏名 void  (月亮算术_函) (炉_状态机结 *L, int 操作_短变量);

#define 月头_月亮_操作等于_宏名	0
#define 月头_月亮_操作小于_宏名	1
#define 月头_月亮_操作小等_宏名	2

配置_月亮_应程接_宏名 int   (月亮原始相等_函) (炉_状态机结 *L, int idx1, int idx2);
配置_月亮_应程接_宏名 int   (月亮对比_函) (炉_状态机结 *L, int idx1, int idx2, int 操作_短变量);


/*
** push functions (C -> 栈_圆小)
*/
配置_月亮_应程接_宏名 void        (月亮推入空值_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 void        (月亮推入数目_函) (炉_状态机结 *L, 炉_数目型 n);
配置_月亮_应程接_宏名 void        (月亮推入整数_函) (炉_状态机结 *L, 炉_整数型 n);
配置_月亮_应程接_宏名 const char *(月亮推入长字符串_函) (炉_状态机结 *L, const char *s, size_t 长度_短变量);
配置_月亮_应程接_宏名 const char *(月亮推入字符串_函) (炉_状态机结 *L, const char *s);
配置_月亮_应程接_宏名 const char *(月亮推入可变格式字符串_函) (炉_状态机结 *L, const char *格式_短变量,
                                                      va_list argp);
配置_月亮_应程接_宏名 const char *(月亮推入格式化字符串_函) (炉_状态机结 *L, const char *格式_短变量, ...);
配置_月亮_应程接_宏名 void  (月亮推入C闭包_函) (炉_状态机结 *L, 炉_C函数半 fn, int n);
配置_月亮_应程接_宏名 void  (月亮推入布尔值_函) (炉_状态机结 *L, int b);
配置_月亮_应程接_宏名 void  (月亮推入轻量用户数据_函) (炉_状态机结 *L, void *p);
配置_月亮_应程接_宏名 int   (月亮推入线程_函) (炉_状态机结 *L);


/*
** get functions (Lua -> 栈_圆小)
*/
配置_月亮_应程接_宏名 int (月亮获取全局变量_函) (炉_状态机结 *L, const char *名称_变量);
配置_月亮_应程接_宏名 int (月亮获取表_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int (月亮获取字段_函) (炉_状态机结 *L, int 索引_缩变量, const char *k);
配置_月亮_应程接_宏名 int (月亮获取索引_函) (炉_状态机结 *L, int 索引_缩变量, 炉_整数型 n);
配置_月亮_应程接_宏名 int (月亮获取原始_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 int (月亮获取原始索引_函) (炉_状态机结 *L, int 索引_缩变量, 炉_整数型 n);
配置_月亮_应程接_宏名 int (月亮获取原始指针_函) (炉_状态机结 *L, int 索引_缩变量, const void *p);

配置_月亮_应程接_宏名 void  (月亮创建表_函) (炉_状态机结 *L, int narr, int nrec);
配置_月亮_应程接_宏名 void *(月亮创建新的用户数据uv_函) (炉_状态机结 *L, size_t 大小_短变量, int 用户值数_缩);
配置_月亮_应程接_宏名 int   (月亮获取元表_函) (炉_状态机结 *L, int objindex);
配置_月亮_应程接_宏名 int  (月亮获取索引用户值_函) (炉_状态机结 *L, int 索引_缩变量, int n);


/*
** set functions (栈_圆小 -> Lua)
*/
配置_月亮_应程接_宏名 void  (月亮设置全局变量_函) (炉_状态机结 *L, const char *名称_变量);
配置_月亮_应程接_宏名 void  (月亮设置表_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void  (月亮设置字段_函) (炉_状态机结 *L, int 索引_缩变量, const char *k);
配置_月亮_应程接_宏名 void  (月亮_设置索引_函) (炉_状态机结 *L, int 索引_缩变量, 炉_整数型 n);
配置_月亮_应程接_宏名 void  (月亮_设置原始_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void  (月亮_设置原始索引_函) (炉_状态机结 *L, int 索引_缩变量, 炉_整数型 n);
配置_月亮_应程接_宏名 void  (月亮_设置原始指针_函) (炉_状态机结 *L, int 索引_缩变量, const void *p);
配置_月亮_应程接_宏名 int   (月亮_设置元表_函) (炉_状态机结 *L, int objindex);
配置_月亮_应程接_宏名 int   (月亮_设置索引用户值_函) (炉_状态机结 *L, int 索引_缩变量, int n);


/*
** 'load' and 'call' functions (load and run Lua 代码_变量)
*/
配置_月亮_应程接_宏名 void  (月亮_调用常量_函) (炉_状态机结 *L, int 实参数们_变量, int 结果数目_变量,
                           炉_K上下文型 ctx, 炉_K函数型 k);
#define 月头_月亮_调用_宏名(L,n,r)		月亮_调用常量_函(L, (n), (r), 0, NULL)

配置_月亮_应程接_宏名 int   (月亮_保护调用常量_函) (炉_状态机结 *L, int 实参数们_变量, int 结果数目_变量, int 错函_短变量,
                            炉_K上下文型 ctx, 炉_K函数型 k);
#define 月头_月亮_保护调用_宏名(L,n,r,f)	月亮_保护调用常量_函(L, (n), (r), (f), 0, NULL)

配置_月亮_应程接_宏名 int   (月亮_加载_函) (炉_状态机结 *L, 炉_读器型 读器_圆, void *dt,
                          const char *大块名_变量, const char *模块_变量);

配置_月亮_应程接_宏名 int (月亮_转储_函) (炉_状态机结 *L, 炉_写器结 写器_小写, void *数据_变量, int 剥离_变量);


/*
** coroutine functions
*/
配置_月亮_应程接_宏名 int  (月亮_让出常量_函)     (炉_状态机结 *L, int 结果数目_变量, 炉_K上下文型 ctx,
                               炉_K函数型 k);
配置_月亮_应程接_宏名 int  (月亮_恢复_函)     (炉_状态机结 *L, 炉_状态机结 *from, int 实参数_变量,
                               int *结果数目_变量);
配置_月亮_应程接_宏名 int  (月亮_状态码_函)     (炉_状态机结 *L);
配置_月亮_应程接_宏名 int (月亮_是否可让出_函) (炉_状态机结 *L);

#define 月头_月亮_让步_宏名(L,n)		月亮_让出常量_函(L, (n), 0, NULL)


/*
** Warning-related functions
*/
配置_月亮_应程接_宏名 void (月亮_设置警告函数_函) (炉_状态机结 *L, 炉_警告函数型 f, void *用数_缩变量);
配置_月亮_应程接_宏名 void (月亮_警告_函)  (炉_状态机结 *L, const char *消息_缩变量, int tocont);


/*
** garbage-collection function and 选项们_变量
*/

#define 月头_月亮_垃圾回收停止_宏名		0
#define 月头_月亮_垃圾回收重启_宏名		1
#define 月头_月亮_垃圾回收收集_宏名		2
#define 月头_月亮_垃圾回收计数_宏名		3
#define 月头_月亮_垃圾回收计数B_宏名		4
#define 月头_月亮_垃圾回收步进_宏名		5
#define 月头_月亮_垃圾回收设置暂停_宏名		6
#define 月头_月亮_垃圾回收设置步进乘法_宏名	7
#define 月头_月亮_垃圾回收是否正在跑_宏名		9
#define 月头_月亮_垃圾回收生成_宏名		10
#define 月头_月亮_垃圾回收递增_宏名		11

配置_月亮_应程接_宏名 int (月亮_垃圾回收_函) (炉_状态机结 *L, int 什么_变量, ...);


/*
** miscellaneous functions
*/

配置_月亮_应程接_宏名 int   (月亮_错误_函) (炉_状态机结 *L);

配置_月亮_应程接_宏名 int   (月亮_下一个_函) (炉_状态机结 *L, int 索引_缩变量);

配置_月亮_应程接_宏名 void  (月亮_拼接_函) (炉_状态机结 *L, int n);
配置_月亮_应程接_宏名 void  (月亮_长度_函)    (炉_状态机结 *L, int 索引_缩变量);

配置_月亮_应程接_宏名 size_t   (月亮_字符串到数目_函) (炉_状态机结 *L, const char *s);

配置_月亮_应程接_宏名 炉_分配半 (月亮_获取分配函数_函) (炉_状态机结 *L, void **用数_缩变量);
配置_月亮_应程接_宏名 void      (月亮_设置分配函数_函) (炉_状态机结 *L, 炉_分配半 f, void *用数_缩变量);

配置_月亮_应程接_宏名 void (月亮_到关闭_函) (炉_状态机结 *L, int 索引_缩变量);
配置_月亮_应程接_宏名 void (月亮_关闭槽_函) (炉_状态机结 *L, int 索引_缩变量);


/*
** {==============================================================
** some useful macros
** ===============================================================
*/

#define 月头_月亮_获取额外空间_宏名(L)	((void *)((char *)(L) - 炉_额外空间))

#define 月头_月亮_到数目_宏名(L,i)	月亮_到数目x_函(L,(i),NULL)
#define 月头_月亮_到整数_宏名(L,i)	月亮_到整数x_函(L,(i),NULL)

#define 月头_月亮_弹出_宏名(L,n)		月亮_设置顶_函(L, -(n)-1)

#define 月头_月亮_新表_宏名(L)		月亮创建表_函(L, 0, 0)

#define 月头_月亮_寄存器_宏名(L,n,f) (月头_月亮_推C函数_宏名(L, (f)), 月亮设置全局变量_函(L, (n)))

#define 月头_月亮_推C函数_宏名(L,f)	月亮推入C闭包_函(L, (f), 0)

#define 月头_月亮_是否函数_宏名(L,n)	(月亮_类型_函(L, (n)) == 月头_月亮_T函数_宏名)
#define 月头_月亮_是否表_宏名(L,n)	(月亮_类型_函(L, (n)) == 月头_月亮_T表_宏名)
#define 月头_月亮_是否轻量用户数据_宏名(L,n)	(月亮_类型_函(L, (n)) == 月头_月亮_T轻量用户数据_宏名)
#define 月头_月亮_是否空值_宏名(L,n)		(月亮_类型_函(L, (n)) == 月头_月亮_T空值_宏名)
#define 月头_月亮_是否布尔值_宏名(L,n)	(月亮_类型_函(L, (n)) == 月头_月亮_T布尔_宏名)
#define 月头_月亮_是否线程_宏名(L,n)	(月亮_类型_函(L, (n)) == 月头_月亮_T线程_宏名)
#define 月头_月亮_是否没有_宏名(L,n)		(月亮_类型_函(L, (n)) == 月头_月亮_T没有_宏名)
#define 月头_月亮_是否没有或空值_宏名(L, n)	(月亮_类型_函(L, (n)) <= 0)

#define 月头_月亮_推字面_宏名(L, s)	月亮推入字符串_函(L, "" s)

#define 月头_月亮_推全局表_宏名(L)  \
	((void)月亮获取原始索引_函(L, 月头_月亮_注册表索引_宏名, 月头_月亮_注册索引_全局的_宏名))

#define 月头_月亮_到字符串_宏名(L,i)	月亮_到长字符串_函(L, (i), NULL)


#define 月头_月亮_插入_宏名(L,索引_缩变量)	月亮_旋转_函(L, (索引_缩变量), 1)

#define 月头_月亮_移除_宏名(L,索引_缩变量)	(月亮_旋转_函(L, (索引_缩变量), -1), 月头_月亮_弹出_宏名(L, 1))

#define 月头_月亮_替换_宏名(L,索引_缩变量)	(月亮_复制_函(L, -1, (索引_缩变量)), 月头_月亮_弹出_宏名(L, 1))

/* }============================================================== */


/*
** {==============================================================
** compatibility macros
** ===============================================================
*/
#if defined(配置_月亮_兼容_应程接整型转换_宏名)

#define 月头_月亮_推无符_宏名(L,n)	月亮推入整数_函(L, (炉_整数型)(n))
#define 月头_月亮_到无符x_宏名(L,i,is)	((lua_Unsigned)月亮_到整数x_函(L,i,is))
#define 月头_月亮_到无符_宏名(L,i)	月头_月亮_到无符x_宏名(L,(i),NULL)

#endif

#define 月头_月亮_新用户数据_宏名(L,s)	月亮创建新的用户数据uv_函(L,s,1)
#define 月头_月亮_获取用户值_宏名(L,索引_缩变量)	月亮获取索引用户值_函(L,索引_缩变量,1)
#define 月头_月亮_设置用户值_宏名(L,索引_缩变量)	月亮_设置索引用户值_函(L,索引_缩变量,1)

#define 月头_月亮_标签数目_宏名		炉_类型数们

/* }============================================================== */

/*
** {======================================================================
** Debug API
** =======================================================================
*/


/*
** Event codes
*/
#define 月头_月亮_钩子调用_宏名	0
#define 月头_月亮_钩子返回_宏名	1
#define 月头_月亮_钩子行_宏名	2
#define 月头_月亮_钩子计数_宏名	3
#define 月头_月亮_钩子尾调用_宏名 4


/*
** Event masks
*/
#define 月头_月亮_掩码调用_宏名	(1 << 月头_月亮_钩子调用_宏名)
#define 月头_月亮_掩码返回_宏名	(1 << 月头_月亮_钩子返回_宏名)
#define 月头_月亮_掩码行_宏名	(1 << 月头_月亮_钩子行_宏名)
#define 月头_月亮_掩码计数_宏名	(1 << 月头_月亮_钩子计数_宏名)


配置_月亮_应程接_宏名 int (月亮_获取栈_函) (炉_状态机结 *L, int 层级_变量, 炉_调试结 *活记_缩变量);
配置_月亮_应程接_宏名 int (月亮_获取信息_函) (炉_状态机结 *L, const char *什么_变量, 炉_调试结 *活记_缩变量);
配置_月亮_应程接_宏名 const char *(月亮_获取本地变量_函) (炉_状态机结 *L, const 炉_调试结 *活记_缩变量, int n);
配置_月亮_应程接_宏名 const char *(月亮_设置本地变量_函) (炉_状态机结 *L, const 炉_调试结 *活记_缩变量, int n);
配置_月亮_应程接_宏名 const char *(月亮_获取上值_函) (炉_状态机结 *L, int funcindex, int n);
配置_月亮_应程接_宏名 const char *(月亮_设置上值_函) (炉_状态机结 *L, int funcindex, int n);

配置_月亮_应程接_宏名 void *(月亮_上值ID_函) (炉_状态机结 *L, int fidx, int n);
配置_月亮_应程接_宏名 void  (月亮_上值加入_函) (炉_状态机结 *L, int fidx1, int 数1_缩变量,
                                               int fidx2, int 数2_缩变量);

配置_月亮_应程接_宏名 void (月亮_设置钩子_函) (炉_状态机结 *L, 炉_钩子型 函_短变量, int 掩码_变量, int 计数_变量);
配置_月亮_应程接_宏名 炉_钩子型 (月亮_获取钩子_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 int (月亮_获取钩子掩码_函) (炉_状态机结 *L);
配置_月亮_应程接_宏名 int (月亮_获取钩子计数_函) (炉_状态机结 *L);

配置_月亮_应程接_宏名 int (月亮_设置C栈限制_函) (炉_状态机结 *L, unsigned int 限制_变量);

struct 炉_调试结 {
  int 事件_变量;
  const char *名称_变量;	/* (n) */
  const char *什么名_圆;	/* (n) 'global', 'local', '月解析器_字段_函', 'method' */
  const char *什么_变量;	/* (S) 'Lua', 'C', 'main', 'tail' */
  const char *源_圆;	/* (S) */
  size_t 源的长_短;	/* (S) */
  int 当前行_变量;	/* (l) */
  int 已定义行_小写;	/* (S) */
  int 最后已定义行_小写;	/* (S) */
  unsigned char 上值数_短缩;	/* (u) number of 上值们_小写 */
  unsigned char 形参数量_变量;/* (u) number of parameters */
  char 是变量实参吗_变量;        /* (u) */
  char 是尾调用吗_圆;	/* (t) */
  unsigned short 函传输_变量;   /* (r) index of 首先_变量 值_圆 transferred */
  unsigned short 转移数_缩;   /* (r) number of transferred values */
  char 短的_源小写[炉_身份大小]; /* (S) */
  /* private part */
  struct 调用信息_结 *i_调信缩;  /* active function */
};

/* }====================================================================== */


/******************************************************************************
* Copyright (C) 1994-2023 Lua.org, PUC-Rio.
*
* Permission is hereby granted, free of charge, 到_变量 any person obtaining
* a copy of this software and associated documentation files (the
* "Software"), 到_变量 deal in the Software without restriction, including
* without limitation the rights 到_变量 use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and 到_变量
* permit persons 到_变量 whom the Software is furnished 到_变量 do so, subject 到_变量
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/


#endif
