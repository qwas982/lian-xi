#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "第四.h"
#include "关键字.h"

整型 检查空白字符(字符 空白符)
{
    返回 (空白符==' ' || 空白符=='\t' || 空白符=='\n');
}


字符 *解析第四字()
{
    字符 *现在的;
    一会儿 (检查空白字符(*代码文本指针)) //跳过字符串头部的空白字符后返回指针
        代码文本指针++;
    
    现在的 = 代码文本指针;
    
    一会儿 ( !检查空白字符(*代码文本指针)  && (*代码文本指针)!='\0') //跳过字符串中第一个词
        代码文本指针++;
    
    若(*代码文本指针 == '\0') 
        返回 现在的;
    *代码文本指针 = '\0';   //将字符串中第一个词后的空格变为'\0'
    代码文本指针++;
    
    返回 现在的;
}


字_ *创建(字符 *名字域, 代码域指针  f指针)
{
    字_ *小字=(字_*)malloc(求大小(字_));
    小字->代码指针=f指针;
    
    小字->名字域=(字符*)malloc(strlen(名字域) + 1);
    strcpy(小字->名字域,名字域);
    
    小字->字形参域=NULL;   

    小字->标记旗 = 隐藏_字;

    返回 小字;
}


空的 去做(字_ *小字, 字_ **列表, 整型 n)
{
    若(n != 0) {
        小字->字形参域 = (字_**)malloc(n);
        memcpy(小字->字形参域,列表, n);
    } 否则 {
        小字->字形参域 = 列表;
    }
    小字->标记旗 = 展现_字;
}


字_ *定义核心字(字符 *名字域, 代码域指针  f指针)
{
    字_ *小字=创建(名字域, f指针);
    小字->标记旗 = 展现_字;

    返回 小字;
}


空的 冒号字代码域()
{
    返回指针++;
    *返回指针=(细胞)指令指针的指针;
    指令指针的指针=(*指令指针的指针)->字形参域-1;
    打印("[调试]进入子例程\n")
}


空的 常量字代码域()
{
    数据栈推((细胞)((*指令指针的指针)->字形参域));
}


空的 变量字代码域()
{
    数据栈推((细胞)*指令指针的指针);
}


字典_ *字典初始化()
{
    字典_ *字典的=(字典_*)malloc(求大小(字典_));
    字典的->字典字数 = 0;
    字典的->字典最后字地址 = NULL;
    返回 字典的;
}


整型 字典指令下一个(字典_ *字典的, 字_ *字的)
{
    字的->链接域 = 字典的->字典最后字地址;
    字典的->字典最后字地址 = 字的;
    字典的->字典字数++;
    返回 0;
}


字_ *字典搜索名字(字典_ *字典的, 字符 *名字域)
{
    字_ *小字 = 字典的->字典最后字地址;
    //为了支持递归忽略隐藏词
    一会儿 ((小字 != NULL && strcmp(小字->名字域,名字域))
        || (小字 != NULL && !strcmp(小字->名字域,名字域) && 小字->标记旗 == 隐藏_字))
    {  
        小字=小字->链接域;
    }
    
    返回 小字;
}


空的 字典摧毁字(字_ *字的)
{
    free(字的->名字域);
    若(字的->代码指针 == 冒号字代码域) free(字的->字形参域);
    free(字的);
}


整型 字典记忆之后(字典_ *字典的, 字符 *名字域)
{
    字_ *小字 = 字典搜索名字(字典的, 名字域);
    字_ *删小字;
    
    若(小字 == NULL)
    {
        printf("%s :\n\t找不到!\n", 名字域);
    }
    否则
    {
        若(小字->字形参域 == NULL)
        {
            printf("\t核心字不能被删除\n");
            返回 0;
        }
        做
        {
            删小字 = 字典的->字典最后字地址;
            字典的->字典最后字地址 = 字典的->字典最后字地址->链接域;
            字典摧毁字(删小字);
            字典的->字典字数--;
        } 一会儿(删小字 != 小字);
        
        返回 1;
    }
    
    返回 0;
}


空的 指令指针的解释()
{
    字_  **指令指针终 = 指令指针的指针;
    指令指针的指针=指令指针的选择指针;
    
    一会儿(指令指针的指针 != 指令指针终)
    {
        打印("[调试]解释执行> %s\n", (*指令指针的指针)->名字域)
        
        (*指令指针的指针)->代码指针();
        ++指令指针的指针;
    }
    指令指针的选择指针 = 指令指针的指针;
}


