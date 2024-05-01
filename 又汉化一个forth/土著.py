'''本土化指令, 翻译成机器码执行'''

'''包含对编译到原生 Python 的极其实验性支持.

请注意,实际上将您的代码作为原生 Python 字节码跑可能会导致解释器段错误!

todo:
    - 读取所有跳定位(实际上并非完全可能),转化为 bp.Label.
    
    - 在整个语言中,要求跳是绝对的吗? 若-跳 是可以的, 
      但它们都应该是常量(或者可以推断出来),所以 "读 整型 跳" 应该是不允许的.
      或者找到其它方法(对环使用结构化编程, 
      将子程序的调用/返回传递给本土化土著编译器,以便它可以推断位置, 编译能够返回这些位置).
      
      一个替代物将会做这事

        labels[lineno] = bp.Label()

      并为每行放一个标号. 那能工作, 
      但我不懂若 byteplay 模块会为我们优化未用的跳吗?(可能会的)
      
    - 它将可能援引 CPython 的优化器?'''

# 我发现这个模块[byteplay]根本不能用, 索性草草汉化下跑路.

import byteplay as bp
import 编译器 as cc
import 指令们 as cr
import sys

def 取模():
    return [(bp.BINARY_MODULO, None)]

def 加法():
    return [(bp.BINARY_ADD, None)]

def 位移与():
    return [(bp.BINARY_AND, None)]

def 乘法():
    return [(bp.BINARY_MULTIPLY, None)]

def 减法():
    return [(bp.BINARY_SUBTRACT, None)]

def 点():
    # TODO: Use current output stream
    return [
        (bp.PRINT_ITEM, None),
        (bp.PRINT_NEWLINE, None),
    ]

def 除法():
    return [(bp.BINARY_DIVIDE, None)]

def 小于():
    return [(bp.COMPARE_OP, "<")]

def 小等():
    return [(bp.COMPARE_OP, "<=")]

def 不等():
    return [(bp.COMPARE_OP, "!=")]

def 等号():
    return [(bp.COMPARE_OP, "==")]

def 大于():
    return [(bp.COMPARE_OP, ">")]

def 大等():
    return [(bp.COMPARE_OP, ">=")]

def 在():
    raise NotImplementedError("@")

def 位移异或():
    return [(bp.BINARY_XOR, None)]

def __调用函数(name, args):
    # TODO: Stuff like this can probably be optimized if we do a global pass
    # (so we don't need the ROT_TWO)
    if args == 0:
        return [
            (bp.LOAD_GLOBAL, name),
            (bp.CALL_FUNCTION, args)
        ]
    elif args == 1:
        return [
            (bp.LOAD_GLOBAL, name),
            (bp.ROT_TWO, None),
            (bp.CALL_FUNCTION, args)
        ]
    else:
        raise NotImplementedError("__调用函数 with more than 1 args")

def 绝对值_():
    return __调用函数("绝对值", 1)

def 转换布尔():
    return __调用函数("布尔", 1)

def 调用():
    # TODO: Could use JUMP_ABSOLUTE, but we'd have to calculate some offsets
    # due to input arguments.
    raise NotImplementedError("调用")

def 返回_():
    raise NotImplementedError("return")

def 落下():
    return [(bp.POP_TOP, None)]

def 副本():
    return [(bp.DUP_TOP, None)]

def 出口():
    # Returns None to Python
    return [
        (bp.LOAD_CONST, None),
        (bp.RETURN_VALUE, None),
    ]

def 假_():
    return [(bp.LOAD_CONST, False)]

def 转换浮点():
    return __调用函数("浮点", 1)

def 若语句():
    # Stack: (p)redicate (c)onsequent (a)lternative
    # Example: "真  'yes' 'no' 若" ==> 'yes'
    # Example: "假 'yes' 'no' 若" ==> 'no'
    pop = bp.Label()
    return [
        (bp.ROT_THREE, None),        # p c a -- a p c
        (bp.ROT_TWO, None),          # a p c -- a c p
        (bp.POP_JUMP_IF_FALSE, pop),
        (bp.ROT_TWO, None),          #  a c  -- c a
        (pop, None),
        (bp.POP_TOP, None),
    ]

def 转换整型():
    return __调用函数("整型", 1)

def 跳():
    # TODO: Make sure that our way of calculating jump locations work
    return [
        (bp.LOAD_CONST, 3),
        (bp.BINARY_FLOOR_DIVIDE, None),
        (bp.JUMP_ABSOLUTE, None),
    ]

def 负号():
    return [(bp.UNARY_NEGATIVE, None)]

def 无操作():
    return [(bp.NOP, None)]

def 布尔与():
    # a b -- (b and a)
    # TODO: Our other lang requires these are booleans
    # TODO: Use JUMP_IF_FALSE_OR_POP and leave either a 或 b
    label_out = bp.Label()
    return [
        (bp.POP_JUMP_IF_TRUE, label_out), # a b -- a
        (bp.POP_TOP, None),
        (bp.LOAD_CONST, False),
        (label_out, None),
    ]

def 布尔非():
    return [(bp.UNARY_NOT, None)]

