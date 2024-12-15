import re
import argparse

class 注释处理器:
    def 处理(self, 行):
        raise NotImplementedError("子类应实现此方法。")

class 单行注释处理器(注释处理器):
    def __init__(self, 替换内容):
        self.替换内容 = 替换内容

    def 处理(self, 行):
        return re.sub(r'//.*', self.替换内容, 行)

class 多行注释处理器(注释处理器):
    def __init__(self, 替换内容):
        self.替换内容 = 替换内容
        self.在多行注释中 = False

    def 处理(self, 行):
        if '/*' in 行:
            self.在多行注释中 = True
            行 = re.sub(r'/\*.*', self.替换内容, 行)
        elif '*/' in 行:
            self.在多行注释中 = False
            行 = re.sub(r'.*\*/', self.替换内容, 行)
        elif self.在多行注释中:
            行 = self.替换内容
        return 行

class 注释处理器工厂:
    @staticmethod
    def 获取处理器(注释类型, 替换内容):
        if 注释类型 == '单行':
            return 单行注释处理器(替换内容)
        elif 注释类型 == '多行':
            return 多行注释处理器(替换内容)
        else:
            raise ValueError("未知的注释类型")

class C文件处理器:
    def __init__(self, 文件路径, 替换内容):
        self.文件路径 = 文件路径
        self.替换内容 = 替换内容

    def 处理文件(self):
        with open(self.文件路径, 'r', encoding='utf-8') as 文件:
            行列表 = 文件.readlines()

        单行注释处理器 = 注释处理器工厂.获取处理器('单行', self.替换内容)
        多行注释处理器 = 注释处理器工厂.获取处理器('多行', self.替换内容)

        处理后的行列表 = []
        for 行 in 行列表:
            行 = 单行注释处理器.处理(行)
            行 = 多行注释处理器.处理(行)
            处理后的行列表.append(行)

        with open(self.文件路径, 'w', encoding='utf-8') as 文件:
            文件.writelines(处理后的行列表)

def 主函数():
    解析器 = argparse.ArgumentParser(description="处理C文件注释。")
    解析器.add_argument('文件路径', type=str, help="要处理的C文件路径。")
    解析器.add_argument('--替换内容', type=str, default='', help="替换注释的内容。")
    参数 = 解析器.parse_args()

    处理器 = C文件处理器(参数.文件路径, 参数.替换内容)
    处理器.处理文件()

if __name__ == "__main__":
    主函数()