整型 是否数字(字符 *短的串)
{
    若(*短的串 == '-')
        短的串++;

    一会儿 (*短的串 != 0)
    {
        若 (!isdigit((细胞)*短的串)) 
            返回 0;
        短的串++;
    }
    返回 1;
}


整型 找字后执行(字典_ *字典的, 字符 *名字域)
{
    字_ *字的指针;
    字的指针 = 字典搜索名字(字典的, 名字域);
    
    若(!strcmp(":", 名字域) || !strcmp("]", 名字域)) 状态量 = 编译;
    
    若(状态量 == 编译)
    {
        若(字的指针==NULL)    //词典链表搜索不到名字后，去判断是不是数字
        {
            若 (!是否数字(名字域))    
            {
                返回 0;    //如果不是数字，返回0
            }
            否则 
            {               //如果是数字
                打印("[调试]成功找到数字%s\n",名字域)
                指令指针推(字典搜索名字(字典的, "(字面)"), 指令指针的选择指针);   //将push核心词指针存入IP数组        
                指令指针推((字_*)(细胞)(atoi(名字域)), 指令指针的选择指针);    //将CELL型数强制转换为Word指针类型

                返回 1;
            }            
        }
        否则 若(字的指针->标记旗 == 立即_字)  //立即词
        {
            打印("[调试]执行立即字 %s\n", 名字域)
            若(字的指针->字形参域 != NULL)
            {
                入解释器();
                指令指针推(字的指针, 指令指针的选择指针);
                指令指针的解释();
                出解释器();
            }
            否则
            {
                字的指针->代码指针();
            }
        }
        否则
        {
            打印("[调试]成功编译%s字\n",名字域)
            指令指针推(字的指针, 指令指针的选择指针);
        }
    }
    否则 若(状态量 == 解释)
    {
        若(字的指针==NULL)    //词典链表搜索不到名字后，去判断是不是数字
        {
            若 (!是否数字(名字域))    
            {
                返回 0;    //如果不是数字，返回0
            }
            否则 
            {               //如果是数字
                打印("[调试]数据栈压入 %s\n",名字域)
                数据栈推((细胞)(atoi(名字域)));

                返回 1;
            }            
        }
        否则
        {
            打印("[调试]成功找到%s字\n",名字域)
            指令指针推(字的指针, 指令指针的选择指针);
            指令指针的解释();
        }
    }

    返回 1;
}


空的 空了的栈()
{
    数据指针=数据栈-1;
    返回指针=返回栈-1;
}


空的 栈错误(整型 个数)
{
    切换(个数)
    {
        情况 1: printf("栈下溢\n"); 断;
        情况 2: printf("栈溢出\n"); 断;
    }
    exit(0);
}


空的 指令指针推(字_ *小字, 字_** 列表)
{
    若(指令指针的指针 >= 列表+缓冲_长度){栈错误(2);}
    *指令指针的指针=小字;
    指令指针的指针++;
}


空的 数据栈推(细胞 个数)
{
    若(数据指针 >= 数据栈+栈_长度-1){栈错误(2);}
    数据指针++;
    *数据指针=个数;
}


空的 返回栈推(细胞 个数)
{
    若(返回指针 >= 返回栈+栈_长度-1){栈错误(2);}
    返回指针++;
    *返回指针=个数;
}


细胞 数据栈弹()
{
    若(数据指针 <= 数据栈-1){栈错误(1);}
    数据指针--;
    返回 *(数据指针+1); 
}


细胞 返回栈弹()
{
    若(返回指针 <= 返回栈-1){栈错误(1);}
    返回指针--;
    返回 *(返回指针+1); 
}


细胞 数据栈顶()
{
    若(数据指针 <= 数据栈-1){栈错误(1);}
    返回 *数据指针;
}


细胞 返回栈顶()
{
    若(返回指针 <= 返回栈-1){栈错误(1);}
    返回 *返回指针;
}


空的 字面()
{
    指令指针的指针++;
    数据栈推((细胞)*指令指针的指针);
    打印("[调试]数%ld压入数据栈\n", (细胞)*指令指针的指针)
}


