汉化对照表

//这次的教训是; 大小写在汉化为汉字标识符的时候一定记得分开标注出区别,
//第二个是; 所用的汉化标识符一定要与标准库函数名区分(以及标准库里的变量名),

sly = 失乐园
SLY_CHAR_ENC_UTF8 = 失乐园_印刻_编码_万国码编码8
SLY_CHAR_ENC_UTF16 = 失乐园_印刻_编码_万国码编码16
SLY_CHAR_ENC_LATIN1 = 失乐园_印刻_编码_拉丁文1

sly_cp1_t = 失乐园_代码点1_类型
sly_cp2_t = 失乐园_代码点2_类型
sly_cp4_t = 失乐园_代码点4_类型
sly_char_t = 失乐园_印刻_类型

sly_fixnum_t = 失乐园_固定整数_类型
sly_state_t = 失乐园_状态_类型
sly_cfunction_t = 失乐园_c函数_类型

sly_reg_t = 失乐园_寄存_类型
sly_open = 失乐园_打开
sly_close = 失乐园_关闭

sly_error = 失乐园_错误
sly_register = 失乐园_寄存器
sly_repl = 失乐园_读求值印环     //读取 求值 打印 循环
sly_load_buffer = 失乐园_载入_缓冲区

name = 名称
func = 函数短    //非全部拼写的词汇 就在后面加个 短 字
S = 大失    //大写就在前面加个 大 字
regs = 寄存复         //后缀加s代表复数 就在后面加个 复 字
buffer = 缓冲区

sly_get_top = 失乐园_取_顶部
sly_pop = 失乐园_弹
num = 数号短

idx = 索引短

//谓词类型
sly_boxp = 失乐园_谓词盒子
sly_charp = 失乐园_谓词印刻
sly_integerp = 失乐园_谓词整数
sly_numberp = 失乐园_谓词号码
sly_pairp = 失乐园_谓词点对
sly_listp = 失乐园_谓词列表
sly_stringp = 失乐园_谓词串
sly_vectorp = 失乐园_谓词向量
sly_procedurep = 失乐园_谓词过程
sly_eof_objectp = 失乐园_谓词文件终_对象
sly_input_portp = 失乐园_谓词输入_端口
sly_output_portp = 失乐园_谓词输出_端口


//推值进到其栈
sly_get_global = 失乐园_取_全局
sly_push_value = 失乐园_推_值
sly_push_boolean = 失乐园_推_布尔
sly_push_char = 失乐园_推_印刻
sly_push_integer = 失乐园_推_整数
sly_push_cclosure = 失乐园_推_c闭包
sly_push_string = 失乐园_推_串
sly_push_vector = 失乐园_推_向量

sly_push_current_input_port = 失乐园_推_当前_输入_端口
sly_push_current_output_port = 失乐园_推_当前_输出_端口
sly_push_current_error_port = 失乐园_推_当前_错误_端口

str = 串短
chr = 印刻短
size = 大小

//取值来自其栈
sly_to_integer = 失乐园_到_整数
sly_to_string = 失乐园_到_串
sly_to_string_latin1 = 失乐园_到_串_拉丁文1
sly_to_string_utf8 = 失乐园_到_串_万国码编码8
sly_to_string_utf16 = 失乐园_到_串_万国码编码16

//对比值在栈上
sly_less_than = 失乐园_小于_
sly_greater_than = 失乐园_大于_

//转换值在栈上
sly_symbol_to_string = 失乐园_符号_到_串
sly_string_to_symbol = 失乐园_串_到_符号

//算术
sly_invert = 失乐园_颠倒
sly_unary_minus = 失乐园_一元_减号
sly_add = 失乐园_加法
sly_subtract = 失乐园_减法
sly_divide = 失乐园_分号
sly_round = 失乐园_环绕
sly_remainder = 失乐园_余数

sly_number_to_string = 失乐园_数号_到_串

sly_box = 失乐园_盒子
sly_unbox = 失乐园_拆盒
sly_set_box = 失乐园_设置_盒子

sly_cons = 失乐园_构造
sly_car = 失乐园_切头
sly_cdr = 失乐园_切尾

sly_string_length = 失乐园_串_长度
sly_string_ref = 失乐园_串_引用
sly_concat = 失乐园_连接

