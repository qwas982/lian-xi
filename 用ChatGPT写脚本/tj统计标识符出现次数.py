#用Python写一个统计标识符出现次数的脚本

import re
from collections import defaultdict


class 标识符计数器:
    def __init__(自身):
        自身.标识符计数 = defaultdict(int)

    def 统计标识符(自身, 文件路径):
        with open(文件路径, 'r', encoding='utf-8') as 文件:
            源代码 = 文件.read()
            标识符列表 = re.findall(r'\b\w+\b', 源代码)

            for 标识符 in 标识符列表:
                自身.标识符计数[标识符] += 1

    def 获取计数(自身, 标识符):
        return 自身.标识符计数[标识符]

    def 获取所有计数(自身):
        return 自身.标识符计数

    def 写入文件(自身, 文件路径):
        with open(文件路径, 'w', encoding='utf-8') as 文件:
            for 标识符, 次数 in 自身.标识符计数.items():
                文件.write(f"{标识符}: {次数}\n")


def 主函数():
    源代码文件路径 = input("请输入要统计标识符出现次数的源代码文件路径：")
    结果文件路径 = input("请输入要将统计结果写入的文件路径：")

    计数器 = 标识符计数器()
    计数器.统计标识符(源代码文件路径)

    print("标识符出现次数统计：")
    for 标识符, 次数 in 计数器.获取所有计数().items():
        print(f"{标识符}: {次数}")

    计数器.写入文件(结果文件路径)
    print(f"统计结果已写入文件：{结果文件路径}")


if __name__ == '__main__':
    主函数()