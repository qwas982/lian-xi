#include "MAL.h"

#include "Environment.h"
#include "ReadLine.h"
#include "Types.h"

#include <iostream>
#include <memory>

制作树语言值指针_小写 READ(const 字符串_大写& input);
字符串_大写 PRINT(制作树语言值指针_小写 ast);
static void installFunctions(制作树语言环境指针_小写 env);
//  Installs functions, macros and constants implemented in MAL.

static void makeArgv(制作树语言环境指针_小写 env, int argc, char* argv[]);
static 字符串_大写 safeRep(const 字符串_大写& input, 制作树语言环境指针_小写 env);
static 制作树语言值指针_小写 quasiquote(制作树语言值指针_小写 obj);
static 制作树语言值指针_小写 macroExpand(制作树语言值指针_小写 obj, 制作树语言环境指针_小写 env);

static ReadLine s_readLine("~/.制作树语言-历史");

static 制作树语言环境指针_小写 replEnv(new 制作树语言环境_小写);

int main(int argc, char* argv[])
{
    字符串_大写 prompt = "用户> ";
    字符串_大写 input;
    installCore(replEnv);
    installFunctions(replEnv);
    makeArgv(replEnv, argc - 2, argv + 2);
    if (argc > 1) {
        字符串_大写 filename = escape(argv[1]);
        safeRep(STRF("(载入-文件 %s)", filename.c_str()), replEnv);
        return 0;
    }
    rep("(打印换行 (串 \"树语言 [\" *宿主-语言* \"]\"))", replEnv);
    while (s_readLine.get(prompt, input)) {
        字符串_大写 out = safeRep(input, replEnv);
        if (out.length() > 0)
            std::cout << out << "\n";
    }
    return 0;
}

static 字符串_大写 safeRep(const 字符串_大写& input, 制作树语言环境指针_小写 env)
{
    try {
        return rep(input, env);
    }
    catch (制作树语言空输入异常_小写&) {
        return 字符串_大写();
    }
    catch (制作树语言值指针_小写& mv) {
        return "错误: " + mv->print(true);
    }
    catch (字符串_大写& s) {
        return "错误: " + s;
    };
}

static void makeArgv(制作树语言环境指针_小写 env, int argc, char* argv[])
{
    制作树语言值向量_小写* args = new 制作树语言值向量_小写();
    for (int i = 0; i < argc; i++) {
        args->push_back(制作树语言_小写::string(argv[i]));
    }
    env->set("*实参值*", 制作树语言_小写::list(args));
}

字符串_大写 rep(const 字符串_大写& input, 制作树语言环境指针_小写 env)
{
    return PRINT(EVAL(READ(input), env));
}

制作树语言值指针_小写 READ(const 字符串_大写& input)
{
    return readStr(input);
}

