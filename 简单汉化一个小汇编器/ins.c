/*
 ** TinyASM - 8086/8088 assembler for DOS
 **
 ** Instruction set.
 **
 ** by Oscar Toledo G.
 **
 ** Creation date: Oct/01/2019.
 */

#include <stdio.h>

/*
 ** This should have been 3 entries per line but DeSmet C only allows 500 strings per module.
 */

/*
 ** Notice some instructions are sorted by 载ESs byte usage first.
 */
char *指令_集合[] = {
    "加\0%d8,%r8\0x00 %d8%r8%d8",
    "加\0%d16,%r16\0x01 %d16%r16%d16",
    "加\0%r8,%d8\0x02 %d8%r8%d8",
    "加\0%r16,%d16\0x03 %d16%r16%d16",
    "加\0AL,%i8\0x04 %i8",
    "加\0AX,%i16\0x05 %i16",
    "推\0ES\0x06",
    "弹\0ES\0x07",
    "或\0%d8,%r8\0x08 %d8%r8%d8",
    "或\0%d16,%r16\0x09 %d16%r16%d16",
    "或\0%r8,%d8\0x0a %d8%r8%d8",
    "或\0%r16,%d16\0x0b %d16%r16%d16",
    "或\0AL,%i8\0x0c %i8",
    "或\0AX,%i16\0x0d %i16",
    "推\0CS\0x0e",
    "进位加\0%d8,%r8\0x10 %d8%r8%d8",
    "进位加\0%d16,%r16\0x11 %d16%r16%d16",
    "进位加\0%r8,%d8\0x12 %d8%r8%d8",
    "进位加\0%r16,%d16\0x13 %d16%r16%d16",
    "进位加\0AL,%i8\0x14 %i8",
    "进位加\0AX,%i16\0x15 %i16",
    "推\0SS\0x16",
    "弹\0SS\0x17",
    "借位减\0%d8,%r8\0x18 %d8%r8%d8",
    "借位减\0%d16,%r16\0x19 %d16%r16%d16",
    "借位减\0%r8,%d8\0x1a %d8%r8%d8",
    "借位减\0%r16,%d16\0x1b %d16%r16%d16",
    "借位减\0AL,%i8\0x1c %i8",
    "借位减\0AX,%i16\0x1d %i16",
    "推\0DS\0x1e",
    "弹\0DS\0x1f",
    "与\0%d8,%r8\0x20 %d8%r8%d8",
    "与\0%d16,%r16\0x21 %d16%r16%d16",
    "与\0%r8,%d8\0x22 %d8%r8%d8",
    "与\0%r16,%d16\0x23 %d16%r16%d16",
    "与\0AL,%i8\0x24 %i8",
    "与\0AX,%i16\0x25 %i16",
    "ES\0\0x26",
    "十进制加\0\0x27",
    "减\0%d8,%r8\0x28 %d8%r8%d8",
    "减\0%d16,%r16\0x29 %d16%r16%d16",
    "减\0%r8,%d8\0x2a %d8%r8%d8",
    "减\0%r16,%d16\0x2b %d16%r16%d16",
    "减\0AL,%i8\0x2c %i8",
    "减\0AX,%i16\0x2d %i16",
    "CS\0\0x2e",
    "十进制减\0\0x2f",
    "异或\0%d8,%r8\0x30 %d8%r8%d8",
    "异或\0%d16,%r16\0x31 %d16%r16%d16",
    "异或\0%r8,%d8\0x32 %d8%r8%d8",
    "异或\0%r16,%d16\0x33 %d16%r16%d16",
    "异或\0AL,%i8\0x34 %i8",
    "异或\0AX,%i16\0x35 %i16",
    "SS\0\0x36",
    "ASCII加\0\0x37",
    "对比\0%d8,%r8\0x38 %d8%r8%d8",
    "对比\0%d16,%r16\0x39 %d16%r16%d16",
    "对比\0%r8,%d8\0x3a %d8%r8%d8",
    "对比\0%r16,%d16\0x3b %d16%r16%d16",
    "对比\0AL,%i8\0x3c %i8",
    "对比\0AX,%i16\0x3d %i16",
    "DS\0\0x3e",
    "ASCII减\0\0x3f",
    "增1\0%r16\0b01000%r16",
    "减1\0%r16\0b01001%r16",
    "推\0%r16\0b01010%r16",
    "弹\0%r16\0b01011%r16",
    "溢出跳\0%a8\0x70 %a8",
    "不溢跳\0%a8\0x71 %a8",
    "无符小于跳\0%a8\0x72 %a8",
    "进位跳\0%a8\0x72 %a8",
    "无符不小于跳\0%a8\0x73 %a8",
    "无进位跳\0%a8\0x73 %a8",
    "为0跳\0%a8\0x74 %a8",
    "不为0跳\0%a8\0x75 %a8",
    "等于跳\0%a8\0x74 %a8",
    "不等跳\0%a8\0x75 %a8",
    "无符小等跳\0%a8\0x76 %a8",
    "无符大于跳\0%a8\0x77 %a8",
    "负跳\0%a8\0x78 %a8",
    "非负跳\0%a8\0x79 %a8",
    "奇偶相等跳\0%a8\0x7a %a8",
    "奇偶不等跳\0%a8\0x7b %a8",
    "有符小于跳\0%a8\0x7C %a8",
    "有符大等跳\0%a8\0x7D %a8",
    "有符小等跳\0%a8\0x7E %a8",
    "有符大于跳\0%a8\0x7F %a8",
    "加\0%d16,%s8\0x83 %d16000%d16 %s8",
    "或\0%d16,%s8\0x83 %d16001%d16 %s8",
    "进位加\0%d16,%s8\0x83 %d16010%d16 %s8",
    "借位减\0%d16,%s8\0x83 %d16011%d16 %s8",
    "与\0%d16,%s8\0x83 %d16100%d16 %s8",
    "减\0%d16,%s8\0x83 %d16101%d16 %s8",
    "异或\0%d16,%s8\0x83 %d16110%d16 %s8",
    "对比\0%d16,%s8\0x83 %d16111%d16 %s8",
    "加\0%d8,%i8\0x80 %d8000%d8 %i8",
    "或\0%d8,%i8\0x80 %d8001%d8 %i8",
    "进位加\0%d8,%i8\0x80 %d8010%d8 %i8",
    "借位减\0%d8,%i8\0x80 %d8011%d8 %i8",
    "与\0%d8,%i8\0x80 %d8100%d8 %i8",
    "减\0%d8,%i8\0x80 %d8101%d8 %i8",
    "异或\0%d8,%i8\0x80 %d8110%d8 %i8",
    "对比\0%d8,%i8\0x80 %d8111%d8 %i8",
    "加\0%d16,%i16\0x81 %d16000%d16 %i16",
    "或\0%d16,%i16\0x81 %d16001%d16 %i16",
    "进位加\0%d16,%i16\0x81 %d16010%d16 %i16",
    "借位减\0%d16,%i16\0x81 %d16011%d16 %i16",
    "与\0%d16,%i16\0x81 %d16100%d16 %i16",
    "减\0%d16,%i16\0x81 %d16101%d16 %i16",
    "异或\0%d16,%i16\0x81 %d16110%d16 %i16",
    "对比\0%d16,%i16\0x81 %d16111%d16 %i16",
    "测试\0%d8,%r8\0x84 %d8%r8%d8",
    "测试\0%r8,%d8\0x84 %d8%r8%d8",
    "测试\0%d16,%r16\0x85 %d16%r16%d16",
    "测试\0%r16,%d16\0x85 %d16%r16%d16",
    
    "移\0AL,[%i16]\0xa0 %i16",
    "移\0AX,[%i16]\0xa1 %i16",
    "移\0[%i16],AL\0xa2 %i16",
    "移\0[%i16],AX\0xa3 %i16",
    "移\0%d8,%r8\0x88 %d8%r8%d8",
    "移\0%d16,%r16\0x89 %d16%r16%d16",
    "移\0%r8,%d8\0x8a %d8%r8%d8",
    "移\0%r16,%d16\0x8b %d16%r16%d16",
    
    "移\0%d16,ES\0x8c %d16000%d16",
    "移\0%d16,CS\0x8c %d16001%d16",
    "移\0%d16,SS\0x8c %d16010%d16",
    "移\0%d16,DS\0x8c %d16011%d16",
    "载有效地址\0%r16,%d16\0x8d %d16%r16%d16",
    "移\0ES,%d16\0x8e %d16000%d16",
    "移\0CS,%d16\0x8e %d16001%d16",
    "移\0SS,%d16\0x8e %d16010%d16",
    "移\0DS,%d16\0x8e %d16011%d16",
    "弹\0%d16\0x8f %d16000%d16",
    "空转\0\0x90",
    "交换\0AX,%r16\0b10010%r16",
    "交换\0%r16,AX\0b10010%r16",
    "交换\0%d8,%r8\0x86 %d8%r8%d8",
    "交换\0%r8,%d8\0x86 %d8%r8%d8",
    "交换\0%d16,%r16\0x87 %d16%r16%d16",
    "交换\0%r16,%d16\0x87 %d16%r16%d16",
    "字节到字\0\0x98",
    "字到双字\0\0x99",
    "等待\0\0x9b",
    "推旗\0\0x9c",
    "弹旗\0\0x9d",
    "存AH旗\0\0x9e",
    "载AH旗\0\0x9f",
    "移串字节\0\0xa4",
    "移串字\0\0xa5",
    "对比串字节\0\0xa6",
    "对比串字\0\0xa7",
    "测试\0AL,%i8\0xa8 %i8",
    "测试\0AX,%i16\0xa9 %i16",
    "存串字节\0\0xaa",
    "存串字\0\0xab",
    "载串字节\0\0xac",
    "载串字\0\0xad",
    "扫描串字节\0\0xae",
    "扫描串字\0\0xaf",
    "移\0%r8,%i8\0b10110%r8 %i8",
    "移\0%r16,%i16\0b10111%r16 %i16",
    "返回\0%i16\0xc2 %i16",
    "返回\0\0xc3",
    "载ES\0%r16,%d16\0b11000100 %d16%r16%d16",
    "载DS\0%r16,%d16\0b11000101 %d16%r16%d16",
    "移\0%db8,%i8\0b11000110 %d8000%d8 %i8",
    "移\0%dw16,%i16\0b11000111 %d16000%d16 %i16",
    "返回F\0%i16\0xca %i16",
    "返回F\0\0xcb",
    "中断3\0\0xcc",
    "中断\0%i8\0xcd %i8",
    "中断溢出\0\0xce",
    "中断返回\0\0xcf",
    "轮流左\0%d8,1\0xd0 %d8000%d8",
    "轮流右\0%d8,1\0xd0 %d8001%d8",
    "轮流进位左\0%d8,1\0xd0 %d8010%d8",
    "轮流进位右\0%d8,1\0xd0 %d8011%d8",
    "左移\0%d8,1\0xd0 %d8100%d8",
    "右移\0%d8,1\0xd0 %d8101%d8",
    "算术右移\0%d8,1\0xd0 %d8111%d8",
    "轮流左\0%d16,1\0xd1 %d16000%d16",
    "轮流右\0%d16,1\0xd1 %d16001%d16",
    "轮流进位左\0%d16,1\0xd1 %d16010%d16",
    "轮流进位右\0%d16,1\0xd1 %d16011%d16",
    "左移\0%d16,1\0xd1 %d16100%d16",
    "右移\0%d16,1\0xd1 %d16101%d16",
    "算术右移\0%d16,1\0xd1 %d16111%d16",
    "轮流左\0%d8,CL\0xd2 %d8000%d8",
    "轮流右\0%d8,CL\0xd2 %d8001%d8",
    "轮流进位左\0%d8,CL\0xd2 %d8010%d8",
    "轮流进位右\0%d8,CL\0xd2 %d8011%d8",
    "左移\0%d8,CL\0xd2 %d8100%d8",
    "右移\0%d8,CL\0xd2 %d8101%d8",
    "算术右移\0%d8,CL\0xd2 %d8111%d8",
    "轮流左\0%d16,CL\0xd3 %d16000%d16",
    "轮流右\0%d16,CL\0xd3 %d16001%d16",
    "轮流进位左\0%d16,CL\0xd3 %d16010%d16",
    "轮流进位右\0%d16,CL\0xd3 %d16011%d16",
    "左移\0%d16,CL\0xd3 %d16100%d16",
    "右移\0%d16,CL\0xd3 %d16101%d16",
    "算术右移\0%d16,CL\0xd3 %d16111%d16",
    "ASCII乘\0\0xd4 x0a",
    "ASCII除\0\0xd5 x0a",
    "字节译\0\0xd7",
    "环非零\0%a8\0xe0 %a8",
    "环不等\0%a8\0xe0 %a8",
    "环0\0%a8\0xe1 %a8",
    "环等\0%a8\0xe1 %a8",
    "环\0%a8\0xe2 %a8",
    "跳CX0\0%a8\0xe3 %a8",
    "入\0AL,DX\0xec",
    "入\0AX,DX\0xed",
    "出\0DX,AL\0xee",
    "出\0DX,AX\0xef",
    "入\0AL,%i8\0xe4 %i8",
    "入\0AX,%i8\0xe5 %i8",
    "出\0%i8,AL\0xe6 %i8",
    "出\0%i8,AX\0xe7 %i8",
    "调用\0FAR %d16\0xff %d16011%d16",
    "跳\0FAR %d16\0xff %d16101%d16",
    "调用\0%f32\0x9a %f32",
    "跳\0%f32\0xea %f32",
    "调用\0%d16\0xff %d16010%d16",
    "跳\0%d16\0xff %d16100%d16",
    "跳\0%a8\0xeb %a8",
    "跳\0%a16\0xe9 %a16",
    "调用\0%a16\0xe8 %a16",
    "锁\0\0xf0",
    "重复非零\0\0xf2",
    "重复不等\0\0xf2",
    "重复0\0\0xf3",
    "重复等\0\0xf3",
    "重复\0\0xf3",
    "停机\0\0xf4",
    "补码进位\0\0xf5",
    "测试\0%db8,%i8\0xf6 %d8000%d8 %i8",
    "非\0%db8\0xf6 %d8010%d8",
    "否定\0%db8\0xf6 %d8011%d8",
    "乘\0%db8\0xf6 %d8100%d8",
    "整数乘\0%db8\0xf6 %d8101%d8",
    "除\0%db8\0xf6 %d8110%d8",
    "整数除\0%db8\0xf6 %d8111%d8",
    "测试\0%dw16,%i16\0xf7 %d8000%d8 %i16",
    "非\0%dw16\0xf7 %d8010%d8",
    "否定\0%dw16\0xf7 %d8011%d8",
    "乘\0%dw16\0xf7 %d8100%d8",
    "整数乘\0%dw16\0xf7 %d8101%d8",
    "除\0%dw16\0xf7 %d8110%d8",
    "整数除\0%dw16\0xf7 %d8111%d8",
    "清理进位\0\0xf8",
    "设置进位\0\0xf9",
    "清理中断\0\0xfa",
    "设置中断 \0\0xfb",
    "清理指示\0\0xfc",
    "设置指示\0\0xfd",
    "增1\0%db8\0xfe %d8000%d8",
    "减1\0%db8\0xfe %d8001%d8",
    "增1\0%dw16\0xff %d16000%d16",
    "减1\0%dw16\0xff %d16001%d16",
    "推\0%d16\0xff %d16110%d16",
    NULL,NULL,NULL
};

