#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

#include "MAL.h"

#include <exception>
#include <map>

class 制作树语言空输入异常_小写 : public std::exception { };

class 制作树语言值_小写 : public 已被引用计数的 {
public:
    制作树语言值_小写() {
        TRACE_OBJECT("创建树语言值 %p\n", this);
    }
    制作树语言值_小写(制作树语言值指针_小写 meta) : m_meta(meta) {
        TRACE_OBJECT("创建树语言值 %p\n", this);
    }
    virtual ~制作树语言值_小写() {
        TRACE_OBJECT("摧毁树语言值 %p\n", this);
    }

    制作树语言值指针_小写 withMeta(制作树语言值指针_小写 meta) const;
    virtual 制作树语言值指针_小写 doWithMeta(制作树语言值指针_小写 meta) const = 0;
    制作树语言值指针_小写 meta() const;

    bool isTrue() const;

    bool isEqualTo(const 制作树语言值_小写* rhs) const;

    virtual 制作树语言值指针_小写 eval(制作树语言环境指针_小写 env);

    virtual 字符串_大写 print(bool readably) const = 0;

protected:
    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const = 0;

    制作树语言值指针_小写 m_meta;
};

template<class T>
T* value_cast(制作树语言值指针_小写 obj, const char* typeName) {
    T* dest = dynamic_cast<T*>(obj.ptr());
    MAL_CHECK(dest != NULL, "%s 不是一个 %s",
              obj->print(true).c_str(), typeName);
    return dest;
}

#define VALUE_CAST(Type, Value)    value_cast<Type>(Value, #Type)
#define DYNAMIC_CAST(Type, Value)  (dynamic_cast<Type*>((Value).ptr()))
#define STATIC_CAST(Type, Value)   (static_cast<Type*>((Value).ptr()))

#define WITH_META(Type) \
    virtual 制作树语言值指针_小写 doWithMeta(制作树语言值指针_小写 meta) const { \
        return new Type(*this, meta); \
    } \

class 制作树语言常量_小写 : public 制作树语言值_小写 {
public:
    制作树语言常量_小写(字符串_大写 name) : m_name(name) { }
    制作树语言常量_小写(const 制作树语言常量_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言值_小写(meta), m_name(that.m_name) { }

    virtual 字符串_大写 print(bool readably) const { return m_name; }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return this == rhs; // these are singletons
    }

    WITH_META(制作树语言常量_小写);

private:
    const 字符串_大写 m_name;
};

class 制作树语言整数_小写 : public 制作树语言值_小写 {
public:
    制作树语言整数_小写(int64_t value) : m_value(value) { }
    制作树语言整数_小写(const 制作树语言整数_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言值_小写(meta), m_value(that.m_value) { }

    virtual 字符串_大写 print(bool readably) const {
        return std::to_string(m_value);
    }

    int64_t value() const { return m_value; }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return m_value == static_cast<const 制作树语言整数_小写*>(rhs)->m_value;
    }

    WITH_META(制作树语言整数_小写);

private:
    const int64_t m_value;
};

class 制作树语言字符串基础_小写 : public 制作树语言值_小写 {
public:
    制作树语言字符串基础_小写(const 字符串_大写& token)
        : m_value(token) { }
    制作树语言字符串基础_小写(const 制作树语言字符串基础_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言值_小写(meta), m_value(that.value()) { }

    virtual 字符串_大写 print(bool readably) const { return m_value; }

    字符串_大写 value() const { return m_value; }

private:
    const 字符串_大写 m_value;
};

class 制作树语言字符串_小写 : public 制作树语言字符串基础_小写 {
public:
    制作树语言字符串_小写(const 字符串_大写& token)
        : 制作树语言字符串基础_小写(token) { }
    制作树语言字符串_小写(const 制作树语言字符串_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言字符串基础_小写(that, meta) { }

    virtual 字符串_大写 print(bool readably) const;

    字符串_大写 escapedValue() const;

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return value() == static_cast<const 制作树语言字符串_小写*>(rhs)->value();
    }

    WITH_META(制作树语言字符串_小写);
};

class 制作树语言关键字_小写 : public 制作树语言字符串基础_小写 {
public:
    制作树语言关键字_小写(const 字符串_大写& token)
        : 制作树语言字符串基础_小写(token) { }
    制作树语言关键字_小写(const 制作树语言关键字_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言字符串基础_小写(that, meta) { }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return value() == static_cast<const 制作树语言关键字_小写*>(rhs)->value();
    }

    WITH_META(制作树语言关键字_小写);
};

class 制作树语言符号_小写 : public 制作树语言字符串基础_小写 {
public:
    制作树语言符号_小写(const 字符串_大写& token)
        : 制作树语言字符串基础_小写(token) { }
    制作树语言符号_小写(const 制作树语言符号_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言字符串基础_小写(that, meta) { }

    virtual 制作树语言值指针_小写 eval(制作树语言环境指针_小写 env);

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return value() == static_cast<const 制作树语言符号_小写*>(rhs)->value();
    }

