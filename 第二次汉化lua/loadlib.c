/*
** $Id: loadlib.c $
** Dynamic library loader for Lua
** See Copyright Notice in lua.h
**
** This module contains an implementation of loadlib for Unix systems
** that have dlfcn, an implementation for Windows, and a stub for other
** systems.
*/

#define loadlib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


/*
** 载入库_月亮_忽略掩码_宏名 is a 记号_变量 到_变量 ignore all before it when building the
** luaopen_ function 名称_变量.
*/
#if !defined (载入库_月亮_忽略掩码_宏名)
#define 载入库_月亮_忽略掩码_宏名		"-"
#endif


/*
** 载入库_月亮_C子分隔符_宏名 is the character that replaces dots in submodule names
** when searching for a C loader.
** 载入库_月亮_L子分隔符_宏名 is the character that replaces dots in submodule names
** when searching for a Lua loader.
*/
#if !defined(载入库_月亮_C子分隔符_宏名)
#define 载入库_月亮_C子分隔符_宏名		配置_月亮_目录分隔符_宏名
#endif

#if !defined(载入库_月亮_L子分隔符_宏名)
#define 载入库_月亮_L子分隔符_宏名		配置_月亮_目录分隔符_宏名
#endif


/* prefix for 打开_圆 functions in C libraries */
#define 载入库_月亮_形参偏移_宏名		"luaopen_"

/* separator for 打开_圆 functions in C libraries */
#define 载入库_月亮_偏移分隔符_宏名	"_"


/*
** 键_小变量 for table in the 注册表_变量 that keeps handles
** for all loaded C libraries
*/
static const char *const C库_变量 = "_CLIBS";

#define 载入库_库_失败_宏名	"打开_圆"


#define 月载入库_设置程序目录_函(L)           ((void)0)


/*
** Special type equivalent 到_变量 '(void*)' for functions in gcc
** (到_变量 suppress warnings when converting function pointers)
*/
typedef void (*voidf)(void);


/*
** system-dependent functions
*/

/*
** unload library '库_短变量'
*/
static void 月载入库_l系统_反载入库_函 (void *库_短变量);

/*
** load C library in file '路径_变量'. If 'seeglb', load with all names in
** the library global.
** Returns the library; in case of 错误_小变量, returns NULL plus an
** 错误_小变量 string in the 栈_圆小.
*/
static void *月载入库_l系统_载入_函 (炉_状态机结 *L, const char *路径_变量, int seeglb);

/*
** Try 到_变量 find a function named 'sym' in library '库_短变量'.
** Returns the function; in case of 错误_小变量, returns NULL plus an
** 错误_小变量 string in the 栈_圆小.
*/
static 炉_C函数半 月载入库_l系统_符号_函 (炉_状态机结 *L, void *库_短变量, const char *sym);




#if defined(配置_月亮_用_动链打开_宏名)	/* { */
/*
** {========================================================================
** This is an implementation of loadlib based on the dlfcn interface.
** The dlfcn interface is available in Linux, SunOS, Solaris, IRIX, FreeBSD,
** NetBSD, AIX 4.2, HPUX 11, and  probably most other Unix flavors, at least
** as an emulation layer on 顶部_变量 of native functions.
** =========================================================================
*/

#include <dlfcn.h>

/*
** Macro 到_变量 convert pointer-到_变量-void* 到_变量 pointer-到_变量-function. This 限制_类型转换_宏名
** is undefined according 到_变量 ISO C, but POSIX assumes that it works.
** (The '__extension__' in gnu compilers is only 到_变量 avoid warnings.)
*/
#if defined(__GNUC__)
#define 载入库_类型转换_函_宏名(p) (__extension__ (炉_C函数半)(p))
#else
#define 载入库_类型转换_函_宏名(p) ((炉_C函数半)(p))
#endif


static void 月载入库_l系统_反载入库_函 (void *库_短变量) {
  dlclose(库_短变量);
}


static void *月载入库_l系统_载入_函 (炉_状态机结 *L, const char *路径_变量, int seeglb) {
  void *库_短变量 = dlopen(路径_变量, RTLD_NOW | (seeglb ? RTLD_GLOBAL : RTLD_LOCAL));
  if (配置_l_可能性低_宏名(库_短变量 == NULL))
    月亮推入字符串_函(L, dlerror());
  return 库_短变量;
}


static 炉_C函数半 月载入库_l系统_符号_函 (炉_状态机结 *L, void *库_短变量, const char *sym) {
  炉_C函数半 f = 载入库_类型转换_函_宏名(dlsym(库_短变量, sym));
  if (配置_l_可能性低_宏名(f == NULL))
    月亮推入字符串_函(L, dlerror());
  return f;
}

