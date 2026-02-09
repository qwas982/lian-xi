"""
WebAssembly Python 解释器 - 调试测试
用于发现和修复边界情况问题
"""
import struct
import unittest
from typing import List

from 枚举定义 import 操作码, 值类型
from 数据结构 import 模块, 栈值, 函数类型, 函数
from 解释器核心 import 解释器
from 模块加载器 import 模块加载器


class 测试解释器边界情况(unittest.TestCase):
    """测试解释器的边界情况"""

    def _编码无符号32位(self, 值: int) -> bytes:
        """编码LEB128无符号32位"""
        结果 = bytearray()
        while True:
            字节 = 值 & 0x7F
            值 >>= 7
            if 值 != 0:
                字节 |= 0x80
            结果.append(字节)
            if 值 == 0:
                break
        return bytes(结果)

    def _编码有符号32位(self, 值: int) -> bytes:
        """编码LEB128有符号32位"""
        结果 = bytearray()
        while True:
            字节 = 值 & 0x7F
            值 >>= 7
            if (值 != 0 or (字节 & 0x40)) and (值 != -1 or not (字节 & 0x40)):
                字节 |= 0x80
            结果.append(字节)
            if not (字节 & 0x80):
                break
        return bytes(结果)

    def _创建简单模块(self, 代码: bytes) -> 模块:
        """创建一个包含单个函数的简单模块"""

        # 构建Wasm模块字节码
        字节码 = bytearray()

        # 头部
        字节码.extend([
            0x00, 0x61, 0x73, 0x6D,  # magic
            0x01, 0x00, 0x00, 0x00,  # version
        ])

        # 类型段 - 一个函数类型 () -> (i32)
        类型段内容 = bytes([
            0x01,  # 类型数量
            0x60,  # 函数类型
            0x00,  # 参数数量
            0x01,  # 返回数量
            0x7F,  # i32
        ])
        字节码.append(0x01)  # 段类型: Type
        字节码.extend(self._编码无符号32位(len(类型段内容)))
        字节码.extend(类型段内容)

        # 函数段 - 一个函数使用类型0
        函数段内容 = bytes([
            0x01,  # 函数数量
            0x00,  # 类型索引
        ])
        字节码.append(0x03)  # 段类型: Function
        字节码.extend(self._编码无符号32位(len(函数段内容)))
        字节码.extend(函数段内容)

        # 导出段 - 导出函数0为"main"
        导出段内容 = bytes([
            0x01,  # 导出数量
            0x04,  # 名称长度
        ])
        导出段内容 += b'main'
        导出段内容 += bytes([
            0x00,  # 导出类型: 函数
            0x00,  # 函数索引
        ])
        字节码.append(0x07)  # 段类型: Export
        字节码.extend(self._编码无符号32位(len(导出段内容)))
        字节码.extend(导出段内容)

        # 代码段
        # 局部变量声明格式：组数 + (每组: 数量 + 类型)
        # 0x00 表示0组局部变量
        # 注意：代码已经包含了 end 指令 (0x0B)
        函数体 = bytes([0x00]) + 代码  # 0 locals + code (code already has end)

        # 代码段内容 = 函数体数 + 函数体大小 + 函数体
        代码段内容 = bytes([0x01]) + self._编码无符号32位(len(函数体)) + 函数体

        字节码.append(0x0A)  # 段类型: Code
        字节码.extend(self._编码无符号32位(len(代码段内容)))
        字节码.extend(代码段内容)

        加载器 = 模块加载器()
        return 加载器.加载模块(bytes(字节码))

    def test_常量加载(self):
        """测试常量加载指令"""
        # i32.const 42
        代码 = bytes([
            0x41,  # i32.const
        ]) + self._编码有符号32位(42)

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        # 设置调用
        解释器实例.设置调用(0)

        # 执行
        解释器实例.解释()

        # 检查结果
        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 42)

    def test_加法运算(self):
        """测试i32加法"""
        # i32.const 10, i32.const 20, i32.add
        代码 = bytes([
            0x41,  # i32.const 10
        ]) + self._编码有符号32位(10) + bytes([
            0x41,  # i32.const 20
        ]) + self._编码有符号32位(20) + bytes([
            0x6A,  # i32.add
        ])

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        解释器实例.设置调用(0)
        解释器实例.解释()

        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 30)

    def test_等于零判断(self):
        """测试i32.eqz"""
        # i32.const 0, i32.eqz
        代码 = bytes([
            0x41,        # i32.const 0
        ]) + self._编码有符号32位(0) + bytes([
            0x45,        # i32.eqz
        ])

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        解释器实例.设置调用(0)
        解释器实例.解释()

        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 1)  # 0 == 0 为真

    def test_比较运算(self):
        """测试比较运算"""
        # i32.const 5, i32.const 10, i32.lt_s
        代码 = bytes([
            0x41,        # i32.const 5
        ]) + self._编码有符号32位(5) + bytes([
            0x41,        # i32.const 10
        ]) + self._编码有符号32位(10) + bytes([
            0x48,        # i32.lt_s
        ])

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        解释器实例.设置调用(0)
        解释器实例.解释()

        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 1)  # 5 < 10 为真

    def test_选择指令(self):
        """测试select指令"""
        # i32.const 42, i32.const 0, i32.const 1, select
        代码 = bytes([
            0x41,        # i32.const 42 (值1)
        ]) + self._编码有符号32位(42) + bytes([
            0x41,        # i32.const 0 (值2)
        ]) + self._编码有符号32位(0) + bytes([
            0x41,        # i32.const 1 (条件)
        ]) + self._编码有符号32位(1) + bytes([
            0x1B,        # select
        ])

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        解释器实例.设置调用(0)
        解释器实例.解释()

        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 42)  # 条件为真，选择值1

    def test_空块(self):
        """测试空块"""
        # block (result i32), i32.const 42, end
        代码 = bytes([
            0x02, 0x7F,  # block (result i32)
            0x41,        # i32.const 42
        ]) + self._编码有符号32位(42) + bytes([
            0x0B,        # end
        ])

        模块实例 = self._创建简单模块(代码)
        解释器实例 = 解释器(模块实例)

        解释器实例.设置调用(0)
        解释器实例.解释()

        结果 = 模块实例.弹出操作数()
        self.assertEqual(结果.值, 42)


def 运行调试测试():
    """运行调试测试"""
    unittest.main(argv=[''], verbosity=2, exit=False)


if __name__ == '__main__':
    运行调试测试()
