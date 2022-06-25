#include "chibicc.h"

#define 通用目的_最大 6
#define 浮点_最大 8

static FILE *输出_文件;
static int 深度;
static char *实参寄存8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *实参寄存16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char *实参寄存32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char *实参寄存64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static 对象_短 *当前_函;

static void 生成_表达式_短(节点 *节点_小);
static void 生成_语句_短(节点 *节点_小);

__attribute__((格式化(printf, 1, 2)))
static void println(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(输出_文件, fmt, ap);
  va_end(ap);
  fprintf(输出_文件, "\n");
}

static int 计数_(void) {
  static int i = 1;
  return i++;
}

static void 推(void) {
  println("  push %%rax");
  深度++;
}

static void 弹(char *arg) {
  println("  pop %s", arg);
  深度--;
}

static void 推f(void) {
  println("  sub $8, %%rsp");
  println("  movsd %%xmm0, (%%rsp)");
  深度++;
}

static void 弹f(int reg) {
  println("  movsd (%%rsp), %%xmm%d", reg);
  println("  add $8, %%rsp");
  深度--;
}

// Round up `n` to the nearest multiple of `对齐_短`. For instance,
// 对齐_到(5, 8) returns 8 and 对齐_到(11, 8) returns 16.
int 对齐_到(int n, int 对齐_短) {
  return (n + 对齐_短 - 1) / 对齐_短 * 对齐_短;
}

static char *寄存_dx(int sz) {
  switch (sz) {
  case 1: return "%dl";
  case 2: return "%dx";
  case 4: return "%edx";
  case 8: return "%rdx";
  }
  不可达();
}

static char *寄存_ax(int sz) {
  switch (sz) {
  case 1: return "%al";
  case 2: return "%ax";
  case 4: return "%eax";
  case 8: return "%rax";
  }
  不可达();
}

// Compute the absolute address of a given 节点_小.
// It's an 错误 if a given 节点_小 does not reside in memory.
static void 生成_地址_短(节点 *节点_小) {
  switch (节点_小->种类) {
  case 抽象节点_变量:
    // Variable-length array, which is always local.
    if (节点_小->变量_短->类型_缩->种类 == 类型_变长数组) {
      println("  mov %d(%%rbp), %%rax", 节点_小->变量_短->偏移);
      return;
    }

    // Local variable
    if (节点_小->变量_短->是否_本地) {
      println("  lea %d(%%rbp), %%rax", 节点_小->变量_短->偏移);
      return;
    }

    if (选项_文件采摘) {
      // Thread-local variable
      if (节点_小->变量_短->is_tls) {
        println("  data16 lea %s@tlsgd(%%rip), %%rdi", 节点_小->变量_短->名字);
        println("  .value 0x6666");
        println("  rex64");
        println("  call __tls_get_addr@PLT");
        return;
      }

      // Function or global variable
      println("  mov %s@GOTPCREL(%%rip), %%rax", 节点_小->变量_短->名字);
      return;
    }

    // Thread-local variable
    if (节点_小->变量_短->is_tls) {
      println("  mov %%fs:0, %%rax");
      println("  add $%s@tpoff, %%rax", 节点_小->变量_短->名字);
      return;
    }

    // Here, we generate an absolute address of a 正函数_ or a global
    // variable. Even though they exist at a certain address at runtime,
    // their addresses are not known at link-time for the following
    // two reasons.
    //
    //  - Address randomization: Executables are loaded to memory as a
    //    whole but it is not known what address they are loaded to.
    //    Therefore, at link-time, relative address in the same
    //    exectuable (i.e. the distance between two functions in the
    //    same executable) is known, but the absolute address is not
    //    known.
    //
    //  - Dynamic linking: Dynamic shared objects (DSOs) or .so files
    //    are loaded to memory alongside an executable at runtime and
    //    linked by the runtime loader in memory. We know nothing
    //    about addresses of global stuff that may be defined by DSOs
    //    until the runtime relocation is complete.
    //
    // In order to deal with the former case, we use RIP-relative
    // addressing, denoted by `(%rip)`. For the latter, we obtain an
    // address of a stuff that may be in a shared object 源文件位置 from the
    // Global Offset Table using `@GOTPCREL(%rip)` notation.

    // Function
    if (节点_小->类型_缩->种类 == 类型_函数) {
      if (节点_小->变量_短->是否_定义了)
        println("  lea %s(%%rip), %%rax", 节点_小->变量_短->名字);
      else
        println("  mov %s@GOTPCREL(%%rip), %%rax", 节点_小->变量_短->名字);
      return;
    }

    // Global variable
    println("  lea %s(%%rip), %%rax", 节点_小->变量_短->名字);
    return;
  case 抽象节点_解引用:
    生成_表达式_短(节点_小->左手边);
    return;
  case 抽象节点_逗号:
    生成_表达式_短(节点_小->左手边);
    生成_地址_短(节点_小->右手边);
    return;
  case 抽象节点_成员:
    生成_地址_短(节点_小->左手边);
    println("  add $%d, %%rax", 节点_小->成员_小->偏移);
    return;
  case 抽象节点_函数调用:
    if (节点_小->返回_缓冲区) {
      生成_表达式_短(节点_小);
      return;
    }
    break;
  case 抽象节点_赋值:
  case 抽象节点_条件:
    if (节点_小->类型_缩->种类 == 类型_结构 || 节点_小->类型_缩->种类 == 类型_联合) {
      生成_表达式_短(节点_小);
      return;
    }
    break;
  case 抽象节点_变长数组_指针:
    println("  lea %d(%%rbp), %%rax", 节点_小->变量_短->偏移);
    return;
  }

  错误_牌_短(节点_小->牌_短, "not an lvalue");
}

// Load a value from where %rax is pointing to.
static void 载入_小写(类型 *类型_缩) {
  switch (类型_缩->种类) {
  case 类型_数组:
  case 类型_结构:
  case 类型_联合:
  case 类型_函数:
  case 类型_变长数组:
    // If it is an array, do not attempt to 载入_小写 a value to the
    // register because in general we can't 载入_小写 an entire array to a
    // register. As a result, the result of an evaluation of an array
    // becomes not the array itself but the address of the array.
    // This is where "array is automatically converted to a pointer to
    // the first element of the array in C" occurs.
    return;
  case 类型_浮点:
    println("  movss (%%rax), %%xmm0");
    return;
  case 类型_双浮:
    println("  movsd (%%rax), %%xmm0");
    return;
  case 类型_长双浮:
    println("  fldt (%%rax)");
    return;
  }

  char *insn = 类型_缩->是否_无符号 ? "movz" : "movs";

  // When we 载入_小写 a char or a short value to a register, we always
  // extend them to the 大小 of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we 载入_小写
  // a long value to a register, it simply occupies the entire register.
  if (类型_缩->大小 == 1)
    println("  %sbl (%%rax), %%eax", insn);
  else if (类型_缩->大小 == 2)
    println("  %swl (%%rax), %%eax", insn);
  else if (类型_缩->大小 == 4)
    println("  movsxd (%%rax), %%rax");
  else
    println("  mov (%%rax), %%rax");
}

