//han_stdarg.h

#ifndef __STDARG_H  // 如果__STDARG_H宏未定义，则编译以下内容
#define __STDARG_H  // 定义__STDARG_H宏，防止重复包含

typedef struct {
  unsigned int gp_offset;  // 通用寄存器偏移量
  unsigned int fp_offset;  // 浮点寄存器偏移量
  void *overflow_arg_area;  // 溢出参数区域指针
  void *reg_save_area;  // 寄存器保存区域指针
} __va_elem;  // 定义__va_elem结构体类型

typedef __va_elem va_list[1];  // 定义va_list为一个包含1个__va_elem元素的数组类型

#define va_start(ap, last) \  // 定义va_start宏用于初始化可变参数列表
  do { *(ap) = *(__va_elem *)__va_area__; } while (0)  // 将__va_area__转换为__va_elem类型，并将其赋值给ap

#define va_end(ap)  // 定义va_end宏，表示可变参数列表的结束

static void *__va_arg_mem(__va_elem *ap, int sz, int align) {  // 定义__va_arg_mem静态函数，用于提取可变参数的内存
  void *p = ap->overflow_arg_area;  // 获取溢出参数区域的指针
  if (align > 8)  // 如果对齐要求大于8
    p = (p + 15) / 16 * 16;  // 将p向上舍入到16的倍数
  ap->overflow_arg_area = ((unsigned long)p + sz + 7) / 8 * 8;  // 更新溢出参数区域指针
  return p;  // 返回参数内存的指针
}

static void *__va_arg_gp(__va_elem *ap, int sz, int align) {  // 定义__va_arg_gp静态函数，用于提取通用寄存器参数
  if (ap->gp_offset >= 48)  // 如果通用寄存器偏移量大于等于48
    return __va_arg_mem(ap, sz, align);  // 调用__va_arg_mem函数提取参数内存

  void *r = ap->reg_save_area + ap->gp_offset;  // 计算参数在寄存器保存区域的地址
  ap->gp_offset += 8;  // 更新通用寄存器偏移量
  return r;  // 返回参数内存的地址
}

static void *__va_arg_fp(__va_elem *ap, int sz, int align) {  // 定义__va_arg_fp静态函数，用于提取浮点寄存器参数
  if (ap->fp_offset >= 112)  // 如果浮点寄存器偏移量大于等于112
    return __va_arg_mem(ap, sz, align);  // 调用__va_arg_mem函数提取参数内存

  void *r = ap->reg_save_area + ap->fp_offset;  // 计算参数在寄存器保存区域的地址
  ap->fp_offset += 8;  // 更新浮点寄存器偏移量
  return r;  // 返回参数内存的地址
}

#define va_arg(ap, ty) \  // 定义va_arg宏，用于提取可变参数的值
  ({ \  // 使用语句块来实现宏的多行定义
    int klass = __builtin_reg_class(ty); \  // 获取类型ty的寄存器类别
    *(ty *)(klass == 0 ? __va_arg_gp(ap, sizeof(ty), _Alignof(ty)) : \  // 如果寄存器类别为0，调用__va_arg_gp提取通用寄存器参数
            klass == 1 ? __va_arg_fp(ap, sizeof(ty), _Alignof(ty)) : \  // 如果寄存器类别为1，调用__va_arg_fp提取浮点寄存器参数
            __va_arg_mem(ap, sizeof(ty), _Alignof(ty))); \  // 否则调用__va_arg_mem提取参数内存
  })  // 将提取到的值转换为ty类型并返回

#define va_copy(dest, src) ((dest)[0] = (src)[0])  // 定义va_copy宏，用于复制可变参数列表

#define __GNUC_VA_LIST 1  // 定义__GNUC_VA_LIST宏为1，表示使用GNU风格的可变参数列表
typedef va_list __gnuc_va_list;  // 定义__gnuc_va_list类型为va_list，即可变参数列表类型

#endif  // 结束条件编译指令，防止重复包含