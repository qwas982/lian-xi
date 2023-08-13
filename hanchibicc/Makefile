#Makefile

CFLAGS=-std=c11 -g -fno-common -Wall -Wno-switch  # 定义CFLAGS变量，赋值为"-std=c11 -g -fno-common -Wall -Wno-switch"

SRCS=$(wildcard *.c)  # 使用通配符获取当前目录下所有的.c文件，并将结果赋值给SRCS变量
OBJS=$(SRCS:.c=.o)  # 将SRCS中的.c后缀替换为.o后缀，并将结果赋值给OBJS变量

TEST_SRCS=$(wildcard test/*.c)  # 使用通配符获取test文件夹下所有的.c文件，并将结果赋值给TEST_SRCS变量
TESTS=$(TEST_SRCS:.c=.exe)  # 将TEST_SRCS中的.c后缀替换为.exe后缀，并将结果赋值给TESTS变量

# Stage 1

chibicc: $(OBJS)  # chibicc目标文件依赖于OBJS变量中的文件
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)  # 使用$(CC)编译链接OBJS中的文件，生成可执行文件chibicc

$(OBJS): chibicc.h  # OBJS中的文件依赖于chibicc.h头文件

test/%.exe: chibicc test/%.c  # test/%.exe目标文件依赖于chibicc和test/%.c文件
	./chibicc -Iinclude -Itest -c -o test/$*.o test/$*.c  # 使用./chibicc编译test/%.c文件，生成目标文件test/$*.o
	$(CC) -pthread -o $@ test/$*.o -xc test/common  # 使用$(CC)链接目标文件test/$*.o和test/common文件，生成可执行文件$@

test: $(TESTS)  # test目标依赖于TESTS变量中的文件
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done  # 对TESTS中的每个文件执行./$$i命令，输出文件名并执行，如果返回值为非零则退出循环
	test/driver.sh ./chibicc  # 执行test/driver.sh脚本，传入参数./chibicc

test-all: test test-stage2  # test-all目标依赖于test和test-stage2目标

# Stage 2

stage2/chibicc: $(OBJS:%=stage2/%)  # stage2/chibicc目标文件依赖于在OBJS中每个文件前添加stage2/路径的文件
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)  # 使用$(CC)编译链接依赖文件，生成可执行文件stage2/chibicc

stage2/%.o: chibicc %.c  # stage2/%.o目标文件依赖于chibicc和同名的.c文件
	mkdir -p stage2/test  # 创建目录stage2/test（如果不存在）
	./chibicc -c -o $(@D)/$*.o $*.c  # 使用./chibicc编译$*.c文件，生成目标文件$(D)/$*.o（$(D)为目标文件所在目录）

stage2/test/%.exe: stage2/chibicc test/%.c  # stage2/test/%.exe目标文件依赖于stage2/chibicc和test/%.c文件
	mkdir -p stage2/test  # 创建目录stage2/test（如果不存在）
	./stage2/chibicc -Iinclude -Itest -c -o stage2/test/$*.o test/$*.c  # 使用./stage2/chibicc编译test/$*.c文件，生成目标文件stage2/test/$*.o
	$(CC) -pthread -o $@ stage2/test/$*.o -xc test/common  # 使用$(CC)链接目标文件stage2/test/$*.o和test/common文件，生成可执行文件$@

test-stage2: $(TESTS:test/%=stage2/test/%)  # test-stage2目标依赖于将TESTS中的每个文件路径替换为stage2/test/后的文件
	for i in $^; do echo $$i; ./$$i || exit 1; echo; done  # 对替换后的每个文件执行./$$i命令，输出文件名并执行，如果返回值为非零则退出循环
	test/driver.sh ./stage2/chibicc  # 执行test/driver.sh脚本，传入参数./stage2/chibicc

# Misc.

clean:
	rm -rf chibicc tmp* $(TESTS) test/*.s test/*.exe stage2  # 删除chibicc、tmp*、$(TESTS)、test/*.s、test/*.exe和stage2文件夹及其内容
	find * -type f '(' -name '*~' -o -name '*.o' ')' -exec rm {} ';'  # 在当前目录及其子目录中查找类型为文件且名称匹配'*~'或'*.o'的文件，并删除

.PHONY: test clean test-stage2  # 声明test、clean和test-stage2为伪目标，即不与文件名冲突

