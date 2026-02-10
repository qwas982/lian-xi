"""
闹钟模块

负责闹钟的创建、编辑、删除、触发等功能。
是应用的核心功能模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import tkinter as tk
from tkinter import ttk, messagebox
import time
import threading
from utils.storage import 存储
from utils.notification import 通知


class 闹钟模块:
    """闹钟模块类
    
    负责管理闹钟的整个生命周期，包括创建、编辑、删除、触发等功能。
    """
    
    def __init__(self, 父窗口):
        """初始化闹钟模块
        
        Args:
            父窗口: 父窗口对象
        """
        self.父窗口 = 父窗口
        self.存储 = 存储()
        self.通知 = 通知()
        
        try:
            # 加载闹钟数据
            self.闹钟列表 = self.存储.加载闹钟()
            # 按时间排序闹钟
            self.排序闹钟()
        except Exception as e:
            print(f"加载闹钟失败: {e}")
            self.闹钟列表 = []
        
        # 创建UI组件
        self.创建界面()
        
        # 刷新闹钟列表
        self.刷新闹钟列表()
    
    def 创建界面(self):
        """创建用户界面
        
        创建闹钟模块的所有UI组件，包括主框架、通知栏、闹钟列表和按钮。
        """
        # 创建主框架
        self.主框架 = ttk.Frame(self.父窗口, padding="20")  # 增大内边距为原来的2倍
        self.主框架.pack(fill=tk.BOTH, expand=True)
        
        # 通知提示栏
        self.通知栏 = ttk.Label(
            self.主框架, 
            text="通知只在电脑处于唤醒状态时显示。", 
            font=("宋体", 18),  # 增大字体为原来的2倍
            foreground="gray"
        )
        self.通知栏.pack(fill=tk.X, pady=(0, 20))  # 增大边距为原来的2倍
        
        # 闹钟列表框架
        self.闹钟列表框架 = ttk.Frame(self.主框架)
        self.闹钟列表框架.pack(fill=tk.BOTH, expand=True)
        
        # 底部按钮框架
        self.底部框架 = ttk.Frame(self.主框架)
        self.底部框架.pack(fill=tk.X, pady=(20, 0))  # 增大边距为原来的2倍
        
        # 添加闹钟按钮
        self.添加闹钟按钮 = ttk.Button(
            self.底部框架, 
            text="+ 添加闹钟", 
            command=self.添加闹钟,
            padding=(20, 10)  # 增大按钮大小
        )
        self.添加闹钟按钮.pack(side=tk.LEFT, padx=(0, 20))  # 增大边距为原来的2倍
        
        # 更多选项按钮
        self.更多按钮 = ttk.Button(
            self.底部框架, 
            text="...", 
            width=6,  # 增大按钮大小为原来的2倍
            command=self.显示更多选项,
            padding=(10, 10)  # 增大按钮大小
        )
        self.更多按钮.pack(side=tk.RIGHT)
    
    def 排序闹钟(self):
        """按时间排序闹钟
        
        根据闹钟的时间属性对闹钟列表进行排序，确保闹钟按时间顺序显示。
        """
        try:
            self.闹钟列表.sort(key=lambda x: x["time"])
        except Exception as e:
            print(f"排序闹钟失败: {e}")
    
    def 刷新闹钟列表(self):
        """刷新闹钟列表
        
        清空现有的闹钟列表，然后重新创建所有闹钟项目的UI组件。
        """
        # 清空现有闹钟
        for 组件 in self.闹钟列表框架.winfo_children():
            组件.destroy()
        
        # 显示闹钟
        for 闹钟 in self.闹钟列表:
            self.创建闹钟项目(闹钟)
    
    def 创建闹钟项目(self, 闹钟):
        """创建闹钟项目
        
        为每个闹钟创建一个UI项目，包括时间、名称、重复设置、开关、编辑和删除按钮。
        
        Args:
            闹钟: 闹钟字典对象
        """
        闹钟框架 = ttk.Frame(self.闹钟列表框架, padding="20")  # 增大内边距为原来的2倍
        闹钟框架.pack(fill=tk.X, pady=(0, 20), ipadx=10, ipady=10)  # 增大边距和内边距为原来的2倍
        
        # 闹钟时间和名称
        时间标签 = ttk.Label(
            闹钟框架, 
            text=闹钟["time"], 
            font=("宋体", 28, "bold")  # 增大字体为原来的2倍
        )
        时间标签.pack(side=tk.LEFT, padx=(0, 40))  # 增大边距为原来的2倍
        
        名称标签 = ttk.Label(
            闹钟框架, 
            text=闹钟["name"], 
            font=("宋体", 24)  # 增大字体为原来的2倍
        )
        名称标签.pack(side=tk.LEFT, padx=(0, 40))  # 增大边距为原来的2倍
        
        # 重复设置
        重复标签 = ttk.Label(
            闹钟框架, 
            text=闹钟["repeat"], 
            font=("宋体", 20),  # 增大字体为原来的2倍
            foreground="gray"
        )
        重复标签.pack(side=tk.LEFT, padx=(0, 40))  # 增大边距为原来的2倍
        
        # 编辑按钮
        编辑按钮 = ttk.Button(
            闹钟框架, 
            text="编辑", 
            command=lambda a=闹钟: self.编辑闹钟(a),
            padding=(10, 5)  # 增大按钮大小
        )
        编辑按钮.pack(side=tk.RIGHT, padx=(0, 10))
        
        # 删除按钮
        删除按钮 = ttk.Button(
            闹钟框架, 
            text="删除", 
            command=lambda a=闹钟: self.删除闹钟(a),
            padding=(10, 5)  # 增大按钮大小
        )
        删除按钮.pack(side=tk.RIGHT, padx=(0, 10))
        
        # 开关
        开关变量 = tk.BooleanVar(value=闹钟["enabled"])
        开关 = ttk.Checkbutton(
            闹钟框架, 
            variable=开关变量,
            command=lambda a=闹钟, v=开关变量: self.切换闹钟状态(a, v)
        )
        开关.pack(side=tk.RIGHT, padx=(10, 0))
        
        # 开关状态标签
        状态标签 = ttk.Label(
            闹钟框架, 
            text="开" if 闹钟["enabled"] else "关",
            font=("宋体", 20)  # 增大字体为原来的2倍
        )
        状态标签.pack(side=tk.RIGHT)
    
    def 切换闹钟状态(self, 闹钟, 开关变量):
        """切换闹钟开关状态
        
        根据开关的状态更新闹钟的启用状态，并保存到存储中。
        
        Args:
            闹钟: 闹钟字典对象
            开关变量: 开关的布尔变量
        """
        try:
            闹钟["enabled"] = 开关变量.get()
            self.存储.保存闹钟(self.闹钟列表)
            # 不需要刷新整个列表，只需要更新状态标签
        except Exception as e:
            print(f"切换闹钟状态失败: {e}")
            messagebox.showerror("错误", f"切换闹钟状态失败: {e}")
    
    def 添加闹钟(self):
        """添加新闹钟
        
        创建一个新的闹钟添加窗口，允许用户设置时间、名称和重复选项。
        """
        # 创建添加闹钟窗口
        添加窗口 = tk.Toplevel(self.父窗口)
        添加窗口.title("添加闹钟")
        添加窗口.geometry("600x400")  # 增大窗口大小为原来的2倍
        添加窗口.resizable(False, False)
        
        # 居中显示
        添加窗口.transient(self.父窗口)
        添加窗口.grab_set()
        
        # 时间输入
        ttk.Label(添加窗口, text="时间 (HH:MM):", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        时间变量 = tk.StringVar(value="07:00")
        时间输入 = ttk.Entry(添加窗口, textvariable=时间变量, font=("宋体", 16), width=20)  # 增大字体和宽度
        时间输入.pack(pady=(0, 20))  # 增大边距
        
        # 名称输入
        ttk.Label(添加窗口, text="名称:", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        名称变量 = tk.StringVar(value="起床闹钟")
        名称输入 = ttk.Entry(添加窗口, textvariable=名称变量, font=("宋体", 16), width=20)  # 增大字体和宽度
        名称输入.pack(pady=(0, 20))  # 增大边距
        
        # 重复设置
        ttk.Label(添加窗口, text="重复:", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        重复变量 = tk.StringVar(value="仅一次")
        重复下拉框 = ttk.Combobox(
            添加窗口, 
            textvariable=重复变量,
            values=["仅一次", "每天", "工作日", "周末"],
            font=("宋体", 16),  # 增大字体
            width=18  # 增大宽度
        )
        重复下拉框.pack(pady=(0, 20))  # 增大边距
        
        # 按钮框架
        按钮框架 = ttk.Frame(添加窗口)
        按钮框架.pack(fill=tk.X, pady=(20, 0))  # 增大边距
        
        # 确定按钮
        ttk.Button(
            按钮框架, 
            text="确定", 
            command=lambda: self.保存新闹钟(
                添加窗口, 时间变量.get(), 名称变量.get(), 重复变量.get()
            ),
            padding=(20, 10)  # 增大按钮大小
        ).pack(side=tk.RIGHT, padx=(20, 0))  # 增大边距
        
        # 取消按钮
        ttk.Button(
            按钮框架, 
            text="取消", 
            command=添加窗口.destroy,
            padding=(20, 10)  # 增大按钮大小
        ).pack(side=tk.RIGHT)
    
    def 保存新闹钟(self, 窗口, 时间字符串, 名称, 重复):
        """保存新闹钟
        
        验证时间格式，创建新闹钟对象，保存到存储中，并刷新闹钟列表。
        
        Args:
            窗口: 窗口对象
            时间字符串: 时间字符串 (HH:MM)
            名称: 闹钟名称
            重复: 重复选项
        """
        try:
            # 验证时间格式
            time.strptime(时间字符串, "%H:%M")
            
            # 创建新闹钟
            新闹钟 = {
                "id": int(time.time()),
                "time": 时间字符串,
                "name": 名称,
                "repeat": 重复,
                "enabled": True
            }
            
            # 添加到闹钟列表
            self.闹钟列表.append(新闹钟)
            # 排序闹钟
            self.排序闹钟()
            self.存储.保存闹钟(self.闹钟列表)
            
            # 刷新列表
            self.刷新闹钟列表()
            窗口.destroy()
        except ValueError:
            messagebox.showerror("错误", "时间格式不正确，请使用 HH:MM 格式")
        except Exception as e:
            print(f"保存闹钟失败: {e}")
            messagebox.showerror("错误", f"保存闹钟失败: {e}")
    
    def 编辑闹钟(self, 闹钟):
        """编辑闹钟
        
        创建一个闹钟编辑窗口，允许用户修改现有闹钟的时间、名称和重复选项。
        
        Args:
            闹钟: 要编辑的闹钟字典对象
        """
        # 创建编辑闹钟窗口
        编辑窗口 = tk.Toplevel(self.父窗口)
        编辑窗口.title("编辑闹钟")
        编辑窗口.geometry("600x400")  # 增大窗口大小为原来的2倍
        编辑窗口.resizable(False, False)
        
        # 居中显示
        编辑窗口.transient(self.父窗口)
        编辑窗口.grab_set()
        
        # 时间输入
        ttk.Label(编辑窗口, text="时间 (HH:MM):", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        时间变量 = tk.StringVar(value=闹钟["time"])
        时间输入 = ttk.Entry(编辑窗口, textvariable=时间变量, font=("宋体", 16), width=20)  # 增大字体和宽度
        时间输入.pack(pady=(0, 20))  # 增大边距
        
        # 名称输入
        ttk.Label(编辑窗口, text="名称:", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        名称变量 = tk.StringVar(value=闹钟["name"])
        名称输入 = ttk.Entry(编辑窗口, textvariable=名称变量, font=("宋体", 16), width=20)  # 增大字体和宽度
        名称输入.pack(pady=(0, 20))  # 增大边距
        
        # 重复设置
        ttk.Label(编辑窗口, text="重复:", font=("宋体", 16)).pack(pady=(20, 10))  # 增大字体和边距
        重复变量 = tk.StringVar(value=闹钟["repeat"])
        重复下拉框 = ttk.Combobox(
            编辑窗口, 
            textvariable=重复变量,
            values=["仅一次", "每天", "工作日", "周末"],
            font=("宋体", 16),  # 增大字体
            width=18  # 增大宽度
        )
        重复下拉框.pack(pady=(0, 20))  # 增大边距
        
        # 按钮框架
        按钮框架 = ttk.Frame(编辑窗口)
        按钮框架.pack(fill=tk.X, pady=(20, 0))  # 增大边距
        
        # 确定按钮
        ttk.Button(
            按钮框架, 
            text="确定", 
            command=lambda: self.保存编辑闹钟(
                编辑窗口, 闹钟, 时间变量.get(), 名称变量.get(), 重复变量.get()
            ),
            padding=(20, 10)  # 增大按钮大小
        ).pack(side=tk.RIGHT, padx=(20, 0))  # 增大边距
        
        # 取消按钮
        ttk.Button(
            按钮框架, 
            text="取消", 
            command=编辑窗口.destroy,
            padding=(20, 10)  # 增大按钮大小
        ).pack(side=tk.RIGHT)
    
    def 保存编辑闹钟(self, 窗口, 闹钟, 时间字符串, 名称, 重复):
        """保存编辑后的闹钟
        
        验证时间格式，更新闹钟对象，保存到存储中，并刷新闹钟列表。
        
        Args:
            窗口: 窗口对象
            闹钟: 要更新的闹钟字典对象
            时间字符串: 时间字符串 (HH:MM)
            名称: 闹钟名称
            重复: 重复选项
        """
        try:
            # 验证时间格式
            time.strptime(时间字符串, "%H:%M")
            
            # 更新闹钟
            闹钟["time"] = 时间字符串
            闹钟["name"] = 名称
            闹钟["repeat"] = 重复
            
            # 排序闹钟
            self.排序闹钟()
            self.存储.保存闹钟(self.闹钟列表)
            
            # 刷新列表
            self.刷新闹钟列表()
            窗口.destroy()
        except ValueError:
            messagebox.showerror("错误", "时间格式不正确，请使用 HH:MM 格式")
        except Exception as e:
            print(f"保存编辑闹钟失败: {e}")
            messagebox.showerror("错误", f"保存编辑闹钟失败: {e}")
    
    def 删除闹钟(self, 闹钟):
        """删除闹钟
        
        显示确认对话框，确认后删除闹钟并刷新列表。
        
        Args:
            闹钟: 要删除的闹钟字典对象
        """
        try:
            if messagebox.askyesno("确认删除", f"确定要删除闹钟 '{闹钟['name']}' 吗？"):
                self.闹钟列表.remove(闹钟)
                self.存储.保存闹钟(self.闹钟列表)
                self.刷新闹钟列表()
        except Exception as e:
            print(f"删除闹钟失败: {e}")
            messagebox.showerror("错误", f"删除闹钟失败: {e}")
    
    def 显示更多选项(self):
        """显示更多选项
        
        显示一个信息对话框，提示更多选项功能待实现。
        """
        messagebox.showinfo("更多选项", "更多选项功能待实现")
    
    def 检查闹钟(self):
        """检查闹钟
        
        检查所有闹钟是否到了触发时间，并触发需要响铃的闹钟。
        """
        try:
            当前时间 = time.strftime("%H:%M")
            当前星期 = time.strftime("%A")
            
            for 闹钟 in self.闹钟列表:
                if 闹钟["enabled"] and 闹钟["time"] == 当前时间:
                    # 检查重复设置
                    if self.应该触发(闹钟, 当前星期):
                        # 触发闹钟
                        self.触发闹钟(闹钟)
        except Exception as e:
            print(f"检查闹钟失败: {e}")
    
    def 应该触发(self, 闹钟, 当前星期):
        """判断闹钟是否应该触发
        
        根据闹钟的重复设置和当前星期几，判断闹钟是否应该在当前时间触发。
        
        Args:
            闹钟: 闹钟字典对象
            当前星期: 当前星期几 (英文)
            
        Returns:
            bool: 是否应该触发闹钟
        """
        try:
            if 闹钟["repeat"] == "仅一次":
                # 仅一次的闹钟触发后禁用
                闹钟["enabled"] = False
                self.存储.保存闹钟(self.闹钟列表)
                return True
            elif 闹钟["repeat"] == "每天":
                return True
            elif 闹钟["repeat"] == "工作日":
                return 当前星期 not in ["Saturday", "Sunday"]
            elif 闹钟["repeat"] == "周末":
                return 当前星期 in ["Saturday", "Sunday"]
        except Exception as e:
            print(f"判断闹钟触发失败: {e}")
        return False
    
    def 触发闹钟(self, 闹钟):
        """触发闹钟
        
        在新线程中播放提示音并显示闹钟通知，避免阻塞UI线程。
        
        Args:
            闹钟: 要触发的闹钟字典对象
        """
        try:
            # 在新线程中播放声音和显示通知
            threading.Thread(
                target=self.通知.显示闹钟通知, 
                args=(闹钟["name"],)
            ).start()
        except Exception as e:
            print(f"触发闹钟失败: {e}")
