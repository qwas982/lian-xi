/*
** $Id: liolib.c $
** Standard I/O (and system) library
** See Copyright Notice in lua.h
*/

#define liolib_c
#define 辅助库_月亮_库_宏名

#include "lprefix.h"


#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"




/*
** Change this macro 到_变量 accept other modes for 'fopen' besides
** the standard ones.
*/
#if !defined(月入出库_l_检查模式_函)

/* accepted extensions 到_变量 '模块_变量' in 'fopen' */
#if !defined(入出库_L_模式扩展_宏名)
#define 入出库_L_模式扩展_宏名	"b"
#endif

/* Check whether '模块_变量' matches '[rwa]%+?[入出库_L_模式扩展_宏名]*' */
static int 月入出库_l_检查模式_函 (const char *模块_变量) {
  return (*模块_变量 != '\0' && strchr("rwa", *(模块_变量++)) != NULL &&
         (*模块_变量 != '+' || ((void)(++模块_变量), 1)) &&  /* skip if char is '+' */
         (strspn(模块_变量, 入出库_L_模式扩展_宏名) == strlen(模块_变量)));  /* 月解析器_检查_函 extensions */
}

#endif

/*
** {======================================================
** 入出库_l_保护打开_宏名 spawns a new process connected 到_变量 the 当前_圆
** one through the file streams.
** =======================================================
*/

#if !defined(入出库_l_保护打开_宏名)		/* { */

#if defined(配置_月亮_用_POSIX_宏名)	/* { */

#define 入出库_l_保护打开_宏名(L,c,m)		(fflush(NULL), popen(c,m))
#define 入出库_l_保护关闭_宏名(L,file)	(pclose(file))

#elif defined(配置_月亮_用_WINDOWS_宏名)	/* }{ */

#define 入出库_l_保护打开_宏名(L,c,m)		(_popen(c,m))
#define 入出库_l_保护关闭_宏名(L,file)	(_pclose(file))

#if !defined(入出库_l_检查模式保护_宏名)
/* Windows accepts "[rw][bt]?" as 有效_变量 modes */
#define 入出库_l_检查模式保护_宏名(m)	((m[0] == 'r' || m[0] == 'w') && \
  (m[1] == '\0' || ((m[1] == 'b' || m[1] == 't') && m[2] == '\0')))
#endif

#else				/* }{ */

/* ISO C definitions */
#define 入出库_l_保护打开_宏名(L,c,m)  \
	  ((void)c, (void)m, \
	  月亮状态_错误_函(L, "'popen' not supported"), \
	  (FILE*)0)
#define 入出库_l_保护关闭_宏名(L,file)		((void)L, (void)file, -1)

#endif				/* } */

#endif				/* } */


#if !defined(入出库_l_检查模式保护_宏名)
/* By default, Lua accepts only "r" or "w" as 有效_变量 modes */
#define 入出库_l_检查模式保护_宏名(m)        ((m[0] == 'r' || m[0] == 'w') && m[1] == '\0')
#endif

/* }====================================================== */


#if !defined(入出库_l_获取c_宏名)		/* { */

#if defined(配置_月亮_用_POSIX_宏名)
#define 入出库_l_获取c_宏名(f)		getc_unlocked(f)
#define 入出库_l_加锁文件_宏名(f)		flockfile(f)
#define 入出库_l_解锁文件_宏名(f)		funlockfile(f)
#else
#define 入出库_l_获取c_宏名(f)		getc(f)
#define 入出库_l_加锁文件_宏名(f)		((void)0)
#define 入出库_l_解锁文件_宏名(f)		((void)0)
#endif

#endif				/* } */


/*
** {======================================================
** 入出库_l_文件寻求_宏名: configuration for longer offsets
** =======================================================
*/

#if !defined(入出库_l_文件寻求_宏名)		/* { */

#if defined(配置_月亮_用_POSIX_宏名)	/* { */

#include <sys/types.h>

#define 入出库_l_文件寻求_宏名(f,o,w)		fseeko(f,o,w)
#define 入出库_l_文件告知_宏名(f)		ftello(f)
#define 入出库_l_寻求数目_宏名		off_t

#elif defined(配置_月亮_用_WINDOWS_宏名) && !defined(_CRTIMP_TYPEINFO) \
   && defined(_MSC_VER) && (_MSC_VER >= 1400)	/* }{ */

/* Windows (but not DDK) and Visual C++ 2005 or higher */
#define 入出库_l_文件寻求_宏名(f,o,w)		_fseeki64(f,o,w)
#define 入出库_l_文件告知_宏名(f)		_ftelli64(f)
#define 入出库_l_寻求数目_宏名		__int64

#else				/* }{ */

