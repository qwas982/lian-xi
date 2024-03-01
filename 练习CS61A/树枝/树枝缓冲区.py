
#缓冲区.py部分
import math

class 缓冲区:
    def __init__(自身, 源码):
        自身.索引 = 0
        自身.行们 = []
        自身.源码 = 源码
        自身.当前行 = ()
        自身.当前()

    def 移除前端(自身):
        当前 = 自身.当前()
        自身.索引 = 自身.索引 + 1
        return 当前
    
    def 当前(自身):
        while not 自身.更多在行上:
            自身.索引 = 0
            try:
                自身.当前行 = next(自身.源码)
                自身.行们.append(自身.当前行)
            except StopIteration:
                自身.当前行 = ()
                return None
        return 自身.当前行[自身.索引]
    
    @property
    def 更多在行上(自身):
        return 自身.索引 < len(自身.当前行)
    
    def __str__(自身):
        数目 = len(自身.行们)
        消息 = '{0:>' + str(math.floor(math.log10(数目))+1) + "}: "
        串文 = ''
        for 游标针 in range(max(0, 数目-4), 数目-1):
            串文 = 串文 + 消息.format(游标针+1) + ' '.join(map(str, 自身.行们[游标针])) + '\n'
        串文 = 串文 + 消息.format(数目)
        串文 = 串文 + ' '.join(map(str, 自身.当前行[:自身.索引]))
        串文 = 串文 + ' >> '
        串文 = 串文 + ' '.join(map(str, 自身.当前行[自身.索引:]))
        return 串文.strip()
    
try:
    import readline
except:
    pass

class 输入读器:
    def __init__(自身, 提示符):
        自身.提示符 = 提示符
    
    def __iter__(自身):
        while True:
            yield input(自身.提示符)
            自身.提示符 = ' ' * len(自身.提示符)

class 行读器:
    def __init__(自身, 行们, 提示符, 注释=";"):
        自身.行们 = 行们
        自身.提示符 = 提示符
        自身.注释 = 注释

    def __iter__(自身):
        while 自身.行们:
            行 = 自身.行们.pop(0).strip('\n')
            if(自身.提示符 is not None and 行 != "" and not 行.lstrip().startswith(自身.注释)):
                print(自身.提示符 + 行)
                自身.提示符 = ' ' * len(自身.提示符)
            yield 行
        raise EOFError