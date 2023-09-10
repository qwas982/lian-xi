/*
** $Id: luac.c $
** Lua compiler (saves bytecodes 到_变量 files; also lists bytecodes)
** See Copyright Notice in lua.h
*/

#define luac_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include "ldebug.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lopnames.h"
#include "lstate.h"
#include "lundump.h"

static void 月编译器_打印函数_函(const 原型_结* f, int full);
#define 编译器_月亮U_打印_宏名	月编译器_打印函数_函

#define 编译器_程序名_宏名	"luac"		/* default program 名称_变量 */
#define 编译器_输出_宏名		编译器_程序名_宏名 ".out"	/* default 输出_变量 file */

static int 列表单_变量=0;			/* 列表_变量 bytecodes? */
static int 转储中_变量=1;			/* dump bytecodes? */
static int 剥离中_变量=0;			/* 剥离_变量 debug information? */
static char 输出_大变量[]={ 编译器_输出_宏名 };	/* default 输出_变量 file 名称_变量 */
static const char* 输出_变量=输出_大变量;	/* 实际上_变量 输出_变量 file 名称_变量 */
static const char* 程序名_变量=编译器_程序名_宏名;	/* 实际上_变量 program 名称_变量 */
static 标签字符串_结 **标方名_变量;

static void 月编译器_致命的_函(const char* message)
{
 fprintf(stderr,"%s: %s\n",程序名_变量,message);
 exit(EXIT_FAILURE);
}

static void 月编译器_不能_函(const char* 什么_变量)
{
 fprintf(stderr,"%s: 月编译器_不能_函 %s %s: %s\n",程序名_变量,什么_变量,输出_变量,strerror(errno));
 exit(EXIT_FAILURE);
}

static void 月编译器_用法_函(const char* message)
{
 if (*message=='-')
  fprintf(stderr,"%s: unrecognized 选项_变量 '%s'\n",程序名_变量,message);
 else
  fprintf(stderr,"%s: %s\n",程序名_变量,message);
 fprintf(stderr,
  "月编译器_用法_函: %s [选项们_变量] [filenames]\n"
  "Available 选项们_变量 are:\n"
  "  -l       列表_变量 (use -l -l for full 列表单_变量)\n"
  "  -o 名称_变量  输出_变量 到_变量 file '名称_变量' (default is \"%s\")\n"
  "  -p       parse only\n"
  "  -s       剥离_变量 debug information\n"
  "  -v       show 版本_变量 information\n"
  "  --       stop handling 选项们_变量\n"
  "  -        stop handling 选项们_变量 and process stdin\n"
  ,程序名_变量,输出_大变量);
 exit(EXIT_FAILURE);
}

#define IS(s)	(strcmp(实参值_变量[i],s)==0)

static int 月编译器_做实参们_函(int 实参计数_变量, char* 实参值_变量[])
{
 int i;
 int 版本_变量=0;
 if (实参值_变量[0]!=NULL && *实参值_变量[0]!=0) 程序名_变量=实参值_变量[0];
 for (i=1; i<实参计数_变量; i++)
 {
  if (*实参值_变量[i]!='-')			/* 终_变量 of 选项们_变量; keep it */
   break;
  else if (IS("--"))			/* 终_变量 of 选项们_变量; skip it */
  {
   ++i;
   if (版本_变量) ++版本_变量;
   break;
  }
  else if (IS("-"))			/* 终_变量 of 选项们_变量; use stdin */
   break;
  else if (IS("-l"))			/* 列表_变量 */
   ++列表单_变量;
  else if (IS("-o"))			/* 输出_变量 file */
  {
   输出_变量=实参值_变量[++i];
   if (输出_变量==NULL || *输出_变量==0 || (*输出_变量=='-' && 输出_变量[1]!=0))
    月编译器_用法_函("'-o' needs argument");
   if (IS("-")) 输出_变量=NULL;
  }
  else if (IS("-p"))			/* parse only */
   转储中_变量=0;
  else if (IS("-s"))			/* 剥离_变量 debug information */
   剥离中_变量=1;
  else if (IS("-v"))			/* show 版本_变量 */
   ++版本_变量;
  else					/* unknown 选项_变量 */
   月编译器_用法_函(实参值_变量[i]);
 }
 if (i==实参计数_变量 && (列表单_变量 || !转储中_变量))
 {
  转储中_变量=0;
  实参值_变量[--i]=输出_大变量;
 }
 if (版本_变量)
 {
  printf("%s\n",月头_月亮_版权_宏名);
  if (版本_变量==实参计数_变量-1) exit(EXIT_SUCCESS);
 }
 return i;
}

