"""
WebAssembly 核心数据结构
对应原 module.h 文件
"""
from dataclasses import dataclass, field
from typing import List, Dict, Optional, Union, Any, Callable
from enum import Enum
import struct

from 枚举定义 import 值类型, 块类型


@dataclass
class 栈值:
    """操作数栈中的值"""
    值类型: 值类型
    值: Union[int, float]


@dataclass
class 栈帧:
    """调用栈帧"""
    函数索引: int
    返回地址: int
    局部变量基址: int
    操作数栈基址: int
    块栈基址: int
    调用者模块: Optional[Any] = None


@dataclass
class 控制块:
    """控制块（块/循环/条件）"""
    块类型: 块类型
    返回类型: Optional[值类型]
    起始地址: int
    结束地址: int
    否则地址: Optional[int] = None  # 仅用于条件块
    是循环: bool = False


@dataclass
class 函数类型:
    """函数类型定义"""
    参数类型: List[值类型]
    返回类型: List[值类型]


@dataclass
class 内存:
    """线性内存"""
    初始页数: int
    最大页数: int
    数据: bytearray = field(default_factory=bytearray)
    
    def __post_init__(self):
        if not self.数据:
            self.数据 = bytearray(self.初始页数 * 64 * 1024)
    
    @property
    def 页数(self) -> int:
        return len(self.数据) // (64 * 1024)
    
    def 增长(self, 增量页数: int) -> int:
        """增长内存，返回之前的页数"""
        原页数 = self.页数
        新页数 = 原页数 + 增量页数
        if 新页数 > self.最大页数:
            return -1
        self.数据.extend(bytearray(增量页数 * 64 * 1024))
        return 原页数


@dataclass
class 表:
    """函数表"""
    初始大小: int
    最大大小: int
    元素: List[Optional[int]] = field(default_factory=list)
    
    def __post_init__(self):
        if not self.元素:
            self.元素 = [None] * self.初始大小


@dataclass
class 导出项:
    """导出项"""
    名称: str
    导出类型: int
    索引: int


@dataclass
class 全局变量:
    """全局变量"""
    值类型: 值类型
    值: Union[int, float]
    可变: bool


@dataclass
class 数据段:
    """数据段"""
    内存索引: int
    偏移量: int
    数据: bytes


@dataclass
class 元素段:
    """元素段"""
    表索引: int
    偏移量: int
    函数索引列表: List[int]


@dataclass
class 导入项:
    """导入项"""
    模块名: str
    字段名: str
    导入类型: int
    索引: int


@dataclass
class 函数:
    """函数定义"""
    类型索引: int
    局部变量类型: List[值类型]
    代码起始: int
    代码结束: int
    函数体: bytes = field(default_factory=bytes)


