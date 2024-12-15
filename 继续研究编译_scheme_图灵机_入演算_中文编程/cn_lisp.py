from io import StringIO
import os
import re
import readline as rl
import atexit
from copy import deepcopy, copy
from typing import *
from collections import namedtuple
import operator
from functools import reduce, lru_cache
import sys

# 获取一个字符串并返回牌。这里有三种牌 "(", ")" 和字。
# 空格被忽略
def 化为牌(输入: str) -> Iterator[str]:
    i = 0
    当前 = lambda: 输入[i]
    空 = lambda: len(输入[i:]) == 0

    def 字():
        nonlocal i

        s = StringIO()
        while not (空() or 当前().isspace() or 当前() in "()"):
            s.write(当前())
            i += 1
        return s.getvalue()

    def 跳过注释():
        nonlocal i
        assert 当前() == ";"
        while 当前() != "\n":
            i += 1

    while not 空():
        if 当前() == ";":
            跳过注释()
        if 当前().isspace():
            i += 1
        elif 当前() in "()":
            yield 当前()
            i += 1
        else:
            yield 字()


# 获取一个由化为牌()返回的迭代器并
# 返回一个嵌套的字符串列表。例如
# _解析(化为牌("(foo (bar))")) 返回 ["foo", ["bar"]]
# 实参们栈是为了处理不平衡的括号
def _解析(输入迭代器, 栈=0):
    结果 = []
    for 牌 in 输入迭代器:
        if 牌 == "(":
            结果.append(_解析(输入迭代器, 栈 + 1))
        elif 牌 == ")":
            return 结果
        else:
            结果.append(牌)

    if 栈 != 0:
        raise ValueError("括号不平衡")
    return 结果[0]


# 更方便调用
def 解析(输入):
    return _解析(化为牌(输入))

# 获取一个AST（嵌套列表）并返回该AST的字符串表示
def 反解析(输入):
    if type(输入) is list:
        if not 输入: # 空列表
            return "()"
        s = StringIO()
        s.write("(")
        for 实参 in 输入[:-1]:
            s.write(反解析(实参) + " ")
        s.write(f"{反解析(输入[-1])})")
        return s.getvalue()
    elif callable(输入):
        return 输入.__name__
    elif type(输入) is closure:
        实参们 = " ".join(输入.args)
        主物自体 = 反解析(输入.body)
        return f"(lambda ({实参们}) {主物自体})"
    else:
        return str(输入)
        
# 当我们看到一个 (lambda (x) y)，我们生成一个
# closure(x, y, env)，其中 env 是当前环境的一个副本
闭包 = namedtuple("闭包", "实参们 物自体 环境")
setattr(闭包, '__repr__', 反解析)

# 当我们看到 (defmacro 名称 (实参们) 物自体)，我们构建
# 一个宏，实参们 = 实参们，物自体 = 物自体
宏 = namedtuple("宏", "实参们 物自体")

# 求值_一个表达式，在这里 inp 是一个嵌套的字符串列表，env 是一个字典
def 求值_(inp, env):
    if type(inp) is str:
        if inp.startswith(":"):
            # 这是一个关键字，原样返回
            return inp
        if inp == "env":
            return env
        try:
            # 也许这是一个整数
            return int(inp)
        except ValueError:
            pass
        try:
            # 或许是环境中的某个值
            return env[inp]
        except KeyError:
            pass
        # 最后尝试，在全局环境中查找
        try:
            return 全局环境[inp]
        except KeyError:
            pass

        print(f"未绑定的变量 {inp}")
        return None
    elif type(inp) is list:
        # 如果这是一个空列表，我们不做任何事
        if not inp:
            return None
        # 这是一个列表，所以我们取第一个实参们
        # 并一直求值_直到它不再是列表为止
        操作符, *实参们 = inp
        while type(操作符) is list:
            操作符 = 求值_(操作符, env)

        # 然后我们调用 apply，操作符是我们的函数，实参们
        # 是实参，env 是包含变量信息的环境
        return 应用_(操作符, 实参们, env)
    else:
        # 否则，我们只是原样返回这个值
        # 这适用于数字，常量如 True, False, None，以及闭包
        return inp

