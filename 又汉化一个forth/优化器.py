'''优化器'''

import 错误们
import 指令们
import 解释器

def 已优化(代码, 静默=True, 忽视错误=True):
    '''表现优化在已准备已解析的代码上'''
    return 常量折叠(代码, 静默=静默, 忽视错误=忽视错误)

def 常量折叠(代码, 静默=True, 忽视错误=True):
    '''常量折叠简单表达式如 2 3 + 到 5.
    
    实参们:
        代码: 代码在非土著类型里.
        静默: 是否打印已制造了优化的控制旗.
        忽视错误: 是否升起异常在找到的错误上.
        
    在环里直到我们没有做任意优化.
    例如, "2 3 + 5 *" 会被已优化到"5 5 *" 与在下一次迭代内到25.
    是的,这是极端慢, 大O表示法,我们会修复那在其它时间. '''

    算术 = list(map(指令们.查找, [
        指令们.加法,
        指令们.位移与,
        指令们.位移或,
        指令们.位移异或,
        指令们.除法,
        指令们.等号,
        指令们.大于,
        指令们.小于,
        指令们.取模,
        指令们.乘法,
        指令们.减法,
    ]))
    
    除零 = map(指令们.查找, [
        指令们.除法,
        指令们.取模,
    ])

    查找 = 指令们.查找

    def 是否函数(操作):
        try:
            指令们.查找(操作)
            return True
        except KeyError:
            return False
        
    def 是否常量(操作):
        return 操作 is None or 解释器.是否常量(操作, 已引号=True) or not 是否函数(操作)
    
    保持跑 = True
    while 保持跑:
        保持跑 = False
        # 找两个连续数目与一个算术操作
        for 游标, 甲 in enumerate(代码):
            乙 = 代码[游标+1] if 游标+1 < len(代码) else None
            丙 = 代码[游标+2] if 游标+2 < len(代码) else None

            # 常量折叠算术操作 (todo: 移到检查函)
            if 解释器.是否数目(甲, 乙) and 丙 in 算术:
                '''尽管我们能在编译时侦测除以零,我们不报告它在这里,
                    因为周围的系统对此掌握得不太好.所以仅离开它为当下.
                    (注意：若我们有一个"错误"指令,
                    我们能实际上变形其表达式到一个错误, 
                    或出口指令多半是)'''
                if 乙 == 0 and 丙 in 除零:
                    if 忽视错误:
                        continue
                    else:
                        raise 错误们.编译错误(ZeroDivisionError("除零"))
                    
                '''计算结果通过跑在机器上
                (lambda 虚拟机: ... 是嵌入推, 视编译器)'''
                结果 = 解释器.机器([lambda 虚机: 虚机.推(甲), lambda 虚机: 虚机.推(乙), 指令们.查找(丙)]).跑().顶部
                del 代码[游标:游标+3]
                代码.insert(游标, 结果)

                if not 静默:
                    print("优化器: 已常量折叠 %s %s %s 到 %s" % (甲, 乙, 丙, 结果))

                保持跑 = True
                break

            # 翻译 <常量> 副本到 <常量> <常量>
            if 是否常量(甲) and 乙 == 查找(指令们.副本):
                代码[游标+1] = 甲
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s %s" % (甲, 乙, 甲, 甲))
                保持跑 = True
                break

            # 死代码移除: <常量> 落下
            if 是否常量(甲) and 乙 == 查找(指令们.落下):
                del 代码[游标:游标+2]
                if not 静默:
                    print("优化器: 已移除死代码 %s %s" % (甲, 乙))
                保持跑 = True
                break

            if 甲 == 查找(指令们.无操作):
                del 代码[游标]
                if not 静默:
                    print("优化器: 已移除死代码 %s" % 甲)
                保持跑 = True
                break

            # 死代码移除: <整数> 转换整型
            if isinstance(甲, int) and 乙 == 查找(指令们.转换整型):
                del 代码[游标+1]
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 甲))
                保持跑 = True
                break

            # 死代码移除: <浮点> 转换浮点
            if isinstance(甲, float) and 乙 == 查找(指令们.转换浮点):
                del 代码[游标+1]
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 甲))
                保持跑 = True
                break

            # 死代码移除: <字符串> 转换串
            if isinstance(甲, str) and 乙 == 查找(指令们.转换串):
                del 代码[游标+1]
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 甲))
                保持跑 = True
                break

            # 死代码移除: <布尔> 转换布尔
            if isinstance(甲, bool) and 乙 == 查找(指令们.转换布尔):
                del 代码[游标+1]
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 甲))
                保持跑 = True
                break

            # <字符1> <字符2> 交换 -> <字符2> <字符1>
            if 是否常量(甲) and 是否常量(乙) and 丙 == 查找(指令们.交换):
                del 代码[游标:游标+3]
                代码 = 代码[:游标] + [乙, 甲] + 代码[游标:]
                if not 静默:
                    print("优化器: 已翻译 %s %s %s 到 %s %s" % (甲, 乙, 丙, 乙, 甲))
                保持跑 = True
                break

            # 甲 乙 超过 -> 甲 乙 甲
            if 是否常量(甲) and 是否常量(乙) and 丙 == 查找(指令们.超过):
                代码[游标+2] = 甲
                if not 静默:
                    print("优化器: 已翻译 %s %s %s 到 %s %s %s" % (甲, 乙, 丙, 甲, 乙, 甲))
                保持跑 = True
                break

            # "123" 转换整型 -> 123
            if 解释器.是否字符串(甲) and 乙 == 查找(指令们.转换整型):
                try:
                    数目 = int(甲)
                    del 代码[游标:游标+2]
                    代码.insert(游标, 数目)
                    if not 静默:
                        print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 数目))
                    保持跑 = True
                    break
                except ValueError:
                    pass

            if 是否常量(甲) and 乙 == 查找(指令们.转换串):
                del 代码[游标:游标+2]
                代码.insert(游标, str(甲))
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, str(甲)))
                保持跑 = True
                break

            if 是否常量(甲) and 乙 == 查找(指令们.转换布尔):
                del 代码[游标:游标+2]
                代码.insert(游标, bool(甲))
                if not 静默:
                    print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, bool(甲)))
                保持跑 = True
                break

            if 是否常量(甲) and 乙 == 查找(指令们.转换浮点):
                try:
                    值了 = float(甲)
                    del 代码[游标:游标+2]
                    代码.insert(游标, 值了)
                    if not 静默:
                        print("优化器: 已翻译 %s %s 到 %s" % (甲, 乙, 值了))
                    保持跑 = True
                    break
                except ValueError:
                    pass
    return 代码
