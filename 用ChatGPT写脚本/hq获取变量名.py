'''
需求: 要安装Python的三方库, 就是clang的这个库,
还要安装clang的二进制可执行程序. 并指定程序所在路径.
'''

import os
import clang.cindex


class 变量信息:
    def __init__(自身, 变量名, 文件路径, 行号):
        自身.变量名 = 变量名
        自身.文件路径 = 文件路径
        自身.行号 = 行号


class 常量信息:
    def __init__(自身, 常量名, 文件路径, 行号):
        自身.常量名 = 常量名
        自身.文件路径 = 文件路径
        自身.行号 = 行号


class AST分析器:
    def __init__(自身, 文件路径):
        自身.文件路径 = 文件路径
        自身.变量列表 = []
        自身.常量列表 = []

    def 遍历AST(自身, cursor, 文件路径):
        if cursor.kind == clang.cindex.CursorKind.VAR_DECL:
            变量名 = cursor.spelling
            if len(变量名) > 1 and not 变量名.isdigit() and not 自身.检查重复变量名(变量名):
                行号 = cursor.location.line
                变量信息对象 = 变量信息(变量名, 文件路径, 行号)
                自身.变量列表.append(变量信息对象)

        if cursor.kind == clang.cindex.CursorKind.INTEGER_LITERAL:
            常量名 = cursor.spelling
            if len(常量名) > 1 and not 自身.检查重复常量名(常量名):
                行号 = cursor.location.line
                常量信息对象 = 常量信息(常量名, 文件路径, 行号)
                自身.常量列表.append(常量信息对象)

        for child in cursor.get_children():
            自身.遍历AST(child, 文件路径)

    def 检查重复变量名(自身, 变量名):
        for 变量信息对象 in 自身.变量列表:
            if 变量信息对象.变量名 == 变量名:
                return True
        return False

    def 检查重复常量名(自身, 常量名):
        for 常量信息对象 in 自身.常量列表:
            if 常量信息对象.常量名 == 常量名:
                return True
        return False

    def 分析源代码(自身):
        index = clang.cindex.Index.create()
        translation_unit = index.parse(自身.文件路径)

        if translation_unit:
            根节点 = translation_unit.cursor
            自身.遍历AST(根节点, 自身.文件路径)

            自身.写入分析结果文件()

    def 写入分析结果文件(自身):
        with open("分析结果.c", "a", encoding='utf-8') as f:
            f.write(f"文件路径：{自身.文件路径}\n")
            f.write("变量列表：\n")
            for 变量信息对象 in 自身.变量列表:
                f.write(f'{变量信息对象.变量名} - 行号：{变量信息对象.行号}\n')

            f.write("\n常量列表：\n")
            for 常量信息对象 in 自身.常量列表:
                f.write(f'{常量信息对象.常量名} - 行号：{常量信息对象.行号}\n')

            f.write("\n\n")

        print("变量和常量提取完毕")


def 迭代遍历文件夹(文件夹路径):
    for 根目录, 子目录, 文件列表 in os.walk(文件夹路径):
        for 文件名 in 文件列表:
            if 文件名.endswith(('.c', '.h')):
                文件路径 = os.path.join(根目录, 文件名)
                分析器 = AST分析器(文件路径)
                分析器.分析源代码()


def 主函数():
    文件夹路径 = input("请输入要分析的源代码文件夹路径：")
    迭代遍历文件夹(文件夹路径)


if __name__ == '__main__':
    clang.cindex.Config.set_library_path('C:\\Program Files\\LLVM\\bin')
    主函数()