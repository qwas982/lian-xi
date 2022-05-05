/*
 * The 失乐园方案编译器
 
 */

#ifndef __失乐园_方案_H__
#define __失乐园_方案_H__

#include <stdint.h>

/* character encodings */
#define 失乐园_印刻_编码_万国码编码8         1
#define 失乐园_印刻_编码_万国码编码16        2
#define 失乐园_印刻_编码_拉丁文1       3

/* 代码 point types */
typedef uint8_t 失乐园_代码点1_类型;
typedef uint16_t 失乐园_代码点2_类型;
typedef uint32_t 失乐园_代码点4_类型;
typedef 失乐园_代码点4_类型 失乐园_印刻_类型;

/* 类型 of small integers */
typedef intptr_t 失乐园_固定整数_类型;

/* the 状态 of the Sly virtual machine */
typedef struct 失乐园_状态_类型 失乐园_状态_类型;

/* 类型 of C functions callable by the VM */
typedef int (*失乐园_c函数_类型)(失乐园_状态_类型* 大失);

/* array of C functions to be registered in the 状态 */
typedef struct 失乐园_寄存_类型 {
  const char *名称;
  失乐园_c函数_类型 函数短;
} 失乐园_寄存_类型;

失乐园_状态_类型* 失乐园_打开(void);
void 失乐园_关闭(失乐园_状态_类型* 大失);

/*
 * shows the top 小数号 elements of the 栈 and
 * then aborts
 */
int 失乐园_错误(失乐园_状态_类型* 大失, uint32_t 小数号);

/*
 * register several functions as 全局复
 * at once
 */
void 失乐园_寄存器(失乐园_状态_类型* 大失, 失乐园_寄存_类型* 寄存复);

/* the input and output ports must be on 栈 */
void 失乐园_读求值印环(失乐园_状态_类型 *大失);

int 失乐园_载入_缓冲区(失乐园_状态_类型* 大失, const uint8_t *缓冲区);

/*
 * object manipulation API
 */

int 失乐园_取_顶部(失乐园_状态_类型* 大失);
void 失乐园_弹(失乐园_状态_类型* 大失, uint32_t 数号短);

/* 类型 predicates */
int 失乐园_谓词盒子(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词印刻(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词整数(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词号码(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词点对(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词列表(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词串(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词向量(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词过程(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词文件终_对象(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词输入_端口(失乐园_状态_类型* 大失, int 索引短);
int 失乐园_谓词输出_端口(失乐园_状态_类型* 大失, int 索引短);

/* push values onto the 栈 */
void 失乐园_取_全局(失乐园_状态_类型* 大失, const char *名称);
void 失乐园_推_值(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_推_布尔(失乐园_状态_类型* 大失, int bool);
void 失乐园_推_印刻(失乐园_状态_类型* 大失, 失乐园_印刻_类型 印刻短);
void 失乐园_推_整数(失乐园_状态_类型* 大失, 失乐园_固定整数_类型 数号短);
void 失乐园_推_c闭包(失乐园_状态_类型* 大失, 失乐园_c函数_类型 函数短, uint32_t 数号_变量);
void 失乐园_推_串(失乐园_状态_类型* 大失, const char* 串短);
void 失乐园_推_向量(失乐园_状态_类型* 大失, uint32_t 大小);

void 失乐园_推_当前_输入_端口(失乐园_状态_类型 *大失);
void 失乐园_推_当前_输出_端口(失乐园_状态_类型 *大失);
void 失乐园_推_当前_错误_端口(失乐园_状态_类型 *大失);

/* get values from the 栈 */
失乐园_固定整数_类型 失乐园_到_整数(失乐园_状态_类型* 大失, int 索引短);

失乐园_印刻_类型* 失乐园_到_串(失乐园_状态_类型* 大失, int 索引短);
失乐园_代码点1_类型* 失乐园_到_串_拉丁文1(失乐园_状态_类型* 大失, int 索引短);
失乐园_代码点1_类型* 失乐园_到_串_万国码编码8(失乐园_状态_类型* 大失, int 索引短);
失乐园_代码点2_类型* 失乐园_到_串_万国码编码16(失乐园_状态_类型* 大失, int 索引短);

/* 对比 values on the 栈 */
int 失乐园_小于_(失乐园_状态_类型* 大失, int 索引1, int 索引2);
int 失乐园_大于_(失乐园_状态_类型* 大失, int 索引1, int 索引2);

/* convert values on 栈 */
void 失乐园_符号_到_串(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_串_到_符号(失乐园_状态_类型* 大失, int 索引短);

/* arithmetic */
void 失乐园_颠倒(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_一元_减号(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_加法(失乐园_状态_类型* 大失, uint32_t nr_numbers);
void 失乐园_减法(失乐园_状态_类型* 大失, uint32_t nr_numbers);
void 失乐园_分号(失乐园_状态_类型* 大失, uint32_t nr_numbers);
void 失乐园_环绕(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_余数(失乐园_状态_类型* 大失, int 索引1, int 索引2);

/* number I/O */
void 失乐园_数号_到_串(失乐园_状态_类型* 大失, int 索引短);

/* boxes */
void 失乐园_盒子_(失乐园_状态_类型* 大失);
void 失乐园_拆盒(失乐园_状态_类型* 大失);
void 失乐园_设置_盒子(失乐园_状态_类型* 大失, int 索引短);

/* lists */
void 失乐园_构造(失乐园_状态_类型* 大失, int 索引1, int 索引2);
void 失乐园_切头(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_切尾(失乐园_状态_类型* 大失, int 索引短);

/* strings */
uint32_t 失乐园_串_长度(失乐园_状态_类型* 大失, int 索引短);
失乐园_印刻_类型 失乐园_串_引用(失乐园_状态_类型* 大失, uint32_t 位置短, int 索引短);
void 失乐园_连接(失乐园_状态_类型* 大失, uint32_t nr_strings);

/* vectors */
uint32_t 失乐园_向量_长度(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_向量_引用(失乐园_状态_类型* 大失, uint32_t 位置短, int 索引短);
void 失乐园_向量_设置(失乐园_状态_类型* 大失, uint32_t 位置短, int 索引短);

/* control */
void 失乐园_应用(失乐园_状态_类型* 大失, int 索引短, uint32_t 数号_实参);

/* evaluation */
void 失乐园_求值(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_调用(失乐园_状态_类型* 大失, uint32_t n_args);
void 失乐园_载入_文件(失乐园_状态_类型* 大失, int 索引短);

/* I/O */
void 失乐园_打开_输入_文件(失乐园_状态_类型* 大失);
void 失乐园_打开_输出_文件(失乐园_状态_类型* 大失);
void 失乐园_关闭_输入_端口(失乐园_状态_类型* 大失);
void 失乐园_关闭_输入_端口(失乐园_状态_类型* 大失);
void 失乐园_新行(失乐园_状态_类型* 大失, int 索引短);
void 失乐园_写(失乐园_状态_类型* 大失, int 索引1, int 索引2);
void 失乐园_显示(失乐园_状态_类型* 大失, int 索引1, int 索引2);

/* sets the current object on top of the 栈 as a 全局 */
void 失乐园_设置_全局(失乐园_状态_类型* 大失, const char* 名称);

#endif