sly_vector_length = 失乐园_向量_长度
sly_vector_ref = 失乐园_向量_引用
sly_vector_set = 失乐园_向量_设置

sly_apply = 失乐园_应用

sly_eval = 失乐园_求值
sly_call = 失乐园_调用
sly_load_file = 失乐园_载入_文件

//入 出
sly_open_input_file = 失乐园_打开_输入_文件
sly_open_output_file = 失乐园_打开_输出_文件
sly_close_input_port = 失乐园_关闭_输入_端口
sly_close_output_port = 失乐园_关闭_输入_端口
sly_newline = 失乐园_新行
sly_write = 失乐园_写
sly_display = 失乐园_显示

sly_set_global = 失乐园_设置_全局

//对象.h
SLY_TYPE_VOID = 失乐园_类型_空             
SLY_TYPE_UNDEF = 失乐园_类型_取消定义            
SLY_TYPE_NIL = 失乐园_类型_零              
SLY_TYPE_EOF = 失乐园_类型_文件终              
SLY_TYPE_BOOL = 失乐园_类型_布尔             
SLY_TYPE_FIXNUM = 失乐园_类型_固定整数
SLY_TYPE_CHAR = 失乐园_类型_印刻             
SLY_TYPE_SYMBOL = 失乐园_类型_符号
SLY_TYPE_CLOSURE = 失乐园_类型_闭包       
SLY_TYPE_PAIR = 失乐园_类型_点对     
SLY_TYPE_CONTI = 失乐园_类型_继续短          
SLY_TYPE_BOX = 失乐园_类型_盒子            
SLY_TYPE_STRING = 失乐园_类型_串         
SLY_TYPE_VECTOR = 失乐园_类型_向量        
SLY_TYPE_DYN_BIND = 失乐园_类型_动态_绑定      
SLY_TYPE_SYNCLO = 失乐园_类型_句法闭包        
SLY_TYPE_INPUT_PORT = 失乐园_类型_输入_端口     
SLY_TYPE_OUTPUT_PORT = 失乐园_类型_输出_端口

//垃圾回收的大小
SLY_SIZE_OF_BOX = 失乐园_大小_的_盒子                         
sly_box_t = 失乐园_盒子_类型
SLY_SIZE_OF_PAIR = 失乐园_大小_的_点对                        
sly_pair_t = 失乐园_点对_类型
SLY_SIZE_OF_CLOSURE = 失乐园_大小_的_闭包
sly_closure_t = 失乐园_闭包_类型
sly_object_t = 失乐园_对象_类型
SLY_SIZE_OF_CONTI = 失乐园_大小_的_继续短
sly_conti_t = 失乐园_继续短_类型
SLY_SIZE_OF_STRING = 失乐园_大小_的_串
sly_string_t = 失乐园_串_类型
SLY_SIZE_OF_VECTOR = 失乐园_大小_的_向量
sly_vector_t = 失乐园_向量_类型

SLY_SIZE_OF_DYN_BIND = 失乐园_大小_的_动态_绑定
sly_dyn_bind_t = 失乐园_动态_绑定_类型
SLY_SIZE_OF_SYNCLO = 失乐园_大小_的_句法闭包
sly_syn_closure_t = 失乐园_句法_闭包_类型
SLY_SIZE_OF_IPORT = 失乐园_大小_的_入端口
sly_iport_t = 失乐园_入端口_类型
SLY_SIZE_OF_OPORT = 失乐园_大小_的_出端口
sly_oport_t = 失乐园_出端口_类型

SLY_TYPE_PORT_FILE = 失乐园_类型_端口_文件          
SLY_PORT_BUF_SIZE = 失乐园_端口_缓冲_大小
sly_file_t = 失乐园_文件_类型

//向前类型声明
sly_gcobject_t = 失乐园_垃圾回收对象_类型
sly_symbol_t = 失乐园_符号_类型
sly_port_t = 失乐园_端口_类型