#define 编译器_函数_宏名 "(function()终_变量)();\n"

static const char* 读器_圆(炉_状态机结* L, void* 用数_缩变量, size_t* 大小_变量)
{
 限制_未使用_宏名(L);
 if ((*(int*)用数_缩变量)--)
 {
  *大小_变量=sizeof(编译器_函数_宏名)-1;
  return 编译器_函数_宏名;
 }
 else
 {
  *大小_变量=0;
  return NULL;
 }
}

#define 编译器_到原型_宏名(L,i) 对象_获取原型_宏名(对象_s到v_宏名(L->顶部_变量.p+(i)))

static const 原型_结* 月编译器_组合_函(炉_状态机结* L, int n)
{
 if (n==1)
  return 编译器_到原型_宏名(L,-1);
 else
 {
  原型_结* f;
  int i=n;
  if (月亮_加载_函(L,读器_圆,&i,"=(" 编译器_程序名_宏名 ")",NULL)!=LUA_OK) 月编译器_致命的_函(月头_月亮_到字符串_宏名(L,-1));
  f=编译器_到原型_宏名(L,-1);
  for (i=0; i<n; i++)
  {
   f->p[i]=编译器_到原型_宏名(L,i-n-1);
   if (f->p[i]->上值大小_小写>0) f->p[i]->上值们_小写[0].栈内_小写=0;
  }
  return f;
 }
}

static int 写器_小写(炉_状态机结* L, const void* p, size_t 大小_变量, void* u)
{
 限制_未使用_宏名(L);
 return (fwrite(p,大小_变量,1,(FILE*)u)!=1) && (大小_变量!=0);
}

static int 月解释器_保护主函_函(炉_状态机结* L)
{
 int 实参计数_变量=(int)月头_月亮_到整数_宏名(L,1);
 char** 实参值_变量=(char**)月亮_到用户数据_函(L,2);
 const 原型_结* f;
 int i;
 标方名_变量=G(L)->标方名_变量;
 if (!月亮_检查栈空间_函(L,实参计数_变量)) 月编译器_致命的_函("too many input files");
 for (i=0; i<实参计数_变量; i++)
 {
  const char* 文件名_变量=IS("-") ? NULL : 实参值_变量[i];
  if (辅助库_月亮l_载入文件_宏名(L,文件名_变量)!=LUA_OK) 月编译器_致命的_函(月头_月亮_到字符串_宏名(L,-1));
 }
 f=月编译器_组合_函(L,实参计数_变量);
 if (列表单_变量) 编译器_月亮U_打印_宏名(f,列表单_变量>1);
 if (转储中_变量)
 {
  FILE* D= (输出_变量==NULL) ? stdout : fopen(输出_变量,"wb");
  if (D==NULL) 月编译器_不能_函("打开_圆");
  限制_月亮_锁_宏名(L);
  月亮实用工具_转储_函(L,f,写器_小写,D,剥离中_变量);
  限制_月亮_解锁_宏名(L);
  if (ferror(D)) 月编译器_不能_函("write");
  if (fclose(D)) 月编译器_不能_函("关闭_圆");
 }
 return 0;
}

