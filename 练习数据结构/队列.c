#include <stdio.h>
#include <stdlib.h>

struct 节点{
	int 数据;
	struct 节点 *下一个;
	struct 节点 *前一个;
} *头部,*尾部,*终端;

int 计数;

void 创建();
void 进队列(int 数);
int 出队列();
int 窥视();
int 大小();
int 是否空();

void 创建(){
	头部 = NULL;
	尾部 = NULL;

}

void 进队列(int 数){
	if (头部 == NULL){
		头部 = (struct 节点 *)malloc(1 * sizeof(struct 节点));
		头部 -> 数据 = 数;
		头部 -> 前一个 = NULL;
		尾部 = 头部;
		printf("计数 : %d \n",尾部);
	}else{
		终端 = (struct 节点 *)malloc(1 * sizeof(struct 节点));
		终端 -> 数据 = 数;
		终端 -> 下一个 = 尾部;
		尾部 = 终端;
		printf("计数 : %d\n", 尾部);

	}
}

int 出队列(){
	int 返回数据 = 0;
	if (头部 == NULL){
		printf("错误: 从空队列出队列.\n");
		exit(1);
	}else{
		返回数据 = 头部->数据;
		if(头部 -> 前一个 == NULL)
			头部 = NULL;
		else
			头部 = 头部 -> 前一个;
		头部 -> 下一个 = NULL;
	}
	return 返回数据;
}

int 大小(){
	return 计数;
}

int main(int 静态参数, char const *动态参数[])
{
	创建();
	进队列(5);
	//大小();
	//出队列();
	
	return 0;
}