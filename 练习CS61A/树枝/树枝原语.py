 
#scheme原语.py 
import math
import sys
from 树枝读 import 点对, 无
import operator
from multiprocessing.dummy import Pool as ThreadPool

try:
    import turtle
    import tkinter
except:
    print("警告: 不能导入turtle模块.", 文件=sys.stderr)

class 树枝错误(BaseException):
    """异常表明一个错误在一个树枝程序内"""

#原语操作#
原语们全局 = []

def 原语(*名称):
    def 加入(函):
        for 名 in 名称:
            原语们全局.append((名, 函, 名称[0]))
        return 函
    return 加入

def 检查类型(值, 谓词, 键, 名称):
    if not 谓词(值):
        消息 = "实参 {0} 的 {1} 有错误类型 ({2})"
        raise 树枝错误(消息.format(键, 名称, type(值).__name__))
    return 值

@原语("布尔?")
def 树枝布尔谓(甲):
    return 甲 is True or 甲 is False

def 树枝真谓(值):
    return 值 is not False

def 树枝假谓(值):
    return 值 is False

@原语("非")
def 树枝非(甲):
    return not 树枝真谓(甲)

@原语("相等?")
def 树枝相等谓(甲, 乙):
    if 树枝点对谓(甲) and 树枝点对谓(乙):
        return 树枝相等谓(甲.第一, 乙.第一) and 树枝相等谓(甲.第二, 乙.第二)
    elif 树枝数目谓(甲) and 树枝数目谓(乙):
        return 甲 == 乙
    else:
        return type(甲) == type(乙) and 甲 == 乙

@原语("等号?")
def 树枝等号谓(甲, 乙):
    if 树枝数目谓(甲) and 树枝数目谓(乙):
        return 甲 == 乙
    elif 树枝符号谓(甲) and 树枝符号谓(乙):
        return 甲 == 乙
    else:
        return 甲 is 乙
    
@原语("点对?")
def 树枝点对谓(甲):
    return isinstance(甲, 点对)

@原语("承诺?")
def 树枝承诺谓(甲):
    return type(甲).__name__ == '承诺大'

@原语("强力")
def 树枝强力(甲):
    检查类型(甲, 树枝承诺谓, 0, '承诺小')
    return 甲.求值的()

@原语("切尾流")
def 树枝切尾流(甲):
    检查类型(甲, lambda 甲: 树枝点对谓(甲) and 树枝承诺谓(甲.第二), 0, '切尾流')
    return 树枝强力(甲.第二)

@原语("空指针?")
def 树枝空指针谓(甲):
    return 甲 is 无

@原语("列表?")
def 树枝列表谓(甲):
    while 甲 is not 无:
        if not isinstance(甲, 点对):
            return False
        甲 = 甲.第二
    return True

@原语("长度")
def 树枝长度(甲):
    检查类型(甲, 树枝列表谓, 0, '长度')
    if 甲 is 无:
        return 0
    return len(甲)

@原语("构造")
def 树枝构造(甲, 乙):
    return 点对(甲, 乙)

@原语("切头")
def 树枝切头(甲):
    检查类型(甲, 树枝点对谓, 0, '切头')
    return 甲.第一

@原语("切尾")
def 树枝切尾(甲):
    检查类型(甲, 树枝点对谓, 0, '切尾')
    return 甲.第二

@原语("列表")
def 树枝列表(*值):
    结果 = 无
    for 元素 in reversed(值):
        结果 = 点对(元素, 结果)
    return 结果

@原语("追加")
def 树枝追加(*值):
    if len(值) == 0:
        return 无
    结果 = 值[-1]
    for 计数 in range(len(值) - 2, -1, -1):
        值的 = 值[计数]
        if 值的 is not 无:
            检查类型(值的, 树枝点对谓, 计数, '追加')
            结了 = 点了 = 点对(值的.第一, 结果)
            值的 = 值的.第二
            while 树枝点对谓(值的):
                点了.第二 = 点对(值的.第一, 结果)
                点了 = 点了.第二
                值的 = 值的.第二
            结果 = 结了
    return 结果

@原语("串?")
def 树枝串谓(甲):
    return isinstance(甲, str) and 甲.startswith('"')