制作树语言值指针_小写 EVAL(制作树语言值指针_小写 ast, 制作树语言环境指针_小写 env)
{
    if (!env) {
        env = replEnv;
    }
    while (1) {
        const 制作树语言列表_小写* list = DYNAMIC_CAST(制作树语言列表_小写, ast);
        if (!list || (list->count() == 0)) {
            return ast->eval(env);
        }

        ast = macroExpand(ast, env);
        list = DYNAMIC_CAST(制作树语言列表_小写, ast);
        if (!list || (list->count() == 0)) {
            return ast->eval(env);
        }

        // From here on down we are evaluating a non-empty list.
        // First handle the special forms.
        if (const 制作树语言符号_小写* symbol = DYNAMIC_CAST(制作树语言符号_小写, list->item(0))) {
            字符串_大写 特殊的 = symbol->value();
            int argCount = list->count() - 1;

            if (特殊的 == "定义!") {
                checkArgsIs("定义!", 2, argCount);
                const 制作树语言符号_小写* id = VALUE_CAST(制作树语言符号_小写, list->item(1));
                return env->set(id->value(), EVAL(list->item(2), env));
            }

            if (特殊的 == "定义宏!") {
                checkArgsIs("定义宏!", 2, argCount);

                const 制作树语言符号_小写* id = VALUE_CAST(制作树语言符号_小写, list->item(1));
                制作树语言值指针_小写 body = EVAL(list->item(2), env);
                const 制作树语言兰姆达_小写* lambda = VALUE_CAST(制作树语言兰姆达_小写, body);
                return env->set(id->value(), 制作树语言_小写::macro(*lambda));
            }

            if (特殊的 == "做") {
                checkArgsAtLeast("做", 1, argCount);

                for (int i = 1; i < argCount; i++) {
                    EVAL(list->item(i), env);
                }
                ast = list->item(argCount);
                continue; // TCO
            }

            if (特殊的 == "函*") {
                checkArgsIs("函*", 2, argCount);

                const 制作树语言序列_小写* bindings =
                    VALUE_CAST(制作树语言序列_小写, list->item(1));
                字符串向量_大写短 params;
                for (int i = 0; i < bindings->count(); i++) {
                    const 制作树语言符号_小写* sym =
                        VALUE_CAST(制作树语言符号_小写, bindings->item(i));
                    params.push_back(sym->value());
                }

                return 制作树语言_小写::lambda(params, list->item(2), env);
            }

            if (特殊的 == "若") {
                checkArgsBetween("若", 2, 3, argCount);

                bool isTrue = EVAL(list->item(1), env)->isTrue();
                if (!isTrue && (argCount == 2)) {
                    return 制作树语言_小写::nilValue();
                }
                ast = list->item(isTrue ? 2 : 3);
                continue; // TCO
            }

            if (特殊的 == "让*") {
                checkArgsIs("让*", 2, argCount);
                const 制作树语言序列_小写* bindings =
                    VALUE_CAST(制作树语言序列_小写, list->item(1));
                int count = checkArgsEven("让*", bindings->count());
                制作树语言环境指针_小写 inner(new 制作树语言环境_小写(env));
                for (int i = 0; i < count; i += 2) {
                    const 制作树语言符号_小写* var =
                        VALUE_CAST(制作树语言符号_小写, bindings->item(i));
                    inner->set(var->value(), EVAL(bindings->item(i+1), inner));
                }
                ast = list->item(2);
                env = inner;
                continue; // TCO
            }

            if (特殊的 == "宏扩展") {
                checkArgsIs("宏扩展", 1, argCount);
                return macroExpand(list->item(1), env);
            }

            if (特殊的 == "准引号扩展") {
                checkArgsIs("准引号", 1, argCount);
                return quasiquote(list->item(1));
            }

            if (特殊的 == "准引号") {
                checkArgsIs("准引号", 1, argCount);
                ast = quasiquote(list->item(1));
                continue; // TCO
            }

            if (特殊的 == "引号") {
                checkArgsIs("引号", 1, argCount);
                return list->item(1);
            }

            if (特殊的 == "尝试*") {
                制作树语言值指针_小写 tryBody = list->item(1);

                if (argCount == 1) {
                    ast = EVAL(tryBody, env);
                    continue; // TCO
                }
                checkArgsIs("尝试*", 2, argCount);
                const 制作树语言列表_小写* catchBlock = VALUE_CAST(制作树语言列表_小写, list->item(2));

                checkArgsIs("抓住*", 2, catchBlock->count() - 1);
                MAL_CHECK(VALUE_CAST(制作树语言符号_小写,
                    catchBlock->item(0))->value() == "抓住*",
                    "抓住块必须以抓住开头*");

                // We don't need excSym at this scope, but we want to check
                // that the catch block is valid always, not just in case of
                // an exception.
                const 制作树语言符号_小写* excSym =
                    VALUE_CAST(制作树语言符号_小写, catchBlock->item(1));

                制作树语言值指针_小写 excVal;

                try {
                    ast = EVAL(tryBody, env);
                }
                catch(字符串_大写& s) {
                    excVal = 制作树语言_小写::string(s);
                }
                catch (制作树语言空输入异常_小写&) {
                    // Not an error, continue as if we got nil
                    ast = 制作树语言_小写::nilValue();
                }
                catch(制作树语言值指针_小写& o) {
                    excVal = o;
                };

                if (excVal) {
                    // we got some exception
                    env = 制作树语言环境指针_小写(new 制作树语言环境_小写(env));
                    env->set(excSym->value(), excVal);
                    ast = catchBlock->item(2);
                }
                continue; // TCO
            }
        }

        // Now we're left with the case of a regular list to be evaluated.
        std::unique_ptr<制作树语言值向量_小写> items(list->evalItems(env));
        制作树语言值指针_小写 op = items->at(0);
        if (const 制作树语言兰姆达_小写* lambda = DYNAMIC_CAST(制作树语言兰姆达_小写, op)) {
            ast = lambda->getBody();
            env = lambda->makeEnv(items->begin()+1, items->end());
            continue; // TCO
        }
        else {
            return APPLY(op, items->begin()+1, items->end());
        }
    }
}

字符串_大写 PRINT(制作树语言值指针_小写 ast)
{
    return ast->print(true);
}

制作树语言值指针_小写 APPLY(制作树语言值指针_小写 op, 制作树语言值迭代器_小写 argsBegin, 制作树语言值迭代器_小写 argsEnd)
{
    const 制作树语言可应用的_小写* handler = DYNAMIC_CAST(制作树语言可应用的_小写, op);
    MAL_CHECK(handler != NULL,
              "\"%s\" 是不可应用的", op->print(true).c_str());

    return handler->apply(argsBegin, argsEnd);
}