/* ISO C definitions */
#define 入出库_l_文件寻求_宏名(f,o,w)		fseek(f,o,w)
#define 入出库_l_文件告知_宏名(f)		ftell(f)
#define 入出库_l_寻求数目_宏名		long

#endif				/* } */

#endif				/* } */

/* }====================================================== */



#define 入出库_入出_前缀_宏名	"_IO_"
#define 入出库_入出前缀_长度_宏名	(sizeof(入出库_入出_前缀_宏名)/sizeof(char) - 1)
#define 入出库_入出_输入_宏名	(入出库_入出_前缀_宏名 "input")
#define 入出库_入出_输出_宏名	(入出库_入出_前缀_宏名 "输出_变量")


typedef 炉L_流结 LStream;


#define 入出库_到l流_宏名(L)	((LStream *)月亮状态_检查用户数据_函(L, 1, 辅助库_月亮_文件手握_宏名))

#define 入出库_是否已关闭_宏名(p)	((p)->关闭f_小写 == NULL)


static int 月入出库_入出_类型_函 (炉_状态机结 *L) {
  LStream *p;
  月亮状态_检查任意_函(L, 1);
  p = (LStream *)月亮状态_测试用户数据_函(L, 1, 辅助库_月亮_文件手握_宏名);
  if (p == NULL)
    辅助库_月亮l_推失败_宏名(L);  /* not a file */
  else if (入出库_是否已关闭_宏名(p))
    月头_月亮_推字面_宏名(L, "closed file");
  else
    月头_月亮_推字面_宏名(L, "file");
  return 1;
}


static int 月入出库_f_到字符串_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  if (入出库_是否已关闭_宏名(p))
    月头_月亮_推字面_宏名(L, "file (closed)");
  else
    月亮推入格式化字符串_函(L, "file (%p)", p->f);
  return 1;
}


static FILE *月入出库_到文件_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  if (配置_l_可能性低_宏名(入出库_是否已关闭_宏名(p)))
    月亮状态_错误_函(L, "attempt 到_变量 use a closed file");
  限制_月亮_断言_宏名(p->f);
  return p->f;
}


/*
** When creating file handles, always creates a 'closed' file handle
** before opening the 实际上_变量 file; so, if there is a memory 错误_小变量, the
** handle is in a consistent 状态机_变量.
*/
static LStream *月入出库_新预备文件_函 (炉_状态机结 *L) {
  LStream *p = (LStream *)月亮创建新的用户数据uv_函(L, sizeof(LStream), 0);
  p->关闭f_小写 = NULL;  /* 记号_变量 file handle as 'closed' */
  月亮状态_设置元表_函(L, 辅助库_月亮_文件手握_宏名);
  return p;
}


/*
** Calls the '关闭_圆' function from a file handle. The 'volatile' avoids
** a bug in some versions of the Clang compiler (e.g., clang 3.0 for
** 32 bits).
*/
static int 月入出库_辅助_关闭_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  volatile 炉_C函数半 闭函_缩变量 = p->关闭f_小写;
  p->关闭f_小写 = NULL;  /* 记号_变量 stream as closed */
  return (*闭函_缩变量)(L);  /* 关闭_圆 it */
}


static int 月入出库_f_关闭_函 (炉_状态机结 *L) {
  月入出库_到文件_函(L);  /* make sure argument is an 打开_圆 stream */
  return 月入出库_辅助_关闭_函(L);
}


static int 月入出库_入出_关闭_函 (炉_状态机结 *L) {
  if (月头_月亮_是否没有_宏名(L, 1))  /* no argument? */
    月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 入出库_入出_输出_宏名);  /* use default 输出_变量 */
  return 月入出库_f_关闭_函(L);
}


static int 月入出库_f_垃圾回收_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  if (!入出库_是否已关闭_宏名(p) && p->f != NULL)
    月入出库_辅助_关闭_函(L);  /* ignore closed and incompletely 打开_圆 files */
  return 0;
}


/*
** function 到_变量 关闭_圆 regular files
*/
static int 月入出库_入出_f关闭_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  int 结果_短变量 = fclose(p->f);
  return 月亮状态_文件结果_函(L, (结果_短变量 == 0), NULL);
}


static LStream *月入出库_新文件_函 (炉_状态机结 *L) {
  LStream *p = 月入出库_新预备文件_函(L);
  p->f = NULL;
  p->关闭f_小写 = &月入出库_入出_f关闭_函;
  return p;
}


static void 月入出库_打开检查_函 (炉_状态机结 *L, const char *函名_缩变量, const char *模块_变量) {
  LStream *p = 月入出库_新文件_函(L);
  p->f = fopen(函名_缩变量, 模块_变量);
  if (配置_l_可能性低_宏名(p->f == NULL))
    月亮状态_错误_函(L, "月编译器_不能_函 打开_圆 file '%s' (%s)", 函名_缩变量, strerror(errno));
}