@原语("符号?")
def 树枝符号谓(甲):
    return isinstance(甲, str) and not 树枝串谓(甲)

@原语("数目?")
def 树枝数目谓(甲):
    return isinstance(甲, (int, float)) and not 树枝布尔谓(甲)

@原语("整数?")
def 树枝整数谓(甲):
    return 树枝数目谓(甲) and (isinstance(甲, int) or round(甲) == 甲)

def 检查数们(*值):
    for 计数, 值的 in enumerate(值):
        if not 树枝数目谓(值的):
            消息 = "操作范围 {0} ({1}) 不是一个数目"
            raise 树枝错误(消息.format(计数, 值的))
        
def 算术之(函, 初始之, 值):
    检查数们(*值)
    符 = 初始之
    for 值吗 in 值:
        符 = 函(符, 值吗)
    if round(符) == 符:
        符 = round(符)
    return 符

@原语("+")
def 树枝加法(*值):
    return 算术之(operator.add, 0, 值)

@原语("-")
def 树枝减法(值0, *值):
    检查数们(值0, *值)
    if len(值) == 0:
        return -值0
    return 算术之(operator.sub, 值0, 值)

@原语("*")
def 树枝乘法(*值):
    return 算术之(operator.mul, 1, 值)

@原语("/")
def 树枝除法(值0, *值):
    检查数们(值0, *值)
    try:
        if len(值) == 0:
            return 1 / 值0
        return 算术之(operator.truediv, 值0, 值)
    except ZeroDivisionError as 错误:
        raise 树枝错误("除零错误", 错误)
    
@原语("指数")
def 树枝指数(值0, 值1):
    检查数们(值0, 值1)
    return pow(值0, 值1)

@原语("绝对值")
def 树枝绝对值(值):
    return abs(值)

@原语("商")
def 树枝商(值0, 值1):
    检查数们(值0, 值1)
    try:
        return int(值0 / 值1)
    except ZeroDivisionError as 错误:
        raise 树枝错误("除零错误", 错误)
    
@原语("取模")
def 树枝取模(值0, 值1):
    检查数们(值0, 值1)
    try:
        return 值0 % 值1
    except ZeroDivisionError as 错误:
        raise 树枝错误("除零错误", 错误)
    
@原语("余数")
def 树枝余数(值0, 值1):
    检查数们(值0, 值1)
    try:
        结果 = 值0 % 值1
    except ZeroDivisionError as 错误:
        raise 树枝错误("除零错误", 错误)
    while 结果 < 0 and 值0 > 0 or 结果 > 0 and 值0 < 0:
        结果 = 结果 - 值1
    return 结果

@原语("四舍五入")
def 树枝四舍五入(值0, 值1):
    return round(值0, 值1)

@原语("地板")
def 树枝地板(值):
    return math.floor(值)

@原语("天花板")
def 树枝天花板(值):
    return math.ceil(值)

@原语("最大公约数")
def 树枝最大公约数(值0, 值1):
    检查数们(值0, 值1)
    while 值1 != 0:
        值0, 值1 = 值1, 值0 % 值1
    return 值0

@原语("最小公倍数")
def 树枝最小公倍数(值0, 值1):
    检查数们(值0, 值1)
    return (值0 * 值1) // 树枝最大公约数(值0, 值1)

@原语("交换")
def 树枝交换(值0, 值1):
    return 值1, 值0

@原语("反余弦")
def 树枝反余弦(值):
    return math.acos(值)

@原语("反正弦")
def 树枝反正弦(值):
    return math.asin(值)

@原语("反正切")
def 树枝反正切(值):
    return math.atan(值)

@原语("反双曲余弦")
def 树枝反双曲余弦(值):
    return math.acosh(值)

@原语("反双曲正弦")
def 树枝反双曲正弦(值):
    return math.asinh(值)

@原语("反双曲正切")
def 树枝反双曲正切(值):
    return math.atanh(值)

@原语("反正切2")
def 树枝反正切2(值0, 值1):
    return math.atan2(值0, 值1)

@原语("平方根")
def 树枝平方根(值):
    return math.sqrt(值)

@原语("余弦")
def 树枝余弦(值):
    return math.cos(值)

@原语("正弦")
def 树枝正弦(值):
    return math.sin(值)

