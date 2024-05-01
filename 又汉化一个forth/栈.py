'''虚拟机要用到的栈'''

from 错误们 import 机器错误

class 栈(object):
    '''一个值们的栈'''
    def __init__(自身, 值们=None):
        自身._值们 = 值们
        if 自身._值们 is None:
            自身._值们 = []

    def 弹(自身):
        if len(自身._值们) == 0:
            raise 机器错误("栈下溢")
        return 自身._值们.pop()
    
    def 推(自身, 值):
        自身._值们.append(值)

    @property
    def 顶部(自身):
        return None if len(自身._值们) == 0 else 自身._值们[-1]
    
    def __str__(自身):
        return str(自身._值们)
    
    def __repr__(自身):
        return "<栈: 值 = %s >" % 自身._值们
    
    def __len__(自身):
        return len(自身._值们)
    
    def __getitem__(自身, 键):
        return 自身._值们[键]
    
    def __eq__(自身, 对象):
        return 自身._值们 == 对象._值们
    
    def __ne__(自身, 对象):
        return 自身._值们 != 对象._值们
    
