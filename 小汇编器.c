#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hong.h"

枚举 操作类型{
    操作停机,操作入,操作出,操作加,操作减,操作乘,操作除,
    操作转储,
    操作载入,操作存储,操作载入码,
    操作跳小于,操作跳小等,
    操作跳大于,操作跳大等,
    操作跳等于,操作跳不等,
    操作跳,
    操作无效
};

类型定义 结构 指令{
    整型 操作;
    整型 实参;

}指令;

#define 代码大小 (1024)
#define 数据大小 (1024)

/*映射到操作类型*/
常量 印刻 *操作描述[] = {
    "停机","入","出","加","减","乘","除",
    "转储",
    "载入","存储","载入码",
    "跳小于","跳小等","跳大于","跳大等",
    "跳等于","跳不等",
    "跳",
    0
};

FILE *文件指针入;
FILE *文件指针出;

指令 指令内存[代码大小];

/*取操作从它的串描述*/
整型 取操作(常量 印刻 *串){
    整型 甲 = 0;
    环(;操作描述[甲] != 0; ++甲){
        若(strcmp(操作描述[甲],串) == 0){
            返回 甲;
        }
    }
    返回 操作无效;
}

/*取操作码实参(操作数)计数*/
整型 取操作数计数(整型 操作){
    整型 返;
    切换(操作){
        情况 操作载入码:
        情况 操作跳小于:
        情况 操作跳小等:
        情况 操作跳大于:
        情况 操作跳大等:
        情况 操作跳等于:
        情况 操作跳不等:
        情况 操作跳:
            返 = 1;
            断;
        默认:
            返 = 0;
    }
    返回 返;
}

空 读汇编(){
    印刻 行[256];
    印刻 操作串[32];
    无符 短 操作;
    整型 实参计数;
    整型 实参;
    无符 短 定位;
    一会儿(!feof(文件指针入)){
        fgets(行, 求大小(行) - 1, 文件指针入);
        sscanf(行, "%d%s", (整型*)&定位, 操作串 );
        操作 = (无符 短) 取操作(操作串);
        实参计数 = 取操作数计数(操作);
        若(实参计数 > 0){
            印刻 *串 = strstr(行, 操作串);
            串 = &串[strcspn(串, " \t" )+1];
            实参 = atoi(串);
        }否则{
            实参 = 0;
        }
        指令内存[定位].操作 = 操作;
        指令内存[定位].实参 = 实参;

    }
}

空 出投掷(){
    整型 定位 = 0;
    整型 实参计数;
    环(; 指令内存[定位].操作 != 操作停机; ++定位){
        印刻 操作 = (印刻) 指令内存[定位].操作;
        fwrite(&操作,求大小(印刻), 1, 文件指针出); /*操作*/
        实参计数 = 取操作数计数(操作);
        若(实参计数 > 0){
            整型 实参 = 指令内存[定位].实参;
            fwrite(&实参, 求大小(实参), 1, 文件指针出); /*实参*/
        }
    }
}

整型 main(整型 实参计数, 印刻 **实参值){
    若(实参计数 < 2){
        fprintf(stderr, "用法:%s <文件名>\n", 实参值[0]); //c的函数库不支持utf8,无法输出中文.
        exit(-1);
    }
    文件指针入 = fopen(实参值[1], "r");
    若(文件指针入 == 0){
        fprintf(stderr ,"打开 %s 失败\n", 实参值[1]);
        exit(-1);
    }否则{
        印刻 出投掷[256] = {0};
        整型 行 = strcspn(实参值[1],".");
        strncpy(出投掷, 实参值[1], 行);
        strcat(出投掷, ".虚拟机"); //c的函数库不支持utf8,无法输出中文.
        文件指针出 = fopen(出投掷,"wb");
        若(文件指针出 == 0){
            fprintf(stderr, "打开 %s 失败\n", 出投掷);
            exit(-1);
        }
    }
    读汇编();
    出投掷();
    fclose(文件指针入);
    fclose(文件指针出);
    返回 0;

}