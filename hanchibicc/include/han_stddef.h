//han_stddef.h

#ifndef __STDDEF_H  // 如果未定义__STDDEF_H宏
#define __STDDEF_H  // 定义__STDDEF_H宏

#define NULL ((void *)0)  // 将NULL定义为指向void类型的指针，其值为0

typedef unsigned long size_t;  // 定义size_t类型为无符号长整型
typedef long ptrdiff_t;        // 定义ptrdiff_t类型为长整型
typedef unsigned int wchar_t;  // 定义wchar_t类型为无符号整型
typedef long max_align_t;      // 定义max_align_t类型为长整型

#define offsetof(type, member) ((size_t)&(((type *)0)->member))  // 定义offsetof宏，用于计算结构体成员相对于结构体起始地址的偏移量

#endif  // 结束条件编译指令