/**
 * 枚举定义.h
 * WebAssembly MVP 操作码、类型、段类型等枚举定义
 * 对应原 opcode.h
 */

#ifndef 枚举定义_H
#define 枚举定义_H

/* ==================== 操作码枚举 (178条指令) ==================== */

/* 控制流指令 (0x00-0x0F) */
#define 操作码_不可达           0x00
#define 操作码_空操作           0x01
#define 操作码_块开始           0x02
#define 操作码_循环开始         0x03
#define 操作码_条件分支         0x04
#define 操作码_否则分支         0x05
#define 操作码_结束标记         0x0B
#define 操作码_分支             0x0C
#define 操作码_条件分支表       0x0E
#define 操作码_返回             0x0F

/* 参数指令 (0x1A-0x1B) */
#define 操作码_丢弃             0x1A
#define 操作码_选择             0x1B

/* 变量访问指令 (0x20-0x24) */
#define 操作码_局部变量读取     0x20
#define 操作码_局部变量写入     0x21
#define 操作码_局部变量读取32   0x22  /* local.tee */
#define 操作码_局部变量写入32   0x23
#define 操作码_全局变量读取     0x24
#define 操作码_全局变量写入     0x25

/* 内存读取指令 - i32 (0x28-0x2C) */
#define 操作码_内存读取32位             0x28
#define 操作码_内存读取8位有符号扩展32  0x2C
#define 操作码_内存读取8位无符号扩展32  0x2D
#define 操作码_内存读取16位有符号扩展32 0x2E
#define 操作码_内存读取16位无符号扩展32 0x2F

/* 内存读取指令 - i64 (0x29, 0x30-0x35) */
#define 操作码_内存读取64位             0x29
#define 操作码_内存读取8位有符号扩展64  0x30
#define 操作码_内存读取8位无符号扩展64  0x31
#define 操作码_内存读取16位有符号扩展64 0x32
#define 操作码_内存读取16位无符号扩展64 0x33
#define 操作码_内存读取32位有符号扩展64 0x34
#define 操作码_内存读取32位无符号扩展64 0x35

/* 内存读取指令 - f32/f64 (0x2A-0x2B) */
#define 操作码_内存读取单精度浮点   0x2A
#define 操作码_内存读取双精度浮点   0x2B

/* 内存写入指令 - i32 (0x36-0x3A) */
#define 操作码_内存写入32位     0x36
#define 操作码_内存写入8位32    0x3A
#define 操作码_内存写入16位32   0x3B

/* 内存写入指令 - i64 (0x37, 0x3C-0x3E) */
#define 操作码_内存写入64位     0x37
#define 操作码_内存写入8位64    0x3C
#define 操作码_内存写入16位64   0x3D
#define 操作码_内存写入32位64   0x3E

/* 内存写入指令 - f32/f64 (0x38-0x39) */
#define 操作码_内存写入单精度浮点   0x38
#define 操作码_内存写入双精度浮点   0x39

/* 内存管理指令 (0x3F-0x40) */
#define 操作码_内存大小         0x3F
#define 操作码_内存增长         0x40

/* i32 常量与一元运算 (0x41-0x45) */
#define 操作码_常量32位         0x41
#define 操作码_等于零判断32     0x45

/* i32 二元运算 - 比较 (0x46-0x4F) */
#define 操作码_等于判断32       0x46
#define 操作码_不等于判断32     0x47
#define 操作码_小于有符号32     0x48
#define 操作码_小于无符号32     0x49
#define 操作码_大于有符号32     0x4A
#define 操作码_大于无符号32     0x4B
#define 操作码_小于等于有符号32 0x4C
#define 操作码_小于等于无符号32 0x4D
#define 操作码_大于等于有符号32 0x4E
#define 操作码_大于等于无符号32 0x4F

/* i32 二元运算 - 算术 (0x6A-0x78) */
#define 操作码_加法32           0x6A
#define 操作码_减法32           0x6B
#define 操作码_乘法32           0x6C
#define 操作码_除法有符号32     0x6D
#define 操作码_除法无符号32     0x6E
#define 操作码_取余有符号32     0x6F
#define 操作码_取余无符号32     0x70
#define 操作码_与运算32         0x71
#define 操作码_或运算32         0x72
#define 操作码_异或运算32       0x73
#define 操作码_左移32           0x74
#define 操作码_右移有符号32     0x75
#define 操作码_右移无符号32     0x76
#define 操作码_循环左移32       0x77
#define 操作码_循环右移32       0x78

