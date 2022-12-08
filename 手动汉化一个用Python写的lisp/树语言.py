# -*- coding:utf-8 -*-

## 用Python写的scheme解释器 ##

## 符号, 过程, 种类

from __future__ import division
import re, sys
from io import StringIO

class 符号(str): pass

def 小符号(单符,符号表={}):
    "为符号表中的str 单符查找或创建唯一的符号条目."
    if 单符 not in 符号表: 符号表[单符] = 符号(单符)
    return 符号表[单符]

_引号, _若, _设置, _定义, _兰姆达, _开头, _定义宏 = map(小符号,
"引号   若   设置!  定义   兰姆达   开头   定义宏".split())

_准引号, _反引号, _反引号拼接 = map(小符号,
"准引号   反引号   反引号拼接".split())

class 过程(object):
    "一个用户定义的Scheme过程."
    def __init__(自身,小形参,小表达式,小环境):
        自身.小形参, 自身.小表达式, 自身.小环境 = 小形参, 小表达式, 小环境
    def __call__(自身,*小实参):
        return 求值(自身.小表达式, 大环境(自身.小形参, 小实参, 自身.小环境))

## 解析, 读, 与用户交互 ##

def 解析(小入端口):
    "解析一个程序: 读与 扩展/错误检查 它"
    #向后兼容: 给定一个str,转换它到一个大入端口
    if isinstance(小入端口,str): 小入端口=大入端口(StringIO(小入端口))
    return 扩展(读(小入端口), 顶层=True)

终文件对象 = 符号('#<终文件对象>') # 笔记: 未经授权; 不能读

class 大入端口(object):
    "一个输入端口. 保留一行字符"
    化为牌 = r"""\s*(,@|[('`,)]|"(?:[\\].|[^\\"])*"|;.*|[^\s('"`,;)]*)(.*)"""
    def __init__(自身, 文件):
        自身.文件 = 文件; 自身.行 = ''
    def 下一个牌(自身):
        "返回其下一个牌, 读新文本进到行缓冲区若需要的话"
        while True:
            if 自身.行 == '': 自身.行 = 自身.文件.readline()
            if 自身.行 == '': return 终文件对象
            牌, 自身.行 = re.match(大入端口.化为牌, 自身.行).groups()
            if 牌 != '' and not 牌.startswith(';'):
                return 牌
            
def 读字符(小入端口):
    "读其下一个字符来自一个输入端口"
    if 小入端口.行 != '':
        小字符, 小入端口.行 = 小入端口.行[0], 小入端口.行[1:]
        return 小字符
    else:
        return 小入端口.文件.读(1) or 终文件对象

def 读(小入端口):
    "读一个scheme表达式来自一个输入端口"
    def 读前方(牌):
        if '(' == 牌:
            大左 = []
            while True:
                牌 = 小入端口.下一个牌()
                if 牌 == ')': return 大左
                else: 大左.append(读前方(牌))
        elif ')' == 牌: raise SyntaxError('不期望的 )')
        elif 牌 in 引号_复数: return [引号_复数[牌], 读(小入端口)]
        elif 牌 is 终文件对象: raise SyntaxError('不期望的文件终在列表内')
        else: return 原子(牌)
    # 读的身体:
    牌1 = 小入端口.下一个牌()
    return 终文件对象 if 牌1 is 终文件对象 else 读前方(牌1)

引号_复数 = {"'":_引号, "`":_准引号, ",":_反引号, ",@":_反引号拼接}

def 原子(牌):
    '数目变成数字; #t 与 #f 是布尔; "..." 字符串; 另外的 符号.'
    if 牌 == '#真': return True
    elif 牌 == '#假': return False
    elif 牌[0] == '"': return 牌[1:-1].decode('字符串跳出')
    try: return int(牌)
    except ValueError:
        try: return float(牌)
        except ValueError:
            try: return complex(牌.replace('计数竖', '计数钩', 1))
            except ValueError:
                return 小符号(牌)

def 到字符串(未知):
    "将Python对象转换回Lisp可读的字符串."
    if 未知 is True: return "#真"
    elif 未知 is False: return "#假"
    elif 是啊(未知,符号): return 未知
    elif 是啊(未知,str): return '"%s"' % 未知.encode('字符串跳出').replace('"', r'\"')
    elif 是啊(未知,list): return '('+' '.join(map(到字符串, 未知))+')'
    elif 是啊(未知,complex): return str(未知).replace('计数钩','计数竖')
    else: return str(未知)

def 载入(文件名):
    "求值每个表达式来自一个文件."
    读求值印环(None,大入端口(open(文件名)), None)

def 读求值印环(提示符='树语言> ', 小入端口=大入端口(sys.stdin),出去=sys.stdout):
    "一个提示 读取-求值-打印 循环."
    sys.stderr.write("树语言版本 2.0 (用Python写的lisp) \n")
    while True:
        try:
            if 提示符: sys.stderr.write(提示符)
            未知 = 解析(小入端口)
            if 未知 is 终文件对象: return
            小写值 = 求值(未知)
            if 小写值 is None and 出去: print >> 出去, 到字符串(小写值)
        except Exception as 例外字母:
            print ('%s: %s' % (type(例外字母).__name__, 例外字母))

