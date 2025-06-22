class 简单虚机():
    def __init__(自身):
        自身.栈 = []
        自身.操作 = {
            "推": 自身.操作_推,
            "弹": 自身.操作_弹,
            "加": 自身.操作_加,
            "减": 自身.操作_减,
            "乘": 自身.操作_乘,
            "除": 自身.操作_除,
            "等": 自身.操作_等,
            "大于": 自身.操作_大于,
            "小于": 自身.操作_小于,
            "与": 自身.操作_与,
            "或": 自身.操作_或,
            "非": 自身.操作_非,
            "异或": 自身.操作_异或
        }
        自身.代码 = []

    def 操作_推(自身, 值):
        自身.栈.append(值)

    def 操作_弹(自身):
        if not 自身.栈:
            raise ValueError("栈为空，无法弹出元素")
        return 自身.栈.pop()

    def 操作_加(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行加法")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 + 值1)

    def 操作_减(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行减法")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 - 值1)

    def 操作_乘(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行乘法")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 * 值1)

    def 操作_除(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行除法")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        if 值1 == 0:
            raise ValueError("除数不能为零")
        自身.操作_推(值2 // 值1)

    def 操作_等(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行等比较")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(1 if 值2 == 值1 else 0)
        
    def 操作_大于(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行大于比较")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(1 if 值2 > 值1 else 0)

    def 操作_小于(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行小于比较")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(1 if 值2 < 值1 else 0)

    def 操作_与(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行与操作")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 & 值1)

    def 操作_或(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行或操作")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 | 值1)

    def 操作_非(自身):
        if not 自身.栈:
            raise ValueError("栈为空，无法执行非操作")
        值1 = 自身.栈.pop()
        自身.操作_推(~值1)

    def 操作_异或(自身):
        if len(自身.栈) < 2:
            raise ValueError("栈中元素不足，无法执行异或操作")
        值1 = 自身.栈.pop()
        值2 = 自身.栈.pop()
        自身.操作_推(值2 ^ 值1)

    def 跑(自身):
        def 移出(列表):
            头 = 列表[0]
            列表 = 列表[1:]
            return 头, 列表
        
        while 自身.代码:
            头, 自身.代码 = 移出(自身.代码)
            if 头 == "推":
                值, 自身.代码 = 移出(自身.代码)
                自身.操作[头](值)
            elif 头 == "弹":
                return 自身.操作[头]()
            else:
                自身.操作[头]()

        return None

def 读取文件(文件名):
    try:
        with open(文件名, "r", encoding="utf-8") as 文件:
            行 = 文件.readlines()
        指令集 = []
        for 行内容 in 行:
            行内容 = 行内容.strip()
            if 行内容 and not 行内容.startswith(";"):  # 忽略空行和注释
                指令 = 行内容.split()
                if 指令[0] == "推" and len(指令) > 1:
                    try:
                        值 = int(指令[1])  # 将操作数转换为整数
                        指令集 += ["推", 值]
                    except ValueError:
                        print(f"错误: 文件 '{文件名}' 中的指令 '{行内容}' 的操作数不是有效的整数。")
                        return None
                else:
                    指令集 += 指令
        return 指令集
    except FileNotFoundError:
        print(f"错误: 文件 '{文件名}' 未找到。")
        return None

def 启动_REPL():
    虚机实例 = 简单虚机()
    print("简单虚机 REPL 已启动。输入指令或输入 '退出' 结束。")
    while True:
        输入 = input(">>> ").strip()
        if 输入 == "退出":
            print("退出 REPL。")
            break
        elif 输入.startswith("读取 "):
            文件名 = 输入.split(" ", 1)[1]
            指令集 = 读取文件(文件名)
            if 指令集:
                虚机实例.代码 += 指令集
                结果 = 虚机实例.跑()
                if 结果 is not None:
                    print(f"结果: {结果}")
                print(f"当前栈: {虚机实例.栈}")
        else:
            try:
                指令 = 输入.split()
                if not 指令:
                    continue
                操作 = 指令[0]
                if 操作 == "推":
                    if len(指令) < 2:
                        print("错误: '推' 操作需要一个参数。")
                        continue
                    值 = int(指令[1])
                    虚机实例.代码 += ["推", 值]
                elif 操作 in 虚机实例.操作:
                    虚机实例.代码 += [操作]
                else:
                    print(f"错误: 未知指令 '{操作}'。")
                    continue
                结果 = 虚机实例.跑()
                if 结果 is not None:
                    print(f"结果: {结果}")
                print(f"当前栈: {虚机实例.栈}")
            except Exception as e:
                print(f"错误: {e}")

if __name__ == "__main__":
    启动_REPL()
    
'''
程序;

推 5
推 3
加
推 2
乘

输出;

PS Z:\> py.exe .\VM简单栈虚机.py
简单虚机 REPL 已启动。输入指令或输入 '退出' 结束。
>>> 读取 test.txt
当前栈: [16]
>>> 退出
退出 REPL。

'''
