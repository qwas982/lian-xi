/*
 * The 失乐园方案 输入/输出
 
 */

#ifndef __失乐园_入出_H__
#define __失乐园_入出_H__

#include "失乐园.h"
#include "对象.h"

char* 失乐园_串转储(const char* 串短);

/*
 * A string 缓冲区. This string 缓冲区 enlarges itself automatically to
 * accommodate new characters. It never shrinks.
 */

typedef struct 失乐园_缓冲区_类型 失乐园_缓冲区_类型;

失乐园_缓冲区_类型* 失乐园_缓冲区_新(void);
void 失乐园_缓冲区_摧毁(失乐园_缓冲区_类型* 缓冲区);
void 失乐园_缓冲区_赋值(失乐园_缓冲区_类型* 缓冲区, const char* 串短);
void 失乐园_缓冲区_添加(失乐园_缓冲区_类型* 缓冲区, 失乐园_印刻_类型 c);
const 失乐园_印刻_类型* 失乐园_缓冲区_串(失乐园_缓冲区_类型* 缓冲区);
int 失乐园_缓冲区_谓词等于(失乐园_缓冲区_类型* 缓冲区, const 失乐园_印刻_类型* 串短);

/*
 * strings
 */

失乐园_代码点1_类型 *失乐园_入出_到_拉丁文1(失乐园_状态_类型 *大失, 失乐园_串_类型 *串短);
失乐园_代码点1_类型 *失乐园_入出_到_万国码编码8(失乐园_状态_类型 *大失, 失乐园_串_类型 *串短);
失乐园_代码点2_类型 *失乐园_入出_万国码编码16(失乐园_状态_类型 *大失, 失乐园_串_类型 *串短);

/*
 * ports
 */

失乐园_垃圾回收对象_类型 *失乐园_入出_创建_标准入(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_入出_创建_标准出(失乐园_状态_类型 *大失);
失乐园_垃圾回收对象_类型 *失乐园_入出_创建_标准错(失乐园_状态_类型 *大失);

失乐园_垃圾回收对象_类型 *失乐园_入出_打开_入文件(失乐园_状态_类型 *大失, 失乐园_对象_类型 *串短, uint8_t 印刻_编码);
失乐园_垃圾回收对象_类型 *失乐园_入出_打开_出文件(失乐园_状态_类型 *大失, 失乐园_对象_类型 *串短, uint8_t 印刻_编码);

void 失乐园_入出_关闭_入端口(失乐园_状态_类型 *大失, 失乐园_对象_类型 *端口);
void 失乐园_入出_关闭_出端口(失乐园_状态_类型 *大失, 失乐园_对象_类型 *端口);

/*
 * reader
 */

void 失乐园_入出_读_牌(失乐园_状态_类型 *大失, 失乐园_对象_类型 *端口, 失乐园_对象_类型 *返回短);

/*
 * writer
 */

void 失乐园_入出_新行(失乐园_状态_类型 *大失, 失乐园_对象_类型 *端口);

void 失乐园_入出_写_印名_串(失乐园_状态_类型 *大失, const char *串短, 失乐园_对象_类型 *端口);
void 失乐园_入出_写_符号(失乐园_状态_类型 *大失, 失乐园_符号_类型 *符号短, 失乐园_对象_类型 *端口);

void 失乐园_入出_写(失乐园_状态_类型 *大失, 失乐园_对象_类型 *对象短, 失乐园_对象_类型 *端口);
void 失乐园_入出_显示(失乐园_状态_类型 *大失, 失乐园_对象_类型 *对象短, 失乐园_对象_类型 *端口);

#endif