//投射
obj = 对象短
SLY_GCOBJECT = 失乐园_垃圾回收对象
SLY_BOX = 失乐园_盒子
SLY_CLOSURE = 失乐园_闭包
SLY_PAIR = 失乐园_点对
SLY_CONTI = 失乐园_继续短
SLY_STRING = 失乐园_串
SLY_VECTOR = 失乐园_向量
SLY_DYN_BIND = 失乐园_动态_绑定
SLY_SYNCLO = 失乐园_句法闭包
SLY_PORT = 失乐园_端口
SLY_IPORT = 失乐园_入端口
SLY_OPORT = 失乐园_出端口
SLY_OBJ_EQ = 失乐园_对象_等号

type = 类型
value = 值
symbol = 符号
o1 = 小对象1
o2 = 小对象2
gc = 小垃圾回收
base = 基础

is_c = 是否_小闭包
entry_point = 进入_点
free_vars = 自由_变量
stack = 栈
chars = 印刻复
data = 数据
tag = 标签
env = 环境短
//free = 自由
exp = 表达式短
char_enc = 印刻_编码
finish = 完成
self = 自己
peek_char = 窥_印刻
read_char = 读_印刻
flush = 刷新
write_char = 写_印刻
next = 下一个                   √

//对象创建
entry = 进入
stack_size = 栈_大小

sly_create_box = 失乐园_创建_盒子
sly_create_sclosure = 失乐园_创建_失闭包
sly_create_cclosure = 失乐园_创建_c闭包
sly_create_pair = 失乐园_创建_点对
sly_create_conti = 失乐园_创建_继续短
sly_create_string = 失乐园_创建_串
sly_create_string_from_ascii = 失乐园_创建_串_来自_美信交标码
sly_create_vector = 失乐园_创建_向量          //ASCII 美国信息交换标准代码
sly_create_dyn_bind = 失乐园_创建_动态_绑定
sly_create_syn_closure = 失乐园_创建_句法_闭包
sly_create_iport = 失乐园_创建_入端口
sly_create_oport = 失乐园_创建_出端口
sly_create_symbol = 失乐园_创建_符号
sly_create_symbol_from_ascii = 失乐园_创建_符号_来自_美信交标码

//垃圾回收.h
sly_fobj_t = 失乐园_最终对象_类型
sly_store_t = 失乐园_储存_类型
sly_roots_cb_t = 失乐园_根_回调_类型
capacity = 容量
os_address = 操系_地址   //操作系统
from_space = 来自_空间
to_space = 到_空间
fobjs = 最终对象复
roots_cb = 根_回调
roots_cb_data = 根_回调_数据
sly_gc_init = 失乐园_垃圾回收_初始
sly_gc_finish = 失乐园_垃圾回收_完成
port = 端口


//状态.h
STK = 栈短
STKGC = 垃圾回收栈
sly_env_t = 失乐园_环境_类型
sly_env_var_t = 失乐园_环境_变量_类型
vars = 变量复
code_size = 代码_大小
stack_size = 栈_大小
nr_consts = 数号_常量
nr = 小数号
sp = 栈指针
fp = 帧指针
pc = 程序计数器
accum = 累加短
proc = 过程短
global_env = 全局_环境 
code = 代码
consts = 常量短
symbol_table = 符号_表 
store = 储存
sly_st_enlarge_globals = 失乐园_状态_扩大_全局
sly_st_get_global_index = 失乐园_状态_取_全局_索引
global = 全局
more = 更多


//虚拟机器.h
SLY_OP_LOAD_NIL = 失乐园_操作_载入_零
SLY_OP_LOAD_FALSE = 失乐园_操作_载入_假
SLY_OP_LOAD_TRUE = 失乐园_操作_载入_真
SLY_OP_LOAD_UNDEF = 失乐园_操作_载入_取消定义
SLY_OP_LOAD_ZERO = 失乐园_操作_载入_零零   //同义字就用叠加法
SLY_OP_LOAD_ONE = 失乐园_操作_载入_一
SLY_OP_PUSH = 失乐园_操作_推
SLY_OP_LOAD_0 = 失乐园_操作_载入_0
SLY_OP_LOAD_1 = 失乐园_操作_载入_1
SLY_OP_LOAD_2 = 失乐园_操作_载入_2
SLY_OP_LOAD_3 = 失乐园_操作_载入_3
SLY_OP_RETURN = 失乐园_操作_返回
SLY_OP_SAVE_CONT = 失乐园_操作_保存_继续短
SLY_OP_REST_CONT = 失乐园_操作_重置_继续短
SLY_OP_BOX = 失乐园_操作_盒子
SLY_OP_OPEN_BOX = 失乐园_操作_打开_盒子
SLY_OP_HALT = 失乐园_操作_停机
SLY_OP_ABORT = 失乐园_操作_中止

