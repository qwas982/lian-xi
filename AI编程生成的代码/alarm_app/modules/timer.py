"""
计时器模块

负责倒计时功能，支持设置小时、分钟、秒，以及开始、暂停、重置操作。
支持创建多个倒计时实例。
是应用的核心功能模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import tkinter as tk
from tkinter import ttk
import time
import threading
from utils.notification import 通知


class 计时器项:
    """计时器项类
    
    负责管理单个倒计时的状态和界面。
    """
    
    def __init__(self, 父框架, 通知):
        """初始化计时器项
        
        Args:
            父框架: 父框架对象
            通知: 通知对象
        """
        self.父框架 = 父框架
        self.通知 = 通知
        
        # 计时器状态
        self.正在运行 = False
        self.已暂停 = False
        self.开始时间 = 0
        self.已用时间 = 0
        self.目标时间 = 0  # 目标时间（秒）
        
        # 创建UI组件
        self.创建界面()
    
    def 创建界面(self):
        """创建计时器项界面
        
        创建单个计时器的所有UI组件，包括时间输入、显示和控制按钮。
        """
        # 创建计时器项框架
        self.计时器框架 = ttk.Frame(self.父框架, padding="20", relief=tk.RAISED, borderwidth=2)
        self.计时器框架.pack(fill=tk.X, pady=(0, 20))
        
        # 时间输入框架
        self.时间输入框架 = ttk.Frame(self.计时器框架)
        self.时间输入框架.pack(fill=tk.X, pady=(0, 20))
        
        # 小时输入
        self.小时变量 = tk.StringVar(value="00")
        ttk.Label(self.时间输入框架, text="小时:", font=("宋体", 12)).pack(side=tk.LEFT, padx=(0, 10))
        ttk.Entry(self.时间输入框架, textvariable=self.小时变量, width=8, font=("宋体", 12)).pack(side=tk.LEFT, padx=(0, 20))
        
        # 分钟输入
        self.分钟变量 = tk.StringVar(value="05")
        ttk.Label(self.时间输入框架, text="分钟:", font=("宋体", 12)).pack(side=tk.LEFT, padx=(0, 10))
        ttk.Entry(self.时间输入框架, textvariable=self.分钟变量, width=8, font=("宋体", 12)).pack(side=tk.LEFT, padx=(0, 20))
        
        # 秒输入
        self.秒变量 = tk.StringVar(value="00")
        ttk.Label(self.时间输入框架, text="秒:", font=("宋体", 12)).pack(side=tk.LEFT, padx=(0, 10))
        ttk.Entry(self.时间输入框架, textvariable=self.秒变量, width=8, font=("宋体", 12)).pack(side=tk.LEFT)
        
        # 计时显示框架
        self.计时显示框架 = ttk.Frame(self.计时器框架)
        self.计时显示框架.pack(fill=tk.X, pady=(0, 20))
        
        # 计时显示
        self.计时标签 = ttk.Label(
            self.计时显示框架, 
            text="00:00:00", 
            font=("Microsoft YaHei", 72, "bold")
        )
        self.计时标签.pack(expand=True)
        
        # 按钮框架
        self.按钮框架 = ttk.Frame(self.计时器框架)
        self.按钮框架.pack(fill=tk.X)
        
        # 开始按钮
        self.开始按钮 = ttk.Button(
            self.按钮框架, 
            text="开始", 
            command=self.开始计时器,
            padding=(15, 8)
        )
        self.开始按钮.pack(side=tk.LEFT, padx=(0, 15))
        
        # 暂停按钮
        self.暂停按钮 = ttk.Button(
            self.按钮框架, 
            text="暂停", 
            command=self.暂停计时器, 
            state=tk.DISABLED,
            padding=(15, 8)
        )
        self.暂停按钮.pack(side=tk.LEFT, padx=(0, 15))
        
        # 重置按钮
        self.重置按钮 = ttk.Button(
            self.按钮框架, 
            text="重置", 
            command=self.重置计时器,
            padding=(15, 8)
        )
        self.重置按钮.pack(side=tk.LEFT, padx=(0, 15))
        
        # 删除按钮
        self.删除按钮 = ttk.Button(
            self.按钮框架, 
            text="删除", 
            command=self.删除计时器,
            padding=(15, 8)
        )
        self.删除按钮.pack(side=tk.RIGHT)
    
    def 开始计时器(self):
        """开始计时器
        
        解析用户输入的时间，验证有效性，然后开始倒计时。
        包含异常处理，确保启动过程不会中断应用运行。
        """
        try:
            if not self.正在运行:
                # 解析目标时间
                try:
                    小时 = int(self.小时变量.get())
                    分钟 = int(self.分钟变量.get())
                    秒 = int(self.秒变量.get())
                    self.目标时间 = 小时 * 3600 + 分钟 * 60 + 秒
                    
                    if self.目标时间 <= 0:
                        tk.messagebox.showerror("错误", "请设置有效的时间")
                        return
                    
                except ValueError:
                    tk.messagebox.showerror("错误", "请输入有效的数字")
                    return
                
                # 开始计时
                self.正在运行 = True
                self.已暂停 = False
                self.开始时间 = time.time()
                self.已用时间 = 0
                
                # 更新按钮状态
                self.开始按钮.config(state=tk.DISABLED)
                self.暂停按钮.config(state=tk.NORMAL)
                self.重置按钮.config(state=tk.NORMAL)
                self.删除按钮.config(state=tk.DISABLED)
                
                # 禁用时间输入
                for 组件 in self.时间输入框架.winfo_children():
                    if isinstance(组件, ttk.Entry):
                        组件.config(state=tk.DISABLED)
                
                # 开始倒计时
                self.更新计时器()
        except Exception as e:
            print(f"开始计时器失败: {e}")
            tk.messagebox.showerror("错误", f"开始计时器失败: {e}")
    
    def 暂停计时器(self):
        """暂停/继续计时器
        
        切换计时器的暂停状态，暂停时保存已用时间，继续时重新开始计时。
        """
        try:
            if self.正在运行:
                self.已暂停 = not self.已暂停
                if self.已暂停:
                    # 暂停
                    self.已用时间 += time.time() - self.开始时间
                    self.暂停按钮.config(text="继续")
                else:
                    # 继续
                    self.开始时间 = time.time()
                    self.暂停按钮.config(text="暂停")
                    self.更新计时器()
        except Exception as e:
            print(f"暂停计时器失败: {e}")
            tk.messagebox.showerror("错误", f"暂停计时器失败: {e}")
    
    def 重置计时器(self):
        """重置计时器
        
        重置计时器的状态和显示，恢复初始状态。
        """
        try:
            self.正在运行 = False
            self.已暂停 = False
            self.已用时间 = 0
            self.目标时间 = 0
            
            # 重置显示
            self.计时标签.config(text="00:00:00")
            
            # 重置按钮状态
            self.开始按钮.config(state=tk.NORMAL)
            self.暂停按钮.config(state=tk.DISABLED, text="暂停")
            self.重置按钮.config(state=tk.NORMAL)
            self.删除按钮.config(state=tk.NORMAL)
            
            # 启用时间输入
            for 组件 in self.时间输入框架.winfo_children():
                if isinstance(组件, ttk.Entry):
                    组件.config(state=tk.NORMAL)
        except Exception as e:
            print(f"重置计时器失败: {e}")
            tk.messagebox.showerror("错误", f"重置计时器失败: {e}")
    
    def 更新计时器(self):
        """更新计时器显示
        
        定期更新计时器的显示，计算剩余时间，并检查是否完成。
        包含异常处理，确保更新过程不会中断应用运行。
        """
        try:
            if self.正在运行 and not self.已暂停:
                # 计算已用时间
                当前时间 = time.time()
                self.已用时间 = 当前时间 - self.开始时间
                
                # 计算剩余时间
                剩余时间 = max(0, self.目标时间 - self.已用时间)
                
                # 格式化时间
                小时 = int(剩余时间 // 3600)
                分钟 = int((剩余时间 % 3600) // 60)
                秒 = int(剩余时间 % 60)
                时间字符串 = f"{小时:02d}:{分钟:02d}:{秒:02d}"
                
                # 更新显示
                self.计时标签.config(text=时间字符串)
                
                # 检查是否完成
                if 剩余时间 <= 0:
                    self.完成计时器()
                else:
                    # 继续更新
                    self.父框架.after(100, self.更新计时器)
        except Exception as e:
            print(f"更新计时器失败: {e}")
    
    def 完成计时器(self):
        """计时器完成
        
        处理计时器完成的逻辑，包括显示完成信息、播放提示音和重置状态。
        """
        try:
            self.正在运行 = False
            
            # 显示完成信息
            self.计时标签.config(text="00:00:00")
            
            # 播放提示音和显示通知
            if self.通知:
                threading.Thread(
                    target=self.通知.显示计时器通知
                ).start()
            
            # 重置按钮状态
            self.开始按钮.config(state=tk.NORMAL)
            self.暂停按钮.config(state=tk.DISABLED, text="暂停")
            self.重置按钮.config(state=tk.NORMAL)
            self.删除按钮.config(state=tk.NORMAL)
            
            # 启用时间输入
            for 组件 in self.时间输入框架.winfo_children():
                if isinstance(组件, ttk.Entry):
                    组件.config(state=tk.NORMAL)
        except Exception as e:
            print(f"计时器完成处理失败: {e}")
    
    def 删除计时器(self):
        """删除计时器
        
        从界面中删除当前计时器项。
        """
        try:
            # 停止计时器
            self.正在运行 = False
            self.已暂停 = False
            
            # 移除框架
            self.计时器框架.destroy()
        except Exception as e:
            print(f"删除计时器失败: {e}")


class 计时器模块:
    """计时器模块类
    
    负责管理多个倒计时实例，支持创建、删除计时器。
    """
    
    def __init__(self, 父窗口):
        """初始化计时器模块
        
        Args:
            父窗口: 父窗口对象
        """
        self.父窗口 = 父窗口
        
        try:
            self.通知 = 通知()
        except Exception as e:
            print(f"初始化通知失败: {e}")
            self.通知 = None
        
        # 计时器项列表
        self.计时器项列表 = []
        
        # 创建UI组件
        self.创建界面()
        
        # 添加默认计时器
        self.添加计时器()
    
    def 创建界面(self):
        """创建用户界面
        
        创建计时器模块的主界面，包括添加计时器按钮和计时器列表。
        """
        # 创建主框架
        self.主框架 = ttk.Frame(self.父窗口, padding="40")  # 增大内边距为原来的2倍
        self.主框架.pack(fill=tk.BOTH, expand=True)
        
        # 标题框架
        self.标题框架 = ttk.Frame(self.主框架)
        self.标题框架.pack(fill=tk.X, pady=(0, 20))
        
        # 标题
        ttk.Label(self.标题框架, text="计时器", font=("Microsoft YaHei", 24, "bold")).pack(side=tk.LEFT)
        
        # 添加计时器按钮
        self.添加计时器按钮 = ttk.Button(
            self.标题框架, 
            text="添加计时器", 
            command=self.添加计时器,
            padding=(15, 8)
        )
        self.添加计时器按钮.pack(side=tk.RIGHT)
        
        # 计时器列表框架
        self.计时器列表框架 = ttk.Frame(self.主框架)
        self.计时器列表框架.pack(fill=tk.BOTH, expand=True)
    
    def 添加计时器(self):
        """添加新计时器
        
        创建并添加一个新的计时器项。
        """
        try:
            # 创建新计时器项
            新计时器项 = 计时器项(self.计时器列表框架, self.通知)
            
            # 添加到列表
            self.计时器项列表.append(新计时器项)
        except Exception as e:
            print(f"添加计时器失败: {e}")
            tk.messagebox.showerror("错误", f"添加计时器失败: {e}")
