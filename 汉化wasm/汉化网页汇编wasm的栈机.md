## 一个粗糙的汉化, 网页汇编 web asm, 的栈机.
简单学习了栈虚拟机的结构, 与执行.   
还有网汇wasm的二进制格式, 将字节码翻译为二进制,(这个项目称之为模块    
这种二进制被栈机执行, 而不是被物理机执行, 虽然物理机也执行二进制.   
这相当于提升了一个维度, 这种二进制不再局限于某种物理机的汇编语言或机器语言,   
而是一种跨平台的机器语言.   
wasm它相当于把链接器的部分功能与可执行文件 ``.exe`` ELF这类二进制文件的部分功能整合了.   

此项目仅实现了相当于物理机的CPU与内存. 也就是一个裸机.   
连汇编器都没有, 它只能执行 ``.asm`` 或者说 ``.wasm`` 的虚拟机器码,   
无法识别 ``.wat`` 这种汇编文件, 也就是只能识别已经编译好的二进制.    

具体的指令在 ``操作码.h`` 文件内定义.   
每个指令都指定了相应的整数, 指令序与枚举序并不一致.   

若是编译器欲生成这台虚拟裸机的机器码, 需要了解每个指令的操作者与操作范围如何定义与执行.   

## 版权与许可跟随原项目.
