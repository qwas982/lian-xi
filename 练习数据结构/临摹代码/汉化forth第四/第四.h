#define 细胞 long
#define 栈长 1024
#define 缓冲长 1024
#define 真 1
#define 假 0
#define 解释 0
#define 编译 1
#define 揭露字 0
#define 立即字 1
#define 隐藏字 2

#define 调试 0
#if 调试
	#define 打印(fmt,args...) printf(fmt,##args);
#else
	#define 打印(fmt,args...)
#endif

typedef void (*函数指针)();

typedef struct 字{
	struct 字 *链接;
	细胞 旗;
	char *名字;
	函数指针 代码指针;
	struct 字 **字参数列表;
}字;

typedef struct 字典{
	细胞 大小;
	字 *头部;
	字 *字参数列表临时[缓冲长];
}字典;

细胞 状态;
char 第四文本[缓冲长];
char *当前文本;
char *文本指针;
字典 *第四字典;
细胞 数据栈[栈长];
细胞 返回栈[栈长];
细胞 *数据栈指针,*返回栈指针;
字 *指令指针列表[缓冲长];
字 **指令指针;
字 **指令指针头部;

int 检查空白(char 检);
char *解析字();

字 *创建(char *名字,函数指针 函指);
void 做过(字 *检,字 **列表,int 数量);
字 *定义核心字(char *名字,函数指针 函指);
void 冒号字();
void 常量字();
void 变量字();

字典 *字典初始化();
int 字典指令下一个(字典 *星字典,字 *星字);
字 *字典搜索名字(字典 *星字典,char *名字);
void 字典摧毁字(字 *星字);
int 字典余留之后(字典 *星字典,char *名字);

void 解释机();
int 是否号码(char *串);
int 找(字典 *星字典,char *名字);

void 空栈();
void 栈错误(int 数量);
void 指令指针推入(字 *星字,字 **列表);
void 数据栈推入(细胞 数量);
void 返回栈推入(细胞 数量);
细胞 数据栈弹出();
细胞 返回栈弹出();
细胞 数据栈顶端();
细胞 返回栈顶端();

void 字面值();
void 弹数据栈();
void 再见();

void 返();
void 深();
void 加();
void 减();
void 乘();
void 除();

void 滴落();
void 展现数据栈();
void 摘取();
void 卷动();

void 入变量();
void 出变量();

void 等于();
void 不等于();
void 大于();
void 小于();

void 若分支();
void 分支();

void 做事();
void 环();

void 去返回栈();
void 返回栈去();
void 返回栈顶端出();

void 发射();
void 字复数();

void 立即数();
void 编译机();
void 编译栈顶数();
void 推入数据栈字执行地址();

void 入解释器();
void 出解释器();
void 我自己的();
void 定义冒号字();
void 终冒号字();
void 若();
void 否则();
void 那么();
void 流程做();
void 流程环();
void 瞧();
void 遗忘();
void 变量();
void 常量();
void 载入();

void 解释器();
int 载入文件(char *文件路径);