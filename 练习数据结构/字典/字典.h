#ifndef __字典__头
#define __字典__头

#define 最大元素 1000

typedef struct 典{
	void *元素[最大元素];
	int 元素号码;
}字典;

字典 *创建字典(void);
int 加项目标签(字典 *,char 标签[],void *);
int 加项目索引(字典 *,int 索引,void *);
void *取元素标签(字典 *,char []);
void *取元素索引(字典 *,int);
void 摧毁(字典 *);

#endif