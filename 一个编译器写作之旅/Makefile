# Makefile 文件的字符编码为 UTF-8
export LANG := zh_CN.UTF-8

# Makefile 文件中的两个命令，用于编译和清理程序。

编译器n: 主入口.c 扫描.c 表达式.c 解释.c 树.c 代生n.c 生成.c 语句.c 杂注.c
	clang -o 编译器n -g 主入口.c 扫描.c 表达式.c 解释.c 树.c 代生n.c 生成.c 语句.c 杂注.c


# 使用 cc 编译器编译 主入口.c 和 扫描.c 两个源文件，生成可执行文件 扫描器。
编译器1: 主入口.c 扫描.c 表达式.c 解释.c 树.c 代生.c 生成.c
	clang -o 编译器1 -g 主入口.c 扫描.c 表达式.c 解释.c 树.c 代生.c 生成.c

# 解析器2: 主入口.c 扫描.c 表达式2.c 解释.c 树.c
# 	clang -o 解析器2 -g 主入口.c 扫描.c 表达式2.c 解释.c 树.c

# 清理编译生成的程序和中间文件。
clean:
	rm -f 编译器1 编译器n *.o *.s 输出