'''主岛, 程序主入口, 我将其称之为主岛, 顾名思义'''

import sys
import optparse

import 解析器
import 解释器
import 编译器
import 指令们
import 优化器
import 读求值印环
import 栈
import 错误们

def 选项():
    选了 = optparse.OptionParser("用法: %prog [选项] [文件]")

    选了.add_option("-转", "--转储", dest="转储", help="转储机器码与去出口.", action="store_true", default=False)

    选了.add_option("-啰", "--啰嗦", dest="啰嗦", help="启用啰嗦输出.", action="store_true", default=False)

    选了.add_option("-优", "--优化", dest="优化", help="启用优化程序.", action="store_true", default=False)

    选了.add_option("-读", "--读求值印环", dest="读求值印环", help="进入读求值印环 - REPL.", action="store_true", default=False)

    选了.disable_interspersed_args()
    return 选了

def 解析与跑(文件, 选了们):
    代码 = 编译器.编译(解析器.解析(文件), 静默=not 选了们.啰嗦, 忽视错误=False, 优化=选了们.优化)

    机器的 = 解释器.机器(代码)

    if not 选了们.转储:
        机器的.跑()
    else:
        读求值印环.打印代码(机器的, 寄存器们=False)

def 总管道(): # 主线路, 查了才知道原来是这个意思
    def 跑(文件, 选了们):
        try:
            解析与跑(文件, 选了们)
        except 错误们.机器错误 as 错了:
            print("跑时错误: %s" % 错了)
            sys.exit(1)
        except 错误们.编译错误 as 错了:
            print("编译的错误: %s" % 错了)
            sys.exit(1)
        except 错误们.解析错误 as 错了:
            print("解析错误: %s" % 错了)
            sys.exit(1)

    选了 = 选项()
    (选了们, 实参们) = 选了.parse_args()

    if 选了们.读求值印环:
        读求值印环.读求值印环()
        sys.exit(0)

    if len(实参们) == 0:
        选了.print_help()
        sys.exit(1)

    for 名称 in 实参们:
        if 名称 == "-":
            跑(sys.stdin, 选了们)
        else:
            with open(名称, 'rt', encoding='utf-8') as 文件:
                跑(文件, 选了们)

if __name__ == "__main__":
    try:
        总管道()
        sys.exit(0)
    except KeyboardInterrupt:
        pass
