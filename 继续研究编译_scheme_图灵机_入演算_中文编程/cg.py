from chibicc import *  # 假设 chibicc.h 中的内容已经转换为 Python 模块

# 定义常量
通用寄存器最大数量 = 6
浮点寄存器最大数量 = 8

# 静态变量
输出文件 = None  # 假设 FILE 类型在 Python 中用 None 表示
深度 = 0
参数寄存器8 = ["%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"]
参数寄存器16 = ["%di", "%si", "%dx", "%cx", "%r8w", "%r9w"]
参数寄存器32 = ["%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"]
参数寄存器64 = ["%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"]
当前函数 = None  # 假设 Obj 类型在 Python 中用 None 表示

# 静态函数声明
def 生成表达式(节点):
    pass  # 具体实现省略

def 生成语句(节点):
    pass  # 具体实现省略
    
import sys
import ctypes

# 假设 FILE 类型在 Python 中用 ctypes.c_void_p 表示
输出文件 = ctypes.c_void_p()
深度 = 0

# 函数 println
def 打印行(格式, *参数):
    ap = ctypes.c_void_p()
    ctypes.va_start(ap, 格式)
    ctypes.vfprintf(输出文件, 格式, ap)
    ctypes.va_end(ap)
    ctypes.fprintf(输出文件, "\n")

# 函数 count
def 计数():
    i = 1
    while True:
        yield i
        i += 1

# 函数 push
def 压栈():
    打印行("  push %%rax")
    深度 += 1

# 函数 pop
def 弹栈(参数):
    打印行("  pop %s", 参数)
    深度 -= 1

# 函数 pushf
def 压栈浮点():
    打印行("  sub $8, %%rsp")
    打印行("  movsd %%xmm0, (%%rsp)")
    深度 += 1

# 函数 popf
def 弹栈浮点(寄存器):
    打印行("  movsd (%%rsp), %%xmm%d", 寄存器)
    打印行("  add $8, %%rsp")
    深度 -= 1
    
# 将 `n` 向上舍入到最接近的 `align` 的倍数。例如，
# align_to(5, 8) 返回 8，align_to(11, 8) 返回 16。
def 对齐到(n, 对齐):
    return (n + 对齐 - 1) // 对齐 * 对齐

# 根据大小返回对应的寄存器名称
def 寄存器_dx(大小):
    if 大小 == 1:
        return "%dl"
    elif 大小 == 2:
        return "%dx"
    elif 大小 == 4:
        return "%edx"
    elif 大小 == 8:
        return "%rdx"
    else:
        raise Exception("不可达")

# 根据大小返回对应的寄存器名称
def 寄存器_ax(大小):
    if 大小 == 1:
        return "%al"
    elif 大小 == 2:
        return "%ax"
    elif 大小 == 4:
        return "%eax"
    elif 大小 == 8:
        return "%rax"
    else:
        raise Exception("不可达")
        
# 计算给定节点的绝对地址。
# 如果给定节点不在内存中，则报错。
def 生成地址(节点):
    if 节点.类型 == "变量":
        # 可变长度数组，总是局部变量。
        if 节点.变量.类型.类型 == "可变长度数组":
            打印行("  mov %d(%%rbp), %%rax", 节点.变量.偏移)
            return

        # 局部变量
        if 节点.变量.是局部变量:
            打印行("  lea %d(%%rbp), %%rax", 节点.变量.偏移)
            return

        if opt_fpic:
            # 线程局部变量
            if 节点.变量.是线程局部变量:
                打印行("  data16 lea %s@tlsgd(%%rip), %%rdi", 节点.变量.名称)
                打印行("  .value 0x6666")
                打印行("  rex64")
                打印行("  call __tls_get_addr@PLT")
                return

            # 函数或全局变量
            打印行("  mov %s@GOTPCREL(%%rip), %%rax", 节点.变量.名称)
            return

        # 线程局部变量
        if 节点.变量.是线程局部变量:
            打印行("  mov %%fs:0, %%rax")
            打印行("  add $%s@tpoff, %%rax", 节点.变量.名称)
            return

        # 函数
        if 节点.类型.类型 == "函数":
            if 节点.变量.是定义:
                打印行("  lea %s(%%rip), %%rax", 节点.变量.名称)
            else:
                打印行("  mov %s@GOTPCREL(%%rip), %%rax", 节点.变量.名称)
            return

        # 全局变量
        打印行("  lea %s(%%rip), %%rax", 节点.变量.名称)
        return
    elif 节点.类型 == "解引用":
        生成表达式(节点.左)
        return
    elif 节点.类型 == "逗号":
        生成表达式(节点.左)
        生成地址(节点.右)
        return
    elif 节点.类型 == "成员":
        生成地址(节点.左)
        打印行("  add $%d, %%rax", 节点.成员.偏移)
        return
    elif 节点.类型 == "函数调用":
        if 节点.返回缓冲区:
            生成表达式(节点)
            return
        return
    elif 节点.类型 == "赋值" or 节点.类型 == "条件":
        if 节点.类型.类型 == "结构体" or 节点.类型.类型 == "联合体":
            生成表达式(节点)
            return
        return
    elif 节点.类型 == "可变长度数组指针":
        打印行("  lea %d(%%rbp), %%rax", 节点.变量.偏移)
        return

    错误_tok(节点.标记, "不是左值")
    
# 从 %rax 指向的位置加载一个值。
def 加载(类型):
    if 类型.类型 in ["数组", "结构体", "联合体", "函数", "可变长度数组"]:
        # 如果是数组，不要尝试将值加载到寄存器中，
        # 因为通常我们无法将整个数组加载到寄存器中。
        # 因此，数组求值的结果不是数组本身，而是数组的地址。
        # 这就是“数组在C中自动转换为指向数组第一个元素的指针”的地方。
        return
    elif 类型.类型 == "浮点数":
        打印行("  movss (%%rax), %%xmm0")
        return
    elif 类型.类型 == "双精度浮点数":
        打印行("  movsd (%%rax), %%xmm0")
        return
    elif 类型.类型 == "长双精度浮点数":
        打印行("  fldt (%%rax)")
        return

    指令 = "movz" if 类型.是无符号 else "movs"

    # 当我们加载一个字符或短整数值到寄存器时，我们总是
    # 将它们扩展到整数的大小，因此我们可以假设寄存器的下半部分
    # 总是包含一个有效的值。字符、短整数和整数的寄存器的上半部分
    # 可能包含垃圾。当我们加载一个长整数值到寄存器时，它只是
    # 占据了整个寄存器。
    if 类型.大小 == 1:
        打印行("  %sbl (%%rax), %%eax" % 指令)
    elif 类型.大小 == 2:
        打印行("  %swl (%%rax), %%eax" % 指令)
    elif 类型.大小 == 4:
        打印行("  movsxd (%%rax), %%rax")
    else:
        打印行("  mov (%%rax), %%rax")

