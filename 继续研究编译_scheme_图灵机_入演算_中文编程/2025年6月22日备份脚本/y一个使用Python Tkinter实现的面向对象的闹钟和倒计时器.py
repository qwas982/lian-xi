import tkinter as tk
from tkinter import ttk, messagebox, filedialog
import datetime
import winsound
import threading
from pygame import mixer  # 需安装pygame库：pip install pygame

# 自定义点：默认音频路径（用户可修改此处默认值）
默认声音路径 = "alarm.wav"  
# 自定义点：界面按钮基础尺寸
按钮垫板 = {'padx': 8, 'pady': 4} 

class 闹钟:
    def __init__(自身, 时, 分, 秒):
        当前时间 = datetime.datetime.now()
        目标时间 = datetime.time(时, 分, 秒)
        自身.目标日期时间 = datetime.datetime.combine(当前时间.date(), 目标时间)
        
        if 自身.目标日期时间 < 当前时间:
            自身.目标日期时间 += datetime.timedelta(days=1)
        
        自身.激活状态 = True

    def 获取剩余时间(自身):
        if not 自身.激活状态:
            return "已触发"
        时间差 = 自身.目标日期时间 - datetime.datetime.now()
        if 时间差.total_seconds() <= 0:
            自身.激活状态 = False
            return "时间到！"
        return str(时间差).split(".")[0]

class 倒计时器:
    def __init__(自身, 小时, 分钟, 秒数):
        自身.原始总秒数 = 小时*3600 + 分钟*60 + 秒数
        自身.剩余秒数 = 自身.原始总秒数
        自身.运行状态 = False

    def 开始暂停(自身):
        自身.运行状态 = not 自身.运行状态

    def 重置(自身):
        自身.剩余秒数 = 自身.原始总秒数
        自身.运行状态 = False

    def 滴答(自身):
        if 自身.运行状态 and 自身.剩余秒数 > 0:
            自身.剩余秒数 -= 1
            if 自身.剩余秒数 == 0:
                自身.运行状态 = False
                return True
        return False

    def 获取剩余时间(自身):
        时 = 自身.剩余秒数 // 3600
        分 = (自身.剩余秒数 % 3600) // 60
        秒 = 自身.剩余秒数 % 60
        return f"{时:02d}:{分:02d}:{秒:02d}"
        
# 在应用类前新增音频配置类
class 音频配置:
    def __init__(自身):
        自身.起始时间 = 0.0
        自身.播放时长 = 5.0
        自身.最大时长 = 0.0

    def 从界面加载配置(自身, 起始控件, 时长控件):
        """从界面控件获取并验证参数"""
        try:
            自身.起始时间 = max(0.0, float(起始控件.get()))
            自身.播放时长 = max(0.1, float(时长控件.get()))
        except ValueError:
            自身.起始时间 = 0.0
            自身.播放时长 = 5.0
            raise ValueError("音频参数无效")

    def 更新音频信息(自身, 文件路径):
        """获取音频元数据更新限制"""
        try:
            from mutagen.mp3 import MP3
            audio = MP3(文件路径)
            自身.最大时长 = audio.info.length
        except:
            自身.最大时长 = 0.0