/* }====================================================== */



#elif defined(配置_月亮_动链_动态链接库_宏名)	/* }{ */
/*
** {======================================================================
** This is an implementation of loadlib for Windows using native functions.
** =======================================================================
*/

#include <windows.h>


/*
** optional 标志们_变量 for LoadLibraryEx
*/
#if !defined(载入库_月亮_库扩展_标志_宏名)
#define 载入库_月亮_库扩展_标志_宏名	0
#endif


#undef 月载入库_设置程序目录_函


/*
** Replace in the 路径_变量 (on the 顶部_变量 of the 栈_圆小) any occurrence
** of 配置_月亮_执行_目录_宏名 with the executable's 路径_变量.
*/
static void 月载入库_设置程序目录_函 (炉_状态机结 *L) {
  char 缓冲_变量[MAX_PATH + 1];
  char *lb;
  DWORD 新大小_变量 = sizeof(缓冲_变量)/sizeof(char);
  DWORD n = GetModuleFileNameA(NULL, 缓冲_变量, 新大小_变量);  /* get exec. 名称_变量 */
  if (n == 0 || n == 新大小_变量 || (lb = strrchr(缓冲_变量, '\\')) == NULL)
    月亮状态_错误_函(L, "unable 到_变量 get ModuleFileName");
  else {
    *lb = '\0';  /* cut 名称_变量 on the 最后_变量 '\\' 到_变量 get the 路径_变量 */
    月亮状态_全局替换_函(L, 月头_月亮_到字符串_宏名(L, -1), 配置_月亮_执行_目录_宏名, 缓冲_变量);
    月头_月亮_移除_宏名(L, -2);  /* remove original string */
  }
}




static void 月载入库_推错误_函 (炉_状态机结 *L) {
  int 错误_小变量 = GetLastError();
  char 缓冲区_变量[128];
  if (FormatMessageA(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL, 错误_小变量, 0, 缓冲区_变量, sizeof(缓冲区_变量)/sizeof(char), NULL))
    月亮推入字符串_函(L, 缓冲区_变量);
  else
    月亮推入格式化字符串_函(L, "system 错误_小变量 %d\n", 错误_小变量);
}

static void 月载入库_l系统_反载入库_函 (void *库_短变量) {
  FreeLibrary((HMODULE)库_短变量);
}


static void *月载入库_l系统_载入_函 (炉_状态机结 *L, const char *路径_变量, int seeglb) {
  HMODULE 库_短变量 = LoadLibraryExA(路径_变量, NULL, 载入库_月亮_库扩展_标志_宏名);
  (void)(seeglb);  /* not used: symbols are 'global' by default */
  if (库_短变量 == NULL) 月载入库_推错误_函(L);
  return 库_短变量;
}


static 炉_C函数半 月载入库_l系统_符号_函 (炉_状态机结 *L, void *库_短变量, const char *sym) {
  炉_C函数半 f = (炉_C函数半)(voidf)GetProcAddress((HMODULE)库_短变量, sym);
  if (f == NULL) 月载入库_推错误_函(L);
  return f;
}

/* }====================================================== */


#else				/* }{ */
/*
** {======================================================
** Fallback for other systems
** =======================================================
*/

#undef 载入库_库_失败_宏名
#define 载入库_库_失败_宏名	"absent"


#define 载入库_动态链接消息_宏名	"dynamic libraries not enabled; 月解析器_检查_函 your Lua installation"


static void 月载入库_l系统_反载入库_函 (void *库_短变量) {
  (void)(库_短变量);  /* not used */
}


static void *月载入库_l系统_载入_函 (炉_状态机结 *L, const char *路径_变量, int seeglb) {
  (void)(路径_变量); (void)(seeglb);  /* not used */
  月头_月亮_推字面_宏名(L, 载入库_动态链接消息_宏名);
  return NULL;
}


static 炉_C函数半 月载入库_l系统_符号_函 (炉_状态机结 *L, void *库_短变量, const char *sym) {
  (void)(库_短变量); (void)(sym);  /* not used */
  月头_月亮_推字面_宏名(L, 载入库_动态链接消息_宏名);
  return NULL;
}

/* }====================================================== */
#endif				/* } */


/*
** {==================================================================
** Set Paths
** ===================================================================
*/

