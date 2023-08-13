//han_stdbool.h

#ifndef __STDBOOL_H  // 如果未定义__STDBOOL_H宏
#define __STDBOOL_H  // 定义__STDBOOL_H宏

#define bool _Bool  // 将bool定义为_Bool类型
#define true 1      // 将true定义为1
#define false 0     // 将false定义为0
#define __bool_true_false_are_defined 1  // 定义__bool_true_false_are_defined宏为1，表示bool类型的true和false已经定义

#endif  // 结束条件编译指令