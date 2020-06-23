#include <stdio.h>
#include <string.h>

#include "栈机器.h"

extern int 取操作数计数(int 操作码);

const char *操作码描述[] = {
	"骤停","入","出","加","减","乘","除",
	"复写",
	"载","存","载值",
	"小于跳","小等跳","大于跳","大等跳","等于跳","不等跳","跳",0
};

void 反汇编出投掷(const char *文件,const 指令 *星指令,int 大小){
	FILE *文件指针;
	char 小文件[256];
	int 甲;
	strcpy(小文件,文件);
	strcat(小文件,".dasm");
	文件指针 = fopen(小文件,"w");
	for(甲 = 0;甲 < 大小 && 星指令[甲].操作码 != 操作码骤停;++甲){
		fprintf(文件指针, "%3d\t%s",甲,操作码描述[星指令[甲].操作码] );
		if(取操作数计数(星指令[甲].操作码) > 0){
			fprintf(文件指针, "\t%d",星指令[甲].操作数 );
		}
		fputc('\n',文件指针);
	}
	fclose(文件指针);
}