class 应用(tk.Tk):
    def __init__(自身):
        super().__init__()
        自身.title("多功能计时器-增强版")
        自身.geometry("780x480")  # 增大窗口尺寸 "800x600"
        自身.闹钟列表 = []
        自身.倒计时列表 = []
        
        # 初始化音频系统
        mixer.init()
        自身.当前音量 = 0.5  # 默认音量（0.0~1.0）
        自身.当前音频路径 = 默认声音路径
        
        自身.创建控件()
        自身.更新()
        自身.style配置()  # 添加样式配置
        
        自身.音频配置 = 音频配置()  # 新增配置实例
        
        自身.播放任务列表 = []  # 存储所有预定的停止任务ID

    def style配置(自身):
        """自定义点：界面样式配置"""
        style = ttk.Style()
        style.configure("TButton", padding=6, font=('宋体', 12))  # 增大按钮尺寸
        style.configure("Large.TButton", padding=8, font=('楷体', 16, 'bold'))  # 大号按钮样式
        
        style.configure("Emergency.TButton", 
                      padding=8, 
                      font=('宋体', 14, 'bold'),
                      foreground='red',
                      background='#22F80B')  # 色警示样式
        
    def 创建音频设置控件(自身, 父框架):
        """独立创建音频参数控件"""
        参数框架 = ttk.Frame(父框架)
        参数框架.pack(side=tk.LEFT, padx=10)

        # 起始时间控件
        ttk.Label(参数框架, text="开始时间(秒):").pack(side=tk.LEFT)
        自身.起始时间输入 = ttk.Spinbox(参数框架, from_=0, to=0, width=6)
        自身.起始时间输入.pack(side=tk.LEFT)
        自身.起始时间输入.set(0)

        # 播放时长控件
        ttk.Label(参数框架, text="播放时长(秒):").pack(side=tk.LEFT, padx=(10,0))
        自身.播放时长输入 = ttk.Spinbox(参数框架, from_=1, to=0, width=6)
        自身.播放时长输入.pack(side=tk.LEFT)
        自身.播放时长输入.set(5)

        # 添加停止按钮
        停止按钮 = ttk.Button(参数框架, text="立即停止", 
                            style="Emergency.TButton",
                            command=自身.立即停止播放)
        停止按钮.pack(side=tk.LEFT, padx=10)
        return 参数框架

    def 创建控件(自身):
        主框架 = ttk.Frame(自身)
        主框架.pack(padx=10, pady=10, fill=tk.BOTH, expand=True)
        
        # 新增音频控制区域
        音频框架 = ttk.LabelFrame(主框架, text="音频设置")
        音频框架.pack(fill=tk.X, pady=5, anchor=tk.N)  # anchor=tk.N 确保顶部对齐

        ttk.Button(音频框架, text="选择音频", style="Large.TButton", 
                 command=自身.选择音频文件).pack(side=tk.LEFT, **按钮垫板)
        
        ttk.Label(音频框架, text="音量:").pack(side=tk.LEFT, padx=5)
        自身.音量滑动条 = ttk.Scale(音频框架, from_=0, to=1, 
                                  command=lambda v: 自身.设置音量(float(v)))
        自身.音量滑动条.set(自身.当前音量)
        自身.音量滑动条.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        
        参数控件框架 = 自身.创建音频设置控件(音频框架)
        参数控件框架.pack(side=tk.RIGHT)

        # 闹钟设置区域
        闹钟框架 = ttk.LabelFrame(主框架, text="设置闹钟")
        闹钟框架.pack(fill=tk.X, pady=5)

        ttk.Label(闹钟框架, text="时:").pack(side=tk.LEFT)
        自身.闹钟_小时 = ttk.Spinbox(闹钟框架, from_=0, to=23, width=6)
        自身.闹钟_小时.pack(side=tk.LEFT)
        
        ttk.Label(闹钟框架, text="分:").pack(side=tk.LEFT)
        自身.闹钟_分钟 = ttk.Spinbox(闹钟框架, from_=0, to=59, width=6)
        自身.闹钟_分钟.pack(side=tk.LEFT)
        
        ttk.Label(闹钟框架, text="秒:").pack(side=tk.LEFT)
        自身.闹钟_秒数 = ttk.Spinbox(闹钟框架, from_=0, to=59, width=6)
        自身.闹钟_秒数.pack(side=tk.LEFT)
        

        # 倒计时设置区域
        倒计时框架 = ttk.LabelFrame(主框架, text="设置倒计时")
        倒计时框架.pack(fill=tk.X, pady=5)

        ttk.Label(倒计时框架, text="时:").pack(side=tk.LEFT)
        自身.倒计时_小时 = ttk.Spinbox(倒计时框架, from_=0, to=99, width=6)
        自身.倒计时_小时.pack(side=tk.LEFT)
        
        ttk.Label(倒计时框架, text="分:").pack(side=tk.LEFT)
        自身.倒计时_分钟 = ttk.Spinbox(倒计时框架, from_=0, to=59, width=6)
        自身.倒计时_分钟.pack(side=tk.LEFT)
        
        ttk.Label(倒计时框架, text="秒:").pack(side=tk.LEFT)
        自身.倒计时_秒数 = ttk.Spinbox(倒计时框架, from_=0, to=59, width=6)
        自身.倒计时_秒数.pack(side=tk.LEFT)
        


        # 活动列表
        自身.列表框架 = ttk.Frame(主框架)
        自身.列表框架.pack(fill=tk.BOTH, expand=True, pady=10)
        
        自身.画布 = tk.Canvas(自身.列表框架)
        自身.滚动条 = ttk.Scrollbar(自身.列表框架, orient="vertical", command=自身.画布.yview)
        自身.可滚动框架 = ttk.Frame(自身.画布)
        
        自身.可滚动框架.bind(
            "<Configure>",
            lambda e: 自身.画布.configure(scrollregion=自身.画布.bbox("all"))
        )
        
        自身.画布.create_window((0, 0), window=自身.可滚动框架, anchor="nw")
        自身.画布.configure(yscrollcommand=自身.滚动条.set)
        
        自身.画布.pack(side="left", fill="both", expand=True)
        自身.滚动条.pack(side="right", fill="y")
        

        
        # 修改原有按钮样式
        ttk.Button(闹钟框架, text="获取闹钟", style="Large.TButton", 
                 command=自身.添加闹钟).pack(side=tk.RIGHT, **按钮垫板)
        
        ttk.Button(倒计时框架, text="获取倒计时", style="Large.TButton", 
                 command=自身.添加倒计时器).pack(side=tk.RIGHT, **按钮垫板)

    def 添加闹钟(自身):
        try:
            时 = int(自身.闹钟_小时.get())
            分 = int(自身.闹钟_分钟.get())
            秒 = int(自身.闹钟_秒数.get())
            新闹钟 = 闹钟(时, 分, 秒)
            自身.创建闹钟条目(新闹钟)
            自身.闹钟列表.append(新闹钟)
        except ValueError:
            messagebox.showerror("错误", "请输入有效的数字")

    def 添加倒计时器(自身):
        try:
            时 = int(自身.倒计时_小时.get())
            分 = int(自身.倒计时_分钟.get())
            秒 = int(自身.倒计时_秒数.get())
            if 时 + 分 + 秒 == 0:
                raise ValueError
            新计时器 = 倒计时器(时, 分, 秒)
            自身.创建倒计时条目(新计时器)
            自身.倒计时列表.append(新计时器)
        except ValueError:
            messagebox.showerror("错误", "请输入有效的数字")

    def 创建闹钟条目(自身, 闹钟实例):
        条目框架 = ttk.Frame(自身.可滚动框架)
        条目框架.pack(fill=tk.X, pady=2)
        
        类型标签 = ttk.Label(条目框架, text="闹钟", width=8)
        类型标签.pack(side=tk.LEFT)
        
        时间字符串 = 闹钟实例.目标日期时间.strftime("%Y-%m-%d %H:%M:%S")
        时间标签 = ttk.Label(条目框架, text=时间字符串)
        时间标签.pack(side=tk.LEFT, padx=10)
        
        状态标签 = ttk.Label(条目框架, text=闹钟实例.获取剩余时间())
        状态标签.pack(side=tk.LEFT, expand=True)
        
        ttk.Button(条目框架, text="删除", 
                 command=lambda f=条目框架, a=闹钟实例: 自身.移除闹钟(f, a)).pack(side=tk.RIGHT)
        
        闹钟实例.条目框架 = 条目框架
        闹钟实例.状态标签 = 状态标签

    def 创建倒计时条目(自身, 计时器实例):
        条目框架 = ttk.Frame(自身.可滚动框架)
        条目框架.pack(fill=tk.X, pady=2)
        
        类型标签 = ttk.Label(条目框架, text="倒计时", width=8)
        类型标签.pack(side=tk.LEFT)
        
        状态标签 = ttk.Label(条目框架, text=计时器实例.获取剩余时间())
        状态标签.pack(side=tk.LEFT, padx=10, expand=True)
        
        按钮框架 = ttk.Frame(条目框架)
        按钮框架.pack(side=tk.RIGHT)
        
        开始按钮 = ttk.Button(按钮框架, text="开始", 
                          command=lambda t=计时器实例: 自身.切换倒计时状态(t))
        开始按钮.pack(side=tk.LEFT)
        
        ttk.Button(按钮框架, text="重置", 
                 command=lambda t=计时器实例: 自身.重置倒计时(t)).pack(side=tk.LEFT)
        ttk.Button(按钮框架, text="删除", 
                 command=lambda f=条目框架, t=计时器实例: 自身.移除倒计时器(f, t)).pack(side=tk.LEFT)
        
        计时器实例.条目框架 = 条目框架
        计时器实例.状态标签 = 状态标签
        计时器实例.开始按钮 = 开始按钮

    def 更新(自身):
        自身.更新闹钟状态()
        自身.更新倒计时器状态()
        自身.after(1000, 自身.更新)

    def 更新闹钟状态(自身):
        for 闹钟 in 自身.闹钟列表.copy():
            剩余时间 = 闹钟.获取剩余时间()
            闹钟.状态标签.config(text=剩余时间)
            if not 闹钟.激活状态:
                threading.Thread(target=自身.播放提示音, args=("闹钟时间到！",)).start()
                自身.移除闹钟(闹钟.条目框架, 闹钟)

    def 更新倒计时器状态(自身):
        for 计时器 in 自身.倒计时列表.copy():
            if 计时器.运行状态:
                已完成 = 计时器.滴答()
                计时器.状态标签.config(text=计时器.获取剩余时间())
                if 已完成:
                    threading.Thread(target=自身.播放提示音, args=("倒计时结束！",)).start()
                    自身.移除倒计时器(计时器.条目框架, 计时器)

    def 切换倒计时状态(自身, 计时器实例):
        计时器实例.开始暂停()
        计时器实例.开始按钮.config(text="暂停" if 计时器实例.运行状态 else "开始")

    def 重置倒计时(自身, 计时器实例):
        计时器实例.重置()
        计时器实例.状态标签.config(text=计时器实例.获取剩余时间())
        计时器实例.开始按钮.config(text="开始")

    def 移除闹钟(自身, 框架, 闹钟实例):
        if 闹钟实例 in 自身.闹钟列表:
            自身.闹钟列表.remove(闹钟实例)
        框架.destroy()

    def 移除倒计时器(自身, 框架, 计时器实例):
        if 计时器实例 in 自身.倒计时列表:
            自身.倒计时列表.remove(计时器实例)
        框架.destroy()
        
    def 立即停止播放(自身):
        """立即终止所有音频播放（增强版）"""
        if messagebox.askyesno("确认", "确定要立即停止所有播放吗？"):
        # 停止所有播放引擎
            try:
                mixer.music.fadeout(100)  # 100ms淡出
            except:
                pass
            try:
                winsound.PlaySound(None, winsound.SND_PURGE)
            except:
                pass
    
            # 取消所有预定任务
            for 任务身份 in 自身.播放任务列表:
                try:
                    自身.after_cancel(任务身份)
                except:
                    pass
            自身.播放任务列表.clear()
    
            # 强制释放音频资源
            mixer.quit()
            mixer.init()  # 重新初始化确保后续播放正常

    def 播放提示音(自身, 消息):
        """自定义点：音频播放实现（支持自定义文件）"""
        try:
            自身.音频配置.从界面加载配置(自身.起始时间输入, 自身.播放时长输入)
            if mixer.get_init():  # 确保mixer已初始化
                try:
                    mixer.music.load(自身.当前音频路径)
                    mixer.music.set_volume(自身.当前音量)
                    mixer.music.play(0, 自身.音频配置.起始时间, 0) # 从第 n 秒开始播放                    
                    任务身份 = 自身.after(
                        int(自身.音频配置.播放时长 * 1000), 
                        mixer.music.stop
                    ) # 使用after方法处理时长限制（示例：播放5秒）
                    自身.播放任务列表.append(任务身份)
                    return
                except pygame.error as e:
                    print(f"pygame播放失败: {str(e)}")  
            # 备用方案1：使用winsound播放WAV
            if 自身.当前音频路径.lower().endswith('.wav'):
                try:
                    winsound.PlaySound(自身.当前音频路径, winsound.SND_FILENAME)
                    return
                except Exception as e:
                    print(f"winsound播放失败: {str(e)}")
        
            # 备用方案2：系统蜂鸣器
            winsound.Beep(1000, 1000)
        
        except Exception as e:
            print(f"所有音频方案失败: {str(e)}")
            winsound.MessageBeep()
        except ValueError as e:
            messagebox.showerror("配置错误", str(e))
    
        finally:
            messagebox.showinfo("提示", 消息)


    def 设置音量(自身, 音量值):
        """设置全局音量（0.0~1.0）"""
        自身.当前音量 = max(0.0, min(1.0, 音量值))
        if mixer.get_init():
            mixer.music.set_volume(自身.当前音量)

    def 选择音频文件(自身):
        """打开文件选择对话框"""
        文件路径 = filedialog.askopenfilename(
            filetypes=[("音频文件", "*.wav *.mp3 *.m4a"), ("All Files", "*.*")])
        if 文件路径:
            if not 文件路径.lower().endswith(('.wav', '.mp3')):
                messagebox.showerror("错误", "仅支持 WAV/MP3 格式")
                return
            自身.当前音频路径 = 文件路径
            自身.音频配置.更新音频信息(文件路径)
            # 更新界面控件限制
            自身.起始时间输入.configure(to=自身.音频配置.最大时长)
            自身.播放时长输入.configure(to=自身.音频配置.最大时长)

    # 性能优化：使用更高效的列表遍历方式
    def 更新闹钟状态(自身):
        current_time = datetime.datetime.now()  # 减少重复调用
        for 闹钟 in 自身.闹钟列表.copy():
            if not 闹钟.激活状态:
                continue
            时间差 = 闹钟.目标日期时间 - current_time
            if 时间差.total_seconds() <= 0:
                闹钟.激活状态 = False
                threading.Thread(target=自身.播放提示音, args=("闹钟时间到！",)).start()
                自身.移除闹钟(闹钟.条目框架, 闹钟)
            else:
                闹钟.状态标签.config(text=str(时间差).split(".")[0])

if __name__ == "__main__":
    程序 = 应用()
    程序.mainloop()
    
"""
主要改进点说明：

性能优化：

在更新闹钟状态时减少datetime.now()的重复调用

使用copy()避免遍历时修改列表导致的问题

优化条件判断逻辑

界面改进：

使用ttk.Style统一设置按钮样式

增大的按钮尺寸（通过padding和字体设置）

新增音量滑动条控件

窗口尺寸调整为800x500

音频自定义：

使用pygame库支持MP3/WAV播放

添加文件选择对话框

支持音量控制（0-1范围）

添加5秒自动停止示例（可通过after参数调整）

代码注释：

使用中文注释标记关键自定义点

重要方法添加docstring说明

关键参数添加注释说明

扩展性改进：

在style配置方法中集中管理样式

使用常量定义默认值（DEFAULT_SOUND_PATH）

音频播放模块与业务逻辑解耦

使用说明：

需要安装pygame库：pip install pygame

可修改DEFAULT_SOUND_PATH设置默认音频路径

通过界面按钮可选择自定义音频文件

音量滑动条实时调整播放音量

按钮样式通过style配置方法统一管理

注意：音频时长控制目前设置为5秒（5000ms），可根据需求修改after()参数值。
"""
