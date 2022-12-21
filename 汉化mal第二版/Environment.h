#ifndef INCLUDE_ENVIRONMENT_H
#define INCLUDE_ENVIRONMENT_H

#include "MAL.h"

#include <map>

class 制作树语言环境_小写 : public 已被引用计数的 {
public:
    制作树语言环境_小写(制作树语言环境指针_小写 outer = NULL);
    制作树语言环境_小写(制作树语言环境指针_小写 outer,
           const 字符串向量_大写短& bindings,
           制作树语言值迭代器_小写 argsBegin,
           制作树语言值迭代器_小写 argsEnd);

    ~制作树语言环境_小写();

    制作树语言值指针_小写 get(const 字符串_大写& symbol);
    制作树语言环境指针_小写   find(const 字符串_大写& symbol);
    制作树语言值指针_小写 set(const 字符串_大写& symbol, 制作树语言值指针_小写 value);
    制作树语言环境指针_小写   getRoot();

private:
    typedef std::map<字符串_大写, 制作树语言值指针_小写> Map;
    Map m_map;
    制作树语言环境指针_小写 m_outer;
};

#endif // INCLUDE_ENVIRONMENT_H