/*
** 载入库_月亮_路径_变量_宏名 and 载入库_月亮_C路径_变量_宏名 are the names of the environment
** variables that Lua 月解析器_检查_函 到_变量 set its paths.
*/
#if !defined(载入库_月亮_路径_变量_宏名)
#define 载入库_月亮_路径_变量_宏名    "LUA_PATH"
#endif

#if !defined(载入库_月亮_C路径_变量_宏名)
#define 载入库_月亮_C路径_变量_宏名   "LUA_CPATH"
#endif



/*
** return 注册表_变量.LUA_NOENV as a boolean
*/
static int 月载入库_无环境_函 (炉_状态机结 *L) {
  int b;
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, "LUA_NOENV");
  b = 月亮_到布尔值_函(L, -1);
  月头_月亮_弹出_宏名(L, 1);  /* remove 值_圆 */
  return b;
}


/*
** Set a 路径_变量
*/
static void 月载入库_设置路径_函 (炉_状态机结 *L, const char *fieldname,
                                   const char *envname,
                                   const char *dft) {
  const char *默认记号_变量;
  const char *版本数_变量 = 月亮推入格式化字符串_函(L, "%s%s", envname, 库_月亮_版本后缀_宏名);
  const char *路径_变量 = getenv(版本数_变量);  /* try versioned 名称_变量 */
  if (路径_变量 == NULL)  /* no versioned environment variable? */
    路径_变量 = getenv(envname);  /* try unversioned 名称_变量 */
  if (路径_变量 == NULL || 月载入库_无环境_函(L))  /* no environment variable? */
    月亮推入字符串_函(L, dft);  /* use default */
  else if ((默认记号_变量 = strstr(路径_变量, 配置_月亮_路径_分隔符_宏名 配置_月亮_路径_分隔符_宏名)) == NULL)
    月亮推入字符串_函(L, 路径_变量);  /* nothing 到_变量 改变_变量 */
  else {  /* 路径_变量 contains a ";;": insert default 路径_变量 in its place */
    size_t 长度_短变量 = strlen(路径_变量);
    炉L_缓冲区结 b;
    月亮状态_缓冲初始的_函(L, &b);
    if (路径_变量 < 默认记号_变量) {  /* is there a prefix before ';;'? */
      月亮状态_添加状态字符串_函(&b, 路径_变量, 默认记号_变量 - 路径_变量);  /* add it */
      luaL_addchar(&b, *配置_月亮_路径_分隔符_宏名);
    }
    月亮状态_添加字符串_函(&b, dft);  /* add default */
    if (默认记号_变量 < 路径_变量 + 长度_短变量 - 2) {  /* is there a suffix after ';;'? */
      luaL_addchar(&b, *配置_月亮_路径_分隔符_宏名);
      月亮状态_添加状态字符串_函(&b, 默认记号_变量 + 2, (路径_变量 + 长度_短变量 - 2) - 默认记号_变量);
    }
    月亮状态_推入结果_函(&b);
  }
  月载入库_设置程序目录_函(L);
  月亮设置字段_函(L, -3, fieldname);  /* package[fieldname] = 路径_变量 值_圆 */
  月头_月亮_弹出_宏名(L, 1);  /* pop versioned variable 名称_变量 ('版本数_变量') */
}

/* }================================================================== */


/*
** return 注册表_变量.C库_变量[路径_变量]
*/
static void *月载入库_检查C库_函 (炉_状态机结 *L, const char *路径_变量) {
  void *保护库_变量;
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, C库_变量);
  月亮获取字段_函(L, -1, 路径_变量);
  保护库_变量 = 月亮_到用户数据_函(L, -1);  /* 保护库_变量 = C库_变量[路径_变量] */
  月头_月亮_弹出_宏名(L, 2);  /* pop C库_变量 table and '保护库_变量' */
  return 保护库_变量;
}


/*
** 注册表_变量.C库_变量[路径_变量] = 保护库_变量        -- for queries
** 注册表_变量.C库_变量[#C库_变量 + 1] = 保护库_变量  -- also keep a 列表_变量 of all libraries
*/
static void 月载入库_添加到C库_函 (炉_状态机结 *L, const char *路径_变量, void *保护库_变量) {
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, C库_变量);
  月亮推入轻量用户数据_函(L, 保护库_变量);
  月亮_推入值_函(L, -1);
  月亮设置字段_函(L, -3, 路径_变量);  /* C库_变量[路径_变量] = 保护库_变量 */
  月亮_设置原始索引_函(L, -2, 月亮状态_长度_函(L, -2) + 1);  /* C库_变量[#C库_变量 + 1] = 保护库_变量 */
  月头_月亮_弹出_宏名(L, 1);  /* pop C库_变量 table */
}