# 将 %rax 存储到栈顶指向的地址。
def 存储(类型):
    弹栈("%rdi")

    if 类型.类型 in ["结构体", "联合体"]:
        for i in range(类型.大小):
            打印行("  mov %d(%%rax), %%r8b" % i)
            打印行("  mov %%r8b, %d(%%rdi)" % i)
        return
    elif 类型.类型 == "浮点数":
        打印行("  movss %%xmm0, (%%rdi)")
        return
    elif 类型.类型 == "双精度浮点数":
        打印行("  movsd %%xmm0, (%%rdi)")
        return
    elif 类型.类型 == "长双精度浮点数":
        打印行("  fstpt (%%rdi)")
        return

    if 类型.大小 == 1:
        打印行("  mov %%al, (%%rdi)")
    elif 类型.大小 == 2:
        打印行("  mov %%ax, (%%rdi)")
    elif 类型.大小 == 4:
        打印行("  mov %%eax, (%%rdi)")
    else:
        打印行("  mov %%rax, (%%rdi)")
        
# 比较类型为零的值
def 比较零(类型):
    if 类型.类型 == "浮点数":
        打印行("  xorps %%xmm1, %%xmm1")
        打印行("  ucomiss %%xmm1, %%xmm0")
        return
    elif 类型.类型 == "双精度浮点数":
        打印行("  xorpd %%xmm1, %%xmm1")
        打印行("  ucomisd %%xmm1, %%xmm0")
        return
    elif 类型.类型 == "长双精度浮点数":
        打印行("  fldz")
        打印行("  fucomip")
        打印行("  fstp %%st(0)")
        return

    if 是整数(类型) and 类型.大小 <= 4:
        打印行("  cmp $0, %%eax")
    else:
        打印行("  cmp $0, %%rax")

# 类型ID枚举
I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 = range(11)

# 获取类型ID
def 获取类型ID(类型):
    if 类型.类型 == "字符":
        return U8 if 类型.是无符号 else I8
    elif 类型.类型 == "短整数":
        return U16 if 类型.是无符号 else I16
    elif 类型.类型 == "整数":
        return U32 if 类型.是无符号 else I32
    elif 类型.类型 == "长整数":
        return U64 if 类型.是无符号 else I64
    elif 类型.类型 == "浮点数":
        return F32
    elif 类型.类型 == "双精度浮点数":
        return F64
    elif 类型.类型 == "长双精度浮点数":
        return F80

    return U64
    
# 类型转换表
i32i8 = "movsbl %al, %eax"
i32u8 = "movzbl %al, %eax"
i32i16 = "movswl %ax, %eax"
i32u16 = "movzwl %ax, %eax"
i32f32 = "cvtsi2ssl %eax, %xmm0"
i32i64 = "movsxd %eax, %rax"
i32f64 = "cvtsi2sdl %eax, %xmm0"
i32f80 = "mov %eax, -4(%rsp); fildl -4(%rsp)"

u32f32 = "mov %eax, %eax; cvtsi2ssq %rax, %xmm0"
u32i64 = "mov %eax, %eax"
u32f64 = "mov %eax, %eax; cvtsi2sdq %rax, %xmm0"
u32f80 = "mov %eax, %eax; mov %rax, -8(%rsp); fildll -8(%rsp)"

i64f32 = "cvtsi2ssq %rax, %xmm0"
i64f64 = "cvtsi2sdq %rax, %xmm0"
i64f80 = "movq %rax, -8(%rsp); fildll -8(%rsp)"

u64f32 = "cvtsi2ssq %rax, %xmm0"
u64f64 = (
    "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
    "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
    "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:"
)
u64f80 = (
    "mov %rax, -8(%rsp); fildq -8(%rsp); test %rax, %rax; jns 1f;"
    "mov $1602224128, %eax; mov %eax, -4(%rsp); fadds -4(%rsp); 1:"
)

f32i8 = "cvttss2sil %xmm0, %eax; movsbl %al, %eax"
f32u8 = "cvttss2sil %xmm0, %eax; movzbl %al, %eax"
f32i16 = "cvttss2sil %xmm0, %eax; movswl %ax, %eax"
f32u16 = "cvttss2sil %xmm0, %eax; movzwl %ax, %eax"
f32i32 = "cvttss2sil %xmm0, %eax"
f32u32 = "cvttss2siq %xmm0, %rax"
f32i64 = "cvttss2siq %xmm0, %rax"
f32u64 = "cvttss2siq %xmm0, %rax"
f32f64 = "cvtss2sd %xmm0, %xmm0"
f32f80 = "movss %xmm0, -4(%rsp); flds -4(%rsp)"

f64i8 = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax"
f64u8 = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax"
f64i16 = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax"
f64u16 = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax"
f64i32 = "cvttsd2sil %xmm0, %eax"
f64u32 = "cvttsd2siq %xmm0, %rax"
f64i64 = "cvttsd2siq %xmm0, %rax"
f64u64 = "cvttsd2siq %xmm0, %rax"
f64f32 = "cvtsd2ss %xmm0, %xmm0"
f64f80 = "movsd %xmm0, -8(%rsp); fldl -8(%rsp)"

FROM_F80_1 = (
    "fnstcw -10(%rsp); movzwl -10(%rsp), %eax; or $12, %ah; "
    "mov %ax, -12(%rsp); fldcw -12(%rsp); "
)

FROM_F80_2 = " -24(%rsp); fldcw -10(%rsp); "

f80i8 = FROM_F80_1 + "fistps" + FROM_F80_2 + "movsbl -24(%rsp), %eax"
f80u8 = FROM_F80_1 + "fistps" + FROM_F80_2 + "movzbl -24(%rsp), %eax"
f80i16 = FROM_F80_1 + "fistps" + FROM_F80_2 + "movzbl -24(%rsp), %eax"
f80u16 = FROM_F80_1 + "fistpl" + FROM_F80_2 + "movswl -24(%rsp), %eax"
f80i32 = FROM_F80_1 + "fistpl" + FROM_F80_2 + "mov -24(%rsp), %eax"
f80u32 = FROM_F80_1 + "fistpl" + FROM_F80_2 + "mov -24(%rsp), %eax"
f80i64 = FROM_F80_1 + "fistpq" + FROM_F80_2 + "mov -24(%rsp), %rax"
f80u64 = FROM_F80_1 + "fistpq" + FROM_F80_2 + "mov -24(%rsp), %rax"
f80f32 = "fstps -8(%rsp); movss -8(%rsp), %xmm0"
f80f64 = "fstpl -8(%rsp); movsd -8(%rsp), %xmm0"

