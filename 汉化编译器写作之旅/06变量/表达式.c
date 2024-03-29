#include "定义.h"
#include "数据.h"
#include "声明.h"

//解析表达式
//解析一个基元因子与返回一个抽象句法树节点代表它
static struct 抽象句法树节点 *基元(void){
	struct 抽象句法树节点 *节点;
	int 身份;

	
	//还要扫描下一个牌.另外,一个句法错误为任意其它牌类型.
	switch (大牌.牌){
		case 牌名_整型字面:
			//为一个 整型字面 牌,制作一个叶抽象句法树节点为它
			节点 = 制作抽象句法树叶(抽象_整型字面,大牌.整型值);
			break;
		case 牌名_标识:
			//检查此标识符是否存在
			身份 = 找全局(文本);
			if(身份 == -1)
				致命符("未知变量",文本);

			//制作一个叶子抽象句法树节点为它
			节点 = 制作抽象句法树叶(抽象_标识,身份);
			break;
		default:
			致命数("句法错误,牌",大牌.牌);
	}

	//扫描下一个牌与返回其叶子节点
	扫描(&大牌);
	return (节点);
}

//转换一个二元操作牌进到一个抽象句法树操作
static int 算术操作(int 牌类型){
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
			致命数("句法错误,牌",牌类型);
	}
}
//操作优先级为每个牌
static int 操作优先级[] = {0,10,10,20,20,0};

//检查我们是否有二元操作与返回其优先级
static int 操作_优先级(int 牌类型){
	int 优先级 = 操作优先级[牌类型];
	if(优先级 == 0){
		致命数("句法错误,牌",牌类型);
	}
	return (优先级);
}

//返回一个抽象句法树的树哪个的根是个二元操作
//形参 上牌优 是上一个牌的优先级,
struct 抽象句法树节点 *二元表达式(int 上牌优){
	struct 抽象句法树节点 *左,*右;
	int 牌类型;
	
	//取左边的基元树.
	//拾取下一个牌在同一时间.
	左 = 基元();

	//若我们击中一个分号,返回仅左节点
	牌类型 = 大牌.牌;
	if(牌类型 == 牌名_分号)
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
		//若我们击中一个分号,就返回左节点,
		牌类型 = 大牌.牌;
		if(牌类型 == 牌名_分号)
			return (左);
	}

	//当我们有个优先级相同或更低时返回此树
	return (左);
}