#include "Environment.h"
#include "Types.h"

#include <algorithm>

制作树语言环境_小写::制作树语言环境_小写(制作树语言环境指针_小写 外层_小写)
: 成员_外层_小写(外层_小写)
{
    追踪_环境_大写短("Creating 制作树语言环境_小写 %p, 外层_小写=%p\n", this, 成员_外层_小写.ptr());
}

制作树语言环境_小写::制作树语言环境_小写(制作树语言环境指针_小写 外层_小写, const 字符串向量_大写短& 绑定_小写复,
               制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写)
: 成员_外层_小写(外层_小写)
{
    追踪_环境_大写短("Creating 制作树语言环境_小写 %p, 外层_小写=%p\n", this, 成员_外层_小写.ptr());
    int n = 绑定_小写复.size();
    auto it = 实参开头_小写;
    for (int i = 0; i < n; i++) {
        if (绑定_小写复[i] == "&") {
            制作树语言_检查_大写(i == n - 2, "There must be one parameter after the &");

            设置_小写(绑定_小写复[n-1], 制作树语言_小写::列表_小写(it, 实参终_小写));
            return;
        }
        制作树语言_检查_大写(it != 实参终_小写, "Not enough parameters");
        设置_小写(绑定_小写复[i], *it);
        ++it;
    }
    制作树语言_检查_大写(it == 实参终_小写, "Too many parameters");
}

制作树语言环境_小写::~制作树语言环境_小写()
{
    追踪_环境_大写短("Destroying 制作树语言环境_小写 %p, 外层_小写=%p\n", this, 成员_外层_小写.ptr());
}

制作树语言环境指针_小写 制作树语言环境_小写::找_小写(const 字符串_大写& 符号_小写)
{
    for (制作树语言环境指针_小写 环境_小写短 = this; 环境_小写短; 环境_小写短 = 环境_小写短->成员_外层_小写) {
        if (环境_小写短->成员_映射_小写.找_小写(符号_小写) != 环境_小写短->成员_映射_小写.end()) {
            return 环境_小写短;
        }
    }
    return NULL;
}

制作树语言值指针_小写 制作树语言环境_小写::获取_小写(const 字符串_大写& 符号_小写)
{
    for (制作树语言环境指针_小写 环境_小写短 = this; 环境_小写短; 环境_小写短 = 环境_小写短->成员_外层_小写) {
        auto it = 环境_小写短->成员_映射_小写.找_小写(符号_小写);
        if (it != 环境_小写短->成员_映射_小写.end()) {
            return it->second;
        }
    }
    制作树语言_失败_大写("'%s' not found", 符号_小写.c_str());
}

制作树语言值指针_小写 制作树语言环境_小写::设置_小写(const 字符串_大写& 符号_小写, 制作树语言值指针_小写 值_小写)
{
    成员_映射_小写[符号_小写] = 值_小写;
    return 值_小写;
}

制作树语言环境指针_小写 制作树语言环境_小写::获取根_小写()
{
    // Work our way down the the global environment.
    for (制作树语言环境指针_小写 环境_小写短 = this; ; 环境_小写短 = 环境_小写短->成员_外层_小写) {
        if (!环境_小写短->成员_外层_小写) {
            return 环境_小写短;
        }
    }
}