SLY_OP_NULL_P = 失乐园_操作_空指针_问
SLY_OP_BOOL_P = 失乐园_操作_布尔_问
SLY_OP_CHAR_P = 失乐园_操作_印刻_问
SLY_OP_FIXNUM_P = 失乐园_操作_固定整数_问
SLY_OP_PAIR_P = 失乐园_操作_点对_问
SLY_OP_SYMBOL_P = 失乐园_操作_符号_问

//优化为指令的原语
SLY_OP_INC = 失乐园_操作_自增
SLY_OP_DEC = 失乐园_操作_自减
SLY_OP_FIXNUM_TO_CHAR = 失乐园_操作_固定整数_到_印刻
SLY_OP_CHAR_TO_FIXNUM = 失乐园_操作_印刻_到_固定整数
SLY_OP_ZERO_P = 失乐园_操作_零零_问
SLY_OP_NOT = 失乐园_操作_非
SLY_OP_PLUS = 失乐园_操作_加号
SLY_OP_MINUS = 失乐园_操作_减
SLY_OP_MULT = 失乐园_操作_乘
SLY_OP_CONS = 失乐园_操作_构造
SLY_OP_CAR = 失乐园_操作_切头
SLY_OP_CDR = 失乐园_操作_切尾
SLY_OP_NUM_EQ = 失乐园_操作_数号_等于
SLY_OP_EQ = 失乐园_操作_等于
SLY_OP_EQV = 失乐园_操作_相等
SLY_OP_MAKE_STRING = 失乐园_操作_制作_串
SLY_OP_STRING_SET = 失乐园_操作_串_设置
SLY_OP_STRING_TO_SYMBOL = 失乐园_操作_串_到_符号
SLY_OP_MAKE_VECTOR = 失乐园_操作_制作_向量
SLY_OP_VECTOR_SET = 失乐园_操作_向量_设置
SLY_OP_DEBUG = 失乐园_操作_调试

//一个操作数的指令
SLY_OP_LOAD_FIXNUM = 失乐园_操作_载入_固定整数
SLY_OP_LOAD_CHAR = 失乐园_操作_载入_印刻
SLY_OP_LOAD = 失乐园_操作_载入
SLY_OP_MAKE_CLOSURE = 失乐园_操作_制作_闭包
SLY_OP_JMP_IF_NOT = 失乐园_操作_跳_若_非
SLY_OP_JMP = 失乐园_操作_跳
SLY_OP_LOAD_FREE = 失乐园_操作_载入_自由
SLY_OP_ASSIGN = 失乐园_操作_赋值
SLY_OP_ASSIGN_FREE = 失乐园_操作_赋值_自由
SLY_OP_FRAME = 失乐园_操作_帧
SLY_OP_LOAD_LOCAL = 失乐园_操作_载入_本地
SLY_OP_INSERT_BOX = 失乐园_操作_插入_盒子
SLY_OP_ASSIGN_LOCAL = 失乐园_操作_赋值_本地
SLY_OP_POP = 失乐园_操作_弹
SLY_OP_GLOBAL_REF = 失乐园_操作_全局_引用
SLY_OP_CHECKED_GLOBAL_REF = 失乐园_操作_已检查_全局_引用
SLY_OP_GLOBAL_SET = 失乐园_操作_全局_设置
SLY_OP_CHECKED_GLOBAL_SET = 失乐园_操作_已检查_全局_设置
SLY_OP_CONST = 失乐园_操作_常量
SLY_OP_CONST_INIT = 失乐园_操作_常量_初始
SLY_OP_ARITY_EQ = 失乐园_操作_元数_等于
SLY_OP_ARITY_GE = 失乐园_操作_大等
SLY_OP_LISTIFY = 失乐园_操作_列出
SLY_OP_CALL = 失乐园_操作_调用
SLY_OP_TAIL_CALL = 失乐园_操作_尾_调用