/*
** __gc tag method for C库_变量 table: calls '月载入库_l系统_反载入库_函' for all 库_短变量
** handles in 列表_变量 C库_变量
*/
static int 月载入库_垃圾回收标签方法_函 (炉_状态机结 *L) {
  炉_整数型 n = 月亮状态_长度_函(L, 1);
  for (; n >= 1; n--) {  /* for each handle, in 月应程接_逆向_函 order */
    月亮获取原始索引_函(L, 1, n);  /* get handle C库_变量[n] */
    月载入库_l系统_反载入库_函(月亮_到用户数据_函(L, -1));
    月头_月亮_弹出_宏名(L, 1);  /* pop handle */
  }
  return 0;
}



/* 错误_小变量 codes for '月载入库_查找函_函' */
#define 载入库_错误库_宏名		1
#define 载入库_错误函_宏名		2

/*
** Look for a C function named 'sym' in a dynamically loaded library
** '路径_变量'.
** First, 月解析器_检查_函 whether the library is already loaded; if not, try
** 到_变量 load it.
** Then, if 'sym' is '*', return true (as library has been loaded).
** Otherwise, look for symbol 'sym' in the library and push a
** C function with that symbol.
** Return 0 and 'true' or a function in the 栈_圆小; in case of
** errors, return an 错误_小变量 代码_变量 and an 错误_小变量 message in the 栈_圆小.
*/
static int 月载入库_查找函_函 (炉_状态机结 *L, const char *路径_变量, const char *sym) {
  void *寄存_短变量 = 月载入库_检查C库_函(L, 路径_变量);  /* 月解析器_检查_函 loaded C libraries */
  if (寄存_短变量 == NULL) {  /* must load library? */
    寄存_短变量 = 月载入库_l系统_载入_函(L, 路径_变量, *sym == '*');  /* global symbols if 'sym'=='*' */
    if (寄存_短变量 == NULL) return 载入库_错误库_宏名;  /* unable 到_变量 load library */
    月载入库_添加到C库_函(L, 路径_变量, 寄存_短变量);
  }
  if (*sym == '*') {  /* loading only library (no function)? */
    月亮推入布尔值_函(L, 1);  /* return 'true' */
    return 0;  /* no errors */
  }
  else {
    炉_C函数半 f = 月载入库_l系统_符号_函(L, 寄存_短变量, sym);
    if (f == NULL)
      return 载入库_错误函_宏名;  /* unable 到_变量 find function */
    月头_月亮_推C函数_宏名(L, f);  /* else create new function */
    return 0;  /* no errors */
  }
}


static int 月载入库_载库_载入库_函 (炉_状态机结 *L) {
  const char *路径_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *初始的_变量 = 辅助库_月亮l_检查字符串_宏名(L, 2);
  int 状态_短变量 = 月载入库_查找函_函(L, 路径_变量, 初始的_变量);
  if (配置_l_可能性高_宏名(状态_短变量 == 0))  /* no errors? */
    return 1;  /* return the loaded function */
  else {  /* 错误_小变量; 错误_小变量 message is on 栈_圆小 顶部_变量 */
    辅助库_月亮l_推失败_宏名(L);
    月头_月亮_插入_宏名(L, -2);
    月亮推入字符串_函(L, (状态_短变量 == 载入库_错误库_宏名) ?  载入库_库_失败_宏名 : "初始的_变量");
    return 3;  /* return fail, 错误_小变量 message, and 哪儿_变量 */
  }
}



/*
** {======================================================
** 'require' function
** =======================================================
*/


static int 月载入库_可读性_函 (const char *文件名_变量) {
  FILE *f = fopen(文件名_变量, "r");  /* try 到_变量 打开_圆 file */
  if (f == NULL) return 0;  /* 打开_圆 failed */
  fclose(f);
  return 1;
}


/*
** Get the 下一个_变量 名称_变量 in '*路径_变量' = 'name1;name2;name3;...', changing
** the ending ';' 到_变量 '\0' 到_变量 create a zero-terminated string. Return
** NULL when 列表_变量 ends.
*/
static const char *月载入库_获取下一个文件名_函 (char **路径_变量, char *终_变量) {
  char *分隔_变量;
  char *名称_变量 = *路径_变量;
  if (名称_变量 == 终_变量)
    return NULL;  /* no 更多_变量 names */
  else if (*名称_变量 == '\0') {  /* from 前一个_变量 iteration? */
    *名称_变量 = *配置_月亮_路径_分隔符_宏名;  /* restore separator */
    名称_变量++;  /* skip it */
  }
  分隔_变量 = strchr(名称_变量, *配置_月亮_路径_分隔符_宏名);  /* find 下一个_变量 separator */
  if (分隔_变量 == NULL)  /* separator not found? */
    分隔_变量 = 终_变量;  /* 名称_变量 goes until the 终_变量 */
  *分隔_变量 = '\0';  /* finish file 名称_变量 */
  *路径_变量 = 分隔_变量;  /* will 起始_变量 下一个_变量 search from here */
  return 名称_变量;
}


