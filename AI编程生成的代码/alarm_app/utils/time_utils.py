"""
时间工具模块

负责提供时间相关的工具函数，包括时间格式化、解析、计算等功能。
是应用的核心工具模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import time
from datetime import datetime, timedelta


class 时间工具:
    """时间工具类
    
    提供时间相关的工具函数，包括时间格式化、解析、计算等功能。
    所有方法均为静态方法，可以直接通过类名调用。
    """
    
    # 星期名映射（英文到中文）
    星期映射 = {
        "周一": "星期一",
        "周二": "星期二",
        "周三": "星期三",
        "周四": "星期四",
        "周五": "星期五",
        "周六": "星期六",
        "周末": "星期日"
    }
    
    @staticmethod
    def 格式化时间(秒数):
        """格式化时间
        
        将秒数格式化为 HH:MM:SS.MMM 格式的时间字符串。
        包含异常处理，确保格式化过程不会中断应用运行。
        
        Args:
            秒数: 秒数（浮点数）
            
        Returns:
            str: 格式化后的时间字符串
        """
        try:
            小时 = int(秒数 // 3600)
            分钟 = int((秒数 % 3600) // 60)
            秒 = int(秒数 % 60)
            毫秒 = int((秒数 % 1) * 1000)
            
            return f"{小时:02d}:{分钟:02d}:{秒:02d}.{毫秒:03d}"
        except Exception as e:
            print(f"格式化时间失败: {e}")
            return "00:00:00.000"
    
    @staticmethod
    def 解析时间(时间字符串):
        """解析时间字符串
        
        解析 HH:MM 格式的时间字符串，返回时间结构体。
        包含异常处理，确保解析过程不会中断应用运行。
        
        Args:
            时间字符串: 时间字符串（HH:MM 格式）
            
        Returns:
            time.struct_time: 时间结构体，如果解析失败则返回 None
        """
        try:
            return time.strptime(时间字符串, "%H:%M")
        except ValueError:
            return None
        except Exception as e:
            print(f"解析时间失败: {e}")
            return None
    
    @staticmethod
    def 获取当前时间():
        """获取当前时间
        
        获取当前时间，格式为 HH:MM:SS。
        包含异常处理，确保获取过程不会中断应用运行。
        
        Returns:
            str: 当前时间字符串
        """
        try:
            return time.strftime("%H:%M:%S")
        except Exception as e:
            print(f"获取当前时间失败: {e}")
            return "00:00:00"
    
    @staticmethod
    def 获取当前日期():
        """获取当前日期
        
        获取当前日期，格式为 YYYY-MM-DD。
        包含异常处理，确保获取过程不会中断应用运行。
        
        Returns:
            str: 当前日期字符串
        """
        try:
            return time.strftime("%Y-%m-%d")
        except Exception as e:
            print(f"获取当前日期失败: {e}")
            return "2000-01-01"
    
    @staticmethod
    def 获取当前星期(语言="en"):
        """获取当前星期
        
        获取当前星期几，支持英文和中文输出。
        包含异常处理，确保获取过程不会中断应用运行。
        
        Args:
            语言: 语言，"en" 为英文，"zh" 为中文
            
        Returns:
            str: 当前星期几
        """
        try:
            星期 = time.strftime("%A")
            if 语言 == "zh" and 星期 in 时间工具.星期映射:
                return 时间工具.星期映射[星期]
            return 星期
        except Exception as e:
            print(f"获取当前星期失败: {e}")
            return "Monday"
    
    @staticmethod
    def 计算时间差(开始时间, 结束时间):
        """计算时间差
        
        计算两个时间之间的差值（秒），支持跨天计算。
        包含异常处理，确保计算过程不会中断应用运行。
        
        Args:
            开始时间: 开始时间（HH:MM:SS 格式）
            结束时间: 结束时间（HH:MM:SS 格式）
            
        Returns:
            float: 时间差（秒）
        """
        try:
            开始 = datetime.strptime(开始时间, "%H:%M:%S")
            结束 = datetime.strptime(结束时间, "%H:%M:%S")
            if 结束 < 开始:
                结束 += timedelta(days=1)
            return (结束 - 开始).total_seconds()
        except Exception as e:
            print(f"计算时间差失败: {e}")
            return 0
    
    @staticmethod
    def 是否为工作日(星期):
        """判断是否为工作日
        
        判断指定的星期几是否为工作日（周一至周五）。
        支持中文和英文星期名。
        包含异常处理，确保判断过程不会中断应用运行。
        
        Args:
            星期: 星期几
            
        Returns:
            bool: 是否为工作日
        """
        try:
            # 支持中文和英文星期名
            if 星期 in ["Saturday", "Sunday", "星期六", "星期日"]:
                return False
            return True
        except Exception as e:
            print(f"判断工作日失败: {e}")
            return True
    
    @staticmethod
    def 是否为周末(星期):
        """判断是否为周末
        
        判断指定的星期几是否为周末（周六和周日）。
        支持中文和英文星期名。
        包含异常处理，确保判断过程不会中断应用运行。
        
        Args:
            星期: 星期几
            
        Returns:
            bool: 是否为周末
        """
        try:
            # 支持中文和英文星期名
            if 星期 in ["Saturday", "Sunday", "星期六", "星期日"]:
                return True
            return False
        except Exception as e:
            print(f"判断周末失败: {e}")
            return False
    
    @staticmethod
    def 获取时区时间(时区偏移):
        """获取指定时区的时间
        
        根据时区偏移计算并返回指定时区的当前时间。
        包含异常处理，确保计算过程不会中断应用运行。
        
        Args:
            时区偏移: 时区偏移（小时）
            
        Returns:
            datetime: 指定时区的当前时间
        """
        try:
            当前时间 = datetime.utcnow()
            本地时间 = 当前时间 + timedelta(hours=时区偏移)
            return 本地时间
        except Exception as e:
            print(f"获取时区时间失败: {e}")
            return datetime.utcnow()
