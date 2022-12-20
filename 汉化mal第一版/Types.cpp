#include "Debug.h"
#include "Environment.h"
#include "Types.h"

#include <algorithm>
#include <memory>
#include <typeinfo>

namespace 制作树语言_小写 {
    制作树语言值指针_小写 原子_小写(制作树语言值指针_小写 值_小写) {
        return 制作树语言值指针_小写(new 制作树语言原子_小写(值_小写));
    };

    制作树语言值指针_小写 布尔_小写(bool 值_小写) {
        return 值_小写 ? 真值_小写() : 假值_小写();
    }

    制作树语言值指针_小写 内建_小写(const 字符串_大写& 名字_小写, 制作树语言内建的_小写::应用函数_大写短 句柄者_小写) {
        return 制作树语言值指针_小写(new 制作树语言内建的_小写(名字_小写, 句柄者_小写));
    };

    制作树语言值指针_小写 假值_小写() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("false"));
        return 制作树语言值指针_小写(c);
    };


    制作树语言值指针_小写 哈希_小写(const 制作树语言哈希_小写::映射_大写& map) {
        return 制作树语言值指针_小写(new 制作树语言哈希_小写(map));
    }

    制作树语言值指针_小写 哈希_小写(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写,
                     bool 是否已求值_小写) {
        return 制作树语言值指针_小写(new 制作树语言哈希_小写(实参开头_小写, 实参终_小写, 是否已求值_小写));
    }

    制作树语言值指针_小写 整数_小写(int64_t 值_小写) {
        return 制作树语言值指针_小写(new 制作树语言整数_小写(值_小写));
    };

    制作树语言值指针_小写 整数_小写(const 字符串_大写& 词牌_小写) {
        return 整数_小写(std::stoi(词牌_小写));
    };

    制作树语言值指针_小写 关键字_小写(const 字符串_大写& 词牌_小写) {
        return 制作树语言值指针_小写(new 制作树语言关键字_小写(词牌_小写));
    };

    制作树语言值指针_小写 兰姆达_小写(const 字符串向量_大写短& 绑定_小写复,
                       制作树语言值指针_小写 body, 制作树语言环境指针_小写 环境_小写短) {
        return 制作树语言值指针_小写(new 制作树语言兰姆达_小写(绑定_小写复, body, 环境_小写短));
    }

    制作树语言值指针_小写 列表_小写(制作树语言值向量_小写* 条目_小写复) {
        return 制作树语言值指针_小写(new 制作树语言列表_小写(条目_小写复));
    };

    制作树语言值指针_小写 列表_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end) {
        return 制作树语言值指针_小写(new 制作树语言列表_小写(begin, end));
    };

    制作树语言值指针_小写 列表_小写(制作树语言值指针_小写 a) {
        制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写(1);
        条目_小写复->at(0) = a;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(条目_小写复));
    }

    制作树语言值指针_小写 列表_小写(制作树语言值指针_小写 a, 制作树语言值指针_小写 b) {
        制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写(2);
        条目_小写复->at(0) = a;
        条目_小写复->at(1) = b;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(条目_小写复));
    }

    制作树语言值指针_小写 列表_小写(制作树语言值指针_小写 a, 制作树语言值指针_小写 b, 制作树语言值指针_小写 c) {
        制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写(3);
        条目_小写复->at(0) = a;
        条目_小写复->at(1) = b;
        条目_小写复->at(2) = c;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(条目_小写复));
    }

    制作树语言值指针_小写 宏_小写(const 制作树语言兰姆达_小写& 兰姆达_小写) {
        return 制作树语言值指针_小写(new 制作树语言兰姆达_小写(兰姆达_小写, true));
    };

    制作树语言值指针_小写 零值_小写() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("零值_小写"));
        return 制作树语言值指针_小写(c);
    };

    制作树语言值指针_小写 字符串_小写(const 字符串_大写& 词牌_小写) {
        return 制作树语言值指针_小写(new 制作树语言字符串_小写(词牌_小写));
    }

    制作树语言值指针_小写 符号_小写(const 字符串_大写& 词牌_小写) {
        return 制作树语言值指针_小写(new 制作树语言符号_小写(词牌_小写));
    };

    制作树语言值指针_小写 真值_小写() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("true"));
        return 制作树语言值指针_小写(c);
    };

    制作树语言值指针_小写 向量_小写(制作树语言值向量_小写* 条目_小写复) {
        return 制作树语言值指针_小写(new 制作树语言向量_小写(条目_小写复));
    };

    制作树语言值指针_小写 向量_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end) {
        return 制作树语言值指针_小写(new 制作树语言向量_小写(begin, end));
    };
};

