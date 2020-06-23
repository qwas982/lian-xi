#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "链栈.h"

#define T 栈T
typedef struct 元素{
	void *值;
	struct 元素 *下一个;
}元素t;

struct T{
	int 计数;
	元素t *头部;
};

T 栈初始化(void){
	T 栈;
	栈 = (T) malloc(sizeof(T));
	栈->计数 = 0;
	栈->头部 = NULL;
	return 栈;
}

int 栈空(T 栈){
	assert(栈);
	return 栈->计数 == 0;
}

int 栈大小(T 栈){
	assert(栈);
	return 栈->计数;
}

void 栈推(T 栈,void *值){
	元素t *t;
	assert(栈);
	t = (元素t *) malloc(sizeof(元素t));
	t->值 = 值;
	t->下一个 = 栈->头部;
	栈->头部 = t;
	栈->计数++;
}

void *栈弹(T 栈){
	void *值;
	元素t *t;

	assert(栈);
	assert(栈->计数 > 0);
	t = 栈->头部;
	栈->头部 = t->下一个;
	栈->计数--;
	值 = t->值;
	free(t);
	return 值;
}

void 栈打印(栈T 栈){
	assert(栈);
	int 甲, 大小 = 栈大小(栈);
	元素t *当前元素 = 栈->头部;
	printf("栈 [顶部 --- 底部]: ");
	for(甲 = 0;甲 < 大小;++甲){
		printf("%p ",(int *)当前元素->值);
		当前元素 = 当前元素 -> 下一个;
	}
	printf("\n");
	
}