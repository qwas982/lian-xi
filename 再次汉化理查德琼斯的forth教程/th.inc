; 不可改变的汇编伪指令:  %macro , %endmacro ,
; 还有:  .data ,  .text ,  _start ,

; 使用宏替换为中文标识符.
; 没想到宏替换竟然是可以, 只是不适用于C, 被只懂 C = 编程 的信息茧房人误导N多年.

; 运算器类
%macro 加 2
    add %1, %2
%endmacro

%macro 减 2
    sub %1, %2
%endmacro

%macro 乘 1
    mul %1
%endmacro

%macro 符号乘 1-3
    %if %0 == 1
        imul %1
    %elif %0 == 2
        imul %1, %2
    %elif %0 == 3
        imul %1, %2, %3
    %endif
%endmacro


%macro 除 1
    div %1
%endmacro

%macro 符号除 1
    idiv %1
%endmacro

%macro 递增 1
    inc %1
%endmacro

%macro 递减 1
    dec %1
%endmacro

%macro 与 2
    and %1, %2
%endmacro

%macro 或 2
    or %1, %2
%endmacro

%macro 非 1
    not %1
%endmacro

%macro 异或 2
    xor %1, %2
%endmacro

%macro 取负号 1
    neg %1
%endmacro

%macro 左移 2
    shl %1, %2
%endmacro

%macro 右移 2
    shr %1, %2
%endmacro

; 控制器类
%macro 叫 1
    call %1
%endmacro

%macro 返 0
    ret
%endmacro

%macro 测试 2
    test %1, %2
%endmacro

%macro 对比 2
    cmp %1, %2
%endmacro

%macro 跳 1  ; 无条件跳转
    jmp %1
%endmacro

%macro 零跳 1  ; 等于也是这个
    je %1
%endmacro

%macro 非零跳 1  ; 不等于也是这个
    jne %1
%endmacro

%macro 大于 1
    jg %1
%endmacro

%macro 小于 1
    jl %1
%endmacro

%macro 大等 1
    jge %1
%endmacro

%macro 小等 1
    jle %1
%endmacro

%macro 环 1
    loop %1
%endmacro

%macro 空操作 0
    nop
%endmacro

%macro 停机 0
    hlt
%endmacro

%macro 清除旗 0
    clc
%endmacro

%macro 设置旗 0
    stc
%endmacro

; 存储器类
%macro 移 2
    mov %1, %2
%endmacro

%macro 推 1
    push %1
%endmacro

%macro 弹 1
    pop %1
%endmacro


; 没有输入输出, 被整合进了系统.
; 仅Linux可用.
; 在系统下编程与在裸机下编程不同, 输入输出被垄断.
%macro 系统叫 0
    syscall
%endmacro

; 裸机与内核层可用
%macro 中断 1
    int %1
%endmacro

%macro 中断返 0
    iret
%endmacro

%macro 入 2
    in %1, %2
%endmacro

%macro 出 2
    out %1, %2
%endmacro

%macro 清除中断 0
    cli
%endmacro

%macro 设置中断 0
    sti
%endmacro

; --------------------------------------------------

; nasm伪指令系列
%macro 节 1-2
    %if %0 == 1
        section %1
    %else
        section %1, %2
    %endif
%endmacro

%macro 段 1
    segment %1
%endmacro

%macro 全局 0-*
    %rep %0
        global %1
        %rotate 1
    %endrep
%endmacro

%macro 外部 0-*
    %rep %0
        extern %1
        %rotate 1
    %endrep
%endmacro

%macro 对齐 1
    align %1
%endmacro

%macro 终 0
    end
%endmacro

%macro 字节 0-*
    %rep %0
        db %1
        %rotate 1
    %endrep
%endmacro

%macro 二字节 0-*
    %rep %0
        dw %1
        %rotate 1
    %endrep
%endmacro

%macro 四字节 0-*
    %rep %0
        dd %1
        %rotate 1
    %endrep
%endmacro

%macro 八字节 0-*
    %rep %0
        dq %1
        %rotate 1
    %endrep
%endmacro

%macro 保留字节 1
    resb %1
%endmacro

%macro 保留二字节 1
    resw %1
%endmacro

%macro 保留四字节 1
    resd %1
%endmacro

%macro 保留八字节 1
    resq %1
%endmacro

%macro 等同 2+
    %rep (%0 / 2)
        %1 equ %2
        %rotate 2
    %endrep
%endmacro


%macro 定义 0-*
    %rep %0
        %define %1
        %rotate 1
    %endrep
%endmacro

; ------------------------
; 寄存器
定义 甲 eax
定义 乙 ebx
定义 丙 ecx
定义 丁 edx

定义 栈指针 esp
定义 基指针 ebp

定义 金 esi
定义 木 edi

; -----------------------------
; 这里面给出的指令是图灵完备的吗
; 是的，这个文件中定义的指令集是图灵完备的。
; 图灵完备性是指一个系统或语言能够模拟任何图灵机，
; 即-能够执行任何可计算的算法。
; 汇编语言作为一种底层编程语言，
; 其指令集通常是图灵完备的，只要它包含了以下基本功能：

; 算术运算：如加法、减法、乘法、除法等。

; 逻辑运算：如与、或、非、异或等。

; 数据移动：如将数据从一个位置移动到另一个位置。

; 控制流：如条件跳转、无条件跳转、循环等。

; 内存操作：如读取和写入内存。

; 这个文件中定义的宏涵盖了上述所有基本功能.
; 总结
; 这个文件中定义的宏涵盖了所有必要的指令，能够执行任何可计算的算法。
; 因此，这些指令集是图灵完备的。
; --------------------------------------
; 问了大模型, 看来可以把这套指令当中间码用.