def 严格实参们(实参们, env):
    return (求值_(实参, env) for 实参 in 实参们)
    
# 将一个函数应用到实参们上
def 应用_(函数, 实参们, 环境):
    if 函数 == "if":
        # (if 条件 则 否)
        # 求值_条件然后
        # 根据条件选择 '则' 或 '否' 分支
        条件, 则, 否 = 实参们
        if 求值_(条件, 环境):
            return 求值_(则, 环境)
        else:
            return 求值_(否, 环境)
    elif 函数 == "lambda":
        # (lambda (实参们) 物自体)
        # 构建一个闭包
        实参们, 物自体 = 实参们
        return 闭包(实参们, 物自体, 环境)
    elif 函数 == "define":
        符号, 值 = 实参们
        环境[符号] = 求值_(值, 环境)
        return None
    elif 函数 == "define-macro":
        断言 len(实参们) == 3
        符号, 实参们, 物自体 = 实参们
        环境[符号] = 宏(实参们, 物自体)
        return None
    elif type(函数) is 闭包:
        # (<闭包> 实参们) ; <闭包> 是一个已经求值_过的 (lambda (实参们) 物自体)
        # 好的，这是一个闭包，这里是调用方式

        # 在调用之前求值_所有实参们（严格求值）
        实参们 = (求值_(实参, 环境) for 实参 in 实参们)

        # 构造一个实参们值对的列表
        实参们对 = zip(函数.实参们, 实参们)

        # 复制环境，我们假装不使用变异
        新环境 = copy(环境)
        # 更新环境为闭包的环境
        新环境.update(函数.环境)
        # 更新环境为实参们值对
        新环境.update(实参们对)
        # 使用新环境求值_物自体
        return 求值_(函数.物自体, 新环境)
    elif type(函数) is str and 函数 in 环境 and type(环境[函数]) is 宏:
        m = 环境[函数]
        断言 len(m.实参们) == len(实参们), f"期望 {len(m.实参们)} 找到 {len(实参们)}"
        实参们字典 = dict(zip(m.实参们, 实参们))

        def 替换(宏物自体, 实参们点对):
            if type(宏物自体) is str:
                if 宏物自体 in 实参们字典:
                    return 实参们字典[宏物自体]
                else:
                    return 宏物自体
            elif type(宏物自体) is list:
                return [替换(e, 实参们字典) for e in 宏物自体]
            else:
                断言 False, f"无效的物自体类型 {宏物自体}"

        物自体 = 替换(m.物自体, 实参们字典)
        return 求值_(物自体, 环境)
    elif type(函数) is str and 函数 in 环境 and type(环境[函数]) is not 宏:
        # (f 实参们) ; 但是 f 是用户定义的函数
        # 这是环境中的函数
        # 求值_实参们（严格求值），
        # 应用_函数并求值_结果
        实参们 = (求值_(实参, 环境) for 实参 in 实参们)
        return 求值_(应用_(环境[函数], 实参们, 环境), 环境)
    elif 函数 == "ignore":
        return None
    elif 函数 == "prog":
        for 实参 in 实参们[:-1]:
            求值_(实参, 环境)
        return 求值_(实参们[-1], 环境)
    elif type(函数) is str and 函数 in 全局环境:
        # (g 实参们) ; 但是 g 是全局定义的函数
        # 与上面相同，但 g 是全局的，如 print
        实参们 = (求值_(实参, 环境) for 实参 in 实参们)
        return 求值_(应用_(全局环境[函数], 实参们, 环境), 环境)
    elif callable(函数):
        # (<callable> 实参们) ; callable 出现在 (g 实参们) 上面
        # 全局函数也是 callable，所以我们需要这个分支来最终调用它们
        实参们 = (求值_(实参, 环境) for 实参 in 实参们)
        return 函数(*实参们)
    elif 函数 == "load":
        断言 len(实参们) == 1, f"load 期望 1 个实参们，收到 {len(实参们)}"
        文件 = 实参们[0]
        with open(文件) as 文件指针:
            数据 = 文件指针.read()
        return 跑(数据, 环境)
    elif 函数 == "string":
        return " ".join(实参们)
    elif 函数 == ".":
        断言 len(实参们) >= 2
        对象, *字段 = 实参们
        结果 = getattr(环境[对象], 字段[0])
        for 字段 in 字段[1:]:
            结果 = getattr(结果, 字段)
        return 结果
    elif 函数 == "define-pyfun":
        class 可调用:
            def __init__(自身, 名称, 实参们, 物自体, 环境):
                自身.实参们 = 实参们
                自身.物自体 = 物自体
                自身.环境 = 环境
                自身.名称 = 名称
                自身.__name__ = 名称
            def __call__(自身, *实参们):
                实参们 = (求值_(实参, 自身.环境) for 实参 in 实参们)
                实参们对 = zip(自身.实参们, 实参们)
                新环境 = copy(自身.环境)
                新环境.update(实参们对)
                return 求值_(自身.物自体, 新环境)

        断言 len(实参们) == 3
        名称, 实参们, 物自体 = 实参们
        环境[名称] = 可调用(名称, 实参们, 物自体, 环境)
        return None
    elif 函数 in globals() and callable(globals()[函数]):
        return globals()[函数](*实参们)
    elif 函数 in __builtins__.__dict__ and callable(__builtins__.__dict__[函数]):
        return __builtins__.__dict__[函数](*实参们)
    else:
        # 不知道该怎么办
        断言 False, f"不知道如何处理 ({函数} {实参们}) 环境键 = {list(环境.keys())}"
        
