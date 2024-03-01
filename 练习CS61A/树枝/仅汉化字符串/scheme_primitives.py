"""This module implements the primitives of the Scheme language."""

import math
import operator
import sys
from scheme_reader import Pair, nil
from multiprocessing.dummy import Pool as ThreadPool

try:
    import turtle
    import tkinter
except:
    print("警告：无法导入turtle模块.", file=sys.stderr)

class SchemeError(Exception):
    """Exception indicating an error in a Scheme program."""

########################
# Primitive Operations #
########################

# A list of triples (NAME, PYTHON-FUNCTION, INTERNAL-NAME).  Added to by
# primitive and used in scheme.create_global_frame.
PRIMITIVES = []

def primitive(*names):
    """An annotation to convert a Python function into a PrimitiveProcedure."""
    def add(fn):
        for name in names:
            PRIMITIVES.append((name, fn, names[0]))
        return fn
    return add

def check_type(val, predicate, k, name):
    """Returns VAL.  Raises a SchemeError if not PREDICATE(VAL)
    using "argument K of NAME" to describe the offending value."""
    if not predicate(val):
        msg = "实参 {0} 的 {1} 有错误类型 ({2})"
        raise SchemeError(msg.format(k, name, type(val).__name__))
    return val

@primitive("布尔?")
def scheme_booleanp(x):
    return x is True or x is False

def scheme_truep(val):
    """All values in Scheme are true except False."""
    return val is not False

def scheme_falsep(val):
    """Only False is false in Scheme."""
    return val is False

@primitive("非")
def scheme_not(x):
    return not scheme_truep(x)

@primitive("相等?")
def scheme_equalp(x, y):
    if scheme_pairp(x) and scheme_pairp(y):
        return scheme_equalp(x.first, y.first) and scheme_equalp(x.second, y.second)
    elif scheme_numberp(x) and scheme_numberp(y):
        return x == y
    else:
        return type(x) == type(y) and x == y

@primitive("等号?")
def scheme_eqp(x, y):
    if scheme_numberp(x) and scheme_numberp(y):
        return x == y
    elif scheme_symbolp(x) and scheme_symbolp(y):
        return x == y
    else:
        return x is y

@primitive("点对?")
def scheme_pairp(x):
    return isinstance(x, Pair)

# Streams
@primitive("承诺?")
def scheme_promisep(x):
    return type(x).__name__ == '承诺'

@primitive("强行")
def scheme_force(x):
    check_type(x, scheme_promisep, 0, '承诺')
    return x.evaluate()

@primitive("切尾流")
def scheme_cdr_stream(x):
    check_type(x, lambda x: scheme_pairp(x) and scheme_promisep(x.second), 0, '切尾流')
    return scheme_force(x.second)

@primitive("空指针?")
def scheme_nullp(x):
    return x is nil

@primitive("列表?")
def scheme_listp(x):
    """Return whether x is a well-formed list. Assumes no cycles."""
    while x is not nil:
        if not isinstance(x, Pair):
            return False
        x = x.second
    return True

@primitive("长度")
def scheme_length(x):
    check_type(x, scheme_listp, 0, '长度')
    if x is nil:
        return 0
    return len(x)

@primitive("构造")
def scheme_cons(x, y):
    return Pair(x, y)

@primitive("切头")
def scheme_car(x):
    check_type(x, scheme_pairp, 0, '切头')
    return x.first

@primitive("切尾")
def scheme_cdr(x):
    check_type(x, scheme_pairp, 0, '切尾')
    return x.second


@primitive("列表")
def scheme_list(*vals):
    result = nil
    for e in reversed(vals):
        result = Pair(e, result)
    return result

@primitive("追加")
def scheme_append(*vals):
    if len(vals) == 0:
        return nil
    result = vals[-1]
    for i in range(len(vals)-2, -1, -1):
        v = vals[i]
        if v is not nil:
            check_type(v, scheme_pairp, i, '追加')
            r = p = Pair(v.first, result)
            v = v.second
            while scheme_pairp(v):
                p.second = Pair(v.first, result)
                p = p.second
                v = v.second
            result = r
    return result

@primitive("字符串?")
def scheme_stringp(x):
    return isinstance(x, str) and x.startswith('"')

@primitive("符号?")
def scheme_symbolp(x):
    return isinstance(x, str) and not scheme_stringp(x)

@primitive("数目?")
def scheme_numberp(x):
    return isinstance(x, (int, float)) and not scheme_booleanp(x)

@primitive("整数?")
def scheme_integerp(x):
    return scheme_numberp(x) and (isinstance(x, int) or round(x) == x)

def _check_nums(*vals):
    """Check that all arguments in VALS are numbers."""
    for i, v in enumerate(vals):
        if not scheme_numberp(v):
            msg = "操作范围 {0} ({1}) 不是数目"
            raise SchemeError(msg.format(i, v))

def _arith(fn, init, vals):
    """Perform the FN operation on the number values of VALS, with INIT as
    the value when VALS is empty. Returns the result as a Scheme value."""
    _check_nums(*vals)
    s = init
    for val in vals:
        s = fn(s, val)
    if round(s) == s:
        s = round(s)
    return s

@primitive("+")
def scheme_add(*vals):
    return _arith(operator.add, 0, vals)

@primitive("-")
def scheme_sub(val0, *vals):
    _check_nums(val0, *vals) # fixes off-by-one error
    if len(vals) == 0:
        return -val0
    return _arith(operator.sub, val0, vals)

@primitive("*")
def scheme_mul(*vals):
    return _arith(operator.mul, 1, vals)

@primitive("/")
def scheme_div(val0, *vals):
    _check_nums(val0, *vals) # fixes off-by-one error
    try:
        if len(vals) == 0:
            return 1 / val0
        return _arith(operator.truediv, val0, vals)
    except ZeroDivisionError as err:
        raise SchemeError(err)