class 模块:
    """WebAssembly 模块"""
    
    # 常量定义
    栈大小 = 64 * 1024
    调用栈大小 = 64 * 1024
    分支表大小 = 64 * 1024
    
    def __init__(self, 字节码: bytes):
        self.字节码 = 字节码
        self.字节数 = len(字节码)
        
        # 段数据
        self.类型列表: List[函数类型] = []
        self.函数列表: List[函数] = []
        self.表: Optional[表] = None
        self.内存: Optional[内存] = None
        self.全局变量列表: List[全局变量] = []
        self.导出列表: List[导出项] = []
        self.数据段列表: List[数据段] = []
        self.元素段列表: List[元素段] = []
        self.导入列表: List[导入项] = []
        
        # 统计信息
        self.导入函数数 = 0
        self.函数数 = 0
        self.起始函数索引: Optional[int] = None
        
        # 运行时状态
        self.程序计数器 = 0
        self.操作数栈指针 = -1
        self.帧指针 = -1
        self.块栈指针 = -1
        
        # 运行时数据结构
        self.操作数栈: List[栈值] = [栈值(值类型.整数32, 0) for _ in range(self.栈大小)]
        self.调用栈: List[栈帧] = [栈帧(0, 0, 0, 0, 0) for _ in range(self.调用栈大小)]
        self.块栈: List[控制块] = []
        self.分支表: List[int] = [0] * self.分支表大小
        
        # 块查找表：地址 -> 控制块
        self.块查找表: Dict[int, 控制块] = {}
        
        # 外部函数映射
        self.外部函数映射: Dict[str, Callable] = {}
    
    def 压入操作数(self, 值: 栈值):
        """压入操作数栈"""
        self.操作数栈指针 += 1
        if self.操作数栈指针 >= self.栈大小:
            raise OverflowError("操作数栈溢出")
        self.操作数栈[self.操作数栈指针] = 值
    
    def 弹出操作数(self) -> 栈值:
        """弹出操作数栈"""
        if self.操作数栈指针 < 0:
            raise IndexError("操作数栈下溢")
        值 = self.操作数栈[self.操作数栈指针]
        self.操作数栈指针 -= 1
        return 值
    
    def 查看栈顶(self, 偏移: int = 0) -> 栈值:
        """查看栈顶元素（不弹出）"""
        if self.操作数栈指针 - 偏移 < 0:
            raise IndexError("操作数栈下溢")
        return self.操作数栈[self.操作数栈指针 - 偏移]
    
    def 压入帧(self, 帧: 栈帧):
        """压入调用栈"""
        self.帧指针 += 1
        if self.帧指针 >= self.调用栈大小:
            raise OverflowError("调用栈溢出")
        self.调用栈[self.帧指针] = 帧
    
    def 弹出帧(self) -> 栈帧:
        """弹出调用栈"""
        if self.帧指针 < 0:
            raise IndexError("调用栈下溢")
        帧 = self.调用栈[self.帧指针]
        self.帧指针 -= 1
        return 帧
    
    def 压入块(self, 块: 控制块):
        """压入块栈"""
        self.块栈指针 += 1
        self.块栈.append(块)
    
    def 弹出块(self) -> 控制块:
        """弹出块栈"""
        if self.块栈指针 < 0:
            raise IndexError("块栈下溢")
        块 = self.块栈.pop()
        self.块栈指针 -= 1
        return 块
    
    def 读取字节(self) -> int:
        """读取当前PC处的字节并前进"""
        if self.程序计数器 >= self.字节数:
            raise IndexError("程序计数器超出范围")
        字节 = self.字节码[self.程序计数器]
        self.程序计数器 += 1
        return 字节
    
    def 读取有符号32位(self) -> int:
        """读取有符号32位整数（小端序）"""
        值 = struct.unpack('<i', self.字节码[self.程序计数器:self.程序计数器+4])[0]
        self.程序计数器 += 4
        return 值
    
    def 读取无符号32位(self) -> int:
        """读取无符号32位整数（小端序）"""
        值 = struct.unpack('<I', self.字节码[self.程序计数器:self.程序计数器+4])[0]
        self.程序计数器 += 4
        return 值
    
    def 读取有符号64位(self) -> int:
        """读取有符号64位整数（小端序）"""
        值 = struct.unpack('<q', self.字节码[self.程序计数器:self.程序计数器+8])[0]
        self.程序计数器 += 8
        return 值
    
    def 读取单精度浮点(self) -> float:
        """读取单精度浮点数（小端序）"""
        值 = struct.unpack('<f', self.字节码[self.程序计数器:self.程序计数器+4])[0]
        self.程序计数器 += 4
        return 值
    
    def 读取双精度浮点(self) -> float:
        """读取双精度浮点数（小端序）"""
        值 = struct.unpack('<d', self.字节码[self.程序计数器:self.程序计数器+8])[0]
        self.程序计数器 += 8
        return 值
    
    def 读取字节数组(self, 长度: int) -> bytes:
        """读取指定长度的字节数组"""
        数据 = self.字节码[self.程序计数器:self.程序计数器+长度]
        self.程序计数器 += 长度
        return 数据