制作树语言值指针_小写 制作树语言内建的_小写::应用_小写(制作树语言值迭代器_小写 实参开头_小写,
                              制作树语言值迭代器_小写 实参终_小写) const
{
    return 成员_句柄者_小写(成员_名字_小写, 实参开头_小写, 实参终_小写);
}

static 字符串_大写 制作哈希键(制作树语言值指针_小写 键_小写单)
{
    if (const 制作树语言字符串_小写* skey = 动态_类型转换_大写(制作树语言字符串_小写, 键_小写单)) {
        return skey->打印_小写(true);
    }
    else if (const 制作树语言关键字_小写* kkey = 动态_类型转换_大写(制作树语言关键字_小写, 键_小写单)) {
        return kkey->打印_小写(true);
    }
    制作树语言_失败_大写("%s is not a string or keyword", 键_小写单->打印_小写(true).c_str());
}

static 制作树语言哈希_小写::映射_大写 增加到映射_小写(制作树语言哈希_小写::映射_大写& map,
    制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写)
{
    // This is intended to be called with pre-evaluated arguments.
    for (auto it = 实参开头_小写; it != 实参终_小写; ++it) {
        字符串_大写 键_小写单 = 制作哈希键(*it++);
        map[键_小写单] = *it;
    }

    return map;
}

static 制作树语言哈希_小写::映射_大写 创建映射_小写(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写)
{
    制作树语言_检查_大写(std::distance(实参开头_小写, 实参终_小写) % 2 == 0,
            "哈希_小写-map requires an even-sized list");

    制作树语言哈希_小写::映射_大写 map;
    return 增加到映射_小写(map, 实参开头_小写, 实参终_小写);
}

制作树语言哈希_小写::制作树语言哈希_小写(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写, bool 是否已求值_小写)
: 成员_映射_小写(创建映射_小写(实参开头_小写, 实参终_小写))
, 成员_是否已求值_小写(是否已求值_小写)
{

}

制作树语言哈希_小写::制作树语言哈希_小写(const 制作树语言哈希_小写::映射_大写& map)
: 成员_映射_小写(map)
, 成员_是否已求值_小写(true)
{

}

制作树语言值指针_小写
制作树语言哈希_小写::关联_小写短(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写) const
{
    制作树语言_检查_大写(std::distance(实参开头_小写, 实参终_小写) % 2 == 0,
            "assoc requires an even-sized list");

    制作树语言哈希_小写::映射_大写 map(成员_映射_小写);
    return 制作树语言_小写::哈希_小写(增加到映射_小写(map, 实参开头_小写, 实参终_小写));
}

bool 制作树语言哈希_小写::内容_小写(制作树语言值指针_小写 键_小写单) const
{
    auto it = 成员_映射_小写.找_小写(制作哈希键(键_小写单));
    return it != 成员_映射_小写.end();
}

制作树语言值指针_小写
制作树语言哈希_小写::解关联_小写短(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写) const
{
    制作树语言哈希_小写::映射_大写 map(成员_映射_小写);
    for (auto it = 实参开头_小写; it != 实参终_小写; ++it) {
        字符串_大写 键_小写单 = 制作哈希键(*it);
        map.erase(键_小写单);
    }
    return 制作树语言_小写::哈希_小写(map);
}

制作树语言值指针_小写 制作树语言哈希_小写::求值_小写短(制作树语言环境指针_小写 环境_小写短)
{
    if (成员_是否已求值_小写) {
        return 制作树语言值指针_小写(this);
    }

    制作树语言哈希_小写::映射_大写 map;
    for (auto it = 成员_映射_小写.begin(), end = 成员_映射_小写.end(); it != end; ++it) {
        map[it->首先_小写] = 求值_大写(it->second, 环境_小写短);
    }
    return 制作树语言_小写::哈希_小写(map);
}

