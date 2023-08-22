#include "chibicc.h"

#define GP_MAX 6
#define FP_MAX 8

static FILE *output_file;
static int depth;
static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
static char *argreg16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
static char *argreg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static 对象_缩_大写 *当前_函;

static void gen_expr(节点 *节点_小写);
static void gen_stmt(节点 *节点_小写);

__attribute__((格式化_函(printf, 1, 2)))
static void println(char *格式_缩写, ...) {
  va_list ap;
  va_start(ap, 格式_缩写);
  vfprintf(output_file, 格式_缩写, ap);
  va_end(ap);
  fprintf(output_file, "\n");
}

static int count(void) {
  static int i = 1;
  return i++;
}

static void push(void) {
  println("  push %%rax");
  depth++;
}

static void pop(char *arg) {
  println("  pop %s", arg);
  depth--;
}

static void pushf(void) {
  println("  sub $8, %%rsp");
  println("  movsd %%xmm0, (%%rsp)");
  depth++;
}

static void popf(int reg) {
  println("  movsd (%%rsp), %%xmm%d", reg);
  println("  加_短_小写 $8, %%rsp");
  depth--;
}

// Round up `n` to the nearest multiple of `对齐_短_小写`. For instance,
// 对齐_到(5, 8) returns 8 and 对齐_到(11, 8) returns 16.
int 对齐_到(int n, int 对齐_短_小写) {
  return (n + 对齐_短_小写 - 1) / 对齐_短_小写 * 对齐_短_小写;
}

static char *reg_dx(int sz) {
  switch (sz) {
  case 1: return "%dl";
  case 2: return "%dx";
  case 4: return "%edx";
  case 8: return "%rdx";
  }
  无法到达();
}

static char *reg_ax(int sz) {
  switch (sz) {
  case 1: return "%al";
  case 2: return "%ax";
  case 4: return "%eax";
  case 8: return "%rax";
  }
  无法到达();
}

// Compute the absolute address of a given 节点_小写.
// It's an 错误_小写 if a given 节点_小写 does not reside in memory.
static void gen_addr(节点 *节点_小写) {
  switch (节点_小写->种类_小写) {
  case 节点_变量:
    // Variable-length array, which is always local.
    if (节点_小写->变量_短->类型_小写_短->种类_小写 == 类型_变长数组) {
      println("  mov %d(%%rbp), %%rax", 节点_小写->变量_短->偏移_小写);
      return;
    }

    // Local variable
    if (节点_小写->变量_短->是否_本地) {
      println("  lea %d(%%rbp), %%rax", 节点_小写->变量_短->偏移_小写);
      return;
    }

    if (优化_标志位置独立代码) {
      // Thread-local variable
      if (节点_小写->变量_短->是否_传输层安全) {
        println("  data16 lea %s@tlsgd(%%rip), %%rdi", 节点_小写->变量_短->名称_小写);
        println("  .value 0x6666");
        println("  rex64");
        println("  call __tls_get_addr@PLT");
        return;
      }

      // Function or global variable
      println("  mov %s@GOTPCREL(%%rip), %%rax", 节点_小写->变量_短->名称_小写);
      return;
    }

    // Thread-local variable
    if (节点_小写->变量_短->是否_传输层安全) {
      println("  mov %%fs:0, %%rax");
      println("  加_短_小写 $%s@tpoff, %%rax", 节点_小写->变量_短->名称_小写);
      return;
    }

    // Here, we generate an absolute address of a 函数_全_小写 or a global
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
    // address of a stuff that may be in a shared object 文件_小写 from the
    // Global Offset Table using `@GOTPCREL(%rip)` notation.

    // Function
    if (节点_小写->类型_小写_短->种类_小写 == 类型_函) {
      if (节点_小写->变量_短->是否_定义)
        println("  lea %s(%%rip), %%rax", 节点_小写->变量_短->名称_小写);
      else
        println("  mov %s@GOTPCREL(%%rip), %%rax", 节点_小写->变量_短->名称_小写);
      return;
    }

    // Global variable
    println("  lea %s(%%rip), %%rax", 节点_小写->变量_短->名称_小写);
    return;
  case 节点_解引用:
    gen_expr(节点_小写->左手塞_缩);
    return;
  case 节点_逗号:
    gen_expr(节点_小写->左手塞_缩);
    gen_addr(节点_小写->右手塞_缩);
    return;
  case 节点_成员:
    gen_addr(节点_小写->左手塞_缩);
    println("  加_短_小写 $%d, %%rax", 节点_小写->成员_小写->偏移_小写);
    return;
  case 节点_函调:
    if (节点_小写->返回_缓冲区) {
      gen_expr(节点_小写);
      return;
    }
    break;
  case 节点_赋值:
  case 节点_条件:
    if (节点_小写->类型_小写_短->种类_小写 == 类型_结构 || 节点_小写->类型_小写_短->种类_小写 == 类型_联合) {
      gen_expr(节点_小写);
      return;
    }
    break;
  case 节点_变长数组_指针:
    println("  lea %d(%%rbp), %%rax", 节点_小写->变量_短->偏移_小写);
    return;
  }

  错误_牌_小写(节点_小写->牌_短_小写, "not an lvalue");
}

