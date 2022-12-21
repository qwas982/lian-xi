#include "MAL.h"
#include "Environment.h"
#include "StaticList.h"
#include "Types.h"

#include <chrono>
#include <fstream>
#include <iostream>

#define CHECK_ARGS_IS(expected) \
    checkArgsIs(name.c_str(), expected, \
                  std::distance(argsBegin, argsEnd))

#define CHECK_ARGS_BETWEEN(min, max) \
    checkArgsBetween(name.c_str(), min, max, \
                       std::distance(argsBegin, argsEnd))

#define CHECK_ARGS_AT_LEAST(expected) \
    checkArgsAtLeast(name.c_str(), expected, \
                        std::distance(argsBegin, argsEnd))

static 字符串_大写 printValues(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end,
                           const 字符串_大写& sep, bool readably);

static 静态列表<制作树语言内建的_小写*> handlers;

#define ARG(type, name) type* name = VALUE_CAST(type, *argsBegin++)

#define FUNCNAME(uniq) builtIn ## uniq
#define HRECNAME(uniq) handler ## uniq
#define BUILTIN_DEF(uniq, symbol) \
    static 制作树语言内建的_小写::ApplyFunc FUNCNAME(uniq); \
    static 静态列表<制作树语言内建的_小写*>::Node HRECNAME(uniq) \
        (handlers, new 制作树语言内建的_小写(symbol, FUNCNAME(uniq))); \
    制作树语言值指针_小写 FUNCNAME(uniq)(const 字符串_大写& name, \
        制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd)

#define BUILTIN(symbol)  BUILTIN_DEF(__LINE__, symbol)

#define BUILTIN_ISA(symbol, type) \
    BUILTIN(symbol) { \
        CHECK_ARGS_IS(1); \
        return 制作树语言_小写::boolean(DYNAMIC_CAST(type, *argsBegin)); \
    }

#define BUILTIN_IS(op, constant) \
    BUILTIN(op) { \
        CHECK_ARGS_IS(1); \
        return 制作树语言_小写::boolean(*argsBegin == 制作树语言_小写::constant()); \
    }

#define BUILTIN_INTOP(op, checkDivByZero) \
    BUILTIN(#op) { \
        CHECK_ARGS_IS(2); \
        ARG(制作树语言整数_小写, lhs); \
        ARG(制作树语言整数_小写, rhs); \
        if (checkDivByZero) { \
            MAL_CHECK(rhs->value() != 0, "被零除"); \
        } \
        return 制作树语言_小写::integer(lhs->value() op rhs->value()); \
    }

BUILTIN_ISA("原子?",        制作树语言原子_小写);
BUILTIN_ISA("关键字?",     制作树语言关键字_小写);
BUILTIN_ISA("列表?",        制作树语言列表_小写);
BUILTIN_ISA("映射?",         制作树语言哈希_小写);
BUILTIN_ISA("数字?",      制作树语言整数_小写);
BUILTIN_ISA("序列?",  制作树语言序列_小写);
BUILTIN_ISA("字符串?",      制作树语言字符串_小写);
BUILTIN_ISA("符号?",      制作树语言符号_小写);
BUILTIN_ISA("向量?",      制作树语言向量_小写);

BUILTIN_INTOP(+,            false);
BUILTIN_INTOP(/,            true);
BUILTIN_INTOP(*,            false);
BUILTIN_INTOP(%,            true);

BUILTIN_IS("真?",         trueValue);
BUILTIN_IS("假?",        falseValue);
BUILTIN_IS("无值?",          nilValue);

BUILTIN("-")
{
    int argCount = CHECK_ARGS_BETWEEN(1, 2);
    ARG(制作树语言整数_小写, lhs);
    if (argCount == 1) {
        return 制作树语言_小写::integer(- lhs->value());
    }

    ARG(制作树语言整数_小写, rhs);
    return 制作树语言_小写::integer(lhs->value() - rhs->value());
}

BUILTIN("<=")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言整数_小写, lhs);
    ARG(制作树语言整数_小写, rhs);

    return 制作树语言_小写::boolean(lhs->value() <= rhs->value());
}

