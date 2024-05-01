'''错误'''
class 机器错误(Exception):
    '''虚拟机运行时错误'''
    pass

class 解析错误(Exception):
    '''一个错误发生在解析期间'''
    pass

class 编译错误(Exception):
    '''一个错误发生在编译期间'''
    pass

class 程序出口(Exception):
    '''用信号那给我们想要到程序出口'''
    pass
