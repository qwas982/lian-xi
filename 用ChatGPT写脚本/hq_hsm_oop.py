'''
如何通过遍历C语言代码的AST来获取函数名?

使用说明:
首先用pip安转第三方模块; pip install clang .
其次下载llvm的二进制文件安装程序; https://github.com/llvm/llvm-project/releases .
我在Windows上使用,所以我下载的 LLVM-xx.x.x-win64.exe ,
这里很重要,llvm的版本必须要与Python的版本一致,都是32位或都是64位的,否则脚本会报错.
llvm的安转路径也要确认一下,你安装到哪里,就要在脚本里改相应的路径.
'''


import clang.cindex


class 函数信息:
    def __init__(自身, 函数名, 出现位置):
        自身.函数名 = 函数名
        自身.出现位置 = 出现位置


class AST分析器:
    def __init__(自身, 文件路径):
        自身.文件路径 = 文件路径
        自身.函数列表 = []
        自身.函数计数 = 0

    def 遍历AST(自身, cursor):
        if cursor.kind == clang.cindex.CursorKind.FUNCTION_DECL:
            函数名 = cursor.spelling
            出现位置 = cursor.extent.start.offset
            #print("函数名：", 函数名)    #根据需要自行启用
            自身.函数计数 += 1

            函数信息对象 = 函数信息(函数名, 出现位置)
            自身.函数列表.append(函数信息对象)

        for child in cursor.get_children():
            自身.遍历AST(child)

    def 分析源代码(自身):
        index = clang.cindex.Index.create()
        translation_unit = index.parse(自身.文件路径)

        if translation_unit:
            根节点 = translation_unit.cursor
            自身.遍历AST(根节点)

            自身.写入分析结果文件()

    def 写入分析结果文件(自身):
        with open("分析结果.c", "w", encoding='utf-8') as f:
            f.write("{\n")
            for 函数信息对象 in 自身.函数列表:
                f.write(f'    "{函数信息对象.函数名}": "占位符字符串",\n')
            f.write("}\n")

        print("函数个数：", 自身.函数计数)
        print("获取完毕")


def 主函数():
    文件路径 = input("请输入要分析的源代码文件路径：")

    分析器 = AST分析器(文件路径)
    分析器.分析源代码()


if __name__ == '__main__':
    clang.cindex.Config.set_library_path('C:\\Program Files\\LLVM\\bin')
    主函数()