static int 月入出库_入出_打开_函 (炉_状态机结 *L) {
  const char *文件名_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *模块_变量 = 辅助库_月亮l_可选字符串_宏名(L, 2, "r");
  LStream *p = 月入出库_新文件_函(L);
  const char *元数_缩变量 = 模块_变量;  /* 到_变量 traverse/月解析器_检查_函 模块_变量 */
  辅助库_月亮l_实参检查_宏名(L, 月入出库_l_检查模式_函(元数_缩变量), 2, "invalid 模块_变量");
  p->f = fopen(文件名_变量, 模块_变量);
  return (p->f == NULL) ? 月亮状态_文件结果_函(L, 0, 文件名_变量) : 1;
}


/*
** function 到_变量 关闭_圆 'popen' files
*/
static int 月入出库_入出_保护关闭_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  errno = 0;
  return 月亮状态_执行结果_函(L, 入出库_l_保护关闭_宏名(L, p->f));
}


static int 月入出库_入出_保护打开_函 (炉_状态机结 *L) {
  const char *文件名_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
  const char *模块_变量 = 辅助库_月亮l_可选字符串_宏名(L, 2, "r");
  LStream *p = 月入出库_新预备文件_函(L);
  辅助库_月亮l_实参检查_宏名(L, 入出库_l_检查模式保护_宏名(模块_变量), 2, "invalid 模块_变量");
  p->f = 入出库_l_保护打开_宏名(L, 文件名_变量, 模块_变量);
  p->关闭f_小写 = &月入出库_入出_保护关闭_函;
  return (p->f == NULL) ? 月亮状态_文件结果_函(L, 0, 文件名_变量) : 1;
}


static int 月入出库_入出_临时文件_函 (炉_状态机结 *L) {
  LStream *p = 月入出库_新文件_函(L);
  p->f = tmpfile();
  return (p->f == NULL) ? 月亮状态_文件结果_函(L, 0, NULL) : 1;
}


static FILE *月入出库_获取入出文件_函 (炉_状态机结 *L, const char *findex) {
  LStream *p;
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, findex);
  p = (LStream *)月亮_到用户数据_函(L, -1);
  if (配置_l_可能性低_宏名(入出库_是否已关闭_宏名(p)))
    月亮状态_错误_函(L, "default %s file is closed", findex + 入出库_入出前缀_长度_宏名);
  return p->f;
}


static int 月入出库_g_入出文件_函 (炉_状态机结 *L, const char *f, const char *模块_变量) {
  if (!月头_月亮_是否没有或空值_宏名(L, 1)) {
    const char *文件名_变量 = 月头_月亮_到字符串_宏名(L, 1);
    if (文件名_变量)
      月入出库_打开检查_函(L, 文件名_变量, 模块_变量);
    else {
      月入出库_到文件_函(L);  /* 月解析器_检查_函 that it's a 有效_变量 file handle */
      月亮_推入值_函(L, 1);
    }
    月亮设置字段_函(L, 月头_月亮_注册表索引_宏名, f);
  }
  /* return 当前_圆 值_圆 */
  月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, f);
  return 1;
}


static int 月入出库_入出_输入_函 (炉_状态机结 *L) {
  return 月入出库_g_入出文件_函(L, 入出库_入出_输入_宏名, "r");
}


static int 月入出库_入出_输出_函 (炉_状态机结 *L) {
  return 月入出库_g_入出文件_函(L, 入出库_入出_输出_宏名, "w");
}


static int 月入出库_入出_读行_函 (炉_状态机结 *L);


/*
** maximum number of arguments 到_变量 'f:lines'/'入出_变量.lines' (it + 3 must fit
** in the 限制_变量 for 上值们_小写 of a closure)
*/
#define 入出库_最大实参行_宏名	250

/*
** Auxiliary function 到_变量 create the iteration function for 'lines'.
** The iteration function is a closure over '月入出库_入出_读行_函', with
** the following 上值们_小写:
** 1) The file being read (首先_变量 值_圆 in the 栈_圆小)
** 2) the number of arguments 到_变量 read
** 3) a boolean, true iff file has 到_变量 be closed when finished ('到关闭_变量')
** *) a variable number of 格式_变量 arguments (rest of the 栈_圆小)
*/
static void 月入出库_辅助_行们_函 (炉_状态机结 *L, int 到关闭_变量) {
  int n = 月亮_获取顶_函(L) - 1;  /* number of arguments 到_变量 read */
  辅助库_月亮l_实参检查_宏名(L, n <= 入出库_最大实参行_宏名, 入出库_最大实参行_宏名 + 2, "too many arguments");
  月亮_推入值_函(L, 1);  /* file */
  月亮推入整数_函(L, n);  /* number of arguments 到_变量 read */
  月亮推入布尔值_函(L, 到关闭_变量);  /* 关闭_圆/not 关闭_圆 file when finished */
  月亮_旋转_函(L, 2, 3);  /* move the three values 到_变量 their positions */
  月亮推入C闭包_函(L, 月入出库_入出_读行_函, 3 + n);
}