    WITH_META(制作树语言符号_小写);
};

class 制作树语言序列_小写 : public 制作树语言值_小写 {
public:
    制作树语言序列_小写(制作树语言值向量_小写* items);
    制作树语言序列_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end);
    制作树语言序列_小写(const 制作树语言序列_小写& that, 制作树语言值指针_小写 meta);
    virtual ~制作树语言序列_小写();

    virtual 字符串_大写 print(bool readably) const;

    制作树语言值向量_小写* evalItems(制作树语言环境指针_小写 env) const;
    int count() const { return m_items->size(); }
    bool isEmpty() const { return m_items->empty(); }
    制作树语言值指针_小写 item(int index) const { return (*m_items)[index]; }

    制作树语言值迭代器_小写 begin() const { return m_items->begin(); }
    制作树语言值迭代器_小写 end()   const { return m_items->end(); }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const;

    virtual 制作树语言值指针_小写 conj(制作树语言值迭代器_小写 argsBegin,
                              制作树语言值迭代器_小写 argsEnd) const = 0;

    制作树语言值指针_小写 first() const;
    virtual 制作树语言值指针_小写 rest() const;

private:
    制作树语言值向量_小写* const m_items;
};

class 制作树语言列表_小写 : public 制作树语言序列_小写 {
public:
    制作树语言列表_小写(制作树语言值向量_小写* items) : 制作树语言序列_小写(items) { }
    制作树语言列表_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end)
        : 制作树语言序列_小写(begin, end) { }
    制作树语言列表_小写(const 制作树语言列表_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言序列_小写(that, meta) { }

    virtual 字符串_大写 print(bool readably) const;
    virtual 制作树语言值指针_小写 eval(制作树语言环境指针_小写 env);

    virtual 制作树语言值指针_小写 conj(制作树语言值迭代器_小写 argsBegin,
                             制作树语言值迭代器_小写 argsEnd) const;

    WITH_META(制作树语言列表_小写);
};

class 制作树语言向量_小写 : public 制作树语言序列_小写 {
public:
    制作树语言向量_小写(制作树语言值向量_小写* items) : 制作树语言序列_小写(items) { }
    制作树语言向量_小写(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end)
        : 制作树语言序列_小写(begin, end) { }
    制作树语言向量_小写(const 制作树语言向量_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言序列_小写(that, meta) { }

    virtual 制作树语言值指针_小写 eval(制作树语言环境指针_小写 env);
    virtual 字符串_大写 print(bool readably) const;

    virtual 制作树语言值指针_小写 conj(制作树语言值迭代器_小写 argsBegin,
                             制作树语言值迭代器_小写 argsEnd) const;

    WITH_META(制作树语言向量_小写);
};

class 制作树语言可应用的_小写 : public 制作树语言值_小写 {
public:
    制作树语言可应用的_小写() { }
    制作树语言可应用的_小写(制作树语言值指针_小写 meta) : 制作树语言值_小写(meta) { }

    virtual 制作树语言值指针_小写 apply(制作树语言值迭代器_小写 argsBegin,
                               制作树语言值迭代器_小写 argsEnd) const = 0;
};

class 制作树语言哈希_小写 : public 制作树语言值_小写 {
public:
    typedef std::map<字符串_大写, 制作树语言值指针_小写> Map;

    制作树语言哈希_小写(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd, bool isEvaluated);
    制作树语言哈希_小写(const 制作树语言哈希_小写::Map& map);
    制作树语言哈希_小写(const 制作树语言哈希_小写& that, 制作树语言值指针_小写 meta)
    : 制作树语言值_小写(meta), m_map(that.m_map), m_isEvaluated(that.m_isEvaluated) { }

