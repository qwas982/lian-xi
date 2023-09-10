/*
** $Id: lauxlib.h $
** Auxiliary functions for building Lua libraries
** See Copyright Notice in lua.h
*/


#ifndef lauxlib_h
#define lauxlib_h


#include <stddef.h>
#include <stdio.h>

#include "luaconf.h"
#include "lua.h"


/* global table */
#define 辅助库_月亮_全局名称_宏名	"_G"


typedef struct 炉L_缓冲区结 炉L_缓冲区结;


/* 额外_变量 错误_小变量 代码_变量 for '月亮状态_加载文件x_函' */
#define 辅助库_月亮_错误文件_宏名     (月头_月亮_错误中的错误_宏名+1)


/* 键_小变量, in the 注册表_变量, for table of loaded modules */
#define 辅助库_月亮_已载入_表_宏名	"_LOADED"


/* 键_小变量, in the 注册表_变量, for table of preloaded loaders */
#define 辅助库_月亮_预载入_表_宏名	"_PRELOAD"


typedef struct 炉L_寄结 {
  const char *名称_变量;
  炉_C函数半 函_短变量;
} 炉L_寄结;


#define 辅助库_月亮L_数量大小_宏名	(sizeof(炉_整数型)*16 + sizeof(炉_数目型))

配置_月亮库_应程接_宏名 void (月亮状态_检查版本_函) (炉_状态机结 *L, 炉_数目型 ver, size_t 大小_短变量);
#define 辅助库_月亮l_检查版本_宏名(L)  \
	  月亮状态_检查版本_函(L, 月头_月亮_版本_数目_宏名, 辅助库_月亮L_数量大小_宏名)

配置_月亮库_应程接_宏名 int (月亮状态_获取元字段_函) (炉_状态机结 *L, int 对象_变量, const char *e);
配置_月亮库_应程接_宏名 int (月亮状态_调用元_函) (炉_状态机结 *L, int 对象_变量, const char *e);
配置_月亮库_应程接_宏名 const char *(月亮状态_到字符串_函) (炉_状态机结 *L, int 索引_缩变量, size_t *长度_短变量);
配置_月亮库_应程接_宏名 int (月亮状态_实参错误_函) (炉_状态机结 *L, int 实参_短变量, const char *extramsg);
配置_月亮库_应程接_宏名 int (月亮状态_类型错误_函) (炉_状态机结 *L, int 实参_短变量, const char *tname);
配置_月亮库_应程接_宏名 const char *(月亮状态_检查状态字符串_函) (炉_状态机结 *L, int 实参_短变量,
                                                          size_t *l);
配置_月亮库_应程接_宏名 const char *(月亮状态_可选状态字符串_函) (炉_状态机结 *L, int 实参_短变量,
                                          const char *def, size_t *l);
配置_月亮库_应程接_宏名 炉_数目型 (月亮状态_检查数目_函) (炉_状态机结 *L, int 实参_短变量);
配置_月亮库_应程接_宏名 炉_数目型 (月亮状态_可选数目_函) (炉_状态机结 *L, int 实参_短变量, 炉_数目型 def);

配置_月亮库_应程接_宏名 炉_整数型 (月亮状态_检查整数_函) (炉_状态机结 *L, int 实参_短变量);
配置_月亮库_应程接_宏名 炉_整数型 (月亮状态_可选整数_函) (炉_状态机结 *L, int 实参_短变量,
                                          炉_整数型 def);

配置_月亮库_应程接_宏名 void (月亮状态_检查栈_函) (炉_状态机结 *L, int 大小_短变量, const char *消息_缩变量);
配置_月亮库_应程接_宏名 void (月亮状态_检查类型_函) (炉_状态机结 *L, int 实参_短变量, int t);
配置_月亮库_应程接_宏名 void (月亮状态_检查任意_函) (炉_状态机结 *L, int 实参_短变量);