static int 月入出库_f_行们_函 (炉_状态机结 *L) {
  月入出库_到文件_函(L);  /* 月解析器_检查_函 that it's a 有效_变量 file handle */
  月入出库_辅助_行们_函(L, 0);
  return 1;
}


/*
** Return an iteration function for '入出_变量.lines'. If file has 到_变量 be
** closed, also returns the file itself as a second 结果_变量 (到_变量 be
** closed as the 状态机_变量 at the 出口_变量 of a generic for).
*/
static int 月入出库_入出_行们_函 (炉_状态机结 *L) {
  int 到关闭_变量;
  if (月头_月亮_是否没有_宏名(L, 1)) 月亮推入空值_函(L);  /* at least one argument */
  if (月头_月亮_是否空值_宏名(L, 1)) {  /* no file 名称_变量? */
    月亮获取字段_函(L, 月头_月亮_注册表索引_宏名, 入出库_入出_输入_宏名);  /* get default input */
    月头_月亮_替换_宏名(L, 1);  /* put it at index 1 */
    月入出库_到文件_函(L);  /* 月解析器_检查_函 that it's a 有效_变量 file handle */
    到关闭_变量 = 0;  /* do not 关闭_圆 it after iteration */
  }
  else {  /* 打开_圆 a new file */
    const char *文件名_变量 = 辅助库_月亮l_检查字符串_宏名(L, 1);
    月入出库_打开检查_函(L, 文件名_变量, "r");
    月头_月亮_替换_宏名(L, 1);  /* put file at index 1 */
    到关闭_变量 = 1;  /* 关闭_圆 it after iteration */
  }
  月入出库_辅助_行们_函(L, 到关闭_变量);  /* push iteration function */
  if (到关闭_变量) {
    月亮推入空值_函(L);  /* 状态机_变量 */
    月亮推入空值_函(L);  /* control */
    月亮_推入值_函(L, 1);  /* file is the 到_变量-be-closed variable (4th 结果_变量) */
    return 4;
  }
  else
    return 1;
}


/*
** {======================================================
** READ
** =======================================================
*/


/* maximum length of a numeral */
#if !defined (L_最大长度数大写)
#define L_最大长度数大写     200
#endif


/* auxiliary structure used by '月入出库_读_数目_函' */
typedef struct {
  FILE *f;  /* file being read */
  int c;  /* 当前_圆 character (look ahead) */
  int n;  /* number of elements in 缓冲区_变量 '缓冲_变量' */
  char 缓冲_变量[L_最大长度数大写 + 1];  /* +1 for ending '\0' */
} RN;


/*
** Add 当前_圆 char 到_变量 缓冲区_变量 (if not out of 空间_圆) and read 下一个_变量 one
*/
static int 月入出库_下一个c_函 (RN *寄数_缩变量) {
  if (配置_l_可能性低_宏名(寄数_缩变量->n >= L_最大长度数大写)) {  /* 缓冲区_变量 overflow? */
    寄数_缩变量->缓冲_变量[0] = '\0';  /* invalidate 结果_变量 */
    return 0;  /* fail */
  }
  else {
    寄数_缩变量->缓冲_变量[寄数_缩变量->n++] = 寄数_缩变量->c;  /* 月词法_保存_函 当前_圆 char */
    寄数_缩变量->c = 入出库_l_获取c_宏名(寄数_缩变量->f);  /* read 下一个_变量 one */
    return 1;
  }
}


/*
** Accept 当前_圆 char if it is in 'set' (of 大小_变量 2)
*/
static int 月入出库_测试2_函 (RN *寄数_缩变量, const char *set) {
  if (寄数_缩变量->c == set[0] || 寄数_缩变量->c == set[1])
    return 月入出库_下一个c_函(寄数_缩变量);
  else return 0;
}


/*
** Read a sequence of (十六进制_变量)digits
*/
static int 月入出库_读数字_函 (RN *寄数_缩变量, int 十六进制_变量) {
  int 计数_变量 = 0;
  while ((十六进制_变量 ? isxdigit(寄数_缩变量->c) : isdigit(寄数_缩变量->c)) && 月入出库_下一个c_函(寄数_缩变量))
    计数_变量++;
  return 计数_变量;
}


