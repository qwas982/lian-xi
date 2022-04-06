//函数原型为所有编译器文件
int 扫描(struct 牌 *牌名);
struct 抽象句法树节点 *制作抽象句法树节点(int 操作,struct 抽象句法树节点 *左,
					struct 抽象句法树节点 *右,int 整型值);
struct 抽象句法树节点 *制作抽象句法树叶(int 操作,int 整型值);
struct 抽象句法树节点 *制作抽象句法树一元(int 操作,struct 抽象句法树节点 *左,int 整型值);
struct 抽象句法树节点 *二元表达式(void);
int 解释抽象句法树(struct 抽象句法树节点 *个数);