'''
你好,请帮我写一个Python脚本,使用oop,从文本文件内分析列表,将代表名称的字符串存储到一个Python字典的key里,value用空的填充,
经过实践,发现这个脚本带有自动去重的功能, 利用了Python字典键的唯一性特性.
'''

import re
import json

class 列表分析器:
    def __init__(自身, 文件路径):
        自身.文件路径 = 文件路径
        自身.数据 = {}

    def 分析文件(自身):
        模式 = r'^(\w+)\s*-\s*行号：\d+$'

        with open(自身.文件路径, 'r', encoding='utf-8') as 文件:
            行列表 = 文件.readlines()

        for 行 in 行列表:
            匹配结果 = re.match(模式, 行.strip())
            if 匹配结果:
                名称 = 匹配结果.group(1)
                自身.数据[名称] = ''

    def 写入文件(自身, 输出文件):
        with open(输出文件, 'w', encoding='utf-8') as 文件:
            json.dump(自身.数据, 文件, ensure_ascii=False, indent=4)


if __name__ == '__main__':
    文件路径 = input("请输入文件路径：")
    输出文件 = input("请输入输出文件路径：")

    分析器 = 列表分析器(文件路径)
    分析器.分析文件()
    分析器.写入文件(输出文件)