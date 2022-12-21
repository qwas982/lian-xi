#ifndef INCLUDE_MAL_H
#define INCLUDE_MAL_H

#include "Debug.h"
#include "RefCountedPtr.h"
#include "String.h"
#include "Validation.h"

#include <vector>

class 制作树语言值_小写;
typedef 已被引用计数的指针<制作树语言值_小写>  制作树语言值指针_小写;
typedef std::vector<制作树语言值指针_小写> 制作树语言值向量_小写;
typedef 制作树语言值向量_小写::iterator    制作树语言值迭代器_小写;

class 制作树语言环境_小写;
typedef 已被引用计数的指针<制作树语言环境_小写>     制作树语言环境指针_小写;

// step*.cpp
extern 制作树语言值指针_小写 APPLY(制作树语言值指针_小写 op,
                         制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd);
extern 制作树语言值指针_小写 EVAL(制作树语言值指针_小写 ast, 制作树语言环境指针_小写 env);
extern 制作树语言值指针_小写 readline(const 字符串_大写& prompt);
extern 字符串_大写 rep(const 字符串_大写& input, 制作树语言环境指针_小写 env);

// Core.cpp
extern void installCore(制作树语言环境指针_小写 env);

// Reader.cpp
extern 制作树语言值指针_小写 readStr(const 字符串_大写& input);

#endif // INCLUDE_MAL_H
