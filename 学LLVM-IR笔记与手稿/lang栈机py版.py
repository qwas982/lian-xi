'''
用大模型翻译了一个简单栈机来用, 
子过程调用存在问题,
可能原始的被翻译源码就有问题,
我还加了一个repl读求值印环, 可以单步调试.

;------------------
过程 加一
    推 1
    加
    返

推 5
叫 加一
印

终
;-------------------
^ 这段有问题. ^


程序:

推 5
推 3
乘
推 2
加
推 4
减
印


推 65
印字符
推 97
印字符

推 19968
印字符
推 29968
印字符
推 19969
印字符
推 23334
印字符
推 31000
印字符

印
印栈

; --------------------------------------

输出:

PS Z:\2> py.exe .\lang.py .\program.txt
13
A
a
一
甐
丁
嬦
礘
31000
[13, 65, 97, 19968, 29968, 19969, 23334, 31000]

'''

from typing import List, Dict, Tuple, Optional
from enum import Enum
import sys

# 类型别名
指针 = int
标号 = Tuple[str, 指针]
标号表 = Dict[str, 指针]
过程表 = Dict[str, Tuple[指针, 指针]]

# 栈框结构
class 栈框:
    def __init__(自身, 栈偏移: 指针, 指令指针: 指针):
        自身.栈偏移 = 栈偏移
        自身.指令指针 = 指令指针

# 栈类
class 栈:
    def __init__(自身, 最大大小: int = 8192):
        自身.数据: List[int] = []
        自身.最大大小 = 最大大小

    def 推(自身, 值: int):
        if len(自身.数据) >= 自身.最大大小:
            raise RuntimeError(f"栈溢出: 栈大小已达到最大值 {自身.最大大小}")
        自身.数据.append(值)

    def 弹(自身) -> int:
        if not 自身.数据:
            raise RuntimeError("试图从空栈中弹出元素")
        return 自身.数据.pop()

    def 窥(自身) -> int:
        if not 自身.数据:
            raise RuntimeError("试图查看空栈的栈顶")
        return 自身.数据[-1]

    def 修改栈顶(自身, 值: int):
        if not 自身.数据:
            raise RuntimeError("试图修改空栈的栈顶")
        自身.数据[-1] = 值

    def _检查索引(自身, 索引: int):
        if 索引 >= len(自身.数据):
            raise RuntimeError(f"索引 {索引} 超出栈范围 (栈大小: {len(自身.数据)})")

    def 获取(自身, 索引: int) -> int:
        自身._检查索引(索引)
        return 自身.数据[索引]

    def 设置(自身, 索引: int, 值: int):
        自身._检查索引(索引)
        自身.数据[索引] = 值

    def __repr__(自身):
        return str(自身.数据)

# 指令枚举
class 指令(Enum):
    推 = "推"
    弹 = "弹"
    加 = "加"
    减 = "减"
    乘 = "乘"
    除 = "除"
    递增 = "递增"
    递减 = "递减"
    跳 = "跳"
    等跳 = "等跳"
    不等跳 = "不等跳"
    大于跳 = "大于跳"
    小于跳 = "小于跳"
    大等跳 = "大等跳"
    小等跳 = "小等跳"
    获取 = "获取"
    设置 = "设置"
    获取参数 = "获取参数"
    设置参数 = "设置参数"
    空操作 = "空操作"
    印 = "印"
    印字符 = "印字符"
    印栈 = "印栈"
    叫 = "叫"
    返 = "返"

