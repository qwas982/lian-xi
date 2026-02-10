"""
秒表模块

负责秒表功能，支持开始、暂停、继续、重置和记录lap操作。
是应用的核心功能模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import tkinter as tk
from tkinter import ttk
import time


class 秒表模块:
    """秒表模块类
    
    负责管理秒表功能，包括计时、暂停、继续、重置和记录圈速操作。
    """
    
    def __init__(self, 父窗口):
        """初始化秒表模块
        
        Args:
            父窗口: 父窗口对象
        """
        self.父窗口 = 父窗口
        
        # 秒表状态
        self.正在运行 = False
        self.已暂停 = False
        self.开始时间 = 0
        self.已用时间 = 0
        self.圈速列表 = []
        
        # 创建UI组件
        self.创建界面()
    
    def 创建界面(self):
        """创建用户界面
        
        创建秒表模块的所有UI组件，包括时间显示、控制按钮和圈速列表。
        """
        # 创建主框架
        self.主框架 = ttk.Frame(self.父窗口, padding="40")  # 增大内边距为原来的2倍
        self.主框架.pack(fill=tk.BOTH, expand=True)
        
        # 计时显示框架
        self.秒表显示框架 = ttk.Frame(self.主框架)
        self.秒表显示框架.pack(fill=tk.BOTH, expand=True)
        
        # 计时显示
        self.秒表标签 = ttk.Label(
            self.秒表显示框架, 
            text="00:00:00.000", 
            font=("Microsoft YaHei", 96, "bold")  # 增大字体为原来的2倍
        )
        self.秒表标签.pack(expand=True)
        
        # 按钮框架
        self.按钮框架 = ttk.Frame(self.主框架)
        self.按钮框架.pack(fill=tk.X, pady=(40, 0))  # 增大边距为原来的2倍
        
        # 开始按钮
        self.开始按钮 = ttk.Button(
            self.按钮框架, 
            text="开始", 
            command=self.开始秒表,
            padding=(20, 10)  # 增大按钮大小
        )
        self.开始按钮.pack(side=tk.LEFT, padx=(0, 20))  # 增大边距为原来的2倍
        
        # 圈速按钮
        self.圈速按钮 = ttk.Button(
            self.按钮框架, 
            text="圈速", 
            command=self.记录圈速, 
            state=tk.DISABLED,
            padding=(20, 10)  # 增大按钮大小
        )
        self.圈速按钮.pack(side=tk.LEFT, padx=(0, 20))  # 增大边距为原来的2倍
        
        # 重置按钮
        self.重置按钮 = ttk.Button(
            self.按钮框架, 
            text="重置", 
            command=self.重置秒表,
            padding=(20, 10)  # 增大按钮大小
        )
        self.重置按钮.pack(side=tk.LEFT)
        
        # 圈速显示框架
        self.圈速框架 = ttk.Frame(self.主框架)
        self.圈速框架.pack(fill=tk.BOTH, expand=True, pady=(40, 0))  # 增大边距为原来的2倍
        
        # 圈速标题
        self.圈速标题 = ttk.Label(
            self.圈速框架, 
            text="圈速记录", 
            font=("Microsoft YaHei", 24, "bold")  # 增大字体为原来的2倍
        )
        self.圈速标题.pack(fill=tk.X, pady=(0, 20))  # 增大边距为原来的2倍
        
        # 圈速列表
        self.圈速列表框 = tk.Listbox(
            self.圈速框架, 
            font=("Microsoft YaHei", 16),  # 增大字体为原来的2倍
            height=8  # 增大高度为原来的1.6倍
        )
        self.圈速列表框.pack(fill=tk.BOTH, expand=True)
    
    def 开始秒表(self):
        """开始/暂停/继续秒表
        
        根据当前状态切换秒表的运行状态：
        - 未运行时：开始计时
        - 运行中：暂停计时
        - 暂停时：继续计时
        包含异常处理，确保操作过程不会中断应用运行。
        """
        try:
            if not self.正在运行:
                # 开始或继续
                self.正在运行 = True
                self.已暂停 = False
                self.开始时间 = time.time() - self.已用时间
                self.开始按钮.config(text="暂停")
                self.圈速按钮.config(state=tk.NORMAL)
                self.重置按钮.config(state=tk.NORMAL)
                
                # 开始计时
                self.更新秒表()
            else:
                # 暂停
                self.已暂停 = True
                self.正在运行 = False
                self.已用时间 = time.time() - self.开始时间
                self.开始按钮.config(text="继续")
        except Exception as e:
            print(f"开始/暂停秒表失败: {e}")
    
    def 记录圈速(self):
        """记录圈速
        
        记录当前秒表时间作为一个圈速，并显示在圈速列表中。
        包含异常处理，确保记录过程不会中断应用运行。
        """
        try:
            if self.正在运行:
                # 计算当前时间
                当前时间 = time.time() - self.开始时间
                
                # 格式化时间
                格式化时间 = self.格式化时间(当前时间)
                
                # 添加到圈速列表
                圈速编号 = len(self.圈速列表) + 1
                圈速文本 = f"圈速 {圈速编号}: {格式化时间}"
                self.圈速列表.append(圈速文本)
                
                # 更新圈速列表框
                self.圈速列表框.insert(tk.END, 圈速文本)
                self.圈速列表框.see(tk.END)
        except Exception as e:
            print(f"记录圈速失败: {e}")
    
    def 重置秒表(self):
        """重置秒表
        
        重置秒表的状态、显示和圈速列表，恢复初始状态。
        包含异常处理，确保重置过程不会中断应用运行。
        """
        try:
            self.正在运行 = False
            self.已暂停 = False
            self.已用时间 = 0
            self.圈速列表 = []
            
            # 重置显示
            self.秒表标签.config(text="00:00:00.000")
            
            # 重置按钮状态
            self.开始按钮.config(text="开始")
            self.圈速按钮.config(state=tk.DISABLED)
            self.重置按钮.config(state=tk.NORMAL)
            
            # 清空圈速列表
            self.圈速列表框.delete(0, tk.END)
        except Exception as e:
            print(f"重置秒表失败: {e}")
    
    def 更新秒表(self):
        """更新秒表显示
        
        定期更新秒表的显示，计算已用时间并格式化显示。
        包含异常处理，确保更新过程不会中断应用运行。
        """
        try:
            if self.正在运行:
                # 计算已用时间
                当前时间 = time.time() - self.开始时间
                
                # 格式化时间
                格式化时间 = self.格式化时间(当前时间)
                
                # 更新显示
                self.秒表标签.config(text=格式化时间)
                
                # 继续更新
                self.父窗口.after(30, self.更新秒表)  # 每30毫秒更新一次
        except Exception as e:
            print(f"更新秒表失败: {e}")
    
    def 格式化时间(self, 秒数):
        """格式化时间
        
        将秒数格式化为 HH:MM:SS.MMM 格式的时间字符串。
        
        Args:
            秒数: 秒数（浮点数）
            
        Returns:
            str: 格式化后的时间字符串
        """
        小时 = int(秒数 // 3600)
        分钟 = int((秒数 % 3600) // 60)
        秒 = int(秒数 % 60)
        毫秒 = int((秒数 % 1) * 1000)
        
        return f"{小时:02d}:{分钟:02d}:{秒:02d}.{毫秒:03d}"
