#ifndef 外部_
	#define 外部_ extern
#endif

//全局变量

外部_ int 行;  //当前行号
外部_ int 放回去;  //扫描器放回去的印刻
外部_ FILE *入文件;  //输入与输出文件
外部_ FILE *出文件;
外部_ struct 牌 大牌; // 最后牌已扫描
外部_ char 文本[文本长 + 1]; //最后标识符已扫描