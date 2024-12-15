# #!/usr/bin/env python3

调试 = False

# 调试函数

def 调试(*参数, 缩进=0, **关键字参数):
    if 调试:
        缩进 = '    ' * 缩进
        打印(f'\033[2m---> {缩进}{" ".join(map(str, 参数))}\033[0m', **关键字参数)

def 成功(*参数, **关键字参数):
    打印(f'\033[92m{" ".join(map(str, 参数))}\033[0m', **关键字参数)

def 警告(*参数, **关键字参数):
    打印(f'\033[93m{" ".join(map(str, 参数))}\033[0m', **关键字参数)


def 解析树(代码):
    标记 = 代码.replace('(', ' ( ').replace(')', ' ) ').split()
    树代码 = []
    for 标记 in 标记:
        if 标记 not in ['(', ')']:
            树代码.append(f'"{标记}"')
        else:
            树代码.append(标记)
        if 标记 != '(':
            树代码.append(',')
    return eval(''.join(['(', *树代码, ')']))


def 是标识符(s):
    # [a-z][a-z\-]*
    首字符, *其余字符 = s
    if not 首字符.islower():
        return False
    for 字符 in 其余字符:
        if not (字符.islower() or 字符 == '-'):
            return False
    return True


class 函数:
    def __init__(自身, 名称='匿名', 函数=None, 参数类型=None, 参数数量=''):
        自身.名称 = 名称
        自身.函数 = 函数
        自身.参数类型 = 参数类型
        自身.参数数量 = 参数数量
        自身.锁定 = False

    def __call__(自身, *参数):
        自身._检查参数(参数)
        return 自身.函数(*参数)
    
    def _检查参数(自身, 参数):
        参数数量 = len(参数)
        断言 eval(f'{参数数量} {自身.参数数量}'), (
            f'期望参数数量 {自身.参数数量}, 实际得到 {参数数量}')
        if 自身.参数类型 is not None:
            if 自身.参数类型 == '相同':
                参数类型 = type(参数[0])
            else:
                参数类型 = 自身.参数类型
            for i, 参数 in enumerate(参数):
                if type(参数) != 参数类型:
                    序号 = i + 1
                    类型1 = getattr(参数类型, '__name__', 参数类型).lower()
                    类型2 = getattr(type(参数), '__name__', type(参数)).lower()
                    断言 False, f'期望第 {序号} 个参数类型为 {类型1} 但是得到 {类型2}'

    def __str__(自身):
        信息 = ''
        if 自身.参数数量 is not None:
            参数数量 = 自身.参数数量.replace('== ', '')
            信息 += ' ({0} 参数)'.format(参数数量)
        if 自身.参数类型 is not None:
            参数类型 = getattr(自身.参数类型, '__name__', str(自身.参数类型))
            信息 += ' (类型 {0})'.format(参数类型)
        return f"<函数 '{自身.名称}'{信息}>"
    
    def __repr__(自身):
        return str(自身)
        
