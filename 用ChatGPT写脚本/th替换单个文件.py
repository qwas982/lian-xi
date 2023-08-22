import re

def 替换C文件中的标识符(input_file, output_file, 替换规则):
    with open(input_file, 'r', encoding='utf-8') as 文件:
        内容 = 文件.read()

    for 模式, 替换字符串 in 替换规则.items():
        内容 = re.sub(r'\b' + 模式 + r'\b', 替换字符串, 内容)

    with open(output_file, 'w', encoding='utf-8') as 文件:
        文件.write(内容)

    print("替换完成！")

# 示例用法
输入文件 = "strings.c"
输出文件 = "字符串.c"
替换规则 = {
    "data": "数据",
    "arr": "数组"
}

替换C文件中的标识符(输入文件, 输出文件, 替换规则)
