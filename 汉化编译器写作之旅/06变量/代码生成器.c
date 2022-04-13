#include "定义.h"
#include "数据.h"
#include "声明.h"

//代码生成器为x86-64

//可用寄存器的列表与他们的名字
static int 自由寄存[4];
static char *寄存列表[4] = {"%寄存8","%寄存9","%寄存10","%寄存11"};

//设置全部寄存器为可用,
void 自由全部_寄存器(void){
	自由寄存[0] = 自由寄存[1] = 自由寄存[2] = 自由寄存[3] = 1;
}

//分配一个自由寄存器,返回此寄存器的号码,
//若无可用寄存器就死,
static int 分配_寄存器(void){
	for(int 甲 = 0; 甲 < 4; 甲++){
		if(自由寄存[甲]){
			自由寄存[甲] = 0;
			return (甲);
		}
	}
	致命("出寄存器");
}

//返回一个寄存器到此列表的可用寄存器,
//检查看它是否在那儿,
static void 自由_寄存器(int 寄存){
	if(自由寄存[寄存] != 0){
		致命数("错误尝试去自由寄存器",寄存);
	}
	自由寄存[寄存] = 1;
}

//打印出此汇编前文,
void 代码生成器前文(){
	自由全部_寄存器();
	fputs(
		"\t.文本\n"
		".LC0:\n"
		"\t.串\t\"%d\\n\"\n"
		"打印整型:\n"
		"\tpushq\t%rbp\n"
		"\tmovq\t%rsp,%rbp\n"
		"\tsubq\t$16,%rsp\n"
		"\tmovl\t%edi,-4(%rbp)\n"
		"\tmovl\t-4(%rbp),%eax\n"
		"\tmovl\t%eax,%esi\n"
		"\tleaq .LC0(%rip),%rdi\n"
		"\tmovl $0,%eax\n"
		"\tcall printf@PLT\n"
		"\tnop\n"
		"\tleave\n"
		"\tret\n"
		"\n"
		"\t.全局\tmain\n"
		"\t.类型\tmain, @function\n"
		"main:\n"
		"\tpushq\t%rbp\n"
		"\tmovq %rsp,%rbp\n",
	出文件);
}

//打印出此汇编后文,
void 代码生成器后文(){
	fputs(
		"\tmovl $0, %eax\n"
		"\tpopq %rbp\n"
		"\tret\n",
	出文件);
}

//载入一个整数字面值进到一个寄存器,
//返回此寄存器的号码,
int 代码生成器载入整型(int 值){
	//取一个新寄存器
	int 寄存 = 分配_寄存器();

	//打印出此代码去初始化它,
	fprintf(出文件,"\tmovq\t$%d,%s\n",值,寄存列表[寄存]);
	return (寄存);
}

//载入值从变量进到寄存器,
//返回其寄存器号码,
int 代码生成器载入全局(char *标识符){
	//取一个新寄存器
	int 寄存 = 分配_寄存器();

	//打印出此代码去初始化它
	fprintf(出文件,"\tmovq\t%s(\%%rip),%s\n",标识符,寄存列表[寄存]);
	return (寄存);
}

//把两个寄存器加在一起与
//返回此寄存器的号码和结果一起,
int 代码生成器加(int 寄存1,int 寄存2){
	fprintf(出文件,"\taddq\t%s,%s\n",寄存列表[寄存1],寄存列表[寄存2]);
	自由_寄存器(寄存1);
	return (寄存2);
}

//从第一个寄存器减去第二个与
//返回此寄存器的号码和结果一起,
int 代码生成器减(int 寄存1,int 寄存2){
	fprintf(出文件,"\tsubq\t%s,%s\n",寄存列表[寄存2],寄存列表[寄存1]);
	自由_寄存器(寄存2);
	return (寄存1);
}

//将两个寄存器相乘与
//返回此寄存器的号码和结果一起,
int 代码生成器乘(int 寄存1,int 寄存2){
	fprintf(出文件,"\timulq\t%s,%s\n",寄存列表[寄存1],寄存列表[寄存2]);
	自由_寄存器(寄存1);
	return (寄存2);
}

//用第一个寄存器除以第二个与
//返回此寄存器的号码和结果一起,
int 代码生成器除(int 寄存1,int 寄存2){
	fprintf(出文件,"\tmovq\t%s,%%rax\n",寄存列表[寄存1]);
	fprintf(出文件,"\tcqo\n");
	fprintf(出文件,"\tidivq\t%s\n",寄存列表[寄存2]);
	fprintf(出文件,"\tmovq\t%%rax,%s\n",寄存列表[寄存1]);
	自由_寄存器(寄存2);
	return (寄存1);
}

//调用 打印整型() 和给定寄存器一起,
void 代码生成器打印整型(int 寄存){
	fprintf(出文件,"\tmovq\t%s,%%rdi\n",寄存列表[寄存]);
	fprintf(出文件,"\tcall\t打印整型\n");
	自由_寄存器(寄存);
}

//存一个寄存器的值进到一个变量
int 代码生成器存全局(int 寄存,char *标识符){
	fprintf(出文件, "\tmovq\t%s, %s(\%%rip)\n",寄存列表[寄存],标识符);
	return (寄存);
}

//生成一个全局符号
void 代码生成器全局符号(char *符号){
	fprintf(出文件,"\t.comm\t%s,8,8\n",符号);
}