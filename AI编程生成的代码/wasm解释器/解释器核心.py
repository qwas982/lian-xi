"""
WebAssembly 解释器核心
对应原 interpreter.c 文件
"""
import struct
import math
from typing import Optional, List, Tuple

from 枚举定义 import 操作码, 值类型, 块类型
from 数据结构 import 模块, 栈值, 栈帧, 控制块
from 工具函数 import (
    解码无符号32位, 解码有符号32位, 解码有符号64位,
    单精度转32位整数, 双精度转32位整数,
    单精度转64位整数, 双精度转64位整数,
    截断饱和32位有符号, 截断饱和32位无符号,
    截断饱和64位有符号, 截断饱和64位无符号,
    复制浮点符号, 浮点最小值, 浮点最大值,
    获取块类型结果, 异常消息
)


class 解释器:
    """WebAssembly 解释器"""
    
    def __init__(self, 模块实例: 模块):
        self.模块 = 模块实例
        self.运行中 = False
    
    def 压入块(self, 块类型值: int, 起始地址: int, 结束地址: int, 
               否则地址: Optional[int] = None, 是循环: bool = False):
        """压入控制块"""
        返回类型列表 = 获取块类型结果(块类型值)
        返回类型 = 返回类型列表[0] if 返回类型列表 else None
        
        # 块类型可以是 0x40 (空) 或值类型 (0x7F, 0x7E, 0x7D, 0x7C)
        if 块类型值 == 0x40:
            块类型枚举 = 块类型.空块
        else:
            块类型枚举 = 块类型.空块  # 对于值类型，我们使用空块但设置返回类型
        
        块 = 控制块(
            块类型=块类型枚举,
            返回类型=返回类型,
            起始地址=起始地址,
            结束地址=结束地址,
            否则地址=否则地址,
            是循环=是循环
        )
        self.模块.压入块(块)
    
    def 弹出块(self) -> 控制块:
        """弹出控制块"""
        return self.模块.弹出块()
    
    def 设置调用(self, 函数索引: int):
        """设置函数调用"""
        函数定义 = self.模块.函数列表[函数索引]
        函数类型 = self.模块.类型列表[函数定义.类型索引]
        
        # 计算局部变量基址
        局部变量基址 = self.模块.操作数栈指针 - len(函数类型.参数类型) + 1
        
        # 创建新帧
        帧 = 栈帧(
            函数索引=函数索引,
            返回地址=self.模块.程序计数器,
            局部变量基址=局部变量基址,
            操作数栈基址=局部变量基址,
            块栈基址=self.模块.块栈指针 + 1
        )
        self.模块.压入帧(帧)
        
        # 设置程序计数器到函数代码起始
        self.模块.程序计数器 = 函数定义.代码起始
        
        # 为局部变量分配空间（已经由参数占用，只需添加额外局部变量）
        for _ in 函数定义.局部变量类型:
            self.模块.压入操作数(栈值(值类型.整数32, 0))
    
    def 执行返回(self):
        """执行返回"""
        # 获取当前帧
        帧 = self.模块.弹出帧()
        
        # 恢复操作数栈
        函数定义 = self.模块.函数列表[帧.函数索引]
        函数类型 = self.模块.类型列表[函数定义.类型索引]
        
        # 保存返回值
        返回值列表 = []
        for _ in range(len(函数类型.返回类型)):
            返回值列表.insert(0, self.模块.弹出操作数())
        
        # 恢复栈指针
        self.模块.操作数栈指针 = 帧.操作数栈基址 - 1
        
        # 压入返回值
        for 返回值 in 返回值列表:
            self.模块.压入操作数(返回值)
        
        # 恢复程序计数器
        self.模块.程序计数器 = 帧.返回地址
    
    def 执行分支(self, 标签索引: int):
        """执行分支"""
        # 弹出块直到目标块
        for _ in range(标签索引):
            self.弹出块()
        
        目标块 = self.模块.块栈[self.模块.块栈指针]
        
        if 目标块.是循环:
            # 循环：跳转到循环开始
            self.模块.程序计数器 = 目标块.起始地址
        else:
            # 块：跳转到块结束
            self.模块.程序计数器 = 目标块.结束地址
    
    def 解释(self) -> bool:
        """解释执行主循环"""
        self.运行中 = True
        
        while self.运行中 and self.模块.程序计数器 < self.模块.字节数:
            当前程序计数器 = self.模块.程序计数器
            操作码值 = self.模块.读取字节()
            
            try:
                操作码枚举 = 操作码(操作码值)
            except ValueError:
                raise ValueError(f"{异常消息.无效操作码}: {操作码值}")
            
            # 控制流指令
            if 操作码枚举 == 操作码.不可达:
                raise RuntimeError(异常消息.不可达代码)
            
            elif 操作码枚举 == 操作码.空操作:
                pass
            
            elif 操作码枚举 == 操作码.块开始:
                块类型值 = self.模块.读取字节()
                self.压入块(块类型值, 当前程序计数器, 0)
            
            elif 操作码枚举 == 操作码.循环开始:
                块类型值 = self.模块.读取字节()
                self.压入块(块类型值, 当前程序计数器, 0, 是循环=True)
            
            elif 操作码枚举 == 操作码.条件分支:
                块类型值 = self.模块.读取字节()
                条件 = self.模块.弹出操作数()
                self.压入块(块类型值, 当前程序计数器, 0)
                if 条件.值 == 0:
                    # 跳过到else或end
                    深度 = 1
                    while 深度 > 0 and self.模块.程序计数器 < self.模块.字节数:
                        字节 = self.模块.读取字节()
                        if 字节 == 0x04:  # if
                            深度 += 1
                        elif 字节 == 0x05:  # else
                            if 深度 == 1:
                                break
                        elif 字节 == 0x0B:  # end
                            深度 -= 1
                    
                    if 字节 == 0x05:  # else
                        self.模块.块栈[self.模块.块栈指针].否则地址 = self.模块.程序计数器
            
            elif 操作码枚举 == 操作码.否则分支:
                # 跳过到end
                深度 = 1
                while 深度 > 0 and self.模块.程序计数器 < self.模块.字节数:
                    字节 = self.模块.读取字节()
                    if 字节 in [0x02, 0x03, 0x04]:  # block, loop, if
                        深度 += 1
                    elif 字节 == 0x0B:  # end
                        深度 -= 1
            
            elif 操作码枚举 == 操作码.结束标记:
                if self.模块.块栈指针 >= 0:
                    self.弹出块()
                else:
                    # 函数结束
                    if self.模块.帧指针 >= 0:
                        self.执行返回()
                        # 如果调用栈为空，停止执行
                        if self.模块.帧指针 < 0:
                            self.运行中 = False
                    else:
                        self.运行中 = False
            
            elif 操作码枚举 == 操作码.分支:
                标签索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                self.执行分支(标签索引)
            
            elif 操作码枚举 == 操作码.条件分支表:
                # br_table 指令
                标签数, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                标签列表 = []
                for _ in range(标签数):
                    标签, self.模块.程序计数器 = 解码无符号32位(
                        self.模块.字节码, self.模块.程序计数器
                    )
                    标签列表.append(标签)
                默认标签, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                
                索引 = self.模块.弹出操作数().值
                if 索引 < len(标签列表):
                    self.执行分支(标签列表[索引])
                else:
                    self.执行分支(默认标签)
            
            elif 操作码枚举 == 操作码.返回:
                self.执行返回()
            
            # 参数指令
            elif 操作码枚举 == 操作码.丢弃:
                self.模块.弹出操作数()
            
            elif 操作码枚举 == 操作码.选择:
                条件 = self.模块.弹出操作数()
                值2 = self.模块.弹出操作数()
                值1 = self.模块.弹出操作数()
                if 条件.值 != 0:
                    self.模块.压入操作数(值1)
                else:
                    self.模块.压入操作数(值2)
            
            # 变量访问指令
            elif 操作码枚举 == 操作码.局部变量读取:
                局部索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                帧 = self.模块.调用栈[self.模块.帧指针]
                值 = self.模块.操作数栈[帧.局部变量基址 + 局部索引]
                self.模块.压入操作数(值)
            
            elif 操作码枚举 == 操作码.局部变量写入:
                局部索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                值 = self.模块.弹出操作数()
                帧 = self.模块.调用栈[self.模块.帧指针]
                self.模块.操作数栈[帧.局部变量基址 + 局部索引] = 值
            
            elif 操作码枚举 == 操作码.局部变量读取32:
                # local.tee - 设置局部变量但不弹出栈
                局部索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                值 = self.模块.查看栈顶()  # 查看但不弹出
                帧 = self.模块.调用栈[self.模块.帧指针]
                self.模块.操作数栈[帧.局部变量基址 + 局部索引] = 值
            
            elif 操作码枚举 == 操作码.全局变量读取:
                全局索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                全局变量 = self.模块.全局变量列表[全局索引]
                self.模块.压入操作数(栈值(全局变量.值类型, 全局变量.值))
            
            elif 操作码枚举 == 操作码.全局变量写入:
                全局索引, self.模块.程序计数器 = 解码无符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                值 = self.模块.弹出操作数()
                self.模块.全局变量列表[全局索引].值 = 值.值
            
            # 内存指令
            elif 操作码枚举 in [
                操作码.内存读取32位, 操作码.内存读取64位,
                操作码.内存读取单精度浮点, 操作码.内存读取双精度浮点,
                操作码.内存读取8位有符号扩展32, 操作码.内存读取8位无符号扩展32,
                操作码.内存读取16位有符号扩展32, 操作码.内存读取16位无符号扩展32,
                操作码.内存读取8位有符号扩展64, 操作码.内存读取8位无符号扩展64,
                操作码.内存读取16位有符号扩展64, 操作码.内存读取16位无符号扩展64,
                操作码.内存读取32位有符号扩展64, 操作码.内存读取32位无符号扩展64
            ]:
                self._执行内存读取(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.内存写入32位, 操作码.内存写入64位,
                操作码.内存写入单精度浮点, 操作码.内存写入双精度浮点,
                操作码.内存写入8位32, 操作码.内存写入16位32,
                操作码.内存写入8位64, 操作码.内存写入16位64, 操作码.内存写入32位64
            ]:
                self._执行内存写入(操作码枚举)
            
            elif 操作码枚举 == 操作码.内存大小:
                self.模块.读取字节()  # 0x00
                if self.模块.内存:
                    self.模块.压入操作数(栈值(值类型.整数32, self.模块.内存.页数))
                else:
                    self.模块.压入操作数(栈值(值类型.整数32, 0))
            
            elif 操作码枚举 == 操作码.内存增长:
                self.模块.读取字节()  # 0x00
                增量 = self.模块.弹出操作数().值
                if self.模块.内存:
                    结果 = self.模块.内存.增长(增量)
                    self.模块.压入操作数(栈值(值类型.整数32, 结果))
                else:
                    self.模块.压入操作数(栈值(值类型.整数32, -1))
            
            # 常量指令
            elif 操作码枚举 == 操作码.常量32位:
                值, self.模块.程序计数器 = 解码有符号32位(
                    self.模块.字节码, self.模块.程序计数器
                )
                self.模块.压入操作数(栈值(值类型.整数32, 值 & 0xFFFFFFFF))
            
            elif 操作码枚举 == 操作码.常量64位:
                值, self.模块.程序计数器 = 解码有符号64位(
                    self.模块.字节码, self.模块.程序计数器
                )
                self.模块.压入操作数(栈值(值类型.整数64, 值 & 0xFFFFFFFFFFFFFFFF))
            
            elif 操作码枚举 == 操作码.常量单精度浮点:
                值 = self.模块.读取单精度浮点()
                位表示 = struct.unpack('<I', struct.pack('<f', 值))[0]
                self.模块.压入操作数(栈值(值类型.单精度浮点, 位表示))
            
            elif 操作码枚举 == 操作码.常量双精度浮点:
                值 = self.模块.读取双精度浮点()
                位表示 = struct.unpack('<Q', struct.pack('<d', 值))[0]
                self.模块.压入操作数(栈值(值类型.双精度浮点, 位表示))
            
            # 整数比较指令
            elif 操作码枚举 in [
                操作码.等于零判断32, 操作码.等于判断32, 操作码.不等于判断32,
                操作码.小于有符号32, 操作码.小于无符号32,
                操作码.大于有符号32, 操作码.大于无符号32,
                操作码.小于等于有符号32, 操作码.小于等于无符号32,
                操作码.大于等于有符号32, 操作码.大于等于无符号32
            ]:
                self._执行整数32比较(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.等于零判断64, 操作码.等于判断64, 操作码.不等于判断64,
                操作码.小于有符号64, 操作码.小于无符号64,
                操作码.大于有符号64, 操作码.大于无符号64,
                操作码.小于等于有符号64, 操作码.小于等于无符号64,
                操作码.大于等于有符号64, 操作码.大于等于无符号64
            ]:
                self._执行整数64比较(操作码枚举)
            
            # 整数运算指令
            elif 操作码枚举 in [
                操作码.加法32, 操作码.减法32, 操作码.乘法32,
                操作码.除法有符号32, 操作码.除法无符号32,
                操作码.取余有符号32, 操作码.取余无符号32,
                操作码.与运算32, 操作码.或运算32, 操作码.异或运算32,
                操作码.左移32, 操作码.右移有符号32, 操作码.右移无符号32,
                操作码.循环左移32, 操作码.循环右移32
            ]:
                self._执行整数32运算(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.加法64, 操作码.减法64, 操作码.乘法64,
                操作码.除法有符号64, 操作码.除法无符号64,
                操作码.取余有符号64, 操作码.取余无符号64,
                操作码.与运算64, 操作码.或运算64, 操作码.异或运算64,
                操作码.左移64, 操作码.右移有符号64, 操作码.右移无符号64,
                操作码.循环左移64, 操作码.循环右移64
            ]:
                self._执行整数64运算(操作码枚举)
            
            # 浮点比较指令
            elif 操作码枚举 in [
                操作码.等于判断单精度, 操作码.不等于判断单精度,
                操作码.小于单精度, 操作码.大于单精度,
                操作码.小于等于单精度, 操作码.大于等于单精度
            ]:
                self._执行浮点32比较(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.等于判断双精度, 操作码.不等于判断双精度,
                操作码.小于双精度, 操作码.大于双精度,
                操作码.小于等于双精度, 操作码.大于等于双精度
            ]:
                self._执行浮点64比较(操作码枚举)
            
            # 浮点运算指令
            elif 操作码枚举 in [
                操作码.取绝对值单精度, 操作码.取负单精度,
                操作码.向上取整单精度, 操作码.向下取整单精度,
                操作码.向零取整单精度, 操作码.平方根单精度
            ]:
                self._执行浮点32一元运算(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.取绝对值双精度, 操作码.取负双精度,
                操作码.向上取整双精度, 操作码.向下取整双精度,
                操作码.向零取整双精度, 操作码.平方根双精度
            ]:
                self._执行浮点64一元运算(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.加法单精度, 操作码.减法单精度,
                操作码.乘法单精度, 操作码.除法单精度,
                操作码.取最小值单精度, 操作码.取最大值单精度,
                操作码.复制符号单精度
            ]:
                self._执行浮点32二元运算(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.加法双精度, 操作码.减法双精度,
                操作码.乘法双精度, 操作码.除法双精度,
                操作码.取最小值双精度, 操作码.取最大值双精度,
                操作码.复制符号双精度
            ]:
                self._执行浮点64二元运算(操作码枚举)
            
            # 类型转换指令
            elif 操作码枚举 in [
                操作码.截断32位有符号, 操作码.截断32位无符号,
                操作码.截断64位有符号32, 操作码.截断64位无符号32,
                操作码.浮点转32位有符号, 操作码.浮点转32位无符号,
                操作码.双精度转32位有符号, 操作码.双精度转32位无符号,
                操作码.截断饱和32位有符号, 操作码.截断饱和32位无符号
            ]:
                self._执行类型转换到32位(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.扩展有符号64, 操作码.扩展无符号64,
                操作码.截断64位有符号64, 操作码.截断64位无符号64,
                操作码.浮点转64位有符号, 操作码.浮点转64位无符号,
                操作码.双精度转64位有符号, 操作码.双精度转64位无符号,
                操作码.截断饱和64位有符号, 操作码.截断饱和64位无符号
            ]:
                self._执行类型转换到64位(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.转换32位有符号单精度, 操作码.转换32位无符号单精度,
                操作码.转换64位有符号单精度, 操作码.转换64位无符号单精度,
                操作码.双精度转单精度
            ]:
                self._执行类型转换到单精度(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.转换32位有符号双精度, 操作码.转换32位无符号双精度,
                操作码.转换64位有符号双精度, 操作码.转换64位无符号双精度,
                操作码.单精度转双精度
            ]:
                self._执行类型转换到双精度(操作码枚举)
            
            # 位运算指令
            elif 操作码枚举 in [
                操作码.位reinterpret32, 操作码.位reinterpret64
            ]:
                # reinterpret：位模式不变，只是类型解释不同
                pass  # Python中不需要额外操作
            
            elif 操作码枚举 in [
                操作码.扩展8位有符号32, 操作码.扩展16位有符号32
            ]:
                self._执行符号扩展32(操作码枚举)
            
            elif 操作码枚举 in [
                操作码.扩展8位有符号64, 操作码.扩展16位有符号64,
                操作码.扩展32位有符号64
            ]:
                self._执行符号扩展64(操作码枚举)
            
            else:
                raise ValueError(f"未实现的操作码: {操作码枚举}")
        
        return True
    
    def _执行内存读取(self, 操作码枚举: 操作码):
        """执行内存读取指令"""
        对齐, self.模块.程序计数器 = 解码无符号32位(
            self.模块.字节码, self.模块.程序计数器
        )
        偏移, self.模块.程序计数器 = 解码无符号32位(
            self.模块.字节码, self.模块.程序计数器
        )
        
        地址 = self.模块.弹出操作数().值 + 偏移
        
        if self.模块.内存 is None:
            raise RuntimeError(异常消息.内存访问越界)
        
        if 操作码枚举 == 操作码.内存读取32位:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<I', bytes(self.模块.内存.数据[地址:地址+4]))[0]
            self.模块.压入操作数(栈值(值类型.整数32, 值))
        
        elif 操作码枚举 == 操作码.内存读取64位:
            if 地址 + 8 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<Q', bytes(self.模块.内存.数据[地址:地址+8]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值))
        
        elif 操作码枚举 == 操作码.内存读取单精度浮点:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            浮点值 = struct.unpack('<f', bytes(self.模块.内存.数据[地址:地址+4]))[0]
            位表示 = struct.unpack('<I', struct.pack('<f', 浮点值))[0]
            self.模块.压入操作数(栈值(值类型.单精度浮点, 位表示))
        
        elif 操作码枚举 == 操作码.内存读取双精度浮点:
            if 地址 + 8 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            浮点值 = struct.unpack('<d', bytes(self.模块.内存.数据[地址:地址+8]))[0]
            位表示 = struct.unpack('<Q', struct.pack('<d', 浮点值))[0]
            self.模块.压入操作数(栈值(值类型.双精度浮点, 位表示))
        
        elif 操作码枚举 == 操作码.内存读取8位有符号扩展32:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<b', bytes(self.模块.内存.数据[地址:地址+1]))[0]
            self.模块.压入操作数(栈值(值类型.整数32, 值 & 0xFFFFFFFF))
        
        elif 操作码枚举 == 操作码.内存读取8位无符号扩展32:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = self.模块.内存.数据[地址]
            self.模块.压入操作数(栈值(值类型.整数32, 值))
        
        elif 操作码枚举 == 操作码.内存读取16位有符号扩展32:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<h', bytes(self.模块.内存.数据[地址:地址+2]))[0]
            self.模块.压入操作数(栈值(值类型.整数32, 值 & 0xFFFFFFFF))
        
        elif 操作码枚举 == 操作码.内存读取16位无符号扩展32:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<H', bytes(self.模块.内存.数据[地址:地址+2]))[0]
            self.模块.压入操作数(栈值(值类型.整数32, 值))
        
        elif 操作码枚举 == 操作码.内存读取8位有符号扩展64:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<b', bytes(self.模块.内存.数据[地址:地址+1]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值 & 0xFFFFFFFFFFFFFFFF))
        
        elif 操作码枚举 == 操作码.内存读取8位无符号扩展64:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = self.模块.内存.数据[地址]
            self.模块.压入操作数(栈值(值类型.整数64, 值))
        
        elif 操作码枚举 == 操作码.内存读取16位有符号扩展64:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<h', bytes(self.模块.内存.数据[地址:地址+2]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值 & 0xFFFFFFFFFFFFFFFF))
        
        elif 操作码枚举 == 操作码.内存读取16位无符号扩展64:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<H', bytes(self.模块.内存.数据[地址:地址+2]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值))
        
        elif 操作码枚举 == 操作码.内存读取32位有符号扩展64:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<i', bytes(self.模块.内存.数据[地址:地址+4]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值 & 0xFFFFFFFFFFFFFFFF))
        
        elif 操作码枚举 == 操作码.内存读取32位无符号扩展64:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            值 = struct.unpack('<I', bytes(self.模块.内存.数据[地址:地址+4]))[0]
            self.模块.压入操作数(栈值(值类型.整数64, 值))
    
    def _执行内存写入(self, 操作码枚举: 操作码):
        """执行内存写入指令"""
        对齐, self.模块.程序计数器 = 解码无符号32位(
            self.模块.字节码, self.模块.程序计数器
        )
        偏移, self.模块.程序计数器 = 解码无符号32位(
            self.模块.字节码, self.模块.程序计数器
        )
        
        值 = self.模块.弹出操作数().值
        地址 = self.模块.弹出操作数().值 + 偏移
        
        if self.模块.内存 is None:
            raise RuntimeError(异常消息.内存访问越界)
        
        if 操作码枚举 == 操作码.内存写入32位:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址:地址+4] = struct.pack('<I', 值 & 0xFFFFFFFF)
        
        elif 操作码枚举 == 操作码.内存写入64位:
            if 地址 + 8 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址:地址+8] = struct.pack('<Q', 值 & 0xFFFFFFFFFFFFFFFF)
        
        elif 操作码枚举 == 操作码.内存写入单精度浮点:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            浮点值 = struct.unpack('<f', struct.pack('<I', 值 & 0xFFFFFFFF))[0]
            self.模块.内存.数据[地址:地址+4] = struct.pack('<f', 浮点值)
        
        elif 操作码枚举 == 操作码.内存写入双精度浮点:
            if 地址 + 8 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            浮点值 = struct.unpack('<d', struct.pack('<Q', 值 & 0xFFFFFFFFFFFFFFFF))[0]
            self.模块.内存.数据[地址:地址+8] = struct.pack('<d', 浮点值)
        
        elif 操作码枚举 == 操作码.内存写入8位32:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址] = 值 & 0xFF
        
        elif 操作码枚举 == 操作码.内存写入16位32:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址:地址+2] = struct.pack('<H', 值 & 0xFFFF)
        
        elif 操作码枚举 == 操作码.内存写入8位64:
            if 地址 + 1 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址] = 值 & 0xFF
        
        elif 操作码枚举 == 操作码.内存写入16位64:
            if 地址 + 2 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址:地址+2] = struct.pack('<H', 值 & 0xFFFF)
        
        elif 操作码枚举 == 操作码.内存写入32位64:
            if 地址 + 4 > len(self.模块.内存.数据):
                raise RuntimeError(异常消息.内存访问越界)
            self.模块.内存.数据[地址:地址+4] = struct.pack('<I', 值 & 0xFFFFFFFF)
    
    def _执行整数32比较(self, 操作码枚举: 操作码):
        """执行32位整数比较指令"""
        if 操作码枚举 == 操作码.等于零判断32:
            值 = self.模块.弹出操作数().值
            有符号值 = struct.unpack('<i', struct.pack('<I', 值 & 0xFFFFFFFF))[0]
            结果 = 1 if 有符号值 == 0 else 0
        else:
            右值 = self.模块.弹出操作数().值
            左值 = self.模块.弹出操作数().值
            有符号左 = struct.unpack('<i', struct.pack('<I', 左值 & 0xFFFFFFFF))[0]
            有符号右 = struct.unpack('<i', struct.pack('<I', 右值 & 0xFFFFFFFF))[0]
            无符号左 = 左值 & 0xFFFFFFFF
            无符号右 = 右值 & 0xFFFFFFFF
            
            if 操作码枚举 == 操作码.等于判断32:
                结果 = 1 if 有符号左 == 有符号右 else 0
            elif 操作码枚举 == 操作码.不等于判断32:
                结果 = 1 if 有符号左 != 有符号右 else 0
            elif 操作码枚举 == 操作码.小于有符号32:
                结果 = 1 if 有符号左 < 有符号右 else 0
            elif 操作码枚举 == 操作码.小于无符号32:
                结果 = 1 if 无符号左 < 无符号右 else 0
            elif 操作码枚举 == 操作码.大于有符号32:
                结果 = 1 if 有符号左 > 有符号右 else 0
            elif 操作码枚举 == 操作码.大于无符号32:
                结果 = 1 if 无符号左 > 无符号右 else 0
            elif 操作码枚举 == 操作码.小于等于有符号32:
                结果 = 1 if 有符号左 <= 有符号右 else 0
            elif 操作码枚举 == 操作码.小于等于无符号32:
                结果 = 1 if 无符号左 <= 无符号右 else 0
            elif 操作码枚举 == 操作码.大于等于有符号32:
                结果 = 1 if 有符号左 >= 有符号右 else 0
            elif 操作码枚举 == 操作码.大于等于无符号32:
                结果 = 1 if 无符号左 >= 无符号右 else 0
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行整数64比较(self, 操作码枚举: 操作码):
        """执行64位整数比较指令"""
        if 操作码枚举 == 操作码.等于零判断64:
            值 = self.模块.弹出操作数().值
            有符号值 = struct.unpack('<q', struct.pack('<Q', 值 & 0xFFFFFFFFFFFFFFFF))[0]
            结果 = 1 if 有符号值 == 0 else 0
        else:
            右值 = self.模块.弹出操作数().值
            左值 = self.模块.弹出操作数().值
            有符号左 = struct.unpack('<q', struct.pack('<Q', 左值 & 0xFFFFFFFFFFFFFFFF))[0]
            有符号右 = struct.unpack('<q', struct.pack('<Q', 右值 & 0xFFFFFFFFFFFFFFFF))[0]
            无符号左 = 左值 & 0xFFFFFFFFFFFFFFFF
            无符号右 = 右值 & 0xFFFFFFFFFFFFFFFF
            
            if 操作码枚举 == 操作码.等于判断64:
                结果 = 1 if 有符号左 == 有符号右 else 0
            elif 操作码枚举 == 操作码.不等于判断64:
                结果 = 1 if 有符号左 != 有符号右 else 0
            elif 操作码枚举 == 操作码.小于有符号64:
                结果 = 1 if 有符号左 < 有符号右 else 0
            elif 操作码枚举 == 操作码.小于无符号64:
                结果 = 1 if 无符号左 < 无符号右 else 0
            elif 操作码枚举 == 操作码.大于有符号64:
                结果 = 1 if 有符号左 > 有符号右 else 0
            elif 操作码枚举 == 操作码.大于无符号64:
                结果 = 1 if 无符号左 > 无符号右 else 0
            elif 操作码枚举 == 操作码.小于等于有符号64:
                结果 = 1 if 有符号左 <= 有符号右 else 0
            elif 操作码枚举 == 操作码.小于等于无符号64:
                结果 = 1 if 无符号左 <= 无符号右 else 0
            elif 操作码枚举 == 操作码.大于等于有符号64:
                结果 = 1 if 有符号左 >= 有符号右 else 0
            elif 操作码枚举 == 操作码.大于等于无符号64:
                结果 = 1 if 无符号左 >= 无符号右 else 0
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行整数32运算(self, 操作码枚举: 操作码):
        """执行32位整数运算指令"""
        右值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        左值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        
        有符号右 = struct.unpack('<i', struct.pack('<I', 右值))[0]
        有符号左 = struct.unpack('<i', struct.pack('<I', 左值))[0]
        
        if 操作码枚举 == 操作码.加法32:
            结果 = (左值 + 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.减法32:
            结果 = (左值 - 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.乘法32:
            结果 = (左值 * 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.除法有符号32:
            if 有符号右 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (有符号左 // 有符号右) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.除法无符号32:
            if 右值 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (左值 // 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.取余有符号32:
            if 有符号右 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (有符号左 % 有符号右) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.取余无符号32:
            if 右值 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (左值 % 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.与运算32:
            结果 = (左值 & 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.或运算32:
            结果 = (左值 | 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.异或运算32:
            结果 = (左值 ^ 右值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.左移32:
            结果 = (左值 << (右值 & 0x1F)) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.右移有符号32:
            结果 = (有符号左 >> (右值 & 0x1F)) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.右移无符号32:
            结果 = (左值 >> (右值 & 0x1F)) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.循环左移32:
            位移 = 右值 & 0x1F
            结果 = ((左值 << 位移) | (左值 >> (32 - 位移))) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.循环右移32:
            位移 = 右值 & 0x1F
            结果 = ((左值 >> 位移) | (左值 << (32 - 位移))) & 0xFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行整数64运算(self, 操作码枚举: 操作码):
        """执行64位整数运算指令"""
        右值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        左值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        
        有符号右 = struct.unpack('<q', struct.pack('<Q', 右值))[0]
        有符号左 = struct.unpack('<q', struct.pack('<Q', 左值))[0]
        
        if 操作码枚举 == 操作码.加法64:
            结果 = (左值 + 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.减法64:
            结果 = (左值 - 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.乘法64:
            结果 = (左值 * 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.除法有符号64:
            if 有符号右 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (有符号左 // 有符号右) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.除法无符号64:
            if 右值 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (左值 // 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.取余有符号64:
            if 有符号右 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (有符号左 % 有符号右) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.取余无符号64:
            if 右值 == 0:
                raise RuntimeError(异常消息.除零错误)
            结果 = (左值 % 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.与运算64:
            结果 = (左值 & 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.或运算64:
            结果 = (左值 | 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.异或运算64:
            结果 = (左值 ^ 右值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.左移64:
            结果 = (左值 << (右值 & 0x3F)) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.右移有符号64:
            结果 = (有符号左 >> (右值 & 0x3F)) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.右移无符号64:
            结果 = (左值 >> (右值 & 0x3F)) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.循环左移64:
            位移 = 右值 & 0x3F
            结果 = ((左值 << 位移) | (左值 >> (64 - 位移))) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.循环右移64:
            位移 = 右值 & 0x3F
            结果 = ((左值 >> 位移) | (左值 << (64 - 位移))) & 0xFFFFFFFFFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数64, 结果))
    
    def _执行浮点32比较(self, 操作码枚举: 操作码):
        """执行32位浮点比较指令"""
        右值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        左值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        
        右值 = struct.unpack('<f', struct.pack('<I', 右值位))[0]
        左值 = struct.unpack('<f', struct.pack('<I', 左值位))[0]
        
        if 操作码枚举 == 操作码.等于判断单精度:
            结果 = 1 if 左值 == 右值 else 0
        elif 操作码枚举 == 操作码.不等于判断单精度:
            结果 = 1 if 左值 != 右值 else 0
        elif 操作码枚举 == 操作码.小于单精度:
            结果 = 1 if 左值 < 右值 else 0
        elif 操作码枚举 == 操作码.大于单精度:
            结果 = 1 if 左值 > 右值 else 0
        elif 操作码枚举 == 操作码.小于等于单精度:
            结果 = 1 if 左值 <= 右值 else 0
        elif 操作码枚举 == 操作码.大于等于单精度:
            结果 = 1 if 左值 >= 右值 else 0
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行浮点64比较(self, 操作码枚举: 操作码):
        """执行64位浮点比较指令"""
        右值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        左值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        
        右值 = struct.unpack('<d', struct.pack('<Q', 右值位))[0]
        左值 = struct.unpack('<d', struct.pack('<Q', 左值位))[0]
        
        if 操作码枚举 == 操作码.等于判断双精度:
            结果 = 1 if 左值 == 右值 else 0
        elif 操作码枚举 == 操作码.不等于判断双精度:
            结果 = 1 if 左值 != 右值 else 0
        elif 操作码枚举 == 操作码.小于双精度:
            结果 = 1 if 左值 < 右值 else 0
        elif 操作码枚举 == 操作码.大于双精度:
            结果 = 1 if 左值 > 右值 else 0
        elif 操作码枚举 == 操作码.小于等于双精度:
            结果 = 1 if 左值 <= 右值 else 0
        elif 操作码枚举 == 操作码.大于等于双精度:
            结果 = 1 if 左值 >= 右值 else 0
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行浮点32一元运算(self, 操作码枚举: 操作码):
        """执行32位浮点一元运算"""
        值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        值 = struct.unpack('<f', struct.pack('<I', 值位))[0]
        
        if 操作码枚举 == 操作码.取绝对值单精度:
            结果 = abs(值)
        elif 操作码枚举 == 操作码.取负单精度:
            结果 = -值
        elif 操作码枚举 == 操作码.向上取整单精度:
            结果 = math.ceil(值)
        elif 操作码枚举 == 操作码.向下取整单精度:
            结果 = math.floor(值)
        elif 操作码枚举 == 操作码.向零取整单精度:
            结果 = math.trunc(值)
        elif 操作码枚举 == 操作码.平方根单精度:
            结果 = math.sqrt(值)
        
        结果位 = struct.unpack('<I', struct.pack('<f', 结果))[0]
        self.模块.压入操作数(栈值(值类型.单精度浮点, 结果位))
    
    def _执行浮点64一元运算(self, 操作码枚举: 操作码):
        """执行64位浮点一元运算"""
        值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
        
        if 操作码枚举 == 操作码.取绝对值双精度:
            结果 = abs(值)
        elif 操作码枚举 == 操作码.取负双精度:
            结果 = -值
        elif 操作码枚举 == 操作码.向上取整双精度:
            结果 = math.ceil(值)
        elif 操作码枚举 == 操作码.向下取整双精度:
            结果 = math.floor(值)
        elif 操作码枚举 == 操作码.向零取整双精度:
            结果 = math.trunc(值)
        elif 操作码枚举 == 操作码.平方根双精度:
            结果 = math.sqrt(值)
        
        结果位 = struct.unpack('<Q', struct.pack('<d', 结果))[0]
        self.模块.压入操作数(栈值(值类型.双精度浮点, 结果位))
    
    def _执行浮点32二元运算(self, 操作码枚举: 操作码):
        """执行32位浮点二元运算"""
        右值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        左值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        
        右值 = struct.unpack('<f', struct.pack('<I', 右值位))[0]
        左值 = struct.unpack('<f', struct.pack('<I', 左值位))[0]
        
        if 操作码枚举 == 操作码.加法单精度:
            结果 = 左值 + 右值
        elif 操作码枚举 == 操作码.减法单精度:
            结果 = 左值 - 右值
        elif 操作码枚举 == 操作码.乘法单精度:
            结果 = 左值 * 右值
        elif 操作码枚举 == 操作码.除法单精度:
            结果 = 左值 / 右值
        elif 操作码枚举 == 操作码.取最小值单精度:
            结果 = 浮点最小值(左值, 右值)
        elif 操作码枚举 == 操作码.取最大值单精度:
            结果 = 浮点最大值(左值, 右值)
        elif 操作码枚举 == 操作码.复制符号单精度:
            结果 = 复制浮点符号(左值, 右值)
        
        结果位 = struct.unpack('<I', struct.pack('<f', 结果))[0]
        self.模块.压入操作数(栈值(值类型.单精度浮点, 结果位))
    
    def _执行浮点64二元运算(self, 操作码枚举: 操作码):
        """执行64位浮点二元运算"""
        右值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        左值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        
        右值 = struct.unpack('<d', struct.pack('<Q', 右值位))[0]
        左值 = struct.unpack('<d', struct.pack('<Q', 左值位))[0]
        
        if 操作码枚举 == 操作码.加法双精度:
            结果 = 左值 + 右值
        elif 操作码枚举 == 操作码.减法双精度:
            结果 = 左值 - 右值
        elif 操作码枚举 == 操作码.乘法双精度:
            结果 = 左值 * 右值
        elif 操作码枚举 == 操作码.除法双精度:
            结果 = 左值 / 右值
        elif 操作码枚举 == 操作码.取最小值双精度:
            结果 = 浮点最小值(左值, 右值)
        elif 操作码枚举 == 操作码.取最大值双精度:
            结果 = 浮点最大值(左值, 右值)
        elif 操作码枚举 == 操作码.复制符号双精度:
            结果 = 复制浮点符号(左值, 右值)
        
        结果位 = struct.unpack('<Q', struct.pack('<d', 结果))[0]
        self.模块.压入操作数(栈值(值类型.双精度浮点, 结果位))
    
    def _执行类型转换到32位(self, 操作码枚举: 操作码):
        """执行类型转换到32位整数"""
        if 操作码枚举 in [操作码.截断32位有符号, 操作码.截断32位无符号]:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            值 = struct.unpack('<f', struct.pack('<I', 值位))[0]
            有符号 = (操作码枚举 == 操作码.截断32位有符号)
            结果 = 单精度转32位整数(值, 有符号)
        elif 操作码枚举 in [操作码.截断64位有符号32, 操作码.截断64位无符号32]:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            有符号 = (操作码枚举 == 操作码.截断64位有符号32)
            结果 = 双精度转32位整数(值, 有符号)
        elif 操作码枚举 in [操作码.浮点转32位有符号, 操作码.浮点转32位无符号]:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            有符号 = (操作码枚举 == 操作码.浮点转32位有符号)
            if 有符号:
                结果 = struct.unpack('<i', struct.pack('<I', 值))[0] & 0xFFFFFFFF
            else:
                结果 = 值
        elif 操作码枚举 in [操作码.双精度转32位有符号, 操作码.双精度转32位无符号]:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            有符号 = (操作码枚举 == 操作码.双精度转32位有符号)
            if 有符号:
                结果 = struct.unpack('<i', struct.pack('<I', 值 & 0xFFFFFFFF))[0] & 0xFFFFFFFF
            else:
                结果 = 值 & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.截断饱和32位有符号:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            结果 = 截断饱和32位有符号(值) & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.截断饱和32位无符号:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            结果 = 截断饱和32位无符号(值) & 0xFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行类型转换到64位(self, 操作码枚举: 操作码):
        """执行类型转换到64位整数"""
        if 操作码枚举 == 操作码.扩展有符号64:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            有符号值 = struct.unpack('<i', struct.pack('<I', 值))[0]
            结果 = 有符号值 & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.扩展无符号64:
            结果 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        elif 操作码枚举 in [操作码.截断64位有符号64, 操作码.截断64位无符号64]:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            有符号 = (操作码枚举 == 操作码.截断64位有符号64)
            结果 = 双精度转64位整数(值, 有符号)
        elif 操作码枚举 in [操作码.浮点转64位有符号, 操作码.浮点转64位无符号]:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            有符号 = (操作码枚举 == 操作码.浮点转64位有符号)
            if 有符号:
                浮点值 = struct.unpack('<f', struct.pack('<I', 值))[0]
                结果 = int(浮点值) & 0xFFFFFFFFFFFFFFFF
            else:
                结果 = 值
        elif 操作码枚举 in [操作码.双精度转64位有符号, 操作码.双精度转64位无符号]:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            有符号 = (操作码枚举 == 操作码.双精度转64位有符号)
            if 有符号:
                结果 = struct.unpack('<q', struct.pack('<Q', 值))[0] & 0xFFFFFFFFFFFFFFFF
            else:
                结果 = 值
        elif 操作码枚举 == 操作码.截断饱和64位有符号:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            结果 = 截断饱和64位有符号(值) & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.截断饱和64位无符号:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            值 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            结果 = 截断饱和64位无符号(值) & 0xFFFFFFFFFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数64, 结果))
    
    def _执行类型转换到单精度(self, 操作码枚举: 操作码):
        """执行类型转换到单精度浮点"""
        if 操作码枚举 == 操作码.转换32位有符号单精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            有符号值 = struct.unpack('<i', struct.pack('<I', 值))[0]
            结果 = float(有符号值)
        elif 操作码枚举 == 操作码.转换32位无符号单精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            结果 = float(值)
        elif 操作码枚举 == 操作码.转换64位有符号单精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            有符号值 = struct.unpack('<q', struct.pack('<Q', 值))[0]
            结果 = float(有符号值)
        elif 操作码枚举 == 操作码.转换64位无符号单精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            结果 = float(值)
        elif 操作码枚举 == 操作码.双精度转单精度:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            结果 = struct.unpack('<d', struct.pack('<Q', 值位))[0]
            结果 = struct.unpack('<f', struct.pack('<f', 结果))[0]
        
        结果位 = struct.unpack('<I', struct.pack('<f', 结果))[0]
        self.模块.压入操作数(栈值(值类型.单精度浮点, 结果位))
    
    def _执行类型转换到双精度(self, 操作码枚举: 操作码):
        """执行类型转换到双精度浮点"""
        if 操作码枚举 == 操作码.转换32位有符号双精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            有符号值 = struct.unpack('<i', struct.pack('<I', 值))[0]
            结果 = float(有符号值)
        elif 操作码枚举 == 操作码.转换32位无符号双精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            结果 = float(值)
        elif 操作码枚举 == 操作码.转换64位有符号双精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            有符号值 = struct.unpack('<q', struct.pack('<Q', 值))[0]
            结果 = float(有符号值)
        elif 操作码枚举 == 操作码.转换64位无符号双精度:
            值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
            结果 = float(值)
        elif 操作码枚举 == 操作码.单精度转双精度:
            值位 = self.模块.弹出操作数().值 & 0xFFFFFFFF
            结果 = struct.unpack('<f', struct.pack('<I', 值位))[0]
        
        结果位 = struct.unpack('<Q', struct.pack('<d', 结果))[0]
        self.模块.压入操作数(栈值(值类型.双精度浮点, 结果位))
    
    def _执行符号扩展32(self, 操作码枚举: 操作码):
        """执行32位符号扩展"""
        值 = self.模块.弹出操作数().值 & 0xFFFFFFFF
        
        if 操作码枚举 == 操作码.扩展8位有符号32:
            结果 = struct.unpack('<b', struct.pack('<B', 值 & 0xFF))[0] & 0xFFFFFFFF
        elif 操作码枚举 == 操作码.扩展16位有符号32:
            结果 = struct.unpack('<h', struct.pack('<H', 值 & 0xFFFF))[0] & 0xFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数32, 结果))
    
    def _执行符号扩展64(self, 操作码枚举: 操作码):
        """执行64位符号扩展"""
        值 = self.模块.弹出操作数().值 & 0xFFFFFFFFFFFFFFFF
        
        if 操作码枚举 == 操作码.扩展8位有符号64:
            结果 = struct.unpack('<b', struct.pack('<B', 值 & 0xFF))[0] & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.扩展16位有符号64:
            结果 = struct.unpack('<h', struct.pack('<H', 值 & 0xFFFF))[0] & 0xFFFFFFFFFFFFFFFF
        elif 操作码枚举 == 操作码.扩展32位有符号64:
            结果 = struct.unpack('<i', struct.pack('<I', 值 & 0xFFFFFFFF))[0] & 0xFFFFFFFFFFFFFFFF
        
        self.模块.压入操作数(栈值(值类型.整数64, 结果))
