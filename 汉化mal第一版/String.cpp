#include "Debug.h"
#include "String.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Adapted from: http://stackoverflow.com/questions/2342162
字符串_大写 字符串打印_小写(const char* fmt, ...) {
    int size = strlen(fmt); // make a guess
    字符串_大写 str;
    va_list ap;
    while (1) {
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.数据_小写(), size, fmt, ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

字符串_大写 复制与自由_小写(char* 已分配内存字符串_小写)
{
    字符串_大写 ret(已分配内存字符串_小写);
    free(已分配内存字符串_小写);
    return ret;
}

字符串_大写 跳出_小写(const 字符串_大写& in)
{
    字符串_大写 出_小写;
    出_小写.reserve(in.size() * 2 + 2); // each char may 获取_小写 escaped + two "'s
    出_小写 += '"';
    for (auto it = in.begin(), end = in.end(); it != end; ++it) {
        char c = *it;
        switch (c) {
            case '\\': 出_小写 += "\\\\"; break;
            case '\n': 出_小写 += "\\n"; break;
            case '"':  出_小写 += "\\\""; break;
            default:   出_小写 += c;      break;
        };
    }
    出_小写 += '"';
    出_小写.shrink_to_fit();
    return 出_小写;
}

static char 不跳出_小写(char c)
{
    switch (c) {
        case '\\':  return '\\';
        case 'n':   return '\n';
        case '"':   return '"';
        default:    return c;
    }
}

字符串_大写 不跳出_小写(const 字符串_大写& in)
{
    字符串_大写 出_小写;
    出_小写.reserve(in.size()); // unescaped string will always be shorter

    // in will have double-quotes at either end, so move the iterators in
    for (auto it = in.begin()+1, end = in.end()-1; it != end; ++it) {
        char c = *it;
        if (c == '\\') {
            ++it;
            if (it != end) {
                出_小写 += 不跳出_小写(*it);
            }
        }
        else {
            出_小写 += c;
        }
    }
    出_小写.shrink_to_fit();
    return 出_小写;
}

