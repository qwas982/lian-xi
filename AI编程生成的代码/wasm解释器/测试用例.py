"""
WebAssembly Python 解释器 - 测试用例
"""
import struct
import unittest

from 枚举定义 import 操作码, 值类型, 段类型
from 数据结构 import 模块, 栈值, 函数类型, 函数, 内存, 表, 全局变量
from 工具函数 import (
    解码无符号32位, 解码有符号32位, 解码有符号64位,
    编码无符号整型, 编码有符号整型,
    单精度转32位整数, 双精度转32位整数,
    截断饱和32位有符号, 截断饱和32位无符号,
    浮点最小值, 浮点最大值, 复制浮点符号,
    值转字符串, 获取块类型结果
)
from 模块加载器 import 模块加载器


class 测试工具函数(unittest.TestCase):
    """测试工具函数"""
    
    def test_解码无符号32位(self):
        """测试LEB128无符号32位解码"""
        # 0x01 -> 1
        值, 偏移 = 解码无符号32位(bytes([0x01]), 0)
        self.assertEqual(值, 1)
        self.assertEqual(偏移, 1)
        
        # 0x80 0x01 -> 128
        值, 偏移 = 解码无符号32位(bytes([0x80, 0x01]), 0)
        self.assertEqual(值, 128)
        self.assertEqual(偏移, 2)
        
        # 0xFF 0xFF 0xFF 0xFF 0x0F -> 4294967295 (u32 max)
        值, 偏移 = 解码无符号32位(bytes([0xFF, 0xFF, 0xFF, 0xFF, 0x0F]), 0)
        self.assertEqual(值, 4294967295)
    
    def test_解码有符号32位(self):
        """测试LEB128有符号32位解码"""
        # 0x01 -> 1
        值, 偏移 = 解码有符号32位(bytes([0x01]), 0)
        self.assertEqual(值, 1)
        
        # 0x7F -> -1
        值, 偏移 = 解码有符号32位(bytes([0x7F]), 0)
        self.assertEqual(值, -1)
        
        # 0x80 0x7F -> -128
        值, 偏移 = 解码有符号32位(bytes([0x80, 0x7F]), 0)
        self.assertEqual(值, -128)
    
    def test_编码无符号整型(self):
        """测试LEB128无符号编码"""
        self.assertEqual(编码无符号整型(1), bytes([0x01]))
        self.assertEqual(编码无符号整型(128), bytes([0x80, 0x01]))
        self.assertEqual(编码无符号整型(255), bytes([0xFF, 0x01]))
    
    def test_浮点转换(self):
        """测试浮点到整数转换"""
        self.assertEqual(单精度转32位整数(1.5, True), 1)
        self.assertEqual(单精度转32位整数(-1.5, True), -1)
        self.assertEqual(双精度转32位整数(3.7, True), 3)
        self.assertEqual(双精度转32位整数(-3.7, True), -3)
    
    def test_截断饱和(self):
        """测试饱和截断"""
        self.assertEqual(截断饱和32位有符号(100.5), 100)
        self.assertEqual(截断饱和32位有符号(2147483648.0), 2147483647)
        self.assertEqual(截断饱和32位有符号(-2147483649.0), -2147483648)
        self.assertEqual(截断饱和32位无符号(-1.0), 0)
        self.assertEqual(截断饱和32位无符号(4294967296.0), 4294967295)
    
    def test_浮点工具(self):
        """测试浮点工具函数"""
        self.assertEqual(浮点最小值(1.0, 2.0), 1.0)
        self.assertEqual(浮点最大值(1.0, 2.0), 2.0)
        self.assertEqual(复制浮点符号(1.0, -2.0), -1.0)
        self.assertEqual(复制浮点符号(1.0, 2.0), 1.0)
    
    def test_值转字符串(self):
        """测试值转字符串"""
        self.assertEqual(值转字符串(值类型.整数32, 42), "42")
        self.assertEqual(值转字符串(值类型.整数32, 0xFFFFFFFF), "-1")


