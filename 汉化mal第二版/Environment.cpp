#include "Environment.h"
#include "Types.h"

#include <algorithm>

制作树语言环境_小写::制作树语言环境_小写(制作树语言环境指针_小写 outer)
: m_outer(outer)
{
    TRACE_ENV("创建树语言环境 %p, 外层的=%p\n", this, m_outer.ptr());
}

制作树语言环境_小写::制作树语言环境_小写(制作树语言环境指针_小写 outer, const 字符串向量_大写短& bindings,
               制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd)
: m_outer(outer)
{
    TRACE_ENV("创建树语言环境 %p, 外层的=%p\n", this, m_outer.ptr());
    int n = bindings.size();
    auto it = argsBegin;
    for (int i = 0; i < n; i++) {
        if (bindings[i] == "&") {
            MAL_CHECK(i == n - 2, "在这之后必须有一个形参 &");

            set(bindings[n-1], 制作树语言_小写::list(it, argsEnd));
            return;
        }
        MAL_CHECK(it != argsEnd, "没有足够的形参");
        set(bindings[i], *it);
        ++it;
    }
    MAL_CHECK(it == argsEnd, "有太多形参");
}

制作树语言环境_小写::~制作树语言环境_小写()
{
    TRACE_ENV("摧毁树语言环境 %p, 外层的=%p\n", this, m_outer.ptr());
}

制作树语言环境指针_小写 制作树语言环境_小写::find(const 字符串_大写& symbol)
{
    for (制作树语言环境指针_小写 env = this; env; env = env->m_outer) {
        if (env->m_map.find(symbol) != env->m_map.end()) {
            return env;
        }
    }
    return NULL;
}

制作树语言值指针_小写 制作树语言环境_小写::get(const 字符串_大写& symbol)
{
    for (制作树语言环境指针_小写 env = this; env; env = env->m_outer) {
        auto it = env->m_map.find(symbol);
        if (it != env->m_map.end()) {
            return it->second;
        }
    }
    MAL_FAIL("'%s' 未找到", symbol.c_str());
}

制作树语言值指针_小写 制作树语言环境_小写::set(const 字符串_大写& symbol, 制作树语言值指针_小写 value)
{
    m_map[symbol] = value;
    return value;
}

制作树语言环境指针_小写 制作树语言环境_小写::getRoot()
{
    // Work our way down the the global environment.
    for (制作树语言环境指针_小写 env = this; ; env = env->m_outer) {
        if (!env->m_outer) {
            return env;
        }
    }
}