/* i64 常量与一元运算 (0x42-0x50) */
#define 操作码_常量64位         0x42
#define 操作码_等于零判断64     0x50

/* i64 二元运算 - 比较 (0x51-0x5A) */
#define 操作码_等于判断64       0x51
#define 操作码_不等于判断64     0x52
#define 操作码_小于有符号64     0x53
#define 操作码_小于无符号64     0x54
#define 操作码_大于有符号64     0x55
#define 操作码_大于无符号64     0x56
#define 操作码_小于等于有符号64 0x57
#define 操作码_小于等于无符号64 0x58
#define 操作码_大于等于有符号64 0x59
#define 操作码_大于等于无符号64 0x5A

/* i64 二元运算 - 算术 (0x7C-0x8A) */
#define 操作码_加法64           0x7C
#define 操作码_减法64           0x7D
#define 操作码_乘法64           0x7E
#define 操作码_除法有符号64     0x7F
#define 操作码_除法无符号64     0x80
#define 操作码_取余有符号64     0x81
#define 操作码_取余无符号64     0x82
#define 操作码_与运算64         0x83
#define 操作码_或运算64         0x84
#define 操作码_异或运算64       0x85
#define 操作码_左移64           0x86
#define 操作码_右移有符号64     0x87
#define 操作码_右移无符号64     0x88
#define 操作码_循环左移64       0x89
#define 操作码_循环右移64       0x8A

/* f32 常量与一元运算 (0x43, 0x8B-0x91) */
#define 操作码_常量单精度浮点   0x43
#define 操作码_取绝对值单精度   0x8B
#define 操作码_取负单精度       0x8C
#define 操作码_向上取整单精度   0x8D
#define 操作码_向下取整单精度   0x8E
#define 操作码_向零取整单精度   0x8F
#define 操作码_平方根单精度     0x91

/* f32 二元运算 - 比较 (0x5B-0x60) */
#define 操作码_等于判断单精度   0x5B
#define 操作码_不等于判断单精度 0x5C
#define 操作码_小于单精度       0x5D
#define 操作码_大于单精度       0x5E
#define 操作码_小于等于单精度   0x5F
#define 操作码_大于等于单精度   0x60

/* f32 二元运算 - 算术 (0x92-0x98) */
#define 操作码_加法单精度       0x92
#define 操作码_减法单精度       0x93
#define 操作码_乘法单精度       0x94
#define 操作码_除法单精度       0x95
#define 操作码_取最小值单精度   0x96
#define 操作码_取最大值单精度   0x97
#define 操作码_复制符号单精度   0x98

/* f64 常量与一元运算 (0x44, 0x99-0x9F) */
#define 操作码_常量双精度浮点   0x44
#define 操作码_取绝对值双精度   0x99
#define 操作码_取负双精度       0x9A
#define 操作码_向上取整双精度   0x9B
#define 操作码_向下取整双精度   0x9C
#define 操作码_向零取整双精度   0x9D
#define 操作码_平方根双精度     0x9F

/* f64 二元运算 - 比较 (0x61-0x66) */
#define 操作码_等于判断双精度   0x61
#define 操作码_不等于判断双精度 0x62
#define 操作码_小于双精度       0x63
#define 操作码_大于双精度       0x64
#define 操作码_小于等于双精度   0x65
#define 操作码_大于等于双精度   0x66

/* f64 二元运算 - 算术 (0xA0-0xA6) */
#define 操作码_加法双精度       0xA0
#define 操作码_减法双精度       0xA1
#define 操作码_乘法双精度       0xA2
#define 操作码_除法双精度       0xA3
#define 操作码_取最小值双精度   0xA4
#define 操作码_取最大值双精度   0xA5
#define 操作码_复制符号双精度   0xA6

/* 类型转换 - i32 (0x67-0x69, 0xAA-0xAB, 0xB2-0xB5, 0xFC-0xFD) */
#define 操作码_截断32位有符号       0x67
#define 操作码_截断32位无符号       0x68
#define 操作码_截断64位有符号32     0xAA
#define 操作码_截断64位无符号32     0xAB
#define 操作码_浮点转32位有符号     0xB2
#define 操作码_浮点转32位无符号     0xB3
#define 操作码_双精度转32位有符号   0xB4
#define 操作码_双精度转32位无符号   0xB5
#define 操作码_截断饱和32位有符号   0xFC
#define 操作码_截断饱和32位无符号   0xFD

