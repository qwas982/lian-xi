#include "Debug.h"
#include "Environment.h"
#include "Types.h"

#include <algorithm>
#include <memory>
#include <typeinfo>

namespace 制作树语言_小写 {
    制作树语言值指针_小写 atom(制作树语言值指针_小写 value) {
        return 制作树语言值指针_小写(new 制作树语言原子_小写(value));
    };

    制作树语言值指针_小写 boolean(bool value) {
        return value ? trueValue() : falseValue();
    }

    制作树语言值指针_小写 builtin(const 字符串_大写& name, 制作树语言内建的_小写::ApplyFunc handler) {
        return 制作树语言值指针_小写(new 制作树语言内建的_小写(name, handler));
    };

    制作树语言值指针_小写 falseValue() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("假"));
        return 制作树语言值指针_小写(c);
    };


    制作树语言值指针_小写 hash(const 制作树语言哈希_小写::Map& map) {
        return 制作树语言值指针_小写(new 制作树语言哈希_小写(map));
    }

    制作树语言值指针_小写 hash(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd,
                     bool isEvaluated) {
        return 制作树语言值指针_小写(new 制作树语言哈希_小写(argsBegin, argsEnd, isEvaluated));
    }

    制作树语言值指针_小写 integer(int64_t value) {
        return 制作树语言值指针_小写(new 制作树语言整数_小写(value));
    };

    制作树语言值指针_小写 integer(const 字符串_大写& token) {
        return integer(std::stoi(token));
    };

    制作树语言值指针_小写 keyword(const 字符串_大写& token) {
        return 制作树语言值指针_小写(new 制作树语言关键字_小写(token));
    };

    制作树语言值指针_小写 lambda(const 字符串向量_大写短& bindings,
                       制作树语言值指针_小写 body, 制作树语言环境指针_小写 env) {
        return 制作树语言值指针_小写(new 制作树语言兰姆达_小写(bindings, body, env));
    }

    制作树语言值指针_小写 list(制作树语言值向量_小写* items) {
        return 制作树语言值指针_小写(new 制作树语言列表_小写(items));
    };

    制作树语言值指针_小写 list(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end) {
        return 制作树语言值指针_小写(new 制作树语言列表_小写(begin, end));
    };

    制作树语言值指针_小写 list(制作树语言值指针_小写 a) {
        制作树语言值向量_小写* items = new 制作树语言值向量_小写(1);
        items->at(0) = a;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(items));
    }

    制作树语言值指针_小写 list(制作树语言值指针_小写 a, 制作树语言值指针_小写 b) {
        制作树语言值向量_小写* items = new 制作树语言值向量_小写(2);
        items->at(0) = a;
        items->at(1) = b;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(items));
    }

    制作树语言值指针_小写 list(制作树语言值指针_小写 a, 制作树语言值指针_小写 b, 制作树语言值指针_小写 c) {
        制作树语言值向量_小写* items = new 制作树语言值向量_小写(3);
        items->at(0) = a;
        items->at(1) = b;
        items->at(2) = c;
        return 制作树语言值指针_小写(new 制作树语言列表_小写(items));
    }

    制作树语言值指针_小写 macro(const 制作树语言兰姆达_小写& lambda) {
        return 制作树语言值指针_小写(new 制作树语言兰姆达_小写(lambda, true));
    };

    制作树语言值指针_小写 nilValue() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("无值"));
        return 制作树语言值指针_小写(c);
    };

    制作树语言值指针_小写 string(const 字符串_大写& token) {
        return 制作树语言值指针_小写(new 制作树语言字符串_小写(token));
    }

    制作树语言值指针_小写 symbol(const 字符串_大写& token) {
        return 制作树语言值指针_小写(new 制作树语言符号_小写(token));
    };

    制作树语言值指针_小写 trueValue() {
        static 制作树语言值指针_小写 c(new 制作树语言常量_小写("真"));
        return 制作树语言值指针_小写(c);
    };

    制作树语言值指针_小写 vector(制作树语言值向量_小写* items) {
        return 制作树语言值指针_小写(new 制作树语言向量_小写(items));
    };

    制作树语言值指针_小写 vector(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end) {
        return 制作树语言值指针_小写(new 制作树语言向量_小写(begin, end));
    };
};