## 环境类 ##

class 大环境(dict):
    "一个环境: 一个由{'值':值}点对组成的字典,和一个外部的大环境."
    def __init__(自身,形参=(),实参=(),外部=None):
        # 绑定形参列表到相应的实参, 或单个形参到实参列表
        自身.外部 = 外部
        if 是啊(形参, 符号):
            自身.update({形参:list(实参)})
        else:
            if len(实参) != len(形参):
                raise TypeError('期望 %s, 给了 %s, ' % (到字符串(形参), 到字符串(实参)))
            自身.update(zip(形参,实参))
    def 找(自身,小变量):
        "找到小变量出现的最内部的大环境"
        if 小变量 in 自身: return 自身
        elif 自身.外部 is None: raise LookupError(小变量)
        else: return 自身.外部.找(小变量)

def 是否点对(未知): return 未知 != [] and 是啊(未知, list)
def 构造(未知,未知1): return [未知]+未知1

def 调用当前继续(过程):
    "调用过程与当前继续一起; 仅跳出"
    球泡 = RuntimeWarning("抱歉, 再也不能继续这样持续了.")
    def 抛出(返回值): 球泡.返回值 = 返回值; raise 球泡
    try:
        return 过程(抛出)
    except RuntimeWarning as 小警:
        if 小警 is 球泡: return 球泡.返回值
        else: raise 小警

def 加全局(自身):
    "增加一些Scheme标准过程."
    import math, cmath, operator as op
    自身.update(vars(math))
    自身.update(vars(cmath))
    自身.update({
        '+':op.add, '-':op.sub, '*':op.mul, '/':op.truediv,'非':op.not_,
        '>':op.gt, '<':op.lt, '>=':op.ge, '<=':op.le, '=':op.eq,
        '等于?':op.eq, '相等?':op.is_, '追加':op.add,
        '长度':len, '构造':构造, '切头':lambda 未知:未知[0], '切尾':lambda 未知:未知[1:],
        '列表':lambda *未知:list(未知), '列表?':lambda 未知:是啊(未知,list),
        '空指针?':lambda 未知:未知==[], '符号?':lambda 未知:是啊(未知,符号),
        '布尔':lambda 未知:是啊(未知,bool), '点对?':是否点对,
        '端口?':lambda 未知:是啊(未知,file), '应用':lambda 过程,小左:过程(*小左),
        '求值':lambda 未知:求值(扩展(未知)), '载入':lambda 函:载入(函), '调用/当前继续':调用当前继续,
        '打开-输入-文件':open, '关闭-输入-端口':lambda 小针:小针.文件.关闭(),
        '打开-输出-文件':lambda 小文:open(小文,'w'), '关闭-输出-端口':lambda 小针: 小针.关闭(),
        '终文件-对象?':lambda 未知:未知 is 终文件对象, '读-字符':读字符,
        '读':读, '写':lambda 未知,端口=sys.stdout:端口.写(到字符串(未知)),
        '显示':lambda 未知,端口=sys.stdout:端口.写(未知 if 是啊(未知,str) else 到字符串(未知))
    })
    return 自身

是啊 = isinstance
全局环境 = 加全局(大环境())

##求值 (尾递归) ##

def 求值(未知, 环境=全局环境):
    "在环境内求值一个表达式"
    while True:
        if 是啊(未知,符号):       #变量引用
            return 环境.找(未知)[未知]
        elif not 是啊(未知,list):    #常量字面
            return 未知
        elif 未知[0] is _引号:       #(引号 表达式)
            (_, 表达式) = 未知
            return 表达式
        elif 未知[0] is _若:           #(若 测试 后果 改动)
            (_,测试, 后果, 改动) = 未知
            未知 = (后果 if 求值(测试, 环境) else 改动)
        elif 未知[0] is _设置:           #(设置! 变量 表达式)
            (_, 变量, 表达式) = 未知
            环境.找(变量)[变量] = 求值(表达式, 环境)
            return None
        elif 未知[0] is _定义:        #(定义 变量 表达式)
            (_, 变量, 表达式) = 未知
            环境[变量] = 求值(表达式, 环境)
            return None
        elif 未知[0] is _兰姆达:    #(兰姆达 (变量*) 表达式)
            (_, 变量复, 表达式) = 未知
            return 过程(变量复, 表达式, 环境)
        elif 未知[0] is _开头:       #(开头 表达式+)
            for 表达式 in 未知[1:-1]:
                求值(表达式, 环境)
            未知  = 未知[-1]
        else:                         #(过程 表达式*)
            表达式复 = [求值(表达式, 环境) for 表达式 in 未知]
            过程短 = 表达式复.pop(0)
            if 是啊(过程短, 过程):
                未知 = 过程短.表达式
                环境 = 大环境(过程短.形参, 表达式复, 过程短.环境)
            else:
                return 过程短(*表达式复)

## 扩展 ##