/*
** Given a 路径_变量 such as ";blabla.so;blublu.so", pushes the string
**
** no file 'blabla.so'
**	no file 'blublu.so'
*/
static void 月载入库_推未找到错误_函 (炉_状态机结 *L, const char *路径_变量) {
  炉L_缓冲区结 b;
  月亮状态_缓冲初始的_函(L, &b);
  月亮状态_添加字符串_函(&b, "no file '");
  月亮状态_添加全局替换_函(&b, 路径_变量, 配置_月亮_路径_分隔符_宏名, "'\n\tno file '");
  月亮状态_添加字符串_函(&b, "'");
  月亮状态_推入结果_函(&b);
}


static const char *月载入库_搜索路径_函 (炉_状态机结 *L, const char *名称_变量,
                                             const char *路径_变量,
                                             const char *分隔_变量,
                                             const char *dirsep) {
  炉L_缓冲区结 缓冲_变量;
  char *路径名_变量;  /* 路径_变量 with 名称_变量 inserted */
  char *终路径名_变量;  /* its 终_变量 */
  const char *文件名_变量;
  /* separator is non-空容器_变量 and appears in '名称_变量'? */
  if (*分隔_变量 != '\0' && strchr(名称_变量, *分隔_变量) != NULL)
    名称_变量 = 月亮状态_全局替换_函(L, 名称_变量, 分隔_变量, dirsep);  /* replace it by 'dirsep' */
  月亮状态_缓冲初始的_函(L, &缓冲_变量);
  /* add 路径_变量 到_变量 the 缓冲区_变量, replacing marks ('?') with the file 名称_变量 */
  月亮状态_添加全局替换_函(&缓冲_变量, 路径_变量, 配置_月亮_路径_记号_宏名, 名称_变量);
  luaL_addchar(&缓冲_变量, '\0');
  路径名_变量 = luaL_buffaddr(&缓冲_变量);  /* writable 列表_变量 of file names */
  终路径名_变量 = 路径名_变量 + luaL_bufflen(&缓冲_变量) - 1;
  while ((文件名_变量 = 月载入库_获取下一个文件名_函(&路径名_变量, 终路径名_变量)) != NULL) {
    if (月载入库_可读性_函(文件名_变量))  /* does file exist and is 月载入库_可读性_函? */
      return 月亮推入字符串_函(L, 文件名_变量);  /* 月词法_保存_函 and return 名称_变量 */
  }
  月亮状态_推入结果_函(&缓冲_变量);  /* push 路径_变量 到_变量 create 错误_小变量 message */
  月载入库_推未找到错误_函(L, 月头_月亮_到字符串_宏名(L, -1));  /* create 错误_小变量 message */
  return NULL;  /* not found */
}


static int 月载入库_载库_低级搜索路径_函 (炉_状态机结 *L) {
  const char *f = 月载入库_搜索路径_函(L, 辅助库_月亮l_检查字符串_宏名(L, 1),
                                辅助库_月亮l_检查字符串_宏名(L, 2),
                                辅助库_月亮l_可选字符串_宏名(L, 3, "."),
                                辅助库_月亮l_可选字符串_宏名(L, 4, 配置_月亮_目录分隔符_宏名));
  if (f != NULL) return 1;
  else {  /* 错误_小变量 message is on 顶部_变量 of the 栈_圆小 */
    辅助库_月亮l_推失败_宏名(L);
    月头_月亮_插入_宏名(L, -2);
    return 2;  /* return fail + 错误_小变量 message */
  }
}


static const char *月载入库_找文件_函 (炉_状态机结 *L, const char *名称_变量,
                                           const char *pname,
                                           const char *dirsep) {
  const char *路径_变量;
  月亮获取字段_函(L, 月头_月亮_上值索引_宏名(1), pname);
  路径_变量 = 月头_月亮_到字符串_宏名(L, -1);
  if (配置_l_可能性低_宏名(路径_变量 == NULL))
    月亮状态_错误_函(L, "'package.%s' must be a string", pname);
  return 月载入库_搜索路径_函(L, 名称_变量, 路径_变量, ".", dirsep);
}