int main(int 实参计数_变量, char* 实参值_变量[])
{
 炉_状态机结* L;
 int i=月编译器_做实参们_函(实参计数_变量,实参值_变量);
 实参计数_变量-=i; 实参值_变量+=i;
 if (实参计数_变量<=0) 月编译器_用法_函("no input files given");
 L=月亮状态_新状态_函();
 if (L==NULL) 月编译器_致命的_函("月编译器_不能_函 create 状态机_变量: not enough memory");
 月头_月亮_推C函数_宏名(L,&月解释器_保护主函_函);
 月亮推入整数_函(L,实参计数_变量);
 月亮推入轻量用户数据_函(L,实参值_变量);
 if (月头_月亮_保护调用_宏名(L,2,0,0)!=LUA_OK) 月编译器_致命的_函(月头_月亮_到字符串_宏名(L,-1));
 月亮_关闭_函(L);
 return EXIT_SUCCESS;
}

/*
** print bytecodes
*/

#define 编译器_上值名_宏名(x) ((f->上值们_小写[x].名称_变量) ? 对象_获取串_宏名(f->上值们_小写[x].名称_变量) : "-")
#define VOID(p) ((const void*)(p))
#define 编译器_事件名_宏名(i) (对象_获取串_宏名(标方名_变量[i]))

static void 月编译器_打印字符串_函(const 标签字符串_结* 类s_变量)
{
 const char* s=对象_获取串_宏名(类s_变量);
 size_t i,n=对象_tss长度_宏名(类s_变量);
 printf("\"");
 for (i=0; i<n; i++)
 {
  int c=(int)(unsigned char)s[i];
  switch (c)
  {
   case '"':
	printf("\\\"");
	break;
   case '\\':
	printf("\\\\");
	break;
   case '\a':
	printf("\\a");
	break;
   case '\b':
	printf("\\b");
	break;
   case '\f':
	printf("\\f");
	break;
   case '\n':
	printf("\\n");
	break;
   case '\r':
	printf("\\r");
	break;
   case '\t':
	printf("\\t");
	break;
   case '\v':
	printf("\\v");
	break;
   default:
	if (isprint(c)) printf("%c",c); else printf("\\%03d",c);
	break;
  }
 }
 printf("\"");
}

static void 月编译器_打印类型_函(const 原型_结* f, int i)
{
 const 标签值_结* o=&f->k[i];
 switch (对象_t类型标签_宏名(o))
 {
  case 对象_月亮_V空值_宏名:
	printf("N");
	break;
  case 对象_月亮_V假_宏名:
  case 对象_月亮_V真_宏名:
	printf("B");
	break;
  case 对象_月亮_V数目浮点_宏名:
	printf("F");
	break;
  case 对象_月亮_V数目整型_宏名:
	printf("I");
	break;
  case 对象_月亮_V短型串_宏名:
  case 对象_月亮_V长型串_宏名:
	printf("S");
	break;
  default:				/* 月编译器_不能_函 happen */
	printf("?%d",对象_t类型标签_宏名(o));
	break;
 }
 printf("\t");
}

static void 月编译器_打印常量_函(const 原型_结* f, int i)
{
 const 标签值_结* o=&f->k[i];
 switch (对象_t类型标签_宏名(o))
 {
  case 对象_月亮_V空值_宏名:
	printf("nil");
	break;
  case 对象_月亮_V假_宏名:
	printf("false");
	break;
  case 对象_月亮_V真_宏名:
	printf("true");
	break;
  case 对象_月亮_V数目浮点_宏名:
	{
	char 缓冲_变量[100];
	sprintf(缓冲_变量,配置_月亮_数目_格式_宏名,对象_浮点值_宏名(o));
	printf("%s",缓冲_变量);
	if (缓冲_变量[strspn(缓冲_变量,"-0123456789")]=='\0') printf(".0");
	break;
	}
  case 对象_月亮_V数目整型_宏名:
	printf(配置_月亮_整数_格式_宏名,对象_整数值_宏名(o));
	break;
  case 对象_月亮_V短型串_宏名:
  case 对象_月亮_V长型串_宏名:
	月编译器_打印字符串_函(对象_ts值_宏名(o));
	break;
  default:				/* 月编译器_不能_函 happen */
	printf("?%d",对象_t类型标签_宏名(o));
	break;
 }
}

