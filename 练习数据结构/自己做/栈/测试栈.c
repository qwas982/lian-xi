#include <stdlib.h>
#include <stdio.h>

#include "自己做栈.h"



int main(){

	初始化栈();
	显示();
	while(是否满()){
		int 进;
		scanf("输入一个值: %d \n",&进);
		推入(进);
		显示();
	}
	while(是否空()){
		printf("手动弹出数据,按1,\n");
		int 弹;
		switch(弹){
			case 1:
				弹出();
				显示();
				break;
			default :
				printf("错误,没有这个功能,\n");		
		}
			
	}
	return 0;
	
	

	
	//释放栈(栈数组);
	//显示(数组栈);
}