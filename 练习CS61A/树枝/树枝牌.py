
#scheme牌.py   字符串 -> 化为牌(牌器) -> 牌
from 树枝ucb import main
import itertools
import string
import tokenize
import sys
#import re

def 找万国码中文汉字区间():
    汉字集合 = set()
    for 游标 in set(range(0x4e00, 0x9fff)):
        每个汉字 = chr(游标)
        汉字集合.add(每个汉字)
    return 汉字集合

中文汉字字符全局 = 找万国码中文汉字区间()
数字开始全局 = set(string.digits) | set('+-.')
#匹配万国码中文汉字区间 = re.compile(r'[\u4e00-\u9fff]')
#中文汉字字符 = set(filter(匹配万国码中文汉字区间.match, map(chr, range(0x4e00, 0x9fff+1))))
符号字符全局 = (set('!$%&*/:<=>?@^_~') | set(string.ascii_lowercase) | set(string.ascii_uppercase) | 数字开始全局 | 中文汉字字符全局)
字符串分隔符全局 = set('"')
空白符号全局 = set(' \t\n\r')
括号和单体符号全局 = set("()[]'`")
终结符牌全局 = 空白符号全局 | 括号和单体符号全局 | 字符串分隔符全局 | {',', ',@'}
分隔符全局 = 括号和单体符号全局 | {'.', ',', ',@'}

def 有效符号(符):
    if len(符) == 0:
        return False
    for 印刻 in 符:
        if 印刻 not in 符号字符全局:
            return False
    return True

def 下一个候选牌(行,牌):
    while 牌 < len(行):
        印刻 = 行[牌]
        if 印刻 == ';':
            return None, len(行)
        elif 印刻 in 空白符号全局:
            牌 = 牌 + 1
        elif 印刻 in 括号和单体符号全局:
            if 印刻 == ']':
                印刻 = ')'
            if 印刻 == '[':
                印刻 = '('
            return 印刻, 牌 + 1
        elif 印刻 == '#':
            return 行[牌:牌+2], min(牌+2, len(行))
        elif 印刻 == ',':
            if 牌+1 < len(行) and 行[牌+1] == '@':
                return ',@', 牌 + 2
            return 印刻, 牌 + 1
        elif 印刻 in 字符串分隔符全局:
            if 牌+1 < len(行) and 行[牌+1] == 印刻:
                return 印刻 + 印刻, 牌 + 2
            行字节 = (bytes(行[牌+1], 编码 = 'utf-8'),)
            生成 = tokenize.tokenize(iter(行字节).__next__)
            next(生成)
            牌符 = next(生成)
            if 牌符.type != tokenize.STRING:
                raise ValueError("无效串: {0}".format(牌符.string))
            return 牌符.string, 牌符.end[1] + 牌
        else:
            计数 = 牌
            while 计数 < len(行) and 行[计数] not in 终结符牌全局:
                计数 = 计数 + 1
            return 行[牌:计数], min(计数, len(行))
    return None, len(行)

def 牌化的行(行):
    结果 = []
    文本, 计数 = 下一个候选牌(行, 0)
    while 文本 is not None:
        if 文本 in 分隔符全局:
            结果.append(文本)
        elif 文本 == '#真' or 文本.lower() == '真':
            结果.append(True)
        elif 文本 == '#假' or 文本.lower() == '假':
            结果.append(False)
        elif 文本 == '无':
            结果.append(文本)
        elif 文本[0] in 符号字符全局:
            数目 = False
            if 文本[0] in 数字开始全局:
                try:
                    结果.append(int(文本))
                    数目 = True
                except ValueError:
                    try:
                        结果.append(float(文本))
                        数目 = True
                    except ValueError:
                        pass
            if not 数目:
                if 有效符号(文本):
                    结果.append(文本.lower())
                else:
                    raise ValueError("无效数码或符号: {0}".format(文本))
        elif 文本[0] in 字符串分隔符全局:
            结果.append(文本)
        else:
            print("警告: 无效牌: {0}".format(文本), file=sys.stderr)
            print("    ", 行, file=sys.stderr)
            print(" " * (计数+3), "^", file=sys.stderr)
        文本, 计数 = 下一个候选牌(行, 计数)
    return 结果

def 牌化的行们(输入):
    return map(牌化的行, 输入)

def 计数牌们(输入):
    return len(list(filter(lambda 甲: 甲 not in 分隔符全局, itertools.chain(*牌化的行们(输入)))))


@main
def 跑词法(*实参们):
    import argparse
    解析器 = argparse.ArgumentParser(description='计数scheme牌们.')
    解析器.add_argument('文件', nargs='?', type=argparse.FileType('r'), default=sys.stdin, help='输入文件将被计数')
    实参们 = 解析器.parse_args()
    print('已计数', 计数牌们(实参们.文件), '非分隔符牌')
