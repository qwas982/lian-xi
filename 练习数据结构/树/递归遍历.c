#include <stdio.h>

void 中序遍历(struct 节点 *节点){
	if(节点 == NULL) //若树为空
		return;
	中序遍历(节点 -> 左子树);
	printf("\t %d \t",节点 -> 数据);
	中序遍历(节点 -> 右子树);
}

void 前序遍历(struct 节点 *节点){
	if(节点 == NULL)
		return;
	printf("\t %d \t",节点 -> 数据);
	前序遍历(节点 -> 左子树);
	前序遍历(节点 -> 右子树);
}

void 后序遍历(struct 节点 *节点){
	if(节点 == NULL)
		return;
	后序遍历(节点 -> 左子树);
	后序遍历(节点 -> 右子树);
	printf("\t %d \t",节点 -> 数据);
}

int main(void){
	/*遍历可以通过简单地调用函数,
	该函数具有指向根节点的指针.*/
	return 0;
}