空的 弹数据栈()
{
    printf("%ld\n", 数据栈弹());
}


空的 再见()
{
    exit(1);
}


空的 返来()
{
    指令指针的指针=(字_**)(返回栈弹());
    打印("[调试]从子程序返回\n")
}


空的 深度()
{
    数据栈推((细胞)(数据指针-数据栈+1));
}


空的 加号()
{
    数据栈推(数据栈弹() + 数据栈弹());
}


空的 减号()
{
    细胞 小数据 = 数据栈弹();
    数据栈推(数据栈弹() - 小数据);
}


空的 乘号()
{
    数据栈推(数据栈弹() * 数据栈弹());
}


空的 除号()
{
    细胞 小数据 = 数据栈弹();
    数据栈推(数据栈弹() / 小数据);
}


空的 丢掉()
{
    数据栈弹();
}


空的 秀数据栈()
{
    printf("<%ld> ", (细胞)(数据指针-数据栈+1));
    细胞 *计数=数据栈;
    为 (;计数<=数据指针 ;计数++ )
    {
        printf("%ld ",*计数);
    }
    printf("\n");
}


空的 选取()
{
    细胞 计数 = 数据栈弹();
    若(数据指针-计数+1 <= 数据栈-1){栈错误(1);}
    数据栈推(*(数据指针-计数+1));
}


空的 滚动()
{
    细胞 计数 = 数据栈弹();
    若(数据指针-计数+1 <= 数据栈-1){栈错误(1);}
    细胞 数据计数 = *(数据指针-计数+1);
    为(; 计数>1; 计数--) {
        *(数据指针-计数+1) = *(数据指针-计数+2);
    }
    数据栈弹();
    数据栈推(数据计数);
}


空的 入变量()
{
    字_ *针 = (字_ *)(数据栈弹());
    针->字形参域 = (字_ **)数据栈弹();
}


空的 出变量() 
{
    字_ *针 = (字_ *)(数据栈弹());
    数据栈推((细胞)(针->字形参域));
}


空的 等号()
{
    若(数据栈弹() == 数据栈弹())
    {
        数据栈推(-1);
    }
    否则
    {
        数据栈推(0);
    }
}


空的 不等号()
{
    若(数据栈弹() != 数据栈弹())
    {
        数据栈推(-1);
    }
    否则
    {
        数据栈推(0);
    }
}


空的 大于号()
{
    细胞 小数据 = 数据栈弹();
    若(数据栈弹() > 小数据)
    {
        数据栈推(-1);
    }
    否则
    {
        数据栈推(0);
    }
}


空的 小于号()
{
    细胞 小数据 = 数据栈弹();
    若(数据栈弹() < 小数据)
    {
        数据栈推(-1);
    }
    否则
    {
        数据栈推(0);
    }
}


空的 若分支()
{
    若(数据栈弹() == 0)
    {
        指令指针的指针 = 指令指针的指针 + (细胞)(*(指令指针的指针+1));
    }
    否则
    {
        指令指针的指针++;
    }
}


空的 分支()
{
    指令指针的指针 = 指令指针的指针 + (细胞)(*(指令指针的指针+1));
}


空的 括号做()
{
    细胞 索引 = 数据栈弹();
    细胞 极限 = 数据栈弹();
    若(极限 <= 索引)
    {
        指令指针的指针 = 指令指针的指针 + (细胞)(*(指令指针的指针+1)); 
    }
    否则
    {
        指令指针的指针++;
        索引++;
        返回栈推(索引);
        返回栈推(极限);
    }
}


空的 括号环() 
{
    指令指针的指针 = 指令指针的指针 - (细胞)(*(指令指针的指针+1)); 
    数据栈推(返回栈弹());
    数据栈推(返回栈弹());
}


空的 到返回()
{
    返回栈推(数据栈弹());
}


空的 返回到()
{
    数据栈推(返回栈弹());
}


空的 返回在()
{
    数据栈推(返回栈顶());
}


空的 发射()
{
    putchar((字符)(数据栈弹()));
}


空的 多个字()
{
    字_ *小字 = 第四字典指针->字典最后字地址;
    一会儿 (小字 != NULL)
    {  
        printf("%s ", 小字->名字域);
        小字=小字->链接域;
    }
    printf("\n");
}