def 求值表达式(表达式, 作用域, 层级=0):
    if isinstance(表达式, tuple):
        调试('表达式:', str(表达式).replace(',', '').replace('\'', ''), 缩进=层级)
        调试('| 变量:', ' '.join(f'{名称}={值}' for 名称, 值 in 作用域.items() if not callable(值)), 缩进=层级)
        调试('| 函数:', ' '.join(名称 for 名称, 值 in 作用域.items() if callable(值)), 缩进=层级)

        断言 len(表达式), '缺少函数'
        主函数 = 表达式[0]
        
        if 主函数 == '定义':
            # 在作用域中定义一个变量
            _, 名称, 值 = 表达式
            断言 是标识符(名称), f'无效的标识符: {名称}'
            if type(值) is tuple:
                临时作用域 = 作用域.copy()
                临时作用域[名称] = 函数(名称)
                临时作用域[名称].锁定 = True
                临时结果 = 求值表达式(值, 临时作用域, 层级 + 1)
                if callable(临时结果) and 临时结果 != 临时作用域[名称]:
                    临时作用域[名称].锁定 = False
                    临时作用域[名称].函数 = 临时结果.函数
                    临时作用域[名称].参数数量 = 临时结果.参数数量
                    作用域[名称] = 临时作用域[名称]
                    return
            作用域[名称] = 求值表达式(值, 作用域, 层级 + 1)
            return

        if 主函数 == '函数':
            # 返回一个函数实例
            _, 参数名称, *定义, 表达式体 = 表达式
            参数数量 = len(参数名称)
            静态作用域 = 作用域.copy() # 复制作用域用于静态变量
            for 定义语句 in 定义:
                # 在复制的作用域中定义静态局部变量
                求值表达式(定义语句, 静态作用域, 层级 + 1)
            def _函数(*参数):
                # 复制静态作用域以添加参数
                函数作用域 = 静态作用域.copy() # pylint: disable=E0601
                for 参数名, 参数值 in zip(参数名称, 参数):
                    函数作用域[参数名] = 求值表达式(参数值, 作用域, 层级 + 1)
                return 求值表达式(表达式体, 函数作用域, 层级 + 1)
            return 函数(函数=_函数, 参数数量=f'== {参数数量}')

        if 主函数 == '如果':
            _, 条件, 真, 假 = 表达式
            if 求值表达式(条件, 作用域, 层级 + 1):
                return 求值表达式(真, 作用域, 层级 + 1)
            else:
                return 求值表达式(假, 作用域, 层级 + 1)
        
        # (...) <- 这应该是一个函数调用

        if isinstance(主函数, tuple):
            # 求值主函数看是否为一个函数
            主函数 = 求值表达式(主函数, 作用域, 层级 + 1)
            断言 type(主函数) == 函数, (
                f'期望一个函数但得到了 {type(主函数).__name__}')
            表达式 = (主函数, *表达式[1:])
            return 求值表达式(表达式, 作用域, 层级)

        # (函数或函数名 ...)

        函数 = None

        if isinstance(主函数, 函数):
            函数, *参数 = 表达式
        
        elif type(主函数) is str and 主函数 in 作用域:
            函数名, *参数 = 表达式
            函数 = 作用域[函数名]
        
        if 函数 is not None:
            断言 callable(函数), f'{函数} 不可调用'
            参数 = [求值表达式(参数值, 作用域, 层级 + 1) for 参数值 in 参数]
            调试('| 调用:', 函数.名称, 参数, 缩进=层级)
            断言 not 函数.锁定, f'调用了一个未完成的函数: {函数.名称}'
            结果 = 函数(*参数)
            调试('| 返回:', 结果, 缩进=层级)
            return 结果
        

        断言 not 是标识符(主函数), f'未定义的函数: {主函数}'
        断言 False, f'无效的函数名: {主函数}'

    else:
        # 求值一个参数

        if callable(表达式):
            return 表达式

        尝试:
            return int(表达式)
        例外:
            pass
        
        尝试:
            return {'#t': True, '#f': False}[表达式]
        例外:
            pass
            
        尝试:
            return 作用域[表达式]
        例外:
            pass
        
        断言 not 是标识符(表达式), f'未定义的变量: {表达式}'
        断言 False, f'无效的语法: {表达式}'

def 初始化作用域():
    # 初始化一个带有预定义变量的干净变量作用域

    def _加(*参数):
        return sum(参数)

    def _乘(*参数):
        结果 = 1
        for i in 参数:
            结果 *= i
        return 结果
    
    def _等于(*参数):
        for 数 in 参数[1:]:
            if 参数[0] != 数:
                return False
        return True
    
    def _与(*参数):
        return all(参数)
    
    def _或(*参数):
        return any(参数)
    
    return {
        '+':            函数('+', _加,                         int, '>= 2'),
        '-':            函数('-', lambda x, y: x - y,           int, '== 2'),
        '*':            函数('*', _乘,                         int, '>= 2'),
        '/':            函数('/', lambda x, y: x // y,          int, '== 2'),
        'mod':          函数('mod', lambda x, y: x % y,         int, '== 2'),
        '=':            函数('=', _等于,                         'same', '>= 2'),
        '>':            函数('>', lambda x, y: x > y,           int, '== 2'),
        '<':            函数('<', lambda x, y: x < y,           int, '== 2'),
        'and':          函数('and', _与,                       bool, '>= 2'),
        'or':           函数('or', _或,                         bool, '>= 2'),
        'not':          函数('not', lambda x: not x,            bool, '== 1'),
        'print-num':    函数('print-num',   lambda x: 打印(x), int, '== 1'),
        'print-bool':   函数('print-bool', lambda x: 打印({True: '#t', False: '#f'}[x]), bool, '== 1')
    }


def 跑(代码, 作用域=初始化作用域(), 交互模式=False):
    尝试:
        表达式列表 = 解析树(代码)
    例外:
        if not (交互模式 or 调试):
            打印('错误:', '无效的语法')
        else:
            警告('错误:', '无效的语法')
        return
    for 表达式 in 表达式列表:
        尝试:
            返回值 = 求值表达式(表达式, 作用域)
            if (调试 or 交互模式) and 返回值 is not None:
                成功('===>', 返回值)
        例外 Exception as e:
            if 调试:
                导入 traceback
                警告(traceback.format_exc())
            elif 交互模式:
                警告('错误:', str(e) or '无效的语法')
            else:
                打印('错误:', str(e) or '无效的语法')


if __name__ == '__main__':
    导入 sys, os, readline

    if len(sys.argv) > 1 and os.path.isfile(sys.argv[1]):
        跑(open(sys.argv[1]).read())

    else:
        while True:
            尝试:
                跑(输入('MiniLisp> '), 交互模式=True)
            例外:
                break
                