制作树语言值指针_小写 制作树语言内建的_小写::apply(制作树语言值迭代器_小写 argsBegin,
                              制作树语言值迭代器_小写 argsEnd) const
{
    return m_handler(m_name, argsBegin, argsEnd);
}

static 字符串_大写 makeHashKey(制作树语言值指针_小写 key)
{
    if (const 制作树语言字符串_小写* skey = DYNAMIC_CAST(制作树语言字符串_小写, key)) {
        return skey->print(true);
    }
    else if (const 制作树语言关键字_小写* kkey = DYNAMIC_CAST(制作树语言关键字_小写, key)) {
        return kkey->print(true);
    }
    MAL_FAIL("%s 不是字符串或关键字", key->print(true).c_str());
}

static 制作树语言哈希_小写::Map addToMap(制作树语言哈希_小写::Map& map,
    制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd)
{
    // This is intended to be called with pre-evaluated arguments.
    for (auto it = argsBegin; it != argsEnd; ++it) {
        字符串_大写 key = makeHashKey(*it++);
        map[key] = *it;
    }

    return map;
}

static 制作树语言哈希_小写::Map createMap(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd)
{
    MAL_CHECK(std::distance(argsBegin, argsEnd) % 2 == 0,
            "哈希-映射需要一个均匀大小的列表");

    制作树语言哈希_小写::Map map;
    return addToMap(map, argsBegin, argsEnd);
}

制作树语言哈希_小写::制作树语言哈希_小写(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd, bool isEvaluated)
: m_map(createMap(argsBegin, argsEnd))
, m_isEvaluated(isEvaluated)
{

}

制作树语言哈希_小写::制作树语言哈希_小写(const 制作树语言哈希_小写::Map& map)
: m_map(map)
, m_isEvaluated(true)
{

}

制作树语言值指针_小写
制作树语言哈希_小写::assoc(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const
{
    MAL_CHECK(std::distance(argsBegin, argsEnd) % 2 == 0,
            "关联 需要一个均匀大小的列表");

    制作树语言哈希_小写::Map map(m_map);
    return 制作树语言_小写::hash(addToMap(map, argsBegin, argsEnd));
}

bool 制作树语言哈希_小写::contains(制作树语言值指针_小写 key) const
{
    auto it = m_map.find(makeHashKey(key));
    return it != m_map.end();
}

制作树语言值指针_小写
制作树语言哈希_小写::dissoc(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const
{
    制作树语言哈希_小写::Map map(m_map);
    for (auto it = argsBegin; it != argsEnd; ++it) {
        字符串_大写 key = makeHashKey(*it);
        map.erase(key);
    }
    return 制作树语言_小写::hash(map);
}

制作树语言值指针_小写 制作树语言哈希_小写::eval(制作树语言环境指针_小写 env)
{
    if (m_isEvaluated) {
        return 制作树语言值指针_小写(this);
    }

    制作树语言哈希_小写::Map map;
    for (auto it = m_map.begin(), end = m_map.end(); it != end; ++it) {
        map[it->first] = EVAL(it->second, env);
    }
    return 制作树语言_小写::hash(map);
}

制作树语言值指针_小写 制作树语言哈希_小写::get(制作树语言值指针_小写 key) const
{
    auto it = m_map.find(makeHashKey(key));
    return it == m_map.end() ? 制作树语言_小写::nilValue() : it->second;
}

制作树语言值指针_小写 制作树语言哈希_小写::keys() const
{
    制作树语言值向量_小写* keys = new 制作树语言值向量_小写();
    keys->reserve(m_map.size());
    for (auto it = m_map.begin(), end = m_map.end(); it != end; ++it) {
        if (it->first[0] == '"') {
            keys->push_back(制作树语言_小写::string(unescape(it->first)));
        }
        else {
            keys->push_back(制作树语言_小写::keyword(it->first));
        }
    }
    return 制作树语言_小写::list(keys);
}

制作树语言值指针_小写 制作树语言哈希_小写::values() const
{
    制作树语言值向量_小写* keys = new 制作树语言值向量_小写();
    keys->reserve(m_map.size());
    for (auto it = m_map.begin(), end = m_map.end(); it != end; ++it) {
        keys->push_back(it->second);
    }
    return 制作树语言_小写::list(keys);
}

字符串_大写 制作树语言哈希_小写::print(bool readably) const
{
    字符串_大写 s = "{";

    auto it = m_map.begin(), end = m_map.end();
    if (it != end) {
        s += it->first + " " + it->second->print(readably);
        ++it;
    }
    for ( ; it != end; ++it) {
        s += " " + it->first + " " + it->second->print(readably);
    }

    return s + "}";
}

bool 制作树语言哈希_小写::doIsEqualTo(const 制作树语言值_小写* rhs) const
{
    const 制作树语言哈希_小写::Map& r_map = static_cast<const 制作树语言哈希_小写*>(rhs)->m_map;
    if (m_map.size() != r_map.size()) {
        return false;
    }

    for (auto it0 = m_map.begin(), end0 = m_map.end(), it1 = r_map.begin();
         it0 != end0; ++it0, ++it1) {

        if (it0->first != it1->first) {
            return false;
        }
        if (!it0->second->isEqualTo(it1->second.ptr())) {
            return false;
        }
    }
    return true;
}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 字符串向量_大写短& bindings,
                     制作树语言值指针_小写 body, 制作树语言环境指针_小写 env)
: m_bindings(bindings)
, m_body(body)
, m_env(env)
, m_isMacro(false)
{

}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, 制作树语言值指针_小写 meta)
: 制作树语言可应用的_小写(meta)
, m_bindings(that.m_bindings)
, m_body(that.m_body)
, m_env(that.m_env)
, m_isMacro(that.m_isMacro)
{

}