BUILTIN(">=")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言整数_小写, lhs);
    ARG(制作树语言整数_小写, rhs);

    return 制作树语言_小写::boolean(lhs->value() >= rhs->value());
}

BUILTIN("<")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言整数_小写, lhs);
    ARG(制作树语言整数_小写, rhs);

    return 制作树语言_小写::boolean(lhs->value() < rhs->value());
}

BUILTIN(">")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言整数_小写, lhs);
    ARG(制作树语言整数_小写, rhs);

    return 制作树语言_小写::boolean(lhs->value() > rhs->value());
}

BUILTIN("=")
{
    CHECK_ARGS_IS(2);
    const 制作树语言值_小写* lhs = (*argsBegin++).ptr();
    const 制作树语言值_小写* rhs = (*argsBegin++).ptr();

    return 制作树语言_小写::boolean(lhs->isEqualTo(rhs));
}

BUILTIN("应用")
{
    CHECK_ARGS_AT_LEAST(2);
    制作树语言值指针_小写 op = *argsBegin++; // this gets checked in APPLY

    // Copy the first N-1 arguments in.
    制作树语言值向量_小写 args(argsBegin, argsEnd-1);

    // Then append the argument as a list.
    const 制作树语言序列_小写* lastArg = VALUE_CAST(制作树语言序列_小写, *(argsEnd-1));
    for (int i = 0; i < lastArg->count(); i++) {
        args.push_back(lastArg->item(i));
    }

    return APPLY(op, args.begin(), args.end());
}

BUILTIN("关联")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(制作树语言哈希_小写, hash);

    return hash->assoc(argsBegin, argsEnd);
}

BUILTIN("原子")
{
    CHECK_ARGS_IS(1);

    return 制作树语言_小写::atom(*argsBegin);
}

BUILTIN("合并")
{
    int count = 0;
    for (auto it = argsBegin; it != argsEnd; ++it) {
        const 制作树语言序列_小写* seq = VALUE_CAST(制作树语言序列_小写, *it);
        count += seq->count();
    }

    制作树语言值向量_小写* items = new 制作树语言值向量_小写(count);
    int offset = 0;
    for (auto it = argsBegin; it != argsEnd; ++it) {
        const 制作树语言序列_小写* seq = STATIC_CAST(制作树语言序列_小写, *it);
        std::copy(seq->begin(), seq->end(), items->begin() + offset);
        offset += seq->count();
    }

    return 制作树语言_小写::list(items);
}

BUILTIN("结合")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(制作树语言序列_小写, seq);

    return seq->conj(argsBegin, argsEnd);
}

BUILTIN("构造")
{
    CHECK_ARGS_IS(2);
    制作树语言值指针_小写 first = *argsBegin++;
    ARG(制作树语言序列_小写, rest);

    制作树语言值向量_小写* items = new 制作树语言值向量_小写(1 + rest->count());
    items->at(0) = first;
    std::copy(rest->begin(), rest->end(), items->begin() + 1);

    return 制作树语言_小写::list(items);
}

BUILTIN("包含?")
{
    CHECK_ARGS_IS(2);
    if (*argsBegin == 制作树语言_小写::nilValue()) {
        return *argsBegin;
    }
    ARG(制作树语言哈希_小写, hash);
    return 制作树语言_小写::boolean(hash->contains(*argsBegin));
}

BUILTIN("计数")
{
    CHECK_ARGS_IS(1);
    if (*argsBegin == 制作树语言_小写::nilValue()) {
        return 制作树语言_小写::integer(0);
    }

    ARG(制作树语言序列_小写, seq);
    return 制作树语言_小写::integer(seq->count());
}

BUILTIN("解引用")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言原子_小写, atom);

    return atom->deref();
}

BUILTIN("解关联")
{
    CHECK_ARGS_AT_LEAST(1);
    ARG(制作树语言哈希_小写, hash);

    return hash->dissoc(argsBegin, argsEnd);
}

BUILTIN("空的?")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言序列_小写, seq);

    return 制作树语言_小写::boolean(seq->isEmpty());
}

BUILTIN("求值")
{
    CHECK_ARGS_IS(1);
    return EVAL(*argsBegin, NULL);
}

