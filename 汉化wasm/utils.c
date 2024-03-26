#include "utils.h"
#include "module.h"
#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

// 全局的异常信息，用于收集运行时（即虚拟机执行指令过程）中的异常信息
char 异常[4096];

/*
 * LEB128（Little Endian Base 128）变长编码格式目的是节约空间
 * 对于 32 位整数，编码后可能是 1 到 5 个字节
 * 对于 64 位整数，编码后可能是 1 到 10 个字节
 * 越小的整数，编码后占用的字节数就越小
 *
 * https://en.wikipedia.org/wiki/LEB128#Decode_unsigned_integer
 * 针对无符号整数的 LEB128 编码特点：
 * 1. 采用小端编码方式，即低位字节在前，高位字节在后
 * 2. 采用 128 进制，每 7 个比特为一组，由一个字节的后 7 位承载，空出来的最高位是标记位，1 表示后面还有后续字节，0 表示没有
 * 例如：LEB128 编码为 11100101 10001110 00100110，解码为 000 0100110 0001110 1100101
 * 注：0x80 -- 10000000    0x7f -- 01111111
 *
 * 针对有符号整数的 LEB128 编码，与上面无符号的完全相同，
 * 只有最后一个字节的第二高位是符号位，如果是 1，表示这是一个负数，需将高位全部补全为 1，如果是 0，表示这是一个正数，需将高位全部补全为 0
*/
uint64_t read_LEB(const uint8_t *字节们, uint32_t *位置, uint32_t 最大位们, bool sign) {
    uint64_t result = 0;
    uint32_t shift = 0;
    uint32_t bcnt = 0;
    uint32_t startpos = *位置;
    uint64_t byte;

    while (true) {
        byte = 字节们[*位置];
        *位置 += 1;
        // 取字节中后 7 位作为值插入到 result 中，按照小端序，即低位字节在前，高位字节在后
        result |= ((byte & 0x7f) << shift);
        shift += 7;
        // 如果某个字节的最高位为 0，即和 0x80 相与结果为 0，则表示该字节为最后一个字节，没有后续字节了
        if ((byte & 0x80) == 0) {
            break;
        }
        bcnt += 1;
        // (最大位们 + 7 - 1) / 7 表示要表示 最大位们 位二进制数字所需要的字节数减 1
        // 由于 bcnt 是从 0 开始
        // 所以 bcnt > (最大位们 + 7 - 1) / 7 表示该次循环所得到的字节数 bcnt + 1 已经超过了 最大位们 位二进制数字所需的字节数 (最大位们 + 7 - 1) / 7 + 1
        // 也就是该数字的位数超出了传入的最大位数值，所以报错
        if (bcnt > (最大位们 + 7 - 1) / 7) {
            致命错("Unsigned LEB at byte %d overflow", startpos)
        }
    }

    // 如果是有符号整数，针对于最后一个字节，则需要
    if (sign && (shift < 最大位们) && (byte & 0x40)) {
        result |= -(1 << shift);
    }
    return result;
}

// 解码针对无符号整数的 LEB128 编码
uint64_t 读_基于小端_无符号(const uint8_t *字节们, uint32_t *位置, uint32_t 最大位们) {
    return read_LEB(字节们, 位置, 最大位们, false);
}

// 解码针对有符号整数的 LEB128 编码
uint64_t 读_基于小端_有符号(const uint8_t *字节们, uint32_t *位置, uint32_t 最大位们) {
    return read_LEB(字节们, 位置, 最大位们, true);
}

// 从字节数组中读取字符串，其中字节数组的开头 4 个字节用于表示字符串的长度
// 注：如果参数 结果_长 不为 NULL，则会被赋值为字符串的长度
char *读_字符串(const uint8_t *字节们, uint32_t *位置, uint32_t *结果_长) {
    // 读取字符串的长度
    uint32_t str_len = 读_基于小端_无符号(字节们, 位置, 32);
    // 为字符串申请内存
    char *串 = malloc(str_len + 1);
    // 将字节数组的数据拷贝到字符串 串 中
    memcpy(串, 字节们 + *位置, str_len);
    // 字符串以字符 '\0' 结尾
    串[str_len] = '\0';
    // 字节数组位置增加相应字符串长度
    *位置 += str_len;
    // 如果参数 结果_长 不为 NULL，则会被赋值为字符串的长度
    if (结果_长) {
        *结果_长 = str_len;
    }
    return 串;
}

