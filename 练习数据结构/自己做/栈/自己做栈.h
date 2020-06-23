#include <stdio.h>
#include <stdlib.h>

typedef struct 自己做栈{
	int 栈顶指针;
	int 栈基指针;
	//int 栈数组[512];
}栈;

栈 *初始化栈(void);
int 推入(int 数据);
int 弹出();
int 显示();
void *释放栈(栈 *小栈);
int 是否空();
int 是否满();