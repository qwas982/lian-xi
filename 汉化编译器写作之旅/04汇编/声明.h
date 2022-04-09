//函数原型为所有编译器文件
int 扫描(struct 牌 *牌名);
struct 抽象句法树节点 *制作抽象句法树节点(int 操作,struct 抽象句法树节点 *左,
					struct 抽象句法树节点 *右,int 整型值);
struct 抽象句法树节点 *制作抽象句法树叶(int 操作,int 整型值);
struct 抽象句法树节点 *制作抽象句法树一元(int 操作,struct 抽象句法树节点 *左,int 整型值);
struct 抽象句法树节点 *二元表达式(int rbp);
int 解释抽象句法树(struct 抽象句法树节点 *节点);

void 生成代码(struct 抽象句法树节点 *节点);
void 自由全部_寄存器(void);
void 代码生成器前文();
void 代码生成器后文();
int 代码生成器载入(int 值);
int 代码生成器加(int 寄存1,int 寄存2);
int 代码生成器减(int 寄存1,int 寄存2);
int 代码生成器乘(int 寄存1,int 寄存2);
int 代码生成器除(int 寄存1,int 寄存2);
void 代码生成器打印整型(int 寄存);