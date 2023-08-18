import re

class 标识符提取器:
    def __init__(self, 文件路径):
        self.文件路径 = 文件路径

    def 提取标识符(self):
        with open(self.文件路径, 'r', encoding='utf-8') as 文件:
            内容 = 文件.read()

        # 正则表达式模式匹配标识符
        模式 = r'\b(?!(?:include|stdio|h|stdlib|math|string|stdbool|ctype|stddef|stdarg|stddef|wchar|wctype)\b)\w+\b'
        标识符列表 = re.findall(模式, 内容)

        # 使用字典进行去重
        去重后的标识符列表 = list(dict.fromkeys(标识符列表))

        return 去重后的标识符列表

    def 写入标识符(self, 输出文件路径, 标识符列表):
        with open(输出文件路径, 'w', encoding='utf-8') as 文件:
            文件.write('\n'.join(标识符列表))

# 示例用法
输入文件 = 'strings.c'
输出文件 = '提取_字符串_oop.c'

标识符提取器 = 标识符提取器(输入文件)
标识符列表 = 标识符提取器.提取标识符()
标识符提取器.写入标识符(输出文件, 标识符列表)
print("已提取.")