#define 编译器_注释_宏名		"\t; "
#define 编译器_额外实参_宏名	操作码_获取实参_Ax_宏名(代码_变量[程序计数_变量+1])
#define 编译器_额外实参计数_宏名	(编译器_额外实参_宏名*(操作码_最大实参_C_宏名+1))
#define 编译器_是否K_宏名		(是种类吗_变量 ? "k" : "")

static void 月编译器_打印代码_函(const 原型_结* f)
{
 const Instruction* 代码_变量=f->代码_变量;
 int 程序计数_变量,n=f->代码大小_小写;
 for (程序计数_变量=0; 程序计数_变量<n; 程序计数_变量++)
 {
  Instruction i=代码_变量[程序计数_变量];
  操作码_枚举 o=操作码_获取_操作码_宏名(i);
  int a=操作码_获取实参_A_宏名(i);
  int b=操作码_获取实参_B_宏名(i);
  int c=操作码_获取实参_C_宏名(i);
  int 辅_变量=操作码_获取实参_Ax_宏名(i);
  int 盒_变量=操作码_获取实参_Bx_宏名(i);
  int 沙盒_变量=操作码_获取实参_sB_宏名(i);
  int 域_变量=操作码_获取实参_sC_宏名(i);
  int 沙盒x_变量=操作码_获取实参_sBx_宏名(i);
  int 是种类吗_变量=操作码_获取实参_k_宏名(i);
  int 行_变量=月亮全局_获取函数行号_函(f,程序计数_变量);
  printf("\t%d\t",程序计数_变量+1);
  if (行_变量>0) printf("[%d]\t",行_变量); else printf("[-]\t");
  printf("%-9s\t",操作名_变量[o]);
  switch (o)
  {
   case 操作_移:
	printf("%d %d",a,b);
	break;
   case 操作_载入整:
	printf("%d %d",a,沙盒x_变量);
	break;
   case 操作_载入浮:
	printf("%d %d",a,沙盒x_变量);
	break;
   case 操作_载入常:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,盒_变量);
	break;
   case 操作_载入常额外:
	printf("%d",a);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,编译器_额外实参_宏名);
	break;
   case 操作_载入假:
	printf("%d",a);
	break;
   case 操作_载入假跳过:
	printf("%d",a);
	break;
   case 操作_载入真:
	printf("%d",a);
	break;
   case 操作_载入空值:
	printf("%d %d",a,b);
	printf(编译器_注释_宏名 "%d out",b+1);
	break;
   case 操作_获取上值:
	printf("%d %d",a,b);
	printf(编译器_注释_宏名 "%s",编译器_上值名_宏名(b));
	break;
   case 操作_设置上值:
	printf("%d %d",a,b);
	printf(编译器_注释_宏名 "%s",编译器_上值名_宏名(b));
	break;
   case 操作_获取表上值内:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名 "%s",编译器_上值名_宏名(b));
	printf(" "); 月编译器_打印常量_函(f,c);
	break;
   case 操作_获取表:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_获取整:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_获取字段:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_设置表上值内:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	printf(编译器_注释_宏名 "%s",编译器_上值名_宏名(a));
	printf(" "); 月编译器_打印常量_函(f,b);
	if (是种类吗_变量) { printf(" "); 月编译器_打印常量_函(f,c); }
	break;
   case 操作_设置表:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	if (是种类吗_变量) { printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c); }
	break;
   case 操作_设置整:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	if (是种类吗_变量) { printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c); }
	break;
   case 操作_设置字段:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,b);
	if (是种类吗_变量) { printf(" "); 月编译器_打印常量_函(f,c); }
	break;
   case 操作_新表:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名 "%d",c+编译器_额外实参计数_宏名);
	break;
   case 操作_自身:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	if (是种类吗_变量) { printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c); }
	break;
   case 操作_加整:
	printf("%d %d %d",a,b,域_变量);
	break;
   case 操作_加常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_减常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_乘常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_取模常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_幂运算常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_除常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_整数除常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_位与常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_位或常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_位异或常:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,c);
	break;
   case 操作_右移整:
	printf("%d %d %d",a,b,域_变量);
	break;
   case 操作_左移整:
	printf("%d %d %d",a,b,域_变量);
	break;
   case 操作_加:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_减:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_乘:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_取模:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_幂运算:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_除:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_整数除:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_位与:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_位或:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_位异或:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_左移:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_右移:
	printf("%d %d %d",a,b,c);
	break;
   case 操作_元方法二元:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名 "%s",编译器_事件名_宏名(c));
	break;
   case 操作_元方法二元整:
	printf("%d %d %d %d",a,沙盒_变量,c,是种类吗_变量);
	printf(编译器_注释_宏名 "%s",编译器_事件名_宏名(c));
	if (是种类吗_变量) printf(" 折叠_变量");
	break;
   case 操作_元方法二元常:
	printf("%d %d %d %d",a,b,c,是种类吗_变量);
	printf(编译器_注释_宏名 "%s ",编译器_事件名_宏名(c)); 月编译器_打印常量_函(f,b);
	if (是种类吗_变量) printf(" 折叠_变量");
	break;
   case 操作_一元减号:
	printf("%d %d",a,b);
	break;
   case 操作_位非:
	printf("%d %d",a,b);
	break;
   case 操作_非:
	printf("%d %d",a,b);
	break;
   case 操作_长度:
	printf("%d %d",a,b);
	break;
   case 操作_拼接:
	printf("%d %d",a,b);
	break;
   case 操作_关闭:
	printf("%d",a);
	break;
   case 操作_待关闭:
	printf("%d",a);
	break;
   case 操作_跳转:
	printf("%d",操作码_获取实参_sJ_宏名(i));
	printf(编译器_注释_宏名 "到_变量 %d",操作码_获取实参_sJ_宏名(i)+程序计数_变量+2);
	break;
   case 操作_相等:
	printf("%d %d %d",a,b,是种类吗_变量);
	break;
   case 操作_小于:
	printf("%d %d %d",a,b,是种类吗_变量);
	break;
   case 操作_小等:
	printf("%d %d %d",a,b,是种类吗_变量);
	break;
   case 操作_相等常:
	printf("%d %d %d",a,b,是种类吗_变量);
	printf(编译器_注释_宏名); 月编译器_打印常量_函(f,b);
	break;
   case 操作_相等整:
	printf("%d %d %d",a,沙盒_变量,是种类吗_变量);
	break;
   case 操作_小于整:
	printf("%d %d %d",a,沙盒_变量,是种类吗_变量);
	break;
   case 操作_小等整:
	printf("%d %d %d",a,沙盒_变量,是种类吗_变量);
	break;
   case 操作_大于整:
	printf("%d %d %d",a,沙盒_变量,是种类吗_变量);
	break;
   case 操作_大等整:
	printf("%d %d %d",a,沙盒_变量,是种类吗_变量);
	break;
   case 操作_测试:
	printf("%d %d",a,是种类吗_变量);
	break;
   case 操作_测试设置:
	printf("%d %d %d",a,b,是种类吗_变量);
	break;
   case 操作_调用:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名);
	if (b==0) printf("all in "); else printf("%d in ",b-1);
	if (c==0) printf("all out"); else printf("%d out",c-1);
	break;
   case 操作_尾调用:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	printf(编译器_注释_宏名 "%d in",b-1);
	break;
   case 操作_返回:
	printf("%d %d %d%s",a,b,c,编译器_是否K_宏名);
	printf(编译器_注释_宏名);
	if (b==0) printf("all out"); else printf("%d out",b-1);
	break;
   case 操作_返回0:
	break;
   case 操作_返回1:
	printf("%d",a);
	break;
   case 操作_为环:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名 "到_变量 %d",程序计数_变量-盒_变量+2);
	break;
   case 操作_为预备:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名 "exit 到_变量 %d",程序计数_变量+盒_变量+3);
	break;
   case 操作_泛型为预备:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名 "到_变量 %d",程序计数_变量+盒_变量+2);
	break;
   case 操作_泛型为调用:
	printf("%d %d",a,c);
	break;
   case 操作_泛型为环:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名 "到_变量 %d",程序计数_变量-盒_变量+2);
	break;
   case 操作_设置列表:
	printf("%d %d %d",a,b,c);
	if (是种类吗_变量) printf(编译器_注释_宏名 "%d",c+编译器_额外实参计数_宏名);
	break;
   case 操作_闭包:
	printf("%d %d",a,盒_变量);
	printf(编译器_注释_宏名 "%p",VOID(f->p[盒_变量]));
	break;
   case 操作_变量实参:
	printf("%d %d",a,c);
	printf(编译器_注释_宏名);
	if (c==0) printf("all out"); else printf("%d out",c-1);
	break;
   case 操作_变量实参预备:
	printf("%d",a);
	break;
   case 操作_额外实参:
	printf("%d",辅_变量);
	break;