// Store %rax to an address that the 栈_小写 顶部 is pointing to.
static void 储存_小写(类型 *类型_缩) {
  弹("%rdi");

  switch (类型_缩->种类) {
  case 类型_结构:
  case 类型_联合:
    for (int i = 0; i < 类型_缩->大小; i++) {
      println("  mov %d(%%rax), %%r8b", i);
      println("  mov %%r8b, %d(%%rdi)", i);
    }
    return;
  case 类型_浮点:
    println("  movss %%xmm0, (%%rdi)");
    return;
  case 类型_双浮:
    println("  movsd %%xmm0, (%%rdi)");
    return;
  case 类型_长双浮:
    println("  fstpt (%%rdi)");
    return;
  }

  if (类型_缩->大小 == 1)
    println("  mov %%al, (%%rdi)");
  else if (类型_缩->大小 == 2)
    println("  mov %%ax, (%%rdi)");
  else if (类型_缩->大小 == 4)
    println("  mov %%eax, (%%rdi)");
  else
    println("  mov %%rax, (%%rdi)");
}

static void 对比_零(类型 *类型_缩) {
  switch (类型_缩->种类) {
  case 类型_浮点:
    println("  xorps %%xmm1, %%xmm1");
    println("  ucomiss %%xmm1, %%xmm0");
    return;
  case 类型_双浮:
    println("  xorpd %%xmm1, %%xmm1");
    println("  ucomisd %%xmm1, %%xmm0");
    return;
  case 类型_长双浮:
    println("  fldz");
    println("  fucomip");
    println("  fstp %%st(0)");
    return;
  }

  if (是否_整数_小(类型_缩) && 类型_缩->大小 <= 4)
    println("  cmp $0, %%eax");
  else
    println("  cmp $0, %%rax");
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };

static int 取类型身份(类型 *类型_缩) {
  switch (类型_缩->种类) {
  case 类型_字符:
    return 类型_缩->是否_无符号 ? U8 : I8;
  case 类型_短的:
    return 类型_缩->是否_无符号 ? U16 : I16;
  case 类型_整型:
    return 类型_缩->是否_无符号 ? U32 : I32;
  case 类型_长的:
    return 类型_缩->是否_无符号 ? U64 : I64;
  case 类型_浮点:
    return F32;
  case 类型_双浮:
    return F64;
  case 类型_长双浮:
    return F80;
  }
  return U64;
}

// The table for type casts
static char i32i8[] = "movsbl %al, %eax";
static char i32u8[] = "movzbl %al, %eax";
static char i32i16[] = "movswl %ax, %eax";
static char i32u16[] = "movzwl %ax, %eax";
static char i32f32[] = "cvtsi2ssl %eax, %xmm0";
static char i32i64[] = "movsxd %eax, %rax";
static char i32f64[] = "cvtsi2sdl %eax, %xmm0";
static char i32f80[] = "mov %eax, -4(%rsp); fildl -4(%rsp)";

static char u32f32[] = "mov %eax, %eax; cvtsi2ssq %rax, %xmm0";
static char u32i64[] = "mov %eax, %eax";
static char u32f64[] = "mov %eax, %eax; cvtsi2sdq %rax, %xmm0";
static char u32f80[] = "mov %eax, %eax; mov %rax, -8(%rsp); fildll -8(%rsp)";

static char i64f32[] = "cvtsi2ssq %rax, %xmm0";
static char i64f64[] = "cvtsi2sdq %rax, %xmm0";
static char i64f80[] = "movq %rax, -8(%rsp); fildll -8(%rsp)";

static char u64f32[] = "cvtsi2ssq %rax, %xmm0";
static char u64f64[] =
  "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "
  "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
  "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";
static char u64f80[] =
  "mov %rax, -8(%rsp); fildq -8(%rsp); test %rax, %rax; jns 1f;"
  "mov $1602224128, %eax; mov %eax, -4(%rsp); fadds -4(%rsp); 1:";

static char f32i8[] = "cvttss2sil %xmm0, %eax; movsbl %al, %eax";
static char f32u8[] = "cvttss2sil %xmm0, %eax; movzbl %al, %eax";
static char f32i16[] = "cvttss2sil %xmm0, %eax; movswl %ax, %eax";
static char f32u16[] = "cvttss2sil %xmm0, %eax; movzwl %ax, %eax";
static char f32i32[] = "cvttss2sil %xmm0, %eax";
static char f32u32[] = "cvttss2siq %xmm0, %rax";
static char f32i64[] = "cvttss2siq %xmm0, %rax";
static char f32u64[] = "cvttss2siq %xmm0, %rax";
static char f32f64[] = "cvtss2sd %xmm0, %xmm0";
static char f32f80[] = "movss %xmm0, -4(%rsp); flds -4(%rsp)";

static char f64i8[] = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax";
static char f64u8[] = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax";
static char f64i16[] = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax";
static char f64u16[] = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";
static char f64i32[] = "cvttsd2sil %xmm0, %eax";
static char f64u32[] = "cvttsd2siq %xmm0, %rax";
static char f64i64[] = "cvttsd2siq %xmm0, %rax";
static char f64u64[] = "cvttsd2siq %xmm0, %rax";
static char f64f32[] = "cvtsd2ss %xmm0, %xmm0";
static char f64f80[] = "movsd %xmm0, -8(%rsp); fldl -8(%rsp)";

#define FROM_F80_1                                           \
  "fnstcw -10(%rsp); movzwl -10(%rsp), %eax; or $12, %ah; " \
  "mov %ax, -12(%rsp); fldcw -12(%rsp); "

#define FROM_F80_2 " -24(%rsp); fldcw -10(%rsp); "

