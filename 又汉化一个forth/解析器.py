"""解析"""

"""递归下降牌器"""

from 错误们 import 解析错误

class 牌器(object):
    整数 = 0
    浮点 = 1
    布尔 = 2
    字符串 = 3
    冒号 = 5
    分号 = 6
    字 = 7

    def __init__(自身, 流):
        自身.流 = 流
        自身.行号 = 1
        自身.列号 = 0
    
    def 裂开(自身, 串):
        '''裂开一个字符串入到一个元组列表(起始列号, 子字符串)'''
        使规范化 = "".join(map(lambda 字符的: " " if 字符的.isspace() else 字符的, 串))

        字符串 = False
        引号 = False
        值列表 = []
        for 列的, 印刻 in enumerate(使规范化.rstrip(), 1):
            if 印刻 == '\\':
                引号 = True
            if 印刻 == '"' and not 引号:
                if 字符串:
                    值列表[-1] = (值列表[-1][0], 值列表[-1][1] + '"')
                字符串 = not 字符串

            if 印刻.isspace() and not 字符串:
                值列表.append((列的, ""))
            else:
                if len(值列表) == 0:
                    值列表.append((列的, 印刻))
                else:
                    列的, 部件 = 值列表[-1]
                    值列表[-1] = (列的, 部件 + 印刻)
                    
            if 印刻 != '\\' and 引号:
                引号 = False
        
        return [元组的 for 元组的 in 值列表 if 元组的[1] != ""]
    
    def 解析数目(自身, 串):
        """解析整数在十进制与十六进制与浮点数"""
        起始 = 1 if 串[0] in ["-", "+"] else 0

        全部数字们 = lambda 未知: all(map(lambda 字符的: 字符的.isdigit(), 未知))
        是否十六进制  = lambda 字符的: 字符的.isdigit() or ord(字符的.lower()) in range(ord("a"), ord("f"))
        全部十六进制 = lambda 未知: all(map(是否十六进制, 未知))

        if 全部数字们(串[起始:]):
            try:
                return (牌器.整数, int(串))
            except ValueError:
                raise 解析错误("%d:%d: 无效整数 '%s'" % (自身.行号, 自身.列号, 串))
            
        if 串[起始:].startswith("0x") and 全部十六进制(串[起始+2:]):
            try:
                return (牌器.整数, int(串, base=16))
            except ValueError:
                raise 解析错误("%d:%d: 无效十六进制整数 '%s'" % (自身.行号, 自身.列号, 串))
                
        if any(map(lambda 字符的: 字符的 == ".", 串)) or any(map(lambda 字符的: 字符的 == "e", 串)):
            try:
                return (牌器.浮点, float(串))
            except ValueError:
                raise 解析错误("%d:%d: 无效浮点数 '%s'" % (自身.行号, 自身.列号, 串))
            
        raise 解析错误("%d:%d: 无效数目 '%s'" % (自身.行号, 自身.列号, 串))
    
    def 解析字符串(自身, 串):
        不转义 = {
            r"\\'": r"'",
            r"\\\\": r"\\",
            r"\\a": r"\a",
            r"\\b": r"\b",
            r"\\c": r"\c",
            r"\\f": r"\f",
            r"\\n": r"\n",
            r"\\r": r"\r",
            r"\\t": r"\t",
            r"\\v": r"\v",
            r'\\"': r'"',
        }

        if not (串[0] == '"' and 串[-1] == '"'):
            raise 解析错误("%d:%d: 无效字符串: '%s'" % (自身.行号, 自身.列号, 串))
        else:
            串 = 串[1:-1]

        出 = ""
        引号 = ""

        for 字符的, (甲, 乙) in enumerate(zip(串, 串[1:])):
            if 甲 == "\\" and 引号 == "":
                引号 = 引号 + 甲
            else:
                if 引号 != "":
                    if (引号 + 甲) not in 不转义:
                        raise 解析错误("%d:%d: 无效转义序列: '%s'" % (自身.行号, 自身.列号+字符的, 引号+甲))
                    出 = 出 + 不转义[引号 + 甲]
                else:
                    出 = 出 + 甲
                引号 = ""
        return (牌器.字符串, 出)
    
    def 解析冒号(自身, 串):
        if 串 != ":":
            raise 解析错误("%d:%d: 无效字前缀: '%s'" % (自身.行号, 自身.列号, 串))
        else:
            return (牌器.冒号, ":")
    
    def 解析分号(自身, 串):
        if 串 != ";":
            raise 解析错误("%d:%d: 无效字后缀: '%s'" % (自身.行号, 自身.列号, 串))
        else:
            return (牌器.分号, ";")
        
    def 牌类型(自身, 串):
        '''解析字符串与返回一个元组(牌器.类型, 值)'''
        甲 = 串[0] if len(串) > 0 else ""
        乙 = 串[1] if len(串) > 1 else ""

        if 甲.isdigit() or (甲 in ["+", "-"] and 乙.isdigit()):
            return 自身.解析数目(串)
        elif 甲 == '"': return 自身.解析字符串(串)
        elif 甲 == ':': return 自身.解析冒号(串)
        elif 甲 == ';': return 自身.解析分号(串)
        else:
            return 自身.解析字(串)
        
    def 解析字(自身, 串):
        return (牌器.字, 串)
    
    def 牌化(自身):
        '''破一个流上入到牌们里.
            产出元组形式: (行号, 列号, 牌器.牌).'''
        def 读行们(串):
            while True:
                行 = 串.readline()
                if 行 != "":
                    yield 行.rstrip()
                else:
                    break
        
        for 自身.行号, 行 in enumerate(读行们(自身.流), 1):
            for 自身.列号, 部件 in 自身.裂开(行):
                if 部件[0] == "#": # 代表注释
                    break
                yield (自身.行号, 自身.列号, 自身.牌类型(部件))


'''解析源码'''

import io



def 解析(源码):
    '''解析源代码,返回一个指令数组,适合优化与执行,通过机器.
    
        实参们:
            源码: 一个字符串或流包含在源代码里.'''
    if isinstance(源码, str):
        return 解析流(io.StringIO(源码))
    else:
        return 解析流(源码)
    
def 解析流(流):
    '''解析一个形如forth的语言与返回代码.'''
    代码 = []

    for (行, 列, (牌, 值)) in 牌器(流).牌化():
        if 牌 == 牌器.字符串:
            值 = '"' + 值 + '"'
        代码.append(值)

    return 代码