制作树语言兰姆达_小写::制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, bool isMacro)
: 制作树语言可应用的_小写(that.m_meta)
, m_bindings(that.m_bindings)
, m_body(that.m_body)
, m_env(that.m_env)
, m_isMacro(isMacro)
{

}

制作树语言值指针_小写 制作树语言兰姆达_小写::apply(制作树语言值迭代器_小写 argsBegin,
                             制作树语言值迭代器_小写 argsEnd) const
{
    return EVAL(m_body, makeEnv(argsBegin, argsEnd));
}

制作树语言值指针_小写 制作树语言兰姆达_小写::doWithMeta(制作树语言值指针_小写 meta) const
{
    return new 制作树语言兰姆达_小写(*this, meta);
}

制作树语言环境指针_小写 制作树语言兰姆达_小写::makeEnv(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const
{
    return 制作树语言环境指针_小写(new 制作树语言环境_小写(m_env, m_bindings, argsBegin, argsEnd));
}

制作树语言值指针_小写 制作树语言列表_小写::conj(制作树语言值迭代器_小写 argsBegin,
                          制作树语言值迭代器_小写 argsEnd) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(argsBegin, argsEnd);

    制作树语言值向量_小写* items = new 制作树语言值向量_小写(oldItemCount + newItemCount);
    std::reverse_copy(argsBegin, argsEnd, items->begin());
    std::copy(begin(), end(), items->begin() + newItemCount);

    return 制作树语言_小写::list(items);
}

制作树语言值指针_小写 制作树语言列表_小写::eval(制作树语言环境指针_小写 env)
{
    // Note, this isn't actually called since the TCO updates, but
    // is required for the earlier steps, so don't get rid of it.
    if (count() == 0) {
        return 制作树语言值指针_小写(this);
    }

    std::unique_ptr<制作树语言值向量_小写> items(evalItems(env));
    auto it = items->begin();
    制作树语言值指针_小写 op = *it;
    return APPLY(op, ++it, items->end());
}

字符串_大写 制作树语言列表_小写::print(bool readably) const
{
    return '(' + 制作树语言序列_小写::print(readably) + ')';
}

制作树语言值指针_小写 制作树语言值_小写::eval(制作树语言环境指针_小写 env)
{
    // Default case of eval is just to return the object itself.
    return 制作树语言值指针_小写(this);
}

bool 制作树语言值_小写::isEqualTo(const 制作树语言值_小写* rhs) const
{
    // Special-case. Vectors and Lists can be compared.
    bool matchingTypes = (typeid(*this) == typeid(*rhs)) ||
        (dynamic_cast<const 制作树语言序列_小写*>(this) &&
         dynamic_cast<const 制作树语言序列_小写*>(rhs));

    return matchingTypes && doIsEqualTo(rhs);
}