SLY_HALT_ADDRESS = 失乐园_停机_地址
sly_module_t = 失乐园_模块_类型

nr_globals = 数号_全局
globals = 全局复
nr_consts = 数号_常量
sly_vm_init = 失乐园_虚拟机_初始
sly_vm_dump = 失乐园_虚拟机_转储
sly_vm_call = 失乐园_虚拟机_调用
sly_vm_load = 失乐园_虚拟机_载入
mod = 模块短
sly_vm_vector_to_module = 失乐园_虚拟机_向量_到_模块
vec = 向量短
sly_vm_link_run_module = 失乐园_虚拟机_链接_跑_模块

//输入输出.h
sly_strdup = 失乐园_串转储
sly_sbuffer_t = 失乐园_缓冲区_类型
sly_sbuffer_new = 失乐园_缓冲区_新
sly_sbuffer_destroy = 失乐园_缓冲区_摧毁
sly_sbuffer_assign = 失乐园_缓冲区_赋值
sly_sbuffer_add = 失乐园_缓冲区_添加
sly_sbuffer_string = 失乐园_缓冲区_串
sly_sbuffer_equalp = 失乐园_缓冲区_谓词等于

sly_io_to_latin1 = 失乐园_入出_到_拉丁文1
sly_io_to_utf8 = 失乐园_入出_到_万国码编码8
sly_io_to_utf16 = 失乐园_入出_万国码编码16
sly_io_create_stdin = 失乐园_入出_创建_标准入
sly_io_create_stdout = 失乐园_入出_创建_标准出
sly_io_create_stderr = 失乐园_入出_创建_标准错
sly_io_open_ifile = 失乐园_入出_打开_入文件
sly_io_open_ofile = 失乐园_入出_打开_出文件
sly_io_close_iport = 失乐园_入出_关闭_入端口
sly_io_close_oport = 失乐园_入出_关闭_出端口

sly_io_read_token = 失乐园_入出_读_牌
ret = 返回短
sly_io_newline = 失乐园_入出_新行
sly_io_write_c_string = 失乐园_入出_写_印名_串
sly_io_write_symbol = 失乐园_入出_写_符号
sly_io_write = 失乐园_入出_写
sly_io_display = 失乐园_入出_显示
sym = 符号短

boot_buf = 启动_缓冲
compiler_buf = 编译_缓冲
init_buf = 初始_缓冲

/********************
********************
********************
********************/

// .c文件的汉化
argc = 实参计数
argv = 实参值
repl = 读求值印环
string_equal_p = 串_等于_问
string_copy_extern = 串_复制_外部
sly_gc_alloc = 失乐园_垃圾回收_分配
nr_vars = 数号_变量
len = 长度短
tmp = 临时短


//应用程序接口.c
calc_index = 计算器_索引
numberp = 谓词数号
i = 计数甲
SLY_DEBUG_API = 失乐园_调试_应程接   //应用程序接口
check_type = 检查_类型
cdr = 切尾
fixnum = 固定整数
idx1 = 索引1
idx2 = 索引2
nr_nums = 数号_号码
first = 第一
last = 最后
box = 盒子
pair = 点对
car = 切头
pos = 位置短
nr_strs = 数号_串
j = 计数乙
k = 计数丙
total_size = 总共_大小
nr_args = 数号_实参
sly_read = 失乐园_读
out = 出


//垃圾回收.c
WSIZE = 字大小
SLY_INITIAL_SPACE_SIZE = 失乐园_初始_空间_大小
SLY_IMMEDIATE_P = 失乐园_立即_问号
SLY_FORWARD_TAG = 失乐园_向前_标签
sly_forward_t = 失乐园_向前_类型
ref = 引用短
sizeof_gcobj = 求大小_垃圾回收对象
nr_free = 数号_自由
copy_object = 复制_对象
collect_fobjs = 回收_向前对象
fobj = 向前对象
prev = 上一个
collect_garbage = 回收_垃圾
scan = 扫描
SLY_DTRACE = 失乐园_动态跟踪
old_size = 旧_大小
SLY_GC_START = 失乐园_垃圾回收_开始
gcobj = 垃圾回收对象
SLY_GC_END_ENABLED = 失乐园_垃圾回收_终_启用
SLY_GC_END = 失乐园_垃圾回收_终
expand_store = 扩展_储存
SLY_GC_RESIZE_ENABLED = 失乐园_垃圾回收_重设大小_启用
SLY_GC_RESIZE = 失乐园_垃圾回收_重设大小
cb = 小回调
ud = 小用户数据
SLY_GC_ALLOC_ENABLED = 失乐园_垃圾回收_分配_启用
SLY_GC_ALLOC = 失乐园_垃圾回收_分配
sly_gc_add_port = 失乐园_垃圾回收_添加_端口
link = 链接


