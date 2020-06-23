#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "栈机器.h"

const char *操作码描述[] = {
	"骤停","入","出","加","减","乘","除",
	"复写",
	"载","存","载值",
	"小于跳","小等跳","大于跳","大等跳","等于跳","不等跳","跳",0
};

FILE *文件指针入;
FILE *文件指针出;

指令 指令内存[代码大小];

//取操作码从它的串描述
int 取操作码(const char *串){
	int 甲 = 0;
	for(;操作码描述[甲] != 0;++甲){
		if(strcmp(操作码描述[甲],串) == 0){
			return 甲;
		}
	}
	return 操作码无效;
}

//取操作数计数
int 取操作数计数(int 操作码){
	int 返;
	switch(操作码){
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

void 读汇编(){
	char 行[256];
	char 操作码串[32];
	unsigned short 操作码;
	int 参数计数;
	int 操作数;
	unsigned short 列;
	while(!feof(文件指针入)){
		fgets(行,sizeof(行) - 1,文件指针入);
		sscanf(行,"%d%s",(int *)&列,操作码串);
		操作码 = (unsigned short)取操作码(操作码串);
		参数计数 = 取操作数计数(操作码);
		if(参数计数 > 0){
			char *串 = strstr(行,操作码串);
			串 = &串[strcspn(串,"\t") + 1];
			操作数 = atoi(串);
		}else{
			操作数 = 0;
		}
		指令内存[列].操作码 = 操作码;
		指令内存[列].操作数 = 操作数;
	}
}

void 出投掷(){
	int 列 = 0;
	int 参数计数;
	for(;指令内存[列].操作码 != 操作码骤停;++列){
		char 操作码 = (char)指令内存[列].操作码;
		fwrite(&操作码,sizeof(char),1,文件指针出);
		参数计数 = 取操作数计数(操作码);
		if(参数计数 > 0){
			int 操作数 = 指令内存[列].操作数;
			fwrite(&操作数,sizeof(操作数),1,文件指针出);
		}
	}
}

int main(int 参计数,char **参矢量){
	if(参计数 < 2){
		fprintf(stderr, "使用:%s<文件名>\n",参矢量[0]);
		exit(-1);
	}
	文件指针入 = fopen(参矢量[1],"r");
	if(文件指针入 == 0){
		fprintf(stderr, "打开%s失败\n",参矢量[1]);
		exit(-1);
	}
	{
		char 出投掷[256] = { 0 };
		int 了 = strcspn(参矢量[1],".");
		strncpy(出投掷,参矢量[1],了);
		strcat(出投掷,".zj");
		文件指针出 = fopen(出投掷,"wb");
		if(文件指针出 == 0){
			fprintf(stderr, "打开%s失败\n",出投掷);
			exit(-1);
		}
	}
	读汇编();
	出投掷();
	fclose(文件指针入);
	fclose(文件指针出);
	return 0;
}