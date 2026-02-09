"""
简单测试 - 详细版
"""
from wasm汇编器 import 汇编
from 模块加载器 import 模块加载器
from 解释器核心 import 解释器
from 数据结构 import 栈值
from 枚举定义 import 值类型

源码 = '''
(module
    (func (result i32)
        i32.const 42
    )
)
'''

print("源码:")
print(源码)

字节码 = 汇编(源码)
print(f"\\n字节码长度: {len(字节码)}")
print(f"字节码 (hex): {字节码.hex()}")

# 打印每个字节
print("\\n字节码详细:")
for i, b in enumerate(字节码):
    print(f"  [{i:2d}] 0x{b:02x} ({b:3d})")

# 解析模块
加载器 = 模块加载器()
模块实例 = 加载器.加载模块(字节码)

print(f"\\n模块加载成功!")
print(f"函数数量: {len(模块实例.函数列表)}")

if 模块实例.函数列表:
    函数 = 模块实例.函数列表[0]
    print(f"函数0类型索引: {函数.类型索引}")
    print(f"函数0代码起始: {函数.代码起始}")
    print(f"函数0代码结束: {函数.代码结束}")
    print(f"函数0局部变量: {函数.局部变量类型}")
    
    # 打印函数体字节码
    函数体 = 模块实例.字节码[函数.代码起始:函数.代码结束]
    print(f"函数0函数体: {函数体.hex()}")
    print(f"函数体字节:", list(函数体))

# 执行
解释器实例 = 解释器(模块实例)

print(f"\\n设置调用前:")
print(f"  PC: {模块实例.程序计数器}")
print(f"  SP: {模块实例.操作数栈指针}")

解释器实例.设置调用(0)

print(f"\\n设置调用后:")
print(f"  PC: {模块实例.程序计数器}")
print(f"  SP: {模块实例.操作数栈指针}")
print(f"  当前字节: 0x{模块实例.字节码[模块实例.程序计数器]:02x}")

try:
    解释器实例.解释()
    print(f"\\n执行成功!")
    print(f"执行后 SP: {模块实例.操作数栈指针}")
    
    if 模块实例.操作数栈指针 >= 0:
        结果 = 模块实例.弹出操作数()
        print(f"结果: {结果.值}")
except Exception as 错误:
    print(f"\\n错误: {错误}")
    print(f"错误时 PC: {模块实例.程序计数器}")
    if 模块实例.程序计数器 < len(模块实例.字节码):
        print(f"错误时当前字节: 0x{模块实例.字节码[模块实例.程序计数器]:02x}")
    import traceback
    traceback.print_exc()
