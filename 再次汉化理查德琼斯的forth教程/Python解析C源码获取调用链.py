import re
import os
from collections import defaultdict

# 正则表达式匹配函数定义和函数调用
函数定义模式 = re.compile(r'\b(\w+)\s+\**(\w+)\s*\([^)]*\)\s*{')
函数调用模式 = re.compile(r'\b(\w+)\s*\([^)]*\)')

# 存储函数调用关系的图
调用图 = defaultdict(list)

# 存储函数定义及其所在文件路径
函数定义表 = {}

def 解析文件(文件路径):
    with open(文件路径, 'r') as 文件:
        内容 = 文件.read()

    # 查找函数定义
    for 匹配 in 函数定义模式.finditer(内容):
        返回类型, 函数名 = 匹配.groups()
        函数定义表[函数名] = (返回类型, 文件路径)

    # 查找函数调用
    for 匹配 in 函数调用模式.finditer(内容):
        被调用函数 = 匹配.group(1)
        if 被调用函数 in 函数定义表:
            调用者函数 = 获取当前函数(内容, 匹配.start())
            if 调用者函数:
                调用图[调用者函数].append(被调用函数)

def 获取当前函数(内容, 位置):
    # 查找当前位置所在的函数
    for 匹配 in 函数定义模式.finditer(内容):
        if 匹配.start() < 位置 < 匹配.end():
            return 匹配.group(2)
    return None

def 遍历调用图(函数, 已访问, 调用顺序, 顺序编号):
    if 函数 in 已访问:
        return
    已访问.add(函数)
    for 被调用者 in 调用图[函数]:
        遍历调用图(被调用者, 已访问, 调用顺序, 顺序编号)
    调用顺序.append((函数, 顺序编号[0], 函数定义表[函数][1]))
    顺序编号[0] += 1

def 主函数():
    目录 = r'Y:\ucc-master'  # 替换为你的源文件目录
    for 根目录, _, 文件列表 in os.walk(目录):
        for 文件名 in 文件列表:
            if 文件名.endswith('.c') or 文件名.endswith('.h'):
                文件路径 = os.path.join(根目录, 文件名)
                解析文件(文件路径)

    # 获取调用顺序
    已访问 = set()
    调用顺序 = []
    顺序编号 = [1]  # 使用列表来传递可变对象
    for 函数 in 函数定义表:
        遍历调用图(函数, 已访问, 调用顺序, 顺序编号)

    # 打印调用顺序
    print("函数调用顺序:")
    for 函数, 编号, 路径 in reversed(调用顺序):
        print(f"{编号}: {函数} (在 {路径})")

if __name__ == "__main__":
    主函数()