配置_月亮库_应程接_宏名 int   (月亮状态_新元表_函) (炉_状态机结 *L, const char *tname);
配置_月亮库_应程接_宏名 void  (月亮状态_设置元表_函) (炉_状态机结 *L, const char *tname);
配置_月亮库_应程接_宏名 void *(月亮状态_测试用户数据_函) (炉_状态机结 *L, int 用数_缩变量, const char *tname);
配置_月亮库_应程接_宏名 void *(月亮状态_检查用户数据_函) (炉_状态机结 *L, int 用数_缩变量, const char *tname);

配置_月亮库_应程接_宏名 void (月亮状态_哪里_函) (炉_状态机结 *L, int lvl);
配置_月亮库_应程接_宏名 int (月亮状态_错误_函) (炉_状态机结 *L, const char *格式_短变量, ...);

配置_月亮库_应程接_宏名 int (月亮状态_检查选项_函) (炉_状态机结 *L, int 实参_短变量, const char *def,
                                   const char *const lst[]);

配置_月亮库_应程接_宏名 int (月亮状态_文件结果_函) (炉_状态机结 *L, int 状态_短变量, const char *函名_缩变量);
配置_月亮库_应程接_宏名 int (月亮状态_执行结果_函) (炉_状态机结 *L, int 状态_短变量);


/* predefined references */
#define 辅助库_月亮_无引用_宏名       (-2)
#define 辅助库_月亮_引用空值_宏名      (-1)

配置_月亮库_应程接_宏名 int (月亮状态_引用_函) (炉_状态机结 *L, int t);
配置_月亮库_应程接_宏名 void (月亮状态_取消引用_函) (炉_状态机结 *L, int t, int 引用_短变量);

配置_月亮库_应程接_宏名 int (月亮状态_加载文件x_函) (炉_状态机结 *L, const char *文件名_变量,
                                               const char *模块_变量);

#define 辅助库_月亮l_载入文件_宏名(L,f)	月亮状态_加载文件x_函(L,f,NULL)

配置_月亮库_应程接_宏名 int (月亮状态_加载缓冲区x_函) (炉_状态机结 *L, const char *缓冲_变量, size_t 大小_短变量,
                                   const char *名称_变量, const char *模块_变量);
配置_月亮库_应程接_宏名 int (月亮状态_加载字符串_函) (炉_状态机结 *L, const char *s);

配置_月亮库_应程接_宏名 炉_状态机结 *(月亮状态_新状态_函) (void);

配置_月亮库_应程接_宏名 炉_整数型 (月亮状态_长度_函) (炉_状态机结 *L, int 索引_缩变量);

配置_月亮库_应程接_宏名 void (月亮状态_添加全局替换_函) (炉L_缓冲区结 *b, const char *s,
                                     const char *p, const char *r);
配置_月亮库_应程接_宏名 const char *(月亮状态_全局替换_函) (炉_状态机结 *L, const char *s,
                                    const char *p, const char *r);

配置_月亮库_应程接_宏名 void (月亮状态_设置函们_函) (炉_状态机结 *L, const 炉L_寄结 *l, int 数量上_短变量);

配置_月亮库_应程接_宏名 int (月亮状态_获取子表_函) (炉_状态机结 *L, int 索引_缩变量, const char *函名_缩变量);

配置_月亮库_应程接_宏名 void (月亮状态_跟踪回溯_函) (炉_状态机结 *L, 炉_状态机结 *L1,
                                  const char *消息_缩变量, int 层级_变量);

配置_月亮库_应程接_宏名 void (月亮状态_要求引用_函) (炉_状态机结 *L, const char *模名_变量,
                                 炉_C函数半 openf, int glb);

/*
** ===============================================================
** some useful macros
** ===============================================================
*/


