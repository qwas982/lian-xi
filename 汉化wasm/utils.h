#ifndef WASMC_UTILS_H
#define WASMC_UTILS_H

#include "module.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint64_t 无整64;
typedef int64_t 整数64;
typedef uint32_t 无整32;
typedef int32_t 整数32;
typedef double 浮点64位;
typedef float 浮点32位;

// 用于保存异常信息内容
extern char 异常[];

// 报错
#define 致命错(...)                                             \
    {                                                          \
        fprintf(stderr, "错误(%s:%d): ", __FILE__, __LINE__); \
        fprintf(stderr, __VA_ARGS__);                          \
        exit(1);                                               \
    }

// 断言
#define 断言(表达式_, ...)                                                    \
    {                                                                       \
        if (!(表达式_)) {                                                       \
            fprintf(stderr, "断言失败 (%s:%d): ", __FILE__, __LINE__); \
            fprintf(stderr, __VA_ARGS__);                                   \
            exit(1);                                                        \
        }                                                                   \
    }

#define 错误(...) fprintf(stderr, __VA_ARGS__);

// 解码针对无符号整数的 LEB128 编码
uint64_t 读_基于小端_无符号(const uint8_t *字节们, uint32_t *位置, uint32_t 最大位们);

// 解码针对有符号整数的 LEB128 编码
uint64_t 读_基于小端_有符号(const uint8_t *字节们, uint32_t *位置, uint32_t 最大位们);

// 从字节数组中读取字符串，其中字节数组的开头 4 个字节用于表示字符串的长度
// 注：如果参数 结果_长 不为 NULL，则会被赋值为字符串的长度
char *读_字符串(const uint8_t *字节们, uint32_t *位置, uint32_t *结果_长);

// 申请内存
void *分配内存配给(size_t nmemb, size_t 大小, char *名称);

// 在原有内存基础上重新申请内存
void *重新分配内存配给(void *指针, size_t old_nmemb, size_t nmemb, size_t 大小, char *名称);

// 查找动态库中的 符号
// 如果解析成功则返回 true
// 如果解析失败则返回 false 并设置 错也
bool 解决_符号(char *文件名, char *符号, void **值也, char **错也);

// 基于函数签名计算唯一的掩码值
uint64_t 获取_类型_面罩(签名类型 *类型);

// 根据表示该控制块的类型的值（占一个字节），返回控制块的类型（或签名），即控制块的返回值的数量和类型
// 0x7f 表示有一个 整数32 类型返回值、0x7e 表示有一个 整数64 类型返回值、0x7d 表示有一个 浮点32位 类型返回值、0x7c 表示有一个 浮点64位 类型返回值、0x40 表示没有返回值
// 注：目前多返回值提案还没有进入 Wasm 标准，根据当前版本的 Wasm 标准，控制块不能有参数，且最多只能有一个返回值
签名类型 *获取_块_类型(uint8_t 值_类型);

// 符号扩展 (sign extension)
// 分以下两种情况：
// 1. 将无符号数转换为更大的数据类型：
// 只需简单地在开头添加 0 至所需位数，这种运算称为 0 扩展
// 2. 将有符号数转换为更大的数据类型：
// 需要执行符号扩展，规则是将符号位扩展至所需的位数，即符号位为 0 时在开头添加 0 至所需位数，符号位为 1 时在开头添加 1 至所需位数，
// 例如 char: 1000 0000  --> short: 1111 1111 1000 0000

void 符号扩展_8_32(uint32_t *值也);

void 符号扩展_16_32(uint32_t *值也);

void 符号扩展_8_64(uint64_t *值也);

void 符号扩展_16_64(uint64_t *值也);

void 符号扩展_32_64(uint64_t *值也);

// 32 位整型循环左移
uint32_t 左旋转32位(uint32_t n, unsigned int c);

// 32 位整型循环右移
uint32_t 右旋转32位(uint32_t n, unsigned int c);

// 64 位整型循环左移
uint64_t 左旋转64位(uint64_t n, unsigned int c);

// 64 位整型循环右移
uint64_t 右旋转64位(uint64_t n, unsigned int c);

// 32 位浮点型比较两数之间最大值
float 网汇_32位浮点最大(float a, float b);

// 32 位浮点型比较两数之间最小值
float 网汇_32位浮点最小(float a, float b);

// 64 位浮点型比较两数之间最大值
double 网汇_64位浮点最大(double a, double b);

// 64 位浮点型比较两数之间最小值
double 网汇_64位浮点最小(double a, double b);

