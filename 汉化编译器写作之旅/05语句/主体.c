#include "定义.h"
#define 外部_
#include "数据.h"
#undef 外部_
#include "声明.h"
#include <errno.h>

//编译器设置与顶层级执行
//初始化全局变量
static void 初始(){
	行 = 1;
	放回去 = '\n';
}

//打印出用法,若开始后不正确
static void 用法(char *程序){
	fprintf(stderr,"用法: %s 文件内\n",程序);
	exit(1);
}


//主程序: 检查实参与打印用法,若我们没有实参,
//打开输入文件与调用 扫描文件() 去扫描其中的牌,
void main(int 实参计数,char *实参变量[]){
	

	if(实参计数 != 2)
		用法(实参变量[0]);

	初始();

	//打开输入文件
	if((入文件 = fopen(实参变量[1],"r")) == NULL){
		fprintf(stderr,"无法打开 %s: %s\n",实参变量[1],strerror(errno));
		exit(1);
	}

	//创建输出文件
	if((出文件 = fopen("出.s","w")) == NULL){
		fprintf(stderr,"无法创建 出.s: %s\n", strerror(errno));
		exit(1);
	}

	//扫描文件();
	扫描(&大牌);  //取第一个牌来自输入
	生成前文();  //输出前文/前言/序言
	语句();  //解析输入的语句
	生成后文();  //输出后文
	fclose(出文件);  //关闭输出文件与退出
	exit(0);
}