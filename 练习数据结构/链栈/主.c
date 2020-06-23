#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "链栈.h"

int main(){
	栈T 小栈;
	小栈 = 栈初始化();
	栈推(小栈,(int *) 1);
	栈推(小栈,(int *) 2);
	栈推(小栈,(int *) 3);
	栈推(小栈,(int *) 4);
	printf("大小: %d\n",栈大小(小栈));
	栈打印(小栈);
	栈弹(小栈);
	printf("栈弹出后: \n");
	栈打印(小栈);
	栈弹(小栈);
	printf("栈弹出后: \n");
	栈打印(小栈);
	return 0;
}