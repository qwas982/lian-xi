"""
存储模块

负责管理应用配置的存储和加载，包括闹钟数据和应用设置。
是应用的核心工具模块之一。

作者: 个性化改造版
版本: 1.0.0
"""

import json
import os
import tempfile


class 存储:
    """存储类
    
    负责管理配置文件的存储和加载，包括闹钟数据和应用设置。
    提供了原子写入、备份恢复等功能，确保配置数据的可靠性。
    """
    
    def __init__(self):
        """初始化存储
        
        初始化配置文件路径，确保配置文件和目录存在。
        包含异常处理，确保初始化过程不会中断应用运行。
        """
        # 确保配置文件路径正确
        try:
            基础目录 = os.path.dirname(os.path.dirname(__file__))
            self.配置文件 = os.path.join(基础目录, "config.json")
            # 确保目录存在
            os.makedirs(基础目录, exist_ok=True)
        except Exception as e:
            print(f"初始化配置文件路径失败: {e}")
            # 使用当前目录作为 fallback
            self.配置文件 = "config.json"
        
        self.确保配置文件存在()
    
    def 确保配置文件存在(self):
        """确保配置文件存在
        
        检查配置文件是否存在，如果不存在则创建默认配置。
        包含异常处理，确保操作过程不会中断应用运行。
        """
        try:
            if not os.path.exists(self.配置文件):
                # 创建默认配置
                默认配置 = {
                    "alarms": [
                        {
                            "id": 1,
                            "time": "07:00",
                            "name": "起床闹钟",
                            "repeat": "仅一次",
                            "enabled": True
                        }
                    ],
                    "settings": {
                        "volume": 80,
                        "notification_enabled": True
                    }
                }
                self.保存配置(默认配置)
        except Exception as e:
            print(f"确保配置文件存在失败: {e}")
    
    def 加载配置(self):
        """加载配置文件
        
        加载配置文件，如果文件格式错误则尝试使用备份文件，
        如果备份文件也失败则返回默认配置。
        包含异常处理，确保加载过程不会中断应用运行。
        
        Returns:
            dict: 配置字典
        """
        try:
            with open(self.配置文件, "r", encoding="utf-8") as f:
                return json.load(f)
        except json.JSONDecodeError as e:
            print(f"配置文件格式错误: {e}")
            # 尝试使用备份文件
            备份文件 = self.配置文件 + ".bak"
            if os.path.exists(备份文件):
                try:
                    with open(备份文件, "r", encoding="utf-8") as f:
                        return json.load(f)
                except Exception as 备份错误:
                    print(f"加载备份文件失败: {备份错误}")
            return {"alarms": [], "settings": {}}
        except Exception as e:
            print(f"加载配置文件失败: {e}")
            return {"alarms": [], "settings": {}}
    
    def 保存配置(self, 配置):
        """保存配置文件
        
        使用原子写入方式保存配置文件，确保写入过程不会损坏文件。
        保存前会备份原有文件，以便在写入失败时恢复。
        包含异常处理，确保保存过程不会中断应用运行。
        
        Args:
            配置: 配置字典
        """
        try:
            # 使用与配置文件相同目录的临时文件，避免跨磁盘移动问题
            配置文件目录 = os.path.dirname(self.配置文件)
            with tempfile.NamedTemporaryFile(mode='w', encoding='utf-8', delete=False, dir=配置文件目录) as 临时文件:
                json.dump(配置, 临时文件, ensure_ascii=False, indent=2)
                临时文件路径 = 临时文件.name
            
            # 备份原文件
            if os.path.exists(self.配置文件):
                备份文件 = self.配置文件 + ".bak"
                try:
                    os.replace(self.配置文件, 备份文件)
                except Exception as 备份错误:
                    print(f"备份配置文件失败: {备份错误}")
            
            # 替换原文件
            os.replace(临时文件路径, self.配置文件)
        except Exception as e:
            print(f"保存配置文件失败: {e}")
            # 清理临时文件
            if '临时文件路径' in locals() and os.path.exists(临时文件路径):
                try:
                    os.unlink(临时文件路径)
                except:
                    pass
    
    def 加载闹钟(self):
        """加载闹钟
        
        从配置文件中加载闹钟数据。
        包含异常处理，确保加载过程不会中断应用运行。
        
        Returns:
            list: 闹钟列表
        """
        try:
            配置 = self.加载配置()
            return 配置.get("alarms", [])
        except Exception as e:
            print(f"加载闹钟失败: {e}")
            return []
    
    def 保存闹钟(self, 闹钟):
        """保存闹钟
        
        将闹钟数据保存到配置文件中。
        包含异常处理，确保保存过程不会中断应用运行。
        
        Args:
            闹钟: 闹钟列表
        """
        try:
            配置 = self.加载配置()
            配置["alarms"] = 闹钟
            self.保存配置(配置)
        except Exception as e:
            print(f"保存闹钟失败: {e}")
    
    def 加载设置(self):
        """加载设置
        
        从配置文件中加载应用设置。
        包含异常处理，确保加载过程不会中断应用运行。
        
        Returns:
            dict: 设置字典
        """
        try:
            配置 = self.加载配置()
            return 配置.get("settings", {})
        except Exception as e:
            print(f"加载设置失败: {e}")
            return {}
    
    def 保存设置(self, 设置):
        """保存设置
        
        将应用设置保存到配置文件中。
        包含异常处理，确保保存过程不会中断应用运行。
        
        Args:
            设置: 设置字典
        """
        try:
            配置 = self.加载配置()
            配置["settings"] = 设置
            self.保存配置(配置)
        except Exception as e:
            print(f"保存设置失败: {e}")
