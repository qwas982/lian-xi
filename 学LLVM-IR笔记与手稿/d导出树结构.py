import subprocess
import os
import asyncio
import time
from tqdm import tqdm
import re

class 目录树导出器:
    def __init__(自身, 路径):
        自身.路径 = 自身.清理路径(路径)
        自身.缓存 = {}
        自身.缓存过期时间 = 60  # 缓存过期时间（秒）
        自身.文件数 = 0
        自身.文件夹数 = 0
        自身.总字符数 = 0

    def 清理路径(自身, 路径):
        """清理路径，防止注入攻击"""
        if not re.match(r'^[a-zA-Z]:\\[\\\S|*\S]?.*$', 路径):
            raise ValueError(f"无效的路径: {路径}")
        return 路径

    def 检查路径是否存在(自身):
        """检查路径是否存在"""
        if not os.path.exists(自身.路径):
            raise FileNotFoundError(f"路径不存在: {自身.路径}")

    def 检查缓存是否过期(自身, 缓存时间):
        """检查缓存是否过期"""
        return time.time() - 缓存时间 > 自身.缓存过期时间

    async def 生成目录树(自身):
        """异步使用 tree 命令生成目录树"""
        if 自身.路径 in 自身.缓存 and not 自身.检查缓存是否过期(自身.缓存[自身.路径]["时间"]):
            print(f"从缓存中加载目录树: {自身.路径}")
            return 自身.缓存[自身.路径]["数据"]

        try:
            过程 = await asyncio.create_subprocess_shell(
                f'tree /F "{自身.路径}"',
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                shell=True
            )
            标准输出, 标准错误 = await 过程.communicate()
            
            if 过程.returncode != 0:
                raise RuntimeError(f"tree 命令执行失败: {标准错误.decode('cp936').strip()}")
            
            # 尝试用常见编码解码输出
            解码成功 = False
            目录树 = None
            for 编码 in ['utf-8', 'cp936', 'gbk']:
                try:
                    目录树 = 标准输出.decode(编码)
                    解码成功 = True
                    break
                except UnicodeDecodeError:
                    continue
            
            if not 解码成功:
                raise RuntimeError("无法解码 tree 命令的输出")
            
            # 统计文件数、文件夹数和总字符数
            自身.文件数 = 目录树.count("\n") - 目录树.count("\n    ")  # 粗略估计文件数
            自身.文件夹数 = 目录树.count("\n    ")  # 粗略估计文件夹数
            自身.总字符数 = len(目录树)
            
            自身.缓存[自身.路径] = {"数据": 目录树, "时间": time.time()}  # 缓存结果和时间
            return 目录树
        except Exception as 错误:
            raise RuntimeError(f"执行 tree 命令时出错: {错误}")

    async def 保存目录树到文件(自身, 输出文件):
        """异步将目录树保存到文件"""
        try:
            自身.检查路径是否存在()
            目录树 = await 自身.生成目录树()
            
            # 异步逐行写入文件
            with open(输出文件, 'w', encoding='utf-8') as 文件:
                for 行 in tqdm(目录树.splitlines(), desc="写入文件", unit="行"):
                    文件.write(行 + '\n')
            
            print(f"目录树已成功导出到 {输出文件}")
            print(f"文件数: {自身.文件数}")
            print(f"文件夹数: {自身.文件夹数}")
            print(f"总字符数: {自身.总字符数}")
        except FileNotFoundError as 错误:
            print(f"路径错误: {错误}")
        except RuntimeError as 错误:
            print(f"命令执行错误: {错误}")
        except IOError as 错误:
            print(f"文件写入错误: {错误}")
        except Exception as 错误:
            print(f"未知错误: {错误}")

async def 主函数():
    # 指定要生成目录树的路径
    路径 = os.getcwd()  # 默认当前目录
    # 路径 = "Z:\\llvm-19.1.0.src"  # 可以指定其他目录
    
    # 指定输出文件路径
    输出文件 = '树结构.txt'
    
    # 创建目录树导出器实例并保存目录树
    导出器 = 目录树导出器(路径)
    await 导出器.保存目录树到文件(输出文件)

if __name__ == "__main__":
    asyncio.run(主函数())