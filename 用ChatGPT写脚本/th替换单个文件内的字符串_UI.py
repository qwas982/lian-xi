import re
import os

class 文件替换器:
    def __init__(self, 文件路径, 替换规则):
        self.文件路径 = 文件路径
        self.替换规则 = 替换规则

    def 替换字符串(self):
        目标扩展名 = ['.c', '.h']  # 指定要替换的文件扩展名列表，可根据需要修改

        if os.path.isfile(self.文件路径) and os.path.splitext(self.文件路径)[1] in 目标扩展名:
            with open(self.文件路径, 'r', encoding='utf-8') as 文件:
                内容 = 文件.read()

            for 模式, 替换字符串 in self.替换规则.items():
                内容 = re.sub(r'\b' + 模式 + r'\b', 替换字符串, 内容)

            with open(self.文件路径, 'w', encoding='utf-8') as 文件:
                文件.write(内容)

            print(f"已替换文件：{self.文件路径}")
            print("替换完成！")
        else:
            print("指定的文件不存在或不是目标文件扩展名！")

# 示例用法
文件路径 = input("请输入文件路径：")
替换规则 = {
    "l_strton": "月虚拟机_l_串到n_函",
    "forlimit": "月虚拟机_为限制_函",
    "forprep": "月虚拟机_为预备_函",
    "floatforloop": "月虚拟机_浮点为环_函",
    "l_strcmp": "月虚拟机_l_串对比_函",
    "LTintfloat": "月虚拟机_整型小于浮点_函",
    "LEintfloat": "月虚拟机_整型小等浮点_函",
    "LTfloatint": "月虚拟机_浮点小于整型_函",
    "LEfloatint": "月虚拟机_浮点小等整型_函",
    "LTnum": "月虚拟机_数目小于_函",
    "LEnum": "月虚拟机_数目小等_函",
    "lessthanothers": "月虚拟机_小于其他_函",
    "lessequalothers": "月虚拟机_小等其他_函",
    "copy2buff": "月虚拟机_复制到缓冲_函",
    "pushclosure": "月虚拟机_推闭包_函",
}

替换器 = 文件替换器(文件路径, 替换规则)
替换器.替换字符串()