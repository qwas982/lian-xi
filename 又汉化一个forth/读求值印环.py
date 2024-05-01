'''读-求值-印 环, REPL'''

import sys
from 编译器 import 编译, 是否嵌入推, 获取嵌入推值
from 错误们 import 解析错误, 机器错误, 编译错误
from 解析器 import 解析
from 解释器 import 是否字符串, 机器

def 打印代码(虚机, 出=sys.stdout, 每行操作=8, 寄存器们=True):
    '''打印代码与状态为虚拟机'''
    if 寄存器们:
        出.write("指令指针: %d\n" % 虚机.指令指针)
        出.write("数据栈: %s\n" % str(虚机.栈))
        出.write("返回栈: %s\n" % str(虚机.返回栈))

    def 到串(操作):
        if 是否嵌入推(操作):
            操作 = 获取嵌入推值(操作)

        if 是否字符串(操作):
            return '"%s"' % repr(操作)[1:-1]
        elif callable(操作):
            return 虚机.查找(操作)
        else:
            return str(操作)
        
    for 地址, 操作 in enumerate(虚机.代码):
        if (地址 % 每行操作) == 0 and (地址 == 0 or (地址+1) < len(虚机.代码)):
            if 地址 > 0:
                出.write("\n")
            出.write("%0*d " % (max(4, len(str(len(虚机.代码)))), 地址))
        出.write("%s " % 到串(操作))
    出.write("\n")

def 读求值印环(优化=True, 坚持=True):
    '''起始一个简单[读取-求值-打印 环]为这台机器.
    
    实参们:
        优化: 控制要不要去跑已输入代码通过其优化器.
        坚持: 若真, 其机器是不删在每行之后. '''
    
    print("额外命令为其读求值印环: ")
    print(" .代码       - 打印代码")
    print(" .未加工     - 打印未加工代码")
    print(" .退出       - 去出口立即!")
    print(" .重置       - 重置机器(指令指针与栈)")
    print(" .重起始     - 创建一个清洁, 新机器")
    print(" .清晰       - 类似 .重起始")
    print(" .栈         - 打印数据栈\n")

    机器的 = 机器([])

    def 匹配(符, *实参们):
        return any(map(lambda 实参: 符.strip() == 实参, 实参们))
    
    while True:
        try:
            源 = input("->-> ").strip()

            if 源[0] == "." and len(源) > 1:
                if 匹配(源, ".退出"):
                    return
                elif 匹配(源, ".代码"):
                    打印代码(机器的)
                elif 匹配(源, ".未加工"):
                    print(机器的.代码)
                elif 匹配(源, ".重置"):
                    机器的.重置()
                elif 匹配(源, ".重起始", ".清晰"):
                    机器的 = 机器([])
                elif 匹配(源, ".栈"):
                    print(机器的.栈)
                else:
                    raise 解析错误("未知命令: %s" % 源)
                continue

            代码 = 编译(解析(源), 静默=False, 优化=优化)

            if not 坚持:
                机器的.重置()

            机器的.代码 = 机器的.代码 + 代码
            机器的.跑()
        except EOFError:
            return
        except KeyboardInterrupt:
            return
        except 解析错误 as 错了:
            print("解析错误: %s" % 错了)
        except 机器错误 as 错了:
            print("机器错误: %s" % 错了)
        except 编译错误 as 错了:
            print("编译错误: %s" % 错了)
