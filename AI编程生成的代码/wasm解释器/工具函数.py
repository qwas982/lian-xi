"""
WebAssembly 工具函数
对应原 utils.c / utils.h 文件
"""
import struct
import math
from typing import Tuple, List
from 枚举定义 import 值类型


class 异常消息:
    """异常消息常量"""
    栈溢出 = "栈溢出"
    栈下溢 = "栈下溢"
    调用栈溢出 = "调用栈溢出"
    调用栈下溢 = "调用栈下溢"
    块栈溢出 = "块栈溢出"
    块栈下溢 = "块栈下溢"
    分支表溢出 = "分支表溢出"
    内存访问越界 = "内存访问越界"
    内存分配失败 = "内存分配失败"
    未初始化内存 = "未初始化内存"
    除零错误 = "除零错误"
    整数溢出 = "整数溢出"
    无效转换 = "无效转换"
    不可达代码 = "不可达代码"
    未定义元素 = "未定义元素"
    间接调用类型不匹配 = "间接调用类型不匹配"
    超出表边界 = "超出表边界"
    未实现 = "未实现"
    解析错误 = "解析错误"
    无效魔数 = "无效的Wasm魔数"
    无效版本 = "无效的Wasm版本"
    无效段 = "无效的段类型"
    无效类型 = "无效的类型"
    无效操作码 = "无效的操作码"


def 解码无符号整型(字节码: bytes, 偏移: int, 最大位数: int = 32) -> Tuple[int, int]:
    """
    解码LEB128无符号整数
    返回 (值, 新偏移)
    """
    结果 = 0
    位移 = 0
    字节数 = 0
    
    while True:
        if 偏移 + 字节数 >= len(字节码):
            raise ValueError("LEB128解码：意外的字节流结束")
        
        字节 = 字节码[偏移 + 字节数]
        字节数 += 1
        
        结果 |= (字节 & 0x7F) << 位移
        
        if (字节 & 0x80) == 0:
            break
        
        位移 += 7
        if 位移 >= 最大位数:
            raise ValueError(f"LEB128解码：超出{最大位数}位限制")
    
    return 结果, 偏移 + 字节数


def 解码有符号整型(字节码: bytes, 偏移: int, 最大位数: int = 32) -> Tuple[int, int]:
    """
    解码LEB128有符号整数
    返回 (值, 新偏移)
    """
    结果 = 0
    位移 = 0
    字节数 = 0
    
    while True:
        if 偏移 + 字节数 >= len(字节码):
            raise ValueError("LEB128解码：意外的字节流结束")
        
        字节 = 字节码[偏移 + 字节数]
        字节数 += 1
        
        结果 |= (字节 & 0x7F) << 位移
        
        if (字节 & 0x80) == 0:
            # 符号扩展
            if (字节 & 0x40) and 位移 < 最大位数:
                结果 |= (~0 << (位移 + 7))
            break
        
        位移 += 7
        if 位移 >= 最大位数:
            raise ValueError(f"LEB128解码：超出{最大位数}位限制")
    
    # 确保结果在指定位数范围内
    if 最大位数 == 32:
        结果 = struct.unpack('<i', struct.pack('<I', 结果 & 0xFFFFFFFF))[0]
    elif 最大位数 == 64:
        结果 = struct.unpack('<q', struct.pack('<Q', 结果 & 0xFFFFFFFFFFFFFFFF))[0]
    
    return 结果, 偏移 + 字节数


def 解码无符号32位(字节码: bytes, 偏移: int) -> Tuple[int, int]:
    """解码LEB128无符号32位整数"""
    return 解码无符号整型(字节码, 偏移, 32)


def 解码无符号64位(字节码: bytes, 偏移: int) -> Tuple[int, int]:
    """解码LEB128无符号64位整数"""
    return 解码无符号整型(字节码, 偏移, 64)


def 解码有符号32位(字节码: bytes, 偏移: int) -> Tuple[int, int]:
    """解码LEB128有符号32位整数"""
    return 解码有符号整型(字节码, 偏移, 32)


def 解码有符号64位(字节码: bytes, 偏移: int) -> Tuple[int, int]:
    """解码LEB128有符号64位整数"""
    return 解码有符号整型(字节码, 偏移, 64)


def 编码无符号整型(值: int) -> bytes:
    """编码LEB128无符号整数"""
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


def 编码有符号整型(值: int) -> bytes:
    """编码LEB128有符号整数"""
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


def 单精度转32位整数(值: float, 有符号: bool = True) -> int:
    """将单精度浮点数转换为32位整数"""
    if math.isnan(值) or math.isinf(值):
        raise ValueError("无法转换NaN或无穷大")
    
    if 值 < 0:
        整数 = math.ceil(值)
    else:
        整数 = math.floor(值)
    
    if 有符号:
        if 整数 < -2**31 or 整数 >= 2**31:
            raise ValueError("整数溢出")
        # 返回有符号整数，不应用掩码
        return int(整数)
    else:
        if 整数 < 0 or 整数 >= 2**32:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFF


