#ifndef __列表__
#define __列表__

#define 列 列表T
typedef struct 列 *列;

struct 列{
	void *值;
	列 下一个;
};

extern 列 列表初始化(void);
extern 列 列表推(列 列表,void *值);
extern int 列表长度(列 列表);
extern void **列表到数组(列 列表);
extern 列 列表追加(列 列表,列 尾部);
extern 列 列表的列表(列 列表,void *值,...);
/*还没做的*/
extern 列 列表复制(列 列表);
extern int 列表弹(列 *列表);

#undef 列
#endif