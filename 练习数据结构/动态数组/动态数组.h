#ifndef __动态数组__
#define __动态数组__
#define 默认容量 1 << 4
#define 索引出界 NULL

typedef struct 动态数组{
	void **项目;
	unsigned 大小;
	unsigned 容量;
}动态数组t;

extern 动态数组t *初始化动态数组();
extern void *添加(动态数组t *动数,const void *值);
extern void *投掷(动态数组t *动数,const void *值,unsigned 索引);
extern void *取(动态数组t *动数,const unsigned 索引);
extern void 删除(动态数组t *动数,const unsigned 索引);
unsigned 包含(const unsigned 大小,const unsigned 索引);
extern void *索取复制的值(const void *值);

#endif