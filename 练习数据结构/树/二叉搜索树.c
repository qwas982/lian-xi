#include <stdio.h>
#include <stdlib.h>

typedef struct 节点{
	struct 节点 *左;
	struct 节点 *右;
	int 数据;
}节点;

节点 *新节点(int 数据){
	节点 *终端 = (节点 *)malloc(sizeof(节点));
	终端 -> 数据 = 数据;
	终端 -> 左 = NULL;
	终端 -> 右 = NULL;
	return 终端;
}

节点 *插入(节点 *根,int 数据){
	if(根 == NULL){
		根 = 新节点(数据);
	}else if(数据 > 根 -> 数据){
		根 -> 右 = 插入(根 -> 右,数据);
	}else if(数据 < 根 -> 数据){
		根 -> 左 = 插入(根 -> 左,数据);
	}
	return 根;
}

节点 *取最大(节点 *根){
	if(根 -> 右 == NULL){
		return 根;
	}else{
		根 -> 右 = 取最大(根 -> 右);
	}
	return 0;
}

节点 *删除(节点 *根,int 数据){
	if(根 == NULL){
		return 根;
	}else if(数据 > 根 -> 数据){
		根 -> 右 = 删除(根 -> 右,数据);
	}else if(数据 < 根 -> 数据){
		根 -> 左 = 删除(根 -> 左,数据);
	}else if(数据 == 根 -> 数据){
		if((根 -> 左 == NULL) && (根 -> 右 == NULL)){
			free(根);
			return NULL;
		}else if(根 -> 左 == NULL){
			节点 *终端 = 根;
			根 = 根 -> 右;
			free(终端);
			return 根;
		}else if(根 -> 右 == NULL){
			节点 *终端 = 根;
			根 = 根 -> 左;
			free(终端);
			return 根;
		}else{
			节点 *终端 = 取最大(根 -> 左);
			根 -> 数据 = 终端 -> 数据;
			根 -> 左 = 删除(根 -> 左,终端 ->数据);
		}
	}
	return 根;	
}

int 找(节点 *根,int 数据){
	if(根 == NULL){
		return 0;
	}else if(数据 > 根 -> 数据){
		return 找(根 -> 右,数据);
	}else if(数据 < 根 -> 数据){
		return 找(根 -> 左,数据);
	}else if(数据 == 根 -> 数据){
		return 1;
	}
	return 0;
}

int 高(节点 *根){
	if(根 == NULL){
		return 0;
	}else{
		int 右高 = 高(根 -> 右);
		int 左高 = 高(根 -> 左);
		if(右高 > 左高){
			return (右高 + 1);
		}else{
			return (左高 + 1);
		}
	}
}

void 消除(节点 *根){
	if(根 != NULL){
		if(根 -> 左 != NULL){
			消除(根 -> 左);
		}
		if(根 -> 右 != NULL){
			消除(根 -> 右);
		}
		free(根);
	}
}

void 中序(节点 *根){
	if(根 != NULL){
		中序(根 -> 左);
		printf("\t[ %d ]\t",根 -> 数据);
		中序(根 -> 右);
	}
}

int main(){

	节点 *根 = NULL;
	int 操作 = -1;
	int 数据 = 0;

	while(操作 != 0){
		printf("\n \
			[1]插入节点,\n \
			[2]删除节点,\n \
			[3]找一节点,\n \
			[4]取当前高,\n \
			[5]打印树在月形序里,\n \
			[0]退出,\n");
		scanf("%d",&操作);
		switch(操作){
			case 1:
				printf("输入新节点的值: \n");
				scanf("%d",&数据);
				根 = 插入(根,数据);
				break;
			case 2:
				printf("输入值去移除: \n");
				if(根 != NULL){
					scanf("%d",&数据);
					根 = 删除(根,数据);
				}else{
					printf("树已经空了! \n");
				}
				break;
			case 3:
				printf("输入搜索值: \n");
				scanf("%d",&数据);
				找(根,数据) ? printf("值在树里. \n") : printf("值不在树里. \n");
				break;
			case 4:
				printf("当前树的高是: %d \n",高(根));
				break;
			case 5:
				中序(根);
				break;
		}
	}
	消除(根);
}