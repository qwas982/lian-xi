#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//结构和枚举定义

//牌类型 
enum{
	牌名_文件终,
	牌名_加,牌名_减,牌名_星,
	牌名_斜杠,牌名_整型字面,
};

//牌结构
struct 牌{
	int 牌;
	int 整型值;
};

//抽象句法树 节点 类型
enum{
	抽象_加法,抽象_减法,抽象_乘法,抽象_除法,
	抽象_整型字面,
};

//抽象句法树结构
struct 抽象句法树节点{
	int 操作;  //在此树上进行"操作"
	struct 抽象句法树节点 *左;  //左与右孩子树
	struct 抽象句法树节点 *右;
	int 整型值;  //为 抽象_整型字面, 的整数值
};