static int 月载入库_检查载入_函 (炉_状态机结 *L, int 状态_短变量, const char *文件名_变量) {
  if (配置_l_可能性高_宏名(状态_短变量)) {  /* module loaded successfully? */
    月亮推入字符串_函(L, 文件名_变量);  /* will be 2nd argument 到_变量 module */
    return 2;  /* return 打开_圆 function and file 名称_变量 */
  }
  else
    return 月亮状态_错误_函(L, "错误_小变量 loading module '%s' from file '%s':\n\t%s",
                          月头_月亮_到字符串_宏名(L, 1), 文件名_变量, 月头_月亮_到字符串_宏名(L, -1));
}


static int 月载入库_搜索器_月亮_函 (炉_状态机结 *L) {
  const char *文件名_变量;
  const char *名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  文件名_变量 = 月载入库_找文件_函(L, 名称_变量, "路径_变量", 载入库_月亮_L子分隔符_宏名);
  if (文件名_变量 == NULL) return 1;  /* module not found in this 路径_变量 */
  return 月载入库_检查载入_函(L, (辅助库_月亮l_载入文件_宏名(L, 文件名_变量) == LUA_OK), 文件名_变量);
}


/*
** Try 到_变量 find a load function for module '模名_变量' at file '文件名_变量'.
** First, 改变_变量 '.' 到_变量 '_' in '模名_变量'; then, if '模名_变量' has
** the 来自_变量 X-Y (that is, it has an "ignore 记号_变量"), build a function
** 名称_变量 "luaopen_X" and look for it. (For compatibility, if that
** fails, it also tries "luaopen_Y".) If there is no ignore 记号_变量,
** look for a function named "luaopen_modname".
*/
static int 月载入库_载入函_函 (炉_状态机结 *L, const char *文件名_变量, const char *模名_变量) {
  const char *打开函_变量;
  const char *记号_变量;
  模名_变量 = 月亮状态_全局替换_函(L, 模名_变量, ".", 载入库_月亮_偏移分隔符_宏名);
  记号_变量 = strchr(模名_变量, *载入库_月亮_忽略掩码_宏名);
  if (记号_变量) {
    int 状态_短变量;
    打开函_变量 = 月亮推入长字符串_函(L, 模名_变量, 记号_变量 - 模名_变量);
    打开函_变量 = 月亮推入格式化字符串_函(L, 载入库_月亮_形参偏移_宏名"%s", 打开函_变量);
    状态_短变量 = 月载入库_查找函_函(L, 文件名_变量, 打开函_变量);
    if (状态_短变量 != 载入库_错误函_宏名) return 状态_短变量;
    模名_变量 = 记号_变量 + 1;  /* else go ahead and try 旧_变量-style 名称_变量 */
  }
  打开函_变量 = 月亮推入格式化字符串_函(L, 载入库_月亮_形参偏移_宏名"%s", 模名_变量);
  return 月载入库_查找函_函(L, 文件名_变量, 打开函_变量);
}


static int 月载入库_C搜索器_函 (炉_状态机结 *L) {
  const char *名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *文件名_变量 = 月载入库_找文件_函(L, 名称_变量, "cpath", 载入库_月亮_C子分隔符_宏名);
  if (文件名_变量 == NULL) return 1;  /* module not found in this 路径_变量 */
  return 月载入库_检查载入_函(L, (月载入库_载入函_函(L, 文件名_变量, 名称_变量) == 0), 文件名_变量);
}


static int 月载入库_C根搜索器_函 (炉_状态机结 *L) {
  const char *文件名_变量;
  const char *名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *p = strchr(名称_变量, '.');
  int 状态_短变量;
  if (p == NULL) return 0;  /* is root */
  月亮推入长字符串_函(L, 名称_变量, p - 名称_变量);
  文件名_变量 = 月载入库_找文件_函(L, 月头_月亮_到字符串_宏名(L, -1), "cpath", 载入库_月亮_C子分隔符_宏名);
  if (文件名_变量 == NULL) return 1;  /* root not found */
  if ((状态_短变量 = 月载入库_载入函_函(L, 文件名_变量, 名称_变量)) != 0) {
    if (状态_短变量 != 载入库_错误函_宏名)
      return 月载入库_检查载入_函(L, 0, 文件名_变量);  /* real 错误_小变量 */
    else {  /* 打开_圆 function not found */
      月亮推入格式化字符串_函(L, "no module '%s' in file '%s'", 名称_变量, 文件名_变量);
      return 1;
    }
  }
  月亮推入字符串_函(L, 文件名_变量);  /* will be 2nd argument 到_变量 module */
  return 2;
}


