import re

class 标识符提取器:
    def __init__(自身):
        自身.文件路径 = None

    def 获取文件路径(自身):
        自身.文件路径 = input("请输入文件名：")

    def 提取标识符(自身):
        print("正在提取标识符...")
        with open(自身.文件路径, 'r', encoding='utf-8') as 文件:
            内容 = 文件.read()
            
        # 移除注释
        内容 = re.sub(r'//.*|/\*[\s\S]*?\*/', '', 内容)

        # 正则表达式模式匹配标识符
        模式 = r'\b(?!(?:include|stdio|h|stdlib|math|string|stdbool|ctype|stddef|stdarg|stddef|wchar|wctype)\b)\w+\b'
        标识符列表 = re.findall(模式, 内容)
        
        # 排除C语言关键字
        关键字列表 = [
            'auto', 'double', 'int', 'struct', 'break', 'else', 'long', 'switch', 'case', 'enum',
            'register', 'typedef', 'char', 'extern', 'return', 'union', 'const', 'float', 'short',
            'unsigned', 'continue', 'for', 'signed', 'void', 'default', 'goto', 'sizeof', 'volatile',
            'do', 'if', 'static', 'while'
        ]
        唯一标识符列表 = [标识符 for 标识符 in 标识符列表 if 标识符 not in 关键字列表]

        # 使用列表推导式进行去重
        唯一标识符列表 = [标识符 for 索引, 标识符 in enumerate(标识符列表) if 标识符 not in 标识符列表[:索引]]
        
        # 排除单个字母的字符
        唯一标识符列表 = [标识符 for 标识符 in 唯一标识符列表 if len(标识符) > 1]

        return 唯一标识符列表

    def 写入标识符(自身, 输出文件路径, 标识符列表):
        with open(输出文件路径, 'w', encoding='utf-8') as 文件:
            文件.write('\n'.join(标识符列表))

# 示例用法
标识符提取工具 = 标识符提取器()
标识符提取工具.获取文件路径()

标识符列表 = 标识符提取工具.提取标识符()

输出文件 = input("请输入输出文件名：")
标识符提取工具.写入标识符(输出文件, 标识符列表)
print("标识符提取完成.")
