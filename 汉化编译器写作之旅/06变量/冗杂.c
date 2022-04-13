#include "定义.h"
#include "数据.h"
#include "声明.h"

//冗杂函数

//确保当前牌是字牌,
//与拾取其下一个牌,另外抛出一个错误,
void 匹配(int 字牌,char *什么){
	if(大牌.牌 == 字牌){
		扫描(&大牌);
	}else{
		致命符("期望",什么);
	}
}

//匹配一个分号与拾取其下一个牌,
void 分号(void){
	匹配(牌名_分号,";");
}

//匹配标识符与拾取其下一个牌,
void 标识(void){
	匹配(牌名_标识,"标识符");
}

//打印出致命消息
void 致命(char *符){
	fprintf(stderr, "%s 在行 %d\n",符,行);
	exit(1);
}

void 致命符(char *符1,char *符2){
	fprintf(stderr, "%s:%s 在行 %d\n",符1,符2,行);
	exit(1);
}

void 致命数(char *符,int 数){
	fprintf(stderr, "%s:%d 在行 %d\n",符,数,行);
	exit(1);
}

void 致命印名(char *符,int 印名){
	fprintf(stderr, "%s:%c 在行 %d\n",符,印名,行);
	exit(1);
}