#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "列表.h"

void 打印列表(char **数组){
	int 甲;
	for(甲 = 0;数组[甲];甲++){
		printf("%s",数组[甲]);
	}
	printf("\n");

}

int main(){
	列表T 列表1,列表2,列表3;
	char **串1 = (char **)malloc(100 * sizeof(char *));
	列表1 = 列表初始化();
	列表1 = 列表推(列表1,"挡,");
	列表1 = 列表推(列表1,"红,");
	列表1 = 列表推(列表1,"海,");
	printf("列表1: ");
	串1 = (char **)列表到数组(列表1);
	打印列表(串1);

	列表2 = 列表初始化();
	列表2 = 列表的列表(列表2,"小明,","小图,","西蒙斯,",NULL);
	printf("列表2: ");
	打印列表((char **)列表到数组(列表2));

	列表3 = 列表追加(列表1,列表2);
	printf("测试追加列表2到列表1: ");
	打印列表((char **)列表到数组(列表3));

	return 0;
}