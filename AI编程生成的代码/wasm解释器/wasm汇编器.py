"""
WebAssembly 汇编器
将WAT (WebAssembly Text Format) 风格的代码转换为Wasm二进制
简化版，支持基本指令
"""
import re
from typing import List, Dict, Tuple, Optional
from io import BytesIO

from 枚举定义 import 操作码, 值类型


class Wasm汇编器:
    """Wasm汇编器 - 将类WAT语法转换为Wasm二进制"""
    
    # 指令映射表
    指令映射 = {
        # 控制流
        'unreachable': 操作码.不可达,
        'nop': 操作码.空操作,
        'block': 操作码.块开始,
        'loop': 操作码.循环开始,
        'if': 操作码.条件分支,
        'else': 操作码.否则分支,
        'end': 操作码.结束标记,
        'br': 操作码.分支,
        'br_if': 操作码.条件分支表,
        'return': 操作码.返回,
        
        # 参数
        'drop': 操作码.丢弃,
        'select': 操作码.选择,
        
        # 变量
        'local.get': 操作码.局部变量读取,
        'local.set': 操作码.局部变量写入,
        'local.tee': 操作码.局部变量读取32,
        'global.get': 操作码.全局变量读取,
        'global.set': 操作码.全局变量写入,
        
        # 内存
        'i32.load': 操作码.内存读取32位,
        'i64.load': 操作码.内存读取64位,
        'f32.load': 操作码.内存读取单精度浮点,
        'f64.load': 操作码.内存读取双精度浮点,
        'i32.store': 操作码.内存写入32位,
        'i64.store': 操作码.内存写入64位,
        'f32.store': 操作码.内存写入单精度浮点,
        'f64.store': 操作码.内存写入双精度浮点,
        'memory.size': 操作码.内存大小,
        'memory.grow': 操作码.内存增长,
        
        # i32 比较
        'i32.eqz': 操作码.等于零判断32,
        'i32.eq': 操作码.等于判断32,
        'i32.ne': 操作码.不等于判断32,
        'i32.lt_s': 操作码.小于有符号32,
        'i32.lt_u': 操作码.小于无符号32,
        'i32.gt_s': 操作码.大于有符号32,
        'i32.gt_u': 操作码.大于无符号32,
        'i32.le_s': 操作码.小于等于有符号32,
        'i32.le_u': 操作码.小于等于无符号32,
        'i32.ge_s': 操作码.大于等于有符号32,
        'i32.ge_u': 操作码.大于等于无符号32,
        
        # i64 比较
        'i64.eqz': 操作码.等于零判断64,
        'i64.eq': 操作码.等于判断64,
        'i64.ne': 操作码.不等于判断64,
        'i64.lt_s': 操作码.小于有符号64,
        'i64.lt_u': 操作码.小于无符号64,
        'i64.gt_s': 操作码.大于有符号64,
        'i64.gt_u': 操作码.大于无符号64,
        'i64.le_s': 操作码.小于等于有符号64,
        'i64.le_u': 操作码.小于等于无符号64,
        'i64.ge_s': 操作码.大于等于有符号64,
        'i64.ge_u': 操作码.大于等于无符号64,
        
        # i32 运算
        'i32.add': 操作码.加法32,
        'i32.sub': 操作码.减法32,
        'i32.mul': 操作码.乘法32,
        'i32.div_s': 操作码.除法有符号32,
        'i32.div_u': 操作码.除法无符号32,
        'i32.rem_s': 操作码.取余有符号32,
        'i32.rem_u': 操作码.取余无符号32,
        'i32.and': 操作码.与运算32,
        'i32.or': 操作码.或运算32,
        'i32.xor': 操作码.异或运算32,
        'i32.shl': 操作码.左移32,
        'i32.shr_s': 操作码.右移有符号32,
        'i32.shr_u': 操作码.右移无符号32,
        'i32.rotl': 操作码.循环左移32,
        'i32.rotr': 操作码.循环右移32,
        
        # i64 运算
        'i64.add': 操作码.加法64,
        'i64.sub': 操作码.减法64,
        'i64.mul': 操作码.乘法64,
        'i64.div_s': 操作码.除法有符号64,
        'i64.div_u': 操作码.除法无符号64,
        'i64.rem_s': 操作码.取余有符号64,
        'i64.rem_u': 操作码.取余无符号64,
        'i64.and': 操作码.与运算64,
        'i64.or': 操作码.或运算64,
        'i64.xor': 操作码.异或运算64,
        'i64.shl': 操作码.左移64,
        'i64.shr_s': 操作码.右移有符号64,
        'i64.shr_u': 操作码.右移无符号64,
        'i64.rotl': 操作码.循环左移64,
        'i64.rotr': 操作码.循环右移64,
    }
    
    # 值类型映射
    类型映射 = {
        'i32': 值类型.整数32,
        'i64': 值类型.整数64,
        'f32': 值类型.单精度浮点,
        'f64': 值类型.双精度浮点,
    }
    
    def __init__(self):
        self.输出 = BytesIO()
        self.函数类型列表: List[Tuple[List[str], List[str]]] = []
        self.函数列表: List[Dict] = []
        self.导出列表: List[Dict] = []
        self.全局列表: List[Dict] = []
        self.内存: Optional[Dict] = None
        self.表: Optional[Dict] = None
    
    def 编码无符号32位(self, 值: int) -> bytes:
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
    
    def 编码有符号32位(self, 值: int) -> bytes:
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
    
    def 编码有符号64位(self, 值: int) -> bytes:
        """编码LEB128有符号64位"""
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
    
    def 解析(self, 源码: str) -> bytes:
        """解析WAT风格源码并生成Wasm二进制"""
        行列表 = 源码.strip().split('\n')
        
        # 第一行应该是模块声明
        if '(module' not in 行列表[0]:
            raise ValueError("缺少模块声明")
        
        # 解析各个部分
        当前函数: Optional[Dict] = None
        缩进级别 = 0
        
        for 行 in 行列表[1:]:  # 跳过模块声明行
            行 = 行.strip()
            if not 行 or 行.startswith(';;'):
                continue
            
            # 处理缩进级别
            if 行.startswith('('):
                缩进级别 += 1
            if 行.endswith(')'):
                缩进级别 -= 1
            
            # 解析函数
            if '(func' in 行:
                当前函数 = self._解析函数声明(行)
                self.函数列表.append(当前函数)
            elif 当前函数 is not None:
                if 行 == ')' or ')' in 行:
                    当前函数 = None
                else:
                    self._解析指令(行, 当前函数)
            
            # 解析导出
            if '(export' in 行:
                self._解析导出(行)
            
            # 解析内存
            if '(memory' in 行:
                self._解析内存(行)
            
            # 解析表
            if '(table' in 行:
                self._解析表(行)
            
            # 解析全局
            if '(global' in 行:
                self._解析全局(行)
        
        return self._生成二进制()
    
    def _解析函数声明(self, 行: str) -> Dict:
        """解析函数声明"""
        函数 = {
            '名称': None,
            '参数类型': [],
            '返回类型': [],
            '局部变量': [],
            '指令': [],
        }
        
        # 解析参数
        参数匹配 = re.findall(r'\(param\s+(\w+)\)', 行)
        for 类型 in 参数匹配:
            函数['参数类型'].append(类型)
        
        # 解析返回类型
        返回匹配 = re.findall(r'\(result\s+(\w+)\)', 行)
        for 类型 in 返回匹配:
            函数['返回类型'].append(类型)
        
        # 解析局部变量
        局部匹配 = re.findall(r'\(local\s+(\w+)\)', 行)
        for 类型 in 局部匹配:
            函数['局部变量'].append(类型)
        
        return 函数
    
    def _解析指令(self, 行: str, 函数: Dict):
        """解析指令"""
        行 = 行.strip()
        
        # i32.const
        匹配 = re.match(r'i32\.const\s+(-?\d+)', 行)
        if 匹配:
            值 = int(匹配.group(1))
            函数['指令'].append(('i32.const', 值))
            return
        
        # i64.const
        匹配 = re.match(r'i64\.const\s+(-?\d+)', 行)
        if 匹配:
            值 = int(匹配.group(1))
            函数['指令'].append(('i64.const', 值))
            return
        
        # local.get/set/tee
        匹配 = re.match(r'local\.(get|set|tee)\s+(\d+)', 行)
        if 匹配:
            操作 = 匹配.group(1)
            索引 = int(匹配.group(2))
            函数['指令'].append((f'local.{操作}', 索引))
            return
        
        # global.get/set
        匹配 = re.match(r'global\.(get|set)\s+(\d+)', 行)
        if 匹配:
            操作 = 匹配.group(1)
            索引 = int(匹配.group(2))
            函数['指令'].append((f'global.{操作}', 索引))
            return
        
        # br/br_if
        匹配 = re.match(r'br(_if)?\s+(\d+)', 行)
        if 匹配:
            指令 = 'br_if' if 匹配.group(1) else 'br'
            标签 = int(匹配.group(2))
            函数['指令'].append((指令, 标签))
            return
        
        # block/loop/if
        匹配 = re.match(r'(block|loop|if)\s*\(?(result\s+(\w+))?\)?', 行)
        if 匹配:
            指令 = 匹配.group(1)
            返回类型 = 匹配.group(3)
            函数['指令'].append((指令, 返回类型))
            return
        
        # 处理end指令
        if 行 == 'end' or 行.startswith('end'):
            函数['指令'].append(('end',))
            return
        
        # 简单指令
        if 行 in self.指令映射:
            函数['指令'].append((行,))
            return
        
        # 尝试匹配无参数指令
        简单匹配 = re.match(r'(\w+(?:\.\w+)*)', 行)
        if 简单匹配:
            指令名 = 简单匹配.group(1)
            if 指令名 in self.指令映射:
                函数['指令'].append((指令名,))
            elif 指令名 == 'end':
                函数['指令'].append(('end',))
    
    def _解析导出(self, 行: str):
        """解析导出"""
        匹配 = re.match(r'\(export\s+"([^"]+)"\s+\(func\s+(\d+)\)\)', 行)
        if 匹配:
            名称 = 匹配.group(1)
            索引 = int(匹配.group(2))
            self.导出列表.append({'名称': 名称, '索引': 索引})
    
    def _解析内存(self, 行: str):
        """解析内存声明"""
        匹配 = re.match(r'\(memory\s+(\d+)\s*(\d+)?\)', 行)
        if 匹配:
            初始 = int(匹配.group(1))
            最大 = int(匹配.group(2)) if 匹配.group(2) else 初始
            self.内存 = {'初始': 初始, '最大': 最大}
    
    def _解析表(self, 行: str):
        """解析表声明"""
        匹配 = re.match(r'\(table\s+(\d+)\s*(\d+)?\s+funcref\)', 行)
        if 匹配:
            初始 = int(匹配.group(1))
            最大 = int(匹配.group(2)) if 匹配.group(2) else 初始
            self.表 = {'初始': 初始, '最大': 最大}
    
    def _解析全局(self, 行: str):
        """解析全局变量"""
        匹配 = re.match(r'\(global\s+\$?(\w+)\s+(\w+)\s+\(?(\w+)\.const\s+(-?\d+)\)?\)', 行)
        if 匹配:
            名称 = 匹配.group(1)
            类型 = 匹配.group(2)
            可变性 = 名称.startswith('mut')
            初始值 = int(匹配.group(4))
            self.全局列表.append({
                '名称': 名称,
                '类型': 类型,
                '可变性': 可变性,
                '初始值': 初始值
            })
    
    def _生成二进制(self) -> bytes:
        """生成Wasm二进制"""
        输出 = BytesIO()
        
        # 头部
        输出.write(b'\x00asm')
        输出.write((1).to_bytes(4, 'little'))
        
        # 类型段
        if self.函数列表:
            self._写类型段(输出)
        
        # 函数段
        if self.函数列表:
            self._写函数段(输出)
        
        # 表段
        if self.表:
            self._写表段(输出)
        
        # 内存段
        if self.内存:
            self._写内存段(输出)
        
        # 全局段
        if self.全局列表:
            self._写全局段(输出)
        
        # 导出段
        if self.导出列表:
            self._写导出段(输出)
        
        # 代码段
        if self.函数列表:
            self._写代码段(输出)
        
        return 输出.getvalue()
    
    def _写类型段(self, 输出: BytesIO):
        """写入类型段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(len(self.函数列表)))
        
        for 函数 in self.函数列表:
            内容.write(bytes([0x60]))  # 函数类型
            内容.write(self.编码无符号32位(len(函数['参数类型'])))
            for 类型 in 函数['参数类型']:
                内容.write(bytes([self.类型映射[类型].value]))
            内容.write(self.编码无符号32位(len(函数['返回类型'])))
            for 类型 in 函数['返回类型']:
                内容.write(bytes([self.类型映射[类型].value]))
        
        输出.write(bytes([0x01]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写函数段(self, 输出: BytesIO):
        """写入函数段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(len(self.函数列表)))
        for i in range(len(self.函数列表)):
            内容.write(self.编码无符号32位(i))  # 类型索引
        
        输出.write(bytes([0x03]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写表段(self, 输出: BytesIO):
        """写入表段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(1))  # 1个表
        内容.write(bytes([0x70]))  # funcref
        内容.write(bytes([0x00]))  # 无最大限制标志
        内容.write(self.编码无符号32位(self.表['初始']))
        
        输出.write(bytes([0x04]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写内存段(self, 输出: BytesIO):
        """写入内存段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(1))  # 1个内存
        内容.write(bytes([0x00]))  # 无最大限制标志
        内容.write(self.编码无符号32位(self.内存['初始']))
        
        输出.write(bytes([0x05]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写全局段(self, 输出: BytesIO):
        """写入全局段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(len(self.全局列表)))
        
        for 全局 in self.全局列表:
            内容.write(bytes([self.类型映射[全局['类型']].value]))
            内容.write(bytes([0x01 if 全局['可变性'] else 0x00]))
            # 初始化表达式
            if 全局['类型'] == 'i32':
                内容.write(bytes([0x41]))  # i32.const
                内容.write(self.编码有符号32位(全局['初始值']))
            elif 全局['类型'] == 'i64':
                内容.write(bytes([0x42]))  # i64.const
                内容.write(self.编码有符号64位(全局['初始值']))
            内容.write(bytes([0x0B]))  # end
        
        输出.write(bytes([0x06]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写导出段(self, 输出: BytesIO):
        """写入导出段"""
        内容 = BytesIO()
        内容.write(self.编码无符号32位(len(self.导出列表)))
        
        for 导出 in self.导出列表:
            内容.write(self.编码无符号32位(len(导出['名称'])))
            内容.write(导出['名称'].encode())
            内容.write(bytes([0x00]))  # 函数导出
            内容.write(self.编码无符号32位(导出['索引']))
        
        输出.write(bytes([0x07]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _写代码段(self, 输出: BytesIO):
        """写入代码段"""
        函数体列表 = []
        
        for 函数 in self.函数列表:
            函数体 = self._生成函数体(函数)
            函数体列表.append(函数体)
        
        内容 = BytesIO()
        内容.write(self.编码无符号32位(len(函数体列表)))
        for 函数体 in 函数体列表:
            内容.write(self.编码无符号32位(len(函数体)))
            内容.write(函数体)
        
        输出.write(bytes([0x0A]))
        输出.write(self.编码无符号32位(内容.tell()))
        输出.write(内容.getvalue())
    
    def _生成函数体(self, 函数: Dict) -> bytes:
        """生成函数体"""
        输出 = BytesIO()
        
        # 局部变量声明
        if 函数['局部变量']:
            # 简化：每个类型单独一组
            输出.write(self.编码无符号32位(len(函数['局部变量'])))
            for 类型 in 函数['局部变量']:
                输出.write(self.编码无符号32位(1))  # 数量
                输出.write(bytes([self.类型映射[类型].value]))
        else:
            输出.write(bytes([0x00]))  # 0组局部变量
        
        # 指令
        for 指令 in 函数['指令']:
            self._写指令(输出, 指令)
        
        # 如果没有return/end，自动添加end
        if not 函数['指令'] or 函数['指令'][-1][0] != 'end':
            输出.write(bytes([0x0B]))
        
        return 输出.getvalue()
    
    def _写指令(self, 输出: BytesIO, 指令: Tuple):
        """写入单条指令"""
        操作 = 指令[0]
        
        if 操作 == 'i32.const':
            输出.write(bytes([0x41]))
            输出.write(self.编码有符号32位(指令[1]))
        elif 操作 == 'i64.const':
            输出.write(bytes([0x42]))
            输出.write(self.编码有符号64位(指令[1]))
        elif 操作 in ('local.get', 'local.set', 'local.tee'):
            映射 = {'local.get': 0x20, 'local.set': 0x21, 'local.tee': 0x22}
            输出.write(bytes([映射[操作]]))
            输出.write(self.编码无符号32位(指令[1]))
        elif 操作 in ('global.get', 'global.set'):
            映射 = {'global.get': 0x23, 'global.set': 0x24}
            输出.write(bytes([映射[操作]]))
            输出.write(self.编码无符号32位(指令[1]))
        elif 操作 in ('br', 'br_if'):
            映射 = {'br': 0x0C, 'br_if': 0x0D}
            输出.write(bytes([映射[操作]]))
            输出.write(self.编码无符号32位(指令[1]))
        elif 操作 in ('block', 'loop', 'if'):
            映射 = {'block': 0x02, 'loop': 0x03, 'if': 0x04}
            输出.write(bytes([映射[操作]]))
            # 块类型
            if 指令[1]:
                输出.write(bytes([self.类型映射[指令[1]].value]))
            else:
                输出.write(bytes([0x40]))  # 空块
        elif 操作 == 'else':
            输出.write(bytes([0x05]))
        elif 操作 == 'end':
            输出.write(bytes([0x0B]))
        elif 操作 in self.指令映射:
            输出.write(bytes([self.指令映射[操作].value]))
        else:
            raise ValueError(f"未知指令: {操作}")


def 汇编(源码: str) -> bytes:
    """便捷的汇编函数"""
    汇编器 = Wasm汇编器()
    return 汇编器.解析(源码)
