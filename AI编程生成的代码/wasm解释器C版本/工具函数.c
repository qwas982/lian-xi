/**
 * 工具函数.c
 * WebAssembly 工具函数实现
 * LEB128编解码、辅助函数
 */

#include <stdlib.h>
#include <math.h>
#include "工具函数.h"

/* ==================== LEB128 解码函数 ==================== */

uint32_t 解码无符号32位(uint8_t* 字节码, uint32_t 偏移, uint32_t* 输出值) {
    uint32_t 结果 = 0;
    uint32_t 位移 = 0;
    uint8_t 字节;
    
    while (1) {
        字节 = 字节码[偏移];
        偏移++;
        
        结果 |= (uint32_t)(字节 & 0x7F) << 位移;
        
        if ((字节 & 0x80) == 0) {
            break;
        }
        
        位移 += 7;
        if (位移 >= 32) {
            return 0; /* 错误：超出32位 */
        }
    }
    
    *输出值 = 结果;
    return 偏移;
}

uint32_t 解码无符号64位(uint8_t* 字节码, uint32_t 偏移, uint64_t* 输出值) {
    uint64_t 结果 = 0;
    uint32_t 位移 = 0;
    uint8_t 字节;
    
    while (1) {
        字节 = 字节码[偏移];
        偏移++;
        
        结果 |= (uint64_t)(字节 & 0x7F) << 位移;
        
        if ((字节 & 0x80) == 0) {
            break;
        }
        
        位移 += 7;
        if (位移 >= 64) {
            return 0; /* 错误：超出64位 */
        }
    }
    
    *输出值 = 结果;
    return 偏移;
}

uint32_t 解码有符号32位(uint8_t* 字节码, uint32_t 偏移, int32_t* 输出值) {
    int32_t 结果 = 0;
    uint32_t 位移 = 0;
    uint8_t 字节;
    
    while (1) {
        字节 = 字节码[偏移];
        偏移++;
        
        结果 |= (int32_t)(字节 & 0x7F) << 位移;
        
        if ((字节 & 0x80) == 0) {
            /* 符号扩展 */
            if ((字节 & 0x40) && 位移 < 32) {
                结果 |= (~0) << (位移 + 7);
            }
            break;
        }
        
        位移 += 7;
        if (位移 >= 32) {
            return 0; /* 错误：超出32位 */
        }
    }
    
    *输出值 = 结果;
    return 偏移;
}

uint32_t 解码有符号64位(uint8_t* 字节码, uint32_t 偏移, int64_t* 输出值) {
    int64_t 结果 = 0;
    uint32_t 位移 = 0;
    uint8_t 字节;
    
    while (1) {
        字节 = 字节码[偏移];
        偏移++;
        
        结果 |= (int64_t)(字节 & 0x7F) << 位移;
        
        if ((字节 & 0x80) == 0) {
            /* 符号扩展 */
            if ((字节 & 0x40) && 位移 < 64) {
                结果 |= (~0LL) << (位移 + 7);
            }
            break;
        }
        
        位移 += 7;
        if (位移 >= 64) {
            return 0; /* 错误：超出64位 */
        }
    }
    
    *输出值 = 结果;
    return 偏移;
}

/* ==================== LEB128 编码函数 ==================== */

uint32_t 编码无符号32位(uint32_t 值, uint8_t* 输出缓冲区) {
    uint32_t 索引 = 0;
    
    while (1) {
        uint8_t 字节 = 值 & 0x7F;
        值 >>= 7;
        
        if (值 != 0) {
            字节 |= 0x80;
        }
        
        输出缓冲区[索引] = 字节;
        索引++;
        
        if (值 == 0) {
            break;
        }
    }
    
    return 索引;
}

