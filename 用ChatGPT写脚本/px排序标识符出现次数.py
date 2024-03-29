#用Python写一个排序算法


class 键值项:
    def __init__(自身, 键, 值):
        自身.键 = 键
        自身.值 = 值

    def __str__(自身):
        return f"{自身.键}: {自身.值}"


class 快速排序:
    def __init__(自身, 项列表):
        自身.项列表 = 项列表

    def 排序(自身):
        自身._快速排序(0, len(自身.项列表) - 1)
        return 自身.项列表

    def _快速排序(自身, 开始, 结束):
        if 开始 < 结束:
            中轴 = 自身._划分(开始, 结束)
            自身._快速排序(开始, 中轴 - 1)
            自身._快速排序(中轴 + 1, 结束)

    def _划分(自身, 开始, 结束):
        中轴 = 自身.项列表[开始]
        左指针 = 开始 + 1
        右指针 = 结束

        完成 = False
        while not 完成:
            while 左指针 <= 右指针 and 自身.项列表[左指针].值 <= 中轴.值:
                左指针 += 1
            while 左指针 <= 右指针 and 自身.项列表[右指针].值 >= 中轴.值:
                右指针 -= 1

            if 左指针 > 右指针:
                完成 = True
            else:
                自身.项列表[左指针], 自身.项列表[右指针] = 自身.项列表[右指针], 自身.项列表[左指针]

        自身.项列表[开始], 自身.项列表[右指针] = 自身.项列表[右指针], 自身.项列表[开始]
        return 右指针

    @staticmethod
    def 是整数(值):
        try:
            int(值)
            return True
        except ValueError:
            return False


def 从文件读取项列表(文件路径):
    项列表 = []
    with open(文件路径, 'r', encoding='utf-8') as 文件:
        行列表 = 文件.readlines()
        for 行 in 行列表:
            部分列表 = 行.split(':')
            if len(部分列表) == 2:
                键 = 部分列表[0].strip()
                值 = 部分列表[1].strip()
                项 = 键值项(键, 值)
                项列表.append(项)
    return 项列表


def 将项列表写入文件(文件路径, 项列表):
    with open(文件路径, 'w', encoding='utf-8') as 文件:
        for i, 项 in enumerate(项列表):
            行 = f"行{i + 1}: {项}\n"
            文件.write(行)


# 示例用法
文件名 = input("请输入要排序的文件名：")
项列表 = 从文件读取项列表(文件名)

排序器 = 快速排序(项列表)
排序后项列表 = 排序器.排序()

# 根据值的大小列出排序后的结果
按值排序后项列表 = sorted(排序后项列表, key=lambda x: int(x.值))
for i, 项 in enumerate(按值排序后项列表):
    print(f"行{i + 1}: {项}")

# 保持原文件中的格式
将项列表写入文件(文件名, 按值排序后项列表)