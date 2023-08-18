import re

def 提取标识符(文件路径):
    with open(文件路径, 'r', encoding='utf-8') as 文件:
        内容 = 文件.read()
        
    # 正则表达式模式匹配标识符
    模式 = r'\b(?!(?:include|stdio|h|stdlib|math|string|stdbool|ctype|stddef|stdarg|stddef|wchar|wctype)\b)\w+\b'
    标识符列表 = re.findall(模式, 内容)
    
    return 标识符列表

def 写入标识符(文件路径, 标识符列表):
    with open(文件路径, 'w', encoding='utf-8') as 文件:
        文件.write('\n'.join(标识符列表))

# 示例用法
输入文件 = 'strings.c'
输出文件 = '提取_字符串.c'

标识符列表 = 提取标识符(输入文件)
写入标识符(输出文件, 标识符列表)
print("已提取.")