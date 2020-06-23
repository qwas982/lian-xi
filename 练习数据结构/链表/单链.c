#include <stdio.h>
#include <stdlib.h>

struct 节点{
	int 信息;
	struct 节点 *链;
};

struct 节点 *开始 = NULL;
struct 节点 *创建节点(){
	struct 节点 *节;
	节 = (struct 节点*)malloc(sizeof(struct 节点));
	return (节);
}

void 插入(){
	struct 节点 *针;
	针 = 创建节点();
	printf("键入你想插入链表的号码: ");
	scanf("%d",&针 -> 信息);
	针 -> 链 = NULL;

	if(开始 == NULL){
		开始 = 针;
	}else{
		针 -> 链 = 开始;
		开始 = 针;
	}
}

void 删除(){
	struct 节点 *针;
	if(开始 == NULL){
		printf("链是空的\n");
	}else{
		struct 节点 *针;
		针 = 开始;
		开始 = 开始 -> 链;
		free(针);
	}
}

void 显示链(){
	struct 节点 *针;
	if(开始 == NULL){
		printf("链是空的\n");
	}else{
		针 = 开始;
		while(针 != NULL){
			printf("当前链表中的元素: %d .\n",针 -> 信息);
			针 = 针 -> 链;
		}
	}
}

void 退出(){
	printf("已退出 \n");
	exit(0);
}

int main(){
	int 甲;
	while(1){
		printf("1,添加值在第一位置\n");
		printf("2,删除值从第一位置\n");
		printf("3,显示值\n");
		printf("4,退出\n");
		printf("进入你的选择\n");
		scanf("%d",&甲);
		switch(甲){
			case 1:
				插入();
				break;
			case 2:
				删除();
				break;
			case 3:
				显示链();
				break;
			case 4:
				退出();
				break;
			default:
				printf("无效选择\n");
		}
	}
	return (0);
}