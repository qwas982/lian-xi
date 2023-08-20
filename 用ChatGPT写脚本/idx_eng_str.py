'''请你写一个Python脚本,从一个列表里只读取英文字符串,不读取中文字符串.
使用oop来写.'''

import re

class 英文字符串读取器:
    def __init__(自身, 输入文件, 输出文件):
        自身.输入文件 = 输入文件
        自身.输出文件 = 输出文件

    def 读取英文字符串(自身):
        英文字符串 = []
        with open(自身.输入文件, 'r', encoding='utf-8') as 文件:
            for 行 in 文件:
                行 = 行.strip()
                print("读取行:", 行)
                if 自身.是英文字符串吗(行):
                    英文字符串.append(行)
                    print("是英文字符串")
                else:
                    print("不是英文字符串")
        return 英文字符串

    def 保存英文字符串(自身, 英文字符串):
        with open(自身.输出文件, 'w', encoding='utf-8') as 文件:
            for 字符串 in 英文字符串:
                print("要保存的字符串:", 字符串)
                文件.write(字符串 + '\n')
                print("写入文件:", 字符串)

    @staticmethod
    def 是英文字符串吗(字符串):
        return bool(re.match(r'^[a-zA-Z0-9_]+$', 字符串))


# 示例用法
输入文件 = input("请输入输入文件名：")
输出文件 = input("请输入输出文件名：")
读取器 = 英文字符串读取器(输入文件, 输出文件)
英文字符串 = 读取器.读取英文字符串()
读取器.保存英文字符串(英文字符串)
print('英文字符串已保存到输出文件中。')