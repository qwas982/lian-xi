#include <stdio.h>
#include <stdlib.h>

#include "栈机器.h"

#define 栈大小 (256)
#define 弹错误 (0x7fffffff)

int 操作栈[栈大小];
int 操作客栈 = 0;

指令 指令内存[代码大小];
int 程序计数器;

int 数据内存[数据大小];

enum 错误代码{
	骤停,好吧,错误除法通过零,错误数据内存,错误指令内存,错误栈溢出,错误栈空,
	错误未知操作码
};

void 错误(const char *错){
	fprintf(stderr,错);
}

int 取操作数计数(int 操作){
	int 返;
	switch(操作){
		case 操作码载值:
		case 操作码小于跳:
		case 操作码小等跳:
		case 操作码大于跳:
		case 操作码大等跳:
		case 操作码等于跳:
		case 操作码不等跳:
		case 操作码跳:
			返 = 1;
			break;
		default:
			返 = 0;
	}
	return 返;
}

int 推操作入栈(int 甲){
	if(操作客栈 >= 栈大小){
		错误("栈溢出");
		return -1;
	}
	操作栈[操作客栈++] = 甲;
	return 0;
}

int 弹操作出栈(){
	if(操作客栈 == 0){
		错误("栈空");
		return 弹错误;
	}
	return 操作栈[--操作客栈];
}

int 操作栈顶(){
	if(操作客栈 == 0){
		错误("栈空");
		return 弹错误;
	}
	return 操作栈[操作客栈-1];
}

#define 增操作(甲子) 代码 += sizeof(甲子); 大小 -= sizeof(甲子)

int 读指令(const char *代码,int 大小){
	int 操作计数, 列 = 0;
	指令 小指令;
	while(大小 > 0 && 列 < 代码大小){
		小指令.操作码 = *代码;
		增操作(char);
		操作计数 = 取操作数计数(小指令.操作码);
		if(操作计数 > 0){
			小指令.操作数 = *(int *)代码;
			增操作(int);
		}else{
			小指令.操作数 = 0;
		}
		指令内存[列++] = 小指令;
	}
	return 1;
}

int 步进运行(){
	指令 *星指令 = &指令内存[程序计数器++];
	int 返 = 好吧;
	switch(星指令->操作码){
		case 操作码骤停:
			{
				返 = 骤停;
			}
			break;
		case 操作码入:
			{
				int 甲;
				printf("输入:");
				scanf("%d",&甲);
				推操作入栈(甲);
			}
			break;
		case 操作码出:
			{
				int 甲 = 弹操作出栈();
				printf("输出:%d\n",甲);
			}
			break;
		case 操作码加:
			{
				int 子 = 弹操作出栈();
				int 丑 = 弹操作出栈();
				推操作入栈(丑 + 子);
			}
			break;
		case 操作码减:
			{
				int 子 = 弹操作出栈();
				int 丑 = 弹操作出栈();
				推操作入栈(丑 - 子);
			}
			break;
		case 操作码乘:
			{
				int 子 = 弹操作出栈();
				int 丑 = 弹操作出栈();
				推操作入栈(丑 * 子);
			}
			break;
		case 操作码除:
			{
				int 子 = 弹操作出栈();
				int 丑 = 弹操作出栈();
				if(子 == 0){
					return 错误除法通过零;
				}
				推操作入栈( 丑 / 子);
			}
			break;
		case 操作码复写:
			{
				推操作入栈(操作栈顶());
			}
			break;
		case 操作码载:
			{
				int 地址 = 弹操作出栈();
				if(地址 < 0 || 地址 >= 数据大小){
					错误("数据内存访问错误");
					return 错误数据内存;
				}else{
					推操作入栈(数据内存[地址]);
				}
			}
			break;
		case 操作码存:
			{
				int 值 = 弹操作出栈();
				int 地址 = 弹操作出栈();
				if(地址 < 0 || 地址 >= 数据大小){
					错误("数据内存访问错误");
					return 错误数据内存;
				}else{
					数据内存[地址] = 值;
				}
			}
			break;
		case 操作码载值:
			{
				推操作入栈(星指令 -> 操作数);
			}
			break;
		case 操作码小于跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 < 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码小等跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 <= 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码大于跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 > 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码大等跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 >= 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码等于跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 == 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码不等跳:
			{
				int 甲 = 弹操作出栈();
				if(甲 != 0){
					程序计数器 = 星指令 -> 操作数;
				}
			}
			break;
		case 操作码跳:
			{			
				程序计数器 = 星指令 -> 操作数;			
			}
			break;
		default:
			返 = 错误未知操作码;
	}
	return 返;
}

void 跑(){
	int 返 = 好吧;
	while(返 == 好吧){
		返 = 步进运行();
	}
}

int 文件大小(FILE *文件指针){
	int 大小;
	fseek(文件指针,0,SEEK_SET);
	fseek(文件指针,0,SEEK_END);
	大小 = ftell(文件指针);
	fseek(文件指针,0,SEEK_SET);
	return 大小;
}

extern void 反汇编出投掷(const char *文件,const 指令 *星指令,int 大小);

int main(int 参计数,char **参矢量){
	FILE *文件指针;
	char *缓冲;
	int 大小;
	if(参计数 < 2){
		错误("用法: 栈机器 <文件名>");
		exit(-1);
	}
	文件指针 = fopen(参矢量[1],"rb");
	if(文件指针 == 0){
		错误("打开文件失败");
		exit(-1);
	}
	大小 = 文件大小(文件指针);
	缓冲 = (char *)malloc(大小);
	fread(缓冲,大小,1,文件指针);

	读指令(缓冲,大小);
	if(参计数 > 2){
		int 数据旗 = atoi(参矢量[2]);
		if(数据旗){
			反汇编出投掷(参矢量[1],指令内存,代码大小);
		}
	}
	跑();
	free(缓冲);
	fclose(文件指针);
	return 0;
}