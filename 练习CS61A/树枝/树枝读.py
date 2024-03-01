from 树枝ucb import main, 追踪, 交互
from 树枝牌 import 牌化的行们, 分隔符全局
from 树枝缓冲区 import 缓冲区, 输入读器, 行读器

#scheme读器.py    牌 -> 句法器(解析) -> 树
class 点对:
    def __init__(自身, 第一, 第二):
        自身.第一 = 第一
        自身.第二 = 第二
    
    def __repr__(自身):
        return '点对({0}, {1})'.format(repr(自身.第一), repr(自身.第二))
    
    def __str__(自身):
        符 = '(' + str(自身.第一)
        第二 = 自身.第二
        while isinstance(第二, 点对):
            符 = 符 + ' ' + str(第二.第一)
            第二 = 第二.第二
        if 第二 is not 无:
            符 = 符 + ' . ' + str(第二)
        return 符 + ')'
    
    def __len__(自身):
        数, 第二 = 1, 自身.第二
        while isinstance(第二, 点对):
            数 = 数 + 1
            第二 = 第二.第二
        if 第二 is not 无:
            raise TypeError('在不适当的列表上尝试长度')
        return 数
    
    def __eq__(自身, 是不是点对):
        if not isinstance(是不是点对, 点对):
            return False
        return 自身.第一 == 是不是点对.第一 and 自身.第二 == 是不是点对.第二
    
    def 映射(自身, 函):
        已映射 = 函(自身.第一)
        if 自身.第二 is 无 or isinstance(自身.第二, 点对):
            return 点对(已映射, 自身.第二.映射(函))
        else:
            raise TypeError('病态形式列表')
        
class 无:
    def __repr__(自身):
        return '无'
    
    def __str__(自身):
        return '()'
    
    def __len__(自身):
        return 0
    
    def 映射(自身, 函):
        return 自身

无 = 无()

def 树枝读(源):
    if 源.当前() is None:
        raise EOFError
    值 = 源.移除前端()
    if 值 == '无':
        return 无
    elif 值 == '(':
        return 读尾(源)
    elif 值 == "'":
        return 点对('引号', 点对(树枝读(源), 无))
    elif 值 not in 分隔符全局:
        return 值
    else:
        raise SyntaxError('不期望的牌: {0}'.format(值))

def 读尾(源):
    try:
        if 源.当前() is None:
            raise SyntaxError('不期望的文件终')
        elif 源.当前() == ')':
            源.移除前端()
            return 无
        elif 源.当前() == '.':
            源.移除前端()
            值 = 树枝读(源)
            if 源.当前() == ')':
                源.移除前端()
                return 值
            else:
                raise SyntaxError("一个点后面有多个元素")
        else:
            第一 = 树枝读(源)
            其余 = 读尾(源)
            return 点对(第一, 其余)
    except EOFError:
        raise SyntaxError('不期望的文件终')

def 缓冲区输入(提示符='树枝>> '):
    return 缓冲区(牌化的行们(输入读器(提示符)))

def 缓冲区行们(行们, 提示符='树枝>> ', 秀提示符=False):
    if 秀提示符:
        输入行们 = 行们
    else:
        输入行们 = 行读器(行们, 提示符)
    return 缓冲区(牌化的行们(输入行们))

def 读行(行):
    return 树枝读(缓冲区(牌化的行们([行])))

@main
def 读打印环():
    while True:
        try:
            源 = 缓冲区输入('句法>> ')
            while 源.更多在行上:
                表达式 = 树枝读(源)
                print('串:', 表达式)
                print('代表:', repr(表达式))
        except (SyntaxError, ValueError) as 错误:
            print(type(错误).__name__ + ':', 错误)
        except (KeyboardInterrupt, EOFError):
            print('Ctrl+Z')
            return
