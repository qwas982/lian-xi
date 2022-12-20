#ifndef INCLUDE_VALIDATION_H
#define INCLUDE_VALIDATION_H

#include "String.h"

#define 制作树语言_检查_大写(条件_小写, ...)  \
    if (!(条件_小写)) { throw 字符串打印_大写缩(__VA_ARGS__); } else { }

#define 制作树语言_失败_大写(...) 制作树语言_检查_大写(false, __VA_ARGS__)

extern int 检查实参是不是(const char* 名字_小写, int 期望_小写, int 得到_小写);
extern int 检查实参之间(const char* 名字_小写, int 最小_小写短, int 最大_小写短, int 得到_小写);
extern int 检查实参至少(const char* 名字_小写, int 最小_小写短, int 得到_小写);
extern int 检查实参即使(const char* 名字_小写, int 得到_小写);

#endif // INCLUDE_VALIDATION_H
