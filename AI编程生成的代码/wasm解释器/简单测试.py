"""
简单测试
"""
from wasm汇编器 import 汇编
from 模块加载器 import 模块加载器
from 解释器核心 import 解释器

源码 = '''
(module
    (func (result i32)
        i32.const 10
        i32.const 20
        i32.add
    )
)
'''

print("源码:")
print(源码)

字节码 = 汇编(源码)
print(f"\\n字节码长度: {len(字节码)}")
print(f"字节码 (hex): {字节码.hex()}")

# 解析模块
加载器 = 模块加载器()
模块实例 = 加载器.加载模块(字节码)

print(f"\\n模块加载成功!")
print(f"函数数量: {len(模块实例.函数列表)}")

if 模块实例.函数列表:
    函数 = 模块实例.函数列表[0]
    print(f"函数0代码起始: {函数.代码起始}")
    print(f"函数0代码结束: {函数.代码结束}")
    print(f"函数0函数体: {模块实例.字节码[函数.代码起始:函数.代码结束].hex()}")

# 执行
解释器实例 = 解释器(模块实例)
解释器实例.设置调用(0)

print(f"\\n执行前 PC: {模块实例.程序计数器}")
print(f"执行前 SP: {模块实例.操作数栈指针}")

try:
    解释器实例.解释()
    print(f"\\n执行成功!")
    print(f"执行后 SP: {模块实例.操作数栈指针}")
    
    if 模块实例.操作数栈指针 >= 0:
        结果 = 模块实例.弹出操作数()
        print(f"结果: {结果.值}")
except Exception as 错误:
    print(f"\\n错误: {错误}")
    import traceback
    traceback.print_exc()
