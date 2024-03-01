    
#伯克利大学.py
import code
import functools
import inspect
import re
import signal
import sys

def main(函):
    if inspect.stack()[1][0].f_locals['__name__'] == '__main__':
        实参们 = sys.argv[1:]
        函(*实参们)
    return 函

_前缀 = ''
def 追踪(函):
    @functools.wraps(函)
    def 已包裹起了(*实参们, **关键的们):
        global _前缀
        代表们 = [repr(游标) for 游标 in 实参们]
        代表们 = 代表们 + [repr(游标1) + '=' + repr(游标2) for 游标1, 游标2 in 关键的们.items()]
        日志('{0}({1})'.format(函.__name__, ', '.join(代表们)) + ':')
        _前缀 = _前缀 + '    '
        try:
            结果 = 函(*实参们, **关键的们)
            _前缀 = _前缀[:-4]
        except Exception as 游标:
            日志(函.__name__ + ' 已透过异常到出口')
            _前缀 = _前缀[:-4]
            raise
        日志('{0}({1}) -> {2}'.format(函.__name__, ', '.join(代表们), 结果))
        return 结果
    return 已包裹起了
def 日志(消息):
    print(_前缀 + re.sub('\n', '\n' + _前缀, str(消息)))

def 日志的当前行():
    帧 = inspect.stack()[1]
    日志('当前行: 文件 "{f[1]}", 行 {f[2]}, 在 {f[3]}'.format(f=帧))

def 交互(消息=None):
    帧 = inspect.currentframe().f_back
    名称空间 = 帧.f_globals.copy()
    名称空间.update(帧.f_locals)

    def 手握器(signum, 帧):
        print()
        exit(0)
    signal.signal(signal.SIGINT, 手握器)

    if not 消息:
        _, 文件名, 行, _, _, _ = inspect.stack()[1]
        消息 = '交互在文件 "{0}", 行 {1} \n'.format(文件名, 行)
        消息 = 消息 + '    Unix: 控制+D 继续其程序; \n'
        消息 = 消息 + '    Windows: 控制+Z 回车 继续其程序; \n'
        消息 = 消息 + '    exit() 或 控制+C 出其程序'

    code.interact(消息, None, 名称空间)