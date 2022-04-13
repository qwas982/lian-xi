#include "定义.h"
#include "数据.h"
#include "声明.h"

//通用代码生成器

//给定一个抽象句法树,递归地生成汇编代码,
//返回此寄存器身份和其树的最终值一起,
int 生成抽象句法树(struct 抽象句法树节点 *节点,int 寄存){
	int 左寄存,右寄存;

	//取此左与右子树值
	if(节点->左)
		左寄存 = 生成抽象句法树(节点->左,-1);
	if(节点->右)
		右寄存 = 生成抽象句法树(节点->右,左寄存);

	switch(节点->操作){
		case 抽象_加法:
			return (代码生成器加(左寄存,右寄存));
		case 抽象_减法:
			return (代码生成器减(左寄存,右寄存));
		case 抽象_乘法:
			return (代码生成器乘(左寄存,右寄存));
		case 抽象_除法:
			return (代码生成器除(左寄存,右寄存));
		case 抽象_整型字面:
			return (代码生成器载入整型(节点->变.整型值));
		case 抽象_标识:
			return (代码生成器载入全局(全局符号[节点->变.身份].名字));
		case 抽象_声明变量标识:
			return (代码生成器存全局(寄存,全局符号[节点->变.身份].名字));
		case 抽象_赋值:
			//这项工作已经完成,返回其结果
			return (右寄存);
		default:
			致命数("未知抽象句法树操作",节点->操作);
	}
}

void 生成前文(){
	代码生成器前文();
}

void 生成后文(){
	代码生成器后文();
}

void 生成自由寄存(){
	自由全部_寄存器();
}

void 生成打印整型(int 寄存){
	代码生成器打印整型(寄存);
}

void 生成全局符号(char *符){
	代码生成器全局符号(符);
}