def 布尔或():
    # TODO: This is wrong. Implement as shown in 布尔与
    return [(bp.BINARY_OR, None)]

def 超过():
    # a b -- a b a
    return [
        (bp.DUP_TOPX, 2),   # a b -- a b a b
        (bp.POP_TOP, None), # a b a b -- a b a
    ]

def 读():
    # TODO: Use current input stream
    return [
        (bp.LOAD_GLOBAL, "sys"),
        (bp.LOAD_ATTR, "stdin"),
        (bp.LOAD_ATTR, "readline"),
        (bp.CALL_FUNCTION, 0),
        (bp.LOAD_ATTR, "rstrip"),
        (bp.CALL_FUNCTION, 0),
    ]

def 旋转():
    # Forth:   a b c -- b c a
    # CPython: a b c -- c a b
    return [
        (bp.ROT_THREE, None), # a b c -- c a b
        (bp.ROT_THREE, None), # c a b -- b c a
    ]

def 转换串():
    return __调用函数("串", 1)

def 交换():
    return [(bp.ROT_TWO, None)]

def 真_():
    return [(bp.LOAD_CONST, True)]

def 写():
    # TODO: Use current output stream
    return [
        (bp.PRINT_ITEM, None),
    ]

def 位移或():
    return [(bp.BINARY_OR, None)]

def 位移补码():
    return [(bp.UNARY_INVERT, None)]

def 推(constant):
    return [(bp.LOAD_CONST, constant)]

def 到代码(bytecode):
    code = []

    for op in bytecode:
        if cc.is_embedded_push(op):
            code += 推(cc.get_embedded_push_value(op))
        else:
            code += opmap[op]()

    return code

def 编译(code, args=0, arglist=(), freevars=[], varargs=False,
        varkwargs=False, newlocals=True, name="", filename="", firstlineno=1,
        docstring=""):

    code = 到代码(code)
    code.append((bp.RETURN_VALUE, None))

    if args > 0:
        for n in xrange(args):
            argname = "arg%d" % n
            arglist = arglist + (argname,)
            code = [(bp.LOAD_FAST, argname)] + code

    # First of all, 推 a None value in case we run code like "'hey' .", so
    # that we always have a value to 返回.
    code = [(bp.LOAD_CONST, None)] + code

    codeobj = bp.Code(code, freevars=freevars, args=arglist, varargs=varargs,
            varkwargs=varkwargs, newlocals=newlocals, name=name,
            filename=filename, firstlineno=firstlineno, docstring=docstring)

    func = lambda: None
    func.func_code = bp.Code.到代码(codeobj)
    func.__doc__ = docstring # TODO: I thought bp.Code was supposed to do this?
    func.__name__ = name # TODO: Ditto
    return func

def 未知编译(source_code, args=0, optimize=True):
    """Parses Crianza source code and returns a 土著 Python function.

    Args:
        args: The resulting function's number of input parameters.

    Returns:
        A callable Python function.
    """
    code = 编译(解析(source_code), optimize=optimize)
    return 土著.编译(code, args=args)

def 未知求值(source, optimize=True):
    """Compiles to 土著 Python bytecode and runs program, returning the
    topmost value on the stack.

    Args:
        optimize: Whether to optimize the code after parsing it.

    Returns:
        None: If the stack is empty
        obj: If the stack contains a single value
        [obj, obj, ...]: If the stack contains many values
    """
    土著 = 未知编译(source, optimize=optimize)
    return 土著()

opmap = {
    cr.lookup("%"):      取模,
    cr.lookup("&"):      位移与,
    cr.lookup("*"):      乘法,
    cr.lookup("+"):      加法,
    cr.lookup("-"):      减法,
    cr.lookup("."):      点,
    cr.lookup("/"):      除法,
    cr.lookup("<"):      小于,
    cr.lookup("<="):     小等,
    cr.lookup("<>"):     不等,
    cr.lookup("="):      等号,
    cr.lookup(">"):      大于,
    cr.lookup(">="):     大等,
    cr.lookup("@"):      在,
    cr.lookup("^"):      位移异或,
    cr.lookup("绝对值"):    绝对值_,
    cr.lookup("与"):    布尔与,
    cr.lookup("布尔"):   转换布尔,
    cr.lookup("调用"):   调用,
    cr.lookup("落下"):   落下,
    cr.lookup("副本"):    副本,
    cr.lookup("出口"):   出口,
    cr.lookup("假"):  假_,
    cr.lookup("浮点"):  转换浮点,
    cr.lookup("若"):     若语句,
    cr.lookup("整型"):    转换整型,
    cr.lookup("跳"):    跳,
    cr.lookup("负号"): 负号,
    cr.lookup("无操作"):    无操作,
    cr.lookup("非"):    布尔非,
    cr.lookup("或"):     布尔或,
    cr.lookup("超过"):   超过,
    cr.lookup("读"):   读,
    cr.lookup("返回"): 返回_,
    cr.lookup("旋转"):    旋转,
    cr.lookup("串"):    转换串,
    cr.lookup("交换"):   交换,
    cr.lookup("真"):   真_,
    cr.lookup("写"):  写,
    cr.lookup("|"):      位移或,
    cr.lookup("~"):      位移补码,
}