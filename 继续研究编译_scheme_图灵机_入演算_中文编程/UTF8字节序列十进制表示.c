#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 读取文件中的字节并存储为十进制表示
unsigned short *读取文件到十进制(const char *文件名, size_t *字节数) {
    FILE *文件 = fopen(文件名, "rb");
    if (!文件) {
        perror("无法打开文件");
        return NULL;
    }

    fseek(文件, 0, SEEK_END);
    *字节数 = ftell(文件);
    fseek(文件, 0, SEEK_SET);

    unsigned short *十进制数组 = (unsigned short *)malloc(*字节数 * sizeof(unsigned short));
    if (!十进制数组) {
        perror("内存分配失败");
        fclose(文件);
        return NULL;
    }

    for (size_t i = 0; i < *字节数; i++) {
        十进制数组[i] = fgetc(文件);
    }

    fclose(文件);
    return 十进制数组;
}

// 判断UTF-8字节编码的十进制表示是否有效
bool 判断有效UTF8字符(const unsigned short *十进制数组, size_t 字节数, size_t *位置) {
    if (*位置 >= 字节数) return false;
    unsigned short 字节 = 十进制数组[*位置];

    if (字节 < 128) {
        // 1字节字符
        (*位置) += 1;
        return true;
    } else if (字节 >= 194 && 字节 <= 223) {
        // 2字节字符
        if (*位置 + 1 >= 字节数) return false;
        if (十进制数组[*位置 + 1] < 128 || 十进制数组[*位置 + 1] > 191) return false;
        (*位置) += 2;
        return true;
    } else if (字节 >= 224 && 字节 <= 239) {
        // 3字节字符
        if (*位置 + 2 >= 字节数) return false;
        if (十进制数组[*位置 + 1] < 128 || 十进制数组[*位置 + 1] > 191) return false;
        if (十进制数组[*位置 + 2] < 128 || 十进制数组[*位置 + 2] > 191) return false;
        (*位置) += 3;
        return true;
    } else if (字节 >= 240 && 字节 <= 244) {
        // 4字节字符
        if (*位置 + 3 >= 字节数) return false;
        if (十进制数组[*位置 + 1] < 128 || 十进制数组[*位置 + 1] > 191) return false;
        if (十进制数组[*位置 + 2] < 128 || 十进制数组[*位置 + 2] > 191) return false;
        if (十进制数组[*位置 + 3] < 128 || 十进制数组[*位置 + 3] > 191) return false;
        (*位置) += 4;
        return true;
    }
    return false;
}

unsigned short main() {
    const char *文件名 = "example.txt";  // 替换为你的文件名
    size_t 字节数;
    unsigned short *十进制数组 = 读取文件到十进制(文件名, &字节数);
    if (!十进制数组) return 1;

    size_t 位置 = 0;
    while (位置 < 字节数) {
        if (判断有效UTF8字符(十进制数组, 字节数, &位置)) {
            printf("有效UTF-8字符在位置 %zu\n", 位置 - 1);
        } else {
            printf("无效UTF-8字符在位置 %zu\n", 位置);
            break;
        }
    }

    free(十进制数组);
    return 0;
}