#include <stdio.h>
#include <stdlib.h>

#include "自己做二叉树.h"

/*二叉树 初始化二叉树(二叉树 树){
	树 = NULL;
	return 树;
}*/

二叉树 创建树(二叉树 树){
	int 结点;
	scanf("%d \n",&结点);
	树->数据 = 结点;
	树->左 = NULL;
	树->右 = NULL;
	树->左 = 创建树(树->左);
	树->右 = 创建树(树->右);
	return 树;
}

/*二叉树 显示树(二叉树 树){
	树 = NULL;
	printf("%p \n",创建树(树));
	return 树;
}*/


二叉树 后序遍历(二叉树 树){
	if(树){
		后序遍历(树->左);
		后序遍历(树->右);
		printf("%d \n",树->数据 );
	}
	return 0;
}