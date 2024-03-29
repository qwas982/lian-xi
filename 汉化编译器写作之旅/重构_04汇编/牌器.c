#include "定义.h"

/*------------------------------------------------
---------扫描-----------
------------------------------------------------*/

//词汇扫描
//返回印刻 印名 的位置
//在串 串名 内,或 -1 若 印名 未找到
static int 印刻位置(char *串名,int 印名){
	char *指针;
	指针 = strchr(串名,印名);
	return(指针? 指针-串名 : -1);
}

//取下一个印刻从输入文件
static int 下一个(void){
	int 印名;
	if(放回去){  //用印刻放
		印名 = 放回去; //若有就回
		放回去 = 0;
		return 印名;
	}

	印名 = fgetc(文件内); //读来自输入文件
	if('\n' == 印名)
		行++;            //增长行计数
	return 印名;
}

//放回不想要的印刻
static void 放回去_(int 印名){
	放回去 = 印名;
}

//跳过我们不需要的输入
//即-空格 新行,放回第一个我们需要处理的印刻
static int 跳过(void){
	int 印名;
	印名 = 下一个();
	while(' ' == 印名 || '\t' == 印名 || '\r'== 印名 ||'\f' == 印名){
		印名 = 下一个();
	}
	return(印名);
}

//扫描与返回一个整数字面值来自输入文件,
//在文本内把串作为值存储,
static int 扫描整型(int 印名){
	int 键,值 = 0;
	//转换每个印刻进到整型值
	while((键 = 印刻位置("0123456789",印名)) >= 0){
		值 = 值 * 10 + 键;
		印名 = 下一个();
	}
	//我们击中一个非整数印刻,放它回去,
	放回去_(印名);
	return 值;
}

//扫描与返回在输入中找到的下一个牌,
//返回1若牌有效,返回0若无牌左,
int 扫描(struct 牌 *牌名){
	int 印名;
	//跳过空格
	印名 = 跳过();
	//下决心牌基于输入印刻
	switch(印名){
	case EOF: //EOF = -1,老实说用-1也可以
		return (0);
	case '+':
		牌名->牌 = 牌名_加;
		break;
	case '-':
		牌名->牌 = 牌名_减;
		break;
	case '*':
		牌名->牌 = 牌名_星;
		break;
	case '/':
		牌名->牌 = 牌名_斜杠;
		break;
	default:
		//若它是数字,扫描字面整数值
		if(isdigit(印名)){
			牌名->整型值 = 扫描整型(印名);
			牌名->牌 = 牌名_整型字面;
			break;
		}
		printf("未识别印刻 %c 在第 %d\n 行",印名,行);
		exit(1);
	}
	//我们找到一个牌
	return (1);
}	