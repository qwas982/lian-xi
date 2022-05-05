/*
 * The 失乐园方案 垃圾回收
 
 */

#ifndef __失乐园_垃圾回收_H__
#define __失乐园_垃圾回收_H__

#include "失乐园.h"
#include "对象.h"

typedef struct 失乐园_最终对象_类型 失乐园_最终对象_类型;
typedef struct 失乐园_储存_类型 失乐园_储存_类型;

/*
 * this callback is called by the garbage collector
 * to get all the mutator roots. It must return NULL
 * when there are no 更多 roots to 扫描
 */
typedef 失乐园_对象_类型* (*失乐园_根_回调_类型)(void*);

/*
 * this is used to create a chain of links to
 * objects that can be finalised
 */
struct 失乐园_最终对象_类型 {

  /* object that may need finalisation */
  失乐园_垃圾回收对象_类型 *对象短;

  /* pointer to 下一个 object in chain */
  失乐园_最终对象_类型 *下一个;
};

struct 失乐园_储存_类型 {
  
  /* the total 大小 of a semispace */
  size_t 容量;

  /* the used 大小 of the semispace */
  size_t 大小;

  /* address of memory received from OS */
  void *操系_地址;

  /* the space from where objects are copied */
  void *来自_空间;

  /* the space to which objects are copied */
  void *到_空间;

  /* list of objects that may need finalisation */
  失乐园_最终对象_类型 *最终对象复;

  /* roots callback */
  失乐园_根_回调_类型 根_回调;

  /* opaque 数据 to pass to callback */
  void *根_回调_数据;
};

int  失乐园_垃圾回收_初始(失乐园_储存_类型 *大失, 失乐园_根_回调_类型 小回调, void* 小用户数据);
void 失乐园_垃圾回收_完成(失乐园_储存_类型 *大失);

void*         失乐园_垃圾回收_分配(失乐园_储存_类型 *大失, size_t 大小);
void          失乐园_垃圾回收_添加_端口(失乐园_储存_类型 *大失, 失乐园_端口_类型 *端口);

#endif
