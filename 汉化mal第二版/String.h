#ifndef INCLUDE_STRING_H
#define INCLUDE_STRING_H

#include <string>
#include <vector>

typedef std::string         字符串_大写;
typedef std::vector<字符串_大写> 字符串向量_大写短;

#define STRF        stringPrintf
#define PLURAL(n)   &("s"[(n)==1])

extern 字符串_大写 stringPrintf(const char* fmt, ...);
extern 字符串_大写 copyAndFree(char* mallocedString);
extern 字符串_大写 escape(const 字符串_大写& s);
extern 字符串_大写 unescape(const 字符串_大写& s);

#endif // INCLUDE_STRING_H