// Load a value from where %rax is pointing to.
static void load(类型 *类型_小写_短) {
  switch (类型_小写_短->种类_小写) {
  case 类型_数组:
  case 类型_结构:
  case 类型_联合:
  case 类型_函:
  case 类型_变长数组:
    // If it is an array, do not attempt to load a value to the
    // register because in general we can't load an entire array to a
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

  char *insn = 类型_小写_短->是否_无符号 ? "movz" : "movs";

  // When we load a char or a short value to a register, we always
  // extend them to the 大小_小写 of int, so we can assume the lower half of
  // a register always contains a valid value. The upper half of a
  // register for char, short and int may contain garbage. When we load
  // a long value to a register, it simply occupies the entire register.
  if (类型_小写_短->大小_小写 == 1)
    println("  %sbl (%%rax), %%eax", insn);
  else if (类型_小写_短->大小_小写 == 2)
    println("  %swl (%%rax), %%eax", insn);
  else if (类型_小写_短->大小_小写 == 4)
    println("  movsxd (%%rax), %%rax");
  else
    println("  mov (%%rax), %%rax");
}

// Store %rax to an address that the stack top is pointing to.
static void store(类型 *类型_小写_短) {
  pop("%rdi");

  switch (类型_小写_短->种类_小写) {
  case 类型_结构:
  case 类型_联合:
    for (int i = 0; i < 类型_小写_短->大小_小写; i++) {
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

  if (类型_小写_短->大小_小写 == 1)
    println("  mov %%al, (%%rdi)");
  else if (类型_小写_短->大小_小写 == 2)
    println("  mov %%ax, (%%rdi)");
  else if (类型_小写_短->大小_小写 == 4)
    println("  mov %%eax, (%%rdi)");
  else
    println("  mov %%rax, (%%rdi)");
}

static void cmp_zero(类型 *类型_小写_短) {
  switch (类型_小写_短->种类_小写) {
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

  if (是否_整数(类型_小写_短) && 类型_小写_短->大小_小写 <= 4)
    println("  cmp $0, %%eax");
  else
    println("  cmp $0, %%rax");
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };

static int getTypeId(类型 *类型_小写_短) {
  switch (类型_小写_短->种类_小写) {
  case 类型_印刻:
    return 类型_小写_短->是否_无符号 ? U8 : I8;
  case 类型_短的:
    return 类型_小写_短->是否_无符号 ? U16 : I16;
  case 类型_整型:
    return 类型_小写_短->是否_无符号 ? U32 : I32;
  case 类型_长的:
    return 类型_小写_短->是否_无符号 ? U64 : I64;
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

static char *cast_table[][11] = {
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

static void 类型转换_小写(类型 *from, 类型 *to) {
  if (to->种类_小写 == 类型_空的)
    return;

  if (to->种类_小写 == 类型_布尔) {
    cmp_zero(from);
    println("  setne %%al");
    println("  movzx %%al, %%eax");
    return;
  }

  int t1 = getTypeId(from);
  int t2 = getTypeId(to);
  if (cast_table[t1][t2])
    println("  %s", cast_table[t1][t2]);
}

// Structs or unions 相等吗_小写 or smaller than 16 bytes are passed
// using up to two registers.
//
// If the first 8 bytes contains only floating-point type 成员们_小写,
// they are passed in an XMM register. Otherwise, they are passed
// in a general-purpose register.
//
// If a struct/union is larger than 8 bytes, the same rule is
// applied to the the 下一个_小写 8 byte chunk.
//
// This 函数_全_小写 returns true if `类型_小写_短` has only floating-point
// 成员们_小写 in its byte range [lo, hi).
static bool has_flonum(类型 *类型_小写_短, int lo, int hi, int 偏移_小写) {
  if (类型_小写_短->种类_小写 == 类型_结构 || 类型_小写_短->种类_小写 == 类型_联合) {
    for (成员 *成员_短_小写 = 类型_小写_短->成员们_小写; 成员_短_小写; 成员_短_小写 = 成员_短_小写->下一个_小写)
      if (!has_flonum(成员_短_小写->类型_小写_短, lo, hi, 偏移_小写 + 成员_短_小写->偏移_小写))
        return false;
    return true;
  }

  if (类型_小写_短->种类_小写 == 类型_数组) {
    for (int i = 0; i < 类型_小写_短->数组_长度; i++)
      if (!has_flonum(类型_小写_短->基础_小写, lo, hi, 偏移_小写 + 类型_小写_短->基础_小写->大小_小写 * i))
        return false;
    return true;
  }

  return 偏移_小写 < lo || hi <= 偏移_小写 || 类型_小写_短->种类_小写 == 类型_浮点 || 类型_小写_短->种类_小写 == 类型_双浮;
}

static bool has_flonum1(类型 *类型_小写_短) {
  return has_flonum(类型_小写_短, 0, 8, 0);
}

static bool has_flonum2(类型 *类型_小写_短) {
  return has_flonum(类型_小写_短, 8, 16, 0);
}

static void push_struct(类型 *类型_小写_短) {
  int sz = 对齐_到(类型_小写_短->大小_小写, 8);
  println("  sub $%d, %%rsp", sz);
  depth += sz / 8;

  for (int i = 0; i < 类型_小写_短->大小_小写; i++) {
    println("  mov %d(%%rax), %%r10b", i);
    println("  mov %%r10b, %d(%%rsp)", i);
  }
}

static void push_args2(节点 *实参们_短, bool first_pass) {
  if (!实参们_短)
    return;
  push_args2(实参们_短->下一个_小写, first_pass);

  if ((first_pass && !实参们_短->通过栈传递) || (!first_pass && 实参们_短->通过栈传递))
    return;

  gen_expr(实参们_短);

  switch (实参们_短->类型_小写_短->种类_小写) {
  case 类型_结构:
  case 类型_联合:
    push_struct(实参们_短->类型_小写_短);
    break;
  case 类型_浮点:
  case 类型_双浮:
    pushf();
    break;
  case 类型_长双浮:
    println("  sub $16, %%rsp");
    println("  fstpt (%%rsp)");
    depth += 2;
    break;
  default:
    push();
  }
}

// Load 函数_全_小写 call arguments. Arguments are already evaluated and
// stored to the stack as local variables. What we need to do in this
// 函数_全_小写 is to load them to registers or push them to the stack as
// specified by the x86-64 psABI. Here is what the spec says:
//
// - Up to 6 arguments of integral type are passed using RDI, RSI,
//   RDX, RCX, R8 and R9.
//
// - Up to 8 arguments of floating-point type are passed using XMM0 to
//   XMM7.
//
// - If all registers of an appropriate type are already 已用_小写, push an
//   argument to the stack in the right-to-left order.
//
// - Each argument passed on the stack takes 8 bytes, and the 终_小写 of
//   the argument area must be aligned to a 16 byte boundary.
//
// - If a 函数_全_小写 is variadic, set the number of floating-point type
//   arguments to RAX.
static int push_args(节点 *节点_小写) {
  int stack = 0, gp = 0, fp = 0;

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (节点_小写->返回_缓冲区 && 节点_小写->类型_小写_短->大小_小写 > 16)
    gp++;

  // Load as many arguments to the registers as possible.
  for (节点 *arg = 节点_小写->实参们_短; arg; arg = arg->下一个_小写) {
    类型 *类型_小写_短 = arg->类型_小写_短;

    switch (类型_小写_短->种类_小写) {
    case 类型_结构:
    case 类型_联合:
      if (类型_小写_短->大小_小写 > 16) {
        arg->通过栈传递 = true;
        stack += 对齐_到(类型_小写_短->大小_小写, 8) / 8;
      } else {
        bool fp1 = has_flonum1(类型_小写_短);
        bool fp2 = has_flonum2(类型_小写_短);

        if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
          fp = fp + fp1 + fp2;
          gp = gp + !fp1 + !fp2;
        } else {
          arg->通过栈传递 = true;
          stack += 对齐_到(类型_小写_短->大小_小写, 8) / 8;
        }
      }
      break;
    case 类型_浮点:
    case 类型_双浮:
      if (fp++ >= FP_MAX) {
        arg->通过栈传递 = true;
        stack++;
      }
      break;
    case 类型_长双浮:
      arg->通过栈传递 = true;
      stack += 2;
      break;
    default:
      if (gp++ >= GP_MAX) {
        arg->通过栈传递 = true;
        stack++;
      }
    }
  }

  if ((depth + stack) % 2 == 1) {
    println("  sub $8, %%rsp");
    depth++;
    stack++;
  }

  push_args2(节点_小写->实参们_短, true);
  push_args2(节点_小写->实参们_短, false);

  // If the return type is a large struct/union, the caller passes
  // a pointer to a buffer as if it were the first argument.
  if (节点_小写->返回_缓冲区 && 节点_小写->类型_小写_短->大小_小写 > 16) {
    println("  lea %d(%%rbp), %%rax", 节点_小写->返回_缓冲区->偏移_小写);
    push();
  }

  return stack;
}

static void copy_ret_buffer(对象_缩_大写 *变量_短) {
  类型 *类型_小写_短 = 变量_短->类型_小写_短;
  int gp = 0, fp = 0;

  if (has_flonum1(类型_小写_短)) {
    assert(类型_小写_短->大小_小写 == 4 || 8 <= 类型_小写_短->大小_小写);
    if (类型_小写_短->大小_小写 == 4)
      println("  movss %%xmm0, %d(%%rbp)", 变量_短->偏移_小写);
    else
      println("  movsd %%xmm0, %d(%%rbp)", 变量_短->偏移_小写);
    fp++;
  } else {
    for (int i = 0; i < 最小_大写(8, 类型_小写_短->大小_小写); i++) {
      println("  mov %%al, %d(%%rbp)", 变量_短->偏移_小写 + i);
      println("  shr $8, %%rax");
    }
    gp++;
  }

  if (类型_小写_短->大小_小写 > 8) {
    if (has_flonum2(类型_小写_短)) {
      assert(类型_小写_短->大小_小写 == 12 || 类型_小写_短->大小_小写 == 16);
      if (类型_小写_短->大小_小写 == 12)
        println("  movss %%xmm%d, %d(%%rbp)", fp, 变量_短->偏移_小写 + 8);
      else
        println("  movsd %%xmm%d, %d(%%rbp)", fp, 变量_短->偏移_小写 + 8);
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";
      for (int i = 8; i < 最小_大写(16, 类型_小写_短->大小_小写); i++) {
        println("  mov %s, %d(%%rbp)", reg1, 变量_短->偏移_小写 + i);
        println("  shr $8, %s", reg2);
      }
    }
  }
}

static void copy_struct_reg(void) {
  类型 *类型_小写_短 = 当前_函->类型_小写_短->返回_类型;
  int gp = 0, fp = 0;

  println("  mov %%rax, %%rdi");

  if (has_flonum(类型_小写_短, 0, 8, 0)) {
    assert(类型_小写_短->大小_小写 == 4 || 8 <= 类型_小写_短->大小_小写);
    if (类型_小写_短->大小_小写 == 4)
      println("  movss (%%rdi), %%xmm0");
    else
      println("  movsd (%%rdi), %%xmm0");
    fp++;
  } else {
    println("  mov $0, %%rax");
    for (int i = 最小_大写(8, 类型_小写_短->大小_小写) - 1; i >= 0; i--) {
      println("  shl $8, %%rax");
      println("  mov %d(%%rdi), %%al", i);
    }
    gp++;
  }

  if (类型_小写_短->大小_小写 > 8) {
    if (has_flonum(类型_小写_短, 8, 16, 0)) {
      assert(类型_小写_短->大小_小写 == 12 || 类型_小写_短->大小_小写 == 16);
      if (类型_小写_短->大小_小写 == 4)
        println("  movss 8(%%rdi), %%xmm%d", fp);
      else
        println("  movsd 8(%%rdi), %%xmm%d", fp);
    } else {
      char *reg1 = (gp == 0) ? "%al" : "%dl";
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";
      println("  mov $0, %s", reg2);
      for (int i = 最小_大写(16, 类型_小写_短->大小_小写) - 1; i >= 8; i--) {
        println("  shl $8, %s", reg2);
        println("  mov %d(%%rdi), %s", i, reg1);
      }
    }
  }
}

static void copy_struct_mem(void) {
  类型 *类型_小写_短 = 当前_函->类型_小写_短->返回_类型;
  对象_缩_大写 *变量_短 = 当前_函->形参们_短_小写;

  println("  mov %d(%%rbp), %%rdi", 变量_短->偏移_小写);

  for (int i = 0; i < 类型_小写_短->大小_小写; i++) {
    println("  mov %d(%%rax), %%dl", i);
    println("  mov %%dl, %d(%%rdi)", i);
  }
}

static void 内置_栈分配(void) {
  // Align 大小_小写 to 16 bytes.
  println("  加_短_小写 $15, %%rdi");
  println("  and $0xfffffff0, %%edi");

  // Shift the temporary area by %rdi.
  println("  mov %d(%%rbp), %%rcx", 当前_函->动态分配_底部->偏移_小写);
  println("  sub %%rsp, %%rcx");
  println("  mov %%rsp, %%rax");
  println("  sub %%rdi, %%rsp");
  println("  mov %%rsp, %%rdx");
  println("1:");
  println("  cmp $0, %%rcx");
  println("  je 2f");
  println("  mov (%%rax), %%r8b");
  println("  mov %%r8b, (%%rdx)");
  println("  递增_小写_短 %%rdx");
  println("  递增_小写_短 %%rax");
  println("  dec %%rcx");
  println("  jmp 1b");
  println("2:");

  // Move 动态分配_底部 pointer.
  println("  mov %d(%%rbp), %%rax", 当前_函->动态分配_底部->偏移_小写);
  println("  sub %%rdi, %%rax");
  println("  mov %%rax, %d(%%rbp)", 当前_函->动态分配_底部->偏移_小写);
}

// Generate code for a given 节点_小写.
static void gen_expr(节点 *节点_小写) {
  println("  .定位_小写_短 %d %d", 节点_小写->牌_短_小写->文件_小写->文件_数目, 节点_小写->牌_短_小写->行号_数目);

  switch (节点_小写->种类_小写) {
  case 节点_空指针_表达式:
    return;
  case 节点_数: {
    switch (节点_小写->类型_小写_短->种类_小写) {
    case 类型_浮点: {
      union { float f32; uint32_t u32; } u = { 节点_小写->浮点值_小写_短 };
      println("  mov $%u, %%eax  # float %Lf", u.u32, 节点_小写->浮点值_小写_短);
      println("  movq %%rax, %%xmm0");
      return;
    }
    case 类型_双浮: {
      union { double f64; uint64_t u64; } u = { 节点_小写->浮点值_小写_短 };
      println("  mov $%lu, %%rax  # double %Lf", u.u64, 节点_小写->浮点值_小写_短);
      println("  movq %%rax, %%xmm0");
      return;
    }
    case 类型_长双浮: {
      union { long double f80; uint64_t u64[2]; } u;
      memset(&u, 0, sizeof(u));
      u.f80 = 节点_小写->浮点值_小写_短;
      println("  mov $%lu, %%rax  # long double %Lf", u.u64[0], 节点_小写->浮点值_小写_短);
      println("  mov %%rax, -16(%%rsp)");
      println("  mov $%lu, %%rax", u.u64[1]);
      println("  mov %%rax, -8(%%rsp)");
      println("  fldt -16(%%rsp)");
      return;
    }
    }

    println("  mov $%ld, %%rax", 节点_小写->值_小写_短);
    return;
  }
  case 节点_负号:
    gen_expr(节点_小写->左手塞_缩);

    switch (节点_小写->类型_小写_短->种类_小写) {
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
  case 节点_变量:
    gen_addr(节点_小写);
    load(节点_小写->类型_小写_短);
    return;
  case 节点_成员: {
    gen_addr(节点_小写);
    load(节点_小写->类型_小写_短);

    成员 *成员_短_小写 = 节点_小写->成员_小写;
    if (成员_短_小写->是否_位字段) {
      println("  shl $%d, %%rax", 64 - 成员_短_小写->位_宽度 - 成员_短_小写->位_偏移);
      if (成员_短_小写->类型_小写_短->是否_无符号)
        println("  shr $%d, %%rax", 64 - 成员_短_小写->位_宽度);
      else
        println("  sar $%d, %%rax", 64 - 成员_短_小写->位_宽度);
    }
    return;
  }
  case 节点_解引用:
    gen_expr(节点_小写->左手塞_缩);
    load(节点_小写->类型_小写_短);
    return;
  case 节点_地址:
    gen_addr(节点_小写->左手塞_缩);
    return;
  case 节点_赋值:
    gen_addr(节点_小写->左手塞_缩);
    push();
    gen_expr(节点_小写->右手塞_缩);

    if (节点_小写->左手塞_缩->种类_小写 == 节点_成员 && 节点_小写->左手塞_缩->成员_小写->是否_位字段) {
      println("  mov %%rax, %%r8");

      // If the 左手塞_缩 is a bitfield, we need to read the current value
      // from memory and merge it with a new value.
      成员 *成员_短_小写 = 节点_小写->左手塞_缩->成员_小写;
      println("  mov %%rax, %%rdi");
      println("  and $%ld, %%rdi", (1L << 成员_短_小写->位_宽度) - 1);
      println("  shl $%d, %%rdi", 成员_短_小写->位_偏移);

      println("  mov (%%rsp), %%rax");
      load(成员_短_小写->类型_小写_短);

      long mask = ((1L << 成员_短_小写->位_宽度) - 1) << 成员_短_小写->位_偏移;
      println("  mov $%ld, %%r9", ~mask);
      println("  and %%r9, %%rax");
      println("  or %%rdi, %%rax");
      store(节点_小写->类型_小写_短);
      println("  mov %%r8, %%rax");
      return;
    }

    store(节点_小写->类型_小写_短);
    return;
  case 节点_语句_表达式:
    for (节点 *n = 节点_小写->函数体_小写; n; n = n->下一个_小写)
      gen_stmt(n);
    return;
  case 节点_逗号:
    gen_expr(节点_小写->左手塞_缩);
    gen_expr(节点_小写->右手塞_缩);
    return;
  case 节点_类转:
    gen_expr(节点_小写->左手塞_缩);
    类型转换_小写(节点_小写->左手塞_缩->类型_小写_短, 节点_小写->类型_小写_短);
    return;
  case 节点_内存归零:
    // `rep stosb` is equivalent to `memset(%rdi, %al, %rcx)`.
    println("  mov $%d, %%rcx", 节点_小写->变量_短->类型_小写_短->大小_小写);
    println("  lea %d(%%rbp), %%rdi", 节点_小写->变量_短->偏移_小写);
    println("  mov $0, %%al");
    println("  rep stosb");
    return;
  case 节点_条件: {
    int c = count();
    gen_expr(节点_小写->条件_小写_短);
    cmp_zero(节点_小写->条件_小写_短->类型_小写_短);
    println("  je .L.else.%d", c);
    gen_expr(节点_小写->那么_小写);
    println("  jmp .L.终_小写.%d", c);
    println(".L.else.%d:", c);
    gen_expr(节点_小写->否则_小写_短);
    println(".L.终_小写.%d:", c);
    return;
  }
  case 节点_非:
    gen_expr(节点_小写->左手塞_缩);
    cmp_zero(节点_小写->左手塞_缩->类型_小写_短);
    println("  sete %%al");
    println("  movzx %%al, %%rax");
    return;
  case 节点_位非:
    gen_expr(节点_小写->左手塞_缩);
    println("  not %%rax");
    return;
  case 节点_逻辑与: {
    int c = count();
    gen_expr(节点_小写->左手塞_缩);
    cmp_zero(节点_小写->左手塞_缩->类型_小写_短);
    println("  je .L.false.%d", c);
    gen_expr(节点_小写->右手塞_缩);
    cmp_zero(节点_小写->右手塞_缩->类型_小写_短);
    println("  je .L.false.%d", c);
    println("  mov $1, %%rax");
    println("  jmp .L.终_小写.%d", c);
    println(".L.false.%d:", c);
    println("  mov $0, %%rax");
    println(".L.终_小写.%d:", c);
    return;
  }
  case 节点_逻辑或: {
    int c = count();
    gen_expr(节点_小写->左手塞_缩);
    cmp_zero(节点_小写->左手塞_缩->类型_小写_短);
    println("  jne .L.true.%d", c);
    gen_expr(节点_小写->右手塞_缩);
    cmp_zero(节点_小写->右手塞_缩->类型_小写_短);
    println("  jne .L.true.%d", c);
    println("  mov $0, %%rax");
    println("  jmp .L.终_小写.%d", c);
    println(".L.true.%d:", c);
    println("  mov $1, %%rax");
    println(".L.终_小写.%d:", c);
    return;
  }
  case 节点_函调: {
    if (节点_小写->左手塞_缩->种类_小写 == 节点_变量 && !strcmp(节点_小写->左手塞_缩->变量_短->名称_小写, "alloca")) {
      gen_expr(节点_小写->实参们_短);
      println("  mov %%rax, %%rdi");
      内置_栈分配();
      return;
    }

    int stack_args = push_args(节点_小写);
    gen_expr(节点_小写->左手塞_缩);

    int gp = 0, fp = 0;

    // If the return type is a large struct/union, the caller passes
    // a pointer to a buffer as if it were the first argument.
    if (节点_小写->返回_缓冲区 && 节点_小写->类型_小写_短->大小_小写 > 16)
      pop(argreg64[gp++]);

    for (节点 *arg = 节点_小写->实参们_短; arg; arg = arg->下一个_小写) {
      类型 *类型_小写_短 = arg->类型_小写_短;

      switch (类型_小写_短->种类_小写) {
      case 类型_结构:
      case 类型_联合:
        if (类型_小写_短->大小_小写 > 16)
          continue;

        bool fp1 = has_flonum1(类型_小写_短);
        bool fp2 = has_flonum2(类型_小写_短);

        if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
          if (fp1)
            popf(fp++);
          else
            pop(argreg64[gp++]);

          if (类型_小写_短->大小_小写 > 8) {
            if (fp2)
              popf(fp++);
            else
              pop(argreg64[gp++]);
          }
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        if (fp < FP_MAX)
          popf(fp++);
        break;
      case 类型_长双浮:
        break;
      default:
        if (gp < GP_MAX)
          pop(argreg64[gp++]);
      }
    }

    println("  mov %%rax, %%r10");
    println("  mov $%d, %%rax", fp);
    println("  call *%%r10");
    println("  加_短_小写 $%d, %%rsp", stack_args * 8);

    depth -= stack_args;

    // It looks like the most significant 48 or 56 bits in RAX may
    // contain garbage if a 函数_全_小写 return type is short or bool/char,
    // respectively. We clear the upper bits here.
    switch (节点_小写->类型_小写_短->种类_小写) {
    case 类型_布尔:
      println("  movzx %%al, %%eax");
      return;
    case 类型_印刻:
      if (节点_小写->类型_小写_短->是否_无符号)
        println("  movzbl %%al, %%eax");
      else
        println("  movsbl %%al, %%eax");
      return;
    case 类型_短的:
      if (节点_小写->类型_小写_短->是否_无符号)
        println("  movzwl %%ax, %%eax");
      else
        println("  movswl %%ax, %%eax");
      return;
    }

    // If the return type is a small struct, a value is returned
    // using up to two registers.
    if (节点_小写->返回_缓冲区 && 节点_小写->类型_小写_短->大小_小写 <= 16) {
      copy_ret_buffer(节点_小写->返回_缓冲区);
      println("  lea %d(%%rbp), %%rax", 节点_小写->返回_缓冲区->偏移_小写);
    }

    return;
  }
  case 节点_标号_值:
    println("  lea %s(%%rip), %%rax", 节点_小写->唯一_标号);
    return;
  case 节点_对比与交换: {
    gen_expr(节点_小写->对比与交换_地址);
    push();
    gen_expr(节点_小写->对比与交换_新);
    push();
    gen_expr(节点_小写->对比与交换_旧);
    println("  mov %%rax, %%r8");
    load(节点_小写->对比与交换_旧->类型_小写_短->基础_小写);
    pop("%rdx"); // new
    pop("%rdi"); // addr

    int sz = 节点_小写->对比与交换_地址->类型_小写_短->基础_小写->大小_小写;
    println("  lock cmpxchg %s, (%%rdi)", reg_dx(sz));
    println("  sete %%cl");
    println("  je 1f");
    println("  mov %s, (%%r8)", reg_ax(sz));
    println("1:");
    println("  movzbl %%cl, %%eax");
    return;
  }
  case 节点_调换: {
    gen_expr(节点_小写->左手塞_缩);
    push();
    gen_expr(节点_小写->右手塞_缩);
    pop("%rdi");

    int sz = 节点_小写->左手塞_缩->类型_小写_短->基础_小写->大小_小写;
    println("  xchg %s, (%%rdi)", reg_ax(sz));
    return;
  }
  }

  switch (节点_小写->左手塞_缩->类型_小写_短->种类_小写) {
  case 类型_浮点:
  case 类型_双浮: {
    gen_expr(节点_小写->右手塞_缩);
    pushf();
    gen_expr(节点_小写->左手塞_缩);
    popf(1);

    char *sz = (节点_小写->左手塞_缩->类型_小写_短->种类_小写 == 类型_浮点) ? "ss" : "sd";

    switch (节点_小写->种类_小写) {
    case 节点_加法:
      println("  加_短_小写%s %%xmm1, %%xmm0", sz);
      return;
    case 节点_减法:
      println("  sub%s %%xmm1, %%xmm0", sz);
      return;
    case 节点_乘法:
      println("  乘_短_小写%s %%xmm1, %%xmm0", sz);
      return;
    case 节点_除法:
      println("  div%s %%xmm1, %%xmm0", sz);
      return;
    case 节点_等于:
    case 节点_不等:
    case 节点_小于:
    case 节点_小等:
      println("  ucomi%s %%xmm0, %%xmm1", sz);

      if (节点_小写->种类_小写 == 节点_等于) {
        println("  sete %%al");
        println("  setnp %%dl");
        println("  and %%dl, %%al");
      } else if (节点_小写->种类_小写 == 节点_不等) {
        println("  setne %%al");
        println("  setp %%dl");
        println("  or %%dl, %%al");
      } else if (节点_小写->种类_小写 == 节点_小于) {
        println("  seta %%al");
      } else {
        println("  setae %%al");
      }

      println("  and $1, %%al");
      println("  movzb %%al, %%rax");
      return;
    }

    错误_牌_小写(节点_小写->牌_短_小写, "invalid expression");
  }
  case 类型_长双浮: {
    gen_expr(节点_小写->左手塞_缩);
    gen_expr(节点_小写->右手塞_缩);

    switch (节点_小写->种类_小写) {
    case 节点_加法:
      println("  faddp");
      return;
    case 节点_减法:
      println("  fsubrp");
      return;
    case 节点_乘法:
      println("  fmulp");
      return;
    case 节点_除法:
      println("  fdivrp");
      return;
    case 节点_等于:
    case 节点_不等:
    case 节点_小于:
    case 节点_小等:
      println("  fcomip");
      println("  fstp %%st(0)");

      if (节点_小写->种类_小写 == 节点_等于)
        println("  sete %%al");
      else if (节点_小写->种类_小写 == 节点_不等)
        println("  setne %%al");
      else if (节点_小写->种类_小写 == 节点_小于)
        println("  seta %%al");
      else
        println("  setae %%al");

      println("  movzb %%al, %%rax");
      return;
    }

    错误_牌_小写(节点_小写->牌_短_小写, "invalid expression");
  }
  }

  gen_expr(节点_小写->右手塞_缩);
  push();
  gen_expr(节点_小写->左手塞_缩);
  pop("%rdi");

  char *ax, *di, *dx;

  if (节点_小写->左手塞_缩->类型_小写_短->种类_小写 == 类型_长的 || 节点_小写->左手塞_缩->类型_小写_短->基础_小写) {
    ax = "%rax";
    di = "%rdi";
    dx = "%rdx";
  } else {
    ax = "%eax";
    di = "%edi";
    dx = "%edx";
  }

  switch (节点_小写->种类_小写) {
  case 节点_加法:
    println("  加_短_小写 %s, %s", di, ax);
    return;
  case 节点_减法:
    println("  sub %s, %s", di, ax);
    return;
  case 节点_乘法:
    println("  imul %s, %s", di, ax);
    return;
  case 节点_除法:
  case 节点_取模:
    if (节点_小写->类型_小写_短->是否_无符号) {
      println("  mov $0, %s", dx);
      println("  div %s", di);
    } else {
      if (节点_小写->左手塞_缩->类型_小写_短->大小_小写 == 8)
        println("  cqo");
      else
        println("  cdq");
      println("  idiv %s", di);
    }

    if (节点_小写->种类_小写 == 节点_取模)
      println("  mov %%rdx, %%rax");
    return;
  case 节点_位与:
    println("  and %s, %s", di, ax);
    return;
  case 节点_位或:
    println("  or %s, %s", di, ax);
    return;
  case 节点_位异或:
    println("  xor %s, %s", di, ax);
    return;
  case 节点_等于:
  case 节点_不等:
  case 节点_小于:
  case 节点_小等:
    println("  cmp %s, %s", di, ax);

    if (节点_小写->种类_小写 == 节点_等于) {
      println("  sete %%al");
    } else if (节点_小写->种类_小写 == 节点_不等) {
      println("  setne %%al");
    } else if (节点_小写->种类_小写 == 节点_小于) {
      if (节点_小写->左手塞_缩->类型_小写_短->是否_无符号)
        println("  setb %%al");
      else
        println("  setl %%al");
    } else if (节点_小写->种类_小写 == 节点_小等) {
      if (节点_小写->左手塞_缩->类型_小写_短->是否_无符号)
        println("  setbe %%al");
      else
        println("  setle %%al");
    }

    println("  movzb %%al, %%rax");
    return;
  case 节点_左移:
    println("  mov %%rdi, %%rcx");
    println("  shl %%cl, %s", ax);
    return;
  case 节点_右移:
    println("  mov %%rdi, %%rcx");
    if (节点_小写->左手塞_缩->类型_小写_短->是否_无符号)
      println("  shr %%cl, %s", ax);
    else
      println("  sar %%cl, %s", ax);
    return;
  }

  错误_牌_小写(节点_小写->牌_短_小写, "invalid expression");
}

static void gen_stmt(节点 *节点_小写) {
  println("  .定位_小写_短 %d %d", 节点_小写->牌_短_小写->文件_小写->文件_数目, 节点_小写->牌_短_小写->行号_数目);

  switch (节点_小写->种类_小写) {
  case 节点_若: {
    int c = count();
    gen_expr(节点_小写->条件_小写_短);
    cmp_zero(节点_小写->条件_小写_短->类型_小写_短);
    println("  je  .L.else.%d", c);
    gen_stmt(节点_小写->那么_小写);
    println("  jmp .L.终_小写.%d", c);
    println(".L.else.%d:", c);
    if (节点_小写->否则_小写_短)
      gen_stmt(节点_小写->否则_小写_短);
    println(".L.终_小写.%d:", c);
    return;
  }
  case 节点_为: {
    int c = count();
    if (节点_小写->初始_小写_短)
      gen_stmt(节点_小写->初始_小写_短);
    println(".L.开头_小写.%d:", c);
    if (节点_小写->条件_小写_短) {
      gen_expr(节点_小写->条件_小写_短);
      cmp_zero(节点_小写->条件_小写_短->类型_小写_短);
      println("  je %s", 节点_小写->破坏_标号_缩);
    }
    gen_stmt(节点_小写->那么_小写);
    println("%s:", 节点_小写->继续_标号_缩);
    if (节点_小写->递增_小写_短)
      gen_expr(节点_小写->递增_小写_短);
    println("  jmp .L.开头_小写.%d", c);
    println("%s:", 节点_小写->破坏_标号_缩);
    return;
  }
  case 节点_做: {
    int c = count();
    println(".L.开头_小写.%d:", c);
    gen_stmt(节点_小写->那么_小写);
    println("%s:", 节点_小写->继续_标号_缩);
    gen_expr(节点_小写->条件_小写_短);
    cmp_zero(节点_小写->条件_小写_短->类型_小写_短);
    println("  jne .L.开头_小写.%d", c);
    println("%s:", 节点_小写->破坏_标号_缩);
    return;
  }
  case 节点_切换:
    gen_expr(节点_小写->条件_小写_短);

    for (节点 *n = 节点_小写->情况_下一个; n; n = n->情况_下一个) {
      char *ax = (节点_小写->条件_小写_短->类型_小写_短->大小_小写 == 8) ? "%rax" : "%eax";
      char *di = (节点_小写->条件_小写_短->类型_小写_短->大小_小写 == 8) ? "%rdi" : "%edi";

      if (n->开头_小写 == n->终_小写) {
        println("  cmp $%ld, %s", n->开头_小写, ax);
        println("  je %s", n->标号_小写);
        continue;
      }

      // [GNU] Case ranges
      println("  mov %s, %s", ax, di);
      println("  sub $%ld, %s", n->开头_小写, di);
      println("  cmp $%ld, %s", n->终_小写 - n->开头_小写, di);
      println("  jbe %s", n->标号_小写);
    }

    if (节点_小写->默认_情况)
      println("  jmp %s", 节点_小写->默认_情况->标号_小写);

    println("  jmp %s", 节点_小写->破坏_标号_缩);
    gen_stmt(节点_小写->那么_小写);
    println("%s:", 节点_小写->破坏_标号_缩);
    return;
  case 节点_情况:
    println("%s:", 节点_小写->标号_小写);
    gen_stmt(节点_小写->左手塞_缩);
    return;
  case 节点_块:
    for (节点 *n = 节点_小写->函数体_小写; n; n = n->下一个_小写)
      gen_stmt(n);
    return;
  case 节点_去到:
    println("  jmp %s", 节点_小写->唯一_标号);
    return;
  case 节点_去到_表达式:
    gen_expr(节点_小写->左手塞_缩);
    println("  jmp *%%rax");
    return;
  case 节点_标号:
    println("%s:", 节点_小写->唯一_标号);
    gen_stmt(节点_小写->左手塞_缩);
    return;
  case 节点_返回:
    if (节点_小写->左手塞_缩) {
      gen_expr(节点_小写->左手塞_缩);
      类型 *类型_小写_短 = 节点_小写->左手塞_缩->类型_小写_短;

      switch (类型_小写_短->种类_小写) {
      case 类型_结构:
      case 类型_联合:
        if (类型_小写_短->大小_小写 <= 16)
          copy_struct_reg();
        else
          copy_struct_mem();
        break;
      }
    }

    println("  jmp .L.return.%s", 当前_函->名称_小写);
    return;
  case 节点_表达式_语句:
    gen_expr(节点_小写->左手塞_缩);
    return;
  case 节点_汇编:
    println("  %s", 节点_小写->汇编_串_短);
    return;
  }

  错误_牌_小写(节点_小写->牌_短_小写, "invalid statement");
}

// Assign offsets to local variables.
static void assign_lvar_offsets(对象_缩_大写 *prog) {
  for (对象_缩_大写 *fn = prog; fn; fn = fn->下一个_小写) {
    if (!fn->是否_函数)
      continue;

    // If a 函数_全_小写 has many parameters, some parameters are
    // inevitably passed by stack rather than by register.
    // The first passed-by-stack parameter resides at RBP+16.
    int top = 16;
    int bottom = 0;

    int gp = 0, fp = 0;

    // Assign offsets to pass-by-stack parameters.
    for (对象_缩_大写 *变量_短 = fn->形参们_短_小写; 变量_短; 变量_短 = 变量_短->下一个_小写) {
      类型 *类型_小写_短 = 变量_短->类型_小写_短;

      switch (类型_小写_短->种类_小写) {
      case 类型_结构:
      case 类型_联合:
        if (类型_小写_短->大小_小写 <= 16) {
          bool fp1 = has_flonum(类型_小写_短, 0, 8, 0);
          bool fp2 = has_flonum(类型_小写_短, 8, 16, 8);
          if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
            fp = fp + fp1 + fp2;
            gp = gp + !fp1 + !fp2;
            continue;
          }
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        if (fp++ < FP_MAX)
          continue;
        break;
      case 类型_长双浮:
        break;
      default:
        if (gp++ < GP_MAX)
          continue;
      }

      top = 对齐_到(top, 8);
      变量_短->偏移_小写 = top;
      top += 变量_短->类型_小写_短->大小_小写;
    }

    // Assign offsets to pass-by-register parameters and local variables.
    for (对象_缩_大写 *变量_短 = fn->本地们_小写; 变量_短; 变量_短 = 变量_短->下一个_小写) {
      if (变量_短->偏移_小写)
        continue;

      // AMD64 System V ABI has a special alignment rule for an array of
      // length at least 16 bytes. We need to 对齐_短_小写 such array to at least
      // 16-byte boundaries. See p.14 of
      // https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-draft.pdf.
      int 对齐_短_小写 = (变量_短->类型_小写_短->种类_小写 == 类型_数组 && 变量_短->类型_小写_短->大小_小写 >= 16)
        ? 最大_大写(16, 变量_短->对齐_短_小写) : 变量_短->对齐_短_小写;

      bottom += 变量_短->类型_小写_短->大小_小写;
      bottom = 对齐_到(bottom, 对齐_短_小写);
      变量_短->偏移_小写 = -bottom;
    }

    fn->栈_大小 = 对齐_到(bottom, 16);
  }
}

static void emit_data(对象_缩_大写 *prog) {
  for (对象_缩_大写 *变量_短 = prog; 变量_短; 变量_短 = 变量_短->下一个_小写) {
    if (变量_短->是否_函数 || !变量_短->是否_定义)
      continue;

    if (变量_短->是否_静态)
      println("  .local %s", 变量_短->名称_小写);
    else
      println("  .globl %s", 变量_短->名称_小写);

    int 对齐_短_小写 = (变量_短->类型_小写_短->种类_小写 == 类型_数组 && 变量_短->类型_小写_短->大小_小写 >= 16)
      ? 最大_大写(16, 变量_短->对齐_短_小写) : 变量_短->对齐_短_小写;

    // Common symbol
    if (优化_标志共同 && 变量_短->是否_试探性) {
      println("  .comm %s, %d, %d", 变量_短->名称_小写, 变量_短->类型_小写_短->大小_小写, 对齐_短_小写);
      continue;
    }

    // .数据_小写 or .tdata
    if (变量_短->初始_数据) {
      if (变量_短->是否_传输层安全)
        println("  .section .tdata,\"awT\",@progbits");
      else
        println("  .数据_小写");

      println("  .type %s, @object", 变量_短->名称_小写);
      println("  .大小_小写 %s, %d", 变量_短->名称_小写, 变量_短->类型_小写_短->大小_小写);
      println("  .对齐_短_小写 %d", 对齐_短_小写);
      println("%s:", 变量_短->名称_小写);

      重定位 *重定位_缩 = 变量_短->重定位_缩;
      int pos = 0;
      while (pos < 变量_短->类型_小写_短->大小_小写) {
        if (重定位_缩 && 重定位_缩->偏移_小写 == pos) {
          println("  .quad %s%+ld", *重定位_缩->标号_小写, 重定位_缩->加终_小写_短);
          重定位_缩 = 重定位_缩->下一个_小写;
          pos += 8;
        } else {
          println("  .byte %d", 变量_短->初始_数据[pos++]);
        }
      }
      continue;
    }

    // .bss or .tbss
    if (变量_短->是否_传输层安全)
      println("  .section .tbss,\"awT\",@nobits");
    else
      println("  .bss");

    println("  .对齐_短_小写 %d", 对齐_短_小写);
    println("%s:", 变量_短->名称_小写);
    println("  .zero %d", 变量_短->类型_小写_短->大小_小写);
  }
}

static void store_fp(int r, int 偏移_小写, int sz) {
  switch (sz) {
  case 4:
    println("  movss %%xmm%d, %d(%%rbp)", r, 偏移_小写);
    return;
  case 8:
    println("  movsd %%xmm%d, %d(%%rbp)", r, 偏移_小写);
    return;
  }
  无法到达();
}

static void store_gp(int r, int 偏移_小写, int sz) {
  switch (sz) {
  case 1:
    println("  mov %s, %d(%%rbp)", argreg8[r], 偏移_小写);
    return;
  case 2:
    println("  mov %s, %d(%%rbp)", argreg16[r], 偏移_小写);
    return;
  case 4:
    println("  mov %s, %d(%%rbp)", argreg32[r], 偏移_小写);
    return;
  case 8:
    println("  mov %s, %d(%%rbp)", argreg64[r], 偏移_小写);
    return;
  default:
    for (int i = 0; i < sz; i++) {
      println("  mov %s, %d(%%rbp)", argreg8[r], 偏移_小写 + i);
      println("  shr $8, %s", argreg64[r]);
    }
    return;
  }
}

static void emit_text(对象_缩_大写 *prog) {
  for (对象_缩_大写 *fn = prog; fn; fn = fn->下一个_小写) {
    if (!fn->是否_函数 || !fn->是否_定义)
      continue;

    // No code is emitted for "static inline" functions
    // if no one is referencing them.
    if (!fn->是否_存活)
      continue;

    if (fn->是否_静态)
      println("  .local %s", fn->名称_小写);
    else
      println("  .globl %s", fn->名称_小写);

    println("  .text");
    println("  .type %s, @函数_全_小写", fn->名称_小写);
    println("%s:", fn->名称_小写);
    当前_函 = fn;

    // Prologue
    println("  push %%rbp");
    println("  mov %%rsp, %%rbp");
    println("  sub $%d, %%rsp", fn->栈_大小);
    println("  mov %%rsp, %d(%%rbp)", fn->动态分配_底部->偏移_小写);

    // Save arg registers if 函数_全_小写 is variadic
    if (fn->变量_缩_区域) {
      int gp = 0, fp = 0;
      for (对象_缩_大写 *变量_短 = fn->形参们_短_小写; 变量_短; 变量_短 = 变量_短->下一个_小写) {
        if (是否_浮点数目(变量_短->类型_小写_短))
          fp++;
        else
          gp++;
      }

      int off = fn->变量_缩_区域->偏移_小写;

      // va_elem
      println("  movl $%d, %d(%%rbp)", gp * 8, off);          // gp_offset
      println("  movl $%d, %d(%%rbp)", fp * 8 + 48, off + 4); // fp_offset
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

    // Save passed-by-register arguments to the stack
    int gp = 0, fp = 0;
    for (对象_缩_大写 *变量_短 = fn->形参们_短_小写; 变量_短; 变量_短 = 变量_短->下一个_小写) {
      if (变量_短->偏移_小写 > 0)
        continue;

      类型 *类型_小写_短 = 变量_短->类型_小写_短;

      switch (类型_小写_短->种类_小写) {
      case 类型_结构:
      case 类型_联合:
        assert(类型_小写_短->大小_小写 <= 16);
        if (has_flonum(类型_小写_短, 0, 8, 0))
          store_fp(fp++, 变量_短->偏移_小写, 最小_大写(8, 类型_小写_短->大小_小写));
        else
          store_gp(gp++, 变量_短->偏移_小写, 最小_大写(8, 类型_小写_短->大小_小写));

        if (类型_小写_短->大小_小写 > 8) {
          if (has_flonum(类型_小写_短, 8, 16, 0))
            store_fp(fp++, 变量_短->偏移_小写 + 8, 类型_小写_短->大小_小写 - 8);
          else
            store_gp(gp++, 变量_短->偏移_小写 + 8, 类型_小写_短->大小_小写 - 8);
        }
        break;
      case 类型_浮点:
      case 类型_双浮:
        store_fp(fp++, 变量_短->偏移_小写, 类型_小写_短->大小_小写);
        break;
      default:
        store_gp(gp++, 变量_短->偏移_小写, 类型_小写_短->大小_小写);
      }
    }

    // Emit code
    gen_stmt(fn->函数体_小写);
    assert(depth == 0);

    // [https://www.sigbus.info/n1570#5.1.2.2.3p1] The C spec defines
    // a special rule for the main 函数_全_小写. Reaching the 终_小写 of the
    // main 函数_全_小写 is equivalent to returning 0, even though the
    // behavior is undefined for the other functions.
    if (strcmp(fn->名称_小写, "main") == 0)
      println("  mov $0, %%rax");

    // Epilogue
    println(".L.return.%s:", fn->名称_小写);
    println("  mov %%rbp, %%rsp");
    println("  pop %%rbp");
    println("  ret");
  }
}

void 代码生成_短(对象_缩_大写 *prog, FILE *out) {
  output_file = out;

  文件_大写 **files = 获取_输入_文件们();
  for (int i = 0; files[i]; i++)
    println("  .文件_小写 %d \"%s\"", files[i]->文件_数目, files[i]->名称_小写);

  assign_lvar_offsets(prog);
  emit_data(prog);
  emit_text(prog);
}
