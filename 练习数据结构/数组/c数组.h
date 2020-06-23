#define 抹去数组 -1
#define 成功 0
#define 无效位置 1
#define 位置初始 2
#define 位置不初始 3
#define 位置空 4
#define 数组满 5

typedef struct c数组{
	int *数组;
	int 大小;
}c数组;

//返回数组
c数组 *取c数组(int 大小);
c数组 *取复制c数组(c数组 *数组);

//输入/输出
int 插入值c数组(c数组 *数组,int 位置,int 值);
int 移除值c数组(c数组 *数组,int 位置);
int 推值c数组(c数组 *数组,int 值);
int 更新值c数组(c数组 *数组,int 位置,int 值);

//擦除
int 擦除c数组(c数组 *数组);

//切换
int 切换值c数组(c数组 *数组,int 位置1,int 位置2);
int 逆向c数组(c数组 *数组);

//排序
int 冒泡排序c数组(c数组 *数组);
int 选择排序c数组(c数组 *数组);
int 插入排序c数组(c数组 *数组);
int 混合排序c数组(c数组 *数组);

//查找
int 值杂狗支撑c数组(c数组 *数组,int 值);
c数组 *值位置c数组(c数组 *数组,int 值);
int 找最大c数组(c数组 *数组);
int 找最小c数组(c数组 *数组);

//显示
int 显示c数组(c数组 *数组);