# 类型转换表
转换表 = [
    # i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
    [None, None,   None,   i32i64, i32u8, i32u16, None,   i32i64, i32f32, i32f64, i32f80], # i8
    [i32i8, None,   None,   i32i64, i32u8, i32u16, None,   i32i64, i32f32, i32f64, i32f80], # i16
    [i32i8, i32i16, None,   i32i64, i32u8, i32u16, None,   i32i64, i32f32, i32f64, i32f80], # i32
    [i32i8, i32i16, None,   None,   i32u8, i32u16, None,   None,   i64f32, i64f64, i64f80], # i64

    [i32i8, None,   None,   i32i64, None,  None,   None,   i32i64, i32f32, i32f64, i32f80], # u8
    [i32i8, i32i16, None,   i32i64, i32u8, None,   None,   i32i64, i32f32, i32f64, i32f80], # u16
    [i32i8, i32i16, None,   u32i64, i32u8, i32u16, None,   u32i64, u32f32, u32f64, u32f80], # u32
    [i32i8, i32i16, None,   None,   i32u8, i32u16, None,   None,   u64f32, u64f64, u64f80], # u64

    [f32i8, f32i16, f32i32, f32i64, f32u8, f32u16, f32u32, f32u64, None,   f32f64, f32f80], # f32
    [f64i8, f64i16, f64i32, f64i64, f64u8, f64u16, f64u32, f64u64, f64f32, None,   f64f80], # f64
    [f80i8, f80i16, f80i32, f80i64, f80u8, f80u16, f80u32, f80u64, f80f32, f80f64, None],   # f80
]

# 类型转换函数
def 类型转换(来源, 目标):
    if 目标.类型 == "空":
        return

    if 目标.类型 == "布尔":
        比较零(来源)
        打印行("  setne %%al")
        打印行("  movzx %%al, %%eax")
        return

    类型ID1 = 获取类型ID(来源)
    类型ID2 = 获取类型ID(目标)
    if 转换表[类型ID1][类型ID2]:
        打印行("  %s" % 转换表[类型ID1][类型ID2])

# 检查结构体或联合体在指定字节范围内是否只包含浮点数成员
def 包含浮点数(类型, 下限, 上限, 偏移):
    if 类型.类型 in ["结构体", "联合体"]:
        for 成员 in 类型.成员:
            if not 包含浮点数(成员.类型, 下限, 上限, 偏移 + 成员.偏移):
                return False
        return True

    if 类型.类型 == "数组":
        for i in range(类型.数组长度):
            if not 包含浮点数(类型.基类型, 下限, 上限, 偏移 + 类型.基类型.大小 * i):
                return False
        return True

    return 偏移 < 下限 or 上限 <= 偏移 or 类型.类型 in ["浮点数", "双精度浮点数"]
    
# 检查结构体或联合体的前8字节是否只包含浮点数成员
def 包含浮点数1(类型):
    return 包含浮点数(类型, 0, 8, 0)

# 检查结构体或联合体的第8到16字节是否只包含浮点数成员
def 包含浮点数2(类型):
    return 包含浮点数(类型, 8, 16, 0)

# 将结构体或联合体压入栈
def 压入结构体(类型):
    大小 = 对齐到(类型.大小, 8)
    打印行("  sub $%d, %%rsp" % 大小)
    深度 += 大小 // 8

    for i in range(类型.大小):
        打印行("  mov %d(%%rax), %%r10b" % i)
        打印行("  mov %%r10b, %d(%%rsp)" % i)

# 递归压入函数调用参数
def 压入参数2(参数, 首次传递):
    if not 参数:
        return
    压入参数2(参数.下一个, 首次传递)

    if (首次传递 and not 参数.通过栈传递) or (not 首次传递 and 参数.通过栈传递):
        return

    生成表达式(参数)

    if 参数.类型.类型 in ["结构体", "联合体"]:
        压入结构体(参数.类型)
    elif 参数.类型.类型 in ["浮点数", "双精度浮点数"]:
        压入浮点()
    elif 参数.类型.类型 == "长双精度浮点数":
        打印行("  sub $16, %%rsp")
        打印行("  fstpt (%%rsp)")
        深度 += 2
    else:
        压入()

# 加载函数调用参数
def 压入参数(节点):
    栈 = 0
    通用寄存器 = 0
    浮点寄存器 = 0

    # 如果返回类型是大的结构体或联合体，调用者传递一个指针作为第一个参数
    if 节点.返回缓冲区 and 节点.类型.大小 > 16:
        通用寄存器 += 1

    # 尽可能将参数加载到寄存器中
    for 参数 in 节点.参数:
        类型 = 参数.类型

        if 类型.类型 in ["结构体", "联合体"]:
            if 类型.大小 > 16:
                参数.通过栈传递 = True
                栈 += 对齐到(类型.大小, 8) // 8
            else:
                浮点数1 = 包含浮点数1(类型)
                浮点数2 = 包含浮点数2(类型)

                if 浮点寄存器 + 浮点数1 + 浮点数2 < FP_MAX and 通用寄存器 + (not 浮点数1) + (not 浮点数2) < GP_MAX:
                    浮点寄存器 = 浮点寄存器 + 浮点数1 + 浮点数2
                    通用寄存器 = 通用寄存器 + (not 浮点数1) + (not 浮点数2)
                else:
                    参数.通过栈传递 = True
                    栈 += 对齐到(类型.大小, 8) // 8
        elif 类型.类型 in ["浮点数", "双精度浮点数"]:
            if 浮点寄存器 >= FP_MAX:
                参数.通过栈传递 = True
                栈 += 1
        elif 类型.类型 == "长双精度浮点数":
            参数.通过栈传递 = True
            栈 += 2
        else:
            if 通用寄存器 >= GP_MAX:
                参数.通过栈传递 = True
                栈 += 1

    if (深度 + 栈) % 2 == 1:
        打印行("  sub $8, %%rsp")
        深度 += 1
        栈 += 1

    压入参数2(节点.参数, True)
    压入参数2(节点.参数, False)

    # 如果返回类型是大的结构体或联合体，调用者传递一个指针作为第一个参数
    if 节点.返回缓冲区 and 节点.类型.大小 > 16:
        打印行("  lea %d(%%rbp), %%rax" % 节点.返回缓冲区.偏移)
        压入()

    return 栈
    