static bool isSymbol(制作树语言值指针_小写 obj, const 字符串_大写& text)
{
    const 制作树语言符号_小写* sym = DYNAMIC_CAST(制作树语言符号_小写, obj);
    return sym && (sym->value() == text);
}

//  Return arg when ast matches ('sym, arg), else NULL.
static 制作树语言值指针_小写 starts_with(const 制作树语言值指针_小写 ast, const char* sym)
{
    const 制作树语言列表_小写* list = DYNAMIC_CAST(制作树语言列表_小写, ast);
    if (!list || list->isEmpty() || !isSymbol(list->item(0), sym))
        return NULL;
    checkArgsIs(sym, 1, list->count() - 1);
    return list->item(1);
}

static 制作树语言值指针_小写 quasiquote(制作树语言值指针_小写 obj)
{
    if (DYNAMIC_CAST(制作树语言符号_小写, obj) || DYNAMIC_CAST(制作树语言哈希_小写, obj))
        return 制作树语言_小写::list(制作树语言_小写::symbol("引号"), obj);

    const 制作树语言序列_小写* seq = DYNAMIC_CAST(制作树语言序列_小写, obj);
    if (!seq)
        return obj;

    const 制作树语言值指针_小写 unquoted = starts_with(obj, "反引号");
    if (unquoted)
        return unquoted;

    制作树语言值指针_小写 res = 制作树语言_小写::list(new 制作树语言值向量_小写(0));
    for (int i=seq->count()-1; 0<=i; i--) {
        const 制作树语言值指针_小写 elt     = seq->item(i);
        const 制作树语言值指针_小写 spl_unq = starts_with(elt, "拼接-反引号");
        if (spl_unq)
            res = 制作树语言_小写::list(制作树语言_小写::symbol("合并"), spl_unq, res);
         else
            res = 制作树语言_小写::list(制作树语言_小写::symbol("构造"), quasiquote(elt), res);
    }
    if (DYNAMIC_CAST(制作树语言向量_小写, obj))
        res = 制作树语言_小写::list(制作树语言_小写::symbol("小向量"), res);
    return res;
}

static const 制作树语言兰姆达_小写* isMacroApplication(制作树语言值指针_小写 obj, 制作树语言环境指针_小写 env)
{
    const 制作树语言列表_小写* seq = DYNAMIC_CAST(制作树语言列表_小写, obj);
    if (seq && !seq->isEmpty()) {
        if (制作树语言符号_小写* sym = DYNAMIC_CAST(制作树语言符号_小写, seq->item(0))) {
            if (制作树语言环境指针_小写 symEnv = env->find(sym->value())) {
                制作树语言值指针_小写 value = sym->eval(symEnv);
                if (制作树语言兰姆达_小写* lambda = DYNAMIC_CAST(制作树语言兰姆达_小写, value)) {
                    return lambda->isMacro() ? lambda : NULL;
                }
            }
        }
    }
    return NULL;
}

static 制作树语言值指针_小写 macroExpand(制作树语言值指针_小写 obj, 制作树语言环境指针_小写 env)
{
    while (const 制作树语言兰姆达_小写* macro = isMacroApplication(obj, env)) {
        const 制作树语言序列_小写* seq = STATIC_CAST(制作树语言序列_小写, obj);
        obj = macro->apply(seq->begin() + 1, seq->end());
    }
    return obj;
}

static const char* malFunctionTable[] = {
    "(定义宏! 条件 (函* (& xs) (若 (> (计数 xs) 0) (列表 '若 (首先 xs) (若 (> (计数 xs) 1) (第n个 xs 1) (抛出 \"奇数数字形式到条件\")) (构造 '条件 (休止符 (休止符 xs)))))))",
    "(定义! 非 (函* (条件) (若 条件 假 真)))",
    "(定义! 载入-文件 (函* (文件名) \
        (求值 (读-字符串 (串 \"(做 \" (嘶嘶声 文件名) \"\n无值)\")))))",
    "(定义! *宿主-语言* \"C++\")",
};

static void installFunctions(制作树语言环境指针_小写 env) {
    for (auto &function : malFunctionTable) {
        rep(function, env);
    }
}

// Added to keep the linker happy at step A
制作树语言值指针_小写 readline(const 字符串_大写& prompt)
{
    字符串_大写 input;
    if (s_readLine.get(prompt, input)) {
        return 制作树语言_小写::string(input);
    }
    return 制作树语言_小写::nilValue();
}

