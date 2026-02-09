/**
 * 类型定义.h
 * 基础类型别名定义，确保跨平台兼容性
 * C89标准 + 固定宽度整数类型
 */

#ifndef 类型定义_H
#define 类型定义_H

#include <stddef.h>

/* ==================== 固定宽度整数类型 ==================== */
/* 使用标准C99的stdint.h，如果不支持则手动定义 */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #include <stdint.h>
#else
    /* 手动定义固定宽度类型 */
    typedef unsigned char       uint8_t;
    typedef signed char         int8_t;
    typedef unsigned short      uint16_t;
    typedef short               int16_t;
    typedef unsigned int        uint32_t;
    typedef int                 int32_t;
    
    #if defined(_MSC_VER)
        typedef unsigned __int64    uint64_t;
        typedef __int64             int64_t;
    #else
        typedef unsigned long long  uint64_t;
        typedef long long           int64_t;
    #endif
    
    typedef uint32_t            uintptr_t;
    typedef int32_t             intptr_t;
#endif

/* ==================== 布尔类型 ==================== */
#ifndef 布尔类型定义
#define 布尔类型定义
    #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
        #include <stdbool.h>
    #else
        typedef int 布尔型;
        #ifndef 真
            #define 真 1
        #endif
        #ifndef 假
            #define 假 0
        #endif
    #endif
#endif

/* ==================== 字节类型 ==================== */
typedef uint8_t 字节型;
typedef uint8_t* 字节指针;

/* ==================== 值类型联合体 ==================== */
/* 栈值可以存储整数或浮点数 */
union 联合体_值 {
    int32_t  整数32;
    int64_t  整数64;
    uint32_t 无符号32;
    uint64_t 无符号64;
    float    单精度浮点;
    double   双精度浮点;
};

typedef union 联合体_值 联合体_值;

/* ==================== 结果类型 ==================== */
/* 函数返回结果，包含错误码 */
struct 结构_结果 {
    int 错误码;         /* 使用枚举_错误码 */
    int 整数值;         /* 可选的整数返回值 */
};

typedef struct 结构_结果 结构_结果;

/* ==================== 常用常量 ==================== */
#define 掩码32位    0xFFFFFFFFU
#define 掩码64位    0xFFFFFFFFFFFFFFFFULL

#define 最大_INT32  0x7FFFFFFF
#define 最小_INT32  (-0x7FFFFFFF - 1)
#define 最大_UINT32 0xFFFFFFFFU

#define 最大_INT64  0x7FFFFFFFFFFFFFFFLL
#define 最小_INT64  (-0x7FFFFFFFFFFFFFFFLL - 1)
#define 最大_UINT64 0xFFFFFFFFFFFFFFFFULL

/* ==================== 内存页常量 ==================== */
#define 每页字节数      65536U      /* 64KB */
#define 最大内存页数    65536U      /* 4GB */
#define 默认内存页数    1

/* ==================== 栈大小常量 ==================== */
#define 默认操作数栈大小    65536
#define 默认调用栈大小      4096
#define 默认块栈大小        4096
#define 默认分支表大小      65536

/* ==================== 字符串长度限制 ==================== */
#define 最大名称长度        256
#define 最大导出项数        1024
#define 最大函数数          65536
#define 最大类型数          65536
#define 最大全局变量数      1024
#define 最大数据段数        1024
#define 最大元素段数        1024

#endif /* 类型定义_H */
