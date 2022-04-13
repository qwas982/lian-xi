//函数原型为所有编译器文件

//扫描.c
int 扫描(struct 牌 *牌名);

//树.c
struct 抽象句法树节点 *制作抽象句法树节点(int 操作,struct 抽象句法树节点 *左,
					struct 抽象句法树节点 *右,int 整型值);
struct 抽象句法树节点 *制作抽象句法树叶(int 操作,int 整型值);
struct 抽象句法树节点 *制作抽象句法树一元(int 操作,struct 抽象句法树节点 *左,int 整型值);

//生成.c
int 生成抽象句法树(struct 抽象句法树节点 *节点,int 寄存);
void 生成前文();
void 生成后文();
void 生成自由寄存();
void 生成打印整型(int 寄存);
void 生成全局符号(char *符);

//代码生成器.c
void 自由全部_寄存器(void);
void 代码生成器前文();
void 代码生成器后文();
int 代码生成器载入整型(int 值);
int 代码生成器载入全局(char *标识符);
int 代码生成器加(int 寄存1,int 寄存2);
int 代码生成器减(int 寄存1,int 寄存2);
int 代码生成器乘(int 寄存1,int 寄存2);
int 代码生成器除(int 寄存1,int 寄存2);
void 代码生成器打印整型(int 寄存);
int 代码生成器存全局(int 寄存,char *标识符);
void 代码生成器全局符号(char *符号);


//表达式.c
struct 抽象句法树节点 *二元表达式(int 上牌优);

//语句.c
void 语句(void);

//冗杂.c
void 匹配(int 字牌,char *什么);
void 分号(void);
void 标识(void);
void 致命(char *符);
void 致命符(char *符1,char *符2);
void 致命数(char *符,int 数);
void 致命印名(char *符,int 印名);

//符号.c
int 找全局(char *符);
int 添加全局(char *名字);

//声明.c
void 变量_声明(void);