@原语("正切")
def 树枝正切(值):
    return math.tan(值)

@原语("双曲余弦")
def 树枝双曲余弦(值):
    return math.cosh(值)

@原语("双曲正弦")
def 树枝双曲正弦(值):
    return math.sinh(值)

@原语("双曲正切")
def 树枝双曲正切(值):
    return math.tanh(值)

@原语("对数10")
def 树枝对数10(值):
    return math.log10(值)

@原语("对数2")
def 树枝对数2(值):
    return math.log2(值)

@原语("对数1加")
def 树枝对数1加(值):
    return math.log1p(值)

@原语("对数")
def 树枝对数(值):
    return math.log(值)

@原语("弧度到角度")
def 树枝弧度到角度(值):
    return math.degrees(值)

@原语("角度到弧度")
def 树枝角度到弧度(值):
    return math.radians(值)

@原语("复制正负号")
def 树枝复制正负号(值0, 值1):
    return math.copysign(值0, 值1)

@原语("截断")
def 树枝截断(值):
    return math.trunc(值)

def 数值对比(操作者, 甲, 乙):
    检查数们(甲, 乙)
    return 操作者(甲, 乙)

@原语("=")
def 树枝等于(甲, 乙):
    return 数值对比(operator.eq, 甲, 乙)

@原语("<")
def 树枝小于(甲, 乙):
    return 数值对比(operator.lt, 甲, 乙)

@原语(">")
def 树枝大于(甲, 乙):
    return 数值对比(operator.gt, 甲, 乙)

@原语("<=")
def 树枝小等(甲, 乙):
    return 数值对比(operator.le, 甲, 乙)

@原语(">=")
def 树枝大等(甲, 乙):
    return 数值对比(operator.ge, 甲, 乙)

@原语("!=")
def 树枝不等于(甲, 乙):
    return 数值对比(operator.ne, 甲, 乙)

@原语("偶数?")
def 树枝偶数谓(值):
    检查数们(值)
    return 值 % 2 == 0

@原语("奇数?")
def 树枝奇数谓(值):
    检查数们(值)
    return 值 % 2 == 1

@原语("非负?")
def 树枝非负谓(值):
    检查数们(值)
    return 值 >= 0

@原语("非正?")
def 树枝非正谓(值):
    检查数们(值)
    return 值 < 0

@原语("零?")
def 树枝零谓(值):
    检查数们(值)
    return 值 == 0

#其它操作

@原语("原子?")
def 树枝原子谓(值):
    return (树枝布尔谓(值) or 树枝数目谓(值) or 树枝符号谓(值) or 树枝空指针谓(值))

@原语("显示")
def 树枝显示(值):
    if 树枝串谓(值):
        值 = eval(值)
    print(str(值), end="")

@原语("读")
def 树枝读():
    return sys.stdin.readline().strip()

@原语("打印")
def 树枝打印(值):
    print(str(值))

@原语("新行")
def 树枝新行():
    print()
    sys.stdout.flush()

@原语("错误")
def 树枝错误(消息=None):
    消息 = "" if 消息 is None else str(消息)
    raise 树枝错误(消息)

@原语("出口")
def 树枝出口():
    raise EOFError


#海龟图形(非必选)

海龟屏幕开 = False

def 海龟屏幕打开():
    return 海龟屏幕开

def 海龟树枝预先():
    global 海龟屏幕开
    if not 海龟屏幕开:
        海龟屏幕开 = True
        turtle.tilt("树枝海龟")
        turtle.mode("logo")

@原语("向前")
def 海龟树枝向前(距离):
    检查数们(距离)
    海龟树枝预先()
    turtle.forward(距离)

@原语("向后")
def 海龟树枝向后(距离):
    检查数们(距离)
    海龟树枝预先()
    turtle.backward(距离)

@原语("向左")
def 海龟树枝向左(距离):
    检查数们(距离)
    海龟树枝预先()
    turtle.left(距离)

@原语("向右")
def 海龟树枝向右(距离):
    检查数们(距离)
    海龟树枝预先()
    turtle.right(距离)

@原语("圆的")
def 海龟树枝圆的(半径, 角度=None):
    if 角度 is None:
        检查数们(半径)
    else:
        检查数们(半径, 角度)
    海龟树枝预先()
    turtle.circle(半径, 角度 and 角度)

