"""
WebAssembly 命令行接口
对应原 cli.c 文件
"""
import sys
import os
import struct
from typing import List, Optional

from 模块加载器 import 模块加载器
from 解释器核心 import 解释器
from 数据结构 import 模块, 栈值
from 枚举定义 import 值类型
from 工具函数 import 值转字符串


class 命令行接口:
    """Wasm解释器命令行接口"""
    
    def __init__(self):
        self.加载器 = 模块加载器()
        self.当前模块: Optional[模块] = None
        self.解释器实例: Optional[解释器] = None
    
    def 运行(self, 参数列表: List[str]):
        """运行命令行接口"""
        if len(参数列表) < 2:
            self._显示帮助()
            return
        
        命令 = 参数列表[1]
        
        if 命令 in ['-h', '--help', 'help']:
            self._显示帮助()
        elif 命令 == 'run':
            if len(参数列表) < 3:
                print("错误: 缺少Wasm文件路径")
                return
            self._运行文件(参数列表[2])
        elif 命令 == 'repl':
            self._交互式循环()
        elif 命令 == 'dump':
            if len(参数列表) < 3:
                print("错误: 缺少Wasm文件路径")
                return
            self._转储模块(参数列表[2])
        elif 命令 == 'invoke':
            if len(参数列表) < 4:
                print("错误: 缺少函数名或文件路径")
                return
            self._调用函数(参数列表[2], 参数列表[3:])
        else:
            # 假设是文件路径
            self._运行文件(命令)
    
    def _显示帮助(self):
        """显示帮助信息"""
        print("""
WebAssembly Python 解释器

用法:
    python -m wasm解释器 <命令> [选项]

命令:
    run <文件>          运行Wasm文件
    repl                启动交互式解释器
    dump <文件>         转储Wasm模块信息
    invoke <文件> <函数> [参数...]  调用指定函数
    help                显示此帮助信息

示例:
    python -m wasm解释器 run example.wasm
    python -m wasm解释器 repl
    python -m wasm解释器 dump example.wasm
    python -m wasm解释器 invoke example.wasm add 1 2
""")
    
    def _运行文件(self, 文件路径: str):
        """运行Wasm文件"""
        if not os.path.exists(文件路径):
            print(f"错误: 文件不存在: {文件路径}")
            return
        
        try:
            with open(文件路径, 'rb') as 文件:
                字节码 = 文件.read()
            
            print(f"加载模块: {文件路径}")
            self.当前模块 = self.加载器.加载模块(字节码)
            
            # 显示模块信息
            self._显示模块信息()
            
            # 执行起始函数
            if self.当前模块.起始函数索引 is not None:
                print(f"\\n执行起始函数: {self.当前模块.起始函数索引}")
                self._执行函数(self.当前模块.起始函数索引)
            
            print("\\n执行完成")
            
        except Exception as 错误:
            print(f"错误: {错误}")
            import traceback
            traceback.print_exc()
    
    def _转储模块(self, 文件路径: str):
        """转储Wasm模块信息"""
        if not os.path.exists(文件路径):
            print(f"错误: 文件不存在: {文件路径}")
            return
        
        try:
            with open(文件路径, 'rb') as 文件:
                字节码 = 文件.read()
            
            print(f"\\n模块: {文件路径}")
            print(f"大小: {len(字节码)} 字节")
            print(f"魔数: {字节码[:4].hex()}")
            print(f"版本: {struct.unpack('<I', 字节码[4:8])[0]}")
            
            模块实例 = self.加载器.加载模块(字节码)
            self._显示模块详细信息(模块实例)
            
        except Exception as 错误:
            print(f"错误: {错误}")
            import traceback
            traceback.print_exc()
    
    def _调用函数(self, 文件路径: str, 参数列表: List[str]):
        """调用指定函数"""
        if not os.path.exists(文件路径):
            print(f"错误: 文件不存在: {文件路径}")
            return
        
        if len(参数列表) < 1:
            print("错误: 缺少函数名")
            return
        
        函数名 = 参数列表[0]
        传入参数 = 参数列表[1:]
        
        try:
            with open(文件路径, 'rb') as 文件:
                字节码 = 文件.read()
            
            self.当前模块 = self.加载器.加载模块(字节码)
            
            # 查找函数
            函数索引 = self._查找函数(函数名)
            if 函数索引 is None:
                print(f"错误: 找不到函数: {函数名}")
                return
            
            # 准备参数
            函数定义 = self.当前模块.函数列表[函数索引]
            函数类型 = self.当前模块.类型列表[函数定义.类型索引]
            
            if len(传入参数) != len(函数类型.参数类型):
                print(f"错误: 参数数量不匹配，期望 {len(函数类型.参数类型)}，实际 {len(传入参数)}")
                return
            
            # 转换参数
            for i, 参数值 in enumerate(传入参数):
                参数类型 = 函数类型.参数类型[i]
                转换值 = self._转换参数(参数值, 参数类型)
                self.当前模块.压入操作数(栈值(参数类型, 转换值))
            
            # 执行函数
            print(f"调用函数: {函数名}({', '.join(传入参数)})")
            self._执行函数(函数索引)
            
            # 显示返回值
            for i in range(len(函数类型.返回类型)):
                返回值 = self.当前模块.弹出操作数()
                print(f"返回值[{i}]: {值转字符串(返回值.值类型, 返回值.值)}")
            
        except Exception as 错误:
            print(f"错误: {错误}")
            import traceback
            traceback.print_exc()
    
    def _执行函数(self, 函数索引: int):
        """执行指定函数"""
        self.解释器实例 = 解释器(self.当前模块)
        self.解释器实例.设置调用(函数索引)
        self.解释器实例.解释()
    
    def _查找函数(self, 名称: str) -> Optional[int]:
        """根据名称查找函数索引"""
        # 首先检查导出
        for 导出 in self.当前模块.导出列表:
            if 导出.名称 == 名称 and 导出.导出类型 == 0:  # 函数导出
                return 导出.索引
        
        # 尝试解析为数字
        try:
            return int(名称)
        except ValueError:
            return None
    
    def _转换参数(self, 值: str, 类型: 值类型) -> int:
        """转换参数值"""
        if 类型 == 值类型.整数32:
            return int(值) & 0xFFFFFFFF
        elif 类型 == 值类型.整数64:
            return int(值) & 0xFFFFFFFFFFFFFFFF
        elif 类型 == 值类型.单精度浮点:
            浮点值 = float(值)
            return struct.unpack('<I', struct.pack('<f', 浮点值))[0]
        elif 类型 == 值类型.双精度浮点:
            浮点值 = float(值)
            return struct.unpack('<Q', struct.pack('<d', 浮点值))[0]
        else:
            return int(值)
    
    def _显示模块信息(self):
        """显示模块基本信息"""
        print(f"\\n模块信息:")
        print(f"  类型数量: {len(self.当前模块.类型列表)}")
        print(f"  函数数量: {len(self.当前模块.函数列表)}")
        print(f"  导入函数: {self.当前模块.导入函数数}")
        print(f"  全局变量: {len(self.当前模块.全局变量列表)}")
        print(f"  导出数量: {len(self.当前模块.导出列表)}")
        print(f"  数据段: {len(self.当前模块.数据段列表)}")
        print(f"  元素段: {len(self.当前模块.元素段列表)}")
        
        if self.当前模块.内存:
            print(f"  内存: {self.当前模块.内存.页数} 页")
        
        if self.当前模块.表:
            print(f"  表: {len(self.当前模块.表.元素)} 项")
    
    def _显示模块详细信息(self, 模块实例: 模块):
        """显示模块详细信息"""
        print(f"\\n类型段 ({len(模块实例.类型列表)} 项):")
        for i, 类型 in enumerate(模块实例.类型列表):
            参数 = ', '.join(str(t) for t in 类型.参数类型)
            返回 = ', '.join(str(t) for t in 类型.返回类型)
            print(f"  [{i}] ({参数}) -> ({返回})")
        
        print(f"\\n函数段 ({len(模块实例.函数列表)} 项):")
        for i, 函数 in enumerate(模块实例.函数列表):
            类型 = 模块实例.类型列表[函数.类型索引]
            参数 = ', '.join(str(t) for t in 类型.参数类型)
            返回 = ', '.join(str(t) for t in 类型.返回类型)
            print(f"  [{i}] 类型[{函数.类型索引}] ({参数}) -> ({返回})")
        
        print(f"\\n导出段 ({len(模块实例.导出列表)} 项):")
        for 导出 in 模块实例.导出列表:
            类型名 = ['函数', '表', '内存', '全局'][导出.导出类型]
            print(f"  {导出.名称}: {类型名}[{导出.索引}]")
        
        if 模块实例.全局变量列表:
            print(f"\\n全局段 ({len(模块实例.全局变量列表)} 项):")
            for i, 全局 in enumerate(模块实例.全局变量列表):
                可变性 = "可变" if 全局.可变 else "不可变"
                print(f"  [{i}] {全局.值类型} = {全局.值} ({可变性})")
    
    def _交互式循环(self):
        """交互式解释器循环"""
        print("""
WebAssembly Python 解释器 - 交互模式
输入 'help' 获取帮助，'quit' 退出
""")
        
        while True:
            try:
                输入 = input("wasm> ").strip()
                
                if not 输入:
                    continue
                
                部分 = 输入.split()
                命令 = 部分[0].lower()
                
                if 命令 in ['quit', 'exit', 'q']:
                    break
                elif 命令 == 'help':
                    self._显示交互帮助()
                elif 命令 == 'load':
                    if len(部分) < 2:
                        print("错误: 缺少文件路径")
                        continue
                    self._加载文件(部分[1])
                elif 命令 == 'run':
                    if self.当前模块 is None:
                        print("错误: 未加载模块")
                        continue
                    self._运行当前模块()
                elif 命令 == 'info':
                    if self.当前模块 is None:
                        print("错误: 未加载模块")
                        continue
                    self._显示模块信息()
                elif 命令 == 'call':
                    if self.当前模块 is None:
                        print("错误: 未加载模块")
                        continue
                    if len(部分) < 2:
                        print("错误: 缺少函数名")
                        continue
                    self._交互式调用(部分[1], 部分[2:])
                else:
                    print(f"未知命令: {命令}")
                    
            except KeyboardInterrupt:
                print()
                break
            except EOFError:
                break
            except Exception as 错误:
                print(f"错误: {错误}")
    
    def _显示交互帮助(self):
        """显示交互式帮助"""
        print("""
可用命令:
    load <文件>         加载Wasm文件
    run                 运行当前模块
    info                显示模块信息
    call <函数> [参数]  调用函数
    help                显示此帮助
    quit                退出
""")
    
    def _加载文件(self, 文件路径: str):
        """加载文件"""
        try:
            with open(文件路径, 'rb') as 文件:
                字节码 = 文件.read()
            self.当前模块 = self.加载器.加载模块(字节码)
            print(f"已加载: {文件路径}")
            self._显示模块信息()
        except Exception as 错误:
            print(f"加载失败: {错误}")
    
    def _运行当前模块(self):
        """运行当前模块"""
        try:
            if self.当前模块.起始函数索引 is not None:
                self._执行函数(self.当前模块.起始函数索引)
            print("执行完成")
        except Exception as 错误:
            print(f"执行失败: {错误}")
    
    def _交互式调用(self, 函数名: str, 参数列表: List[str]):
        """交互式调用函数"""
        try:
            函数索引 = self._查找函数(函数名)
            if 函数索引 is None:
                print(f"错误: 找不到函数: {函数名}")
                return
            
            # 准备参数
            函数定义 = self.当前模块.函数列表[函数索引]
            函数类型 = self.当前模块.类型列表[函数定义.类型索引]
            
            if len(参数列表) != len(函数类型.参数类型):
                print(f"错误: 参数数量不匹配")
                return
            
            for i, 参数值 in enumerate(参数列表):
                参数类型 = 函数类型.参数类型[i]
                转换值 = self._转换参数(参数值, 参数类型)
                self.当前模块.压入操作数(栈值(参数类型, 转换值))
            
            self._执行函数(函数索引)
            
            # 显示返回值
            for i in range(len(函数类型.返回类型)):
                返回值 = self.当前模块.弹出操作数()
                print(f"返回值[{i}]: {值转字符串(返回值.值类型, 返回值.值)}")
                
        except Exception as 错误:
            print(f"调用失败: {错误}")


def 主函数():
    """主入口"""
    接口 = 命令行接口()
    接口.run(sys.argv)


if __name__ == '__main__':
    主函数()
