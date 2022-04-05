#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

//结构和枚举定义

//牌 
enum{
	牌名_加,牌名_减,牌名_星,
	牌名_斜杠,牌名_整型字面,
};

//牌结构
struct 牌{
	int 牌;
	int 整型值;
};