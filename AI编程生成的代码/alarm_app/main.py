"""
Windows 10 闹钟应用主入口

这是一个完整复刻 Windows 10 闹钟应用的 Python 实现，使用 tkinter 作为 GUI 库。
应用包含四大模块：闹钟、时钟、计时器和秒表。

作者: 个性化改造版
版本: 1.0.0
"""

import tkinter as tk
from tkinter import ttk, messagebox
import sys
import os
import logging

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('alarm_app.log'),
        logging.StreamHandler()
    ]
)
日志 = logging.getLogger(__name__)

# 添加项目路径到系统路径
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

# 导入模块
from modules.alarm import 闹钟模块
from modules.clock import 时钟模块
from modules.timer import 计时器模块
from modules.stopwatch import 秒表模块


class 闹钟应用:
    """主应用类
    
    负责创建主窗口、标签页导航，并初始化各个功能模块。
    实现了应用的核心架构和模块管理。
    """
    
    def __init__(self, 根窗口):
        """初始化主应用
        
        Args:
            根窗口: tkinter 根窗口对象
        """
        # 初始化根窗口
        self.根窗口 = 根窗口
        self.根窗口.title("闹钟和时钟")
        self.根窗口.geometry("1000x800")  # 增大窗口大小为原来的2倍
        self.根窗口.resizable(True, True)
        
        # 设置窗口关闭事件
        self.根窗口.protocol("WM_DELETE_WINDOW", self.关闭窗口)
        
        # 设置窗口图标（可选）
        # self.根窗口.iconbitmap("icon.ico")
        
        # 初始化应用组件
        self.创建主框架()
        self.创建标签页()
        self.初始化模块()
        self.启动闹钟检查()
        
        日志.info("应用初始化完成")
    
    def 创建主框架(self):
        """创建主框架
        
        创建应用的主容器框架，设置适当的内边距。
        """
        self.主框架 = ttk.Frame(self.根窗口, padding="20")  # 增大内边距为原来的2倍
        self.主框架.pack(fill=tk.BOTH, expand=True)
    
    def 创建标签页(self):
        """创建标签页导航
        
        创建包含四个功能模块的标签页控件，分别是闹钟、时钟、计时器和秒表。
        """
        # 创建标签页控件
        self.标签页 = ttk.Notebook(self.主框架)
        self.标签页.pack(fill=tk.BOTH, expand=True)
        
        # 创建标签页
        self.闹钟框架 = ttk.Frame(self.标签页)
        self.时钟框架 = ttk.Frame(self.标签页)
        self.计时器框架 = ttk.Frame(self.标签页)
        self.秒表框架 = ttk.Frame(self.标签页)
        
        # 添加标签页到notebook
        self.标签页.add(self.闹钟框架, text="闹钟")
        self.标签页.add(self.时钟框架, text="时钟")
        self.标签页.add(self.计时器框架, text="计时器")
        self.标签页.add(self.秒表框架, text="秒表")
    
    def 初始化模块(self):
        """初始化各个功能模块
        
        分别初始化闹钟、时钟、计时器和秒表模块，处理可能的异常。
        """
        try:
            日志.info("开始初始化功能模块")
            
            # 初始化闹钟模块
            self.闹钟模块 = 闹钟模块(self.闹钟框架)
            
            # 初始化时钟模块
            self.时钟模块 = 时钟模块(self.时钟框架)
            
            # 初始化计时器模块
            self.计时器模块 = 计时器模块(self.计时器框架)
            
            # 初始化秒表模块
            self.秒表模块 = 秒表模块(self.秒表框架)
            
            日志.info("功能模块初始化成功")
        except Exception as e:
            错误信息 = f"初始化模块失败: {e}"
            日志.error(错误信息)
            messagebox.showerror("错误", 错误信息)
    
    def 启动闹钟检查(self):
        """启动闹钟检查
        
        启动周期性的闹钟检查任务。
        """
        self.检查闹钟()
    
    def 检查闹钟(self):
        """定期检查闹钟
        
        每60秒检查一次闹钟状态，触发需要响铃的闹钟。
        包含异常处理，确保检查过程不会中断应用运行。
        """
        try:
            if hasattr(self, '闹钟模块'):
                self.闹钟模块.检查闹钟()
        except Exception as e:
            错误信息 = f"检查闹钟失败: {e}"
            日志.error(错误信息)
        finally:
            # 每60秒检查一次
            self.根窗口.after(60000, self.检查闹钟)
    
    def 关闭窗口(self):
        """窗口关闭事件处理
        
        处理应用关闭时的清理工作，记录关闭日志。
        """
        日志.info("应用正在关闭...")
        
        # 这里可以添加一些清理工作，比如保存配置等
        # 例如：if hasattr(self, '闹钟模块'): self.闹钟模块.保存配置()
        
        self.根窗口.destroy()
        日志.info("应用已关闭")


def 主函数():
    """应用主函数
    
    应用的核心入口函数，负责创建根窗口和应用实例。
    """
    日志.info("应用启动")
    try:
        根窗口 = tk.Tk()
        应用 = 闹钟应用(根窗口)
        根窗口.mainloop()
    except Exception as e:
        错误信息 = f"应用运行失败: {e}"
        日志.error(错误信息)
        messagebox.showerror("错误", 错误信息)
    finally:
        日志.info("应用退出")


if __name__ == "__main__":
    """应用入口点"""
    主函数()