空的 立即的()
{
    第四字典指针->字典最后字地址->标记旗 = 立即_字;
}


空的 推数据栈_冒号字拾取地址()
{
    当前字首指针 = 解析第四字();
    数据栈推((细胞)字典搜索名字(第四字典指针, 当前字首指针));
}


空的 编译到冒号字形参域()
{
    指令指针的指针++;
    字_ **临时的 = 指令指针的指针;
    字_ *字的指针 = *指令指针的指针;
    字_ **指令指针完了 = (字_ **)返回栈弹();
    指令指针的指针 = (字_ **)返回栈弹();
    若(字的指针->字形参域 != NULL)
    {
        字_ **针 = 字的指针->字形参域;
        字_ *终 = 字典搜索名字(第四字典指针, "返");
        为 (; *针 != 终; 针++)
        {
            指令指针推(*针, 第四字典指针->冒号字形参域);
        }
    }
    否则
    {
        打印("[调试]编译核心字 %s\n", 字的指针->名字域)
        指令指针推((字_ *)字的指针, 第四字典指针->冒号字形参域);
    }
    返回栈推((细胞)指令指针的指针);
    返回栈推((细胞)指令指针完了);
    指令指针的指针 = 临时的;
}


空的 编译栈顶到冒号字()
{
    字_ **临时的 = 指令指针的指针;
    字_ **指令指针完了 = (字_ **)返回栈弹();
    指令指针的指针 = (字_ **)返回栈弹();
    细胞 数字 = 数据栈弹();
    打印("[调试]编译栈顶数 %ld\n", 数字)
    指令指针推((字_ *)数字, 第四字典指针->冒号字形参域);
    返回栈推((细胞)指令指针的指针);
    返回栈推((细胞)指令指针完了);
    指令指针的指针 = 临时的;
}


空的 入解释器()
{
    状态量 = 解释;
    指令指针的选择指针 = 指令指针列表;
    返回栈推((细胞)指令指针的指针);
    指令指针的指针=指令指针的选择指针;
}


空的 出解释器()
{
    指令指针的选择指针 = 第四字典指针->冒号字形参域;
    指令指针的指针 = (字_ **)返回栈弹();
    状态量 = 编译;
}


空的 我自己()
{
    指令指针推(第四字典指针->字典最后字地址, 指令指针的选择指针);
}


空的 定义冒号字()
{
    指令指针的选择指针 = 第四字典指针->冒号字形参域;
    指令指针的指针=指令指针的选择指针;
    当前字首指针 = 解析第四字();
    字典指令下一个(第四字典指针, 创建(当前字首指针, 冒号字代码域));
}


空的 结束冒号字()
{
    指令指针推(字典搜索名字(第四字典指针, "返"), 指令指针的选择指针);
    整型 个数 = (细胞)指令指针的指针 - (细胞)指令指针的选择指针;
    去做(第四字典指针->字典最后字地址, 指令指针的选择指针, 个数);
    
    //DEBUG模式下打印出IP指针列表
    若(调试) {
        printf("[调试]指令指针的指针列表> ");
        字_ **针=指令指针的选择指针;
        为 (;针<指令指针的指针 ;针++ )
        {
            printf("%ld ",(细胞)(*针));
        }
        printf("\n");
        秀数据栈();
    }
    
    指令指针的选择指针 = 指令指针列表;
    指令指针的指针=指令指针的选择指针;
    状态量 = 解释;
}


空的 _若字面()
{
    指令指针推(字典搜索名字(第四字典指针, "?分支"), 指令指针的选择指针);
    返回栈推((细胞)指令指针的指针);
    指令指针推((字_ *)0, 指令指针的选择指针);
}


空的 _否则字面()
{
    指令指针推(字典搜索名字(第四字典指针, "分支"), 指令指针的选择指针);
    字_** 否则的针 = 指令指针的指针;
    字_** 若的针 = (字_**)(返回栈弹());
    返回栈推((细胞)否则的针);
    *若的针 = (字_*)(指令指针的指针 - 若的针 + 1);
    指令指针推((字_ *)0, 指令指针的选择指针);
}


空的 _那么字面()
{
    字_** 分支的针 = (字_**)(返回栈弹());
    *分支的针 = (字_*)(指令指针的指针 - 分支的针); 
}