/*
** Read a number: 首先_变量 reads a 有效_变量 prefix of a numeral into a 缓冲区_变量.
** Then it calls '月亮_字符串到数目_函' 到_变量 月解析器_检查_函 whether the 格式_变量 is
** correct and 到_变量 convert it 到_变量 a Lua number.
*/
static int 月入出库_读_数目_函 (炉_状态机结 *L, FILE *f) {
  RN 寄数_缩变量;
  int 计数_变量 = 0;
  int 十六进制_变量 = 0;
  char 十进制针_变量[2];
  寄数_缩变量.f = f; 寄数_缩变量.n = 0;
  十进制针_变量[0] = 配置_月亮_获取本地化小数点_宏名();  /* get decimal 针点_变量 from locale */
  十进制针_变量[1] = '.';  /* always accept a 点_变量 */
  入出库_l_加锁文件_宏名(寄数_缩变量.f);
  do { 寄数_缩变量.c = 入出库_l_获取c_宏名(寄数_缩变量.f); } while (isspace(寄数_缩变量.c));  /* skip spaces */
  月入出库_测试2_函(&寄数_缩变量, "-+");  /* optional sign */
  if (月入出库_测试2_函(&寄数_缩变量, "00")) {
    if (月入出库_测试2_函(&寄数_缩变量, "xX")) 十六进制_变量 = 1;  /* numeral is hexadecimal */
    else 计数_变量 = 1;  /* 计数_变量 initial '0' as a 有效_变量 数字_变量 */
  }
  计数_变量 += 月入出库_读数字_函(&寄数_缩变量, 十六进制_变量);  /* integral part */
  if (月入出库_测试2_函(&寄数_缩变量, 十进制针_变量))  /* decimal 针点_变量? */
    计数_变量 += 月入出库_读数字_函(&寄数_缩变量, 十六进制_变量);  /* fractional part */
  if (计数_变量 > 0 && 月入出库_测试2_函(&寄数_缩变量, (十六进制_变量 ? "pP" : "eE"))) {  /* exponent 记号_变量? */
    月入出库_测试2_函(&寄数_缩变量, "-+");  /* exponent sign */
    月入出库_读数字_函(&寄数_缩变量, 0);  /* exponent digits */
  }
  ungetc(寄数_缩变量.c, 寄数_缩变量.f);  /* unread look-ahead char */
  入出库_l_解锁文件_宏名(寄数_缩变量.f);
  寄数_缩变量.缓冲_变量[寄数_缩变量.n] = '\0';  /* finish string */
  if (配置_l_可能性高_宏名(月亮_字符串到数目_函(L, 寄数_缩变量.缓冲_变量)))
    return 1;  /* ok, it is a 有效_变量 number */
  else {  /* invalid 格式_变量 */
   月亮推入空值_函(L);  /* "结果_变量" 到_变量 be removed */
   return 0;  /* read fails */
  }
}


static int 月入出库_测试_eof_函 (炉_状态机结 *L, FILE *f) {
  int c = getc(f);
  ungetc(c, f);  /* no-操作_短变量 when c == EOF */
  月头_月亮_推字面_宏名(L, "");
  return (c != EOF);
}


static int 月入出库_读_行_函 (炉_状态机结 *L, FILE *f, int chop) {
  炉L_缓冲区结 b;
  int c;
  月亮状态_缓冲初始的_函(L, &b);
  do {  /* may need 到_变量 read several chunks 到_变量 get whole 行_变量 */
    char *缓冲_变量 = luaL_prepbuffer(&b);  /* preallocate 缓冲区_变量 空间_圆 */
    int i = 0;
    入出库_l_加锁文件_宏名(f);  /* no memory errors can happen inside the lock */
    while (i < 炉L_缓冲区大小 && (c = 入出库_l_获取c_宏名(f)) != EOF && c != '\n')
      缓冲_变量[i++] = c;  /* read 上_小变量 到_变量 终_变量 of 行_变量 or 缓冲区_变量 限制_变量 */
    入出库_l_解锁文件_宏名(f);
    luaL_addsize(&b, i);
  } while (c != EOF && c != '\n');  /* repeat until 终_变量 of 行_变量 */
  if (!chop && c == '\n')  /* want a 新行_小变量 and have one? */
    luaL_addchar(&b, c);  /* add ending 新行_小变量 到_变量 结果_变量 */
  月亮状态_推入结果_函(&b);  /* 关闭_圆 缓冲区_变量 */
  /* return ok if read something (either a 新行_小变量 or something else) */
  return (c == '\n' || 月亮_原始长度_函(L, -1) > 0);
}


