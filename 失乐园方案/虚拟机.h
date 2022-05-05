/*
 * The 失乐园方案 虚拟机器
 
 */

#ifndef __失乐园_虚拟机_H__
#define __失乐园_虚拟机_H__

#include "失乐园.h"

/* instructions without operands */
#define 失乐园_操作_载入_零                1
#define 失乐园_操作_载入_假              2
#define 失乐园_操作_载入_真               3
#define 失乐园_操作_载入_取消定义              4
#define 失乐园_操作_载入_零零               5
#define 失乐园_操作_载入_一                6
#define 失乐园_操作_推                    7
#define 失乐园_操作_载入_0                  8
#define 失乐园_操作_载入_1                  9
#define 失乐园_操作_载入_2                 10
#define 失乐园_操作_载入_3                 11
#define 失乐园_操作_返回                 12
#define 失乐园_操作_保存_继续短              13
#define 失乐园_操作_重置_继续短              14
#define 失乐园_操作_盒子                    15
#define 失乐园_操作_打开_盒子               16
#define 失乐园_操作_停机                   17
#define 失乐园_操作_中止                  18

#define 失乐园_操作_空指针_问                 40
#define 失乐园_操作_布尔_问                 41
#define 失乐园_操作_印刻_问                 42
#define 失乐园_操作_固定整数_问               43
#define 失乐园_操作_点对_问                 44
#define 失乐园_操作_符号_问               45

/* primitives optimised as instructions */
#define 失乐园_操作_自增                    60
#define 失乐园_操作_自减                    61
#define 失乐园_操作_固定整数_到_印刻         62
#define 失乐园_操作_印刻_到_固定整数         63
#define 失乐园_操作_零零_问                 64
#define 失乐园_操作_非                    65
#define 失乐园_操作_加号                   66
#define 失乐园_操作_减                  67
#define 失乐园_操作_乘                   68
#define 失乐园_操作_构造                   69
#define 失乐园_操作_切头                    70
#define 失乐园_操作_切尾                    71
#define 失乐园_操作_数号_等于                 72
#define 失乐园_操作_等于                     73
#define 失乐园_操作_相等                    74
#define 失乐园_操作_制作_串            75
#define 失乐园_操作_串_设置             76
#define 失乐园_操作_串_到_符号       77
#define 失乐园_操作_制作_向量            78
#define 失乐园_操作_向量_设置             79
#define 失乐园_操作_调试                  80

/* instructions with one operand */
#define 失乐园_操作_载入_固定整数           120
#define 失乐园_操作_载入_印刻             121
#define 失乐园_操作_载入                  122
#define 失乐园_操作_制作_闭包          123
#define 失乐园_操作_跳_若_非            124
#define 失乐园_操作_跳                   125
#define 失乐园_操作_载入_自由             126
#define 失乐园_操作_赋值                127
#define 失乐园_操作_赋值_自由           128
#define 失乐园_操作_帧                 129
#define 失乐园_操作_载入_本地            130
#define 失乐园_操作_插入_盒子            131
#define 失乐园_操作_赋值_本地          132
#define 失乐园_操作_弹                   133
#define 失乐园_操作_全局_引用            134
#define 失乐园_操作_已检查_全局_引用    135
#define 失乐园_操作_全局_设置            136
#define 失乐园_操作_已检查_全局_设置    137
#define 失乐园_操作_常量                 138
#define 失乐园_操作_常量_初始            139
#define 失乐园_操作_元数_等于              140
#define 失乐园_操作_大等              141
#define 失乐园_操作_列出               142
#define 失乐园_操作_调用                  143
#define 失乐园_操作_尾_调用             144

/* address of HALT instruction */
#define 失乐园_停机_地址               0

typedef struct 失乐园_模块_类型 失乐园_模块_类型;

struct 失乐园_模块_类型 {

  /* uninterned 全局复 */
  uint32_t 数号_全局;
  失乐园_串_类型 **全局复;

  /* constants */
  uint32_t 数号_常量;

  /* 代码 */
  uint32_t *代码, 代码_大小;
};

/* initialises the virtual machine */
void 失乐园_虚拟机_初始(失乐园_状态_类型* 大失);

/* dump the 状态 of the VM to standard output */
void 失乐园_虚拟机_转储(失乐园_状态_类型* 大失);

/* performs the call protocol */
void 失乐园_虚拟机_调用(失乐园_状态_类型* 大失, 失乐园_对象_类型 过程短, uint32_t 实参数号);

/* loads compiled 代码 into the vm */
int 失乐园_虚拟机_载入(失乐园_状态_类型* 大失, 失乐园_对象_类型 模块短);

/* deserialises a module */
void 失乐园_虚拟机_向量_到_模块(失乐园_对象_类型 向量短, 失乐园_模块_类型 *模块短);

/* links a module to the running image and executes it */
int 失乐园_虚拟机_链接_跑_模块(失乐园_状态_类型* 大失, 失乐园_模块_类型 *模块短);

#endif