uint32_t 编码有符号32位(int32_t 值, uint8_t* 输出缓冲区) {
    uint32_t 索引 = 0;
    int 更多 = 1;
    
    while (更多) {
        uint8_t 字节 = 值 & 0x7F;
        值 >>= 7;
        
        /* 检查是否需要更多字节 */
        if ((值 == 0 && (字节 & 0x40) == 0) ||
            (值 == -1 && (字节 & 0x40) != 0)) {
            更多 = 0;
        } else {
            字节 |= 0x80;
        }
        
        输出缓冲区[索引] = 字节;
        索引++;
    }
    
    return 索引;
}

uint32_t 编码无符号64位(uint64_t 值, uint8_t* 输出缓冲区) {
    uint32_t 索引 = 0;
    
    while (1) {
        uint8_t 字节 = 值 & 0x7F;
        值 >>= 7;
        
        if (值 != 0) {
            字节 |= 0x80;
        }
        
        输出缓冲区[索引] = 字节;
        索引++;
        
        if (值 == 0) {
            break;
        }
    }
    
    return 索引;
}

uint32_t 编码有符号64位(int64_t 值, uint8_t* 输出缓冲区) {
    uint32_t 索引 = 0;
    int 更多 = 1;
    
    while (更多) {
        uint8_t 字节 = 值 & 0x7F;
        值 >>= 7;
        
        /* 检查是否需要更多字节 */
        if ((值 == 0 && (字节 & 0x40) == 0) ||
            (值 == -1 && (字节 & 0x40) != 0)) {
            更多 = 0;
        } else {
            字节 |= 0x80;
        }
        
        输出缓冲区[索引] = 字节;
        索引++;
    }
    
    return 索引;
}

/* ==================== 浮点转换函数 ==================== */

int 单精度转32位有符号(float 值, int32_t* 输出) {
    double 整数部分;
    
    if (是单精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 最小_INT32 || 整数部分 > 最大_INT32) {
        return 错误码_整数溢出;
    }
    
    *输出 = (int32_t)整数部分;
    return 错误码_成功;
}

int 单精度转32位无符号(float 值, uint32_t* 输出) {
    double 整数部分;
    
    if (是单精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 0 || 整数部分 > 最大_UINT32) {
        return 错误码_整数溢出;
    }
    
    *输出 = (uint32_t)整数部分;
    return 错误码_成功;
}

int 双精度转32位有符号(double 值, int32_t* 输出) {
    double 整数部分;
    
    if (是双精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 最小_INT32 || 整数部分 > 最大_INT32) {
        return 错误码_整数溢出;
    }
    
    *输出 = (int32_t)整数部分;
    return 错误码_成功;
}

int 双精度转32位无符号(double 值, uint32_t* 输出) {
    double 整数部分;
    
    if (是双精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 0 || 整数部分 > 最大_UINT32) {
        return 错误码_整数溢出;
    }
    
    *输出 = (uint32_t)整数部分;
    return 错误码_成功;
}

int 单精度转64位有符号(float 值, int64_t* 输出) {
    double 整数部分;
    
    if (是单精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < (double)最小_INT64 || 整数部分 > (double)最大_INT64) {
        return 错误码_整数溢出;
    }
    
    *输出 = (int64_t)整数部分;
    return 错误码_成功;
}

int 单精度转64位无符号(float 值, uint64_t* 输出) {
    double 整数部分;
    
    if (是单精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 0 || 整数部分 > (double)最大_UINT64) {
        return 错误码_整数溢出;
    }
    
    *输出 = (uint64_t)整数部分;
    return 错误码_成功;
}

int 双精度转64位有符号(double 值, int64_t* 输出) {
    double 整数部分;
    
    if (是双精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < (double)最小_INT64 || 整数部分 > (double)最大_INT64) {
        return 错误码_整数溢出;
    }
    
    *输出 = (int64_t)整数部分;
    return 错误码_成功;
}

int 双精度转64位无符号(double 值, uint64_t* 输出) {
    double 整数部分;
    
    if (是双精度NaN(值) || 值 == INFINITY || 值 == -INFINITY) {
        return 错误码_无效转换;
    }
    
    if (值 < 0) {
        整数部分 = ceil(值);
    } else {
        整数部分 = floor(值);
    }
    
    if (整数部分 < 0 || 整数部分 > (double)最大_UINT64) {
        return 错误码_整数溢出;
    }
    
    *输出 = (uint64_t)整数部分;
    return 错误码_成功;
}