def 扩展(未知, 顶层=False):
    "遍历 x 树，进行优化/修复，并发出句法错误信号"
    需求(未知, 未知!=[])                    # () => 错误
    if not 是啊(未知, list):                # 常量 => 不改变
        return 未知         
    elif 未知[0] is _引号:                  # (引号 表达式)
        需求(未知, len(未知) == 2)
        return 未知
    elif 未知[0] is _若:
        if len(未知) == 3: 未知 = 未知 + [None]         #(若 t c) => (若 t c None)
        需求(未知, len(未知) == 4)
        return map(扩展, 未知)
    elif 未知[0] is _设置:
        需求(未知, len(未知) == 3)
        变量 = 未知[1]                              #(设置! 无变量 表达式) => 错误
        需求(未知, 是啊(变量, 符号), "不能 设置! 只有一个符号")
        return [_设置, 变量, 扩展(未知[2])]
    elif 未知[0] is _定义 or 未知[0] is _定义宏:
        需求(未知, len(未知) >= 3)
        _定了, 变字母, 身体 = 未知[0], 未知[1], 未知[2:]
        if 是啊(变字母, list) and 变字母:               #(定义 (f 实参) 身体)
            函字母, 实参 = 变字母[0], 变字母[1:]        # => (定义 f (兰姆达 (实参) 身体))
            return 扩展([_定了, 函字母, [_兰姆达, 实参]+身体])
        else:
            需求(未知, len(未知) == 3)                  # (定义 无变量/列表 表达式) => 错误
            需求(未知, 是啊(变字母, 符号), "能定义只有一个符号")
            表达式 = 扩展(未知[2])
            if _定了 is _定义宏:
                需求(未知, 顶层, "定义宏只允许在顶层")
                过程短 = 求值(表达式)
                需求(未知, callable(过程短), "宏必须是一个过程")
                宏_表[变字母] = 过程短                  # (定义-宏 v 过程)
                return None                             # => None; 加 v:过程到宏_表
            return [_定义, 变字母, 表达式]
    elif 未知[0] is _开头:
        if len(未知) == 1: return None                  # (开头) => None
        else: return [扩展(未知数, 顶层) for 未知数 in 未知]
    elif 未知[0] is _兰姆达:                            # (兰姆达 (未知) 表达式1 表达式2)
        需求(未知, len(未知) >= 3)                      #  => (兰姆达 (未知) (开头 表达式1 表达式2))
        变量复, 身体 = 未知[1], 未知[2:]
        需求(未知, (是啊(变量复, list) and all(是啊(变字母, 符号) for 变字母 in 变量复))
            or 是啊(变量复, 符号), "不合法兰姆达实参列表")
        表达式 = 身体[0] if len(身体) == 1 else [_开头] + 身体
        return [_兰姆达, 变量复, 扩展(表达式)]
    elif 未知[0] is _准引号:                            # `未知 => 扩展_准引号(未知)
        需求(未知, len(未知) == 2)
        return 扩展_准引号(未知[1])
    elif 是啊(未知[0], 符号) and 未知[0] in 宏_表:
        return 扩展(宏_表[未知[0]](*未知[1:]), 顶层)        # (m 实参...)
    else:                                                   #  => 宏扩展 若 m 是啊 宏
        return map(扩展, 未知)                               #  (f 实参...) => 扩展每个

def 需求(未知, 断定, 消息短 = "错误的长度"):
    "若断定为假,则发出句法错误的信号"
    if not 断定: raise SyntaxError(到字符串(未知) + ': ' + 消息短)

_追加, _构造, _让 = map(小符号, "追加 构造 让".split())

def 扩展_准引号(未知):
    """扩展 '未知 => '未知; ` , 未知 => 未知; `(,@未知 不明) => (追加 未知 不明) """
    if not 是否点对(未知):
        return [_引号, 未知]
    需求(未知, 未知[0] is not _反引号拼接, "不能拼接这儿")
    if 未知[0] is _反引号:
        需求(未知, len(未知) == 2)
        return 未知[1]
    elif 是否点对(未知[0]) and 未知[0][0] is _反引号拼接:
        需求(未知[0], len(未知[0]) == 2)
        return [_追加, 未知[0][1], 扩展_准引号(未知[1:])]
    else:
        return [_构造, 扩展_准引号(未知[0]), 扩展_准引号(未知[1:])]

def 让(*实参):
    实参 = list(实参)
    未知 = 构造(_让, 实参)
    需求(未知, len(实参) > 1)
    绑定, 身体 = 实参[0], 实参[1:]
    需求(未知, all(是啊(绑字母, list) and len(绑字母) == 2 and 是啊(绑字母[0], 符号)
                    for 绑字母 in 绑定), "不合法的绑定列表")
    变量复, 值复 = zip(*绑定)
    return [[_兰姆达, list(变量复)] + map(扩展, 身体)] + map(扩展, 值复)

宏_表 = {_让:让}     #更多宏能到这里

求值(解析("""(开启

(定义宏 与 (兰姆达 实参
    (若 (空指针? 实参) #真
        (若 (= (长度 实参) 1) (切头 实参)
            `(若 ,(切头 实参) (与 ,@(切尾 实参)) #假)))))

;; 更多宏也能到这里

)"""))

if __name__ == '__main__':
    读求值印环()