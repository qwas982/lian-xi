#include "Validation.h"

int 检查实参是不是(const char* 名字_小写, int 期望_小写, int 得到_小写)
{
    制作树语言_检查_大写(得到_小写 == 期望_小写,
           "\"%s\" expects %d arg%s, %d supplied",
           名字_小写, 期望_小写, 复数_大写(期望_小写), 得到_小写);
    return 得到_小写;
}

int 检查实参之间(const char* 名字_小写, int 最小_小写短, int 最大_小写短, int 得到_小写)
{
    制作树语言_检查_大写((得到_小写 >= 最小_小写短) && (得到_小写 <= 最大_小写短),
           "\"%s\" expects between %d and %d arg%s, %d supplied",
           名字_小写, 最小_小写短, 最大_小写短, 复数_大写(最大_小写短), 得到_小写);
    return 得到_小写;
}

int 检查实参至少(const char* 名字_小写, int 最小_小写短, int 得到_小写)
{
    制作树语言_检查_大写(得到_小写 >= 最小_小写短,
           "\"%s\" expects at least %d arg%s, %d supplied",
           名字_小写, 最小_小写短, 复数_大写(最小_小写短), 得到_小写);
    return 得到_小写;
}

int 检查实参即使(const char* 名字_小写, int 得到_小写)
{
    制作树语言_检查_大写(得到_小写 % 2 == 0,
           "\"%s\" expects an even number of args, %d supplied",
           名字_小写, 得到_小写);
    return 得到_小写;
}