/* ==================== 饱和截断函数 ==================== */

int32_t 截断饱和32位有符号(double 值) {
    if (是双精度NaN(值)) {
        return 0;
    }
    if (值 == INFINITY) {
        return 最大_INT32;
    }
    if (值 == -INFINITY) {
        return 最小_INT32;
    }
    if (值 >= (double)最大_INT32) {
        return 最大_INT32;
    }
    if (值 <= (double)最小_INT32) {
        return 最小_INT32;
    }
    return (int32_t)值;
}

uint32_t 截断饱和32位无符号(double 值) {
    if (是双精度NaN(值)) {
        return 0;
    }
    if (值 == INFINITY || 值 > (double)最大_UINT32) {
        return 最大_UINT32;
    }
    if (值 == -INFINITY || 值 < 0) {
        return 0;
    }
    return (uint32_t)值;
}

int64_t 截断饱和64位有符号(double 值) {
    if (是双精度NaN(值)) {
        return 0;
    }
    if (值 == INFINITY) {
        return 最大_INT64;
    }
    if (值 == -INFINITY) {
        return 最小_INT64;
    }
    if (值 >= (double)最大_INT64) {
        return 最大_INT64;
    }
    if (值 <= (double)最小_INT64) {
        return 最小_INT64;
    }
    return (int64_t)值;
}

uint64_t 截断饱和64位无符号(double 值) {
    if (是双精度NaN(值)) {
        return 0;
    }
    if (值 == INFINITY) {
        return 最大_UINT64;
    }
    if (值 == -INFINITY || 值 < 0) {
        return 0;
    }
    if (值 > (double)最大_UINT64) {
        return 最大_UINT64;
    }
    return (uint64_t)值;
}

/* ==================== 浮点辅助函数 ==================== */

float 复制单精度符号(float 目标, float 源) {
    if (源 < 0 || (源 == 0 && 1.0 / 源 < 0)) {
        return -fabs(目标);
    } else {
        return fabs(目标);
    }
}

double 复制双精度符号(double 目标, double 源) {
    if (源 < 0 || (源 == 0 && 1.0 / 源 < 0)) {
        return -fabs(目标);
    } else {
        return fabs(目标);
    }
}

float 单精度最小值(float 甲, float 乙) {
    if (是单精度NaN(甲)) return 甲;
    if (是单精度NaN(乙)) return 乙;
    if (甲 == 0 && 乙 == 0) {
        /* 处理-0和+0 */
        if (1.0 / 甲 < 0) return 甲; /* 甲是-0 */
        if (1.0 / 乙 < 0) return 乙; /* 乙是-0 */
    }
    return 甲 < 乙 ? 甲 : 乙;
}

double 双精度最小值(double 甲, double 乙) {
    if (是双精度NaN(甲)) return 甲;
    if (是双精度NaN(乙)) return 乙;
    if (甲 == 0 && 乙 == 0) {
        /* 处理-0和+0 */
        if (1.0 / 甲 < 0) return 甲; /* 甲是-0 */
        if (1.0 / 乙 < 0) return 乙; /* 乙是-0 */
    }
    return 甲 < 乙 ? 甲 : 乙;
}

float 单精度最大值(float 甲, float 乙) {
    if (是单精度NaN(甲)) return 甲;
    if (是单精度NaN(乙)) return 乙;
    if (甲 == 0 && 乙 == 0) {
        /* 处理-0和+0 */
        if (1.0 / 甲 > 0) return 甲; /* 甲是+0 */
        if (1.0 / 乙 > 0) return 乙; /* 乙是+0 */
    }
    return 甲 > 乙 ? 甲 : 乙;
}