BUILTIN("首先")
{
    CHECK_ARGS_IS(1);
    if (*argsBegin == 制作树语言_小写::nilValue()) {
        return 制作树语言_小写::nilValue();
    }
    ARG(制作树语言序列_小写, seq);
    return seq->first();
}

BUILTIN("函?")
{
    CHECK_ARGS_IS(1);
    制作树语言值指针_小写 arg = *argsBegin++;

    // Lambdas are functions, unless they're macros.
    if (const 制作树语言兰姆达_小写* lambda = DYNAMIC_CAST(制作树语言兰姆达_小写, arg)) {
        return 制作树语言_小写::boolean(!lambda->isMacro());
    }
    // Builtins are functions.
    return 制作树语言_小写::boolean(DYNAMIC_CAST(制作树语言内建的_小写, arg));
}

BUILTIN("获取")
{
    CHECK_ARGS_IS(2);
    if (*argsBegin == 制作树语言_小写::nilValue()) {
        return *argsBegin;
    }
    ARG(制作树语言哈希_小写, hash);
    return hash->get(*argsBegin);
}

BUILTIN("哈希映射")
{
    return 制作树语言_小写::hash(argsBegin, argsEnd, true);
}

BUILTIN("键")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言哈希_小写, hash);
    return hash->keys();
}

BUILTIN("关键字")
{
    CHECK_ARGS_IS(1);
    const 制作树语言值指针_小写 arg = *argsBegin++;
    if (制作树语言关键字_小写* s = DYNAMIC_CAST(制作树语言关键字_小写, arg))
      return s;
    if (const 制作树语言字符串_小写* s = DYNAMIC_CAST(制作树语言字符串_小写, arg))
      return 制作树语言_小写::keyword(":" + s->value());
    MAL_FAIL("关键字期望一个关键字或字符串");
}

BUILTIN("列表")
{
    return 制作树语言_小写::list(argsBegin, argsEnd);
}

BUILTIN("宏?")
{
    CHECK_ARGS_IS(1);

    // Macros are implemented as lambdas, with a special flag.
    const 制作树语言兰姆达_小写* lambda = DYNAMIC_CAST(制作树语言兰姆达_小写, *argsBegin);
    return 制作树语言_小写::boolean((lambda != NULL) && lambda->isMacro());
}

BUILTIN("映射")
{
    CHECK_ARGS_IS(2);
    制作树语言值指针_小写 op = *argsBegin++; // this gets checked in APPLY
    ARG(制作树语言序列_小写, source);

    const int length = source->count();
    制作树语言值向量_小写* items = new 制作树语言值向量_小写(length);
    auto it = source->begin();
    for (int i = 0; i < length; i++) {
      items->at(i) = APPLY(op, it+i, it+i+1);
    }

    return  制作树语言_小写::list(items);
}

BUILTIN("元")
{
    CHECK_ARGS_IS(1);
    制作树语言值指针_小写 obj = *argsBegin++;

    return obj->meta();
}

BUILTIN("第n个")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言序列_小写, seq);
    ARG(制作树语言整数_小写,  index);

    int i = index->value();
    MAL_CHECK(i >= 0 && i < seq->count(), "索引出范围");

    return seq->item(i);
}

BUILTIN("打印字符串")
{
    return 制作树语言_小写::string(printValues(argsBegin, argsEnd, " ", true));
}

BUILTIN("打印换行")
{
    std::cout << printValues(argsBegin, argsEnd, " ", false) << "\n";
    return 制作树语言_小写::nilValue();
}

BUILTIN("打印")
{
    std::cout << printValues(argsBegin, argsEnd, " ", true) << "\n";
    return 制作树语言_小写::nilValue();
}

BUILTIN("读字符串")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言字符串_小写, str);

    return readStr(str->value());
}

BUILTIN("读行")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言字符串_小写, str);

    return readline(str->value());
}

BUILTIN("重置!")
{
    CHECK_ARGS_IS(2);
    ARG(制作树语言原子_小写, atom);
    return atom->reset(*argsBegin);
}

BUILTIN("休止符")
{
    CHECK_ARGS_IS(1);
    if (*argsBegin == 制作树语言_小写::nilValue()) {
        return 制作树语言_小写::list(new 制作树语言值向量_小写(0));
    }
    ARG(制作树语言序列_小写, seq);
    return seq->rest();
}

