/*
 * The 失乐园方案系统
 
 */

#ifndef __失乐园_对象_H__
#define __失乐园_对象_H__

#include <stdio.h>

#include "失乐园.h"

/*
 * 数据 types tags
 */
#define 失乐园_类型_空             1
#define 失乐园_类型_取消定义            2
#define 失乐园_类型_零              3
#define 失乐园_类型_文件终              4
#define 失乐园_类型_布尔             5
#define 失乐园_类型_固定整数           6
#define 失乐园_类型_印刻             7
#define 失乐园_类型_符号           8
#define 失乐园_类型_闭包          9
#define 失乐园_类型_点对            10
#define 失乐园_类型_继续短           11
#define 失乐园_类型_盒子             12
#define 失乐园_类型_串          13
#define 失乐园_类型_向量          14
#define 失乐园_类型_动态_绑定        15
#define 失乐园_类型_句法闭包          16
#define 失乐园_类型_输入_端口      17
#define 失乐园_类型_输出_端口     18

/* sizes for GC */
#define 失乐园_大小_的_盒子                         \
  (sizeof(失乐园_盒子_类型))
#define 失乐园_大小_的_点对                        \
  (sizeof(失乐园_点对_类型))
#define 失乐园_大小_的_闭包(n)                          \
  (sizeof(失乐园_闭包_类型) + (n) * sizeof(失乐园_对象_类型))
#define 失乐园_大小_的_继续短(n)                            \
  (sizeof(失乐园_继续短_类型) + (n) * sizeof(失乐园_对象_类型))
#if defined(__LP64__) || defined(__LLP64__)
#define 失乐园_大小_的_串(n)                           \
  (sizeof(失乐园_串_类型) + (n % 2 ? n+1 : n) * sizeof(失乐园_印刻_类型))
#else
#define 失乐园_大小_的_串(n)                           \
  (sizeof(失乐园_串_类型) + (n) * sizeof(失乐园_印刻_类型))
#endif
#define 失乐园_大小_的_向量(n)                           \
  (sizeof(失乐园_向量_类型) + (n) * sizeof(失乐园_对象_类型))
#define 失乐园_大小_的_动态_绑定                    \
  (sizeof(失乐园_动态_绑定_类型))
#define 失乐园_大小_的_句法闭包                      \
  (sizeof(失乐园_句法_闭包_类型))
#define 失乐园_大小_的_入端口                       \
  (sizeof(失乐园_入端口_类型))
#define 失乐园_大小_的_出端口                       \
  (sizeof(失乐园_出端口_类型))

/* 端口 types */
#define 失乐园_类型_端口_文件          1

/* 端口 缓冲区 大小 */
#define 失乐园_端口_缓冲_大小         512

/* 文件 descriptors */
#ifdef _WIN32
typedef HANDLE 失乐园_文件_类型;
#else
typedef int 失乐园_文件_类型;
#endif

/* forward 类型 declarations */
typedef struct 失乐园_对象_类型 失乐园_对象_类型;
typedef struct 失乐园_垃圾回收对象_类型 失乐园_垃圾回收对象_类型;

typedef struct 失乐园_盒子_类型 失乐园_盒子_类型;
typedef struct 失乐园_闭包_类型 失乐园_闭包_类型;
typedef struct 失乐园_点对_类型 失乐园_点对_类型;
typedef struct 失乐园_继续短_类型 失乐园_继续短_类型;
typedef struct 失乐园_串_类型 失乐园_串_类型;
typedef struct 失乐园_向量_类型 失乐园_向量_类型;
typedef struct 失乐园_动态_绑定_类型 失乐园_动态_绑定_类型;
typedef struct 失乐园_句法_闭包_类型 失乐园_句法_闭包_类型;

typedef struct 失乐园_符号_类型 失乐园_符号_类型;

typedef struct 失乐园_端口_类型      失乐园_端口_类型;
typedef struct 失乐园_入端口_类型     失乐园_入端口_类型;
typedef struct 失乐园_出端口_类型     失乐园_出端口_类型;

/* casts */
#define 失乐园_垃圾回收对象(对象短)      ((失乐园_垃圾回收对象_类型*)(对象短))
#define 失乐园_盒子(对象短)           ((失乐园_盒子_类型*)(对象短))
#define 失乐园_闭包(对象短)       ((失乐园_闭包_类型*)(对象短))
#define 失乐园_点对(对象短)          ((失乐园_点对_类型*)(对象短))
#define 失乐园_继续短(对象短)         ((失乐园_继续短_类型*)(对象短))
#define 失乐园_串(对象短)        ((失乐园_串_类型*)(对象短))
#define 失乐园_向量(对象短)        ((失乐园_向量_类型*)(对象短))
#define 失乐园_动态_绑定(对象短)      ((失乐园_动态_绑定_类型*)(对象短))
#define 失乐园_句法闭包(对象短)        ((失乐园_句法_闭包_类型*)(对象短))
#define 失乐园_端口(对象短)          ((失乐园_端口_类型*)(对象短))
#define 失乐园_入端口(对象短)         ((失乐园_入端口_类型*)(对象短))
#define 失乐园_出端口(对象短)         ((失乐园_出端口_类型*)(对象短))