制作树语言值指针_小写 制作树语言哈希_小写::获取_小写(制作树语言值指针_小写 键_小写单) const
{
    auto it = 成员_映射_小写.找_小写(制作哈希键(键_小写单));
    return it == 成员_映射_小写.end() ? 制作树语言_小写::零值_小写() : it->second;
}

制作树语言值指针_小写 制作树语言哈希_小写::键_小写复() const
{
    制作树语言值向量_小写* 键_小写复 = new 制作树语言值向量_小写();
    键_小写复->reserve(成员_映射_小写.size());
    for (auto it = 成员_映射_小写.begin(), end = 成员_映射_小写.end(); it != end; ++it) {
        if (it->首先_小写[0] == '"') {
            键_小写复->push_back(制作树语言_小写::字符串_小写(不跳出_小写(it->首先_小写)));
        }
        else {
            键_小写复->push_back(制作树语言_小写::关键字_小写(it->首先_小写));
        }
    }
    return 制作树语言_小写::列表_小写(键_小写复);
}

制作树语言值指针_小写 制作树语言哈希_小写::值_小写复() const
{
    制作树语言值向量_小写* 键_小写复 = new 制作树语言值向量_小写();
    键_小写复->reserve(成员_映射_小写.size());
    for (auto it = 成员_映射_小写.begin(), end = 成员_映射_小写.end(); it != end; ++it) {
        键_小写复->push_back(it->second);
    }
    return 制作树语言_小写::列表_小写(键_小写复);
}

字符串_大写 制作树语言哈希_小写::打印_小写(bool 可读的_小写) const
{
    字符串_大写 s = "{";

    auto it = 成员_映射_小写.begin(), end = 成员_映射_小写.end();
    if (it != end) {
        s += it->首先_小写 + " " + it->second->打印_小写(可读的_小写);
        ++it;
    }
    for ( ; it != end; ++it) {
        s += " " + it->首先_小写 + " " + it->second->打印_小写(可读的_小写);
    }

    return s + "}";
}

bool 制作树语言哈希_小写::执行是否等同的_小写(const 制作树语言值_小写* 右手边_小写缩) const
{
    const 制作树语言哈希_小写::映射_大写& r_map = static_cast<const 制作树语言哈希_小写*>(右手边_小写缩)->成员_映射_小写;
    if (成员_映射_小写.size() != r_map.size()) {
        return false;
    }

    for (auto it0 = 成员_映射_小写.begin(), end0 = 成员_映射_小写.end(), it1 = r_map.begin();
         it0 != end0; ++it0, ++it1) {

        if (it0->首先_小写 != it1->首先_小写) {
            return false;
        }
        if (!it0->second->是否等同的_小写(it1->second.ptr())) {
            return false;
        }
    }
    return true;
}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 字符串向量_大写短& 绑定_小写复,
                     制作树语言值指针_小写 body, 制作树语言环境指针_小写 环境_小写短)
: 成员_绑定_小写复(绑定_小写复)
, 成员_身体_小写(body)
, 成员_环境_小写短(环境_小写短)
, 成员_是否宏_小写(false)
{

}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, 制作树语言值指针_小写 元_小写)
: 制作树语言可应用的_小写(元_小写)
, 成员_绑定_小写复(that.成员_绑定_小写复)
, 成员_身体_小写(that.成员_身体_小写)
, 成员_环境_小写短(that.成员_环境_小写短)
, 成员_是否宏_小写(that.成员_是否宏_小写)
{

}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, bool 是否宏_小写)
: 制作树语言可应用的_小写(that.成员_元_小写)
, 成员_绑定_小写复(that.成员_绑定_小写复)
, 成员_身体_小写(that.成员_身体_小写)
, 成员_环境_小写短(that.成员_环境_小写短)
, 成员_是否宏_小写(是否宏_小写)
{

}