// 申请内存
void *分配内存配给(size_t nmemb, size_t 大小, char *名称) {
    void *res = calloc(nmemb, 大小);
    if (res == NULL) {
        致命错("Could not allocate %lu 字节们 for %s", nmemb * 大小, 名称)
    }
    return res;
}

// 在原有内存基础上重新申请内存
void *重新分配内存配给(void *指针, size_t old_nmemb, size_t nmemb, size_t 大小, char *名称) {
    // 重新分配内存
    void *res = realloc(指针, nmemb * 大小);
    if (res == NULL) {
        致命错("Could not allocate %lu 字节们 for %s", nmemb * 大小, 名称)
    }
    // 将新申请的内存中的前面部分--即为新数据准备的内存空间，用 0 进行初始化
    memset(res + old_nmemb * 大小, 0, (nmemb - old_nmemb) * 大小);
    return res;
}

// 查找动态库中的 符号
// 如果解析成功则返回 true
// 如果解析失败则返回 false 并设置 错也
bool 解决_符号(char *文件名, char *符号, void **值也, char **错也) {
    void *handle = NULL;
    dlerror();

    if (文件名) {
        handle = dlopen(文件名, RTLD_LAZY);
        if (!handle) {
            *错也 = dlerror();
            return false;
        }
    }

    // 查找动态库中的 符号
    // 根据 动态链接库 操作句柄 (handle) 与符号 (符号)，返回符号对应的地址。使用这个函数不但可以获取函数地址，也可以获取变量地址。
    // handle：由 dlopen 打开动态链接库后返回的指针；
    // 符号：要求获取的函数或全局变量的名称。
    // 返回值：指向函数的地址，供调用使用。
    *值也 = dlsym(handle, 符号);

    if ((*错也 = dlerror()) != NULL) {
        return false;
    }
    return true;
}

// 基于函数签名计算唯一的掩码值
uint64_t 获取_类型_面罩(签名类型 *类型) {
    uint64_t 面罩 = 0x80;

    if (类型->结果_计数 == 1) {
        面罩 |= 0x80 - 类型->结果们[0];
    }
    面罩 = 面罩 << 4;
    for (uint32_t p = 0; p < 类型->形参_计数; p++) {
        面罩 = ((uint64_t) 面罩) << 4;
        面罩 |= 0x80 - 类型->形参们[p];
    }
    return 面罩;
}

// 根据目前版本的 Wasm 标准，控制块不能有参数，且最多只能有一个返回值
// 注：目前多返回值提案还没有进入 Wasm 标准
uint32_t block_type_results[4][1] = {{整32}, {整64}, {浮32}, {浮64}};

签名类型 block_types[5] = {
        {
                .结果_计数 = 0,
        },
        {
                .结果_计数 = 1,
                .结果们 = block_type_results[0],
        },
        {
                .结果_计数 = 1,
                .结果们 = block_type_results[1],
        },
        {
                .结果_计数 = 1,
                .结果们 = block_type_results[2],
        },
        {
                .结果_计数 = 1,
                .结果们 = block_type_results[3],
        }};

