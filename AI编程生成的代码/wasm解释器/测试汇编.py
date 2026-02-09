"""
测试汇编器
"""
from wasm汇编器 import 汇编

源码 = '''
(module
    (func (result i32)
        i32.const 42
    )
)
'''

字节码 = 汇编(源码)
print(f"字节码长度: {len(字节码)}")
print(f"字节码 (hex): {字节码.hex()}")
print(f"前8字节: {list(字节码[:8])}")
print(f"魔数: {字节码[:4]}")
print(f"版本: {字节码[4:8]}")

# 检查魔数
预期魔数 = b'\x00asm'
print(f"预期魔数: {预期魔数}")
print(f"魔数匹配: {字节码[:4] == 预期魔数}")