制作树语言值指针_小写 制作树语言兰姆达_小写::应用_小写(制作树语言值迭代器_小写 实参开头_小写,
                             制作树语言值迭代器_小写 实参终_小写) const
{
    return 求值_大写(成员_身体_小写, 制作环境_小写短(实参开头_小写, 实参终_小写));
}

制作树语言值指针_小写 制作树语言兰姆达_小写::执行和元一起_小写(制作树语言值指针_小写 元_小写) const
{
    return new 制作树语言兰姆达_小写(*this, 元_小写);
}

制作树语言环境指针_小写 制作树语言兰姆达_小写::制作环境_小写短(制作树语言值迭代器_小写 实参开头_小写, 制作树语言值迭代器_小写 实参终_小写) const
{
    return 制作树语言环境指针_小写(new 制作树语言环境_小写(成员_环境_小写短, 成员_绑定_小写复, 实参开头_小写, 实参终_小写));
}

制作树语言值指针_小写 制作树语言列表_小写::共轭_小写短(制作树语言值迭代器_小写 实参开头_小写,
                          制作树语言值迭代器_小写 实参终_小写) const
{
    int 旧条目计数_小写 = std::distance(begin(), end());
    int 新条目计数_小写 = std::distance(实参开头_小写, 实参终_小写);

    制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写(旧条目计数_小写 + 新条目计数_小写);
    std::reverse_copy(实参开头_小写, 实参终_小写, 条目_小写复->begin());
    std::copy(begin(), end(), 条目_小写复->begin() + 新条目计数_小写);

    return 制作树语言_小写::列表_小写(条目_小写复);
}

制作树语言值指针_小写 制作树语言列表_小写::求值_小写短(制作树语言环境指针_小写 环境_小写短)
{
    // Note, this isn't actually called since the TCO updates, but
    // is required for the earlier steps, so don't 获取_小写 rid of it.
    if (计数_小写() == 0) {
        return 制作树语言值指针_小写(this);
    }

    std::unique_ptr<制作树语言值向量_小写> 条目_小写复(求值条目_小写复(环境_小写短));
    auto it = 条目_小写复->begin();
    制作树语言值指针_小写 op = *it;
    return 应用_大写(op, ++it, 条目_小写复->end());
}

字符串_大写 制作树语言列表_小写::打印_小写(bool 可读的_小写) const
{
    return '(' + 制作树语言队列_小写::打印_小写(可读的_小写) + ')';
}

制作树语言值指针_小写 制作树语言值_小写::求值_小写短(制作树语言环境指针_小写 环境_小写短)
{
    // Default case of 求值_小写短 is just to return the object itself.
    return 制作树语言值指针_小写(this);
}

bool 制作树语言值_小写::是否等同的_小写(const 制作树语言值_小写* 右手边_小写缩) const
{
    // Special-case. Vectors and Lists can be compared.
    bool matchingTypes = (typeid(*this) == typeid(*右手边_小写缩)) ||
        (dynamic_cast<const 制作树语言队列_小写*>(this) &&
         dynamic_cast<const 制作树语言队列_小写*>(右手边_小写缩));

    return matchingTypes && 执行是否等同的_小写(右手边_小写缩);
}

bool 制作树语言值_小写::是否真_小写() const
{
    return (this != 制作树语言_小写::假值_小写().ptr())
        && (this != 制作树语言_小写::零值_小写().ptr());
}

制作树语言值指针_小写 制作树语言值_小写::元_小写() const
{
    return 成员_元_小写.ptr() == NULL ? 制作树语言_小写::零值_小写() : 成员_元_小写;
}

制作树语言值指针_小写 制作树语言值_小写::和元一起_小写(制作树语言值指针_小写 元_小写) const
{
    return 执行和元一起_小写(元_小写);
}

制作树语言队列_小写::制作树语言队列_小写(制作树语言值向量_小写* 条目_小写复)
: 成员_条目_小写复(条目_小写复)
{

}

制作树语言队列_小写::制作树语言队列_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end)
: 成员_条目_小写复(new 制作树语言值向量_小写(begin, end))
{

}