@原语("集位置", "集位点", "去到")
def 海龟树枝集位置(甲, 乙):
    检查数们(甲, 乙)
    海龟树枝预先()
    turtle.setposition(甲, 乙)

@原语("集头部")
def 海龟树枝集头部(高):
    检查数们(高)
    海龟树枝预先()
    turtle.setheading(高)

@原语("集笔上")
def 海龟树枝集笔上():
    海龟树枝预先()
    turtle.penup()

@原语("集笔下")
def 海龟树枝集笔下():
    海龟树枝预先()
    turtle.pendown()

@原语("秀海龟")
def 海龟树枝秀海龟():
    海龟树枝预先()
    turtle.showturtle()

@原语("隐藏海龟")
def 海龟树枝隐藏海龟():
    海龟树枝预先()
    turtle.hideturtle()

@原语("清除")
def 海龟树枝清除():
    海龟树枝预先()
    turtle.clear()

@原语("颜色")
def 海龟树枝颜色(颜色):
    海龟树枝预先()
    检查类型(颜色, 树枝串谓, 0, "颜色")
    turtle.color(eval(颜色))

@原语("红绿蓝")
def 海龟树枝红绿蓝(红, 绿, 蓝):
    颜色 = (红, 绿, 蓝)
    for 游标 in 颜色:
        if 游标 < 0 or 游标 > 1:
            raise 树枝错误("非法颜色强度在 " + str(颜色))
    切片 = tuple(int(游标*255) for 游标 in 颜色)
    return '"%02x%02x%02x"' % 切片

@原语("开始填充")
def 海龟树枝开始填充():
    海龟树枝预先()
    turtle.begin_fill()

@原语("终填充")
def 海龟树枝终填充():
    海龟树枝预先()
    turtle.end_fill()

@原语("上颜色")
def 海龟树枝上颜色(颜色):
    海龟树枝预先()
    检查类型(颜色, 树枝串谓, 0, "上颜色")
    turtle.bgcolor(eval(颜色))

@原语("单击退出")
def 海龟树枝单击退出():
    global 海龟屏幕开
    if 海龟屏幕开:
        print("关闭或单击海龟窗口完成退出")
        turtle.exitonclick()
        海龟屏幕开 = False

@原语("速度")
def 海龟树枝速度(速度):
    检查类型(速度, 树枝整数谓, 0, "速度")
    海龟树枝预先()
    turtle.speed(速度)

@原语("像素")
def 海龟树枝像素(横轴, 纵轴, 颜色):
    检查类型(颜色, 树枝串谓, 0, "像素")
    颜色乎 = eval(颜色)
    帆布 = turtle.getcanvas()
    宽, 高 = 帆布.winfo_width(), 帆布.winfo_height()
    if not hasattr(海龟树枝像素, "图像"):
        海龟树枝预先()
        海龟树枝像素.图像 = tkinter.PhotoImage(宽乎=宽, 高乎=高)
        帆布.create_image((0, 0), 图像 = 海龟树枝像素.图像, 状态 = "普通")
    大小 = 海龟树枝像素.大小
    for 显横轴 in range(大小):
        for 显纵轴 in range(大小):
            屏幕横轴, 屏幕纵轴 = 横轴 * 大小 + 显横轴, 高-(纵轴 * 大小 + 显纵轴)
            if 0 < 屏幕横轴 < 宽 and 0 < 屏幕纵轴 < 高:
                海龟树枝像素.图像.put(颜色乎, (屏幕横轴, 屏幕纵轴))

海龟树枝像素.大小 = 1
@原语("像素大小")
def 海龟树枝像素大小(大小):
    检查数们(大小)
    if 大小 <= 0 or not isinstance(大小, int):
        raise 树枝错误("无效像素大小: " + str(大小))
    海龟树枝像素.大小 = 大小

@原语("屏幕宽")
def 海龟树枝屏幕宽():
    return turtle.getcanvas().winfo_width() // 海龟树枝像素.大小

@原语("屏幕高")
def 海龟树枝屏幕高():
    return turtle.getcanvas().winfo_height() // 海龟树枝像素.大小