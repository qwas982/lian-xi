#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

#include <stdio.h>
#include <stdlib.h>

#define 调试_追踪_大写                    1
//#define 调试_对象_生命时间_大写         1
//#define 调试_环境_生命时间_大写短            1

#define 调试_追踪_文件_大写    stderr

#define 空操作_大写    do { } while (false)
#define 不追踪_大写(...)    空操作_大写

#if 调试_追踪_大写
    #define 追踪_大写(...) fprintf(调试_追踪_文件_大写, __VA_ARGS__)
#else
    #define 追踪_大写 不追踪_大写
#endif

#if 调试_对象_生命时间_大写
    #define 追踪_对象_大写 追踪_大写
#else
    #define 追踪_对象_大写 不追踪_大写
#endif

#if 调试_环境_生命时间_大写短
    #define 追踪_环境_大写短 追踪_大写
#else
    #define 追踪_环境_大写短 不追踪_大写
#endif

#define _断言_大写(文件_小写, 行_小写, 条件_小写, ...) \
    if (!(条件_小写)) { \
        printf("断言失败在位置 %s(%d): ", 文件_小写, 行_小写); \
        printf(__VA_ARGS__); \
        exit(1); \
    } else { }


#define 断言_大写(条件_小写, ...) \
    _断言_大写(__FILE__, __LINE__, 条件_小写, __VA_ARGS__)

#endif // INCLUDE_DEBUG_H