static void 月入出库_读_全部_函 (炉_状态机结 *L, FILE *f) {
  size_t 数寄_缩变量;
  炉L_缓冲区结 b;
  月亮状态_缓冲初始的_函(L, &b);
  do {  /* read file in chunks of 炉L_缓冲区大小 bytes */
    char *p = luaL_prepbuffer(&b);
    数寄_缩变量 = fread(p, sizeof(char), 炉L_缓冲区大小, f);
    luaL_addsize(&b, 数寄_缩变量);
  } while (数寄_缩变量 == 炉L_缓冲区大小);
  月亮状态_推入结果_函(&b);  /* 关闭_圆 缓冲区_变量 */
}


static int 月入出库_读_字符_函 (炉_状态机结 *L, FILE *f, size_t n) {
  size_t 数寄_缩变量;  /* number of chars actually read */
  char *p;
  炉L_缓冲区结 b;
  月亮状态_缓冲初始的_函(L, &b);
  p = 月亮状态_预备缓冲区大小_函(&b, n);  /* prepare 缓冲区_变量 到_变量 read whole 月解析器_块_函 */
  数寄_缩变量 = fread(p, sizeof(char), n, f);  /* try 到_变量 read 'n' chars */
  luaL_addsize(&b, 数寄_缩变量);
  月亮状态_推入结果_函(&b);  /* 关闭_圆 缓冲区_变量 */
  return (数寄_缩变量 > 0);  /* true iff read something */
}


static int 月入出库_g_读_函 (炉_状态机结 *L, FILE *f, int 首先_变量) {
  int 实参数们_变量 = 月亮_获取顶_函(L) - 1;
  int n, 成功_变量;
  clearerr(f);
  if (实参数们_变量 == 0) {  /* no arguments? */
    成功_变量 = 月入出库_读_行_函(L, f, 1);
    n = 首先_变量 + 1;  /* 到_变量 return 1 结果_变量 */
  }
  else {
    /* ensure 栈_圆小 空间_圆 for all results and for auxlib's 缓冲区_变量 */
    月亮状态_检查栈_函(L, 实参数们_变量+月头_月亮_最小栈_宏名, "too many arguments");
    成功_变量 = 1;
    for (n = 首先_变量; 实参数们_变量-- && 成功_变量; n++) {
      if (月亮_类型_函(L, n) == 月头_月亮_T数目_宏名) {
        size_t l = (size_t)月亮状态_检查整数_函(L, n);
        成功_变量 = (l == 0) ? 月入出库_测试_eof_函(L, f) : 月入出库_读_字符_函(L, f, l);
      }
      else {
        const char *p = 辅助库_月亮l_检查字符串_宏名(L, n);
        if (*p == '*') p++;  /* skip optional '*' (for compatibility) */
        switch (*p) {
          case 'n':  /* number */
            成功_变量 = 月入出库_读_数目_函(L, f);
            break;
          case 'l':  /* 行_变量 */
            成功_变量 = 月入出库_读_行_函(L, f, 1);
            break;
          case 'L':  /* 行_变量 with 终_变量-of-行_变量 */
            成功_变量 = 月入出库_读_行_函(L, f, 0);
            break;
          case 'a':  /* file */
            月入出库_读_全部_函(L, f);  /* read entire file */
            成功_变量 = 1; /* always 成功_变量 */
            break;
          default:
            return 月亮状态_实参错误_函(L, n, "invalid 格式_变量");
        }
      }
    }
  }
  if (ferror(f))
    return 月亮状态_文件结果_函(L, 0, NULL);
  if (!成功_变量) {
    月头_月亮_弹出_宏名(L, 1);  /* remove 最后_变量 结果_变量 */
    辅助库_月亮l_推失败_宏名(L);  /* push nil instead */
  }
  return n - 首先_变量;
}


static int 月入出库_入出_读_函 (炉_状态机结 *L) {
  return 月入出库_g_读_函(L, 月入出库_获取入出文件_函(L, 入出库_入出_输入_宏名), 1);
}


static int 月入出库_f_读_函 (炉_状态机结 *L) {
  return 月入出库_g_读_函(L, 月入出库_到文件_函(L), 2);
}