// 根据表示该控制块的签名的值（占一个字节），返回控制块的签名，即控制块的返回值的数量和类型
// 0x7f 表示有一个 整数32 类型返回值、0x7e 表示有一个 整数64 类型返回值、0x7d 表示有一个 浮点32位 类型返回值、0x7c 表示有一个 浮点64位 类型返回值、0x40 表示没有返回值
// 注：目前多返回值提案还没有进入 Wasm 标准，根据当前版本的 Wasm 标准，控制块不能有参数，且最多只能有一个返回值
签名类型 *获取_块_类型(uint8_t 值_类型) {
    switch (值_类型) {
        case 块状:
            return &block_types[0];
        case 整32:
            return &block_types[1];
        case 整64:
            return &block_types[2];
        case 浮32:
            return &block_types[3];
        case 浮64:
            return &block_types[4];
        default:
            致命错("Invalid 块_类型 值_类型: %d\n", 值_类型)
    }
}


// 符号扩展 (sign extension)
// 分以下两种情况：
// 1. 将无符号数转换为更大的数据类型：
// 只需简单地在开头添加 0 至所需位数，这种运算称为 0 扩展
// 2. 将有符号数转换为更大的数据类型：
// 需要执行符号扩展，规则是将符号位扩展至所需的位数，即符号位为 0 时在开头添加 0 至所需位数，符号位为 1 时在开头添加 1 至所需位数，
// 例如 char: 1000 0000  --> short: 1111 1111 1000 0000

void 符号扩展_8_32(uint32_t *值也) {
    if (*值也 & 0x80) {
        *值也 = *值也 | 0xffffff00;
    }
}
void 符号扩展_16_32(uint32_t *值也) {
    if (*值也 & 0x8000) {
        *值也 = *值也 | 0xffff0000;
    }
}
void 符号扩展_8_64(uint64_t *值也) {
    if (*值也 & 0x80) {
        *值也 = *值也 | 0xffffffffffffff00;
    }
}
void 符号扩展_16_64(uint64_t *值也) {
    if (*值也 & 0x8000) {
        *值也 = *值也 | 0xffffffffffff0000;
    }
}
void 符号扩展_32_64(uint64_t *值也) {
    if (*值也 & 0x80000000) {
        *值也 = *值也 | 0xffffffff00000000;
    }
}

// 32 位整型循环左移
uint32_t 左旋转32位(uint32_t n, unsigned int c) {
    const unsigned int 面罩 = (CHAR_BIT * sizeof(n) - 1);
    c = c % 32;
    c &= 面罩;
    return (n << c) | (n >> ((-c) & 面罩));
}

// 32 位整型循环右移
uint32_t 右旋转32位(uint32_t n, unsigned int c) {
    const unsigned int 面罩 = (CHAR_BIT * sizeof(n) - 1);
    c = c % 32;
    c &= 面罩;
    return (n >> c) | (n << ((-c) & 面罩));
}

// 64 位整型循环左移
uint64_t 左旋转64位(uint64_t n, unsigned int c) {
    const unsigned int 面罩 = (CHAR_BIT * sizeof(n) - 1);
    c = c % 64;
    c &= 面罩;
    return (n << c) | (n >> ((-c) & 面罩));
}

// 64 位整型循环右移
uint64_t 右旋转64位(uint64_t n, unsigned int c) {
    const unsigned int 面罩 = (CHAR_BIT * sizeof(n) - 1);
    c = c % 64;
    c &= 面罩;
    return (n >> c) | (n << ((-c) & 面罩));
}

// 32 位浮点型比较两数之间最大值
float 网汇_32位浮点最大(float a, float b) {
    float c = fmaxf(a, b);
    if (c == 0 && a == b) {
        return signbit(a) ? b : a;
    }
    return c;
}

// 32 位浮点型比较两数之间最小值
float 网汇_32位浮点最小(float a, float b) {
    float c = fminf(a, b);
    if (c == 0 && a == b) {
        return signbit(a) ? a : b;
    }
    return c;
}

// 64 位浮点型比较两数之间最大值
double 网汇_64位浮点最大(double a, double b) {
    double c = fmax(a, b);
    if (c == 0 && a == b) {
        return signbit(a) ? b : a;
    }
    return c;
}

// 64 位浮点型比较两数之间最小值
double 网汇_64位浮点最小(double a, double b) {
    double c = fmin(a, b);
    if (c == 0 && a == b) {
        return signbit(a) ? a : b;
    }
    return c;
}

