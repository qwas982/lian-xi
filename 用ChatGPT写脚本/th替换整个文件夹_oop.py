'''
使用方法说明:
将这个脚本程序放到欲替换的文件夹同级目录(注意; 不是文件夹内)
然后将需要替换的.c .h文件放到那个文件夹下.

然后到这个脚本内,到-替换规则-变量所存储的字典内,按示例添加你欲替换的字符串(标识符)
之后到此脚本所在目录启动终端,执行 Python.exe th_oop.py 即可批量替换多个文件中的字符串.
'''

import re
import os

class 文件替换器:
    def __init__(self, 文件夹路径, 替换规则):
        self.文件夹路径 = 文件夹路径
        self.替换规则 = 替换规则

    def 替换字符串(self):
        目标扩展名 = ['.c', '.h']  # 指定要替换的扩展名列表，可根据需要修改

        for 文件名 in os.listdir(self.文件夹路径):
            文件路径 = os.path.join(self.文件夹路径, 文件名)

            if os.path.isfile(文件路径) and os.path.splitext(文件路径)[1] in 目标扩展名:
                with open(文件路径, 'r', encoding='utf-8') as 文件:
                    内容 = 文件.read()

                for 模式, 替换字符串 in self.替换规则.items():
                    内容 = re.sub(r'\b' + 模式 + r'\b', 替换字符串, 内容)

                with open(文件路径, 'w', encoding='utf-8') as 文件:
                    文件.write(内容)

                print(f"已替换文件：{文件路径}")

        print("替换完成！")

# 示例用法
文件夹路径 = "1"
替换规则 = {
    "TY_VOID": "类型_空的",
}

替换器 = 文件替换器(文件夹路径, 替换规则)
替换器.替换字符串()
