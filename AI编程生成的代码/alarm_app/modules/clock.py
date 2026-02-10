"""
时钟模块

负责显示当前时间、日期和世界时钟。
是应用的核心功能模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import tkinter as tk
from tkinter import ttk
import time
from datetime import datetime, timedelta


class 时钟模块:
    """时钟模块类
    
    负责显示当前时间、日期和世界时钟，支持多城市时区显示。
    """
    
    def __init__(self, 父窗口):
        """初始化时钟模块
        
        Args:
            父窗口: 父窗口对象
        """
        self.父窗口 = 父窗口
        
        # 创建UI组件
        self.创建界面()
        
        # 创建示例世界时钟
        self.创建示例世界时钟()
        
        # 开始更新时间
        self.更新时间()
    
    def 创建界面(self):
        """创建用户界面
        
        创建时钟模块的所有UI组件，包括主框架、当前时间显示和世界时钟框架。
        """
        # 创建主框架
        self.主框架 = ttk.Frame(self.父窗口, padding="40")  # 增大内边距为原来的2倍
        self.主框架.pack(fill=tk.BOTH, expand=True)
        
        # 当前时间显示
        self.当前时间框架 = ttk.Frame(self.主框架)
        self.当前时间框架.pack(fill=tk.X, pady=(0, 40))  # 增大边距为原来的2倍
        
        self.时间标签 = ttk.Label(
            self.当前时间框架, 
            text="", 
            font=("Microsoft YaHei", 72, "bold")  # 增大字体为原来的2倍
        )
        self.时间标签.pack(side=tk.LEFT)
        
        self.日期标签 = ttk.Label(
            self.当前时间框架, 
            text="", 
            font=("Microsoft YaHei", 24)  # 增大字体为原来的2倍
        )
        self.日期标签.pack(side=tk.LEFT, padx=(40, 0), pady=(20, 0))  # 增大边距为原来的2倍
        
        # 世界时钟框架
        self.世界时钟框架 = ttk.Frame(self.主框架)
        self.世界时钟框架.pack(fill=tk.BOTH, expand=True)
        
        # 世界时钟标题
        self.世界时钟标题 = ttk.Label(
            self.世界时钟框架, 
            text="世界时钟", 
            font=("Microsoft YaHei", 28, "bold")  # 增大字体为原来的2倍
        )
        self.世界时钟标题.pack(fill=tk.X, pady=(0, 20))  # 增大边距为原来的2倍
    
    def 创建示例世界时钟(self):
        """创建示例世界时钟
        
        创建几个主要城市的世界时钟作为示例，包括北京、纽约、伦敦和东京。
        """
        try:
            self.创建世界时钟("北京", "+8")
            self.创建世界时钟("纽约", "-5")
            self.创建世界时钟("伦敦", "+0")
            self.创建世界时钟("东京", "+9")
        except Exception as e:
            print(f"创建世界时钟失败: {e}")
    
    def 更新时间(self):
        """更新时间显示
        
        每秒更新一次当前时间和所有世界时钟的显示。
        包含异常处理，确保更新过程不会中断应用运行。
        """
        try:
            # 更新当前时间
            当前时间 = time.strftime("%H:%M:%S")
            当前日期 = time.strftime("%Y年%m月%d日 %A")
            self.时间标签.config(text=当前时间)
            self.日期标签.config(text=当前日期)
            
            # 更新世界时钟
            for 组件 in self.世界时钟框架.winfo_children():
                if isinstance(组件, ttk.Frame) and hasattr(组件, "time_label"):
                    try:
                        时区 = 组件.timezone
                        城市 = 组件.city
                        self.更新世界时钟(组件, 城市, 时区)
                    except Exception as e:
                        print(f"更新世界时钟失败: {e}")
        except Exception as e:
            print(f"更新时间失败: {e}")
        finally:
            # 每秒更新一次
            self.父窗口.after(1000, self.更新时间)
    
    def 创建世界时钟(self, 城市, 时区):
        """创建世界时钟
        
        为指定城市创建一个世界时钟UI项目，显示城市名称、时区、时间和日期。
        
        Args:
            城市: 城市名称
            时区: 时区偏移 (例如 "+8", "-5")
        """
        try:
            时钟框架 = ttk.Frame(self.世界时钟框架, padding="20")  # 增大内边距为原来的2倍
            时钟框架.pack(fill=tk.X, pady=(0, 20))  # 增大边距为原来的2倍
            时钟框架.city = 城市
            时钟框架.timezone = 时区
            
            # 城市名称
            城市标签 = ttk.Label(
                时钟框架, 
                text=城市, 
                font=("Microsoft YaHei", 24, "bold"),  # 增大字体为原来的2倍
                width=15  # 增大宽度为原来的1.5倍
            )
            城市标签.pack(side=tk.LEFT, padx=(0, 40))  # 增大边距为原来的2倍
            
            # 时区
            时区标签 = ttk.Label(
                时钟框架, 
                text=f"UTC{时区}", 
                font=("Microsoft YaHei", 20),  # 增大字体为原来的2倍
                foreground="gray",
                width=12  # 增大宽度为原来的1.2倍
            )
            时区标签.pack(side=tk.LEFT, padx=(0, 40))  # 增大边距为原来的2倍
            
            # 时间
            时间标签 = ttk.Label(
                时钟框架, 
                text="", 
                font=("Microsoft YaHei", 28)  # 增大字体为原来的2倍
            )
            时间标签.pack(side=tk.LEFT)
            时钟框架.time_label = 时间标签
            
            # 日期
            日期标签 = ttk.Label(
                时钟框架, 
                text="", 
                font=("Microsoft YaHei", 20),  # 增大字体为原来的2倍
                foreground="gray"
            )
            日期标签.pack(side=tk.LEFT, padx=(40, 0))  # 增大边距为原来的2倍
            时钟框架.date_label = 日期标签
        except Exception as e:
            print(f"创建世界时钟失败: {e}")
    
    def 更新世界时钟(self, 时钟框架, 城市, 时区):
        """更新世界时钟
        
        根据时区偏移计算并更新指定城市的时间和日期显示。
        
        Args:
            时钟框架: 时钟框架对象
            城市: 城市名称
            时区: 时区偏移 (例如 "+8", "-5")
        """
        try:
            # 计算时区偏移
            偏移 = int(时区)
            现在 = datetime.utcnow()
            # 处理时区偏移，避免小时数超出范围
            本地时间 = 现在 + timedelta(hours=偏移)
            
            # 更新时间和日期
            当前时间 = 本地时间.strftime("%H:%M:%S")
            当前日期 = 本地时间.strftime("%Y-%m-%d")
            
            时钟框架.time_label.config(text=当前时间)
            时钟框架.date_label.config(text=当前日期)
        except Exception as e:
            print(f"更新世界时钟失败: {e}")