// 将 栈值 类型数值用字符串形式展示，展示形式 "<值>:<值_类型>"
char value_str[256];
char *值_代表(栈值 *v) {
    switch (v->值_类型) {
        case 整32:
            snprintf(value_str, 255, "0x%x:整数32", v->值.无符整32位);
            break;
        case 整64:
            snprintf(value_str, 255, "%" PRIu64 ":整数64", v->值.无符整64位);
            break;
        case 浮32:
            snprintf(value_str, 255, "%.7g:浮点32位", v->值.浮点32位);
            break;
        case 浮64:
            snprintf(value_str, 255, "%.7g:浮点64位", v->值.浮点64位);
            break;
    }
    return value_str;
}

// 通过名称从 Wasm 模块中查找同名的导出项
void *获取_导出(网汇模块 *m, char *名称) {
    for (uint32_t e = 0; e < m->导出_计数; e++) {
        char *导出_名称 = m->导出们[e].导出_名称;
        if (!导出_名称) {
            continue;
        }
        if (strncmp(名称, 导出_名称, 1024) == 0) {
            return m->导出们[e].值;
        }
    }
    return NULL;
}

// 打开文件并将文件映射进内存
uint8_t *模映射_文件(char *路径, int *长度) {
    int fd;
    int res;
    struct stat sb;
    uint8_t *字节们;

    fd = open(路径, O_RDONLY);
    if (fd < 0) {
        致命错("Could not open file '%s'\n", 路径)
    }

    res = fstat(fd, &sb);
    if (res < 0) {
        致命错("Could not stat file '%s' (%d)\n", 路径, res)
    }

    字节们 = mmap(0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    // 如果有需要，会将映射的内存大小赋值给参数 长度
    if (长度) {
        *长度 = (int) sb.st_size;
    }
    if (字节们 == MAP_FAILED) {
        致命错("Could not mmap file '%s'", 路径)
    }
    return 字节们;
}

// 将字符串 串 按照空格拆分成多个参数
// 其中 实参计数 被赋值为拆分字符串 串 得到的参数数量
char *argv_buf[100];
char **拆分_实参值(char *串, int *实参计数) {
    argv_buf[(*实参计数)++] = 串;

    for (int i = 1; 串[i] != '\0'; i += 1) {
        if (串[i - 1] == ' ') {
            串[i - 1] = '\0';
            argv_buf[(*实参计数)++] = 串 + i;
        }
    }
    argv_buf[(*实参计数)] = NULL;
    return argv_buf;
}

// 解析函数参数，并将参数压入到操作数栈
void 解析_实参们(网汇模块 *m, 签名类型 *类型, int 实参计数, char **实参值) {
    for (int i = 0; i < 实参计数; i++) {
        for (int j = 0; 实参值[i][j]; j++) {
            实参值[i][j] = (char) tolower(实参值[i][j]);
        }
        m->栈指针++;
        // 将参数压入到操作数栈顶
        栈值 *sv = &m->栈[m->栈指针];
        // 设置参数的值类型
        sv->值_类型 = 类型->形参们[i];
        // 按照参数的值类型，设置参数的值
        switch (类型->形参们[i]) {
            case 整32:
                sv->值.无符整32位 = strtoul(实参值[i], NULL, 0);
                break;
            case 整64:
                sv->值.无符整64位 = strtoull(实参值[i], NULL, 0);
                break;
            case 浮32:
                if (strncmp("-nan", 实参值[i], 4) == 0) {
                    sv->值.浮点32位 = -NAN;
                } else {
                    sv->值.浮点32位 = (float) strtod(实参值[i], NULL);
                }
                break;
            case 浮64:
                if (strncmp("-nan", 实参值[i], 4) == 0) {
                    sv->值.浮点64位 = -NAN;
                } else {
                    sv->值.浮点64位 = strtod(实参值[i], NULL);
                }
                break;
        }
    }
}
