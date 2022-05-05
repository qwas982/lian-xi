
# Conditionally enables dtrace user probes
ifneq ($(动态跟踪),)
动跟_旗 = -动失乐园_动态跟踪
动跟_头 = 失乐园_提供者.h
else
动跟_旗 =
动跟_头 =
endif

编译器 = gcc
编译的旗 = -g3 -O0 -Wall -W -DSLY_DEBUG_API $(动跟_旗)
链接器动 = gcc
链接器动旗 =
动态跟踪_编译器 = _动态跟踪

目标 = 失乐园

源 = 应用程序接口.c 垃圾回收.c 入出.c \
 库.c 对象.c 失乐园.c 状态.c 标准.c 虚拟机.c

对象 = $(源:.c=.o)

all: $(目标) fasl2h

$(目标): $(对象)
	$(链接器动) $(对象) $(链接器动旗) -o $(目标)

fasl2h: fasl2h.o
	$(链接器动) fasl2h.o $(链接器动旗) -o fasl2h

失乐园_提供者.h: 失乐园_提供者.d
	$(动态跟踪_编译器) -h -s 失乐园_提供者.d -o 失乐园_提供者.h

%.o: %.c $(动跟_头)
	$(编译器) $(编译的旗) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(目标) fasl2h fasl2h.o $(对象) $(动跟_头) *~ *.fasl

