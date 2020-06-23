#include <stdio.h>
#include <stdlib.h>

struct 节点{
	struct 节点 *左子树;
	int 数据;
	struct 节点 *右子树;
};

struct 节点 *新节点(int 数据){
	struct 节点 *节点 = (struct 节点 *)malloc(sizeof(struct 节点));
	节点 -> 左子树 = NULL;
	节点 -> 数据 = 数据;
	节点 -> 右子树 = NULL;
	return 节点;
}

int main(void){
	/* 这里可以创建新节点
	struct 节点 *节点的名字 = 新节点(数据);

	
	树可以通过创建更多的节点来形成,节点的名字 -> 左子树, 
	以此类推*/
}