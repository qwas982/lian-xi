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

//可打印牌的列表
char *牌串[] = {"+","-","*","/","整型字面"};

//环形扫描输入文件内的所有牌,
//为每个找到的牌打印出细节,
static void 扫描文件(){
	struct 牌 牌名;
	while(扫描(&牌名)){
		printf("牌 %s",牌串[牌名.牌]);
		if(牌名.牌 == 牌名_整型字面)
			printf(",值 %d",牌名.整型值);
		printf("\n");
	}
}

//主程序: 检查实参与打印用法,若我们没有实参,
//打开输入文件与调用 扫描文件() 去扫描其中的牌,
void main(int 实参计数,char *实参变量[]){
	if(实参计数 != 2)
		用法(实参变量[0]);
	初始();
	if((文件内 = fopen(实参变量[1],"r")) == NULL){
		fprintf(stderr,"无法打开 %s: %s\n",实参变量[1],strerror(errno));
		exit(1);
	}
	扫描文件();
	exit(0);
}