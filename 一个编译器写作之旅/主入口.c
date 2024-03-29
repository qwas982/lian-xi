#include "定义.h"  // 包含自定义头文件，提供了一些宏定义和函数声明
#define 外部的_  // 定义宏 extern_
#include "数据.h"  // 包含自定义头文件，定义了一些全局变量
#undef 外部的_  // 取消宏 extern_ 的定义
#include "声明.h"  // 包含自定义头文件，定义了一些函数
#include <errno.h>  // 包含错误处理头文件，提供了处理错误的函数和变量

// 编译器设置与顶层执行

// 初始化全局变量
static void 初始化() {
  行号 = 1;  // 初始化全局变量 Line
  放回 = '\n';  // 初始化全局变量 Putback
}

// 如果程序启动不正确，则打印使用方法
static void 用法(char *程序) {
  fprintf(stderr, "用法: %s 入文件\n", 程序);  // 打印使用方法
  exit(1);  // 退出程序
}

/* // 可打印的 token 列表
char *牌的串[] = { "+", "-", "*", "/", "整型字面" };

// 循环扫描输入文件中的所有 token。
// 打印找到的每个 token 的详细信息。
static void 扫描文件() {
  struct 牌 牌子;

  while (扫描(&牌子)) {
    printf("牌 %s", 牌的串[牌子.牌]);  // 打印 token 类型
    if (牌子.牌 == 牌子_整型字面)
      printf(", 值 %d", 牌子.整型值);  // 打印整型字面量的值
    printf("\n");  // 换行
  }
} */

// 主程序：检查参数并打印使用方法，如果没有参数的话。
// 打开输入文件并调用 scanfile() 扫描其中的 token。
int main(int 实参计数, char *实参值[]) {
  //struct 抽象句法树节点 *节点指针;

  if (实参计数 != 2)  // 如果参数数量不为 2
    用法(实参值[0]);  // 打印使用方法并退出程序。

  初始化();  // 初始化全局变量

// 打开输入文件
  if ((入文件 = fopen(实参值[1], "r")) == NULL) {  // 打开输入文件
    fprintf(stderr, "无法打开 %s: %s\n", 实参值[1], strerror(errno));  // 打印错误信息
    exit(1);  // 退出程序
  }

  // 创建输出文件
  // 如果无法打开名为 out.s 的文件，则输出错误信息并退出程序
  if ((出文件 = fopen("输出.S", "w")) == NULL) {
    fprintf(stderr, "无法创建 输出.S: %s\n", strerror(errno));
    exit(1);
  }

  // 扫描文件();  // 扫描输入文件中的 token
  
  扫描(&大写_牌); // 获取输入的第一个标记,在此处调用名为“scan”的函数，该函数将从输入中读取下一个标记，并将其存储在名为“Token”的全局变量中
  
  生成预漫步();  // 输出汇编代码的前导部分
  语句();   // 解析输入中的语句
  生成后漫步(); // 输出汇编代码的尾随部分
  fclose(出文件); // 关闭输出文件并退出

  exit(0);  // 退出程序
}