//输入输出.c
SLY_UCS_CHAR_TAB = 失乐园_通用字符集_印刻_表
SLY_UCS_LINE_FEED = 失乐园_通用字符集_行_喂养
SLY_UCS_LINE_TAB = 失乐园_通用字符集_行_表
SLY_UCS_FORM_FEED = 失乐园_通用字符集_来自_喂养
SLY_UCS_CAR_RETURN = 失乐园_通用字符集_切头_返回
SLY_UCS_SPACE = 失乐园_通用字符集_空格
SLY_UCS_NEXT_LINE = 失乐园_通用字符集_下一个_行
SLY_UCS_LINE_SEP = 失乐园_通用字符集_行_自升式
SLY_UCS_PARA_SEP = 失乐园_通用字符集_伞兵_自升式
SLY_UCS_LEFT_QUOTE = 失乐园_通用字符集_左_引号
SLY_UCS_RIGHT_QUOTE = 失乐园_通用字符集_右_引号
SLY_UCS_ELLIPSIS = 失乐园_通用字符集_省略号
SLY_UCS_FRACTION_SLASH = 失乐园_通用字符集_分数_斜杠

SLY_UCS_EOF = 失乐园_通用字符集_文件终
SLY_UCS_NO_CHAR = 失乐园_通用字符集_不_印刻

SLY_TOK_NONE = 失乐园_牌_没有
SLY_TOK_EOF = 失乐园_牌_文件终
SLY_TOK_DATUM = 失乐园_牌_基准
SLY_TOK_LEFT_PAREN = 失乐园_牌_左_小括号
SLY_TOK_RIGHT_PAREN = 失乐园_牌_右_小括号
SLY_TOK_SHARP_PAREN = 失乐园_牌_尖_小括号
SLY_TOK_QUOTE = 失乐园_牌_引号
SLY_TOK_BACKQUOTE = 失乐园_牌_反引号
SLY_TOK_COMMA = 失乐园_牌_逗号
SLY_TOK_COMMA_AT = 失乐园_牌_逗号_在
SLY_TOK_DOT = 失乐园_牌_句号

delim_set = 分隔符_设置
pec_begin_set = 加减句_开始_设置
sym_begin_set = 符号_开始_设置
num_set = 数号_设置
space_str = 空格_串
newline_str = 新行_串
ellipsis_str = 省略号_串

is_line_ending = 是否_行_终
is_space = 是否_空格
is_char_in_set = 是否_印刻_内_设置
set = 设置
str_len = 串_长度
char2utf8 = 印刻到万国码编码8
buf = 缓冲短
char2utf16 = 印刻到万国码编码16
buf_ = 缓冲短_
char2latin1 = 印刻到拉丁文1
from_string = 来自_串
sly_io_create_sbuffer = 失乐园_入出_创建_串缓冲区
sly_io_destroy_sbuffer = 失乐园_入出_摧毁_串缓冲区
fp_priv = 文件端口_私有
closable = 可关闭
fp_finish = 文件端口_完成
priv = 私有短
fp_read_char_utf8 = 文件端口_读_印刻_万国码编码8
fp_read_char_utf16 = 文件端口_读_印刻_万国码编码16
fp_read_char_latin1 = 文件端口_读_印刻_拉丁文1
fp_read_once = 文件端口_读_一次
fp_read_char = 文件端口_读_印刻
fp_peek_char = 文件端口_窥_印刻
fp_flush = 文件端口_刷新
fp_write_char = 文件端口_写_印刻
PEEK_CHAR = 窥_印刻_大
READ_CHAR = 读_印刻_大
read_string = 读_串
read_till_delimiter = 读_直到_分隔符
digit_value = 数字_值
digits = 数字复
parse_number = 解析_数号
val = 值短
res = 结果短
is_exact = 是否_确切
sign = 征兆
read_token = 读_牌
tok = 牌短
tmp1 = 临时1
tmp2 = 临时2
FLUSH = 刷新大
WRITE_CHAR = 写_印刻_大
write_string = 写_串
quote = 引号
sly_io_write_i = 失乐园_入出_写_入
sly_io_create_ifport = 失乐园_入出_创建_入文件端口
file = 文件
sly_io_create_ofport = 失乐园_入出_创建_出文件端口
//stdin = 标准入
//stderr = 标准错
fname = 文件名