/*
** Iteration function for 'lines'.
*/
static int 月入出库_入出_读行_函 (炉_状态机结 *L) {
  LStream *p = (LStream *)月亮_到用户数据_函(L, 月头_月亮_上值索引_宏名(1));
  int i;
  int n = (int)月头_月亮_到整数_宏名(L, 月头_月亮_上值索引_宏名(2));
  if (入出库_是否已关闭_宏名(p))  /* file is already closed? */
    return 月亮状态_错误_函(L, "file is already closed");
  月亮_设置顶_函(L , 1);
  月亮状态_检查栈_函(L, n, "too many arguments");
  for (i = 1; i <= n; i++)  /* push arguments 到_变量 '月入出库_g_读_函' */
    月亮_推入值_函(L, 月头_月亮_上值索引_宏名(3 + i));
  n = 月入出库_g_读_函(L, p->f, 2);  /* 'n' is number of results */
  限制_月亮_断言_宏名(n > 0);  /* should return at least a nil */
  if (月亮_到布尔值_函(L, -n))  /* read at least one 值_圆? */
    return n;  /* return them */
  else {  /* 首先_变量 结果_变量 is false: EOF or 错误_小变量 */
    if (n > 1) {  /* is there 错误_小变量 information? */
      /* 2nd 结果_变量 is 错误_小变量 message */
      return 月亮状态_错误_函(L, "%s", 月头_月亮_到字符串_宏名(L, -n + 1));
    }
    if (月亮_到布尔值_函(L, 月头_月亮_上值索引_宏名(3))) {  /* generator created file? */
      月亮_设置顶_函(L, 0);  /* clear 栈_圆小 */
      月亮_推入值_函(L, 月头_月亮_上值索引_宏名(1));  /* push file at index 1 */
      月入出库_辅助_关闭_函(L);  /* 关闭_圆 it */
    }
    return 0;
  }
}

/* }====================================================== */


static int 月入出库_g_写_函 (炉_状态机结 *L, FILE *f, int 实参_短变量) {
  int 实参数们_变量 = 月亮_获取顶_函(L) - 实参_短变量;
  int 状态码_变量 = 1;
  for (; 实参数们_变量--; 实参_短变量++) {
    if (月亮_类型_函(L, 实参_短变量) == 月头_月亮_T数目_宏名) {
      /* optimization: could be done exactly as for strings */
      int 长度_短变量 = 月亮_是否整数_函(L, 实参_短变量)
                ? fprintf(f, 配置_月亮_整数_格式_宏名,
                             (配置_月亮I_UAC整型_宏名)月头_月亮_到整数_宏名(L, 实参_短变量))
                : fprintf(f, 配置_月亮_数目_格式_宏名,
                             (配置_月亮I_UAC数目_宏名)月头_月亮_到数目_宏名(L, 实参_短变量));
      状态码_变量 = 状态码_变量 && (长度_短变量 > 0);
    }
    else {
      size_t l;
      const char *s = 月亮状态_检查状态字符串_函(L, 实参_短变量, &l);
      状态码_变量 = 状态码_变量 && (fwrite(s, sizeof(char), l, f) == l);
    }
  }
  if (配置_l_可能性高_宏名(状态码_变量))
    return 1;  /* file handle already on 栈_圆小 顶部_变量 */
  else return 月亮状态_文件结果_函(L, 状态码_变量, NULL);
}


static int 月入出库_入出_写_函 (炉_状态机结 *L) {
  return 月入出库_g_写_函(L, 月入出库_获取入出文件_函(L, 入出库_入出_输出_宏名), 1);
}


static int 月入出库_f_写_函 (炉_状态机结 *L) {
  FILE *f = 月入出库_到文件_函(L);
  月亮_推入值_函(L, 1);  /* push file at the 栈_圆小 顶部_变量 (到_变量 be returned) */
  return 月入出库_g_写_函(L, f, 2);
}


static int 月入出库_f_寻求_函 (炉_状态机结 *L) {
  static const int 模块_变量[] = {SEEK_SET, SEEK_CUR, SEEK_END};
  static const char *const 现代名_变量[] = {"set", "cur", "终_变量", NULL};
  FILE *f = 月入出库_到文件_函(L);
  int 操作_短变量 = 月亮状态_检查选项_函(L, 2, "cur", 现代名_变量);
  炉_整数型 p3 = 月亮状态_可选整数_函(L, 3, 0);
  入出库_l_寻求数目_宏名 偏移_变量 = (入出库_l_寻求数目_宏名)p3;
  辅助库_月亮l_实参检查_宏名(L, (炉_整数型)偏移_变量 == p3, 3,
                  "not an integer in proper range");
  操作_短变量 = 入出库_l_文件寻求_宏名(f, 偏移_变量, 模块_变量[操作_短变量]);
  if (配置_l_可能性低_宏名(操作_短变量))
    return 月亮状态_文件结果_函(L, 0, NULL);  /* 错误_小变量 */
  else {
    月亮推入整数_函(L, (炉_整数型)入出库_l_文件告知_宏名(f));
    return 1;
  }
}


static int 月入出库_f_设置v缓冲_函 (炉_状态机结 *L) {
  static const int 模块_变量[] = {_IONBF, _IOFBF, _IOLBF};
  static const char *const 现代名_变量[] = {"no", "full", "行_变量", NULL};
  FILE *f = 月入出库_到文件_函(L);
  int 操作_短变量 = 月亮状态_检查选项_函(L, 2, NULL, 现代名_变量);
  炉_整数型 大小_短变量 = 月亮状态_可选整数_函(L, 3, 炉L_缓冲区大小);
  int 结果_短变量 = setvbuf(f, NULL, 模块_变量[操作_短变量], (size_t)大小_短变量);
  return 月亮状态_文件结果_函(L, 结果_短变量 == 0, NULL);
}