// 非饱和截断
#define 操作_截断(RES, A, TYPE, RMIN, RMAX)                   \
    if (isnan(A)) {                                          \
        sprintf(异常, "无效变换到整数"); \
        return false;                                        \
    }                                                        \
    if ((A) <= (RMIN) || (A) >= (RMAX)) {                    \
        sprintf(异常, "整数溢出");              \
        return false;                                        \
    }                                                        \
    (RES) = (TYPE) (A);

#define OP_I32_TRUNC_F32(RES, A) 操作_截断(RES, A, 整数32, -2147483904.0f, 2147483648.0f)
#define OP_U32_TRUNC_F32(RES, A) 操作_截断(RES, A, 无整32, -1.0f, 4294967296.0f)
#define OP_I32_TRUNC_F64(RES, A) 操作_截断(RES, A, 整数32, -2147483649.0, 2147483648.0)
#define OP_U32_TRUNC_F64(RES, A) 操作_截断(RES, A, 无整32, -1.0, 4294967296.0)

#define OP_I64_TRUNC_F32(RES, A) 操作_截断(RES, A, 整数64, -9223373136366403584.0f, 9223372036854775808.0f)
#define OP_U64_TRUNC_F32(RES, A) 操作_截断(RES, A, 无整64, -1.0f, 18446744073709551616.0f)
#define OP_I64_TRUNC_F64(RES, A) 操作_截断(RES, A, 整数64, -9223372036854777856.0, 9223372036854775808.0)
#define OP_U64_TRUNC_F64(RES, A) 操作_截断(RES, A, 无整64, -1.0, 18446744073709551616.0)

// 饱和截断
// 注：和非饱和截断的区别在于，饱和截断会对异常情况做特殊处理。
// 比如将 NaN 转换为 0。再比如如果超出了类型能表达的范围，让该变量等于一个最大值或者最小值。
#define 操作_截断_饱和(RES, A, TYPE, RMIN, RMAX, IMIN, IMAX) \
    if (isnan(A)) {                                        \
        (RES) = 0;                                         \
    } else if ((A) <= (RMIN)) {                            \
        (RES) = IMIN;                                      \
    } else if ((A) >= (RMAX)) {                            \
        (RES) = IMAX;                                      \
    } else {                                               \
        (RES) = (TYPE) (A);                                \
    }

#define OP_I32_TRUNC_SAT_F32(RES, A) 操作_截断_饱和(RES, A, 整数32, -2147483904.0f, 2147483648.0f, INT32_MIN, INT32_MAX)
#define OP_U32_TRUNC_SAT_F32(RES, A) 操作_截断_饱和(RES, A, 无整32, -1.0f, 4294967296.0f, 0UL, UINT32_MAX)
#define OP_I32_TRUNC_SAT_F64(RES, A) 操作_截断_饱和(RES, A, 整数32, -2147483649.0, 2147483648.0, INT32_MIN, INT32_MAX)
#define OP_U32_TRUNC_SAT_F64(RES, A) 操作_截断_饱和(RES, A, 无整32, -1.0, 4294967296.0, 0UL, UINT32_MAX)

#define OP_I64_TRUNC_SAT_F32(RES, A) 操作_截断_饱和(RES, A, 整数64, -9223373136366403584.0f, 9223372036854775808.0f, INT64_MIN, INT64_MAX)
#define OP_U64_TRUNC_SAT_F32(RES, A) 操作_截断_饱和(RES, A, 无整64, -1.0f, 18446744073709551616.0f, 0ULL, UINT64_MAX)
#define OP_I64_TRUNC_SAT_F64(RES, A) 操作_截断_饱和(RES, A, 整数64, -9223372036854777856.0, 9223372036854775808.0, INT64_MIN, INT64_MAX)
#define OP_U64_TRUNC_SAT_F64(RES, A) 操作_截断_饱和(RES, A, 无整64, -1.0, 18446744073709551616.0, 0ULL, UINT64_MAX)

// 将 栈值 类型数值用字符串形式展示，展示形式 "<值>:<值_类型>"
char *值_代表(栈值 *v);

// 通过名称从 Wasm 模块中查找同名的导出项
void *获取_导出(网汇模块 *m, char *名称);

// 打开文件并将文件映射进内存
uint8_t *模映射_文件(char *路径, int *长度);

// 将字符串 串 按照空格拆分成多个参数
// 其中 实参计数 被赋值为拆分字符串 串 得到的参数数量
char **拆分_实参值(char *串, int *实参计数);

// 解析函数参数，并将参数压入到操作数栈
void 解析_实参们(网汇模块 *m, 签名类型 *类型, int 实参计数, char **实参值);

#endif