空的 _做字面()
{
    指令指针推(字典搜索名字(第四字典指针, "(做)"), 指令指针的选择指针);
    返回栈推((细胞)指令指针的指针);
    指令指针推((字_ *)0, 指令指针的选择指针);
    
}


空的 _环字面()
{
    指令指针推(字典搜索名字(第四字典指针, "(环)"), 指令指针的选择指针); 
    字_** 做的针 = (字_**)(返回栈弹());
    *做的针 = (字_*)(指令指针的指针 - 做的针 + 1); 
    指令指针推((字_*)(指令指针的指针 - 做的针 + 1), 指令指针的选择指针); 
}


空的 看见()
{
    当前字首指针 = 解析第四字();
    字_ *字的针 = 字典搜索名字(第四字典指针, 当前字首指针);
    
    若(字的针 == NULL)
    {
        printf("%s :\n\t找不到!\n", 当前字首指针);
    }
    否则
    {   //反编译wplist，得出扩展词的字符串定义
        printf("%s :\n\t", 当前字首指针);
        若(字的针->代码指针 == 冒号字代码域)
        {
            字_ **针 = 字的针->字形参域;
            字_ *终 = 字典搜索名字(第四字典指针, "返");
            字_ *字典的针 = 第四字典指针->字典最后字地址;
            为(; *针 != 终; 针++)
            {
                一会儿 (字典的针 != NULL && 字典的针 != *针)
                {  
                    字典的针=字典的针->链接域;
                }

                若(字典的针 != NULL)
                    printf("%s ", (*针)->名字域);
                否则
                    printf("%ld ", (细胞)(*针));
                字典的针 = 第四字典指针->字典最后字地址;
            }
            printf(";");
            若(字的针->标记旗 == 立即_字)
                printf(" 立即的\n");
            否则
                printf("\n");
        }
        否则
        {
            printf("%s\n", 字的针->名字域);
        }
    }
}


空的 忘记()
{
    当前字首指针 = 解析第四字();
    字典记忆之后(第四字典指针, 当前字首指针); //删除当前扩展词以及词典中该词之后定义的所有扩展词
}


空的 变量的()
{
    当前字首指针 = 解析第四字();
    字典指令下一个(第四字典指针, 创建(当前字首指针, 变量字代码域));
    去做(第四字典指针->字典最后字地址, (字_ **)0, 0);
}


空的 常量的()
{
    当前字首指针 = 解析第四字();
    字典指令下一个(第四字典指针, 创建(当前字首指针, 常量字代码域));
    去做(第四字典指针->字典最后字地址, (字_ **)数据栈弹(), 0);
    
}


空的 载入()
{
    当前字首指针 = 解析第四字();
    载入_文件(当前字首指针);
}


空的 解释器()
{
    状态量 = 解释;
    代码文本指针 = 第四代码文本;
    指令指针的选择指针 = 指令指针列表;
    指令指针的指针=指令指针的选择指针;
       
    一会儿 (*(当前字首指针 = 解析第四字()) != '\0')
    {
        若(!strcmp(".\"",当前字首指针))  //如果是." str " 则立即编译其中的字符串str
        {
            打印("[调试]编译词句\n")

            字符 临时的串[缓冲_长度]; 
            一会儿(*代码文本指针 != '\"')
            {
                sprintf(临时的串, "%ld", (细胞)(*代码文本指针));
                找字后执行(第四字典指针, 临时的串);
                找字后执行(第四字典指针, "发射");
                代码文本指针++;
            }
            代码文本指针++;
        }
        否则 若(!strcmp("(",当前字首指针))  //注释模式
        {

            一会儿(*代码文本指针 != ')')
            {
                代码文本指针++;
            }
            代码文本指针++;
        }
        否则 若(!找字后执行(第四字典指针, 当前字首指针))
        {
            printf("[%s]?\n",当前字首指针);
            空了的栈();
            指令指针的指针=指令指针的选择指针;
            返回;
        }
    }
}


