#include <stdlib.h>
#include <stdio.h>

#include "字典.h"

字典 *创建字典(void){
	字典 *字典指针 = malloc(sizeof(字典));
	if(字典指针){
		字典指针 -> 元素号码 = 0;
		for(int 甲 = 0;甲 < 最大元素;甲++){
			字典指针 -> 元素[甲] = NULL;
		}
		return 字典指针;
	}else{
		printf("不能创建字典.\n");
		return NULL;
	}
}

int 取哈希(char s[]){
	unsigned int 哈希代码 = 0;
	for(int 计数器 = 0;s[计数器] != '\0';计数器++){
		哈希代码 = s[计数器] + (哈希代码 << 6) + (哈希代码 << 16) - 哈希代码;
	}
	return 哈希代码 % 最大元素;
}

int 加项目标签(字典 * 字字,char 标签[],void *项目){
	unsigned int 索引 = 取哈希(标签);
	if(索引 < 最大元素){
		字字 -> 元素[索引] = 项目;
		return 0;
	}
	return -1;
}

int 加项目索引(字典 *字字,int 索引,void *项目){
	if(!字字->元素[索引]){
		字字 -> 元素[索引] = 项目;
		return 0;
	}
	return -1;
}

void *取元素标签(字典 *字字,char s[]){
	int 索引 = 取哈希(s);
	if(字字 -> 元素[索引]){
		return 字字 -> 元素[索引];
	}
	printf("无标签输入\n");
	return NULL;
}

void *取元素索引(字典 *字字,int 索引){
	if(索引 >= 0 && 索引 < 最大元素){
		return 字字 -> 元素[索引];
	}
	printf("索引出界!\n");
	return NULL;
}

void 摧毁(字典 *字字){
	free(字字);
}