#if 0
   default:
	printf("%d %d %d",a,b,c);
	printf(编译器_注释_宏名 "not handled");
	break;
#endif
  }
  printf("\n");
 }
}


#define SS(x)	((x==1)?"":"s")
#define S(x)	(int)(x),SS(x)

static void 月编译器_打印头部_函(const 原型_结* f)
{
 const char* s=f->源_圆 ? 对象_获取串_宏名(f->源_圆) : "=?";
 if (*s=='@' || *s=='=')
  s++;
 else if (*s==月头_月亮_签名_宏名[0])
  s="(bstring)";
 else
  s="(string)";
 printf("\n%s <%s:%d,%d> (%d instruction%s at %p)\n",
	(f->已定义行_小写==0)?"main":"function",s,
	f->已定义行_小写,f->最后已定义行_小写,
	S(f->代码大小_小写),VOID(f));
 printf("%d%s param%s, %d 插槽_变量%s, %d 上值_圆%s, ",
	(int)(f->形参数_小写),f->是否_变量实参短?"+":"",SS(f->形参数_小写),
	S(f->最大栈大小_小写),S(f->上值大小_小写));
 printf("%d local%s, %d constant%s, %d function%s\n",
	S(f->本地变量大小_短),S(f->k大小_缩),S(f->p大小_缩));
}

static void 月编译器_打印调试_函(const 原型_结* f)
{
 int i,n;
 n=f->k大小_缩;
 printf("constants (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t",i);
  月编译器_打印类型_函(f,i);
  月编译器_打印常量_函(f,i);
  printf("\n");
 }
 n=f->本地变量大小_短;
 printf("locals (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t%s\t%d\t%d\n",
  i,对象_获取串_宏名(f->本地变量们_短[i].变量名称_变量),f->本地变量们_短[i].始程计_缩+1,f->本地变量们_短[i].终程计_缩+1);
 }
 n=f->上值大小_小写;
 printf("上值们_小写 (%d) for %p:\n",n,VOID(f));
 for (i=0; i<n; i++)
 {
  printf("\t%d\t%s\t%d\t%d\n",
  i,编译器_上值名_宏名(i),f->上值们_小写[i].栈内_小写,f->上值们_小写[i].索引_缩变量);
 }
}

static void 月编译器_打印函数_函(const 原型_结* f, int full)
{
 int i,n=f->p大小_缩;
 月编译器_打印头部_函(f);
 月编译器_打印代码_函(f);
 if (full) 月编译器_打印调试_函(f);
 for (i=0; i<n; i++) 月编译器_打印函数_函(f->p[i],full);
}