static char f80i8[] = FROM_F80_1 "fistps" FROM_F80_2 "movsbl -24(%rsp), %eax";
static char f80u8[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";
static char f80i16[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";
static char f80u16[] = FROM_F80_1 "fistpl" FROM_F80_2 "movswl -24(%rsp), %eax";
static char f80i32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";
static char f80u32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";
static char f80i64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";
static char f80u64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";
static char f80f32[] = "fstps -8(%rsp); movss -8(%rsp), %xmm0";
static char f80f64[] = "fstpl -8(%rsp); movsd -8(%rsp), %xmm0";

static char *类型转换_表[][11] = {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i8
  {i32i8, NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i16
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i32
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   i64f32, i64f64, i64f80}, // i64

  {i32i8, NULL,   NULL,   i32i64, NULL,  NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u8
  {i32i8, i32i16, NULL,   i32i64, i32u8, NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u16
  {i32i8, i32i16, NULL,   u32i64, i32u8, i32u16, NULL,   u32i64, u32f32, u32f64, u32f80}, // u32
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   u64f32, u64f64, u64f80}, // u64

  {f32i8, f32i16, f32i32, f32i64, f32u8, f32u16, f32u32, f32u64, NULL,   f32f64, f32f80}, // f32
  {f64i8, f64i16, f64i32, f64i64, f64u8, f64u16, f64u32, f64u64, f64f32, NULL,   f64f80}, // f64
  {f80i8, f80i16, f80i32, f80i64, f80u8, f80u16, f80u32, f80u64, f80f32, f80f64, NULL},   // f80
};

static void 类型转换_(类型 *from, 类型 *to) {
  if (to->种类 == 类型_空的)
    return;

  if (to->种类 == 类型_布尔) {
    对比_零(from);
    println("  setne %%al");
    println("  movzx %%al, %%eax");
    return;
  }

  int t1 = 取类型身份(from);
  int t2 = 取类型身份(to);
  if (类型转换_表[t1][t2])
    println("  %s", 类型转换_表[t1][t2]);
}

// Structs or unions 等于 or smaller than 16 bytes are passed
// using up to two registers.
//
// If the first 8 bytes contains only floating-point type 成员_复数,
// they are passed in an XMM register. Otherwise, they are passed
// in a general-purpose register.
//
// If a struct/union is larger than 8 bytes, the same rule is
// applied to the the 下一个 8 byte chunk.
//
// This 正函数_ returns true if `类型_缩` has only floating-point
// 成员_复数 in its byte 范围_短 [lo, hi).
static bool 是否有_浮点数(类型 *类型_缩, int lo, int hi, int 偏移) {
  if (类型_缩->种类 == 类型_结构 || 类型_缩->种类 == 类型_联合) {
    for (成员 *成员_短 = 类型_缩->成员_复数; 成员_短; 成员_短 = 成员_短->下一个)
      if (!是否有_浮点数(成员_短->类型_缩, lo, hi, 偏移 + 成员_短->偏移))
        return false;
    return true;
  }

  if (类型_缩->种类 == 类型_数组) {
    for (int i = 0; i < 类型_缩->数组_长度; i++)
      if (!是否有_浮点数(类型_缩->基础, lo, hi, 偏移 + 类型_缩->基础->大小 * i))
        return false;
    return true;
  }

  return 偏移 < lo || hi <= 偏移 || 类型_缩->种类 == 类型_浮点 || 类型_缩->种类 == 类型_双浮;
}

static bool 是否有_浮点数1(类型 *类型_缩) {
  return 是否有_浮点数(类型_缩, 0, 8, 0);
}

static bool 是否有_浮点数2(类型 *类型_缩) {
  return 是否有_浮点数(类型_缩, 8, 16, 0);
}

static void 推_结构(类型 *类型_缩) {
  int sz = 对齐_到(类型_缩->大小, 8);
  println("  sub $%d, %%rsp", sz);
  深度 += sz / 8;

  for (int i = 0; i < 类型_缩->大小; i++) {
    println("  mov %d(%%rax), %%r10b", i);
    println("  mov %%r10b, %d(%%rsp)", i);
  }
}

static void 推_实参2(节点 *实参_短, bool 第一趟) {
  if (!实参_短)
    return;
  推_实参2(实参_短->下一个, 第一趟);

  if ((第一趟 && !实参_短->按栈传递) || (!第一趟 && 实参_短->按栈传递))
    return;

  生成_表达式_短(实参_短);

  switch (实参_短->类型_缩->种类) {
  case 类型_结构:
  case 类型_联合:
    推_结构(实参_短->类型_缩);
    break;
  case 类型_浮点:
  case 类型_双浮:
    推f();
    break;
  case 类型_长双浮:
    println("  sub $16, %%rsp");
    println("  fstpt (%%rsp)");
    深度 += 2;
    break;
  default:
    推();
  }
}

// Load 正函数_ call arguments. Arguments are already evaluated and
// stored to the 栈_小写 as local variables. What we need to do in this
// 正函数_ is to 载入_小写 them to registers or 推 them to the 栈_小写 as
// specified by the x86-64 psABI. Here is what the spec says:
//
// - Up to 6 arguments of integral type are passed using RDI, RSI,
//   RDX, RCX, R8 and R9.
//
// - Up to 8 arguments of floating-point type are passed using XMM0 to
//   XMM7.
//
// - If all registers of an appropriate type are already 已用, 推 an
//   argument to the 栈_小写 in the right-to-left order.
//
// - Each argument passed on the 栈_小写 takes 8 bytes, and the 终 of
//   the argument area must be aligned to a 16 byte boundary.
//
// - If a 正函数_ is variadic, set the number of floating-point type
//   arguments to RAX.
static int 推_实参(节点 *节点_小) {
  int 栈_小写 = 0, 通用目的 = 0, 浮点_ = 0;

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (节点_小->返回_缓冲区 && 节点_小->类型_缩->大小 > 16)
    通用目的++;

  // Load as many arguments to the registers as possible.
  for (节点 *arg = 节点_小->实参_短; arg; arg = arg->下一个) {
    类型 *类型_缩 = arg->类型_缩;

    switch (类型_缩->种类) {
    case 类型_结构:
    case 类型_联合:
      if (类型_缩->大小 > 16) {
        arg->按栈传递 = true;
        栈_小写 += 对齐_到(类型_缩->大小, 8) / 8;
      } else {
        bool fp1 = 是否有_浮点数1(类型_缩);
        bool fp2 = 是否有_浮点数2(类型_缩);

        if (浮点_ + fp1 + fp2 < 浮点_最大 && 通用目的 + !fp1 + !fp2 < 通用目的_最大) {
          浮点_ = 浮点_ + fp1 + fp2;
          通用目的 = 通用目的 + !fp1 + !fp2;
        } else {
          arg->按栈传递 = true;
          栈_小写 += 对齐_到(类型_缩->大小, 8) / 8;
        }
      }
      break;
    case 类型_浮点:
    case 类型_双浮:
      if (浮点_++ >= 浮点_最大) {
        arg->按栈传递 = true;
        栈_小写++;
      }
      break;
    case 类型_长双浮:
      arg->按栈传递 = true;
      栈_小写 += 2;
      break;
    default:
      if (通用目的++ >= 通用目的_最大) {
        arg->按栈传递 = true;
        栈_小写++;
      }
    }
  }

  if ((深度 + 栈_小写) % 2 == 1) {
    println("  sub $8, %%rsp");
    深度++;
    栈_小写++;
  }

  推_实参2(节点_小->实参_短, true);
  推_实参2(节点_小->实参_短, false);

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (节点_小->返回_缓冲区 && 节点_小->类型_缩->大小 > 16) {
    println("  lea %d(%%rbp), %%rax", 节点_小->返回_缓冲区->偏移);
    推();
  }

  return 栈_小写;
}

static void 复制_返回_缓冲区(对象_短 *变量_短) {
  类型 *类型_缩 = 变量_短->类型_缩;
  int 通用目的 = 0, 浮点_ = 0;

  if (是否有_浮点数1(类型_缩)) {
    assert(类型_缩->大小 == 4 || 8 <= 类型_缩->大小);
    if (类型_缩->大小 == 4)
      println("  movss %%xmm0, %d(%%rbp)", 变量_短->偏移);
    else
      println("  movsd %%xmm0, %d(%%rbp)", 变量_短->偏移);
    浮点_++;
  } else {
    for (int i = 0; i < 最小_缩(8, 类型_缩->大小); i++) {
      println("  mov %%al, %d(%%rbp)", 变量_短->偏移 + i);
      println("  shr $8, %%rax");
    }
    通用目的++;
  }

  if (类型_缩->大小 > 8) {
    if (是否有_浮点数2(类型_缩)) {
      assert(类型_缩->大小 == 12 || 类型_缩->大小 == 16);
      if (类型_缩->大小 == 12)
        println("  movss %%xmm%d, %d(%%rbp)", 浮点_, 变量_短->偏移 + 8);
      else
        println("  movsd %%xmm%d, %d(%%rbp)", 浮点_, 变量_短->偏移 + 8);
    } else {
      char *寄存1 = (通用目的 == 0) ? "%al" : "%dl";
      char *寄存2 = (通用目的 == 0) ? "%rax" : "%rdx";
      for (int i = 8; i < 最小_缩(16, 类型_缩->大小); i++) {
        println("  mov %s, %d(%%rbp)", 寄存1, 变量_短->偏移 + i);
        println("  shr $8, %s", 寄存2);
      }
    }
  }
}

static void 复制_结构_寄存(void) {
  类型 *类型_缩 = 当前_函->类型_缩->返回_类型;
  int 通用目的 = 0, 浮点_ = 0;

  println("  mov %%rax, %%rdi");

  if (是否有_浮点数(类型_缩, 0, 8, 0)) {
    assert(类型_缩->大小 == 4 || 8 <= 类型_缩->大小);
    if (类型_缩->大小 == 4)
      println("  movss (%%rdi), %%xmm0");
    else
      println("  movsd (%%rdi), %%xmm0");
    浮点_++;
  } else {
    println("  mov $0, %%rax");
    for (int i = 最小_缩(8, 类型_缩->大小) - 1; i >= 0; i--) {
      println("  shl $8, %%rax");
      println("  mov %d(%%rdi), %%al", i);
    }
    通用目的++;
  }

  if (类型_缩->大小 > 8) {
    if (是否有_浮点数(类型_缩, 8, 16, 0)) {
      assert(类型_缩->大小 == 12 || 类型_缩->大小 == 16);
      if (类型_缩->大小 == 4)
        println("  movss 8(%%rdi), %%xmm%d", 浮点_);
      else
        println("  movsd 8(%%rdi), %%xmm%d", 浮点_);
    } else {
      char *寄存1 = (通用目的 == 0) ? "%al" : "%dl";
      char *寄存2 = (通用目的 == 0) ? "%rax" : "%rdx";
      println("  mov $0, %s", 寄存2);
      for (int i = 最小_缩(16, 类型_缩->大小) - 1; i >= 8; i--) {
        println("  shl $8, %s", 寄存2);
        println("  mov %d(%%rdi), %s", i, 寄存1);
      }
    }
  }
}

static void 复制_结构_成员(void) {
  类型 *类型_缩 = 当前_函->类型_缩->返回_类型;
  对象_短 *变量_短 = 当前_函->形参_短;

  println("  mov %d(%%rbp), %%rdi", 变量_短->偏移);

  for (int i = 0; i < 类型_缩->大小; i++) {
    println("  mov %d(%%rax), %%dl", i);
    println("  mov %%dl, %d(%%rdi)", i);
  }
}

static void 内建_分配a(void) {
  // Align 大小 to 16 bytes.
  println("  add $15, %%rdi");
  println("  and $0xfffffff0, %%edi");

  // Shift the temporary area by %rdi.
  println("  mov %d(%%rbp), %%rcx", 当前_函->动态分配_底部->偏移);
  println("  sub %%rsp, %%rcx");
  println("  mov %%rsp, %%rax");
  println("  sub %%rdi, %%rsp");
  println("  mov %%rsp, %%rdx");
  println("1:");
  println("  cmp $0, %%rcx");
  println("  je 2f");
  println("  mov (%%rax), %%r8b");
  println("  mov %%r8b, (%%rdx)");
  println("  add %%rdx");
  println("  add %%rax");
  println("  dec %%rcx");
  println("  jmp 1b");
  println("2:");

  // Move 动态分配_底部 pointer.
  println("  mov %d(%%rbp), %%rax", 当前_函->动态分配_底部->偏移);
  println("  sub %%rdi, %%rax");
  println("  mov %%rax, %d(%%rbp)", 当前_函->动态分配_底部->偏移);
}

// Generate code for a given 节点_小.
static void 生成_表达式_短(节点 *节点_小) {
  println("  .loc %d %d", 节点_小->牌_短->源文件位置->文件_号, 节点_小->牌_短->行_号);

  switch (节点_小->种类) {
  case 抽象节点_空指针_表达式_短:
    return;
  case 抽象节点_整数: {
    switch (节点_小->类型_缩->种类) {
    case 类型_浮点: {
      union { float f32; uint32_t u32; } u = { 节点_小->浮点值_短 };
      println("  mov $%u, %%eax  # float %Lf", u.u32, 节点_小->浮点值_短);
      println("  movq %%rax, %%xmm0");
      return;
    }
    case 类型_双浮: {
      union { double f64; uint64_t u64; } u = { 节点_小->浮点值_短 };
      println("  mov $%lu, %%rax  # double %Lf", u.u64, 节点_小->浮点值_短);
      println("  movq %%rax, %%xmm0");
      return;
    }
    case 类型_长双浮: {
      union { long double f80; uint64_t u64[2]; } u;
      memset(&u, 0, sizeof(u));
      u.f80 = 节点_小->浮点值_短;
      println("  mov $%lu, %%rax  # long double %Lf", u.u64[0], 节点_小->浮点值_短);
      println("  mov %%rax, -16(%%rsp)");
      println("  mov $%lu, %%rax", u.u64[1]);
      println("  mov %%rax, -8(%%rsp)");
      println("  fldt -16(%%rsp)");
      return;
    }
    }

    println("  mov $%ld, %%rax", 节点_小->值_短);
    return;
  }
  case 抽象节点_负:
    生成_表达式_短(节点_小->左手边);

    switch (节点_小->类型_缩->种类) {
    case 类型_浮点:
      println("  mov $1, %%rax");
      println("  shl $31, %%rax");
      println("  movq %%rax, %%xmm1");
      println("  xorps %%xmm1, %%xmm0");
      return;
    case 类型_双浮:
      println("  mov $1, %%rax");
      println("  shl $63, %%rax");
      println("  movq %%rax, %%xmm1");
      println("  xorpd %%xmm1, %%xmm0");
      return;
    case 类型_长双浮:
      println("  fchs");
      return;
    }

    println("  neg %%rax");
    return;
  case 抽象节点_变量:
    生成_地址_短(节点_小);
    载入_小写(节点_小->类型_缩);
    return;
  case 抽象节点_成员: {
    生成_地址_短(节点_小);
    载入_小写(节点_小->类型_缩);

    成员 *成员_短 = 节点_小->成员_小;
    if (成员_短->是否_位字段) {
      println("  shl $%d, %%rax", 64 - 成员_短->位_宽度 - 成员_短->位_偏移);
      if (成员_短->类型_缩->是否_无符号)
        println("  shr $%d, %%rax", 64 - 成员_短->位_宽度);
      else
        println("  sar $%d, %%rax", 64 - 成员_短->位_宽度);
    }
    return;
  }
  case 抽象节点_解引用:
    生成_表达式_短(节点_小->左手边);
    载入_小写(节点_小->类型_缩);
    return;
  case 抽象节点_地址:
    生成_地址_短(节点_小->左手边);
    return;
  case 抽象节点_赋值:
    生成_地址_短(节点_小->左手边);
    推();
    生成_表达式_短(节点_小->右手边);

    if (节点_小->左手边->种类 == 抽象节点_成员 && 节点_小->左手边->成员_小->是否_位字段) {
      println("  mov %%rax, %%r8");

      // If the 左手边 is a bitfield, we need to read the current value
      // from memory and merge it with a 新的 value.
      成员 *成员_短 = 节点_小->左手边->成员_小;
      println("  mov %%rax, %%rdi");
      println("  and $%ld, %%rdi", (1L << 成员_短->位_宽度) - 1);
      println("  shl $%d, %%rdi", 成员_短->位_偏移);

      println("  mov (%%rsp), %%rax");
      载入_小写(成员_短->类型_缩);

      long mask = ((1L << 成员_短->位_宽度) - 1) << 成员_短->位_偏移;
      println("  mov $%ld, %%r9", ~mask);
      println("  and %%r9, %%rax");
      println("  or %%rdi, %%rax");
      储存_小写(节点_小->类型_缩);
      println("  mov %%r8, %%rax");
      return;
    }

    储存_小写(节点_小->类型_缩);
    return;
  case 抽象节点_语句_表达式:
    for (节点 *n = 节点_小->体; n; n = n->下一个)
      生成_语句_短(n);
    return;
  case 抽象节点_逗号:
    生成_表达式_短(节点_小->左手边);
    生成_表达式_短(节点_小->右手边);
    return;
  case 抽象节点_类型转换:
    生成_表达式_短(节点_小->左手边);
    类型转换_(节点_小->左手边->类型_缩, 节点_小->类型_缩);
    return;
  case 抽象节点_内存清零:
    // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
    println("  mov $%d, %%rcx", 节点_小->变量_短->类型_缩->大小);
    println("  lea %d(%%rbp), %%rdi", 节点_小->变量_短->偏移);
    println("  mov $0, %%al");
    println("  rep stosb");
    return;
  case 抽象节点_条件: {
    int c = 计数_();
    生成_表达式_短(节点_小->条件_短);
    对比_零(节点_小->条件_短->类型_缩);
    println("  je .L.else.%d", c);
    生成_表达式_短(节点_小->那么);
    println("  jmp .L.end.%d", c);
    println(".L.else.%d:", c);
    生成_表达式_短(节点_小->否则);
    println(".L.end.%d:", c);
    return;
  }
  case 抽象节点_非:
    生成_表达式_短(节点_小->左手边);
    对比_零(节点_小->左手边->类型_缩);
    println("  sete %%al");
    println("  movzx %%al, %%rax");
    return;
  case 抽象节点_位非:
    生成_表达式_短(节点_小->左手边);
    println("  not %%rax");
    return;
  case 抽象节点_逻辑与: {
    int c = 计数_();
    生成_表达式_短(节点_小->左手边);
    对比_零(节点_小->左手边->类型_缩);
    println("  je .L.false.%d", c);
    生成_表达式_短(节点_小->右手边);
    对比_零(节点_小->右手边->类型_缩);
    println("  je .L.false.%d", c);
    println("  mov $1, %%rax");
    println("  jmp .L.end.%d", c);
    println(".L.false.%d:", c);
    println("  mov $0, %%rax");
    println(".L.end.%d:", c);
    return;
  }
  case 抽象节点_逻辑或: {
    int c = 计数_();
    生成_表达式_短(节点_小->左手边);
    对比_零(节点_小->左手边->类型_缩);
    println("  jne .L.true.%d", c);
    生成_表达式_短(节点_小->右手边);
    对比_零(节点_小->右手边->类型_缩);
    println("  jne .L.true.%d", c);
    println("  mov $0, %%rax");
    println("  jmp .L.end.%d", c);
    println(".L.true.%d:", c);
    println("  mov $1, %%rax");
    println(".L.end.%d:", c);
    return;
  }
  case 抽象节点_函数调用: {
    if (节点_小->左手边->种类 == 抽象节点_变量 && !strcmp(节点_小->左手边->变量_短->名字, "alloca")) {
      生成_表达式_短(节点_小->实参_短);
      println("  mov %%rax, %%rdi");
      内建_分配a();
      return;
    }

    int 栈_实参 = 推_实参(节点_小);
    生成_表达式_短(节点_小->左手边);

    int 通用目的 = 0, 浮点_ = 0;

    // If the return type is a large struct/union, the caller passes
    // a pointer to a buffer as if it were the first argument.
    if (节点_小->返回_缓冲区 && 节点_小->类型_缩->大小 > 16)
      弹(实参寄存64[通用目的++]);

    for (节点 *arg = 节点_小->实参_短; arg; arg = arg->下一个) {
      类型 *类型_缩 = arg->类型_缩;

      switch (类型_缩->种类) {
      case 类型_结构:
      case 类型_联合:
        if (类型_缩->大小 > 16)
          continue;

        bool fp1 = 是否有_浮点数1(类型_缩);
        bool fp2 = 是否有_浮点数2(类型_缩);

        if (浮点_ + fp1 + fp2 < 浮点_最大 && 通用目的 + !fp1 + !fp2 < 通用目的_最大) {
          if (fp1)
            弹f(浮点_++);
          else
            弹(实参寄存64[通用目的++]);

          if (类型_缩->大小 > 8) {
            if (fp2)
              弹f(浮点_++);
            else
              弹(实参寄存64[通用目的++]);
          }
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        if (浮点_ < 浮点_最大)
          弹f(浮点_++);
        break;
      case 类型_长双浮:
        break;
      default:
        if (通用目的 < 通用目的_最大)
          弹(实参寄存64[通用目的++]);
      }
    }

    println("  mov %%rax, %%r10");
    println("  mov $%d, %%rax", 浮点_);
    println("  call *%%r10");
    println("  add $%d, %%rsp", 栈_实参 * 8);

    深度 -= 栈_实参;

    // It looks like the most significant 48 or 56 bits in RAX may
    // contain garbage if a 正函数_ return type is short or bool/char,
    // respectively. We clear the upper bits here.
    switch (节点_小->类型_缩->种类) {
    case 类型_布尔:
      println("  movzx %%al, %%eax");
      return;
    case 类型_字符:
      if (节点_小->类型_缩->是否_无符号)
        println("  movzbl %%al, %%eax");
      else
        println("  movsbl %%al, %%eax");
      return;
    case 类型_短的:
      if (节点_小->类型_缩->是否_无符号)
        println("  movzwl %%ax, %%eax");
      else
        println("  movswl %%ax, %%eax");
      return;
    }

    // If the return type is a small struct, a value is returned
    // using up to two registers.
    if (节点_小->返回_缓冲区 && 节点_小->类型_缩->大小 <= 16) {
      复制_返回_缓冲区(节点_小->返回_缓冲区);
      println("  lea %d(%%rbp), %%rax", 节点_小->返回_缓冲区->偏移);
    }

    return;
  }
  case 抽象节点_标号_值:
    println("  lea %s(%%rip), %%rax", 节点_小->独特性_标号);
    return;
  case 抽象节点_对比与交换: {
    生成_表达式_短(节点_小->对比与交换_地址);
    推();
    生成_表达式_短(节点_小->对比与交换_新);
    推();
    生成_表达式_短(节点_小->对比与交换_旧);
    println("  mov %%rax, %%r8");
    载入_小写(节点_小->对比与交换_旧->类型_缩->基础);
    弹("%rdx"); // 新的
    弹("%rdi"); // 地址_短的

    int sz = 节点_小->对比与交换_地址->类型_缩->基础->大小;
    println("  lock cmpxchg %s, (%%rdi)", 寄存_dx(sz));
    println("  sete %%cl");
    println("  je 1f");
    println("  mov %s, (%%r8)", 寄存_ax(sz));
    println("1:");
    println("  movzbl %%cl, %%eax");
    return;
  }
  case 抽象节点_互换: {
    生成_表达式_短(节点_小->左手边);
    推();
    生成_表达式_短(节点_小->右手边);
    弹("%rdi");

    int sz = 节点_小->左手边->类型_缩->基础->大小;
    println("  xchg %s, (%%rdi)", 寄存_ax(sz));
    return;
  }
  }

  switch (节点_小->左手边->类型_缩->种类) {
  case 类型_浮点:
  case 类型_双浮: {
    生成_表达式_短(节点_小->右手边);
    推f();
    生成_表达式_短(节点_小->左手边);
    弹f(1);

    char *sz = (节点_小->左手边->类型_缩->种类 == 类型_浮点) ? "ss" : "sd";

    switch (节点_小->种类) {
    case 抽象节点_加:
      println("  add%s %%xmm1, %%xmm0", sz);
      return;
    case 抽象节点_减:
      println("  sub%s %%xmm1, %%xmm0", sz);
      return;
    case 抽象节点_乘:
      println("  mul%s %%xmm1, %%xmm0", sz);
      return;
    case 抽象节点_除:
      println("  div%s %%xmm1, %%xmm0", sz);
      return;
    case 抽象节点_等于:
    case 抽象节点_不等:
    case 抽象节点_小于:
    case 抽象节点_小等:
      println("  ucomi%s %%xmm0, %%xmm1", sz);

      if (节点_小->种类 == 抽象节点_等于) {
        println("  sete %%al");
        println("  setnp %%dl");
        println("  and %%dl, %%al");
      } else if (节点_小->种类 == 抽象节点_不等) {
        println("  setne %%al");
        println("  setp %%dl");
        println("  or %%dl, %%al");
      } else if (节点_小->种类 == 抽象节点_小于) {
        println("  seta %%al");
      } else {
        println("  setae %%al");
      }

      println("  and $1, %%al");
      println("  movzb %%al, %%rax");
      return;
    }

    错误_牌_短(节点_小->牌_短, "invalid expression");
  }
  case 类型_长双浮: {
    生成_表达式_短(节点_小->左手边);
    生成_表达式_短(节点_小->右手边);

    switch (节点_小->种类) {
    case 抽象节点_加:
      println("  faddp");
      return;
    case 抽象节点_减:
      println("  fsubrp");
      return;
    case 抽象节点_乘:
      println("  fmulp");
      return;
    case 抽象节点_除:
      println("  fdivrp");
      return;
    case 抽象节点_等于:
    case 抽象节点_不等:
    case 抽象节点_小于:
    case 抽象节点_小等:
      println("  fcomip");
      println("  fstp %%st(0)");

      if (节点_小->种类 == 抽象节点_等于)
        println("  sete %%al");
      else if (节点_小->种类 == 抽象节点_不等)
        println("  setne %%al");
      else if (节点_小->种类 == 抽象节点_小于)
        println("  seta %%al");
      else
        println("  setae %%al");

      println("  movzb %%al, %%rax");
      return;
    }

    错误_牌_短(节点_小->牌_短, "invalid expression");
  }
  }

  生成_表达式_短(节点_小->右手边);
  推();
  生成_表达式_短(节点_小->左手边);
  弹("%rdi");

  char *ax, *di, *dx;

  if (节点_小->左手边->类型_缩->种类 == 类型_长的 || 节点_小->左手边->类型_缩->基础) {
    ax = "%rax";
    di = "%rdi";
    dx = "%rdx";
  } else {
    ax = "%eax";
    di = "%edi";
    dx = "%edx";
  }

  switch (节点_小->种类) {
  case 抽象节点_加:
    println("  add %s, %s", di, ax);
    return;
  case 抽象节点_减:
    println("  sub %s, %s", di, ax);
    return;
  case 抽象节点_乘:
    println("  imul %s, %s", di, ax);
    return;
  case 抽象节点_除:
  case 抽象节点_余数:
    if (节点_小->类型_缩->是否_无符号) {
      println("  mov $0, %s", dx);
      println("  div %s", di);
    } else {
      if (节点_小->左手边->类型_缩->大小 == 8)
        println("  cqo");
      else
        println("  cdq");
      println("  idiv %s", di);
    }

    if (节点_小->种类 == 抽象节点_余数)
      println("  mov %%rdx, %%rax");
    return;
  case 抽象节点_位与:
    println("  and %s, %s", di, ax);
    return;
  case 抽象节点_位或:
    println("  or %s, %s", di, ax);
    return;
  case 抽象节点_位异或:
    println("  xor %s, %s", di, ax);
    return;
  case 抽象节点_等于:
  case 抽象节点_不等:
  case 抽象节点_小于:
  case 抽象节点_小等:
    println("  cmp %s, %s", di, ax);

    if (节点_小->种类 == 抽象节点_等于) {
      println("  sete %%al");
    } else if (节点_小->种类 == 抽象节点_不等) {
      println("  setne %%al");
    } else if (节点_小->种类 == 抽象节点_小于) {
      if (节点_小->左手边->类型_缩->是否_无符号)
        println("  setb %%al");
      else
        println("  setl %%al");
    } else if (节点_小->种类 == 抽象节点_小等) {
      if (节点_小->左手边->类型_缩->是否_无符号)
        println("  setbe %%al");
      else
        println("  setle %%al");
    }

    println("  movzb %%al, %%rax");
    return;
  case 抽象节点_左移:
    println("  mov %%rdi, %%rcx");
    println("  shl %%cl, %s", ax);
    return;
  case 抽象节点_右移:
    println("  mov %%rdi, %%rcx");
    if (节点_小->左手边->类型_缩->是否_无符号)
      println("  shr %%cl, %s", ax);
    else
      println("  sar %%cl, %s", ax);
    return;
  }

  错误_牌_短(节点_小->牌_短, "invalid expression");
}

static void 生成_语句_短(节点 *节点_小) {
  println("  .loc %d %d", 节点_小->牌_短->源文件位置->文件_号, 节点_小->牌_短->行_号);

  switch (节点_小->种类) {
  case 抽象节点_若: {
    int c = 计数_();
    生成_表达式_短(节点_小->条件_短);
    对比_零(节点_小->条件_短->类型_缩);
    println("  je  .L.else.%d", c);
    生成_语句_短(节点_小->那么);
    println("  jmp .L.end.%d", c);
    println(".L.else.%d:", c);
    if (节点_小->否则)
      生成_语句_短(节点_小->否则);
    println(".L.end.%d:", c);
    return;
  }
  case 抽象节点_为: {
    int c = 计数_();
    if (节点_小->初始)
      生成_语句_短(节点_小->初始);
    println(".L.begin.%d:", c);
    if (节点_小->条件_短) {
      生成_表达式_短(节点_小->条件_短);
      对比_零(节点_小->条件_短->类型_缩);
      println("  je %s", 节点_小->断_标号);
    }
    生成_语句_短(节点_小->那么);
    println("%s:", 节点_小->继续_标号);
    if (节点_小->增_短)
      生成_表达式_短(节点_小->增_短);
    println("  jmp .L.begin.%d", c);
    println("%s:", 节点_小->断_标号);
    return;
  }
  case 抽象节点_做: {
    int c = 计数_();
    println(".L.begin.%d:", c);
    生成_语句_短(节点_小->那么);
    println("%s:", 节点_小->继续_标号);
    生成_表达式_短(节点_小->条件_短);
    对比_零(节点_小->条件_短->类型_缩);
    println("  jne .L.begin.%d", c);
    println("%s:", 节点_小->断_标号);
    return;
  }
  case 抽象节点_切换:
    生成_表达式_短(节点_小->条件_短);

    for (节点 *n = 节点_小->情况_下一个; n; n = n->情况_下一个) {
      char *ax = (节点_小->条件_短->类型_缩->大小 == 8) ? "%rax" : "%eax";
      char *di = (节点_小->条件_短->类型_缩->大小 == 8) ? "%rdi" : "%edi";

      if (n->开始 == n->终) {
        println("  cmp $%ld, %s", n->开始, ax);
        println("  je %s", n->标号);
        continue;
      }

      // [GNU] Case ranges
      println("  mov %s, %s", ax, di);
      println("  sub $%ld, %s", n->开始, di);
      println("  cmp $%ld, %s", n->终 - n->开始, di);
      println("  jbe %s", n->标号);
    }

    if (节点_小->默认_情况)
      println("  jmp %s", 节点_小->默认_情况->标号);

    println("  jmp %s", 节点_小->断_标号);
    生成_语句_短(节点_小->那么);
    println("%s:", 节点_小->断_标号);
    return;
  case 抽象节点_情况:
    println("%s:", 节点_小->标号);
    生成_语句_短(节点_小->左手边);
    return;
  case 抽象节点_块:
    for (节点 *n = 节点_小->体; n; n = n->下一个)
      生成_语句_短(n);
    return;
  case 抽象节点_跳转:
    println("  jmp %s", 节点_小->独特性_标号);
    return;
  case 抽象节点_跳转_表达式:
    生成_表达式_短(节点_小->左手边);
    println("  jmp *%%rax");
    return;
  case 抽象节点_标号:
    println("%s:", 节点_小->独特性_标号);
    生成_语句_短(节点_小->左手边);
    return;
  case 抽象节点_返回:
    if (节点_小->左手边) {
      生成_表达式_短(节点_小->左手边);
      类型 *类型_缩 = 节点_小->左手边->类型_缩;

      switch (类型_缩->种类) {
      case 类型_结构:
      case 类型_联合:
        if (类型_缩->大小 <= 16)
          复制_结构_寄存();
        else
          复制_结构_成员();
        break;
      }
    }

    println("  jmp .L.return.%s", 当前_函->名字);
    return;
  case 抽象节点_表达式_语句:
    生成_表达式_短(节点_小->左手边);
    return;
  case 抽象节点_汇编:
    println("  %s", 节点_小->汇编_字符串);
    return;
  }

  错误_牌_短(节点_小->牌_短, "invalid statement");
}

// Assign offsets to local variables.
static void 赋值_本地变量_偏移(对象_短 *程序_短) {
  for (对象_短 *fn = 程序_短; fn; fn = fn->下一个) {
    if (!fn->是否_函数)
      continue;

    // If a 正函数_ has many parameters, some parameters are
    // inevitably passed by 栈_小写 rather than by register.
    // The first passed-by-栈_小写 parameter resides at RBP+16.
    int 顶部 = 16;
    int 底部 = 0;

    int 通用目的 = 0, 浮点_ = 0;

    // Assign offsets to pass-by-栈_小写 parameters.
    for (对象_短 *变量_短 = fn->形参_短; 变量_短; 变量_短 = 变量_短->下一个) {
      类型 *类型_缩 = 变量_短->类型_缩;

      switch (类型_缩->种类) {
      case 类型_结构:
      case 类型_联合:
        if (类型_缩->大小 <= 16) {
          bool fp1 = 是否有_浮点数(类型_缩, 0, 8, 0);
          bool fp2 = 是否有_浮点数(类型_缩, 8, 16, 8);
          if (浮点_ + fp1 + fp2 < 浮点_最大 && 通用目的 + !fp1 + !fp2 < 通用目的_最大) {
            浮点_ = 浮点_ + fp1 + fp2;
            通用目的 = 通用目的 + !fp1 + !fp2;
            continue;
          }
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        if (浮点_++ < 浮点_最大)
          continue;
        break;
      case 类型_长双浮:
        break;
      default:
        if (通用目的++ < 通用目的_最大)
          continue;
      }

      顶部 = 对齐_到(顶部, 8);
      变量_短->偏移 = 顶部;
      顶部 += 变量_短->类型_缩->大小;
    }

    // Assign offsets to pass-by-register parameters and local variables.
    for (对象_短 *变量_短 = fn->本地; 变量_短; 变量_短 = 变量_短->下一个) {
      if (变量_短->偏移)
        continue;

      // AMD64 System V ABI has a special alignment rule for an array of
      // length at least 16 bytes. We need to 对齐_短 such array to at least
      // 16-byte boundaries. See p.14 of
      // https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-draft.pdf.
      int 对齐_短 = (变量_短->类型_缩->种类 == 类型_数组 && 变量_短->类型_缩->大小 >= 16)
        ? 最大_缩(16, 变量_短->对齐_短) : 变量_短->对齐_短;

      底部 += 变量_短->类型_缩->大小;
      底部 = 对齐_到(底部, 对齐_短);
      变量_短->偏移 = -底部;
    }

    fn->栈_大小 = 对齐_到(底部, 16);
  }
}

static void 发射_数据(对象_短 *程序_短) {
  for (对象_短 *变量_短 = 程序_短; 变量_短; 变量_短 = 变量_短->下一个) {
    if (变量_短->是否_函数 || !变量_短->是否_定义了)
      continue;

    if (变量_短->是否_静态)
      println("  .local %s", 变量_短->名字);
    else
      println("  .globl %s", 变量_短->名字);

    int 对齐_短 = (变量_短->类型_缩->种类 == 类型_数组 && 变量_短->类型_缩->大小 >= 16)
      ? 最大_缩(16, 变量_短->对齐_短) : 变量_短->对齐_短;

    // Common symbol
    if (选项_文件公共的 && 变量_短->是否_试探性的) {
      println("  .comm %s, %d, %d", 变量_短->名字, 变量_短->类型_缩->大小, 对齐_短);
      continue;
    }

    // .数据 or .tdata
    if (变量_短->初始_数据) {
      if (变量_短->is_tls)
        println("  .section .tdata,\"awT\",@progbits");
      else
        println("  .data");

      println("  .type %s, @object", 变量_短->名字);
      println("  .size %s, %d", 变量_短->名字, 变量_短->类型_缩->大小);
      println("  .align %d", 对齐_短);
      println("%s:", 变量_短->名字);

      重定位 *重定位_短 = 变量_短->重定位_短;
      int 地点位 = 0;
      while (地点位 < 变量_短->类型_缩->大小) {
        if (重定位_短 && 重定位_短->偏移 == 地点位) {
          println("  .quad %s%+ld", *重定位_短->标号, 重定位_短->附加物);
          重定位_短 = 重定位_短->下一个;
          地点位 += 8;
        } else {
          println("  .byte %d", 变量_短->初始_数据[地点位++]);
        }
      }
      continue;
    }

    // .bss or .tbss
    if (变量_短->is_tls)
      println("  .section .tbss,\"awT\",@nobits");
    else
      println("  .bss");

    println("  .align %d", 对齐_短);
    println("%s:", 变量_短->名字);
    println("  .zero %d", 变量_短->类型_缩->大小);
  }
}

static void 储存_浮点(int r, int 偏移, int sz) {
  switch (sz) {
  case 4:
    println("  movss %%xmm%d, %d(%%rbp)", r, 偏移);
    return;
  case 8:
    println("  movsd %%xmm%d, %d(%%rbp)", r, 偏移);
    return;
  }
  不可达();
}

static void 储存_通用目的(int r, int 偏移, int sz) {
  switch (sz) {
  case 1:
    println("  mov %s, %d(%%rbp)", 实参寄存8[r], 偏移);
    return;
  case 2:
    println("  mov %s, %d(%%rbp)", 实参寄存16[r], 偏移);
    return;
  case 4:
    println("  mov %s, %d(%%rbp)", 实参寄存32[r], 偏移);
    return;
  case 8:
    println("  mov %s, %d(%%rbp)", 实参寄存64[r], 偏移);
    return;
  default:
    for (int i = 0; i < sz; i++) {
      println("  mov %s, %d(%%rbp)", 实参寄存8[r], 偏移 + i);
      println("  shr $8, %s", 实参寄存64[r]);
    }
    return;
  }
}

static void 发射_文本(对象_短 *程序_短) {
  for (对象_短 *fn = 程序_短; fn; fn = fn->下一个) {
    if (!fn->是否_函数 || !fn->是否_定义了)
      continue;

    // No code is emitted for "static inline" functions
    // if no one is referencing them.
    if (!fn->是否_活着)
      continue;

    if (fn->是否_静态)
      println("  .local %s", fn->名字);
    else
      println("  .globl %s", fn->名字);

    println("  .text");
    println("  .type %s, @function", fn->名字);
    println("%s:", fn->名字);
    当前_函 = fn;

    // Prologue
    println("  push %%rbp");
    println("  mov %%rsp, %%rbp");
    println("  sub $%d, %%rsp", fn->栈_大小);
    println("  mov %%rsp, %d(%%rbp)", fn->动态分配_底部->偏移);

    // Save arg registers if 正函数_ is variadic
    if (fn->va_区域) {
      int 通用目的 = 0, 浮点_ = 0;
      for (对象_短 *变量_短 = fn->形参_短; 变量_短; 变量_短 = 变量_短->下一个) {
        if (是否_浮点数_小(变量_短->类型_缩))
          浮点_++;
        else
          通用目的++;
      }

      int off = fn->va_区域->偏移;

      // va_elem
      println("  movl $%d, %d(%%rbp)", 通用目的 * 8, off);          // gp_offset
      println("  movl $%d, %d(%%rbp)", 浮点_ * 8 + 48, off + 4); // fp_offset
      println("  movq %%rbp, %d(%%rbp)", off + 8);            // overflow_arg_area
      println("  addq $16, %d(%%rbp)", off + 8);
      println("  movq %%rbp, %d(%%rbp)", off + 16);           // reg_save_area
      println("  addq $%d, %d(%%rbp)", off + 24, off + 16);

      // __reg_save_area__
      println("  movq %%rdi, %d(%%rbp)", off + 24);
      println("  movq %%rsi, %d(%%rbp)", off + 32);
      println("  movq %%rdx, %d(%%rbp)", off + 40);
      println("  movq %%rcx, %d(%%rbp)", off + 48);
      println("  movq %%r8, %d(%%rbp)", off + 56);
      println("  movq %%r9, %d(%%rbp)", off + 64);
      println("  movsd %%xmm0, %d(%%rbp)", off + 72);
      println("  movsd %%xmm1, %d(%%rbp)", off + 80);
      println("  movsd %%xmm2, %d(%%rbp)", off + 88);
      println("  movsd %%xmm3, %d(%%rbp)", off + 96);
      println("  movsd %%xmm4, %d(%%rbp)", off + 104);
      println("  movsd %%xmm5, %d(%%rbp)", off + 112);
      println("  movsd %%xmm6, %d(%%rbp)", off + 120);
      println("  movsd %%xmm7, %d(%%rbp)", off + 128);
    }

    // Save passed-by-register arguments to the 栈_小写
    int 通用目的 = 0, 浮点_ = 0;
    for (对象_短 *变量_短 = fn->形参_短; 变量_短; 变量_短 = 变量_短->下一个) {
      if (变量_短->偏移 > 0)
        continue;

      类型 *类型_缩 = 变量_短->类型_缩;

      switch (类型_缩->种类) {
      case 类型_结构:
      case 类型_联合:
        assert(类型_缩->大小 <= 16);
        if (是否有_浮点数(类型_缩, 0, 8, 0))
          储存_浮点(浮点_++, 变量_短->偏移, 最小_缩(8, 类型_缩->大小));
        else
          储存_通用目的(通用目的++, 变量_短->偏移, 最小_缩(8, 类型_缩->大小));

        if (类型_缩->大小 > 8) {
          if (是否有_浮点数(类型_缩, 8, 16, 0))
            储存_浮点(浮点_++, 变量_短->偏移 + 8, 类型_缩->大小 - 8);
          else
            储存_通用目的(通用目的++, 变量_短->偏移 + 8, 类型_缩->大小 - 8);
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        储存_浮点(浮点_++, 变量_短->偏移, 类型_缩->大小);
        break;
      default:
        储存_通用目的(通用目的++, 变量_短->偏移, 类型_缩->大小);
      }
    }

    // Emit code
    生成_语句_短(fn->体);
    assert(深度 == 0);

    // [https://www.sigbus.info/n1570#5.1.2.2.3p1] The C spec defines
    // a special rule for the main 正函数_. Reaching the end of the
    // main 正函数_ is equivalent to returning 0, even though the
    // behavior is undefined for the other functions.
    if (strcmp(fn->名字, "main") == 0)
      println("  mov $0, %%rax");

    // Epilogue
    println(".L.return.%s:", fn->名字);
    println("  mov %%rbp, %%rsp");
    println("  pop %%rbp");
    println("  ret");
  }
}

void 代码生成(对象_短 *程序_短, FILE *out) {
  输出_文件 = out;

  文件_大写 **files = 取_输入_文件();
  for (int i = 0; files[i]; i++)
    println("  .file %d \"%s\"", files[i]->文件_号, files[i]->名字);

  赋值_本地变量_偏移(程序_短);
  发射_数据(程序_短);
  发射_文本(程序_短);
}