double 双精度最大值(double 甲, double 乙) {
    if (是双精度NaN(甲)) return 甲;
    if (是双精度NaN(乙)) return 乙;
    if (甲 == 0 && 乙 == 0) {
        /* 处理-0和+0 */
        if (1.0 / 甲 > 0) return 甲; /* 甲是+0 */
        if (1.0 / 乙 > 0) return 乙; /* 乙是+0 */
    }
    return 甲 > 乙 ? 甲 : 乙;
}

int 是单精度NaN(float 值) {
    return 值 != 值;
}

int 是双精度NaN(double 值) {
    return 值 != 值;
}

/* ==================== 块类型辅助函数 ==================== */

uint8_t 获取块返回类型(uint8_t 块类型) {
    switch (块类型) {
        case 值类型_整数32:
            return 值类型_整数32;
        case 值类型_整数64:
            return 值类型_整数64;
        case 值类型_单精度浮点:
            return 值类型_单精度浮点;
        case 值类型_双精度浮点:
            return 值类型_双精度浮点;
        case 块类型_空块:
        default:
            return 0xFF; /* 无返回值 */
    }
}

int 块类型有返回值(uint8_t 块类型) {
    return 块类型 != 块类型_空块;
}

/* ==================== 错误消息函数 ==================== */

const char* 获取错误消息(int 错误码) {
    switch (错误码) {
        case 错误码_成功:
            return "成功";
        case 错误码_栈溢出:
            return "操作数栈溢出";
        case 错误码_栈下溢:
            return "操作数栈下溢";
        case 错误码_调用栈溢出:
            return "调用栈溢出";
        case 错误码_调用栈下溢:
            return "调用栈下溢";
        case 错误码_块栈溢出:
            return "块栈溢出";
        case 错误码_块栈下溢:
            return "块栈下溢";
        case 错误码_内存访问越界:
            return "内存访问越界";
        case 错误码_除零错误:
            return "除零错误";
        case 错误码_整数溢出:
            return "整数溢出";
        case 错误码_无效转换:
            return "无效类型转换";
        case 错误码_不可达代码:
            return "执行到不可达代码";
        case 错误码_无效魔数:
            return "无效的Wasm魔数";
        case 错误码_无效版本:
            return "无效的Wasm版本";
        case 错误码_无效段:
            return "无效的段类型";
        case 错误码_无效类型:
            return "无效的类型";
        case 错误码_无效操作码:
            return "无效的操作码";
        case 错误码_未实现:
            return "功能未实现";
        case 错误码_解析错误:
            return "解析错误";
        case 错误码_内存不足:
            return "内存不足";
        default:
            return "未知错误";
    }
}

/* ==================== 字节序辅助函数 ==================== */

uint16_t 读取小端16位(uint8_t* 数据) {
    return (uint16_t)(数据[0] | (数据[1] << 8));
}

uint32_t 读取小端32位(uint8_t* 数据) {
    return (uint32_t)(数据[0] | (数据[1] << 8) | (数据[2] << 16) | (数据[3] << 24));
}

uint64_t 读取小端64位(uint8_t* 数据) {
    uint32_t 低32位 = 读取小端32位(数据);
    uint32_t 高32位 = 读取小端32位(数据 + 4);
    return ((uint64_t)高32位 << 32) | (uint64_t)低32位;
}

void 写入小端16位(uint8_t* 数据, uint16_t 值) {
    数据[0] = (uint8_t)(值 & 0xFF);
    数据[1] = (uint8_t)((值 >> 8) & 0xFF);
}

void 写入小端32位(uint8_t* 数据, uint32_t 值) {
    数据[0] = (uint8_t)(值 & 0xFF);
    数据[1] = (uint8_t)((值 >> 8) & 0xFF);
    数据[2] = (uint8_t)((值 >> 16) & 0xFF);
    数据[3] = (uint8_t)((值 >> 24) & 0xFF);
}

void 写入小端64位(uint8_t* 数据, uint64_t 值) {
    写入小端32位(数据, (uint32_t)(值 & 0xFFFFFFFF));
    写入小端32位(数据 + 4, (uint32_t)((值 >> 32) & 0xFFFFFFFF));
}
