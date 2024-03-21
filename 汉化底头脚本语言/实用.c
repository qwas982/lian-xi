#include <stdarg.h>
#include <stdlib.h>

#include "底头.h"

// 内存管理函数，根据传入的参数会有三种不同作用：
// 1.申请内存 molloc：当 ptr 为 NULL 且 newSize 不为 0时，
// realloc(ptr, newSize) 相当于 malloc(newSize)，即申请内存
// 2.释放内存 free：当 ptr 不为 NULL 且 newSize 为 0 时，调用 free 进行释放内存
// 3.修改空间大小 realloc：当 ptr 不为 NULL 且 newSize 不为 0 时，则执行 realloc(ptr, newSize)
// 相当于修改空间大小，可能是在原内存空间继续分配新的空间，或者是重新分配一个新的内存空间
void *内存管理器(虚拟机大写 *虚机针, void *指针, uint32_t 旧大小, uint32_t 新大小) {
    // 记录系统分配的内存总和
    虚机针->已分配字节们 += 新大小 - 旧大小;

    // 避免 realloc(NULL, 0) 来定义新地址，该地址不能被释放
    if (新大小 == 0) {
        free(指针);
        return NULL;
    }

    // 将 ptr 指向的内存大小调整到 newSize
    // 如果将 realloc 的返回的地址直接赋给原指针变量，当 realloc 申请内存失败（内存不足等）则会返回 NULL，
    // 这样原指针变量就会被 NULL 替换，丢失原地址空间，无法释放而产生内存泄漏
    return realloc(指针, 新大小);
}

// 找出大于等于 v 的最小的 2 次幂
uint32_t 向上取最接近的2次幂(uint32_t 值也) {
    值也 += (值也 == 0); // 兼容 v 等于 0 时结果为 0 等边界情况
    值也--;
    值也 |= 值也 >> 1;
    值也 |= 值也 >> 2;
    值也 |= 值也 >> 4;
    值也 |= 值也 >> 8;
    值也 |= 值也 >> 16;
    值也++;
    return 值也;
}

// TODO: 暂时未搞懂，后面填坑
定义缓冲区方法大写(字符串大写)
定义缓冲区方法大写(整型大写)
定义缓冲区方法大写(字符大写)
定义缓冲区方法大写(字节大写)

// TODO: 暂时未搞懂，后面填坑
void 符号表清除(虚拟机大写 *虚机针, 符号表大写 *缓冲区) {
    uint32_t 索引了 = 0;
    while (索引了 < 缓冲区->计数) {
        内存管理器(虚机针, 缓冲区->数据们[索引了++].串, 0, 0);
    }
    字符串大写缓冲区清除大写(虚机针, 缓冲区);
}

// 通用报错函数
void 错误报告(void *词法器, 错误类型大写 错误类型, const char *格式, ...) {
    char 缓冲区[默认_缓冲区_大小] = {'\0'};
    va_list 实针;
    va_start(实针, 格式);
    vsnprintf(缓冲区, 默认_缓冲区_大小, 格式, 实针);
    va_end(实针);

    switch (错误类型) {
        case 错误_入出大写:
        case 错误_内存大写:
            fprintf(stderr, "%s:%d 在函数 %s() 内:%s\n",
                    __FILE__, __LINE__, __func__, 缓冲区);
            break;
        case 错误_词法大写:
        case 错误_编译大写:
            断言大写(词法器 != NULL, "词法分析器是空指针!");
            fprintf(stderr, "%s:%d \"%s\"\n", ((词法器大写 *)词法器)->文件,
                    ((词法器大写 *)词法器)->前一个牌.行号, 缓冲区);
            break;
        case 错误_运行时大写:
            fprintf(stderr, "%s\n", 缓冲区);
            break;
        default:
            不可达大写()
    }
    exit(1);
}