static int 月载入库_预载入搜索器_函 (炉_状态机结 *L) {
  const char *名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_预载入_表_宏名);
  if (月亮获取字段_函(L, -1, 名称_变量) == 月头_月亮_T空值_宏名) {  /* not found? */
    月亮推入格式化字符串_函(L, "no 月解析器_字段_函 package.preload['%s']", 名称_变量);
    return 1;
  }
  else {
    月头_月亮_推字面_宏名(L, ":preload:");
    return 2;
  }
}


static void 月载入库_找载入器_函 (炉_状态机结 *L, const char *名称_变量) {
  int i;
  炉L_缓冲区结 消息_缩变量;  /* 到_变量 build 错误_小变量 message */
  /* push 'package.搜索器_变量' 到_变量 index 3 in the 栈_圆小 */
  if (配置_l_可能性低_宏名(月亮获取字段_函(L, 月头_月亮_上值索引_宏名(1), "搜索器_变量")
                 != 月头_月亮_T表_宏名))
    月亮状态_错误_函(L, "'package.搜索器_变量' must be a table");
  月亮状态_缓冲初始的_函(L, &消息_缩变量);
  /*  iterate over available 搜索器_变量 到_变量 find a loader */
  for (i = 1; ; i++) {
    月亮状态_添加字符串_函(&消息_缩变量, "\n\t");  /* 错误_小变量-message prefix */
    if (配置_l_可能性低_宏名(月亮获取原始索引_函(L, 3, i) == 月头_月亮_T空值_宏名)) {  /* no 更多_变量 搜索器_变量? */
      月头_月亮_弹出_宏名(L, 1);  /* remove nil */
      luaL_buffsub(&消息_缩变量, 2);  /* remove prefix */
      月亮状态_推入结果_函(&消息_缩变量);  /* create 错误_小变量 message */
      月亮状态_错误_函(L, "module '%s' not found:%s", 名称_变量, 月头_月亮_到字符串_宏名(L, -1));
    }
    月亮推入字符串_函(L, 名称_变量);
    月头_月亮_调用_宏名(L, 1, 2);  /* call it */
    if (月头_月亮_是否函数_宏名(L, -2))  /* did it find a loader? */
      return;  /* module loader found */
    else if (月亮_是否字符串_函(L, -2)) {  /* searcher returned 错误_小变量 message? */
      月头_月亮_弹出_宏名(L, 1);  /* remove 额外_变量 return */
      月亮状态_添加值_函(&消息_缩变量);  /* concatenate 错误_小变量 message */
    }
    else {  /* no 错误_小变量 message */
      月头_月亮_弹出_宏名(L, 2);  /* remove both returns */
      luaL_buffsub(&消息_缩变量, 2);  /* remove prefix */
    }
  }
}


static int 月载入库_载库_需求_函 (炉_状态机结 *L) {
  const char *名称_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  月亮_设置顶_函(L, 1);  /* LOADED table will be at index 2 */
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_已载入_表_宏名);
  月亮获取字段_函(L, 2, 名称_变量);  /* LOADED[名称_变量] */
  if (月亮_到布尔值_函(L, -1))  /* is it there? */
    return 1;  /* package is already loaded */
  /* else must load package */
  月头_月亮_弹出_宏名(L, 1);  /* remove '月操作系统库_获取字段_函' 结果_变量 */
  月载入库_找载入器_函(L, 名称_变量);
  月亮_旋转_函(L, -2, 1);  /* function <-> loader 数据_变量 */
  月亮_推入值_函(L, 1);  /* 名称_变量 is 1st argument 到_变量 module loader */
  月亮_推入值_函(L, -3);  /* loader 数据_变量 is 2nd argument */
  /* 栈_圆小: ...; loader 数据_变量; loader function; mod. 名称_变量; loader 数据_变量 */
  月头_月亮_调用_宏名(L, 2, 1);  /* run loader 到_变量 load module */
  /* 栈_圆小: ...; loader 数据_变量; 结果_变量 from loader */
  if (!月头_月亮_是否空值_宏名(L, -1))  /* non-nil return? */
    月亮设置字段_函(L, 2, 名称_变量);  /* LOADED[名称_变量] = returned 值_圆 */
  else
    月头_月亮_弹出_宏名(L, 1);  /* pop nil */
  if (月亮获取字段_函(L, 2, 名称_变量) == 月头_月亮_T空值_宏名) {   /* module set no 值_圆? */
    月亮推入布尔值_函(L, 1);  /* use true as 结果_变量 */
    月亮_复制_函(L, -1, -2);  /* replace loader 结果_变量 */
    月亮设置字段_函(L, 2, 名称_变量);  /* LOADED[名称_变量] = true */
  }
  月亮_旋转_函(L, -2, 1);  /* loader 数据_变量 <-> module 结果_变量  */
  return 2;  /* return module 结果_变量 and loader 数据_变量 */
}