//虚拟机.c
IS_TYPE_B = 是否_类型_乙
instr = 指令短
EXTRACT_OP = 提取_操作
EXTRACT_ARG = 提取_实参
sly_opcode_ = 失乐园_操作码_
op = 小操作
sly_opcode_t = 失乐园_操作码_类型
global_opcodes = 全局_操作码
opc = 小操作码

dump_instr = 转储_指令
dbg = 小调试
disassemble = 反汇编
check_alloc = 检查_分配
ptr = 指针短
return_from_call = 返回_来自_调用
call_c_closure = 调用_c_闭包
clos = 闭包短_
SLY_SET_BOOL = 失乐园_设置_布尔
cond = 条件短
sly_vm_run = 失乐园_虚拟机_跑
debug = 调试
go_on = 继_续
nargs = 实参数号
sly_destroy_module = 失乐园_摧毁_模块
sly_link_module = 失乐园_链接_模块
consts_base = 常量_基础
code_base = 代码_基础
growth = 生长
CAR = 切头大
CDR = 切尾大
VECTOR_REF = 向量_引用
get_opcode = 取_操作码
list_length = 列表_长度
lis = 列表短
vec_instr = 向量_指令
FIXNUM = 固定整数大
write_fixnum = 写_固定整数
fix = 固定短
sly_vm_write_code = 失乐园_虚拟机_写_代码
get_next = 取_下一个
get_fixnum = 取_固定整数
get_string = 取_串
load_code_from_file = 载入_代码_来自_文件
dw1 = 数据字1
dw2 = 数据字2
get_fixnum_b = 取_固定整数_乙
get_string_b = 取_串_乙
load_code_from_buffer = 载入_代码_来自_缓冲区


//库.c
dynamic_lookup = 动态_查找
dynamic_store = 动态_储存
open_stdin_port = 打开_标准入_端口
open_stdout_port = 打开_标准出_端口
open_stderr_port = 打开_标准错_端口
make_syn_clo = 制作_句法_闭包
synclo = 句法闭包
syn_clo_p = 句法_闭包_问
syn_clo_env = 句法_闭包_环境
syn_clo_free = 句法_闭包_自由
syn_clo_exp = 句法_闭包_表达式
lib_regs = 库_寄存
sly_open_lib = 失乐园_打开_库


//状态.c
sly_open_std = 失乐园_打开_标准
gc_data = 垃圾回收_数据
state = 状态
count = 计数
gc_callback = 垃圾回收_回调
sly_create_state = 失乐园_创建_状态


//标准.c
compare = 对比
less_than = 小_于
greater_than = 大_于
plus = 加
minus = 减
divide = 除
_round = _环绕
quotient = 商
_remainder = 余数
number_to_string = 数号_到_串
cons = 构造短
listp = 列表问
string_to_symbol = 串_到_符号
symbol_to_string = 符号_到_串
stringp = 串问
string_length = 串_长度
string_ref = 串_引用
string_append = 串_追加
vectorp = 向量问
make_vector = 制作_向量
vector_length = 向量_长度
vector_ref = 向量_引用
vector_set = 向量_设置
procedurep = 过程问
apply = 应用
eval = 求值
load = 载入
input_portp = 输入_端口问
eof_objectp = 文件终_对象问
output_portp = 输出_端口问
open_input_file = 打开_输入_文件
open_output_file = 打开_输出_文件
close_input_port = 关闭_输入_端口
close_output_port = 关闭_输出_端口
write = 写
display = 显示
newline = 新行
boxp = 盒子问
unbox = 拆盒
set_box = 设置_盒子
error = 错误
std_regs = 标准_寄存