# 复制返回缓冲区
def 复制返回缓冲区(变量):
    类型 = 变量.类型
    通用寄存器 = 0
    浮点寄存器 = 0

    if 包含浮点数1(类型):
        assert 类型.大小 == 4 or 8 <= 类型.大小
        if 类型.大小 == 4:
            打印行("  movss %%xmm0, %d(%%rbp)" % 变量.偏移)
        else:
            打印行("  movsd %%xmm0, %d(%%rbp)" % 变量.偏移)
        浮点寄存器 += 1
    else:
        for i in range(min(8, 类型.大小)):
            打印行("  mov %%al, %d(%%rbp)" % (变量.偏移 + i))
            打印行("  shr $8, %%rax")
        通用寄存器 += 1

    if 类型.大小 > 8:
        if 包含浮点数2(类型):
            assert 类型.大小 == 12 or 类型.大小 == 16
            if 类型.大小 == 12:
                打印行("  movss %%xmm%d, %d(%%rbp)" % (浮点寄存器, 变量.偏移 + 8))
            else:
                打印行("  movsd %%xmm%d, %d(%%rbp)" % (浮点寄存器, 变量.偏移 + 8))
        else:
            寄存器1 = "%al" if 通用寄存器 == 0 else "%dl"
            寄存器2 = "%rax" if 通用寄存器 == 0 else "%rdx"
            for i in range(8, min(16, 类型.大小)):
                打印行("  mov %s, %d(%%rbp)" % (寄存器1, 变量.偏移 + i))
                打印行("  shr $8, %s" % 寄存器2)

# 复制结构体寄存器
def 复制结构体寄存器():
    类型 = 当前函数.类型.返回类型
    通用寄存器 = 0
    浮点寄存器 = 0

    打印行("  mov %%rax, %%rdi")

    if 包含浮点数(类型, 0, 8, 0):
        assert 类型.大小 == 4 or 8 <= 类型.大小
        if 类型.大小 == 4:
            打印行("  movss (%%rdi), %%xmm0")
        else:
            打印行("  movsd (%%rdi), %%xmm0")
        浮点寄存器 += 1
    else:
        打印行("  mov $0, %%rax")
        for i in range(min(8, 类型.大小) - 1, -1, -1):
            打印行("  shl $8, %%rax")
            打印行("  mov %d(%%rdi), %%al" % i)
        通用寄存器 += 1

    if 类型.大小 > 8:
        if 包含浮点数(类型, 8, 16, 0):
            assert 类型.大小 == 12 or 类型.大小 == 16
            if 类型.大小 == 4:
                打印行("  movss 8(%%rdi), %%xmm%d" % 浮点寄存器)
            else:
                打印行("  movsd 8(%%rdi), %%xmm%d" % 浮点寄存器)
        else:
            寄存器1 = "%al" if 通用寄存器 == 0 else "%dl"
            寄存器2 = "%rax" if 通用寄存器 == 0 else "%rdx"
            打印行("  mov $0, %s" % 寄存器2)
            for i in range(min(16, 类型.大小) - 1, 7, -1):
                打印行("  shl $8, %s" % 寄存器2)
                打印行("  mov %d(%%rdi), %s" % (i, 寄存器1))

# 复制结构体内存
def 复制结构体内存():
    类型 = 当前函数.类型.返回类型
    变量 = 当前函数.参数

    打印行("  mov %d(%%rbp), %%rdi" % 变量.偏移)

    for i in range(类型.大小):
        打印行("  mov %d(%%rax), %%dl" % i)
        打印行("  mov %%dl, %d(%%rdi)" % i)

# 内置 alloca 函数
def 内置_alloca():
    # 对齐大小到 16 字节
    打印行("  add $15, %%rdi")
    打印行("  and $0xfffffff0, %%edi")

    # 移动临时区域
    打印行("  mov %d(%%rbp), %%rcx" % 当前函数.alloca_bottom.偏移)
    打印行("  sub %%rsp, %%rcx")
    打印行("  mov %%rsp, %%rax")
    打印行("  sub %%rdi, %%rsp")
    打印行("  mov %%rsp, %%rdx")
    打印行("1:")
    打印行("  cmp $0, %%rcx")
    打印行("  je 2f")
    打印行("  mov (%%rax), %%r8b")
    打印行("  mov %%r8b, (%%rdx)")
    打印行("  inc %%rdx")
    打印行("  inc %%rax")
    打印行("  dec %%rcx")
    打印行("  jmp 1b")
    打印行("2:")

    # 移动 alloca_bottom 指针
    打印行("  mov %d(%%rbp), %%rax" % 当前函数.alloca_bottom.偏移)
    打印行("  sub %%rdi, %%rax")
    打印行("  mov %%rax, %d(%%rbp)" % 当前函数.alloca_bottom.偏移)
    