def 双精度转32位整数(值: float, 有符号: bool = True) -> int:
    """将双精度浮点数转换为32位整数"""
    if math.isnan(值) or math.isinf(值):
        raise ValueError("无法转换NaN或无穷大")
    
    if 值 < 0:
        整数 = math.ceil(值)
    else:
        整数 = math.floor(值)
    
    if 有符号:
        if 整数 < -2**31 or 整数 >= 2**31:
            raise ValueError("整数溢出")
        # 返回有符号整数，不应用掩码
        return int(整数)
    else:
        if 整数 < 0 or 整数 >= 2**32:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFF


def 单精度转64位整数(值: float, 有符号: bool = True) -> int:
    """将单精度浮点数转换为64位整数"""
    if math.isnan(值) or math.isinf(值):
        raise ValueError("无法转换NaN或无穷大")
    
    if 值 < 0:
        整数 = math.ceil(值)
    else:
        整数 = math.floor(值)
    
    if 有符号:
        if 整数 < -2**63 or 整数 >= 2**63:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFFFFFFFFFF
    else:
        if 整数 < 0 or 整数 >= 2**64:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFFFFFFFFFF


def 双精度转64位整数(值: float, 有符号: bool = True) -> int:
    """将双精度浮点数转换为64位整数"""
    if math.isnan(值) or math.isinf(值):
        raise ValueError("无法转换NaN或无穷大")
    
    if 值 < 0:
        整数 = math.ceil(值)
    else:
        整数 = math.floor(值)
    
    if 有符号:
        if 整数 < -2**63 or 整数 >= 2**63:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFFFFFFFFFF
    else:
        if 整数 < 0 or 整数 >= 2**64:
            raise ValueError("整数溢出")
        return int(整数) & 0xFFFFFFFFFFFFFFFF


def 截断饱和32位有符号(值: float) -> int:
    """饱和截断到32位有符号整数"""
    if math.isnan(值):
        return 0
    if math.isinf(值):
        return -2**31 if 值 < 0 else 2**31 - 1
    
    if 值 < -2**31:
        return -2**31
    elif 值 >= 2**31:
        return 2**31 - 1
    
    return int(值)


def 截断饱和32位无符号(值: float) -> int:
    """饱和截断到32位无符号整数"""
    if math.isnan(值):
        return 0
    if math.isinf(值):
        return 0 if 值 < 0 else 2**32 - 1
    
    if 值 < 0:
        return 0
    elif 值 >= 2**32:
        return 2**32 - 1
    
    return int(值)


def 截断饱和64位有符号(值: float) -> int:
    """饱和截断到64位有符号整数"""
    if math.isnan(值):
        return 0
    if math.isinf(值):
        return -2**63 if 值 < 0 else 2**63 - 1
    
    if 值 < -2**63:
        return -2**63
    elif 值 >= 2**63:
        return 2**63 - 1
    
    return int(值)


def 截断饱和64位无符号(值: float) -> int:
    """饱和截断到64位无符号整数"""
    if math.isnan(值):
        return 0
    if math.isinf(值):
        return 0 if 值 < 0 else 2**64 - 1
    
    if 值 < 0:
        return 0
    elif 值 >= 2**64:
        return 2**64 - 1
    
    return int(值)


def 复制浮点符号(目标: float, 源: float) -> float:
    """复制浮点数的符号位"""
    if math.copysign(1.0, 源) < 0:
        return -abs(目标)
    else:
        return abs(目标)


def 浮点最小值(甲: float, 乙: float) -> float:
    """返回两个浮点数的最小值（处理NaN）"""
    if math.isnan(甲):
        return 甲
    if math.isnan(乙):
        return 乙
    return min(甲, 乙)


def 浮点最大值(甲: float, 乙: float) -> float:
    """返回两个浮点数的最大值（处理NaN）"""
    if math.isnan(甲):
        return 甲
    if math.isnan(乙):
        return 乙
    return max(甲, 乙)


def 值转字符串(值类型: 值类型, 值: int) -> str:
    """将栈值转换为字符串表示"""
    if 值类型 == 值类型.整数32:
        return str(struct.unpack('<i', struct.pack('<I', 值 & 0xFFFFFFFF))[0])
    elif 值类型 == 值类型.整数64:
        return str(struct.unpack('<q', struct.pack('<Q', 值 & 0xFFFFFFFFFFFFFFFF))[0])
    elif 值类型 == 值类型.单精度浮点:
        浮点 = struct.unpack('<f', struct.pack('<I', 值 & 0xFFFFFFFF))[0]
        return f"{浮点:.7g}"
    elif 值类型 == 值类型.双精度浮点:
        浮点 = struct.unpack('<d', struct.pack('<Q', 值 & 0xFFFFFFFFFFFFFFFF))[0]
        return f"{浮点:.7g}"
    else:
        return str(值)


def 获取块类型结果(块类型: int) -> List[值类型]:
    """根据块类型获取结果类型列表"""
    if 块类型 == 0x40:  # 空块
        return []
    elif 块类型 == 0x7F:  # i32
        return [值类型.整数32]
    elif 块类型 == 0x7E:  # i64
        return [值类型.整数64]
    elif 块类型 == 0x7D:  # f32
        return [值类型.单精度浮点]
    elif 块类型 == 0x7C:  # f64
        return [值类型.双精度浮点]
    else:
        return []


def 获取块类型参数(块类型: int) -> List[值类型]:
    """根据块类型获取参数类型列表"""
    return []  # MVP中块类型没有参数