@primitive("指数")
def scheme_expt(val0, val1):
    _check_nums(val0, val1)
    return pow(val0, val1)

@primitive("绝对值")
def scheme_abs(val0):
    return abs(val0)

@primitive("商")
def scheme_quo(val0, val1):
    _check_nums(val0, val1)
    try:
        return int(val0 / val1)
    except ZeroDivisionError as err:
        raise SchemeError(err)

@primitive("取模")
def scheme_modulo(val0, val1):
    _check_nums(val0, val1)
    try:
        return val0 % val1
    except ZeroDivisionError as err:
        raise SchemeError(err)

@primitive("取余")
def scheme_remainder(val0, val1):
    _check_nums(val0, val1)
    try:
        result = val0 % val1
    except ZeroDivisionError as err:
        raise SchemeError(err)
    while result < 0 and val0 > 0 or result > 0 and val0 < 0:
        result -= val1
    return result

@primitive("四舍五入")
def 树枝四舍五入(值0, 值1):
    return round(值0, 值1)

@primitive("地板")
def 树枝地板(值):
    return math.floor(值)

@primitive("天花板")
def 树枝天花板(值):
    return math.ceil(值)

@primitive("最大公约数")
def 树枝最大公约数(值0, 值1):
    检查数们(值0, 值1)
    while 值1 != 0:
        值0, 值1 = 值1, 值0 % 值1
    return 值0

@primitive("最小公倍数")
def 树枝最小公倍数(值0, 值1):
    检查数们(值0, 值1)
    return (值0 * 值1) // 树枝最大公约数(值0, 值1)

@primitive("交换")
def 树枝交换(值0, 值1):
    return 值1, 值0

@primitive("反余弦")
def 树枝反余弦(值):
    return math.acos(值)

@primitive("反正弦")
def 树枝反正弦(值):
    return math.asin(值)

@primitive("反正切")
def 树枝反正切(值):
    return math.atan(值)

@primitive("反双曲余弦")
def 树枝反双曲余弦(值):
    return math.acosh(值)

@primitive("反双曲正弦")
def 树枝反双曲正弦(值):
    return math.asinh(值)

@primitive("反双曲正切")
def 树枝反双曲正切(值):
    return math.atanh(值)

@primitive("反正切2")
def 树枝反正切2(值0, 值1):
    return math.atan2(值0, 值1)

@primitive("平方根")
def 树枝平方根(值):
    return math.sqrt(值)

@primitive("余弦")
def 树枝余弦(值):
    return math.cos(值)

@primitive("正弦")
def 树枝正弦(值):
    return math.sin(值)

@primitive("正切")
def 树枝正切(值):
    return math.tan(值)

@primitive("双曲余弦")
def 树枝双曲余弦(值):
    return math.cosh(值)

@primitive("双曲正弦")
def 树枝双曲正弦(值):
    return math.sinh(值)

@primitive("双曲正切")
def 树枝双曲正切(值):
    return math.tanh(值)

@primitive("对数10")
def 树枝对数10(值):
    return math.log10(值)

@primitive("对数2")
def 树枝对数2(值):
    return math.log2(值)

@primitive("对数1加")
def 树枝对数1加(值):
    return math.log1p(值)

@primitive("对数")
def 树枝对数(值):
    return math.log(值)

@primitive("弧度到角度")
def 树枝弧度到角度(值):
    return math.degrees(值)

@primitive("角度到弧度")
def 树枝角度到弧度(值):
    return math.radians(值)

@primitive("复制正负号")
def 树枝复制正负号(值0, 值1):
    return math.copysign(值0, 值1)

@primitive("截断")
def 树枝截断(值):
    return math.trunc(值)

def _numcomp(op, x, y):
    _check_nums(x, y)
    return op(x, y)

@primitive("=")
def scheme_eq(x, y):
    return _numcomp(operator.eq, x, y)

@primitive("<")
def scheme_lt(x, y):
    return _numcomp(operator.lt, x, y)

@primitive(">")
def scheme_gt(x, y):
    return _numcomp(operator.gt, x, y)

@primitive("<=")
def scheme_le(x, y):
    return _numcomp(operator.le, x, y)

@primitive(">=")
def scheme_ge(x, y):
    return _numcomp(operator.ge, x, y)
    
@primitive("!=")
def 树枝不等于(甲, 乙):
    return _numcomp(operator.ne, 甲, 乙)

@primitive("偶数?")
def scheme_evenp(x):
    _check_nums(x)
    return x % 2 == 0

@primitive("奇数?")
def scheme_oddp(x):
    _check_nums(x)
    return x % 2 == 1

@primitive("零?")
def scheme_zerop(x):
    _check_nums(x)
    return x == 0
    
@primitive("非负?")
def 树枝非负谓(值):
    _check_nums(值)
    return 值 >= 0

@primitive("非正?")
def 树枝非正谓(值):
    _check_nums(值)
    return 值 < 0

##
## Other operations
##

@primitive("原子?")
def scheme_atomp(x):
    return (scheme_booleanp(x) or scheme_numberp(x) or
            scheme_symbolp(x) or scheme_nullp(x))

@primitive("显示")
def scheme_display(val):
    if scheme_stringp(val):
        val = eval(val)
    print(str(val), end="")

@primitive("印")
def scheme_print(val):
    print(str(val))

@primitive("新行")
def scheme_newline():
    print()
    sys.stdout.flush()

@primitive("错误")
def scheme_error(msg=None):
    msg = "" if msg is None else str(msg)
    raise SchemeError(msg)

@primitive("出口")
def scheme_exit():
    raise EOFError

