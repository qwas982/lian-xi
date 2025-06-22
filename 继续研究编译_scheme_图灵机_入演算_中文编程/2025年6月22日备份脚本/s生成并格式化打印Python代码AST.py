import ast
from pprint import pformat
from enum import Enum, auto
import sys
from typing import Optional, Dict, Any

class AST操作类型(Enum):
    """AST操作类型枚举"""
    转储 = auto()
    分析 = auto()
    可视化 = auto()

class AST生成器:
    """高内聚的AST生成器类，负责核心AST生成逻辑"""
    
    def __init__(自身):
        自身._缓存: Dict[str, ast.AST] = {}
    
    def 从文件生成(自身, 文件路径: str) -> ast.AST:
        """从文件生成AST，带有缓存功能"""
        if 文件路径 in 自身._缓存:
            return 自身._缓存[文件路径]
        
        with open(文件路径, 'r', encoding='utf-8') as 文件:
            代码 = 文件.read()
        
        语法树 = ast.parse(代码)
        自身._缓存[文件路径] = 语法树
        return 语法树
    
    def 从代码生成(自身, 代码: str) -> ast.AST:
        """从代码字符串生成AST"""
        return ast.parse(代码)
    
    @staticmethod
    def 格式化输出(语法树: ast.AST, 包含属性: bool = False) -> str:
        """格式化AST输出"""
        return pformat(ast.dump(语法树, indent=4, include_attributes=包含属性))

class AST分析器:
    """AST分析器类，负责各种AST分析操作"""
    
    def __init__(自身, 生成器: AST生成器):
        自身.生成器 = 生成器
    
    def 执行操作(自身, 语法树: ast.AST, 操作: AST操作类型) -> Any:
        """执行AST操作"""
        if 操作 == AST操作类型.转储:
            return 自身.生成器.格式化输出(语法树)
        elif 操作 == AST操作类型.分析:
            return 自身._分析语法树(语法树)
        elif 操作 == AST操作类型.可视化:
            return 自身._可视化语法树(语法树)
        else:
            raise ValueError("未知的AST操作类型")
    
    def _分析语法树(自身, 语法树: ast.AST) -> Dict[str, int]:
        """分析AST并返回统计数据"""
        统计 = {
            '函数': 0,
            '类': 0,
            '导入': 0,
            '调用': 0
        }
        
        for 节点 in ast.walk(语法树):
            if isinstance(节点, ast.FunctionDef):
                统计['函数'] += 1
            elif isinstance(节点, ast.ClassDef):
                统计['类'] += 1
            elif isinstance(节点, (ast.Import, ast.ImportFrom)):
                统计['导入'] += 1
            elif isinstance(节点, ast.Call):
                统计['调用'] += 1
        
        return 统计
    
    def _可视化语法树(自身, 语法树: ast.AST) -> str:
        """生成AST的可视化表示（简化版）"""
        return f"AST可视化（节点总数: {len(list(ast.walk(语法树)))}）"

class AST交互界面:
    """REPL交互界面类，负责用户交互"""
    
    def __init__(自身, 生成器: AST生成器, 分析器: AST分析器):
        自身.生成器 = 生成器
        自身.分析器 = 分析器
        自身.当前语法树: Optional[ast.AST] = None
    
    def 启动(自身):
        """启动REPL"""
        print("Python AST 分析工具 交互界面")
        print("命令: 加载 <文件路径>, 转储, 分析, 可视化, 帮助, 退出")
        
        while True:
            try:
                用户输入 = input(">>> ").strip()
                if not 用户输入:
                    continue
                
                if 用户输入.lower() in ('退出', 'quit'):
                    break
                elif 用户输入.lower() == '帮助':
                    自身._显示帮助()
                elif 用户输入.startswith('加载 '):
                    自身._处理加载(用户输入[3:])
                elif 用户输入 == '转储':
                    自身._处理转储()
                elif 用户输入 == '分析':
                    自身._处理分析()
                elif 用户输入 == '可视化':
                    自身._处理可视化()
                else:
                    print("未知命令，输入'帮助'查看帮助")
            
            except KeyboardInterrupt:
                print("\n使用'退出'命令退出")
            except Exception as 异常:
                print(f"错误: {str(异常)}")
    
    def _显示帮助(自身):
        """显示帮助信息"""
        帮助文本 = """
可用命令:
  加载 <文件路径> - 加载Python文件
  转储 - 显示AST结构
  分析 - 分析AST统计信息
  可视化 - 生成AST可视化（简化版）
  帮助 - 显示此帮助
  退出 - 退出交互界面
"""
        print(帮助文本)
    
    def _处理加载(自身, 文件路径: str):
        """处理加载文件命令"""
        try:
            自身.当前语法树 = 自身.生成器.从文件生成(文件路径)
            print(f"已加载文件: {文件路径}")
        except Exception as 异常:
            print(f"加载文件失败: {str(异常)}")
    
    def _处理转储(自身):
        """处理转储命令"""
        if 自身.当前语法树 is None:
            print("请先使用'加载'命令加载文件")
            return
        
        print("AST结构:")
        print(自身.分析器.执行操作(自身.当前语法树, AST操作类型.转储))
    
    def _处理分析(自身):
        """处理分析命令"""
        if 自身.当前语法树 is None:
            print("请先使用'加载'命令加载文件")
            return
        
        统计结果 = 自身.分析器.执行操作(自身.当前语法树, AST操作类型.分析)
        print("AST分析结果:")
        for 键, 值 in 统计结果.items():
            print(f"{键}: {值}")
    
    def _处理可视化(自身):
        """处理可视化命令"""
        if 自身.当前语法树 is None:
            print("请先使用'加载'命令加载文件")
            return
        
        结果 = 自身.分析器.执行操作(自身.当前语法树, AST操作类型.可视化)
        print(结果)

class AST处理器:
    """高层AST处理器，协调各模块工作"""
    
    def __init__(自身):
        自身.生成器 = AST生成器()
        自身.分析器 = AST分析器(自身.生成器)
        自身.交互界面 = AST交互界面(自身.生成器, 自身.分析器)
    
    def 处理文件(自身, 文件路径: str):
        """直接处理文件并显示结果"""
        try:
            语法树 = 自身.生成器.从文件生成(文件路径)
            print("AST结构:")
            print(自身.分析器.执行操作(语法树, AST操作类型.转储))
            print("\nAST分析:")
            统计结果 = 自身.分析器.执行操作(语法树, AST操作类型.分析)
            for 键, 值 in 统计结果.items():
                print(f"{键}: {值}")
        except Exception as 异常:
            print(f"处理文件失败: {str(异常)}")
    
    def 启动交互界面(自身):
        """启动REPL交互模式"""
        自身.交互界面.启动()

def 主函数():
    """主函数"""
    处理器 = AST处理器()
    
    if len(sys.argv) > 1:
        # 命令行参数模式
        处理器.处理文件(sys.argv[1])
    else:
        # 交互界面模式
        处理器.启动交互界面()

if __name__ == "__main__":
    主函数()