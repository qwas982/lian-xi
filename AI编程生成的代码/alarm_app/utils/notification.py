"""
通知模块

负责管理应用的通知功能，包括闹钟提醒和计时器提醒。
是应用的核心工具模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import os
import sys
import time
from tkinter import messagebox


class 通知:
    """通知类
    
    负责管理应用的通知功能，包括播放提示音和显示通知对话框。
    支持音量控制和通知启用/禁用功能。
    """
    
    def __init__(self):
        """初始化通知
        
        初始化通知设置，包括默认音量和通知启用状态。
        """
        self.音量 = 80
        self.通知启用 = True
    
    def 显示闹钟通知(self, 闹钟名称):
        """显示闹钟通知
        
        播放提示音并显示闹钟提醒对话框。
        如果通知被禁用则跳过。
        包含异常处理，确保通知过程不会中断应用运行。
        
        Args:
            闹钟名称: 闹钟名称
        """
        if not self.通知启用:
            return
        
        try:
            # 播放提示音
            self.播放提示音()
            
            # 显示通知对话框
            messagebox.showinfo(
                "闹钟提醒", 
                f"{闹钟名称}\n时间到了！"
            )
        except Exception as e:
            print(f"显示闹钟通知失败: {e}")
    
    def 显示计时器通知(self):
        """显示计时器通知
        
        播放提示音并显示计时器提醒对话框。
        如果通知被禁用则跳过。
        包含异常处理，确保通知过程不会中断应用运行。
        """
        if not self.通知启用:
            return
        
        try:
            # 播放提示音
            self.播放提示音()
            
            # 显示通知对话框
            messagebox.showinfo(
                "计时器提醒", 
                "计时器时间到了！"
            )
        except Exception as e:
            print(f"显示计时器通知失败: {e}")
    
    def 播放提示音(self):
        """播放提示音
        
        根据当前平台播放提示音，Windows平台使用winsound，
        其他平台显示提示信息。
        根据音量设置调整提示音频率。
        包含异常处理，确保播放过程不会中断应用运行。
        """
        try:
            if sys.platform == "win32":
                # Windows平台
                import winsound
                # 根据音量调整频率
                频率 = int(800 * (self.音量 / 100))
                频率 = max(200, min(1000, 频率))
                # 播放系统提示音
                winsound.MessageBeep(winsound.MB_ICONASTERISK)  # 提示音，1秒
                time.sleep(0.5)
                winsound.MessageBeep(winsound.MB_ICONASTERISK)
            else:
                # 其他平台
                print("提示音功能仅在Windows平台可用")
        except Exception as e:
            print(f"播放提示音失败: {e}")
    
    def 设置音量(self, 音量值):
        """设置音量
        
        设置通知提示音的音量，范围为0-100。
        包含异常处理，确保设置过程不会中断应用运行。
        
        Args:
            音量值: 音量值（0-100）
        """
        try:
            self.音量 = max(0, min(100, 音量值))
        except Exception as e:
            print(f"设置音量失败: {e}")
    
    def 设置通知启用(self, 启用):
        """设置通知是否启用
        
        设置通知功能的启用状态。
        包含异常处理，确保设置过程不会中断应用运行。
        
        Args:
            启用: 是否启用通知
        """
        try:
            self.通知启用 = bool(启用)
        except Exception as e:
            print(f"设置通知启用状态失败: {e}")