/* }====================================================== */




static const 炉L_寄结 保常_函_变量[] = {
  {"loadlib", 月载入库_载库_载入库_函},
  {"月载入库_搜索路径_函", 月载入库_载库_低级搜索路径_函},
  /* placeholders */
  {"preload", NULL},
  {"cpath", NULL},
  {"路径_变量", NULL},
  {"搜索器_变量", NULL},
  {"loaded", NULL},
  {NULL, NULL}
};


static const 炉L_寄结 行列_函_变量[] = {
  {"require", 月载入库_载库_需求_函},
  {NULL, NULL}
};


static void 月载入库_创建搜索器表_函 (炉_状态机结 *L) {
  static const 炉_C函数半 搜索器_变量[] = {
    月载入库_预载入搜索器_函,
    月载入库_搜索器_月亮_函,
    月载入库_C搜索器_函,
    月载入库_C根搜索器_函,
    NULL
  };
  int i;
  /* create '搜索器_变量' table */
  月亮创建表_函(L, sizeof(搜索器_变量)/sizeof(搜索器_变量[0]) - 1, 0);
  /* fill it with predefined 搜索器_变量 */
  for (i=0; 搜索器_变量[i] != NULL; i++) {
    月亮_推入值_函(L, -2);  /* set 'package' as 上值_圆 for all 搜索器_变量 */
    月亮推入C闭包_函(L, 搜索器_变量[i], 1);
    月亮_设置原始索引_函(L, -2, i+1);
  }
  月亮设置字段_函(L, -2, "搜索器_变量");  /* put it in 月解析器_字段_函 '搜索器_变量' */
}


/*
** create table C库_变量 到_变量 keep track of loaded C libraries,
** setting a finalizer 到_变量 关闭_圆 all libraries when closing 状态机_变量.
*/
static void 月载入库_创建C库表_函 (炉_状态机结 *L) {
  月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, C库_变量);  /* create C库_变量 table */
  月亮创建表_函(L, 0, 1);  /* create 元表_小写 for C库_变量 */
  月头_月亮_推C函数_宏名(L, 月载入库_垃圾回收标签方法_函);
  月亮设置字段_函(L, -2, "__gc");  /* set finalizer for C库_变量 table */
  月亮_设置元表_函(L, -2);
}


配置_月亮模块_应程接_宏名 int 月亮打开包_函 (炉_状态机结 *L) {
  月载入库_创建C库表_函(L);
  辅助库_月亮l_新库_宏名(L, 保常_函_变量);  /* create 'package' table */
  月载入库_创建搜索器表_函(L);
  /* set paths */
  月载入库_设置路径_函(L, "路径_变量", 载入库_月亮_路径_变量_宏名, 配置_月亮_路径_默认_宏名);
  月载入库_设置路径_函(L, "cpath", 载入库_月亮_C路径_变量_宏名, 配置_月亮_C路径_默认_宏名);
  /* store config information */
  月头_月亮_推字面_宏名(L, 配置_月亮_目录分隔符_宏名 "\n" 配置_月亮_路径_分隔符_宏名 "\n" 配置_月亮_路径_记号_宏名 "\n"
                     配置_月亮_执行_目录_宏名 "\n" 载入库_月亮_忽略掩码_宏名 "\n");
  月亮设置字段_函(L, -2, "config");
  /* set 月解析器_字段_函 'loaded' */
  月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_已载入_表_宏名);
  月亮设置字段_函(L, -2, "loaded");
  /* set 月解析器_字段_函 'preload' */
  月亮状态_获取子表_函(L, 月头_月亮_注册表索引_宏名, 辅助库_月亮_预载入_表_宏名);
  月亮设置字段_函(L, -2, "preload");
  月头_月亮_推全局表_宏名(L);
  月亮_推入值_函(L, -2);  /* set 'package' as 上值_圆 for 下一个_变量 库_短变量 */
  月亮状态_设置函们_函(L, 行列_函_变量, 1);  /* 打开_圆 库_短变量 into global table */
  月头_月亮_弹出_宏名(L, 1);  /* pop global table */
  return 1;  /* return 'package' table */
}