# 虚机类
class 虚机:
    def __init__(自身, 程序: List[Tuple[指令, Optional[int]]]):
        自身.程序 = 程序
        自身.栈 = 栈()
        自身.指令指针 = 0
        自身.叫栈: List[栈框] = []

    def _检查指令指针(自身, 指令指针: int):
        if 指令指针 < 0 or 指令指针 >= len(自身.程序):
            raise RuntimeError(f"无效的指令指针: {指令指针} (程序大小: {len(自身.程序)})")

    def 跑(自身):
        while 自身.指令指针 < len(自身.程序):
            指令, 操作数 = 自身.程序[自身.指令指针]
            自身.指令指针 += 1

            if 指令 == 指令.空操作:
                pass
            elif 指令 == 指令.推:
                自身.栈.推(操作数)
            elif 指令 == 指令.弹:
                自身.栈.弹()
            elif 指令 == 指令.加:
                if len(自身.栈.数据) < 2:
                    raise RuntimeError("加指令: 栈中元素不足")
                a = 自身.栈.弹()
                b = 自身.栈.弹()
                自身.栈.推(a + b)
            elif 指令 == 指令.减:
                if len(自身.栈.数据) < 2:
                    raise RuntimeError("减指令: 栈中元素不足")
                a = 自身.栈.弹()
                b = 自身.栈.弹()
                自身.栈.推(b - a)
            elif 指令 == 指令.乘:
                if len(自身.栈.数据) < 2:
                    raise RuntimeError("乘指令: 栈中元素不足")
                a = 自身.栈.弹()
                b = 自身.栈.弹()
                自身.栈.推(a * b)
            elif 指令 == 指令.除:
                if len(自身.栈.数据) < 2:
                    raise RuntimeError("除指令: 栈中元素不足")
                a = 自身.栈.弹()
                b = 自身.栈.弹()
                if a == 0:
                    raise RuntimeError("除零错误")
                自身.栈.推(b // a)
            elif 指令 == 指令.递增:
                if len(自身.栈.数据) < 1:
                    raise RuntimeError("递增指令: 栈中元素不足")
                自身.栈.修改栈顶(自身.栈.窥() + 1)
            elif 指令 == 指令.递减:
                if len(自身.栈.数据) < 1:
                    raise RuntimeError("递减指令: 栈中元素不足")
                自身.栈.修改栈顶(自身.栈.窥() - 1)
            elif 指令 == 指令.跳:
                自身._检查指令指针(操作数)
                自身.指令指针 = 操作数
            elif 指令 == 指令.等跳:
                if 自身.栈.窥() == 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.不等跳:
                if 自身.栈.窥() != 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.大于跳:
                if 自身.栈.窥() > 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.小于跳:
                if 自身.栈.窥() < 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.大等跳:
                if 自身.栈.窥() >= 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.小等跳:
                if 自身.栈.窥() <= 0:
                    自身.栈.弹()
                    自身._检查指令指针(操作数)
                    自身.指令指针 = 操作数
            elif 指令 == 指令.获取:
                偏移 = 自身.叫栈[-1].栈偏移 if 自身.叫栈 else 0
                自身.栈.推(自身.栈.获取(操作数 + 偏移))
            elif 指令 == 指令.设置:
                偏移 = 自身.叫栈[-1].栈偏移 if 自身.叫栈 else 0
                自身.栈.设置(操作数 + 偏移, 自身.栈.窥())
            elif 指令 == 指令.获取参数:
                if not 自身.叫栈:
                    raise RuntimeError("获取参数指令: 没有调用过程，无法获取参数")
                偏移 = 自身.叫栈[-1].栈偏移 - 操作数
                自身.栈.推(自身.栈.获取(偏移))
            elif 指令 == 指令.设置参数:
                if not 自身.叫栈:
                    raise RuntimeError("设置参数指令: 没有调用过程，无法设置参数")
                偏移 = 自身.叫栈[-1].栈偏移 - 操作数
                自身.栈.设置(偏移, 自身.栈.窥())
            elif 指令 == 指令.印:
                print(自身.栈.窥(), end="\n")
            elif 指令 == 指令.印字符:
                码点 = 自身.栈.窥()
                if 0 <= 码点 <= 0x10FFFF:  # Unicode 码点范围
                    print(chr(码点), end="\n")
                else:
                    raise RuntimeError(f"无效的 Unicode 码点: {码点}")
            elif 指令 == 指令.印栈:
                print(自身.栈, end="\n")
            elif 指令 == 指令.叫:
                自身.叫栈.append(栈框(len(自身.栈.数据), 自身.指令指针))
                print(f"调用过程: 指令指针={自身.指令指针}, 栈偏移={len(自身.栈.数据)}")
                自身.指令指针 = 操作数  # 直接跳转到操作数
            elif 指令 == 指令.返:
                if not 自身.叫栈:
                    raise RuntimeError("返指令: 调用栈为空")
                框 = 自身.叫栈.pop()
                print(f"返回过程: 指令指针={框.指令指针}, 栈偏移={框.栈偏移}")
                自身.指令指针 = 框.指令指针
                # 恢复栈帧
                自身.栈.数据 = 自身.栈.数据[:框.栈偏移]

# 辅助函数
def 解析指令(牌: List[str], 标号: 标号表, 过程: 过程表) -> Tuple[指令, Optional[int]]:
    if not 牌:
        raise RuntimeError("空指令")

    操作 = 牌[0]

    if 操作 == 指令.推.value:
        return (指令.推, int(牌[1]))
    elif 操作 == 指令.弹.value:
        return (指令.弹, None)
    elif 操作 == 指令.加.value:
        return (指令.加, None)
    elif 操作 == 指令.减.value:
        return (指令.减, None)
    elif 操作 == 指令.乘.value:
        return (指令.乘, None)
    elif 操作 == 指令.除.value:
        return (指令.除, None)
    elif 操作 == 指令.递增.value:
        return (指令.递增, None)
    elif 操作 == 指令.递减.value:
        return (指令.递减, None)
    elif 操作 in [指令.跳.value, 指令.等跳.value, 指令.不等跳.value, 指令.大于跳.value, 指令.小于跳.value, 指令.大等跳.value, 指令.小等跳.value]:
        return (指令(操作), 标号[牌[1]])
    elif 操作 == 指令.获取.value:
        return (指令.获取, int(牌[1]))
    elif 操作 == 指令.设置.value:
        return (指令.设置, int(牌[1]))
    elif 操作 == 指令.获取参数.value:
        return (指令.获取参数, int(牌[1]))
    elif 操作 == 指令.设置参数.value:
        return (指令.设置参数, int(牌[1]))
    elif 操作 == 指令.印.value:
        return (指令.印, None)
    elif 操作 == 指令.印字符.value:
        return (指令.印字符, None)
    elif 操作 == 指令.印栈.value:
        return (指令.印栈, None)
    elif 操作 == 指令.叫.value:
        return (指令.叫, 过程[牌[1]][0])  # 不需要 +1
    elif 操作 == 指令.返.value:
        return (指令.返, None)
    elif 操作 == "标号" or 操作 == "终" or 操作 == "过程":
        return (指令.空操作, None)
    else:
        raise RuntimeError(f"无效指令: {操作}")

def 查找标号(行: List[List[str]]) -> 标号表:
    标号: 标号表 = {}
    for i, 牌 in enumerate(行):
        if len(牌) == 2 and 牌[0] == "标号":
            标号[牌[1]] = i
    return 标号

def 查找过程(行: List[List[str]]) -> 过程表:
    过程: 过程表 = {}
    指令指针 = 0

    while 指令指针 < len(行):
        if len(行[指令指针]) == 2 and 行[指令指针][0] == "过程":
            过程名 = 行[指令指针][1]
            开始指令指针 = 指令指针
            while 行[指令指针] != ["终"]:
                指令指针 += 1
            过程[过程名] = (开始指令指针, 指令指针 + 1)
        else:
            指令指针 += 1
    return 过程

def 解析程序(行: List[List[str]], 标号: 标号表, 过程: 过程表) -> List[Tuple[指令, Optional[int]]]:
    程序 = []
    for 牌 in 行:
        程序.append(解析指令(牌, 标号, 过程))
    return 程序

def 主程序():
    if len(sys.argv) < 2:
        print("未提供文件参数，进入 REPL 模式。输入 '退出' 以退出。")
        repl()
    else:
        with open(sys.argv[1], "r", encoding="utf-8") as 文件:
            行 = [行内容.strip().split() for 行内容 in 文件 if 行内容.strip() and not 行内容.startswith(";")]

        标号 = 查找标号(行)
        过程 = 查找过程(行)
        程序 = 解析程序(行, 标号, 过程)

        虚机实例 = 虚机(程序)
        虚机实例.跑()

def repl():
    标号: 标号表 = {}
    过程: 过程表 = {}
    程序: List[Tuple[指令, Optional[int]]] = []
    虚机实例 = 虚机(程序)
    当前过程: Optional[str] = None
    过程体: List[List[str]] = []

    while True:
        try:
            输入 = input(">>> ").strip()
            if 输入 == "退出":
                print("退出 REPL 模式。")
                break

            # 解析单行输入
            牌 = 输入.split()
            if not 牌:
                continue

            # 处理标号定义
            if 牌[0] == "标号":
                if len(牌) != 2:
                    print("错误: 标号指令需要一个名称")
                    continue
                标号[牌[1]] = len(程序)
                continue

            # 处理过程定义
            if 牌[0] == "过程":
                if len(牌) != 2:
                    print("错误: 过程指令需要一个名称")
                    continue
                if 当前过程 is not None:
                    print("错误: 不能嵌套定义过程")
                    continue
                当前过程 = 牌[1]
                过程体 = []
                continue

            # 处理过程结束
            if 牌[0] == "终":
                if 当前过程 is None:
                    print("错误: 没有正在定义的过程")
                    continue
                # 解析过程体
                过程行 = 过程体
                过程标号 = 查找标号(过程行)
                过程程序 = 解析程序(过程行, 过程标号, {})
                过程[当前过程] = (len(程序), len(程序) + len(过程程序))
                程序.extend(过程程序)
                当前过程 = None
                过程体 = []
                continue

            # 如果正在定义过程，将指令添加到过程体
            if 当前过程 is not None:
                过程体.append(牌)
                continue

            # 解析指令并执行
            指令, 操作数 = 解析指令(牌, 标号, 过程)
            虚机实例.程序.append((指令, 操作数))
            虚机实例.跑()

        except RuntimeError as e:
            print(f"错误: {e}")
        except Exception as e:
            print(f"未知错误: {e}")

if __name__ == "__main__":
    主程序()
    
    