#define 失乐园_对象_等号(小对象1, 小对象2)                                              \
  (((小对象1).类型 == (小对象2).类型) && ((小对象1).值.符号 == (小对象2).值.符号))

/* 值 types */
struct 失乐园_对象_类型 {
  /* the runtime 类型 标签 */
  uint8_t 类型;

  /* the 值 of this object */
  union {
    /* immediates */
    uint8_t bool;
    失乐园_印刻_类型 印刻短;
    失乐园_固定整数_类型 固定整数;
    失乐园_符号_类型 *符号;

    /* collectable objects */
    失乐园_垃圾回收对象_类型 *小垃圾回收;
  } 值;
};

struct 失乐园_垃圾回收对象_类型 {
  uint32_t 类型;
};

struct 失乐园_盒子_类型 {
  失乐园_垃圾回收对象_类型 基础;
  失乐园_对象_类型 值;
};

struct 失乐园_闭包_类型 {
  失乐园_垃圾回收对象_类型 基础;
  uint8_t 是否_小闭包;
  union {
    uint32_t scm;
    失乐园_c函数_类型 c;
  } 进入_点;
  uint32_t 数号_自由;
  失乐园_对象_类型 自由_变量[0];
};

struct 失乐园_点对_类型 {
  失乐园_垃圾回收对象_类型 基础;
  失乐园_对象_类型 切头;
  失乐园_对象_类型 切尾;  
};

struct 失乐园_继续短_类型 {
  失乐园_垃圾回收对象_类型 基础;
  size_t 大小;
  失乐园_对象_类型 栈[0];
};

struct 失乐园_串_类型 {
  失乐园_垃圾回收对象_类型 基础;
  size_t 大小;
  失乐园_印刻_类型 印刻复[0];
};

struct 失乐园_向量_类型 {
  失乐园_垃圾回收对象_类型 基础;
  size_t 大小;
  失乐园_对象_类型 数据[0];
};

struct 失乐园_动态_绑定_类型 {
  失乐园_垃圾回收对象_类型 基础;
  失乐园_对象_类型 标签;
  失乐园_对象_类型 值;
};

struct 失乐园_句法_闭包_类型 {
  失乐园_垃圾回收对象_类型 基础;
  失乐园_对象_类型 环境短;
  失乐园_对象_类型 自由_;
  失乐园_对象_类型 表达式短;
};

/* I/O 端口 "classes" */
struct 失乐园_端口_类型 {
  失乐园_垃圾回收对象_类型 基础;

  /* 端口 类型 */
  uint8_t 类型;

  /* character encoding of this 端口 */
  uint8_t 印刻_编码;

  /* finalisation function */
  int (*完成)(失乐园_端口_类型 *自己);

  /* private 端口 数据 */
  void *private;
};

struct 失乐园_入端口_类型 {
  失乐园_端口_类型 基础;

  int (*窥_印刻)(失乐园_入端口_类型 *自己, 失乐园_印刻_类型 *c);
  int (*读_印刻)(失乐园_入端口_类型 *自己, 失乐园_印刻_类型 *c);
};

struct 失乐园_出端口_类型 {
  失乐园_端口_类型 基础;

  int (*刷新)(失乐园_出端口_类型* 自己);
  int (*写_印刻)(失乐园_出端口_类型* 自己, 失乐园_印刻_类型 c);
};



/*
 * 进入 in the 符号 table
 * symbols are not collected
 * some hash functions:
 * http://burtleburtle.net/bob/c/lookup3.c
 */
struct 失乐园_符号_类型 {
  /* 符号 textual representation */
  失乐园_串_类型 *串短;

  /* 下一个 符号 in chain */
  失乐园_符号_类型 *下一个;
};

/*
 * object creation
 */


失乐园_垃圾回收对象_类型 *失乐园_创建_盒子(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_创建_失闭包(失乐园_状态_类型 *大失, uint32_t 进入, uint32_t 数号_变量);
失乐园_垃圾回收对象_类型 *失乐园_创建_c闭包(失乐园_状态_类型 *大失, 失乐园_c函数_类型 函数短, uint32_t 数号_变量);
失乐园_垃圾回收对象_类型 *失乐园_创建_点对(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_创建_继续短(失乐园_状态_类型 *大失, uint32_t 栈_大小);
失乐园_垃圾回收对象_类型 *失乐园_创建_串(失乐园_状态_类型 *大失, const 失乐园_印刻_类型* 串短, uint32_t 大小);
失乐园_垃圾回收对象_类型 *失乐园_创建_串_来自_美信交标码(失乐园_状态_类型 *大失, const char* 串短);
失乐园_垃圾回收对象_类型 *失乐园_创建_向量(失乐园_状态_类型 *大失, uint32_t 大小);
失乐园_垃圾回收对象_类型 *失乐园_创建_动态_绑定(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_创建_句法_闭包(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_创建_入端口(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_创建_出端口(失乐园_状态_类型 *大失);

失乐园_对象_类型 失乐园_创建_符号(失乐园_状态_类型* 大失, 失乐园_串_类型* 串短);
失乐园_对象_类型 失乐园_创建_符号_来自_美信交标码(失乐园_状态_类型* 大失, const char* 名称);

#endif