class 测试数据结构(unittest.TestCase):
    """测试数据结构"""
    
    def test_内存增长(self):
        """测试内存增长"""
        内存实例 = 内存(初始页数=1, 最大页数=10)
        self.assertEqual(内存实例.页数, 1)
        
        原页数 = 内存实例.增长(2)
        self.assertEqual(原页数, 1)
        self.assertEqual(内存实例.页数, 3)
        
        # 超过最大页数
        结果 = 内存实例.增长(100)
        self.assertEqual(结果, -1)
    
    def test_表初始化(self):
        """测试表初始化"""
        表实例 = 表(初始大小=10, 最大大小=100)
        self.assertEqual(len(表实例.元素), 10)
        self.assertIsNone(表实例.元素[0])
    
    def test_模块栈操作(self):
        """测试模块栈操作"""
        模块实例 = 模块(bytes())
        
        # 测试操作数栈
        模块实例.压入操作数(栈值(值类型.整数32, 42))
        模块实例.压入操作数(栈值(值类型.整数64, 100))
        
        self.assertEqual(模块实例.操作数栈指针, 1)
        
        值 = 模块实例.弹出操作数()
        self.assertEqual(值.值, 100)
        self.assertEqual(值.值类型, 值类型.整数64)
        
        值 = 模块实例.查看栈顶()
        self.assertEqual(值.值, 42)
    
    def test_全局变量(self):
        """测试全局变量"""
        全局 = 全局变量(值类型=值类型.整数32, 值=42, 可变=True)
        self.assertEqual(全局.值, 42)
        self.assertTrue(全局.可变)


class 测试枚举定义(unittest.TestCase):
    """测试枚举定义"""
    
    def test_操作码值(self):
        """测试操作码值"""
        self.assertEqual(操作码.不可达.value, 0x00)
        self.assertEqual(操作码.空操作.value, 0x01)
        self.assertEqual(操作码.块开始.value, 0x02)
        self.assertEqual(操作码.结束标记.value, 0x0B)
        self.assertEqual(操作码.常量32位.value, 0x41)
        self.assertEqual(操作码.常量64位.value, 0x42)
        self.assertEqual(操作码.加法32.value, 0x6A)
    
    def test_值类型(self):
        """测试值类型"""
        self.assertEqual(值类型.整数32.value, 0x7F)
        self.assertEqual(值类型.整数64.value, 0x7E)
        self.assertEqual(值类型.单精度浮点.value, 0x7D)
        self.assertEqual(值类型.双精度浮点.value, 0x7C)
        self.assertEqual(值类型.空类型.value, 0x40)


class 测试模块加载器(unittest.TestCase):
    """测试模块加载器"""
    
    def test_简单模块(self):
        """测试加载简单模块"""
        # 创建一个最小的有效Wasm模块
        # 魔数 + 版本 + 空段
        字节码 = bytes([
            0x00, 0x61, 0x73, 0x6D,  # magic: \0asm
            0x01, 0x00, 0x00, 0x00,  # version: 1
        ])
        
        加载器 = 模块加载器()
        模块实例 = 加载器.加载模块(字节码)
        
        self.assertIsNotNone(模块实例)
        self.assertEqual(len(模块实例.类型列表), 0)
        self.assertEqual(len(模块实例.函数列表), 0)
    
    def test_带类型段的模块(self):
        """测试带类型段的模块"""
        # 魔数 + 版本 + 类型段
        字节码 = bytes([
            0x00, 0x61, 0x73, 0x6D,  # magic
            0x01, 0x00, 0x00, 0x00,  # version
            0x01,  # 段类型: Type
            0x05,  # 段长度
            0x01,  # 类型数量: 1
            0x60,  # 函数类型
            0x00,  # 参数数量: 0
            0x01,  # 返回数量: 1
            0x7F,  # 返回类型: i32
        ])
        
        加载器 = 模块加载器()
        模块实例 = 加载器.加载模块(字节码)
        
        self.assertEqual(len(模块实例.类型列表), 1)
        self.assertEqual(len(模块实例.类型列表[0].参数类型), 0)
        self.assertEqual(len(模块实例.类型列表[0].返回类型), 1)
        self.assertEqual(模块实例.类型列表[0].返回类型[0], 值类型.整数32)


def 运行测试():
    """运行所有测试"""
    unittest.main(argv=[''], verbosity=2, exit=False)


if __name__ == '__main__':
    运行测试()