# 修 操作，有了这个就可以使用匿名函数进行递归
# 你的函数需要接受一个继续函数作为最后一个实参们，例如 (lambda (x cont) ...)
# 然后修会调用这个函数并将自身作为最后一个实参们传递，
# 这样函数就可以通过调用继续函数来进行递归

# 例如，这返回 120
# (修 (lambda (x k) (if (= x 0) 1 (* x (k (- x 1) k)))) 5)
def 修(函数, *实参):
    return 应用_(函数, [*实参, 函数], {})


def 断言_(x):
    assert x


# 我们的全局环境，这里的一切都是常量或函数
# 你可以通过添加更多的全局变量来扩展语言
全局环境 = {
    "打印": print,
    "真": True,
    "假": False,
    "空": None,
    "+": operator.add,
    "-": operator.sub,
    "*": operator.mul,
    "/": operator.floordiv,
    "%": operator.mod,
    "=": operator.eq,
    ">": operator.gt,
    "<": operator.lt,
    "<=": operator.le,
    ">=": operator.ge,
    "!=": operator.ne,
    "&": operator.and_,
    "|": operator.or_,
    "修": 修,
    "环境": print,
    "断言": 断言_,
}


# 更容易调用
def 跑(输入: str, 环境):
    return 求值_(解析(输入), 环境)


# 读取求值打印循环
def 读求值印环():
    # 使用 readline 库以获得更好的输入体验
    历史记录 = os.path.join(os.path.expanduser("~"), ".lispy_history")
    try:
        rl.read_history_file(历史记录)
    except FileNotFoundError:
        pass

    atexit.register(rl.write_history_file, 历史记录)
    提示 = "lispy"
    部分输入 = []
    环境 = {}

    def 替换最后的历史记录(输入):
        hlen = rl.get_current_history_length()
        rl.remove_history_item(hlen - 1)
        rl.add_history("\n".join(部分输入) + "\n" + 输入)

    def 重置提示():
        nonlocal 提示
        提示 = "lispy"

    while (输入 := input(f"{提示}> ")) != "退出":
        try:
            if 输入:
                if 部分输入:
                    print(跑(" ".join(部分输入) + " " + 输入), 环境)
                    替换最后的历史记录(输入)
                    重置提示()
                    部分输入 = []
                else:
                    print(跑(输入, 环境))
                    重置提示()
        except ValueError as e:
            if str(e) == "括号不平衡":
                部分输入.append(输入)
                提示 = "..."
        except Exception as e:
            import traceback

            print(f"错误 {e}")
            print(traceback.format_exc())
        except KeyboardInterrupt:
            print("键盘中断，退出")
            return


def 主程序():
    if sys.stdin.isatty():
        return 读求值印环()
    else:
        return 跑(sys.stdin.read(), {})

if __name__ == "__main__":
    主程序()