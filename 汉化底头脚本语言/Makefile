编译器 = clang

编译器旗 = -Wall -g -finput-charset=utf8 -fexec-charset=utf8  # -I xxx头文件目录

目标 = 底头

#目录 = xxx代码文件目录

点C文件们 = $(wildcard *.c)

对象文件们 = $(patsubst %.c, %.o, $(点C文件们))

$(目标):$(对象文件们)
	$(编译器) $(编译器旗) $^ -o $@

清除:
	-$(RM) $(目标) $(对象文件们)

r: clean $(目标)

#clang 代码点到字节序列.c 实用.c 词法.c 编译器.c 对象文件夹.c 核心.c 虚拟机.c 命令行.c -o 底头 -Wall -g