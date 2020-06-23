#include <stdio.h>
#include <stdlib.h>

typedef struct 节点{
	int 数据;
	struct 节点 *左;
	struct 节点 *右;
}*二叉树;

二叉树 初始化二叉树(二叉树 树);
二叉树 后序遍历(二叉树 树);
二叉树 显示树(二叉树 树);
二叉树 树深(二叉树 树);
二叉树 创建树(二叉树 树);