制作树语言队列_小写::制作树语言队列_小写(const 制作树语言队列_小写& that, 制作树语言值指针_小写 元_小写)
: 制作树语言值_小写(元_小写)
, 成员_条目_小写复(new 制作树语言值向量_小写(*(that.成员_条目_小写复)))
{

}

制作树语言队列_小写::~制作树语言队列_小写()
{
    delete 成员_条目_小写复;
}

bool 制作树语言队列_小写::执行是否等同的_小写(const 制作树语言值_小写* 右手边_小写缩) const
{
    const 制作树语言队列_小写* rhsSeq = static_cast<const 制作树语言队列_小写*>(右手边_小写缩);
    if (计数_小写() != rhsSeq->计数_小写()) {
        return false;
    }

    for (制作树语言值迭代器_小写 it0 = 成员_条目_小写复->begin(),
                      it1 = rhsSeq->begin(),
                      end = 成员_条目_小写复->end(); it0 != end; ++it0, ++it1) {

        if (! (*it0)->是否等同的_小写((*it1).ptr())) {
            return false;
        }
    }
    return true;
}

制作树语言值向量_小写* 制作树语言队列_小写::求值条目_小写复(制作树语言环境指针_小写 环境_小写短) const
{
    制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写;;
    条目_小写复->reserve(计数_小写());
    for (auto it = 成员_条目_小写复->begin(), end = 成员_条目_小写复->end(); it != end; ++it) {
        条目_小写复->push_back(求值_大写(*it, 环境_小写短));
    }
    return 条目_小写复;
}

制作树语言值指针_小写 制作树语言队列_小写::首先_小写() const
{
    return 计数_小写() == 0 ? 制作树语言_小写::零值_小写() : 条目_小写(0);
}

字符串_大写 制作树语言队列_小写::打印_小写(bool 可读的_小写) const
{
    字符串_大写 str;
    auto end = 成员_条目_小写复->cend();
    auto it = 成员_条目_小写复->cbegin();
    if (it != end) {
        str += (*it)->打印_小写(可读的_小写);
        ++it;
    }
    for ( ; it != end; ++it) {
        str += " ";
        str += (*it)->打印_小写(可读的_小写);
    }
    return str;
}

制作树语言值指针_小写 制作树语言队列_小写::其他_小写() const
{
    制作树语言值迭代器_小写 start = (计数_小写() > 0) ? begin() + 1 : end();
    return 制作树语言_小写::列表_小写(start, end());
}

字符串_大写 制作树语言字符串_小写::已跳出的值_小写() const
{
    return 跳出_小写(值_小写());
}

字符串_大写 制作树语言字符串_小写::打印_小写(bool 可读的_小写) const
{
    return 可读的_小写 ? 已跳出的值_小写() : 值_小写();
}

制作树语言值指针_小写 制作树语言符号_小写::求值_小写短(制作树语言环境指针_小写 环境_小写短)
{
    return 环境_小写短->获取_小写(值_小写());
}

制作树语言值指针_小写 制作树语言向量_小写::共轭_小写短(制作树语言值迭代器_小写 实参开头_小写,
                            制作树语言值迭代器_小写 实参终_小写) const
{
    int 旧条目计数_小写 = std::distance(begin(), end());
    int 新条目计数_小写 = std::distance(实参开头_小写, 实参终_小写);

    制作树语言值向量_小写* 条目_小写复 = new 制作树语言值向量_小写(旧条目计数_小写 + 新条目计数_小写);
    std::copy(begin(), end(), 条目_小写复->begin());
    std::copy(实参开头_小写, 实参终_小写, 条目_小写复->begin() + 旧条目计数_小写);

    return 制作树语言_小写::向量_小写(条目_小写复);
}

制作树语言值指针_小写 制作树语言向量_小写::求值_小写短(制作树语言环境指针_小写 环境_小写短)
{
    return 制作树语言_小写::向量_小写(求值条目_小写复(环境_小写短));
}

字符串_大写 制作树语言向量_小写::打印_小写(bool 可读的_小写) const
{
    return '[' + 制作树语言队列_小写::打印_小写(可读的_小写) + ']';
}