#define 辅助库_月亮l_新库表_宏名(L,l)	\
  月亮创建表_函(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

#define 辅助库_月亮l_新库_宏名(L,l)  \
  (辅助库_月亮l_检查版本_宏名(L), 辅助库_月亮l_新库表_宏名(L,l), 月亮状态_设置函们_函(L,l,0))

#define 辅助库_月亮l_实参检查_宏名(L, 条件_变量,实参_短变量,extramsg)	\
	((void)(配置_月亮i_可能性高_宏名(条件_变量) || 月亮状态_实参错误_函(L, (实参_短变量), (extramsg))))

#define 辅助库_月亮l_实参期望_宏名(L,条件_变量,实参_短变量,tname)	\
	((void)(配置_月亮i_可能性高_宏名(条件_变量) || 月亮状态_类型错误_函(L, (实参_短变量), (tname))))

#define 辅助库_月亮l_检查字符串_宏名(L,n)	(月亮状态_检查状态字符串_函(L, (n), NULL))
#define 辅助库_月亮l_可选字符串_宏名(L,n,d)	(月亮状态_可选状态字符串_函(L, (n), (d), NULL))

#define 辅助库_月亮l_类型名称_宏名(L,i)	月亮_类型名称_函(L, 月亮_类型_函(L,(i)))

#define 辅助库_月亮l_做文件_宏名(L, fn) \
	(辅助库_月亮l_载入文件_宏名(L, fn) || 月头_月亮_保护调用_宏名(L, 0, 月头_月亮_多返回_宏名, 0))

#define 辅助库_月亮l_做字符串_宏名(L, s) \
	(月亮状态_加载字符串_函(L, s) || 月头_月亮_保护调用_宏名(L, 0, 月头_月亮_多返回_宏名, 0))

#define 辅助库_月亮l_获取元表_宏名(L,n)	(月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, (n)))

#define 辅助库_月亮l_可选_宏名(L,f,n,d)	(月头_月亮_是否没有或空值_宏名(L,(n)) ? (d) : f(L,(n)))

#define 辅助库_月亮l_载入缓冲区_宏名(L,s,大小_短变量,n)	月亮状态_加载缓冲区x_函(L,s,大小_短变量,n,NULL)


/*
** Perform arithmetic operations on 炉_整数型 values with wrap-around
** semantics, as the Lua core does.
*/
#define 辅助库_月亮l_整型操作_宏名(操作_短变量,变1_缩变量,变2_缩变量)  \
	((炉_整数型)((lua_Unsigned)(变1_缩变量) 操作_短变量 (lua_Unsigned)(变2_缩变量)))


/* push the 值_圆 used 到_变量 represent failure/错误_小变量 */
#define 辅助库_月亮l_推失败_宏名(L)	月亮推入空值_函(L)


/*
** Internal assertions for in-house debugging
*/
#if !defined(限制_月亮_断言_宏名)

#if defined LUAI_ASSERT
  #include <assert.h>
  #define 限制_月亮_断言_宏名(c)		assert(c)
#else
  #define 限制_月亮_断言_宏名(c)		((void)0)
#endif

#endif



/*
** {======================================================
** Generic Buffer manipulation
** =======================================================
*/

struct 炉L_缓冲区结 {
  char *b;  /* 缓冲区_变量 address */
  size_t 大小_变量;  /* 缓冲区_变量 大小_变量 */
  size_t n;  /* number of characters in 缓冲区_变量 */
  炉_状态机结 *L;
  union {
    炉I_最大对齐;  /* ensure maximum alignment for 缓冲区_变量 */
    char b[炉L_缓冲区大小];  /* initial 缓冲区_变量 */
  } 初始的_变量;
};


#define luaL_bufflen(基函_缩变量)	((基函_缩变量)->n)
#define luaL_buffaddr(基函_缩变量)	((基函_缩变量)->b)


#define luaL_addchar(B,c) \
  ((void)((B)->n < (B)->大小_变量 || 月亮状态_预备缓冲区大小_函((B), 1)), \
   ((B)->b[(B)->n++] = (c)))

#define luaL_addsize(B,s)	((B)->n += (s))

#define luaL_buffsub(B,s)	((B)->n -= (s))

配置_月亮库_应程接_宏名 void (月亮状态_缓冲初始的_函) (炉_状态机结 *L, 炉L_缓冲区结 *B);
配置_月亮库_应程接_宏名 char *(月亮状态_预备缓冲区大小_函) (炉L_缓冲区结 *B, size_t 大小_短变量);
配置_月亮库_应程接_宏名 void (月亮状态_添加状态字符串_函) (炉L_缓冲区结 *B, const char *s, size_t l);
配置_月亮库_应程接_宏名 void (月亮状态_添加字符串_函) (炉L_缓冲区结 *B, const char *s);
配置_月亮库_应程接_宏名 void (月亮状态_添加值_函) (炉L_缓冲区结 *B);
配置_月亮库_应程接_宏名 void (月亮状态_推入结果_函) (炉L_缓冲区结 *B);
配置_月亮库_应程接_宏名 void (月亮状态_推入结果大小_函) (炉L_缓冲区结 *B, size_t 大小_短变量);
配置_月亮库_应程接_宏名 char *(月亮状态_缓冲初始的大小_函) (炉_状态机结 *L, 炉L_缓冲区结 *B, size_t 大小_短变量);

#define luaL_prepbuffer(B)	月亮状态_预备缓冲区大小_函(B, 炉L_缓冲区大小)

/* }====================================================== */



/*
** {======================================================
** File handles for IO library
** =======================================================
*/

/*
** A file handle is a userdata with 元表_小写 '辅助库_月亮_文件手握_宏名' and
** initial structure '炉L_流结' (it may contain other fields
** after that initial structure).
*/

#define 辅助库_月亮_文件手握_宏名          "FILE*"


typedef struct 炉L_流结 {
  FILE *f;  /* stream (NULL for incompletely created streams) */
  炉_C函数半 关闭f_小写;  /* 到_变量 关闭_圆 stream (NULL for closed streams) */
} 炉L_流结;

/* }====================================================== */

/*
** {==================================================================
** "Abstraction Layer" for basic 月解释器_报告_函 of messages and errors
** ===================================================================
*/

/* print a string */
#if !defined(辅助库_月亮_写字符串_宏名)
#define 辅助库_月亮_写字符串_宏名(s,l)   fwrite((s), sizeof(char), (l), stdout)
#endif

/* print a 新行_小变量 and flush the 输出_变量 */
#if !defined(辅助库_月亮_写行_宏名)
#define 辅助库_月亮_写行_宏名()        (辅助库_月亮_写字符串_宏名("\n", 1), fflush(stdout))
#endif

/* print an 错误_小变量 message */
#if !defined(辅助库_月亮_写字符串错误_宏名)
#define 辅助库_月亮_写字符串错误_宏名(s,p) \
        (fprintf(stderr, (s), (p)), fflush(stderr))
#endif

/* }================================================================== */


/*
** {============================================================
** Compatibility with deprecated conversions
** =============================================================
*/
#if defined(配置_月亮_兼容_应程接整型转换_宏名)

#define 辅助库_月亮l_检查无符号_宏名(L,a)	((lua_Unsigned)月亮状态_检查整数_函(L,a))
#define 辅助库_月亮l_可选无符号_宏名(L,a,d)	\
	((lua_Unsigned)月亮状态_可选整数_函(L,a,(炉_整数型)(d)))

#define 辅助库_月亮l_检查整型_宏名(L,n)	((int)月亮状态_检查整数_函(L, (n)))
#define 辅助库_月亮l_可选整型_宏名(L,n,d)	((int)月亮状态_可选整数_函(L, (n), (d)))

#define 辅助库_月亮l_检查长型_宏名(L,n)	((long)月亮状态_检查整数_函(L, (n)))
#define 辅助库_月亮l_可选长型_宏名(L,n,d)	((long)月亮状态_可选整数_函(L, (n), (d)))

#endif
/* }============================================================ */



#endif