static int 月入出库_入出_刷新_函 (炉_状态机结 *L) {
  return 月亮状态_文件结果_函(L, fflush(月入出库_获取入出文件_函(L, 入出库_入出_输出_宏名)) == 0, NULL);
}


static int 月入出库_f_刷新_函 (炉_状态机结 *L) {
  return 月亮状态_文件结果_函(L, fflush(月入出库_到文件_函(L)) == 0, NULL);
}


/*
** functions for '入出_变量' library
*/
static const 炉L_寄结 入出库_变量[] = {
  {"关闭_圆", 月入出库_入出_关闭_函},
  {"flush", 月入出库_入出_刷新_函},
  {"input", 月入出库_入出_输入_函},
  {"lines", 月入出库_入出_行们_函},
  {"打开_圆", 月入出库_入出_打开_函},
  {"输出_变量", 月入出库_入出_输出_函},
  {"popen", 月入出库_入出_保护打开_函},
  {"read", 月入出库_入出_读_函},
  {"tmpfile", 月入出库_入出_临时文件_函},
  {"type", 月入出库_入出_类型_函},
  {"write", 月入出库_入出_写_函},
  {NULL, NULL}
};


/*
** methods for file handles
*/
static const 炉L_寄结 方法_短变量[] = {
  {"read", 月入出库_f_读_函},
  {"write", 月入出库_f_写_函},
  {"lines", 月入出库_f_行们_函},
  {"flush", 月入出库_f_刷新_函},
  {"seek", 月入出库_f_寻求_函},
  {"关闭_圆", 月入出库_f_关闭_函},
  {"setvbuf", 月入出库_f_设置v缓冲_函},
  {NULL, NULL}
};


/*
** metamethods for file handles
*/
static const 炉L_寄结 元方法_短变量[] = {
  {"__index", NULL},  /* place holder */
  {"__gc", 月入出库_f_垃圾回收_函},
  {"__close", 月入出库_f_垃圾回收_函},
  {"__tostring", 月入出库_f_到字符串_函},
  {NULL, NULL}
};


static void 月入出库_创建元_函 (炉_状态机结 *L) {
  月亮状态_新元表_函(L, 辅助库_月亮_文件手握_宏名);  /* 元表_小写 for file handles */
  月亮状态_设置函们_函(L, 元方法_短变量, 0);  /* add metamethods 到_变量 new 元表_小写 */
  辅助库_月亮l_新库表_宏名(L, 方法_短变量);  /* create method table */
  月亮状态_设置函们_函(L, 方法_短变量, 0);  /* add file methods 到_变量 method table */
  月亮设置字段_函(L, -2, "__index");  /* 元表_小写.__index = method table */
  月头_月亮_弹出_宏名(L, 1);  /* pop 元表_小写 */
}


/*
** function 到_变量 (not) 关闭_圆 the standard files stdin, stdout, and stderr
*/
static int 月入出库_入出_不关闭_函 (炉_状态机结 *L) {
  LStream *p = 入出库_到l流_宏名(L);
  p->关闭f_小写 = &月入出库_入出_不关闭_函;  /* keep file opened */
  辅助库_月亮l_推失败_宏名(L);
  月头_月亮_推字面_宏名(L, "月编译器_不能_函 关闭_圆 standard file");
  return 2;
}


static void 月入出库_创建标准文件_函 (炉_状态机结 *L, FILE *f, const char *k,
                           const char *函名_缩变量) {
  LStream *p = 月入出库_新预备文件_函(L);
  p->f = f;
  p->关闭f_小写 = &月入出库_入出_不关闭_函;
  if (k != NULL) {
    月亮_推入值_函(L, -1);
    月亮设置字段_函(L, 月头_月亮_注册表索引_宏名, k);  /* add file 到_变量 注册表_变量 */
  }
  月亮设置字段_函(L, -2, 函名_缩变量);  /* add file 到_变量 module */
}


配置_月亮模块_应程接_宏名 int 月亮打开输入输出_函 (炉_状态机结 *L) {
  辅助库_月亮l_新库_宏名(L, 入出库_变量);  /* new module */
  月入出库_创建元_函(L);
  /* create (and set) default files */
  月入出库_创建标准文件_函(L, stdin, 入出库_入出_输入_宏名, "stdin");
  月入出库_创建标准文件_函(L, stdout, 入出库_入出_输出_宏名, "stdout");
  月入出库_创建标准文件_函(L, stderr, NULL, "stderr");
  return 1;
}

