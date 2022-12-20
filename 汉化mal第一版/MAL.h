#ifndef INCLUDE_MAL_H
#define INCLUDE_MAL_H

#include "Debug.h"
#include "RefCountedPtr.h"
#include "String.h"
#include "Validation.h"

#include <vector>

class 制作树语言值_小写;
typedef 引用计数指针_短<制作树语言值_小写>  制作树语言值指针_小写;
typedef std::vector<制作树语言值指针_小写> 制作树语言值向量_小写;
typedef 制作树语言值向量_小写::iterator    制作树语言值迭代器_小写;

class 制作树语言环境_小写;
typedef 引用计数指针_短<制作树语言环境_小写>     制作树语言环境指针_小写;

// step*.cpp
extern 制作树语言值指针_小写 应用_大写(制作树语言值指针_小写 op,
                         制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写);
extern 制作树语言值指针_小写 求值_大写(制作树语言值指针_小写 抽象句法树_小写, 制作树语言环境指针_小写 环境_小写短);
extern 制作树语言值指针_小写 读行_小写(const 字符串_大写& 提示符_小写);
extern 字符串_大写 读求值印_小写(const 字符串_大写& 输入_小写, 制作树语言环境指针_小写 环境_小写短);

// Core.cpp
extern void 安装核心_小写(制作树语言环境指针_小写 环境_小写短);

// Reader.cpp
extern 制作树语言值指针_小写 读字符串_小写(const 字符串_大写& 输入_小写);

#endif // INCLUDE_MAL_H
