#ifndef INCLUDE_STRING_H
#define INCLUDE_STRING_H

#include <string>
#include <vector>

typedef std::string         字符串_大写;
typedef std::vector<字符串_大写> 字符串向量_大写短;

#define 字符串打印_大写缩        字符串打印_小写
#define 复数_大写(n)   &("s"[(n)==1])

extern 字符串_大写 字符串打印_小写(const char* fmt, ...);
extern 字符串_大写 复制与自由_小写(char* 已分配内存字符串_小写);
extern 字符串_大写 跳出_小写(const 字符串_大写& s);
extern 字符串_大写 不跳出_小写(const 字符串_大写& s);

#endif // INCLUDE_STRING_H
