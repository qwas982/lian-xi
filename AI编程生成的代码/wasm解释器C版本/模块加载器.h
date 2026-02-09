/**
 * 模块加载器.h
 * WebAssembly 模块加载器接口
 * 对应原 module_loader.h
 */

#ifndef 模块加载器_H
#define 模块加载器_H

#include "数据结构.h"

/**
 * 加载Wasm二进制模块
 * @param 字节码 Wasm字节码
 * @param 字节数 字节码长度
 * @return 模块指针，失败返回NULL
 */
结构_模块* 加载模块(uint8_t* 字节码, uint32_t 字节数);

/**
 * 从文件加载Wasm模块
 * @param 文件路径 文件路径
 * @return 模块指针，失败返回NULL
 */
结构_模块* 从文件加载模块(const char* 文件路径);

#endif /* 模块加载器_H */
