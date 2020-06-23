#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "动态数组.h"

动态数组t *初始化动态数组(){
	动态数组t *动数 = malloc(sizeof(动态数组t));
	动数 -> 项目 = calloc(默认容量,sizeof(void *));
	动数 -> 容量 = 默认容量;

	return 动数;
}

void *添加(动态数组t *动数,const void *值){
	if(动数 -> 大小 >= 动数 -> 容量){
		void **新项目 = realloc(动数 -> 项目,(动数->容量 <<= 1) * sizeof(void **));
		free(动数->项目);
		动数->项目 = 新项目;
	}
	void *复制值 = 索取复制的值(值);
	动数-> 项目[动数->大小++] = 复制值;
	return 复制值;
}

void *投掷(动态数组t *动数,const void *值,const unsigned 索引){
	if(!包含(动数 -> 大小,索引)){
		return 索引出界;
	}
	free(动数 -> 项目[索引]);
	void *复制值 = 索取复制的值(值);
	动数-> 项目[索引] = 复制值;
	return 复制值;
}

void *取(动态数组t *动数,const unsigned 索引){
	if(!包含(动数 -> 大小,索引)){
		return 索引出界;
	}
	return 动数 -> 项目[索引];
}

void 删除(动态数组t *动数,const unsigned 索引){
	if(!包含(动数 -> 大小,索引)){
		return;
	}
	for(unsigned 甲 = 索引;甲 < 动数->大小;甲++){
		动数-> 项目[甲] = 动数 -> 项目[甲+1];
	}
	动数->大小--;
	free(动数->项目[动数->大小]);
}

unsigned 包含(const unsigned 大小,const unsigned 索引){
	if(大小 >= 0 && 索引 < 大小){
		return 1;
	}
	printf("索引 [%d] 出界! \n",索引);
	return 0;
}

void *索取复制的值(const void *值){
	void *复制值 = malloc(sizeof(void *));
	memcpy(复制值,值,sizeof(void *));
	return 复制值;
}