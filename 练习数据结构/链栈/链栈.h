#ifndef __栈__
#define __栈__

#define T 栈T
typedef struct T *T;

extern T 栈初始化 (void);
extern int 栈大小 (T 栈);
extern int 栈空 (T 栈);
extern void 栈推 (T 栈,void *值);
extern void *栈弹 (T 栈);
extern void 栈打印 (T 栈);

#undef T
#endif