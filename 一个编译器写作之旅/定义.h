#include <stdlib.h>  // 标准库头文件
#include <stdio.h>  // 标准输入输出头文件
#include <string.h>  // 字符串操作头文件
#include <ctype.h>  // 字符分类函数头文件


// 结构体和枚举定义


#define 文本长度		512	//输入中符号的长度

// 定义符号常量，表示不同的 token 类型
enum {
  牌子_EOF, 牌子_加号, 牌子_减号, 牌子_星号, 牌子_斜杠, 牌子_整型字面, 牌子_分号, 牌子_印
};

// 定义结构体 token，表示一个 token（词法单元）
struct 牌 {
  int 牌;  // token 类型，取值为上面定义的符号常量之一
  int 整型值;  // 整型字面量的值，对于其他类型的 token，该字段无意义
};

// AST node types 抽象句法树节点类型
enum {
  抽象_加,          // addition 加法
  抽象_减,     // subtraction 减法
  抽象_乘,     // multiplication 乘法
  抽象_除,       // division 除法
  抽象_整型字面        // integer literal 整数字面量
};

// Abstract Syntax Tree structure 抽象句法树结构
struct 抽象句法树节点 {
  int 操作;             // "Operation" to be performed on this tree 该节点要执行的操作
  struct 抽象句法树节点 *左子树;    // Left and right child trees 左右子树
  struct 抽象句法树节点 *右子树;
  int 整型值;       // For 抽象_整型字面, the integer value 如果该节点是整数字面量类型，表示该节点的整数值
};

