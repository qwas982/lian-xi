/*
 * The 失乐园方案系统
 
 */

#ifndef __失乐园_状态_H__
#define __失乐园_状态_H__

#include "失乐园.h"
#include "垃圾回收.h"

#define 栈短(计数甲)   (大失->栈[计数甲])
#define 垃圾回收栈(计数甲) (大失->栈[计数甲].值.小垃圾回收)

typedef struct 失乐园_环境_类型 失乐园_环境_类型;
typedef struct 失乐园_环境_变量_类型 失乐园_环境_变量_类型;

/* an 进入 in an environment */
struct 失乐园_环境_变量_类型 {

  /* 进入 in the 符号 table */
  失乐园_符号_类型 *符号;

  /* the actual 值 */
  失乐园_对象_类型 值;
};

/* a 全局 environment */
struct 失乐园_环境_类型 {
  uint32_t 大小;
  失乐园_环境_变量_类型 *变量复;
};

struct 失乐园_状态_类型 {

  /* the 大小 of the bytecode vector used */
  uint32_t 代码_大小;

  /* 栈 allocated 大小 */
  uint32_t 栈_大小;

  /* number of constants */
  uint32_t 数号_常量;

  /* where is the top of the 栈 */
  uint32_t 栈指针;

  /* the frame pointer */
  uint32_t 帧指针;

  /* the program counter */
  uint32_t 程序计数器;

  /* accumulator register */
  失乐园_对象_类型 累加短;

  /* the current procedure */
  失乐园_对象_类型 过程短;

  /* 全局 environment */
  失乐园_环境_类型 全局_环境;

  /* the bytecode to be interpreted */
  uint32_t *代码;

  /* the machine 栈 */
  失乐园_对象_类型 *栈;

  /* constants */
  失乐园_对象_类型 *常量短;

  /* 符号 table */
  失乐园_符号_类型 *符号_表;

  /* VM memory */
  失乐园_储存_类型 储存;
};

void 失乐园_状态_扩大_全局(失乐园_状态_类型* 大失, uint32_t 更多);
int 失乐园_状态_取_全局_索引(失乐园_状态_类型* 大失, 失乐园_符号_类型 *全局);


#endif