bool 制作树语言值_小写::isTrue() const
{
    return (this != 制作树语言_小写::falseValue().ptr())
        && (this != 制作树语言_小写::nilValue().ptr());
}

制作树语言值指针_小写 制作树语言值_小写::meta() const
{
    return m_meta.ptr() == NULL ? 制作树语言_小写::nilValue() : m_meta;
}

制作树语言值指针_小写 制作树语言值_小写::withMeta(制作树语言值指针_小写 meta) const
{
    return doWithMeta(meta);
}

制作树语言序列_小写::制作树语言序列_小写(制作树语言值向量_小写* items)
: m_items(items)
{

}

制作树语言序列_小写::制作树语言序列_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end)
: m_items(new 制作树语言值向量_小写(begin, end))
{

}

制作树语言序列_小写::制作树语言序列_小写(const 制作树语言序列_小写& that, 制作树语言值指针_小写 meta)
: 制作树语言值_小写(meta)
, m_items(new 制作树语言值向量_小写(*(that.m_items)))
{

}

制作树语言序列_小写::~制作树语言序列_小写()
{
    delete m_items;
}

bool 制作树语言序列_小写::doIsEqualTo(const 制作树语言值_小写* rhs) const
{
    const 制作树语言序列_小写* rhsSeq = static_cast<const 制作树语言序列_小写*>(rhs);
    if (count() != rhsSeq->count()) {
        return false;
    }

    for (制作树语言值迭代器_小写 it0 = m_items->begin(),
                      it1 = rhsSeq->begin(),
                      end = m_items->end(); it0 != end; ++it0, ++it1) {

        if (! (*it0)->isEqualTo((*it1).ptr())) {
            return false;
        }
    }
    return true;
}

制作树语言值向量_小写* 制作树语言序列_小写::evalItems(制作树语言环境指针_小写 env) const
{
    制作树语言值向量_小写* items = new 制作树语言值向量_小写;;
    items->reserve(count());
    for (auto it = m_items->begin(), end = m_items->end(); it != end; ++it) {
        items->push_back(EVAL(*it, env));
    }
    return items;
}

制作树语言值指针_小写 制作树语言序列_小写::first() const
{
    return count() == 0 ? 制作树语言_小写::nilValue() : item(0);
}

字符串_大写 制作树语言序列_小写::print(bool readably) const
{
    字符串_大写 str;
    auto end = m_items->cend();
    auto it = m_items->cbegin();
    if (it != end) {
        str += (*it)->print(readably);
        ++it;
    }
    for ( ; it != end; ++it) {
        str += " ";
        str += (*it)->print(readably);
    }
    return str;
}

制作树语言值指针_小写 制作树语言序列_小写::rest() const
{
    制作树语言值迭代器_小写 start = (count() > 0) ? begin() + 1 : end();
    return 制作树语言_小写::list(start, end());
}

字符串_大写 制作树语言字符串_小写::escapedValue() const
{
    return escape(value());
}

字符串_大写 制作树语言字符串_小写::print(bool readably) const
{
    return readably ? escapedValue() : value();
}

制作树语言值指针_小写 制作树语言符号_小写::eval(制作树语言环境指针_小写 env)
{
    return env->get(value());
}

制作树语言值指针_小写 制作树语言向量_小写::conj(制作树语言值迭代器_小写 argsBegin,
                            制作树语言值迭代器_小写 argsEnd) const
{
    int oldItemCount = std::distance(begin(), end());
    int newItemCount = std::distance(argsBegin, argsEnd);

    制作树语言值向量_小写* items = new 制作树语言值向量_小写(oldItemCount + newItemCount);
    std::copy(begin(), end(), items->begin());
    std::copy(argsBegin, argsEnd, items->begin() + oldItemCount);

    return 制作树语言_小写::vector(items);
}

制作树语言值指针_小写 制作树语言向量_小写::eval(制作树语言环境指针_小写 env)
{
    return 制作树语言_小写::vector(evalItems(env));
}

字符串_大写 制作树语言向量_小写::print(bool readably) const
{
    return '[' + 制作树语言序列_小写::print(readably) + ']';
}
