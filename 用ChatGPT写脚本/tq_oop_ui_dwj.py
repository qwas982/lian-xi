import re
from tqdm import tqdm

class 标识符提取器:
    def __init__(自身):
        自身.文件路径列表 = []

    def 获取文件路径列表(自身):
        文件数 = int(input("请输入文件数量："))
        for i in range(文件数):
            文件路径 = input("请输入第{}个文件名：".format(i+1))
            自身.文件路径列表.append(文件路径)

    def 提取标识符(自身):
        提取结果 = {}
        for 文件路径 in tqdm(自身.文件路径列表, desc="提取进度", ncols=80):
            print("正在提取文件：", 文件路径)
            with open(文件路径, 'r', encoding='utf-8') as 文件:
                内容 = 文件.read()

            # 正则表达式模式匹配标识符
            模式 = r'\b(?!(?:include|stdio|h|stdlib|math|string|stdbool|ctype|stddef|stdarg|stddef|wchar|wctype)\b)\w+\b'
            标识符列表 = re.findall(模式, 内容)

            # 使用列表推导式进行去重
            唯一标识符列表 = [标识符 for 索引, 标识符 in enumerate(标识符列表) if 标识符 not in 标识符列表[:索引]]

            提取结果[文件路径] = 唯一标识符列表

        return 提取结果

    def 写入标识符(自身, 输出文件夹路径, 提取结果):
        for 文件路径, 标识符列表 in 提取结果.items():
            文件名 = 文件路径.split('/')[-1]  # 获取文件名
            输出文件路径 = 输出文件夹路径 + '/' + 文件名 + '_输出.c'
            with open(输出文件路径, 'w', encoding='utf-8') as 文件:
                文件.write("文件：{}\n".format(文件路径))
                文件.write('\n'.join(标识符列表))
                文件.write('\n\n')

# 示例用法
标识符提取工具 = 标识符提取器()
标识符提取工具.获取文件路径列表()

提取结果 = 标识符提取工具.提取标识符()

输出文件夹 = input("请输入输出文件夹名：")
标识符提取工具.写入标识符(输出文件夹, 提取结果)
print("标识符提取完成.")