BUILTIN("序列")
{
    CHECK_ARGS_IS(1);
    制作树语言值指针_小写 arg = *argsBegin++;
    if (arg == 制作树语言_小写::nilValue()) {
        return 制作树语言_小写::nilValue();
    }
    if (const 制作树语言序列_小写* seq = DYNAMIC_CAST(制作树语言序列_小写, arg)) {
        return seq->isEmpty() ? 制作树语言_小写::nilValue()
                              : 制作树语言_小写::list(seq->begin(), seq->end());
    }
    if (const 制作树语言字符串_小写* strVal = DYNAMIC_CAST(制作树语言字符串_小写, arg)) {
        const 字符串_大写 str = strVal->value();
        int length = str.length();
        if (length == 0)
            return 制作树语言_小写::nilValue();

        制作树语言值向量_小写* items = new 制作树语言值向量_小写(length);
        for (int i = 0; i < length; i++) {
            (*items)[i] = 制作树语言_小写::string(str.substr(i, 1));
        }
        return 制作树语言_小写::list(items);
    }
    MAL_FAIL("%s 不是一个字符串或序列", arg->print(true).c_str());
}


BUILTIN("嘶嘶声")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言字符串_小写, filename);

    std::ios_base::openmode openmode =
        std::ios::ate | std::ios::in | std::ios::binary;
    std::ifstream file(filename->value().c_str(), openmode);
    MAL_CHECK(!file.fail(), "不能打开 %s", filename->value().c_str());

    字符串_大写 data;
    data.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    data.append(std::istreambuf_iterator<char>(file.rdbuf()),
                std::istreambuf_iterator<char>());

    return 制作树语言_小写::string(data);
}

BUILTIN("串")
{
    return 制作树语言_小写::string(printValues(argsBegin, argsEnd, "", false));
}

BUILTIN("交换!")
{
    CHECK_ARGS_AT_LEAST(2);
    ARG(制作树语言原子_小写, atom);

    制作树语言值指针_小写 op = *argsBegin++; // this gets checked in APPLY

    制作树语言值向量_小写 args(1 + argsEnd - argsBegin);
    args[0] = atom->deref();
    std::copy(argsBegin, argsEnd, args.begin() + 1);

    制作树语言值指针_小写 value = APPLY(op, args.begin(), args.end());
    return atom->reset(value);
}

BUILTIN("符号")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言字符串_小写, token);
    return 制作树语言_小写::symbol(token->value());
}

BUILTIN("抛出")
{
    CHECK_ARGS_IS(1);
    throw *argsBegin;
}

BUILTIN("时间-毫秒")
{
    CHECK_ARGS_IS(0);

    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(
        high_resolution_clock::now().time_since_epoch()
    );

    return 制作树语言_小写::integer(ms.count());
}

BUILTIN("小值")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言哈希_小写, hash);
    return hash->values();
}

BUILTIN("小向量")
{
    CHECK_ARGS_IS(1);
    ARG(制作树语言序列_小写, s);
    return 制作树语言_小写::vector(s->begin(), s->end());
}

BUILTIN("向量")
{
    return 制作树语言_小写::vector(argsBegin, argsEnd);
}

BUILTIN("带有-元")
{
    CHECK_ARGS_IS(2);
    制作树语言值指针_小写 obj  = *argsBegin++;
    制作树语言值指针_小写 meta = *argsBegin++;
    return obj->withMeta(meta);
}

void installCore(制作树语言环境指针_小写 env) {
    for (auto it = handlers.begin(), end = handlers.end(); it != end; ++it) {
        制作树语言内建的_小写* handler = *it;
        env->set(handler->name(), handler);
    }
}

static 字符串_大写 printValues(制作树语言值迭代器_小写 begin, 制作树语言值迭代器_小写 end,
                          const 字符串_大写& sep, bool readably)
{
    字符串_大写 out;

    if (begin != end) {
        out += (*begin)->print(readably);
        ++begin;
    }

    for ( ; begin != end; ++begin) {
        out += sep;
        out += (*begin)->print(readably);
    }

    return out;
}
