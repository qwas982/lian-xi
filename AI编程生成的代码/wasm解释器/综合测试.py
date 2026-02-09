"""
WebAssembly Python 解释器 - 综合测试
使用Wasm汇编器编写全面的测试用例
"""
import unittest
from 模块加载器 import 模块加载器
from 解释器核心 import 解释器
from 数据结构 import 栈值
from 枚举定义 import 值类型
from wasm汇编器 import 汇编


class 综合测试(unittest.TestCase):
    """综合功能测试"""
    
    def _执行函数(self, 源码: str, 函数索引: int = 0, 参数: list = None):
        """汇编并执行函数"""
        字节码 = 汇编(源码)
        加载器 = 模块加载器()
        模块实例 = 加载器.加载模块(字节码)
        
        解释器实例 = 解释器(模块实例)
        
        # 压入参数
        if 参数:
            for 参数值 in 参数:
                模块实例.压入操作数(栈值(值类型.整数32, 参数值 & 0xFFFFFFFF))
        
        # 设置调用并执行
        解释器实例.设置调用(函数索引)
        解释器实例.解释()
        
        # 返回结果
        return 模块实例.弹出操作数()
    
    # ========== 算术运算测试 ==========
    
    def test_加法(self):
        """测试加法"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 10
                i32.const 20
                i32.add
            )
            (export "add" (func 0))
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 30)
    
    def test_减法(self):
        """测试减法"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 50
                i32.const 20
                i32.sub
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 30)
    
    def test_乘法(self):
        """测试乘法"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 6
                i32.const 7
                i32.mul
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    def test_除法(self):
        """测试除法"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 100
                i32.const 4
                i32.div_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 25)
    
    def test_取余(self):
        """测试取余"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 17
                i32.const 5
                i32.rem_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 2)
    
    # ========== 位运算测试 ==========
    
    def test_与运算(self):
        """测试按位与"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 0b1100
                i32.const 0b1010
                i32.and
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 0b1000)
    
    def test_或运算(self):
        """测试按位或"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 0b1100
                i32.const 0b1010
                i32.or
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 0b1110)
    
    def test_异或运算(self):
        """测试按位异或"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 0b1100
                i32.const 0b1010
                i32.xor
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 0b0110)
    
    def test_左移(self):
        """测试左移"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 1
                i32.const 5
                i32.shl
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 32)
    
    def test_右移(self):
        """测试右移"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 32
                i32.const 3
                i32.shr_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 4)
    
    # ========== 比较运算测试 ==========
    
    def test_等于(self):
        """测试等于"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 42
                i32.const 42
                i32.eq
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 1)
    
    def test_不等于(self):
        """测试不等于"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 42
                i32.const 24
                i32.ne
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 1)
    
    def test_小于(self):
        """测试小于"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 10
                i32.const 20
                i32.lt_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 1)
    
    def test_大于(self):
        """测试大于"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 30
                i32.const 20
                i32.gt_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 1)
    
    def test_等于零(self):
        """测试等于零"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 0
                i32.eqz
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 1)
    
    # ========== 局部变量测试 ==========
    
    def test_局部变量读写(self):
        """测试局部变量读写"""
        源码 = '''
        (module
            (func (result i32)
                (local i32)
                i32.const 42
                local.set 0
                local.get 0
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    def test_局部变量运算(self):
        """测试局部变量参与运算"""
        源码 = '''
        (module
            (func (result i32)
                (local i32)
                i32.const 10
                local.set 0
                local.get 0
                i32.const 5
                i32.add
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 15)
    
    def test_local_tee(self):
        """测试local.tee"""
        源码 = '''
        (module
            (func (result i32)
                (local i32)
                i32.const 42
                local.tee 0
                i32.const 8
                i32.add
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 50)
    
    # ========== 控制流测试 ==========
    
    def test_空块(self):
        """测试空块"""
        源码 = '''
        (module
            (func (result i32)
                block
                    i32.const 42
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    def test_带返回值的块(self):
        """测试带返回值的块"""
        源码 = '''
        (module
            (func (result i32)
                block (result i32)
                    i32.const 100
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 100)
    
    def test_if_then(self):
        """测试if-then"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 1
                if (result i32)
                    i32.const 42
                else
                    i32.const 0
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    def test_if_else(self):
        """测试if-else"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 0
                if (result i32)
                    i32.const 42
                else
                    i32.const 100
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 100)
    
    def test_嵌套块(self):
        """测试嵌套块"""
        源码 = '''
        (module
            (func (result i32)
                block (result i32)
                    block (result i32)
                        i32.const 42
                    end
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    # ========== 栈操作测试 ==========
    
    def test_drop(self):
        """测试drop"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 10
                i32.const 20
                drop
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 10)
    
    def test_select_true(self):
        """测试select（条件为真）"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 42
                i32.const 100
                i32.const 1
                select
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 42)
    
    def test_select_false(self):
        """测试select（条件为假）"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 42
                i32.const 100
                i32.const 0
                select
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 100)
    
    # ========== 复杂表达式测试 ==========
    
    def test_复杂算术表达式(self):
        """测试复杂算术表达式: (10 + 20) * (30 - 15) / 5 = 90"""
        源码 = '''
        (module
            (func (result i32)
                ;; 10 + 20 = 30
                i32.const 10
                i32.const 20
                i32.add
                ;; 30 - 15 = 15
                i32.const 30
                i32.const 15
                i32.sub
                ;; 30 * 15 = 450
                i32.mul
                ;; 450 / 5 = 90
                i32.const 5
                i32.div_s
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 90)
    
    def test_条件表达式(self):
        """测试条件表达式"""
        源码 = '''
        (module
            (func (result i32)
                ;; max(30, 20)
                i32.const 30
                i32.const 20
                i32.gt_s
                if (result i32)
                    i32.const 30
                else
                    i32.const 20
                end
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 30)
    
    def test_阶乘计算(self):
        """测试阶乘计算: 5! = 120"""
        源码 = '''
        (module
            (func (result i32)
                (local i32)      ;; 结果
                (local i32)      ;; 计数器
                
                ;; 初始化
                i32.const 1
                local.set 0      ;; 结果 = 1
                i32.const 5
                local.set 1      ;; 计数器 = 5
                
                ;; 计算阶乘
                block
                    loop
                        ;; 检查计数器是否为0
                        local.get 1
                        i32.eqz
                        br_if 1      ;; 如果计数器为0，退出循环
                        
                        ;; 结果 *= 计数器
                        local.get 0
                        local.get 1
                        i32.mul
                        local.set 0
                        
                        ;; 计数器--
                        local.get 1
                        i32.const 1
                        i32.sub
                        local.set 1
                        
                        br 0         ;; 继续循环
                    end
                end
                
                local.get 0
            )
        )
        '''
        结果 = self._执行函数(源码)
        self.assertEqual(结果.值, 120)
    
    # ========== 多函数测试 ==========
    
    def test_多函数调用(self):
        """测试多个函数"""
        源码 = '''
        (module
            (func (result i32)
                i32.const 10
            )
            (func (result i32)
                i32.const 20
            )
        )
        '''
        字节码 = 汇编(源码)
        加载器 = 模块加载器()
        模块实例 = 加载器.加载模块(字节码)
        
        # 测试函数0
        解释器实例 = 解释器(模块实例)
        解释器实例.设置调用(0)
        解释器实例.解释()
        结果0 = 模块实例.弹出操作数()
        self.assertEqual(结果0.值, 10)
        
        # 测试函数1
        解释器实例 = 解释器(模块实例)
        解释器实例.设置调用(1)
        解释器实例.解释()
        结果1 = 模块实例.弹出操作数()
        self.assertEqual(结果1.值, 20)


def 运行综合测试():
    """运行综合测试"""
    unittest.main(argv=[''], verbosity=2, exit=False)


if __name__ == '__main__':
    运行综合测试()