//从外部文件读取Forth代码
整型 载入_文件(字符 *文件_路径)
{
    FILE *文件指针; //文件指针
    字符 单个字符;
    整型 计数 = 0;
    整型 冒号旗 = 假的;

    若((文件指针 = fopen(文件_路径, "r")) == NULL)
    {
        printf("打不开 %s\n", 文件_路径);
        返回 0;
    }
    
    做
    {
        单个字符 = getc(文件指针);
        若((单个字符 != '\n' && 单个字符 != EOF) 
            || (单个字符 == '\n' && 冒号旗 == 真的))
        {
            若(单个字符 == ':') 冒号旗 = 真的;
            否则 若(单个字符 == ';') 冒号旗 = 假的;
            第四代码文本[计数] = 单个字符;
            计数++;
        }
        否则 若((单个字符 == '\n' && 冒号旗 == 假的)
            || 单个字符 == EOF)
        {
            第四代码文本[计数] = '\0';
            解释器();
            计数 = 0;
        }           
    } 一会儿(单个字符 != EOF);
    fclose(文件指针);

    返回 1;
}


//主程序入口
整型 main(整型 实参计数, 字符 *实参值[]) 
{
    空了的栈();
    指令指针的选择指针 = 指令指针列表;
    指令指针的指针 = 指令指针的选择指针;
    第四字典指针= 字典初始化();
    
    //初始化词典
    字典指令下一个(第四字典指针, 定义核心字("(字面)",字面));
    字典指令下一个(第四字典指针, 定义核心字(".",弹数据栈));
    字典指令下一个(第四字典指针, 定义核心字("再见",再见));
    字典指令下一个(第四字典指针, 定义核心字("返",返来));
    字典指令下一个(第四字典指针, 定义核心字("深度",深度));
    字典指令下一个(第四字典指针, 定义核心字("+",加号));
    字典指令下一个(第四字典指针, 定义核心字("-",减号));
    字典指令下一个(第四字典指针, 定义核心字("*",乘号));
    字典指令下一个(第四字典指针, 定义核心字("/",除号));
    字典指令下一个(第四字典指针, 定义核心字("丢掉",丢掉));
    字典指令下一个(第四字典指针, 定义核心字(".栈",秀数据栈));
    字典指令下一个(第四字典指针, 定义核心字("选取",选取));
    字典指令下一个(第四字典指针, 定义核心字("滚动",滚动));
    字典指令下一个(第四字典指针, 定义核心字("!", 入变量));
    字典指令下一个(第四字典指针, 定义核心字("@", 出变量));
    字典指令下一个(第四字典指针, 定义核心字("=",等号));
    字典指令下一个(第四字典指针, 定义核心字("<>",不等号));
    字典指令下一个(第四字典指针, 定义核心字(">",大于号));
    字典指令下一个(第四字典指针, 定义核心字("<",小于号));
    字典指令下一个(第四字典指针, 定义核心字("?分支",若分支));
    字典指令下一个(第四字典指针, 定义核心字("分支",分支));
    字典指令下一个(第四字典指针, 定义核心字("(做)",括号做));
    字典指令下一个(第四字典指针, 定义核心字("(环)",括号环));
    字典指令下一个(第四字典指针, 定义核心字(">返",到返回));
    字典指令下一个(第四字典指针, 定义核心字("返>",返回到));
    字典指令下一个(第四字典指针, 定义核心字("返@",返回在));
    字典指令下一个(第四字典指针, 定义核心字("发射", 发射));
    字典指令下一个(第四字典指针, 定义核心字("字",多个字));
    字典指令下一个(第四字典指针, 定义核心字("立即",立即的));
    字典指令下一个(第四字典指针, 定义核心字("编译", 编译到冒号字形参域)); 
    字典指令下一个(第四字典指针, 定义核心字(",", 编译栈顶到冒号字));
    字典指令下一个(第四字典指针, 定义核心字("'", 推数据栈_冒号字拾取地址));
    
    字典指令下一个(第四字典指针, 定义核心字("[",入解释器)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("]",出解释器)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("我自己", 我自己)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字(":",定义冒号字)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字(";",结束冒号字)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("若",_若字面)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("否则",_否则字面)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("那么",_那么字面)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("做",_做字面)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("环",_环字面)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("看",看见)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("忘记",忘记)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("变量",变量的)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("常量",常量的)); 立即的();
    字典指令下一个(第四字典指针, 定义核心字("载入",载入)); 立即的();
    
    
    为(; 实参计数 > 1; 实参计数--)
        载入_文件(*++实参值);

    一会儿 (1)
    {
        printf("读.求值.打印.环>>> ");
        gets(第四代码文本);
        解释器();
    }

    
    返回 0;
}