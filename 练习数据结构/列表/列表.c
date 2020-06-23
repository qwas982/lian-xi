#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "列表.h"

#define 列 列表T

列 列表初始化(void){
	列 列表;
	列表 = (列)malloc(sizeof(列));
	列表->下一个 = NULL;
	return 列表;
}

列 列表推(列 列表,void *值){
	列 新元素 = (列)malloc(sizeof(列));
	新元素 -> 值 = 值;
	新元素 -> 下一个 = 列表;
	return 新元素;
}

int 列表长度(列 列表){
	int 甲;
	for(甲 = 0;列表;列表 = 列表 -> 下一个){
		甲++;
	}
	return 甲;
}

void **列表到数组(列 列表){
	int 甲,乙= 列表长度(列表);
	void **数组 = (void **)malloc((乙+1) *sizeof(*数组));
	for(甲 = 0;甲 < 乙;甲++){
		数组[甲] = 列表 -> 值;
		列表 = 列表 -> 下一个;

	}
	数组[甲] = NULL;
	return 数组;
}

列 列表的列表(列 列表,void *值, ...){
	va_list 安排;  //导入的类型无法用汉字标识符,
	列 *针 = &列表;
	va_start(安排,值); //导入的函数亦是如此,
	for(;值;值 = va_arg(安排,void *)){
		*针 = malloc(sizeof(列));
		(*针) -> 值 = 值;
		针 = &(*针) -> 下一个;

	}
	*针 = NULL;
	va_end(安排);
	return 列表;

}

列 列表追加(列 列表,列 尾部){
	列 *针= &列表;
	while((*针) -> 下一个){
		针 = &(*针)->下一个;

	}
	*针 = 尾部;
	return 列表;
}