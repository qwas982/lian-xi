//han_stdalign.h

#ifndef __STDALIGN_H  // 如果__STDALIGN_H宏未定义，则编译以下内容
#define __STDALIGN_H  // 定义__STDALIGN_H宏，防止重复包含

#define alignas _Alignas  // 将alignas别名为_Alignas
#define alignof _Alignof  // 将alignof别名为_Alignof
#define __alignas_is_defined 1  // 定义__alignas_is_defined宏为1
#define __alignof_is_defined 1  // 定义__alignof_is_defined宏为1

#endif  // 结束条件编译指令，防止重复包含