    制作树语言值指针_小写 assoc(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const;
    制作树语言值指针_小写 dissoc(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const;
    bool contains(制作树语言值指针_小写 key) const;
    制作树语言值指针_小写 eval(制作树语言环境指针_小写 env);
    制作树语言值指针_小写 get(制作树语言值指针_小写 key) const;
    制作树语言值指针_小写 keys() const;
    制作树语言值指针_小写 values() const;

    virtual 字符串_大写 print(bool readably) const;

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const;

    WITH_META(制作树语言哈希_小写);

private:
    const Map m_map;
    const bool m_isEvaluated;
};

class 制作树语言内建的_小写 : public 制作树语言可应用的_小写 {
public:
    typedef 制作树语言值指针_小写 (ApplyFunc)(const 字符串_大写& name,
                                    制作树语言值迭代器_小写 argsBegin,
                                    制作树语言值迭代器_小写 argsEnd);

    制作树语言内建的_小写(const 字符串_大写& name, ApplyFunc* handler)
    : m_name(name), m_handler(handler) { }

    制作树语言内建的_小写(const 制作树语言内建的_小写& that, 制作树语言值指针_小写 meta)
    : 制作树语言可应用的_小写(meta), m_name(that.m_name), m_handler(that.m_handler) { }

    virtual 制作树语言值指针_小写 apply(制作树语言值迭代器_小写 argsBegin,
                              制作树语言值迭代器_小写 argsEnd) const;

    virtual 字符串_大写 print(bool readably) const {
        return STRF("#内建函数(%s)", m_name.c_str());
    }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return this == rhs; // these are singletons
    }

    字符串_大写 name() const { return m_name; }

    WITH_META(制作树语言内建的_小写);

private:
    const 字符串_大写 m_name;
    ApplyFunc* m_handler;
};

class 制作树语言兰姆达_小写 : public 制作树语言可应用的_小写 {
public:
    制作树语言兰姆达_小写(const 字符串向量_大写短& bindings, 制作树语言值指针_小写 body, 制作树语言环境指针_小写 env);
    制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, 制作树语言值指针_小写 meta);
    制作树语言兰姆达_小写(const 制作树语言兰姆达_小写& that, bool isMacro);

    virtual 制作树语言值指针_小写 apply(制作树语言值迭代器_小写 argsBegin,
                              制作树语言值迭代器_小写 argsEnd) const;

    制作树语言值指针_小写 getBody() const { return m_body; }
    制作树语言环境指针_小写 makeEnv(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd) const;

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return this == rhs; // do we need to do a deep inspection?
    }

    virtual 字符串_大写 print(bool readably) const {
        return STRF("#用户-%s(%p)", m_isMacro ? "宏" : "函数", this);
    }

    bool isMacro() const { return m_isMacro; }

    virtual 制作树语言值指针_小写 doWithMeta(制作树语言值指针_小写 meta) const;

private:
    const 字符串向量_大写短   m_bindings;
    const 制作树语言值指针_小写 m_body;
    const 制作树语言环境指针_小写   m_env;
    const bool        m_isMacro;
};

class 制作树语言原子_小写 : public 制作树语言值_小写 {
public:
    制作树语言原子_小写(制作树语言值指针_小写 value) : m_value(value) { }
    制作树语言原子_小写(const 制作树语言原子_小写& that, 制作树语言值指针_小写 meta)
        : 制作树语言值_小写(meta), m_value(that.m_value) { }

    virtual bool doIsEqualTo(const 制作树语言值_小写* rhs) const {
        return this->m_value->isEqualTo(rhs);
    }

    virtual 字符串_大写 print(bool readably) const {
        return "(原子 " + m_value->print(readably) + ")";
    };

    制作树语言值指针_小写 deref() const { return m_value; }

    制作树语言值指针_小写 reset(制作树语言值指针_小写 value) { return m_value = value; }

    WITH_META(制作树语言原子_小写);

private:
    制作树语言值指针_小写 m_value;
};

namespace 制作树语言_小写 {
    制作树语言值指针_小写 atom(制作树语言值指针_小写 value);
    制作树语言值指针_小写 boolean(bool value);
    制作树语言值指针_小写 builtin(const 字符串_大写& name, 制作树语言内建的_小写::ApplyFunc handler);
    制作树语言值指针_小写 falseValue();
    制作树语言值指针_小写 hash(制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd,
                     bool isEvaluated);
    制作树语言值指针_小写 hash(const 制作树语言哈希_小写::Map& map);
    制作树语言值指针_小写 integer(int64_t value);
    制作树语言值指针_小写 integer(const 字符串_大写& token);
    制作树语言值指针_小写 keyword(const 字符串_大写& token);
    制作树语言值指针_小写 lambda(const 字符串向量_大写短&, 制作树语言值指针_小写, 制作树语言环境指针_小写);
    制作树语言值指针_小写 list(制作树语言值向量_小写* items);
    制作树语言值指针_小写 list(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end);
    制作树语言值指针_小写 list(制作树语言值指针_小写 a);
    制作树语言值指针_小写 list(制作树语言值指针_小写 a, 制作树语言值指针_小写 b);
    制作树语言值指针_小写 list(制作树语言值指针_小写 a, 制作树语言值指针_小写 b, 制作树语言值指针_小写 c);
    制作树语言值指针_小写 macro(const 制作树语言兰姆达_小写& lambda);
    制作树语言值指针_小写 nilValue();
    制作树语言值指针_小写 string(const 字符串_大写& token);
    制作树语言值指针_小写 symbol(const 字符串_大写& token);
    制作树语言值指针_小写 trueValue();
    制作树语言值指针_小写 vector(制作树语言值向量_小写* items);
    制作树语言值指针_小写 vector(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end);
};

#endif // INCLUDE_TYPES_H
