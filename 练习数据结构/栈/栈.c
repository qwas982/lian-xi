#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "栈.h"

void **数组;
int 最大 = 10;
int 计数 = 0;
int 偏移 = -1;

void 初始化栈(){
	数组 = malloc(sizeof(void *) *最大);
	assert(数组);
}

void 增长(){
	最大 += 10;
	void **临时 = malloc(sizeof(void *) *最大);
	int 甲;
	for (int 甲 = 0; 甲 < 计数; 甲++){
		*(临时 + 甲 ) = *(数组 + 甲);
	}
	free(数组);
	临时 = 数组;
}

void 推(void *对象){
	assert(对象);
	if (计数 < 最大){
		偏移++;
		*(数组 + 偏移) = 对象;
		计数++;
	}
	else{
		增长();
		推(对象);
	}
}

void *弹(){
	void *顶部 = *(数组 + 偏移);
	assert(顶部);
	assert(!空吗());
	偏移--;
	计数--;
	return 顶部;

}

int 大小(){
	return 计数;
}

int 空吗(){
	return 计数 == 0;
}

void *顶部(){
	return 数组[偏移];
}