# 生成给定节点的代码
def 生成表达式(节点):
    打印行("  .loc %d %d" % (节点.标记.文件.文件编号, 节点.标记.行号))

    if 节点.类型 == "空表达式":
        return
    elif 节点.类型 == "数值":
        if 节点.类型.类型 == "浮点数":
            u = { "f32": 节点.浮点值, "u32": 0 }
            打印行("  mov $%u, %%eax  # float %Lf" % (u["u32"], 节点.浮点值))
            打印行("  movq %%rax, %%xmm0")
            return
        elif 节点.类型.类型 == "双精度浮点数":
            u = { "f64": 节点.浮点值, "u64": 0 }
            打印行("  mov $%lu, %%rax  # double %Lf" % (u["u64"], 节点.浮点值))
            打印行("  movq %%rax, %%xmm0")
            return
        elif 节点.类型.类型 == "长双精度浮点数":
            u = { "f80": 节点.浮点值, "u64": [0, 0] }
            memset(u, 0, sizeof(u))
            u["f80"] = 节点.浮点值
            打印行("  mov $%lu, %%rax  # long double %Lf" % (u["u64"][0], 节点.浮点值))
            打印行("  mov %%rax, -16(%%rsp)")
            打印行("  mov $%lu, %%rax" % u["u64"][1])
            打印行("  mov %%rax, -8(%%rsp)")
            打印行("  fldt -16(%%rsp)")
            return

        打印行("  mov $%ld, %%rax" % 节点.值)
        return
    elif 节点.类型 == "负数":
        生成表达式(节点.左)

        if 节点.类型.类型 == "浮点数":
            打印行("  mov $1, %%rax")
            打印行("  shl $31, %%rax")
            打印行("  movq %%rax, %%xmm1")
            打印行("  xorps %%xmm1, %%xmm0")
            return
        elif 节点.类型.类型 == "双精度浮点数":
            打印行("  mov $1, %%rax")
            打印行("  shl $63, %%rax")
            打印行("  movq %%rax, %%xmm1")
            打印行("  xorpd %%xmm1, %%xmm0")
            return
        elif 节点.类型.类型 == "长双精度浮点数":
            打印行("  fchs")
            return

        打印行("  neg %%rax")
        return
    elif 节点.类型 == "变量":
        生成地址(节点)
        加载(节点.类型)
        return
    elif 节点.类型 == "成员":
        生成地址(节点)
        加载(节点.类型)

        成员 = 节点.成员
        if 成员.是位域:
            打印行("  shl $%d, %%rax" % (64 - 成员.位宽 - 成员.位偏移))
            if 成员.类型.是无符号:
                打印行("  shr $%d, %%rax" % (64 - 成员.位宽))
            else:
                打印行("  sar $%d, %%rax" % (64 - 成员.位宽))
        return
    elif 节点.类型 == "解引用":
        生成表达式(节点.左)
        加载(节点.类型)
        return
    elif 节点.类型 == "地址":
        生成地址(节点.左)
        return
    elif 节点.类型 == "赋值":
        生成地址(节点.左)
        压入()
        生成表达式(节点.右)

        if 节点.左.类型 == "成员" and 节点.左.成员.是位域:
            打印行("  mov %%rax, %%r8")

            成员 = 节点.左.成员
            打印行("  mov %%rax, %%rdi")
            打印行("  and $%ld, %%rdi" % ((1 << 成员.位宽) - 1))
            打印行("  shl $%d, %%rdi" % 成员.位偏移)

            打印行("  mov (%%rsp), %%rax")
            加载(成员.类型)

            掩码 = ((1 << 成员.位宽) - 1) << 成员.位偏移
            打印行("  mov $%ld, %%r9" % ~掩码)
            打印行("  and %%r9, %%rax")
            打印行("  or %%rdi, %%rax")
            存储(节点.类型)
            打印行("  mov %%r8, %%rax")
            return

        存储(节点.类型)
        return
    elif 节点.类型 == "语句表达式":
        for n in 节点.主体:
            生成语句(n)
        return
    elif 节点.类型 == "逗号":
        生成表达式(节点.左)
        生成表达式(节点.右)
        return
    elif 节点.类型 == "类型转换":
        生成表达式(节点.左)
        类型转换(节点.左.类型, 节点.类型)
        return
    elif 节点.类型 == "内存清零":
        打印行("  mov $%d, %%rcx" % 节点.变量.类型.大小)
        打印行("  lea %d(%%rbp), %%rdi" % 节点.变量.偏移)
        打印行("  mov $0, %%al")
        打印行("  rep stosb")
        return
    elif 节点.类型 == "条件":
        计数 = 计数()
        生成表达式(节点.条件)
        比较零(节点.条件.类型)
        打印行("  je .L.else.%d" % 计数)
        生成表达式(节点.然后)
        打印行("  jmp .L.end.%d" % 计数)
        打印行(".L.else.%d:" % 计数)
        生成表达式(节点.否则)
        打印行(".L.end.%d:" % 计数)
        return
    elif 节点.类型 == "非":
        生成表达式(节点.左)
        比较零(节点.左.类型)
        打印行("  sete %%al")
        打印行("  movzx %%al, %%rax")
        return
    elif 节点.类型 == "位非":
        生成表达式(节点.左)
        打印行("  not %%rax")
        return
    elif 节点.类型 == "逻辑与":
        计数 = 计数()
        生成表达式(节点.左)
        比较零(节点.左.类型)
        打印行("  je .L.false.%d" % 计数)
        生成表达式(节点.右)
        比较零(节点.右.类型)
        打印行("  je .L.false.%d" % 计数)
        打印行("  mov $1, %%rax")
        打印行("  jmp .L.end.%d" % 计数)
        打印行(".L.false.%d:" % 计数)
        打印行("  mov $0, %%rax")
        打印行(".L.end.%d:" % 计数)
        return
    elif 节点.类型 == "逻辑或":
        计数 = 计数()
        生成表达式(节点.左)
        比较零(节点.左.类型)
        打印行("  jne .L.true.%d" % 计数)
        生成表达式(节点.右)
        比较零(节点.右.类型)
        打印行("  jne .L.true.%d" % 计数)
        打印行("  mov $0, %%rax")
        打印行("  jmp .L.end.%d" % 计数)
        打印行(".L.true.%d:" % 计数)
        打印行("  mov $1, %%rax")
        打印行(".L.end.%d:" % 计数)
        return
    elif 节点.类型 == "函数调用":
        if 节点.左.类型 == "变量" and not strcmp(节点.左.变量.名称, "alloca"):
            生成表达式(节点.参数)
            打印行("  mov %%rax, %%rdi")
            内置_alloca()
            return

        栈参数 = 压入参数(节点)
        生成表达式(节点.左)

        通用寄存器 = 0
        浮点寄存器 = 0

        if 节点.返回缓冲区 and 节点.类型.大小 > 16:
            弹栈(参数寄存器64[通用寄存器])
            通用寄存器 += 1

        for 参数 in 节点.参数:
            类型 = 参数.类型

            if 类型.类型 in ["结构体", "联合体"]:
                if 类型.大小 > 16:
                    continue

                浮点数1 = 包含浮点数1(类型)
                浮点数2 = 包含浮点数2(类型)

                if 浮点寄存器 + 浮点数1 + 浮点数2 < FP_MAX and 通用寄存器 + (not 浮点数1) + (not 浮点数2) < GP_MAX:
                    if 浮点数1:
                        弹栈浮点(浮点寄存器)
                        浮点寄存器 += 1
                    else:
                        弹栈(参数寄存器64[通用寄存器])
                        通用寄存器 += 1

                    if 类型.大小 > 8:
                        if 浮点数2:
                            弹栈浮点(浮点寄存器)
                            浮点寄存器 += 1
                        else:
                            弹栈(参数寄存器64[通用寄存器])
                            通用寄存器 += 1
                break
            elif 类型.类型 in ["浮点数", "双精度浮点数"]:
                if 浮点寄存器 < FP_MAX:
                    弹栈浮点(浮点寄存器)
                    浮点寄存器 += 1
                break
            elif 类型.类型 == "长双精度浮点数":
                break
            else:
                if 通用寄存器 < GP_MAX:
                    弹栈(参数寄存器64[通用寄存器])
                    通用寄存器 += 1
                break

        打印行("  mov %%rax, %%r10")
        打印行("  mov $%d, %%rax" % 浮点寄存器)
        打印行("  call *%%r10")
        打印行("  add $%d, %%rsp" % (栈参数 * 8))

        深度 -= 栈参数

        if 节点.类型.类型 == "布尔":
            打印行("  movzx %%al, %%eax")
            return
        elif 节点.类型.类型 == "字符":
            if 节点.类型.是无符号:
                打印行("  movzbl %%al, %%eax")
            else:
                打印行("  movsbl %%al, %%eax")
            return
        elif 节点.类型.类型 == "短整数":
            if 节点.类型.是无符号:
                打印行("  movzwl %%ax, %%eax")
            else:
                打印行("  movswl %%ax, %%eax")
            return

        if 节点.返回缓冲区 and 节点.类型.大小 <= 16:
            复制返回缓冲区(节点.返回缓冲区)
            打印行("  lea %d(%%rbp), %%rax" % 节点.返回缓冲区.偏移)
        return
    elif 节点.类型 == "标签值":
        打印行("  lea %s(%%rip), %%rax" % 节点.唯一标签)
        return
    elif 节点.类型 == "CAS":
        生成表达式(节点.CAS地址)
        压入()
        生成表达式(节点.CAS新值)
        压入()
        生成表达式(节点.CAS旧值)
        打印行("  mov %%rax, %%r8")
        加载(节点.CAS旧值.类型.基类型)
        弹栈("%rdx")  # 新值
        弹栈("%rdi")  # 地址

        大小 = 节点.CAS地址.类型.基类型.大小
        打印行("  lock cmpxchg %s, (%%rdi)" % 寄存器_dx(大小))
        打印行("  sete %%cl")
        打印行("  je 1f")
        打印行("  mov %s, (%%r8)" % 寄存器_ax(大小))
        打印行("1:")
        打印行("  movzbl %%cl, %%eax")
        return
    elif 节点.类型 == "交换":
        生成表达式(节点.左)
        压入()
        生成表达式(节点.右)
        弹栈("%rdi")

        大小 = 节点.左.类型.基类型.大小
        打印行("  xchg %s, (%%rdi)" % 寄存器_ax(大小))
        return

    if 节点.左.类型.类型 in ["浮点数", "双精度浮点数"]:
        生成表达式(节点.右)
        压入浮点()
        生成表达式(节点.左)
        弹栈浮点(1)

        大小 = "ss" if 节点.左.类型.类型 == "浮点数" else "sd"

        if 节点.类型 == "加":
            打印行("  add%s %%xmm1, %%xmm0" % 大小)
            return
        elif 节点.类型 == "减":
            打印行("  sub%s %%xmm1, %%xmm0" % 大小)
            return
        elif 节点.类型 == "乘":
            打印行("  mul%s %%xmm1, %%xmm0" % 大小)
            return
        elif 节点.类型 == "除":
            打印行("  div%s %%xmm1, %%xmm0" % 大小)
            return
        elif 节点.类型 in ["等于", "不等于", "小于", "小于等于"]:
            打印行("  ucomi%s %%xmm0, %%xmm1" % 大小)

            if 节点.类型 == "等于":
                打印行("  sete %%al")
                打印行("  setnp %%dl")
                打印行("  and %%dl, %%al")
            elif 节点.类型 == "不等于":
                打印行("  setne %%al")
                打印行("  setp %%dl")
                打印行("  or %%dl, %%al")
            elif 节点.类型 == "小于":
                打印行("  seta %%al")
            else:
                打印行("  setae %%al")

            打印行("  and $1, %%al")
            打印行("  movzb %%al, %%rax")
            return

        错误_tok(节点.标记, "无效表达式")
    elif 节点.左.类型.类型 == "长双精度浮点数":
        生成表达式(节点.左)
        生成表达式(节点.右)

        if 节点.类型 == "加":
            打印行("  faddp")
            return
        elif 节点.类型 == "减":
            打印行("  fsubrp")
            return
        elif 节点.类型 == "乘":
            打印行("  fmulp")
            return
        elif 节点.类型 == "除":
            打印行("  fdivrp")
            return
        elif 节点.类型 in ["等于", "不等于", "小于", "小于等于"]:
            打印行("  fcomip")
            打印行("  fstp %%st(0)")

            if 节点.类型 == "等于":
                打印行("  sete %%al")
            elif 节点.类型 == "不等于":
                打印行("  setne %%al")
            elif 节点.类型 == "小于":
                打印行("  seta %%al")
            else:
                打印行("  setae %%al")

            打印行("  movzb %%al, %%rax")
            return

        错误_tok(节点.标记, "无效表达式")

    生成表达式(节点.右)
    压入()
    生成表达式(节点.左)
    弹栈("%rdi")

    寄存器_ax, 寄存器_di, 寄存器_dx = ("%rax", "%rdi", "%rdx") if 节点.左.类型.类型 == "长整数" or 节点.左.类型.基类型 else ("%eax", "%edi", "%edx")

    if 节点.类型 == "加":
        打印行("  add %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 == "减":
        打印行("  sub %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 == "乘":
        打印行("  imul %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 == "除" or 节点.类型 == "取模":
        if 节点.类型.是无符号:
            打印行("  mov $0, %s" % 寄存器_dx)
            打印行("  div %s" % 寄存器_di)
        else:
            if 节点.左.类型.大小 == 8:
                打印行("  cqo")
            else:
                打印行("  cdq")
            打印行("  idiv %s" % 寄存器_di)

        if 节点.类型 == "取模":
            打印行("  mov %%rdx, %%rax")
        return
    elif 节点.类型 == "位与":
        打印行("  and %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 == "位或":
        打印行("  or %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 == "位异或":
        打印行("  xor %s, %s" % (寄存器_di, 寄存器_ax))
        return
    elif 节点.类型 in ["等于", "不等于", "小于", "小于等于"]:
        打印行("  cmp %s, %s" % (寄存器_di, 寄存器_ax))

        if 节点.类型 == "等于":
            打印行("  sete %%al")
        elif 节点.类型 == "不等于":
            打印行("  setne %%al")
        elif 节点.类型 == "小于":
            if 节点.左.类型.是无符号:
                打印行("  setb %%al")
            else:
                打印行("  setl %%al")
        elif 节点.类型 == "小于等于":
            if 节点.左.类型.是无符号:
                打印行("  setbe %%al")
            else:
                打印行("  setle %%al")

        打印行("  movzb %%al, %%rax")
        return
    elif 节点.类型 == "左移":
        打印行("  mov %%rdi, %%rcx")
        打印行("  shl %%cl, %s" % 寄存器_ax)
        return
    elif 节点.类型 == "右移":
        打印行("  mov %%rdi, %%rcx")
        if 节点.左.类型.是无符号:
            打印行("  shr %%cl, %s" % 寄存器_ax)
        else:
            打印行("  sar %%cl, %s" % 寄存器_ax)
        return

    错误_tok(节点.标记, "无效表达式")
    
# 生成给定语句的代码
def 生成语句(节点):
    打印行("  .loc %d %d" % (节点.标记.文件.文件编号, 节点.标记.行号))

    if 节点.类型 == "IF":
        计数 = 计数()
        生成表达式(节点.条件)
        比较零(节点.条件.类型)
        打印行("  je  .L.else.%d" % 计数)
        生成语句(节点.然后)
        打印行("  jmp .L.end.%d" % 计数)
        打印行(".L.else.%d:" % 计数)
        if 节点.否则:
            生成语句(节点.否则)
        打印行(".L.end.%d:" % 计数)
        return
    elif 节点.类型 == "FOR":
        计数 = 计数()
        if 节点.初始化:
            生成语句(节点.初始化)
        打印行(".L.begin.%d:" % 计数)
        if 节点.条件:
            生成表达式(节点.条件)
            比较零(节点.条件.类型)
            打印行("  je %s" % 节点.跳出标签)
        生成语句(节点.然后)
        打印行("%s:" % 节点.继续标签)
        if 节点.增量:
            生成表达式(节点.增量)
        打印行("  jmp .L.begin.%d" % 计数)
        打印行("%s:" % 节点.跳出标签)
        return
    elif 节点.类型 == "DO":
        计数 = 计数()
        打印行(".L.begin.%d:" % 计数)
        生成语句(节点.然后)
        打印行("%s:" % 节点.继续标签)
        生成表达式(节点.条件)
        比较零(节点.条件.类型)
        打印行("  jne .L.begin.%d" % 计数)
        打印行("%s:" % 节点.跳出标签)
        return
    elif 节点.类型 == "SWITCH":
        生成表达式(节点.条件)

        for n in 节点.下一个情况:
            寄存器_ax = "%rax" if 节点.条件.类型.大小 == 8 else "%eax"
            寄存器_di = "%rdi" if 节点.条件.类型.大小 == 8 else "%edi"

            if n.开始 == n.结束:
                打印行("  cmp $%ld, %s" % (n.开始, 寄存器_ax))
                打印行("  je %s" % n.标签)
                continue

            # [GNU] 情况范围
            打印行("  mov %s, %s" % (寄存器_ax, 寄存器_di))
            打印行("  sub $%ld, %s" % (n.开始, 寄存器_di))
            打印行("  cmp $%ld, %s" % (n.结束 - n.开始, 寄存器_di))
            打印行("  jbe %s" % n.标签)

        if 节点.默认情况:
            打印行("  jmp %s" % 节点.默认情况.标签)

        打印行("  jmp %s" % 节点.跳出标签)
        生成语句(节点.然后)
        打印行("%s:" % 节点.跳出标签)
        return
    elif 节点.类型 == "CASE":
        打印行("%s:" % 节点.标签)
        生成语句(节点.左)
        return
    elif 节点.类型 == "BLOCK":
        for n in 节点.主体:
            生成语句(n)
        return
    elif 节点.类型 == "GOTO":
        打印行("  jmp %s" % 节点.唯一标签)
        return
    elif 节点.类型 == "GOTO_EXPR":
        生成表达式(节点.左)
        打印行("  jmp *%%rax")
        return
    elif 节点.类型 == "LABEL":
        打印行("%s:" % 节点.唯一标签)
        生成语句(节点.左)
        return
    elif 节点.类型 == "RETURN":
        if 节点.左:
            生成表达式(节点.左)
            类型 = 节点.左.类型

            if 类型.类型 in ["结构体", "联合体"]:
                if 类型.大小 <= 16:
                    复制结构体寄存器()
                else:
                    复制结构体内存()

        打印行("  jmp .L.return.%s" % 当前函数.名称)
        return
    elif 节点.类型 == "表达式语句":
        生成表达式(节点.左)
        return
    elif 节点.类型 == "ASM":
        打印行("  %s" % 节点.汇编字符串)
        return

    错误_tok(节点.标记, "无效语句")
    
# 为局部变量分配偏移量
def 分配局部变量偏移量(程序):
    for 函数 in 程序:
        if not 函数.是函数:
            continue

        # 如果函数有很多参数，一些参数不可避免地会通过栈而不是寄存器传递。
        # 第一个通过栈传递的参数位于RBP+16。
        顶部 = 16
        底部 = 0

        通用寄存器 = 0
        浮点寄存器 = 0

        # 为通过栈传递的参数分配偏移量
        for 变量 in 函数.参数:
            类型 = 变量.类型

            if 类型.类型 in ["结构体", "联合体"]:
                if 类型.大小 <= 16:
                    浮点数1 = 包含浮点数(类型, 0, 8, 0)
                    浮点数2 = 包含浮点数(类型, 8, 16, 8)
                    if 浮点寄存器 + 浮点数1 + 浮点数2 < FP_MAX and 通用寄存器 + (not 浮点数1) + (not 浮点数2) < GP_MAX:
                        浮点寄存器 = 浮点寄存器 + 浮点数1 + 浮点数2
                        通用寄存器 = 通用寄存器 + (not 浮点数1) + (not 浮点数2)
                        continue
                break
            elif 类型.类型 in ["浮点数", "双精度浮点数"]:
                if 浮点寄存器 < FP_MAX:
                    浮点寄存器 += 1
                    continue
                break
            elif 类型.类型 == "长双精度浮点数":
                break
            else:
                if 通用寄存器 < GP_MAX:
                    通用寄存器 += 1
                    continue
                break

            顶部 = 对齐到(顶部, 8)
            变量.偏移 = 顶部
            顶部 += 变量.类型.大小

        # 为通过寄存器传递的参数和局部变量分配偏移量
        for 变量 in 函数.局部变量:
            if 变量.偏移:
                continue

            # AMD64 System V ABI 对长度至少为16字节的数组有特殊的对齐规则。
            # 我们需要将这种数组对齐到至少16字节的边界。
            对齐 = 16 if 变量.类型.类型 == "数组" and 变量.类型.大小 >= 16 else 变量.对齐

            底部 += 变量.类型.大小
            底部 = 对齐到(底部, 对齐)
            变量.偏移 = -底部

        函数.栈大小 = 对齐到(底部, 16)

# 输出数据段
def 输出数据段(程序):
    for 变量 in 程序:
        if 变量.是函数 or not 变量.是定义:
            continue

        if 变量.是静态:
            打印行("  .local %s" % 变量.名称)
        else:
            打印行("  .globl %s" % 变量.名称)

        对齐 = 16 if 变量.类型.类型 == "数组" and 变量.类型.大小 >= 16 else 变量.对齐

        # 公共符号
        if opt_fcommon and 变量.是暂定:
            打印行("  .comm %s, %d, %d" % (变量.名称, 变量.类型.大小, 对齐))
            continue

        # .data 或 .tdata
        if 变量.初始化数据:
            if 变量.是线程局部:
                打印行("  .section .tdata,\"awT\",@progbits")
            else:
                打印行("  .data")

            打印行("  .type %s, @object" % 变量.名称)
            打印行("  .size %s, %d" % (变量.名称, 变量.类型.大小))
            打印行("  .align %d" % 对齐)
            打印行("%s:" % 变量.名称)

            重定位 = 变量.重定位
            位置 = 0
            while 位置 < 变量.类型.大小:
                if 重定位 and 重定位.偏移 == 位置:
                    打印行("  .quad %s%+ld" % (*重定位.标签, 重定位.加数))
                    重定位 = 重定位.下一个
                    位置 += 8
                else:
                    打印行("  .byte %d" % 变量.初始化数据[位置])
                    位置 += 1
            continue

        # .bss 或 .tbss
        if 变量.是线程局部:
            打印行("  .section .tbss,\"awT\",@nobits")
        else:
            打印行("  .bss")

        打印行("  .align %d" % 对齐)
        打印行("%s:" % 变量.名称)
        打印行("  .zero %d" % 变量.类型.大小)
        
# 存储浮点数寄存器
def 存储浮点数寄存器(寄存器, 偏移, 大小):
    if 大小 == 4:
        打印行("  movss %%xmm%d, %d(%%rbp)" % (寄存器, 偏移))
        return
    elif 大小 == 8:
        打印行("  movsd %%xmm%d, %d(%%rbp)" % (寄存器, 偏移))
        return
    不可达()

# 存储通用寄存器
def 存储通用寄存器(寄存器, 偏移, 大小):
    if 大小 == 1:
        打印行("  mov %s, %d(%%rbp)" % (参数寄存器8[寄存器], 偏移))
        return
    elif 大小 == 2:
        打印行("  mov %s, %d(%%rbp)" % (参数寄存器16[寄存器], 偏移))
        return
    elif 大小 == 4:
        打印行("  mov %s, %d(%%rbp)" % (参数寄存器32[寄存器], 偏移))
        return
    elif 大小 == 8:
        打印行("  mov %s, %d(%%rbp)" % (参数寄存器64[寄存器], 偏移))
        return
    else:
        for i in range(大小):
            打印行("  mov %s, %d(%%rbp)" % (参数寄存器8[寄存器], 偏移 + i))
            打印行("  shr $8, %s" % 参数寄存器64[寄存器])
        return

# 输出代码段
def 输出代码段(程序):
    for 函数 in 程序:
        if not 函数.是函数 or not 函数.是定义:
            continue

        # 如果没有引用 "static inline" 函数，则不生成代码。
        if not 函数.是活跃:
            continue

        if 函数.是静态:
            打印行("  .local %s" % 函数.名称)
        else:
            打印行("  .globl %s" % 函数.名称)

        打印行("  .text")
        打印行("  .type %s, @function" % 函数.名称)
        打印行("%s:" % 函数.名称)
        当前函数 = 函数

        # 序言
        打印行("  push %%rbp")
        打印行("  mov %%rsp, %%rbp")
        打印行("  sub $%d, %%rsp" % 函数.栈大小)
        打印行("  mov %%rsp, %d(%%rbp)" % 函数.alloca_bottom.偏移)

        # 如果函数是可变参数，保存参数寄存器
        if 函数.va_area:
            通用寄存器 = 0
            浮点寄存器 = 0
            for 变量 in 函数.参数:
                if 是浮点数(变量.类型):
                    浮点寄存器 += 1
                else:
                    通用寄存器 += 1

            偏移 = 函数.va_area.偏移

            # va_elem
            打印行("  movl $%d, %d(%%rbp)" % (通用寄存器 * 8, 偏移))          # gp_offset
            打印行("  movl $%d, %d(%%rbp)" % (浮点寄存器 * 8 + 48, 偏移 + 4)) # fp_offset
            打印行("  movq %%rbp, %d(%%rbp)" % (偏移 + 8))                   # overflow_arg_area
            打印行("  addq $16, %d(%%rbp)" % (偏移 + 8))
            打印行("  movq %%rbp, %d(%%rbp)" % (偏移 + 16))                  # reg_save_area
            打印行("  addq $%d, %d(%%rbp)" % (偏移 + 24, 偏移 + 16))

            # __reg_save_area__
            打印行("  movq %%rdi, %d(%%rbp)" % (偏移 + 24))
            打印行("  movq %%rsi, %d(%%rbp)" % (偏移 + 32))
            打印行("  movq %%rdx, %d(%%rbp)" % (偏移 + 40))
            打印行("  movq %%rcx, %d(%%rbp)" % (偏移 + 48))
            打印行("  movq %%r8, %d(%%rbp)" % (偏移 + 56))
            打印行("  movq %%r9, %d(%%rbp)" % (偏移 + 64))
            打印行("  movsd %%xmm0, %d(%%rbp)" % (偏移 + 72))
            打印行("  movsd %%xmm1, %d(%%rbp)" % (偏移 + 80))
            打印行("  movsd %%xmm2, %d(%%rbp)" % (偏移 + 88))
            打印行("  movsd %%xmm3, %d(%%rbp)" % (偏移 + 96))
            打印行("  movsd %%xmm4, %d(%%rbp)" % (偏移 + 104))
            打印行("  movsd %%xmm5, %d(%%rbp)" % (偏移 + 112))
            打印行("  movsd %%xmm6, %d(%%rbp)" % (偏移 + 120))
            打印行("  movsd %%xmm7, %d(%%rbp)" % (偏移 + 128))

        # 将通过寄存器传递的参数保存到栈中
        通用寄存器 = 0
        浮点寄存器 = 0
        for 变量 in 函数.参数:
            if 变量.偏移 > 0:
                continue

            类型 = 变量.类型

            if 类型.类型 in ["结构体", "联合体"]:
                assert 类型.大小 <= 16
                if 包含浮点数(类型, 0, 8, 0):
                    存储浮点数寄存器(浮点寄存器, 变量.偏移, min(8, 类型.大小))
                    浮点寄存器 += 1
                else:
                    存储通用寄存器(通用寄存器, 变量.偏移, min(8, 类型.大小))
                    通用寄存器 += 1

                if 类型.大小 > 8:
                    if 包含浮点数(类型, 8, 16, 0):
                        存储浮点数寄存器(浮点寄存器, 变量.偏移 + 8, 类型.大小 - 8)
                        浮点寄存器 += 1
                    else:
                        存储通用寄存器(通用寄存器, 变量.偏移 + 8, 类型.大小 - 8)
                        通用寄存器 += 1
                break
            elif 类型.类型 in ["浮点数", "双精度浮点数"]:
                存储浮点数寄存器(浮点寄存器, 变量.偏移, 类型.大小)
                浮点寄存器 += 1
                break
            else:
                存储通用寄存器(通用寄存器, 变量.偏移, 类型.大小)
                通用寄存器 += 1
                break

        # 生成代码
        生成语句(函数.主体)
        assert 深度 == 0

        # [https://www.sigbus.info/n1570#5.1.2.2.3p1] C 规范为 main 函数定义了一个特殊规则。
        # 即使行为未定义，到达 main 函数的末尾等同于返回 0。
        if strcmp(函数.名称, "main") == 0:
            打印行("  mov $0, %%rax")

        # 尾声
        打印行(".L.return.%s:" % 函数.名称)
        打印行("  mov %%rbp, %%rsp")
        打印行("  pop %%rbp")
        打印行("  ret")
        
# 代码生成
def 代码生成(程序, 输出文件):
    输出文件 = 输出文件

    文件列表 = 获取输入文件()
    for i in range(len(文件列表)):
        文件 = 文件列表[i]
        if 文件:
            打印行("  .file %d \"%s\"" % (文件.文件编号, 文件.名称))

    分配局部变量偏移量(程序)
    输出数据段(程序)
    输出代码段(程序)
    
