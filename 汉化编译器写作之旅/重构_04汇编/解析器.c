/*------------------------------------------------
------  树          表达式             解释-------
------------------------------------------------*/

/*------------------------------------------------
------树 -------
------------------------------------------------*/
#include "定义.h"

//抽象句法树的树函数
//建造与返回一个通用抽象句法树节点
struct 抽象句法树节点 *制作抽象句法树节点(int 操作,struct 抽象句法树节点 *左,
			struct 抽象句法树节点 *右, int 整型值){
	struct 抽象句法树节点 *个数;
	//内存分配一个新的抽象句法树节点
	个数 = (struct 抽象句法树节点 *) malloc(sizeof(struct 抽象句法树节点));
	if(个数 == NULL){
		fprintf(stderr,"不能内存分配在制作抽象句法树节点()内\n");
		exit(1);
	}
	//复制字段内的值与返回它
	个数->操作 = 操作;
	个数->左 = 左;
	个数->右 = 右;
	个数->整型值 = 整型值;
	return (个数);
}

//制作一个抽象句法树叶节点
struct 抽象句法树节点 *制作抽象句法树叶(int 操作,int 整型值){
	return (制作抽象句法树节点(操作,NULL,NULL,整型值));
}

//制作一个一元抽象句法树节点: 只有一个孩子
struct 抽象句法树节点 *制作抽象句法树一元(int 操作,struct 抽象句法树节点 *左,int 整型值){
	return (制作抽象句法树节点(操作,左,NULL,整型值));
}


/*------------------------------------------------
------表达式 -------
------------------------------------------------*/


//解析表达式
//解析一个基元因子与返回一个抽象句法树节点代表它
static struct 抽象句法树节点 *基元(void){
	struct 抽象句法树节点 *个数;
	//为一个 整型字面 牌,制作一个叶抽象句法树节点为它
	//还要扫描下一个牌.另外,一个句法错误为任意其它牌类型.
	switch (大牌.牌){
		case 牌名_整型字面:
			个数 = 制作抽象句法树叶(抽象_整型字面,大牌.整型值);
			扫描(&大牌);
			return (个数);
		default:
			fprintf(stderr,"句法错误在行 %d, 牌 %d\n",行,大牌.牌);
			exit(1);
	}
}
//转换一个二元操作牌进到一个抽象句法树操作
int 算术操作(int 牌类型){
	switch (牌类型){
		case 牌名_加:
			return (抽象_加法);
		case 牌名_减:
			return (抽象_减法);
		case 牌名_星:
			return (抽象_乘法);
		case 牌名_斜杠:
			return (抽象_除法);
		default:
			fprintf(stderr,"句法错误在行 %d,牌 %d\n",行,牌类型);
			exit(1);
	}
}
//操作优先级为每个牌
static int 操作优先级[] = {0,10,10,20,20,0};

//检查我们是否有二元操作与返回其优先级
static int 操作_优先级(int 牌类型){
	int 优先级 = 操作优先级[牌类型];
	if(优先级 == 0){
		fprintf(stderr, "句法错误在行 %d, 牌 %d\n",行,牌类型);
		exit(1);
	}
	return (优先级);
}

//返回一个抽象句法树的树哪个的根是个二元操作
//形参 上牌优 是上一个牌的优先级,
struct 抽象句法树节点 *二元表达式(int 上牌优){
	struct 抽象句法树节点 *左,*右;
	int 牌类型;
	//取左边的整数字面.
	//拾取下一个牌在同一时间.
	左 = 基元();

	//若左边无牌,返回仅左节点
	牌类型 = 大牌.牌;
	if(牌类型 == 牌名_文件终)
		return (左);

/*	//转换此牌进到一个节点类型
	节点类型 = 算术操作(大牌.牌);

	//取下一个牌进入 */
	//尽管这个牌的优先级比上一个的优先级多/高
	while(操作_优先级(牌类型) > 上牌优){
		//拾取下一个整数字面
		扫描(&大牌);
		//递归地调用 二元表达式() 和我们的牌的优先级一起
		//去建造一个子树,
		右 = 二元表达式(操作优先级[牌类型]);
		//加入我们的子树,转换此牌进到抽象句法树操作在同时,
		左 = 制作抽象句法树节点(算术操作(牌类型),左,右,0);

		//当前牌的细节更新
		//若无牌左,就返回左节点,
		牌类型 = 大牌.牌;
		if(牌类型 == 牌名_文件终)
			return (左);
	}

	//当我们有个优先级相同或更低时返回此树
	return (左);
}


/*------------------------------------------------
------解释 -------
------------------------------------------------*/



//抽象句法树的树解释器
//抽象句法树操作的列表
static char *抽象句法树操作[] = {"+","-","*","/"};

//给定一个抽象句法树,
//解释里面的操作,返回一个最终值.
int 解释抽象句法树(struct 抽象句法树节点 *个数){
	int 左值,右值;

	//取左与右的子树值
	if(个数->左)
		左值 = 解释抽象句法树(个数->左);
	if(个数->右)
		右值 = 解释抽象句法树(个数->右);
/*
	//调试: 打印我们将要做的事
	if(个数->操作 == 抽象_整型字面)
		printf("整型 %d\n",个数->整型值);
	else
		printf("%d %s %d\n",左值,抽象句法树操作[个数->操作],右值);
*/
	switch (个数->操作){
		case 抽象_加法:
			return (左值 + 右值);
		case 抽象_减法:
			return (左值 - 右值);
		case 抽象_乘法:
			return (左值 * 右值);
		case 抽象_除法:
			return (左值 / 右值);
		case 抽象_整型字面:
			return (个数->整型值);
		default:
			fprintf(stderr,"未知抽象句法树操作 %d\n",个数->操作);
			exit(1);
	}
}