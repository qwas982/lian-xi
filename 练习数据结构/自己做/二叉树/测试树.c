#include <stdio.h>
#include <stdlib.h>

#include "自己做二叉树.h"

int main(){
	二叉树 树结点;
	二叉树 树;
	//初始化二叉树(树结点);
	树 = 创建树(树结点);
	/*显示树(树结点);*/
	后序遍历(树);
}