//han_float.h

#ifndef __STDFLOAT_H  // 如果__STDFLOAT_H宏未定义，则编译以下内容
#define __STDFLOAT_H  // 定义__STDFLOAT_H宏，防止重复包含

#define DECIMAL_DIG 21  // 十进制数字的最大位数（包括小数点和指数）
#define FLT_EVAL_METHOD 0  // 浮点类型的求值方法（C11标准5.2.4.2.2p9）
#define FLT_RADIX 2  // 浮点数的基数
#define FLT_ROUNDS 1  // 浮点数舍入模式（C11标准5.2.4.2.2p8：最接近模式）

#define FLT_DIG 6  // 单精度浮点数的十进制数字个数
#define FLT_EPSILON 0x1p-23  // 单精度浮点数的最小精度
#define FLT_MANT_DIG 24  // 单精度浮点数尾数的二进制位数
#define FLT_MAX 0x1.fffffep+127  // 单精度浮点数的最大值
#define FLT_MAX_10_EXP 38  // 单精度浮点数最大的十进制指数
#define FLT_MAX_EXP 128  // 单精度浮点数的最大指数
#define FLT_MIN 0x1p-126  // 单精度浮点数的最小正值
#define FLT_MIN_10_EXP -37  // 单精度浮点数最小的十进制指数
#define FLT_MIN_EXP -125  // 单精度浮点数的最小指数
#define FLT_TRUE_MIN 0x1p-149  // 单精度浮点数的最小非零正值

#define DBL_DIG 15  // 双精度浮点数的十进制数字个数
#define DBL_EPSILON 0x1p-52  // 双精度浮点数的最小精度
#define DBL_MANT_DIG 53  // 双精度浮点数尾数的二进制位数
#define DBL_MAX 0x1.fffffffffffffp+1023  // 双精度浮点数的最大值
#define DBL_MAX_10_EXP 308  // 双精度浮点数最大的十进制指数
#define DBL_MAX_EXP 1024  // 双精度浮点数的最大指数
#define DBL_MIN 0x1p-1022  // 双精度浮点数的最小正值
#define DBL_MIN_10_EXP -307  // 双精度浮点数最小的十进制指数
#define DBL_MIN_EXP -1021  // 双精度浮点数的最小指数
#define DBL_TRUE_MIN 0x0.0000000000001p-1022  // 双精度浮点数的最小非零正值

#define LDBL_DIG 15  // 扩展双精度浮点数的十进制数字个数
#define LDBL_EPSILON 0x1p-52  // 扩展双精度浮点数的最小精度
#define LDBL_MANT_DIG 53  // 扩展双精度浮点数尾数的二进制位数
#define LDBL_MAX 0x1.fffffffffffffp+1023  // 扩展双精度浮点数的最大值
#define LDBL_MAX_10_EXP 308  // 扩展双精度浮点数最大的十进制指数
#define LDBL_MAX_EXP 1024  // 扩展双精度浮点数的最大指数
#define LDBL_MIN 0x1p-1022  // 扩展双精度浮点数的最小正值
#define LDBL_MIN_10_EXP -307  // 扩展双精度浮点数最小的十进制指数
#define LDBL_MIN_EXP -1021  // 扩展双精度浮点数的最小指数
#define LDBL_TRUE_MIN 0x0.0000000000001p-1022  // 扩展双精度浮点数的最小非零正值

#endif  // 结束条件编译指令，防止重复包含