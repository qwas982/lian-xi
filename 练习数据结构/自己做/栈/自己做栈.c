#include <stdlib.h>
#include <stdio.h>

#include "自己做栈.h"

int 栈数组[100];


栈 *初始化栈(void){
	栈 *小栈 = (栈 *)malloc(sizeof(栈));
	printf("\n 栈已初始化: %p \n",小栈);
	return 小栈;
}

void *释放栈(栈 *小栈){
	free(小栈);
	return 0;
}

int 显示(){
	//栈 *针;
	栈 小栈;
	printf("\n \
		---------- \n \
		| %d \n \
		---------- \n",栈数组[小栈.栈顶指针]);
	return 0;
}

int 是否空(){
	栈 小栈;
	if(小栈.栈顶指针 == 小栈.栈基指针){
		return printf("栈是空的 \n");
		显示();
	}else{
		return printf("栈里有数据 \n");
		显示();
	}
}

int 是否满(){
	栈 小栈;
	if(小栈.栈顶指针 >= *栈数组){
		return printf("栈已满 \n");
		显示();
	}else{
		return printf("栈未满 \n");
		显示();
	}
}

int 推入(int 数据){
	if(是否满()){
		printf("栈已满,无法推入数据 \n");
	}else{
		int 索引;
		栈 小栈;
		小栈.栈顶指针 += 1;
		栈数组[索引] = 小栈.栈顶指针;
		栈数组[索引] = 数据;
		显示();
	}
	return 0;
}

int 弹出(){
	if(是否空()){
		printf("栈是空的,没有数据可弹出 \n");
	}else{
		int 索引;
		栈 小栈;
		小栈.栈顶指针 -= 1;
		栈数组[索引] = 小栈.栈顶指针;
		return 栈数组[索引];
	}
	显示();
	return 0;
}