/* 类型转换 - i64 (0xAC-0xAD, 0xAE-0xAF, 0xB6-0xB9, 0xFE-0xFF) */
#define 操作码_扩展有符号64         0xAC
#define 操作码_扩展无符号64         0xAD
#define 操作码_截断64位有符号64     0xAE
#define 操作码_截断64位无符号64     0xAF
#define 操作码_浮点转64位有符号     0xB6
#define 操作码_浮点转64位无符号     0xB7
#define 操作码_双精度转64位有符号   0xB8
#define 操作码_双精度转64位无符号   0xB9
#define 操作码_截断饱和64位有符号   0xFE
#define 操作码_截断饱和64位无符号   0xFF

/* 类型转换 - f32 (0xB2-0xB6) */
#define 操作码_转换32位有符号单精度 0xB2
#define 操作码_转换32位无符号单精度 0xB3
#define 操作码_转换64位有符号单精度 0xB4
#define 操作码_转换64位无符号单精度 0xB5
#define 操作码_双精度转单精度       0xB6

/* 类型转换 - f64 (0xB7-0xBB) */
#define 操作码_转换32位有符号双精度 0xB7
#define 操作码_转换32位无符号双精度 0xB8
#define 操作码_转换64位有符号双精度 0xB9
#define 操作码_转换64位无符号双精度 0xBA
#define 操作码_单精度转双精度       0xBB

/* 位运算与 reinterpret (0xBC-0xC4) */
#define 操作码_位reinterpret32      0xBC
#define 操作码_位reinterpret64      0xBD
#define 操作码_扩展8位有符号32      0xC0
#define 操作码_扩展16位有符号32     0xC1
#define 操作码_扩展8位有符号64      0xC2
#define 操作码_扩展16位有符号64     0xC3
#define 操作码_扩展32位有符号64     0xC4


/* ==================== 值类型枚举 ==================== */
#define 值类型_空类型       0x40
#define 值类型_函数类型     0x60
#define 值类型_外部函数     0x00
#define 值类型_外部表       0x01
#define 值类型_外部内存     0x02
#define 值类型_外部全局     0x03
#define 值类型_整数32       0x7F
#define 值类型_整数64       0x7E
#define 值类型_浮点32       0x7D
#define 值类型_浮点64       0x7C


/* ==================== 块类型枚举 ==================== */
#define 块类型_空块         0x40


/* ==================== 段类型枚举 ==================== */
#define 段类型_自定义段     0
#define 段类型_类型段       1
#define 段类型_导入段       2
#define 段类型_函数段       3
#define 段类型_表段         4
#define 段类型_内存段       5
#define 段类型_全局段       6
#define 段类型_导出段       7
#define 段类型_起始段       8
#define 段类型_元素段       9
#define 段类型_代码段       10
#define 段类型_数据段       11


/* ==================== 导出类型枚举 ==================== */
#define 导出类型_函数   0
#define 导出类型_表     1
#define 导出类型_内存   2
#define 导出类型_全局   3


/* ==================== 错误码枚举 ==================== */
enum 枚举_错误码 {
    错误码_成功 = 0,
    错误码_栈溢出,
    错误码_栈下溢,
    错误码_调用栈溢出,
    错误码_调用栈下溢,
    错误码_块栈溢出,
    错误码_块栈下溢,
    错误码_内存访问越界,
    错误码_除零错误,
    错误码_整数溢出,
    错误码_无效转换,
    错误码_不可达代码,
    错误码_无效魔数,
    错误码_无效版本,
    错误码_无效段,
    错误码_无效类型,
    错误码_无效操作码,
    错误码_未实现,
    错误码_解析错误,
    错误码_内存不足
};


/* ==================== 常量定义 ==================== */
#define WASM魔数_0      0x00
#define WASM魔数_1      0x61
#define WASM魔数_2      0x73
#define WASM魔数_3      0x6D
#define WASM版本        1

#define 页大小          65536       /* 64KB */
#define 默认栈大小      65536       /* 操作数栈大小 */
#define 默认调用栈大小  4096        /* 调用栈帧数 */
#define 默认块栈大小    4096        /* 块栈大小 */
#define 默认分支表大小  65536       /* 分支表大小 */

#endif /* 枚举定义_H */
