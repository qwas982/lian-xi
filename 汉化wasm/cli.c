#include "interpreter.h"
#include "module.h"
#include "utils.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdint.h>
#include <string.h>

#define BEGIN(x, y) "\033[" #x ";" #y "m"// x: 背景，y: 前景
#define CLOSE "\033[0m"                  // 关闭所有属性

// 命令行主函数
int main(int 实参计数, char **实参值) {
    char *mod_path;       // Wasm 模块文件路径
    uint8_t *字节们 = NULL;// Wasm 模块文件映射的内存
    int 字节_计数;       // Wasm 模块文件映射的内存大小
    char *line = NULL;    // 指向每行输入的字符串的指针
    int res;              // 调用函数过程中的返回值，true 表示函数调用成功，false 表示函数调用失败

    // 如果参数数量不为 2，则报错并提示正确调用方式，然后退出
    if (实参计数 != 2) {
        fprintf(stderr, "The right usage is:\n%s WASM_FILE_PATH\n", 实参值[0]);
        return 2;
    }

    // 第二个参数即 Wasm 文件路径
    mod_path = 实参值[1];

    // 加载 Wasm 模块，并映射到内存中
    字节们 = 模映射_文件(mod_path, &字节_计数);

    // 如果 Wasm 模块文件映射的内存为 NULL，则报错提示
    if (字节们 == NULL) {
        fprintf(stderr, "Could not load %s", mod_path);
        return 2;
    }

    // 解析 Wasm 模块，即将 Wasm 二进制格式转化成内存格式
    网汇模块 *m = 载入_模块(字节们, 字节_计数);

    // 无限循环，每次循环处理单行命令
    while (1) {
        line = readline(BEGIN(49, 34) "wasmc$ " CLOSE);

        // 指向每行输入的字符串的指针仍为 NULL，则退出命令行
        if (!line) {
            break;
        }

        // 如果输入的字符串为 quit，则退出命令行
        if (strcmp(line, "quit") == 0) {
            free(line);
            break;
        }

        // 将输入的字符串加入到历史命令
        add_history(line);

        // 参数个数初始化为 0
        实参计数 = 0;

        // 将输入的字符串按照空格拆分成多个参数
        实参值 = 拆分_实参值(line, &实参计数);

        // 如果没有参数，则继续下一个循环
        if (实参计数 == 0) {
            continue;
        }

        // 重置运行时相关状态，主要是清空操作数栈、调用栈等
        m->栈指针 = -1;
        m->框指针 = -1;
        m->调用栈指针 = -1;

        // 通过名称（即第一个参数）从 Wasm 模块中查找同名的导出函数
        控制块 *func = 获取_导出(m, 实参值[0]);

        // 如果没有查找到函数，则报错提示信息，并进入下一个循环
        if (!func) {
            错误("no exported 函数之 named '%s'\n", 实参值[0])
            continue;
        }

        // 解析函数参数，并将参数压入到操作数栈
        解析_实参们(m, func->类型, 实参计数 - 1, 实参值 + 1);

        // 调用指定函数
        res = 援引(m, func->函索引);

        // 如果 援引 函数返回 true，则说明函数成功执行，
        // 在判断函数是否有返回值，如果有返回值，则将返回值打印出来；
        // 如果 援引 函数返回 true，则说明函数执行过程中出现异常，将异常信息打印出来即可。
        // 注：在解释执行函数过程中，如果有异常，会将异常信息写入到 异常 中
        if (res) {
            if (m->栈指针 >= 0) {
                printf("%s\n", 值_代表(&m->栈[m->栈指针]));
                // 刷新标准输出缓冲区，把输出缓冲区里的东西打印到标准输出设备上，已实现及时获取执行结果
                fflush(stdout);
            }
        } else {
            错误("Exception: %s\n", 异常)
        }

        // readline 会为输入的字符串动态分配内存，所以使用完之后需要将内存释放掉
        free(line);
    }

    return 0;
}
