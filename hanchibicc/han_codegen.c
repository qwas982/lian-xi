//han_codegen.c

#include "han_chibicc.h"

#define GP_MAX 6   // 定义常量 GP_MAX，值为 6
#define FP_MAX 8   // 定义常量 FP_MAX，值为 8

static FILE *output_file;   // 声明静态变量 output_file，用于文件输出
static int depth;   // 声明静态变量 depth，表示深度
static char *argreg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};   // 声明字符指针数组 argreg8，存储寄存器名称
static char *argreg16[] = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};   // 声明字符指针数组 argreg16，存储寄存器名称
static char *argreg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};   // 声明字符指针数组 argreg32，存储寄存器名称
static char *argreg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};   // 声明字符指针数组 argreg64，存储寄存器名称
static Obj *current_fn;   // 声明对象指针 current_fn，表示当前函数

static void gen_expr(Node *node);   // 声明静态函数 gen_expr，用于生成表达式
static void gen_stmt(Node *node);   // 声明静态函数 gen_stmt，用于生成语句

// 带有格式化参数的静态函数 println，用于打印一行文本
// 参数 fmt 是格式化字符串，后面可能跟着其他参数
// __attribute__((format(printf, 1, 2))) 是指定函数参数的格式化信息，类似于 printf 函数
__attribute__((format(printf, 1, 2))) 
static void println(char *fmt, ...) {
  va_list ap;   // 声明 va_list 类型的变量 ap，用于访问可变参数列表
  va_start(ap, fmt);   // 初始化 va_list 变量 ap，使其指向可变参数列表的起始位置
  vfprintf(output_file, fmt, ap);   // 使用 vfprintf 函数将格式化字符串 fmt 和可变参数列表 ap 写入到 output_file 文件中
  va_end(ap);   // 结束可变参数的访问
  fprintf(output_file, "\n");   // 在 output_file 文件中写入换行符，表示打印结束
}

// 静态函数 count，返回一个递增的整数
static int count(void) {
  static int i = 1;   // 静态变量 i，初始值为 1
  return i++;   // 返回 i 的值，并将 i 增加 1
}

// 静态函数 push，将寄存器 %%rax 压栈
static void push(void) {
  println("  push %%rax");   // 打印 "push %%rax"，表示将 %%rax 寄存器压栈
  depth++;   // 深度加 1
}

// 静态函数 pop，将栈顶元素弹出到指定寄存器 arg
static void pop(char *arg) {
  println("  pop %s", arg);   // 打印 "pop %s"，将栈顶元素弹出到寄存器 arg
  depth--;   // 深度减 1
}

// 静态函数 pushf，执行浮点数压栈操作
static void pushf(void) {
  println("  sub $8, %%rsp");   // 打印 "sub $8, %%rsp"，将栈指针向下移动 8 字节，为浮点数预留空间
  println("  movsd %%xmm0, (%%rsp)");   // 打印 "movsd %%xmm0, (%%rsp)"，将 xmm0 寄存器中的浮点数值存储到栈顶
  depth++;   // 深度加 1
}

// 静态函数 popf，执行浮点数出栈操作，并将值存储到指定的 xmm 寄存器
static void popf(int reg) {
  println("  movsd (%%rsp), %%xmm%d", reg);   // 打印 "movsd (%%rsp), %%xmm%d"，将栈顶的浮点数值加载到 xmm 寄存器 reg
  println("  add $8, %%rsp");   // 打印 "add $8, %%rsp"，将栈指针向上移动 8 字节，恢复栈的状态
  depth--;   // 深度减 1
}

// 将整数 n 向上舍入到最接近的 align 的倍数。例如，
// align_to(5, 8) 返回 8，align_to(11, 8) 返回 16。
int align_to(int n, int align) {
  return (n + align - 1) / align * align;   // 将 n 加上 align - 1，然后除以 align，再乘以 align，得到结果
}

// 根据大小 sz 返回对应的寄存器名称，用于存储 dx 寄存器
static char *reg_dx(int sz) {
  switch (sz) {
  case 1: return "%dl";   // 如果 sz 为 1，返回 "%dl"
  case 2: return "%dx";   // 如果 sz 为 2，返回 "%dx"
  case 4: return "%edx";  // 如果 sz 为 4，返回 "%edx"
  case 8: return "%rdx";  // 如果 sz 为 8，返回 "%rdx"
  }
  unreachable();   // 不可达代码，用于处理未匹配的情况
}

// 根据大小 sz 返回对应的寄存器名称，用于存储 ax 寄存器
static char *reg_ax(int sz) {
  switch (sz) {
  case 1: return "%al";   // 如果 sz 为 1，返回 "%al"
  case 2: return "%ax";   // 如果 sz 为 2，返回 "%ax"
  case 4: return "%eax";  // 如果 sz 为 4，返回 "%eax"
  case 8: return "%rax";  // 如果 sz 为 8，返回 "%rax"
  }
  unreachable();   // 不可达代码，用于处理未匹配的情况
}

// 计算给定节点的绝对地址。
// 如果给定节点不在内存中，则会出现错误。
static void gen_addr(Node *node) {
  switch (node->kind) {
  case ND_VAR: // 变量类型节点
    if (node->var->ty->kind == TY_VLA) { // 变长数组，始终为局部变量
      println("  mov %d(%%rbp), %%rax", node->var->offset); // 将变量的值移动到寄存器 %rax
      return;
    }

    if (node->var->is_local) { // 局部变量
      println("  lea %d(%%rbp), %%rax", node->var->offset); // 将变量的地址加载到寄存器 %rax
      return;
    }

    if (opt_fpic) {
	  // 如果启用了位置无关代码（-fpic选项）
	  if (node->var->is_tls) { // 线程局部变量
	    // 将变量名和偏移量加载到寄存器 %rdi
	    println("  data16 lea %s@tlsgd(%%rip), %%rdi", node->var->name); // 加载线程局部变量的全局动态模型指令
	    // 输出 0x6666
	    println("  .value 0x6666"); // 输出占位值 0x6666
	    println("  rex64"); // 64位寻址
	    // 调用 __tls_get_addr@PLT 函数
	    println("  call __tls_get_addr@PLT"); // 调用 __tls_get_addr@PLT 函数获取线程局部变量的地址
	    return;
	  }

	  // 函数或全局变量
	  // 将变量名加载到寄存器 %rax
	  println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name); // 将变量的 GOTPCREL 形式加载到寄存器 %rax
	  return;
	}

	// 非位置无关代码
	if (node->var->is_tls) { // 线程局部变量
	  // 将 %fs 寄存器的值加载到 %rax 寄存器
	  println("  mov %%fs:0, %%rax"); // 将 %fs 寄存器的值加载到 %rax 寄存器
	  // 将变量名和偏移量相加，结果存储在 %rax 寄存器
	  println("  add $%s@tpoff, %%rax", node->var->name); // 将变量名和偏移量相加，结果存储在 %rax 寄存器
	  return;
	}

	// 在这里，我们生成函数或全局变量的绝对地址。尽管它们在运行时存在于某个地址，
	// 但由于以下两个原因，在链接时它们的地址是未知的。
	//
	// - 地址随机化：可执行文件作为一个整体加载到内存中，但不知道加载到哪个地址。
	//   因此，在链接时，同一可执行文件中的相对地址（即同一可执行文件中两个函数之间的距离）是已知的，
	//   但绝对地址是未知的。
	//
	// - 动态链接：动态共享对象（DSO）或 .so 文件在运行时与可执行文件一起加载到内存中，
	//   并由运行时加载器在内存中进行链接。在运行时重定位完成之前，我们对可能由 DSO 定义的全局数据的地址一无所知。
	//
	// 为了处理前一种情况，我们使用了 RIP 相对寻址，用 `(%rip)` 表示。对于后一种情况，
	// 我们使用 `@GOTPCREL(%rip)` 表示从全局偏移表获取可能位于共享对象文件中的数据的地址。

	if (node->ty->kind == TY_FUNC) {                          // 如果变量的类型为函数
	  if (node->var->is_definition)                           // 如果变量是定义的函数
	    println("  lea %s(%%rip), %%rax", node->var->name);   // 将函数名的地址加载到寄存器 %rax   ,// 使用 RIP 相对寻址加载函数名的地址
	  else                                                    // 如果变量不是定义的函数
	    println("  mov %s@GOTPCREL(%%rip), %%rax", node->var->name); // 将函数名的 GOTPCREL 形式加载到寄存器 %rax   ,// 使用 GOTPCREL 形式加载函数名的地址
	  return;
	}

	// 全局变量
    println("  lea %s(%%rip), %%rax", node->var->name); // 将全局变量的地址加载到寄存器 %rax
    return; // 返回
  case ND_DEREF:
    gen_expr(node->lhs); // 生成左子树的表达式
    return; // 返回
  case ND_COMMA:
    gen_expr(node->lhs); // 生成左子树的表达式
    gen_addr(node->rhs); // 生成右子树的地址
    return; // 返回
  case ND_MEMBER:
    gen_addr(node->lhs); // 生成左子树的地址
    println("  add $%d, %%rax", node->member->offset); // 将成员的偏移量添加到 %rax 寄存器
    return; // 返回
  case ND_FUNCALL:
    if (node->ret_buffer) {
      gen_expr(node); // 生成函数调用表达式
      return; // 返回
    }
    break; // 终止
  case ND_ASSIGN:                                        // 如果是赋值语句或条件语句
  case ND_COND:
    if (node->ty->kind == TY_STRUCT || node->ty->kind == TY_UNION) {  // 如果变量的类型是结构体或联合体
      gen_expr(node);                                   // 生成表达式
      return;                                           // 返回
    }
    break;                                              // 终止
  case ND_VLA_PTR:                                       // 如果是变长数组指针
    println("  lea %d(%%rbp), %%rax", node->var->offset); // 将变量的偏移量加载到寄存器 %rax
    return;                                             // 返回
  }

  error_tok(node->tok, "not an lvalue");                  // 报错，不是左值
}

// 从 %rax 指向的位置加载一个值。
static void load(Type *ty) {
  switch (ty->kind) {
    case TY_ARRAY:
    case TY_STRUCT:
    case TY_UNION:
    case TY_FUNC:
    case TY_VLA:
      // 如果是数组，则不尝试将值加载到寄存器中，
      // 因为通常无法将整个数组加载到寄存器中。
      // 结果是数组的求值结果成为数组本身的地址。
      // 这就是C语言中“数组自动转换为指向数组第一个元素的指针”的发生之处。
      return;  // 对于数组、结构体、联合体、函数和可变长度数组类型，不进行加载操作
    case TY_FLOAT:
      println("  movss (%%rax), %%xmm0");  // 将 (%%rax) 指向的值加载到 %%xmm0 寄存器中
      return;
    case TY_DOUBLE:
      println("  movsd (%%rax), %%xmm0");  // 将 (%%rax) 指向的值加载到 %%xmm0 寄存器中
      return;
    case TY_LDOUBLE:
      println("  fldt (%%rax)");  // 将 (%%rax) 指向的值加载到长双精度浮点寄存器中
      return;
    }

    char *insn = ty->is_unsigned ? "movz" : "movs";  // 根据ty->is_unsigned的值，选择指令"movz"或"movs"赋值给insn变量。
    // 当我们将char或short值加载到寄存器时，我们总是将它们扩展为int大小，
    // 因此我们可以假设寄存器的低半部分始终包含有效值。
    // 对于char、short和int来说，寄存器的上半部分可能包含垃圾值。
    // 当我们将long值加载到寄存器时，它将占据整个寄存器。
    if (ty->size == 1)
      println("  %sbl (%%rax), %%eax", insn);  // 如果ty->size等于1，打印格式化字符串"%sbl (%%rax), %%eax"，其中%s会被insn的值替换。
    else if (ty->size == 2)
      println("  %swl (%%rax), %%eax", insn);  // 如果ty->size等于2，打印格式化字符串"%swl (%%rax), %%eax"，其中%s会被insn的值替换。
    else if (ty->size == 4)
      println("  movsxd (%%rax), %%rax");  // 如果ty->size等于4，打印字符串"movsxd (%%rax), %%rax"。
    else
      println("  mov (%%rax), %%rax");  // 如果ty->size既不等于1、2、4，打印字符串"mov (%%rax), %%rax"。
}

// Store %rax to an address that the stack top is pointing to.
// 将%rax的值存储到栈顶指向的地址。
static void store(Type *ty) {
  pop("%rdi");  // 从栈中弹出一个值，存储到%rdi寄存器中。

  switch (ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    for (int i = 0; i < ty->size; i++) {
      println("  mov %d(%%rax), %%r8b", i);  // 将%rax寄存器偏移i的字节的值加载到%r8b寄存器中。
      println("  mov %%r8b, %d(%%rdi)", i);  // 将%r8b寄存器的值存储到%rdi寄存器偏移i的字节的地址中。
    }
    return;
  case TY_FLOAT:
    println("  movss %%xmm0, (%%rdi)");  // 将%xmm0寄存器的值存储到%rdi寄存器指向的地址中。
    return;
  case TY_DOUBLE:
    println("  movsd %%xmm0, (%%rdi)");  // 将%xmm0寄存器的值存储到%rdi寄存器指向的地址中。
    return;
  case TY_LDOUBLE:
    println("  fstpt (%%rdi)");  // 将%st(0)寄存器的值存储到%rdi寄存器指向的地址中。
    return;
  }

  if (ty->size == 1)
    println("  mov %%al, (%%rdi)");  // 将%al寄存器的值存储到%rdi寄存器指向的地址中。
  else if (ty->size == 2)
    println("  mov %%ax, (%%rdi)");  // 将%ax寄存器的值存储到%rdi寄存器指向的地址中。
  else if (ty->size == 4)
    println("  mov %%eax, (%%rdi)");  // 将%eax寄存器的值存储到%rdi寄存器指向的地址中。
  else
    println("  mov %%rax, (%%rdi)");  // 将%rax寄存器的值存储到%rdi寄存器指向的地址中。
}

static void cmp_zero(Type *ty) {
  switch (ty->kind) {
  case TY_FLOAT:
    println("  xorps %%xmm1, %%xmm1");  // 将寄存器%%xmm1的值与自身进行异或操作，清零寄存器。
    println("  ucomiss %%xmm1, %%xmm0");  // 比较寄存器%%xmm0和%%xmm1中的单精度浮点数，设置相应的标志位。
    return;

  case TY_DOUBLE:
    println("  xorpd %%xmm1, %%xmm1");  // 将寄存器%%xmm1的值与自身进行异或操作，清零寄存器。
    println("  ucomisd %%xmm1, %%xmm0");  // 比较寄存器%%xmm0和%%xmm1中的双精度浮点数，设置相应的标志位。
    return;

  case TY_LDOUBLE:
    println("  fldz");  // 将0.0加载到浮点寄存器栈顶。
    println("  fucomip");  // 比较浮点寄存器栈顶的值与寄存器%%st(0)的值，并设置相应的标志位。
    println("  fstp %%st(0)");  // 弹出浮点寄存器栈顶的值。
    return;
  }

  if (is_integer(ty) && ty->size <= 4)
    println("  cmp $0, %%eax");  // 将寄存器%%eax的值与0进行比较，设置相应的标志位。
  else
    println("  cmp $0, %%rax");  // 将寄存器%%rax的值与0进行比较，设置相应的标志位。
}

enum { I8, I16, I32, I64, U8, U16, U32, U64, F32, F64, F80 };  // 定义枚举类型，表示不同类型的ID

// 根据类型返回相应的类型ID
static int getTypeId(Type *ty) {
  switch (ty->kind) {
  case TY_CHAR:
    return ty->is_unsigned ? U8 : I8;  // 如果是有符号类型，返回U8，否则返回I8。
  case TY_SHORT:
    return ty->is_unsigned ? U16 : I16;  // 如果是有符号类型，返回U16，否则返回I16。
  case TY_INT:
    return ty->is_unsigned ? U32 : I32;  // 如果是有符号类型，返回U32，否则返回I32。
  case TY_LONG:
    return ty->is_unsigned ? U64 : I64;  // 如果是有符号类型，返回U64，否则返回I64。
  case TY_FLOAT:
    return F32;  // 返回F32，表示单精度浮点数。
  case TY_DOUBLE:
    return F64;  // 返回F64，表示双精度浮点数。
  case TY_LDOUBLE:
    return F80;  // 返回F80，表示扩展精度浮点数。
  }
  return U64;  // 默认返回U64，表示64位无符号整数。
}

// The table for type casts
// 定义了一个用于类型转换的表
static char i32i8[] = "movsbl %al, %eax";              // 将有符号的8位整数转换为32位整数，符号扩展
static char i32u8[] = "movzbl %al, %eax";              // 将无符号的8位整数转换为32位整数，零扩展
static char i32i16[] = "movswl %ax, %eax";             // 将有符号的16位整数转换为32位整数，符号扩展
static char i32u16[] = "movzwl %ax, %eax";             // 将无符号的16位整数转换为32位整数，零扩展
static char i32f32[] = "cvtsi2ssl %eax, %xmm0";        // 将32位整数转换为32位浮点数，使用SSE指令
static char i32i64[] = "movsxd %eax, %rax";            // 将32位整数扩展为64位整数，符号扩展
static char i32f64[] = "cvtsi2sdl %eax, %xmm0";        // 将32位整数转换为64位浮点数，使用SSE指令
static char i32f80[] = "mov %eax, -4(%rsp); fildl -4(%rsp)";  // 将32位整数转换为80位浮点数，将值存储在栈上，并使用FPU指令加载

static char u32f32[] = "mov %eax, %eax; cvtsi2ssq %rax, %xmm0"; // 将无符号的32位整数转换为32位浮点数，使用SSE指令
static char u32i64[] = "mov %eax, %eax";               // 将无符号的32位整数转换为64位整数
static char u32f64[] = "mov %eax, %eax; cvtsi2sdq %rax, %xmm0"; // 将无符号的32位整数转换为64位浮点数，使用SSE指令
static char u32f80[] = "mov %eax, %eax; mov %rax, -8(%rsp); fildll -8(%rsp)"; // 将无符号的32位整数转换为80位浮点数，将值存储在栈上，并使用FPU指令加载

static char i64f32[] = "cvtsi2ssq %rax, %xmm0";        // 将64位整数转换为32位浮点数，使用SSE指令
static char i64f64[] = "cvtsi2sdq %rax, %xmm0";        // 将64位整数转换为64位浮点数，使用SSE指令
static char i64f80[] = "movq %rax, -8(%rsp); fildll -8(%rsp)";  // 将64位整数转换为80位浮点数，将值存储在栈上，并使用FPU指令加载

static char u64f32[] = "cvtsi2ssq %rax, %xmm0";  // 将无符号的64位整数转换为32位浮点数，使用SSE指令
static char u64f64[] =
  "test %rax,%rax; js 1f; pxor %xmm0,%xmm0; cvtsi2sd %rax,%xmm0; jmp 2f; "  // 检查是否为负数，如果是则跳转到1标签，执行相应指令序列；否则继续执行2标签处的指令序列
  "1: mov %rax,%rdi; and $1,%eax; pxor %xmm0,%xmm0; shr %rdi; "
  "or %rax,%rdi; cvtsi2sd %rdi,%xmm0; addsd %xmm0,%xmm0; 2:";  // 根据条件将64位整数转换为64位浮点数，使用不同的指令序列
static char u64f80[] =
  "mov %rax, -8(%rsp); fildq -8(%rsp); test %rax, %rax; jns 1f;"  // 将64位整数存储在栈上，并使用FPU指令加载；检查是否为负数，如果是则跳转到1标签
  "mov $1602224128, %eax; mov %eax, -4(%rsp); fadds -4(%rsp); 1:";  // 执行相应的指令序列进行无符号64位整数到80位浮点数的转换

static char f32i8[] = "cvttss2sil %xmm0, %eax; movsbl %al, %eax";  // 将32位浮点数转换为有符号的8位整数，将结果存储在32位整数寄存器中，并将低8位符号扩展到32位
static char f32u8[] = "cvttss2sil %xmm0, %eax; movzbl %al, %eax";  // 将32位浮点数转换为无符号的8位整数，将结果存储在32位整数寄存器中，并将低8位零扩展到32位
static char f32i16[] = "cvttss2sil %xmm0, %eax; movswl %ax, %eax";  // 将32位浮点数转换为有符号的16位整数，将结果存储在32位整数寄存器中，并将低16位符号扩展到32位
static char f32u16[] = "cvttss2sil %xmm0, %eax; movzwl %ax, %eax";  // 将32位浮点数转换为无符号的16位整数，将结果存储在32位整数寄存器中，并将低16位零扩展到32位
static char f32i32[] = "cvttss2sil %xmm0, %eax";  // 将32位浮点数转换为有符号的32位整数，将结果存储在32位整数寄存器中
static char f32u32[] = "cvttss2siq %xmm0, %rax";  // 将32位浮点数转换为无符号的32位整数，将结果存储在64位整数寄存器中
static char f32i64[] = "cvttss2siq %xmm0, %rax";  // 将32位浮点数转换为有符号的64位整数，将结果存储在64位整数寄存器中
static char f32u64[] = "cvttss2siq %xmm0, %rax";  // 将32位浮点数转换为无符号的64位整数，将结果存储在64位整数寄存器中
static char f32f64[] = "cvtss2sd %xmm0, %xmm0";  // 将32位浮点数转换为64位浮点数，结果覆盖原始的xmm0寄存器
static char f32f80[] = "movss %xmm0, -4(%rsp); flds -4(%rsp)";  // 将32位浮点数存储在栈上，并使用FPU指令加载为80位浮点数

static char f64i8[] = "cvttsd2sil %xmm0, %eax; movsbl %al, %eax";  // 将64位浮点数转换为有符号的8位整数，将结果存储在32位整数寄存器中，并将低8位符号扩展到32位
static char f64u8[] = "cvttsd2sil %xmm0, %eax; movzbl %al, %eax";  // 将64位浮点数转换为无符号的8位整数，将结果存储在32位整数寄存器中，并将低8位零扩展到32位
static char f64i16[] = "cvttsd2sil %xmm0, %eax; movswl %ax, %eax";  // 将64位浮点数转换为有符号的16位整数，将结果存储在32位整数寄存器中，并将低16位符号扩展到32位
static char f64u16[] = "cvttsd2sil %xmm0, %eax; movzwl %ax, %eax";  // 将64位浮点数转换为无符号的16位整数，将结果存储在32位整数寄存器中，并将低16位零扩展到32位
static char f64i32[] = "cvttsd2sil %xmm0, %eax";  // 将64位浮点数转换为有符号的32位整数，将结果存储在32位整数寄存器中
static char f64u32[] = "cvttsd2siq %xmm0, %rax";  // 将64位浮点数转换为无符号的32位整数，将结果存储在64位整数寄存器中
static char f64i64[] = "cvttsd2siq %xmm0, %rax";  // 将64位浮点数转换为有符号的64位整数，将结果存储在64位整数寄存器中
static char f64u64[] = "cvttsd2siq %xmm0, %rax";  // 将64位浮点数转换为无符号的64位整数，将结果存储在64位整数寄存器中
static char f64f32[] = "cvtsd2ss %xmm0, %xmm0";  // 将64位浮点数转换为32位浮点数，结果覆盖原始的xmm0寄存器
static char f64f80[] = "movsd %xmm0, -8(%rsp); fldl -8(%rsp)";  // 将64位浮点数存储在栈上，并使用FPU指令加载为80位浮点数

#define FROM_F80_1                                           \  // 定义宏 FROM_F80_1
  "fnstcw -10(%rsp); movzwl -10(%rsp), %eax; or $12, %ah; " \  // 将 FPU 控制字存储到栈中，将低字节扩展为字，并将 12 和高字节进行或操作，存入 eax 寄存器
  "mov %ax, -12(%rsp); fldcw -12(%rsp); "                     // 将 ax 寄存器的值存入栈中，然后加载新的 FPU 控制字

#define FROM_F80_2 " -24(%rsp); fldcw -10(%rsp); "              // 定义宏 FROM_F80_2，加载存储在栈中的新的 FPU 控制字

static char f80i8[] = FROM_F80_1 "fistps" FROM_F80_2 "movsbl -24(%rsp), %eax";  // 定义静态字符数组 f80i8，将栈中的值转换为有符号字节并存入 eax 寄存器
static char f80u8[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";  // 定义静态字符数组 f80u8，将栈中的值转换为无符号字节并存入 eax 寄存器
static char f80i16[] = FROM_F80_1 "fistps" FROM_F80_2 "movzbl -24(%rsp), %eax";  // 定义静态字符数组 f80i16，将栈中的值转换为有符号字节并存入 eax 寄存器
static char f80u16[] = FROM_F80_1 "fistpl" FROM_F80_2 "movswl -24(%rsp), %eax";  // 定义静态字符数组 f80u16，将栈中的值转换为无符号字节并存入 eax 寄存器
static char f80i32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";  // 定义静态字符数组 f80i32，将栈中的值转换为有符号双字并存入 eax 寄存器
static char f80u32[] = FROM_F80_1 "fistpl" FROM_F80_2 "mov -24(%rsp), %eax";  // 定义静态字符数组 f80u32，将栈中的值转换为无符号双字并存入 eax 寄存器
static char f80i64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";  // 定义静态字符数组 f80i64，将栈中的值转换为有符号四字并存入 rax 寄存器
static char f80u64[] = FROM_F80_1 "fistpq" FROM_F80_2 "mov -24(%rsp), %rax";  // 定义静态字符数组 f80u64，将栈中的值转换为无符号四字并存入 rax 寄存器
static char f80f32[] = "fstps -8(%rsp); movss -8(%rsp), %xmm0";  // 定义静态字符数组 f80f32，将栈中的值存储到内存中并加载到 xmm0 寄存器中
static char f80f64[] = "fstpl -8(%rsp); movsd -8(%rsp), %xmm0";  // 定义静态字符数组 f80f64，将栈中的值存储到内存中并加载到 xmm0 寄存器中

static char *cast_table[][11] = {
  // i8   i16     i32     i64     u8     u16     u32     u64     f32     f64     f80
  {NULL,  NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i8，使用不同的转换方式将栈中的值转换为有符号字节
  {i32i8, NULL,   NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i16，使用不同的转换方式将栈中的值转换为有符号短整数
  {i32i8, i32i16, NULL,   i32i64, i32u8, i32u16, NULL,   i32i64, i32f32, i32f64, i32f80}, // i32，使用不同的转换方式将栈中的值转换为有符号整数
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   i64f32, i64f64, i64f80}, // i64，使用不同的转换方式将栈中的值转换为有符号长整数

  {i32i8, NULL,   NULL,   i32i64, NULL,  NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u8，使用不同的转换方式将栈中的值转换为无符号字节
  {i32i8, i32i16, NULL,   i32i64, i32u8, NULL,   NULL,   i32i64, i32f32, i32f64, i32f80}, // u16，使用不同的转换方式将栈中的值转换为无符号短整数
  {i32i8, i32i16, NULL,   u32i64, i32u8, i32u16, NULL,   u32i64, u32f32, u32f64, u32f80}, // u32，使用不同的转换方式将栈中的值转换为无符号整数
  {i32i8, i32i16, NULL,   NULL,   i32u8, i32u16, NULL,   NULL,   u64f32, u64f64, u64f80}, // u64，使用不同的转换方式将栈中的值转换为无符号长整数

  {f32i8, f32i16, f32i32, f32i64, f32u8, f32u16, f32u32, f32u64, NULL,   f32f64, f32f80}, // f32，将栈中的值转换为单精度浮点数
  {f64i8, f64i16, f64i32, f64i64, f64u8, f64u16, f64u32, f64u64, f64f32, NULL,   f64f80}, // f64，将栈中的值转换为双精度浮点数
  {f80i8, f80i16, f80i32, f80i64, f80u8, f80u16, f80u32, f80u64, f80f32, f80f64, NULL},   // f80，将栈中的值存储到内存中并加载到 xmm0 寄存器中
};

static void cast(Type *from, Type *to) {
  if (to->kind == TY_VOID)
    return; // 如果目标类型是void，则不进行任何转换

  if (to->kind == TY_BOOL) {
    cmp_zero(from); // 将from与零进行比较
    println("  setne %%al"); // 设置al寄存器为非零
    println("  movzx %%al, %%eax"); // 将al寄存器的值扩展到eax寄存器
    return; // 返回
  }

  int t1 = getTypeId(from); // 获取from类型的ID
  int t2 = getTypeId(to); // 获取to类型的ID
  if (cast_table[t1][t2]) // 如果在转换表中存在对应的转换方式
    println("  %s", cast_table[t1][t2]); // 输出对应的转换方式
}

// 如果结构体或联合体的大小小于或等于16字节，则最多使用两个寄存器进行传递。
//
// 如果前8字节只包含浮点类型的成员，则它们将传递到XMM寄存器中。否则，它们将传递到通用寄存器中。
//
// 如果结构体/联合体大于8字节，则相同的规则适用于下一个8字节块。
//
// 此函数在字节范围[lo, hi)内的`ty`具有只包含浮点成员时返回true。
static bool has_flonum(Type *ty, int lo, int hi, int offset) { // 检查给定类型是否含有浮点数
  if (ty->kind == TY_STRUCT || ty->kind == TY_UNION) { // 如果类型是结构体或联合体
    for (Member *mem = ty->members; mem; mem = mem->next) // 遍历结构体或联合体的成员
      if (!has_flonum(mem->ty, lo, hi, offset + mem->offset)) // 递归调用has_flonum函数，检查成员类型是否含有浮点数，并且满足偏移量要求
        return false;
    return true; // 如果所有成员都含有浮点数，并且满足偏移量要求，则返回true
  }

  if (ty->kind == TY_ARRAY) { // 如果类型是数组
    for (int i = 0; i < ty->array_len; i++) // 遍历数组的元素
      if (!has_flonum(ty->base, lo, hi, offset + ty->base->size * i)) // 递归调用has_flonum函数，检查数组元素类型是否含有浮点数，并且满足偏移量要求
        return false;
    return true; // 如果所有数组元素都含有浮点数，并且满足偏移量要求，则返回true
  }

  return offset < lo || hi <= offset || ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE; // 如果偏移量小于下界lo或大于等于上界hi，或者类型是float或double，则返回true
}

static bool has_flonum1(Type *ty) {
  return has_flonum(ty, 0, 8, 0); // 调用has_flonum函数，检查类型ty中偏移量范围为0-7的成员是否含有浮点数
}

static bool has_flonum2(Type *ty) {
  return has_flonum(ty, 8, 16, 0); // 调用has_flonum函数，检查类型ty中偏移量范围为8-15的成员是否含有浮点数
}

static void push_struct(Type *ty) {
  int sz = align_to(ty->size, 8); // 计算结构体大小，并按8字节对齐
  println("  sub $%d, %%rsp", sz); // 在栈上为结构体分配空间
  depth += sz / 8; // 更新栈的深度

  for (int i = 0; i < ty->size; i++) {
    println("  mov %d(%%rax), %%r10b", i); // 从寄存器rax中读取结构体的成员值
    println("  mov %%r10b, %d(%%rsp)", i); // 将成员值存储到栈上
  }
}

static void push_args2(Node *args, bool first_pass) {
  if (!args)
    return; // 如果参数列表为空，则直接返回
  push_args2(args->next, first_pass); // 递归调用push_args2函数，处理下一个参数

  if ((first_pass && !args->pass_by_stack) || (!first_pass && args->pass_by_stack))
    return; // 如果是第一次遍历并且参数不通过栈传递，或者不是第一次遍历并且参数通过栈传递，则直接返回

  gen_expr(args); // 生成参数表达式的代码

  switch (args->ty->kind) {
  case TY_STRUCT:
  case TY_UNION:
    push_struct(args->ty); // 如果参数类型是结构体或联合体，则调用push_struct函数将其推入栈中
    break;
  case TY_FLOAT:
  case TY_DOUBLE:
    pushf(); // 如果参数类型是float或double，则调用pushf函数将其推入栈中
    break;
  case TY_LDOUBLE:
    println("  sub $16, %%rsp"); // 在栈上为long double类型的参数分配空间
    println("  fstpt (%%rsp)"); // 将long double值存储到栈上
    depth += 2; // 更新栈的深度
    break;
  default:
    push(); // 否则，调用push函数将参数推入栈中
  }
}

// 加载函数调用实参。实参已经被计算并作为局部变量存储在栈中。
// 在这个函数中，我们需要将它们加载到寄存器中，或按照x86-64 psABI的规定将它们推送到栈中。
// 下面是规范中的说明：
//
// - 最多可以使用RDI、RSI、RDX、RCX、R8和R9寄存器传递6个整型实参。
//
// - 最多可以使用XMM0到XMM7寄存器传递8个浮点型实参。
//
// - 如果所有适当类型的寄存器都已经被使用，按照从右到左的顺序将实参推送到栈中。
//
// - 在栈上传递的每个实参占用8个字节，并且实参区域的末尾必须对齐到16字节边界。
//
// - 如果函数是可变实参的，将浮点型实参的数量设置为RAX。
static int push_args(Node *node) {
  int stack = 0, gp = 0, fp = 0;  // 栈、通用寄存器、浮点寄存器的数量初始化为0
  // 如果返回类型是一个大结构体/联合体，调用者将一个指向缓冲区的指针作为第一个实参传递。
  if (node->ret_buffer && node->ty->size > 16)
    gp++;  // 如果返回类型是大结构体/联合体且大小超过16字节，则通用寄存器数量加1
  // 尽可能将实参加载到寄存器中。
  for (Node *arg = node->args; arg; arg = arg->next) {
    Type *ty = arg->ty;  // 遍历参数列表，将每个参数的类型保存在ty变量中

    switch (ty->kind) {
    // 根据ty的类型进行分支判断
    case TY_STRUCT:
    case TY_UNION:
      // 如果ty的大小大于16
      if (ty->size > 16) {
        arg->pass_by_stack = true;  // 设置参数通过栈传递
        stack += align_to(ty->size, 8) / 8;  // 将栈指针增加ty大小对齐到8的值除以8
      } else {
        // 检查ty是否包含浮点数类型
        bool fp1 = has_flonum1(ty);  // 检查ty是否包含浮点数类型1
        bool fp2 = has_flonum2(ty);  // 检查ty是否包含浮点数类型2

        // 如果浮点寄存器的使用数量加上ty中浮点数类型的数量小于FP_MAX
        // 并且通用寄存器的使用数量加上ty中非浮点数类型的数量小于GP_MAX
        if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) {
          fp = fp + fp1 + fp2;  // 增加浮点寄存器使用数量
          gp = gp + !fp1 + !fp2;  // 增加通用寄存器使用数量
        } else {
          arg->pass_by_stack = true;  // 设置参数通过栈传递
          stack += align_to(ty->size, 8) / 8;  // 将栈指针增加ty大小对齐到8的值除以8
        }
      }
      break;

    case TY_FLOAT:
    case TY_DOUBLE:
      // 根据ty的类型进行分支判断，处理浮点数类型
      if (fp++ >= FP_MAX) {
        arg->pass_by_stack = true;  // 设置参数通过栈传递
        stack++;  // 栈指针增加1
      }
      break;
    case TY_LDOUBLE:
      // 根据ty的类型进行分支判断，处理长双精度浮点数类型
      arg->pass_by_stack = true;  // 设置参数通过栈传递
      stack += 2;  // 栈指针增加2
      break;
    default:
      // 根据ty的类型进行分支判断，处理其他类型
      if (gp++ >= GP_MAX) {
        arg->pass_by_stack = true;  // 设置参数通过栈传递
        stack++;  // 栈指针增加1
      }
    }
  }

  // 如果栈的深度和栈指针之和为奇数
  if ((depth + stack) % 2 == 1) {
    println("  sub $8, %%rsp");  // 在栈上分配8字节的空间
    depth++;  // 栈的深度增加1
    stack++;  // 栈指针增加1
  }

  // 将参数压入栈中（从右到左）
  push_args2(node->args, true);
  push_args2(node->args, false);

  // 如果返回类型是一个大的结构体/联合体，调用者将传递一个指向缓冲区的指针，就像它是第一个实参一样。
  // 如果返回缓冲区不为空且返回类型大小大于16
  if (node->ret_buffer && node->ty->size > 16) {
    println("  lea %d(%%rbp), %%rax", node->ret_buffer->offset);  // 将返回缓冲区的偏移量加载到 %rax 寄存器
    push();  // 将 %rax 寄存器的值压入栈中
  }

  return stack;  // 返回栈的大小
}

static void copy_ret_buffer(Obj *var) {
  Type *ty = var->ty;  // 获取变量的类型
  int gp = 0, fp = 0;  // 通用寄存器和浮点寄存器的使用数量

  // 如果类型包含浮点数
  if (has_flonum1(ty)) {
    assert(ty->size == 4 || 8 <= ty->size);  // 断言类型大小为4或大于等于8
    if (ty->size == 4)
      println("  movss %%xmm0, %d(%%rbp)", var->offset);  // 将 xmm0 寄存器的值移动到指定偏移量处
    else
      println("  movsd %%xmm0, %d(%%rbp)", var->offset);  // 将 xmm0 寄存器的值移动到指定偏移量处
    fp++;  // 浮点寄存器使用数量加1
  } else {
    for (int i = 0; i < MIN(8, ty->size); i++) {
      println("  mov %%al, %d(%%rbp)", var->offset + i);  // 将 al 寄存器的值移动到指定偏移量处
      println("  shr $8, %%rax");  // 右移 %rax 寄存器的值8位
    }
    gp++;  // 通用寄存器使用数量加1
  }

  if (ty->size > 8) {
    // 如果类型大小大于8
    if (has_flonum2(ty)) {
      // 如果类型包含浮点数
      assert(ty->size == 12 || ty->size == 16);  // 断言类型大小为12或16
      if (ty->size == 12)
        println("  movss %%xmm%d, %d(%%rbp)", fp, var->offset + 8);  // 将 xmm 寄存器的值移动到指定偏移量处
      else
        println("  movsd %%xmm%d, %d(%%rbp)", fp, var->offset + 8);  // 将 xmm 寄存器的值移动到指定偏移量处
    } else {
      // 如果类型不包含浮点数
      char *reg1 = (gp == 0) ? "%al" : "%dl";  // 根据通用寄存器使用数量选择寄存器
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";  // 根据通用寄存器使用数量选择寄存器
      for (int i = 8; i < MIN(16, ty->size); i++) {
        // 将寄存器的值移动到指定偏移量处
        println("  mov %s, %d(%%rbp)", reg1, var->offset + i);
        // 右移寄存器的值8位
        println("  shr $8, %s", reg2);
      }
    }
  }
}

static void copy_struct_reg(void) {
  Type *ty = current_fn->ty->return_ty;  // 获取当前函数的返回类型
  int gp = 0, fp = 0;  // 初始化通用寄存器和浮点寄存器的计数器

  println("  mov %%rax, %%rdi");  // 将 %%rax 寄存器的值移动到 %%rdi 寄存器

  if (has_flonum(ty, 0, 8, 0)) {
    // 如果类型包含浮点数
    assert(ty->size == 4 || 8 <= ty->size);  // 断言类型大小为4或大于等于8
    if (ty->size == 4)
      println("  movss (%%rdi), %%xmm0");  // 将指定地址处的值移动到 %%xmm0 浮点寄存器
    else
      println("  movsd (%%rdi), %%xmm0");  // 将指定地址处的值移动到 %%xmm0 浮点寄存器
    fp++;  // 浮点寄存器计数器加1
  } else {
    // 如果类型不包含浮点数
    println("  mov $0, %%rax");  // 将0移动到 %%rax 寄存器
    for (int i = MIN(8, ty->size) - 1; i >= 0; i--) {
      // 从最高字节开始循环移动值到寄存器中
      println("  shl $8, %%rax");  // 将 %%rax 寄存器的值左移8位
      println("  mov %d(%%rdi), %%al", i);  // 将指定地址处的值移动到 %%al 寄存器中
    }
    gp++;  // 通用寄存器计数器加1
  }

  if (ty->size > 8) {
    // 如果类型大小大于8
    if (has_flonum(ty, 8, 16, 0)) {
      // 如果类型包含浮点数
      assert(ty->size == 12 || ty->size == 16);  // 断言类型大小为12或16
      if (ty->size == 4)
        println("  movss 8(%%rdi), %%xmm%d", fp);  // 将指定地址处的值移动到 %%xmm 寄存器中
      else
        println("  movsd 8(%%rdi), %%xmm%d", fp);  // 将指定地址处的值移动到 %%xmm 寄存器中
    } else {
      // 如果类型不包含浮点数
      char *reg1 = (gp == 0) ? "%al" : "%dl";  // 根据通用寄存器使用数量选择寄存器
      char *reg2 = (gp == 0) ? "%rax" : "%rdx";  // 根据通用寄存器使用数量选择寄存器
      println("  mov $0, %s", reg2);  // 将0移动到指定寄存器
      for (int i = MIN(16, ty->size) - 1; i >= 8; i--) {
        // 从最高字节开始循环移动值到寄存器中
        println("  shl $8, %s", reg2);  // 将指定寄存器的值左移8位
        println("  mov %d(%%rdi), %s", i, reg1);  // 将指定地址处的值移动到指定寄存器中
      }
    }
  }
}

static void copy_struct_mem(void) {
  // 静态函数，用于复制结构体到内存中
  Type *ty = current_fn->ty->return_ty;  // 获取当前函数的返回类型
  Obj *var = current_fn->params;  // 获取当前函数的参数

  println("  mov %d(%%rbp), %%rdi", var->offset);  // 将指定地址处的值移动到 %%rdi 寄存器

  for (int i = 0; i < ty->size; i++) {
    // 循环，从0到结构体的大小
    println("  mov %d(%%rax), %%dl", i);  // 将指定地址处的值移动到 %%dl 寄存器
    println("  mov %%dl, %d(%%rdi)", i);  // 将 %%dl 寄存器的值移动到指定地址处
  }
}

static void builtin_alloca(void) {

  // 内置函数 alloca，用于在栈上分配内存
  // 将大小对齐到 16 字节
  println("  add $15, %%rdi");  // 将 rdi 寄存器的值加上 15
  println("  and $0xfffffff0, %%edi");  // 将 edi 寄存器的值与 0xfffffff0 进行按位与操作
  // 将临时区域向下偏移 rdi
  println("  mov %d(%%rbp), %%rcx", current_fn->alloca_bottom->offset);  // 将当前函数 alloca_bottom 成员的偏移量移动到 rcx 寄存器
  println("  sub %%rsp, %%rcx");  // 将 rsp 寄存器的值减去 rcx 寄存器的值，并将结果移动到 rcx 寄存器
  println("  mov %%rsp, %%rax");  // 将 rsp 寄存器的值移动到 rax 寄存器
  println("  sub %%rdi, %%rsp");  // 将 rdi 寄存器的值减去 rsp 寄存器的值，并将结果移动到 rsp 寄存器
  println("  mov %%rsp, %%rdx");  // 将 rsp 寄存器的值移动到 rdx 寄存器
  println("1:");  // 标签 1
  println("  cmp $0, %%rcx");  // 将 rcx 寄存器的值与 0 进行比较
  println("  je 2f");  // 如果 rcx 寄存器的值等于 0，则跳转到标签 2
  println("  mov (%%rax), %%r8b");  // 将 rax 寄存器指向的内存中的字节值移动到 r8b 寄存器
  println("  mov %%r8b, (%%rdx)");  // 将 r8b 寄存器的值移动到 rdx 寄存器指向的内存中
  println("  inc %%rdx");  // rdx 寄存器的值加 1
  println("  inc %%rax");  // rax 寄存器的值加 1
  println("  dec %%rcx");  // rcx 寄存器的值减 1
  println("  jmp 1b");  // 无条件跳转到标签 1
  println("2:");  // 标签 2
  // 移动 alloca_bottom 指针
  println("  mov %d(%%rbp), %%rax", current_fn->alloca_bottom->offset);  // 将当前函数 alloca_bottom 成员的偏移量移动到 rax 寄存器
  println("  sub %%rdi, %%rax");  // 将 rdi 寄存器的值减去 rax 寄存器的值，并将结果移动到 rax 寄存器
  println("  mov %%rax, %d(%%rbp)", current_fn->alloca_bottom->offset);  // 将 rax 寄存器的值移动到当前函数 alloca_bottom 成员的偏移量处
}

// 为给定的节点生成代码
static void gen_expr(Node *node) {
  println("  .loc %d %d", node->tok->file->file_no, node->tok->line_no);  // 打印当前节点的源代码位置信息

  switch (node->kind) {
  case ND_NULL_EXPR:  // 如果是空表达式，直接返回
    return;
  case ND_NUM: {
    switch (node->ty->kind) {
    case TY_FLOAT: {  // 如果是浮点数类型
      union { float f32; uint32_t u32; } u = { node->fval };
      println("  mov $%u, %%eax  # float %Lf", u.u32, node->fval);  // 将浮点数的值加载到寄存器eax中
      println("  movq %%rax, %%xmm0");  // 将eax中的值传递给xmm0寄存器
      return;
    }
    case TY_DOUBLE: {  // 如果是双精度浮点数类型
      union { double f64; uint64_t u64; } u = { node->fval };
      println("  mov $%lu, %%rax  # double %Lf", u.u64, node->fval);  // 将双精度浮点数的值加载到寄存器rax中
      println("  movq %%rax, %%xmm0");  // 将rax中的值传递给xmm0寄存器
      return;
    }
    case TY_LDOUBLE: {  // 如果是长双精度浮点数类型
      union { long double f80; uint64_t u64[2]; } u;
      memset(&u, 0, sizeof(u));
      u.f80 = node->fval;
      println("  mov $%lu, %%rax  # long double %Lf", u.u64[0], node->fval);  // 将长双精度浮点数的高位部分加载到寄存器rax中
      println("  mov %%rax, -16(%%rsp)");  // 将rax中的值存储到栈顶-16的位置
      println("  mov $%lu, %%rax", u.u64[1]);  // 将长双精度浮点数的低位部分加载到寄存器rax中
      println("  mov %%rax, -8(%%rsp)");  // 将rax中的值存储到栈顶-8的位置
      println("  fldt -16(%%rsp)");  // 将栈顶-16的位置的值加载到FPU寄存器栈中
      return;
    }
    }

    println("  mov $%ld, %%rax", node->val);  // 将整数的值加载到寄存器rax中
    return;
  }

  case ND_NEG:  // 处理取负操作
    gen_expr(node->lhs);  // 生成左操作数的表达式

    switch (node->ty->kind) {  // 根据节点类型的种类进行处理
    case TY_FLOAT:  // 浮点类型
      println("  mov $1, %%rax");  // 将1移到rax寄存器
      println("  shl $31, %%rax");  // 左移31位
      println("  movq %%rax, %%xmm1");  // 将rax的值移到xmm1寄存器
      println("  xorps %%xmm1, %%xmm0");  // xmm1和xmm0进行异或操作
      return;
    case TY_DOUBLE:  // 双精度浮点类型
      println("  mov $1, %%rax");  // 将1移到rax寄存器
      println("  shl $63, %%rax");  // 左移63位
      println("  movq %%rax, %%xmm1");  // 将rax的值移到xmm1寄存器
      println("  xorpd %%xmm1, %%xmm0");  // xmm1和xmm0进行异或操作
      return;
    case TY_LDOUBLE:  // 长双精度浮点类型
      println("  fchs");  // 调用fchs指令取反
      return;
    }

    println("  neg %%rax");  // rax取负
    return;
  case ND_VAR:  // 处理变量
    gen_addr(node);  // 生成变量的地址
    load(node->ty);  // 加载变量的类型
    return;
  case ND_MEMBER: {  // 处理成员
    gen_addr(node);  // 生成成员的地址
    load(node->ty);  // 加载成员的类型

    Member *mem = node->member;  // 获取成员
    if (mem->is_bitfield) {  // 如果是位字段
      println("  shl $%d, %%rax", 64 - mem->bit_width - mem->bit_offset);  // 左移
      if (mem->ty->is_unsigned)  // 如果是无符号类型
        println("  shr $%d, %%rax", 64 - mem->bit_width);  // 右移
      else
        println("  sar $%d, %%rax", 64 - mem->bit_width);  // 算术右移
    }
    return;
  }

  case ND_DEREF:     // 当节点类型为解引用时
    gen_expr(node->lhs);     // 生成左子树的代码
    load(node->ty);     // 加载节点的类型
    return;     // 返回

  case ND_ADDR:     // 当节点类型为取地址时
    gen_addr(node->lhs);     // 生成左子树的地址代码
    return;     // 返回

  case ND_ASSIGN:     // 当节点类型为赋值时
    gen_addr(node->lhs);     // 生成左子树的地址代码
    push();     // 将左子树的地址压栈
    gen_expr(node->rhs);     // 生成右子树的代码

    if (node->lhs->kind == ND_MEMBER && node->lhs->member->is_bitfield) {
      println("  mov %%rax, %%r8");     // 将rax寄存器的值移动到r8寄存器

      // 如果左子树是位域，则需要从内存中读取当前值并与新值合并。
      Member *mem = node->lhs->member;
      println("  mov %%rax, %%rdi");     // 将rax寄存器的值移动到rdi寄存器
      println("  and $%ld, %%rdi", (1L << mem->bit_width) - 1);     // 对rdi寄存器进行与运算
      println("  shl $%d, %%rdi", mem->bit_offset);     // 对rdi寄存器进行左移运算

      println("  mov (%%rsp), %%rax");     // 将栈顶的值移动到rax寄存器
      load(mem->ty);     // 加载位域的类型

      long mask = ((1L << mem->bit_width) - 1) << mem->bit_offset;     // 创建掩码
      println("  mov $%ld, %%r9", ~mask);     // 将掩码的补码移动到r9寄存器
      println("  and %%r9, %%rax");     // 对rax寄存器进行与运算
      println("  or %%rdi, %%rax");     // 对rax寄存器进行或运算
      store(node->ty);     // 存储结果到内存
      println("  mov %%r8, %%rax");     // 将r8寄存器的值移动到rax寄存器
      return;     // 返回
    }

    store(node->ty);     // 存储结果到内存
    return;     // 返回

  case ND_STMT_EXPR:     // 当节点类型为语句表达式时
    for (Node *n = node->body; n; n = n->next)     // 遍历语句表达式的每个语句
      gen_stmt(n);     // 生成语句的代码
    return;     // 返回

  case ND_COMMA:     // 当节点类型为逗号表达式时
    gen_expr(node->lhs);     // 生成左子树的代码
    gen_expr(node->rhs);     // 生成右子树的代码
    return;     // 返回

  case ND_CAST:     // 当节点类型为类型转换时
    gen_expr(node->lhs);     // 生成左子树的代码
    cast(node->lhs->ty, node->ty);     // 执行类型转换操作
    return;     // 返回

  case ND_MEMZERO:     // 当节点类型为内存清零时
    // rep stosb 相当于 memset(%rdi, %al, %rcx)。
    println("  mov $%d, %%rcx", node->var->ty->size);     // 将变量的大小移动到rcx寄存器
    println("  lea %d(%%rbp), %%rdi", node->var->offset);     // 计算变量的地址并移动到rdi寄存器
    println("  mov $0, %%al");     // 将0移动到al寄存器
    println("  rep stosb");     // 重复执行stosb指令，相当于memset(%rdi, %al, %rcx)
    return;     // 返回

  case ND_COND: {
    // 处理条件表达式
    int c = count();  // 计数器，用于生成唯一标识符
    gen_expr(node->cond);  // 生成条件表达式的代码
    cmp_zero(node->cond->ty);  // 将条件表达式的结果与零进行比较
    println("  je .L.else.%d", c);  // 如果条件为假，跳转到else分支
    gen_expr(node->then);  // 生成then分支的代码
    println("  jmp .L.end.%d", c);  // 跳转到条件表达式结束
    println(".L.else.%d:", c);  // else分支的标签
    gen_expr(node->els);  // 生成else分支的代码
    println(".L.end.%d:", c);  // 条件表达式结束的标签
    return;
  }
  case ND_NOT:
    // 逻辑非操作
    gen_expr(node->lhs);  // 生成操作数的代码
    cmp_zero(node->lhs->ty);  // 将操作数与零进行比较
    println("  sete %%al");  // 将比较结果设置到al寄存器
    println("  movzx %%al, %%rax");  // 将al寄存器的值扩展到rax寄存器
    return;
  case ND_BITNOT:
    // 按位取反操作
    gen_expr(node->lhs);  // 生成操作数的代码
    println("  not %%rax");  // 对rax寄存器的值进行按位取反
    return;
  case ND_LOGAND: {
    // 逻辑与操作
    int c = count();  // 计数器，用于生成唯一标识符
    gen_expr(node->lhs);  // 生成左操作数的代码
    cmp_zero(node->lhs->ty);  // 将左操作数与零进行比较
    println("  je .L.false.%d", c);  // 如果左操作数为假，跳转到false分支
    gen_expr(node->rhs);  // 生成右操作数的代码
    cmp_zero(node->rhs->ty);  // 将右操作数与零进行比较
    println("  je .L.false.%d", c);  // 如果右操作数为假，跳转到false分支
    println("  mov $1, %%rax");  // 将1赋值给rax寄存器，表示逻辑与的结果为真
    println("  jmp .L.end.%d", c);  // 跳转到结束标签
    println(".L.false.%d:", c);  // false分支的标签
    println("  mov $0, %%rax");  // 将0赋值给rax寄存器，表示逻辑与的结果为假
    println(".L.end.%d:", c);  // 结束标签
    return;
  }

  case ND_LOGOR: {
    int c = count();    // 定义变量c并赋值为count()的返回值
    gen_expr(node->lhs);    // 生成左操作数的表达式代码
    cmp_zero(node->lhs->ty);    // 将左操作数与0进行比较
    println("  jne .L.true.%d", c);    // 如果左操作数不等于0，则跳转到标号.L.true.c处
    gen_expr(node->rhs);    // 生成右操作数的表达式代码
    cmp_zero(node->rhs->ty);    // 将右操作数与0进行比较
    println("  jne .L.true.%d", c);    // 如果右操作数不等于0，则跳转到标号.L.true.c处
    println("  mov $0, %%rax");    // 将寄存器rax的值设为0
    println("  jmp .L.end.%d", c);    // 无条件跳转到标号.L.end.c处
    println(".L.true.%d:", c);    // 标号.L.true.c处的代码
    println("  mov $1, %%rax");    // 将寄存器rax的值设为1
    println(".L.end.%d:", c);    // 标号.L.end.c处的代码
    return;
  }

  case ND_FUNCALL: {
    if (node->lhs->kind == ND_VAR && !strcmp(node->lhs->var->name, "alloca")) {    // 如果函数调用是alloca函数
      gen_expr(node->args);    // 生成参数的表达式代码
      println("  mov %%rax, %%rdi");    // 将寄存器rax的值移动到寄存器rdi
      builtin_alloca();    // 调用内置的alloca函数
      return;
    }

    int stack_args = push_args(node);    // 将参数压入栈中，并返回参数的个数
    gen_expr(node->lhs);    // 生成函数调用的表达式代码

    int gp = 0, fp = 0;    // 定义变量gp和fp，并初始化为0
    
    // 如果返回类型是一个大的结构体/联合体，调用者将传递一个指向缓冲区的指针，就好像它是第一个实参一样。
    // 检查是否存在返回缓冲区，并且返回类型的大小大于16
    if (node->ret_buffer && node->ty->size > 16)
      pop(argreg64[gp++]); // 从argreg64数组中弹出一个元素，并将gp自增

    // 遍历参数链表，从第一个参数开始，直到参数为空
    for (Node *arg = node->args; arg; arg = arg->next) {
      Type *ty = arg->ty; // 获取当前参数的类型

      switch (ty->kind) { // 根据ty的kind进行条件判断
      case TY_STRUCT: // 如果kind为TY_STRUCT
      case TY_UNION: // 或者kind为TY_UNION
        if (ty->size > 16) // 如果ty的size大于16
          continue; // 跳过当前循环的迭代

        bool fp1 = has_flonum1(ty); // 定义bool型变量fp1并调用has_flonum1函数赋值
        bool fp2 = has_flonum2(ty); // 定义bool型变量fp2并调用has_flonum2函数赋值

        if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) { // 如果fp + fp1 + fp2小于FP_MAX且gp + !fp1 + !fp2小于GP_MAX
          if (fp1) // 如果fp1为true
            popf(fp++); // 弹出浮点数并将fp自增
          else // 否则
            pop(argreg64[gp++]); // 弹出argreg64数组中的元素并将gp自增

          if (ty->size > 8) { // 如果ty的size大于8
            if (fp2) // 如果fp2为true
              popf(fp++); // 弹出浮点数并将fp自增
            else // 否则
              pop(argreg64[gp++]); // 弹出argreg64数组中的元素并将gp自增
          }
        }
        break; // 结束switch语句
      case TY_FLOAT: // 如果kind为TY_FLOAT
      case TY_DOUBLE: // 或者kind为TY_DOUBLE
        if (fp < FP_MAX) // 如果fp小于FP_MAX
          popf(fp++); // 弹出浮点数并将fp自增
        break; // 结束switch语句
      case TY_LDOUBLE: // 如果kind为TY_LDOUBLE
        break; // 结束switch语句
      default: // 默认情况
        if (gp < GP_MAX) // 如果gp小于GP_MAX
          pop(argreg64[gp++]); // 弹出argreg64数组中的元素并将gp自增
      }
    }

    println("  mov %%rax, %%r10");  // 将寄存器 %%rax 的值移动到寄存器 %%r10
    println("  mov $%d, %%rax", fp);  // 将立即数 fp 移动到寄存器 %%rax
    println("  call *%%r10");  // 调用 %%r10 寄存器指向的函数
    println("  add $%d, %%rsp", stack_args * 8);  // 将立即数 stack_args * 8 加到寄存器 %%rsp
    depth -= stack_args;  // 将 depth 减去 stack_args

    // 如果函数返回类型分别为short 或bool/char，
    // 那么RAX 中最高有效的48 或56 位可能包含垃圾。 我们在这里清除高位/上位位。
    switch (node->ty->kind) {                 // 根据节点类型进行选择
    case TY_BOOL:                            // 如果类型是布尔类型
      println("  movzx %%al, %%eax");        // 将低位寄存器al的值零扩展到eax寄存器，并打印结果
      return;                                // 返回
    case TY_CHAR:                            // 如果类型是字符类型
      if (node->ty->is_unsigned)              // 如果是无符号字符
        println("  movzbl %%al, %%eax");     // 将低位寄存器al的值零扩展到eax寄存器，并打印结果
      else                                   // 否则
        println("  movsbl %%al, %%eax");     // 将低位寄存器al的值符号扩展到eax寄存器，并打印结果
      return;                                // 返回
    case TY_SHORT:                           // 如果类型是短整型
      if (node->ty->is_unsigned)              // 如果是无符号短整型
        println("  movzwl %%ax, %%eax");     // 将低位寄存器ax的值零扩展到eax寄存器，并打印结果
      else                                   // 否则
        println("  movswl %%ax, %%eax");     // 将低位寄存器ax的值符号扩展到eax寄存器，并打印结果
      return;                                // 返回
    }

    // 如果返回类型是一个小结构体，则使用最多两个寄存器返回一个值。
    if (node->ret_buffer && node->ty->size <= 16) {  // 检查是否有返回缓冲区并且返回类型大小不超过16字节
      copy_ret_buffer(node->ret_buffer);  // 复制返回缓冲区内容
      println("  lea %d(%%rbp), %%rax", node->ret_buffer->offset);  // 打印将返回缓冲区的偏移量加载到rax寄存器的指令
    }

    return;  // 返回空值
  }

  case ND_LABEL_VAL:
    // 标签值情况：将带有地址偏移的标签值加载到寄存器rax
    println("  lea %s(%%rip), %%rax", node->unique_label);
    return;
  case ND_CAS: {
    // CAS操作情况
    gen_expr(node->cas_addr); // 生成CAS地址的代码
    push(); // 将CAS地址压入栈顶
    gen_expr(node->cas_new); // 生成CAS新值的代码
    push(); // 将CAS新值压入栈顶
    gen_expr(node->cas_old); // 生成CAS旧值的代码
    println("  mov %%rax, %%r8"); // 将rax中的值存入r8寄存器
    load(node->cas_old->ty->base); // 载入CAS旧值的类型
    pop("%rdx"); // 弹出栈顶的值到rdx寄存器（新值）
    pop("%rdi"); // 弹出栈顶的值到rdi寄存器（地址）

    int sz = node->cas_addr->ty->base->size; // 获取CAS地址的大小
    println("  lock cmpxchg %s, (%%rdi)", reg_dx(sz)); // 使用cmpxchg指令进行原子比较和交换
    println("  sete %%cl"); // 设置cl寄存器为比较结果
    println("  je 1f"); // 如果比较结果为相等，则跳转到标签1处
    println("  mov %s, (%%r8)", reg_ax(sz)); // 将rax的值存入r8指向的内存地址
    println("1:"); // 标签1，用于跳转
    println("  movzbl %%cl, %%eax"); // 将cl寄存器的值移动到eax寄存器的低位
    return;
  }
  case ND_EXCH: {
    // 交换操作情况
    gen_expr(node->lhs); // 生成左操作数的代码
    push(); // 将左操作数压入栈顶
    gen_expr(node->rhs); // 生成右操作数的代码
    pop("%rdi"); // 弹出栈顶的值到rdi寄存器（右操作数）

    int sz = node->lhs->ty->base->size; // 获取左操作数的大小
    println("  xchg %s, (%%rdi)", reg_ax(sz)); // 使用xchg指令进行交换
    return;
  }
  }

  switch (node->lhs->ty->kind) {  // 根据左操作数的类型进行切换
  case TY_FLOAT:  // 如果左操作数类型为浮点数
  case TY_DOUBLE: {  // 或者双精度浮点数
      gen_expr(node->rhs);  // 生成右操作数的代码
      pushf();  // 将浮点寄存器栈顶的值压入栈中
      gen_expr(node->lhs);  // 生成左操作数的代码
      popf(1);  // 弹出栈顶的值到浮点寄存器

      char *sz = (node->lhs->ty->kind == TY_FLOAT) ? "ss" : "sd";  // 根据左操作数的类型选择指令后缀

      switch (node->kind) {  // 根据节点的类型进行切换
      case ND_ADD:  // 如果是加法操作
          println("  add%s %%xmm1, %%xmm0", sz);  // 执行浮点数加法并将结果存储在xmm0寄存器中
          return;
      case ND_SUB:  // 如果是减法操作
          println("  sub%s %%xmm1, %%xmm0", sz);  // 执行浮点数减法并将结果存储在xmm0寄存器中
          return;
      case ND_MUL:  // 如果是乘法操作
          println("  mul%s %%xmm1, %%xmm0", sz);  // 执行浮点数乘法并将结果存储在xmm0寄存器中
          return;
      case ND_DIV:  // 如果是除法操作
          println("  div%s %%xmm1, %%xmm0", sz);  // 执行浮点数除法并将结果存储在xmm0寄存器中
          return;
      case ND_EQ:  // 如果是等于操作
      case ND_NE:  // 如果是不等于操作
      case ND_LT:  // 如果是小于操作
      case ND_LE:  // 如果是小于等于操作
          println("  ucomi%s %%xmm0, %%xmm1", sz);  // 比较两个浮点数并设置标志位

          if (node->kind == ND_EQ) {  // 如果是等于操作
              println("  sete %%al");  // 设置ZF标志位到AL寄存器
              println("  setnp %%dl");  // 设置PF标志位到DL寄存器
              println("  and %%dl, %%al");  // AL寄存器与DL寄存器进行与操作
          } else if (node->kind == ND_NE) {  // 如果是不等于操作
              println("  setne %%al");  // 设置ZF标志位的反义到AL寄存器
              println("  setp %%dl");  // 设置PF标志位到DL寄存器
              println("  or %%dl, %%al");  // AL寄存器与DL寄存器进行或操作
          } else if (node->kind == ND_LT) {  // 如果是小于操作
              println("  seta %%al");  // 设置CF和ZF都为0时，PF为1时，将AL寄存器设置为1
          } else {  // 如果是小于等于操作
              println("  setae %%al");  // 设置CF为0时，ZF和PF都为1时，将AL寄存器设置为1
          }

          println("  and $1, %%al");  // 将AL寄存器与1进行与操作，结果存储在AL寄存器中
          println("  movzb %%al, %%rax");  // 将AL寄存器的低8位移动到RAX寄存器中
          return;
      }

      error_tok(node->tok, "invalid expression");  // 如果不是以上操作，报错
    }
    
    case TY_LDOUBLE: {  // 如果节点类型是TY_LDOUBLE
      gen_expr(node->lhs);  // 生成左子表达式的代码
      gen_expr(node->rhs);  // 生成右子表达式的代码

      switch (node->kind) {  // 根据节点的操作类型进行处理
      case ND_ADD:  // 如果是加法
        println("  faddp");  // 打印"  faddp"，表示执行浮点加法
        return;
      case ND_SUB:  // 如果是减法
        println("  fsubrp");  // 打印"  fsubrp"，表示执行浮点减法
        return;
      case ND_MUL:  // 如果是乘法
        println("  fmulp");  // 打印"  fmulp"，表示执行浮点乘法
        return;
      case ND_DIV:  // 如果是除法
        println("  fdivrp");  // 打印"  fdivrp"，表示执行浮点除法
        return;
      case ND_EQ:  // 如果是等于
      case ND_NE:  // 如果是不等于
      case ND_LT:  // 如果是小于
      case ND_LE:  // 如果是小于等于
        println("  fcomip");  // 打印"  fcomip"，表示执行浮点比较并设置标志位
        println("  fstp %%st(0)");  // 打印"  fstp %%st(0)"，表示将栈顶元素弹出

        if (node->kind == ND_EQ)  // 如果是等于操作
          println("  sete %%al");  // 打印"  sete %%al"，根据ZF标志位设置AL寄存器为1或0
        else if (node->kind == ND_NE)  // 如果是不等于操作
          println("  setne %%al");  // 打印"  setne %%al"，根据ZF标志位设置AL寄存器为1或0
        else if (node->kind == ND_LT)  // 如果是小于操作
          println("  seta %%al");  // 打印"  seta %%al"，根据CF和ZF标志位设置AL寄存器为1或0
        else
          println("  setae %%al");  // 打印"  setae %%al"，根据CF标志位设置AL寄存器为1或0

        println("  movzb %%al, %%rax");  // 打印"  movzb %%al, %%rax"，将AL寄存器的值复制到RAX寄存器的低8位，并进行零扩展
        return;
      }

      error_tok(node->tok, "invalid expression");  // 如果无法识别节点的操作类型，打印错误消息
    }
    }

    gen_expr(node->rhs);    // 生成右子表达式的代码
    push();                 // 将右子表达式的结果推入栈中
    gen_expr(node->lhs);    // 生成左子表达式的代码
    pop("%rdi");            // 将栈顶元素弹出到寄存器"%rdi"

    char *ax, *di, *dx;     // 声明字符指针变量ax，di，dx

    if (node->lhs->ty->kind == TY_LONG || node->lhs->ty->base) {
      ax = "%rax";          // 如果左子表达式的类型为长整型或者基础类型，则将ax指向"%rax"
      di = "%rdi";          // 将di指向"%rdi"
      dx = "%rdx";          // 将dx指向"%rdx"
    } else {
      ax = "%eax";          // 如果左子表达式的类型不是长整型或者基础类型，则将ax指向"%eax"
      di = "%edi";          // 将di指向"%edi"
      dx = "%edx";          // 将dx指向"%edx"
    }

    switch (node->kind) {  // 开始一个 switch 语句，根据 node 的类型进行分支判断
    case ND_ADD:  // 如果 node 的类型为 ND_ADD
      println("  add %s, %s", di, ax);  // 打印字符串 "add %s, %s"，并将 di 和 ax 的值进行相加
      return;  // 结束当前函数的执行，返回调用该函数的位置
    case ND_SUB:  // 如果 node 的类型为 ND_SUB
      println("  sub %s, %s", di, ax);  // 打印字符串 "sub %s, %s"，并将 di 和 ax 的值进行相减
      return;  // 结束当前函数的执行，返回调用该函数的位置
    case ND_MUL:  // 如果 node 的类型为 ND_MUL
      println("  imul %s, %s", di, ax);  // 打印字符串 "imul %s, %s"，并将 di 和 ax 的值进行相乘
      return;  // 结束当前函数的执行，返回调用该函数的位置
    case ND_DIV:  // 如果 node 的类型为 ND_DIV
    case ND_MOD:  // 或者 node 的类型为 ND_MOD
      if (node->ty->is_unsigned) {  // 如果 node 的类型是无符号的
        println("  mov $0, %s", dx);  // 将 0 移动到寄存器 dx
        println("  div %s", di);  // 执行无符号除法操作，结果保存在寄存器 ax 中，商存储在 ax，余数存储在 dx
      } else {  // 否则，即为有符号类型
        if (node->lhs->ty->size == 8)
          println("  cqo");  // 扩展寄存器 ax 中的有符号值到寄存器 dx:ax 中
        else
          println("  cdq");  // 扩展寄存器 ax 中的有符号值到寄存器 dx:ax 中
        println("  idiv %s", di);  // 执行有符号除法操作，结果保存在寄存器 ax 中，商存储在 ax，余数存储在 dx
      }

      if (node->kind == ND_MOD)  // 如果 node 的类型为 ND_MOD
        println("  mov %%rdx, %%rax");  // 将寄存器 %%rdx 的值移动到寄存器 %%rax
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_BITAND:  // 如果 node 的类型为 ND_BITAND
      println("  and %s, %s", di, ax);  // 对 di 和 ax 进行按位与操作，并打印结果
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_BITOR:  // 如果 node 的类型为 ND_BITOR
      println("  or %s, %s", di, ax);  // 对 di 和 ax 进行按位或操作，并打印结果
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_BITXOR:  // 如果 node 的类型为 ND_BITXOR
      println("  xor %s, %s", di, ax);  // 对 di 和 ax 进行按位异或操作，并打印结果
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_EQ:  // 如果 node 的类型为 ND_EQ
    case ND_NE:  // 或者 node 的类型为 ND_NE
    case ND_LT:  // 或者 node 的类型为 ND_LT
    case ND_LE:  // 或者 node 的类型为 ND_LE
      println("  cmp %s, %s", di, ax);  // 比较 di 和 ax 的值，并设置相应的标志位

      if (node->kind == ND_EQ) {  // 如果 node 的类型为 ND_EQ
        println("  sete %%al");  // 如果相等，将标志位 ZF 设置为 1，否则为 0，并将结果保存在寄存器 %%al
      } else if (node->kind == ND_NE) {  // 如果 node 的类型为 ND_NE
        println("  setne %%al");  // 如果不相等，将标志位 ZF 设置为 0，否则为 1，并将结果保存在寄存器 %%al
      } else if (node->kind == ND_LT) {  // 如果 node 的类型为 ND_LT
        if (node->lhs->ty->is_unsigned)
          println("  setb %%al");  // 如果无符号类型，如果小于则将标志位 CF 设置为 1，否则为 0，并将结果保存在寄存器 %%al
        else
          println("  setl %%al");  // 如果有符号类型，如果小于则将标志位 SF 设置为 1，否则为 0，并将结果保存在寄存器 %%al
      } else if (node->kind == ND_LE) {  // 如果 node 的类型为 ND_LE
        if (node->lhs->ty->is_unsigned)
          println("  setbe %%al");  // 如果无符号类型，如果小于等于则将标志位 CF 或 ZF 设置为 1，否则为 0，并将结果保存在寄存器 %%al
        else
          println("  setle %%al");  // 如果有符号类型，如果小于等于则将标志位 SF 或 ZF 设置为 1，否则为 0，并将结果保存在寄存器 %%al
      }

      println("  movzb %%al, %%rax");  // 将寄存器 %%al 的值移动到寄存器 %%rax，并进行零扩展
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_SHL:  // 如果 node 的类型为 ND_SHL
      println("  mov %%rdi, %%rcx");  // 将寄存器 %%rdi 的值移动到寄存器 %%rcx
      println("  shl %%cl, %s", ax);  // 将寄存器 %%cl 的值作为位移数，对寄存器 ax 进行左移操作，并打印结果
      return;  // 结束当前函数的执行，返回调用该函数的位置

    case ND_SHR:  // 如果 node 的类型为 ND_SHR
      println("  mov %%rdi, %%rcx");  // 将寄存器 %%rdi 的值移动到寄存器 %%rcx
      if (node->lhs->ty->is_unsigned)
        println("  shr %%cl, %s", ax);  // 如果操作数为无符号类型，将寄存器 %%cl 的值作为位移数，对寄存器 ax 进行逻辑右移操作，并打印结果
      else
        println("  sar %%cl, %s", ax);  // 如果操作数为有符号类型，将寄存器 %%cl 的值作为位移数，对寄存器 ax 进行算术右移操作，并打印结果
      return;  // 结束当前函数的执行，返回调用该函数的位置
    }

    error_tok(node->tok, "invalid expression");  // 报告错误，无效的表达式
}

static void gen_stmt(Node *node) {
  // 打印当前行的文件位置信息
  println("  .loc %d %d", node->tok->file->file_no, node->tok->line_no);

  switch (node->kind) {
  case ND_IF: {
    int c = count();
    // 获取当前的计数器值
    gen_expr(node->cond);  // 生成条件表达式的代码
    cmp_zero(node->cond->ty);  // 将条件结果与零进行比较
    println("  je  .L.else.%d", c);  // 如果条件为假，跳转到else部分
    gen_stmt(node->then);  // 生成then部分的代码
    println("  jmp .L.end.%d", c);  // 跳转到end部分
    println(".L.else.%d:", c);  // 输出else标签
    if (node->els)
      gen_stmt(node->els);  // 如果有else部分，生成else部分的代码
    println(".L.end.%d:", c);  // 输出end标签
    return;
  }

  case ND_FOR: {
    // 对于 for 循环节点进行处理
    int c = count(); // 获取计数器 c
    if (node->init)
      gen_stmt(node->init); // 生成初始化语句
    println(".L.begin.%d:", c); // 打印标签 .L.begin.c
    if (node->cond) {
      gen_expr(node->cond); // 生成条件表达式
      cmp_zero(node->cond->ty); // 将条件表达式与零进行比较
      println("  je %s", node->brk_label); // 如果条件为假，则跳转到循环结束的标签
    }
    gen_stmt(node->then); // 生成循环体语句
    println("%s:", node->cont_label); // 打印循环继续的标签
    if (node->inc)
      gen_expr(node->inc); // 生成增量表达式
    println("  jmp .L.begin.%d", c); // 跳转到循环开始的标签
    println("%s:", node->brk_label); // 打印循环结束的标签
    return;
  }
  case ND_DO: {
    // 对于 do-while 循环节点进行处理
    int c = count(); // 获取计数器 c
    println(".L.begin.%d:", c); // 打印标签 .L.begin.c
    gen_stmt(node->then); // 生成循环体语句
    println("%s:", node->cont_label); // 打印循环继续的标签
    gen_expr(node->cond); // 生成条件表达式
    cmp_zero(node->cond->ty); // 将条件表达式与零进行比较
    println("  jne .L.begin.%d", c); // 如果条件为真，则跳转到循环开始的标签
    println("%s:", node->brk_label); // 打印循环结束的标签
    return;
  }

  case ND_SWITCH:
    // 对于 switch 语句节点进行处理
    gen_expr(node->cond); // 生成条件表达式

    for (Node *n = node->case_next; n; n = n->case_next) {
      char *ax = (node->cond->ty->size == 8) ? "%rax" : "%eax"; // 根据条件表达式的大小选择寄存器
      char *di = (node->cond->ty->size == 8) ? "%rdi" : "%edi"; // 根据条件表达式的大小选择寄存器

      if (n->begin == n->end) {
        println("  cmp $%ld, %s", n->begin, ax); // 将条件表达式与指定值进行比较
        println("  je %s", n->label); // 如果条件为相等，则跳转到相应的标签
        continue;
      }

      // [GNU] Case ranges
      println("  mov %s, %s", ax, di); // 将条件表达式的值移动到另一个寄存器
      println("  sub $%ld, %s", n->begin, di); // 将寄存器的值减去指定的值
      println("  cmp $%ld, %s", n->end - n->begin, di); // 将寄存器的值与指定的范围进行比较
      println("  jbe %s", n->label); // 如果在范围内，则跳转到相应的标签
    }

    if (node->default_case)
      println("  jmp %s", node->default_case->label); // 跳转到默认情况的标签

    println("  jmp %s", node->brk_label); // 跳转到循环结束的标签
    gen_stmt(node->then); // 生成 switch 语句的主体部分
    println("%s:", node->brk_label); // 打印循环结束的标签
    return;

  case ND_CASE:
    // 对于 case 语句节点进行处理
    println("%s:", node->label); // 打印 case 标签
    gen_stmt(node->lhs); // 生成 case 语句的主体部分
    return;
  case ND_BLOCK:
    // 对于代码块节点进行处理
    for (Node *n = node->body; n; n = n->next)
      gen_stmt(n); // 逐个生成代码块中的语句
    return;
  case ND_GOTO:
    // 对于 goto 语句节点进行处理
    println("  jmp %s", node->unique_label); // 跳转到目标标签
    return;
  case ND_GOTO_EXPR:
    // 对于带表达式的 goto 语句节点进行处理
    gen_expr(node->lhs); // 生成表达式
    println("  jmp *%%rax"); // 通过寄存器间接跳转
    return;
  case ND_LABEL:
    // 对于标签语句节点进行处理
    println("%s:", node->unique_label); // 打印标签
    gen_stmt(node->lhs); // 生成标签语句的主体部分
    return;

  case ND_RETURN: // 如果是返回语句
    if (node->lhs) { // 如果返回值不为空
      gen_expr(node->lhs); // 生成返回值的代码
      Type *ty = node->lhs->ty; // 获取返回值的类型

      switch (ty->kind) { // 根据类型进行处理
      case TY_STRUCT: // 结构体类型
      case TY_UNION: // 联合类型
        if (ty->size <= 16) // 如果大小小于等于 16 字节
          copy_struct_reg(); // 使用寄存器传递结构体
        else
          copy_struct_mem(); // 使用内存传递结构体
        break;
      }
    }

    println("  jmp .L.return.%s", current_fn->name); // 跳转到当前函数的返回标签
    return; // 返回
  case ND_EXPR_STMT: // 如果是表达式语句
    gen_expr(node->lhs); // 生成表达式的代码
    return; // 返回
  case ND_ASM: // 如果是内联汇编语句
    println("  %s", node->asm_str); // 打印汇编代码
    return; // 返回
  }

  error_tok(node->tok, "invalid statement"); // 报告错误，无效的语句
}

// 为本地变量赋值偏移量。
// Assign offsets to local variables.
static void assign_lvar_offsets(Obj *prog) {
  for (Obj *fn = prog; fn; fn = fn->next) { // 遍历函数列表
    if (!fn->is_function) // 如果当前对象不是函数
      continue; // 跳过当前对象

    // 如果一个函数有很多形参，一些形参必然通过栈而不是寄存器传递。
    // 第一个通过栈传递的形参位于 RBP+16。
    int top = 16; // 栈顶偏移量为 16
    int bottom = 0; // 栈底偏移量为 0

    int gp = 0, fp = 0; // 通用寄存器和浮点寄存器的偏移量

    // 为通过栈传递的形参分配偏移量
    for (Obj *var = fn->params; var; var = var->next) { // 遍历参数列表
      Type *ty = var->ty; // 获取参数的类型

      switch (ty->kind) { // 根据类型进行处理
      case TY_STRUCT: // 如果是结构体类型
      case TY_UNION: // 如果是联合类型
        if (ty->size <= 16) { // 如果大小小于等于 16 字节
          bool fp1 = has_flonum(ty, 0, 8, 0); // 检查是否包含浮点数成员1
          bool fp2 = has_flonum(ty, 8, 16, 8); // 检查是否包含浮点数成员2
          if (fp + fp1 + fp2 < FP_MAX && gp + !fp1 + !fp2 < GP_MAX) { // 如果浮点数寄存器和通用寄存器的使用数量未超过最大限制
            fp = fp + fp1 + fp2; // 更新浮点数寄存器的使用数量
            gp = gp + !fp1 + !fp2; // 更新通用寄存器的使用数量
            continue; // 继续下一次循环
          }
        }
        break;
      case TY_FLOAT: // 如果是浮点数类型
      case TY_DOUBLE: // 如果是双精度浮点数类型
        if (fp++ < FP_MAX) // 如果浮点数寄存器的使用数量未超过最大限制
          continue; // 继续下一次循环
        break;
      case TY_LDOUBLE: // 如果是长双精度浮点数类型
        break; // 跳出 switch
      default: // 其他情况
        if (gp++ < GP_MAX) // 如果通用寄存器的使用数量未超过最大限制
          continue; // 继续下一次循环
      }

      top = align_to(top, 8); // 将栈顶偏移量按 8 字节对齐
      var->offset = top; // 设置变量的偏移量
      top += var->ty->size; // 更新栈顶偏移量，加上变量的大小
    }

    // 为寄存器传递的形参和局部变量分配偏移量。
    for (Obj *var = fn->locals; var; var = var->next) { // 遍历局部变量列表
      if (var->offset) // 如果变量已经有偏移量，则跳过
        continue;

      // AMD64 System V ABI 对长度至少为 16 字节的数组有特殊的对齐规则。
      // 我们需要将这样的数组对齐到至少 16 字节的边界。
      // 参考：https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-draft.pdf 的第 14 页。
      int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)
        ? MAX(16, var->align) : var->align; // 计算对齐值

      bottom += var->ty->size; // 计算栈底偏移量
      bottom = align_to(bottom, align); // 将栈底偏移量按对齐值对齐
      var->offset = -bottom; // 设置变量的偏移量为负的栈底偏移量
    }

    fn->stack_size = align_to(bottom, 16); // 将栈底偏移量按 16 字节对齐，作为函数的栈大小
  }
}

static void emit_data(Obj *prog) {  // 定义静态函数emit_data，参数为指向Obj类型的指针prog
  for (Obj *var = prog; var; var = var->next) {  // 遍历prog链表，每次迭代将var指向下一个元素
    if (var->is_function || !var->is_definition)  // 如果var是函数或者不是定义
      continue;  // 跳过当前迭代，继续下一次迭代
    
    if (var->is_static)  // 如果var的is_static属性为真
      println("  .local %s", var->name);  // 打印字符串 ".local" 和 var的名称
    else
      println("  .globl %s", var->name);  // 打印字符串 ".globl" 和 var的名称

    int align = (var->ty->kind == TY_ARRAY && var->ty->size >= 16)  // 如果var的类型是数组且大小大于等于16
      ? MAX(16, var->align) : var->align;  // align等于16和var的align中的较大值，否则align等于var的align
    
    // 公共符号
    // 如果启用了-fcommon选项且var是不完全定义的
    if (opt_fcommon && var->is_tentative) {
      println("  .comm %s, %d, %d", var->name, var->ty->size, align);  // 打印格式化字符串 ".comm"、var的名称、var的大小和align的值
      continue;  // 继续下一次迭代
    }

    // .data or .tdata
    // 如果var具有初始化数据
    if (var->init_data) {
      if (var->is_tls)  // 如果var是线程本地存储
        println("  .section .tdata,\"awT\",@progbits");  // 打印字符串 ".section .tdata,\"awT\",@progbits"
      else
        println("  .data");  // 打印字符串 ".data"

      println("  .type %s, @object", var->name);  // 打印格式化字符串 ".type"、var的名称和字符串 "@object"
      println("  .size %s, %d", var->name, var->ty->size);  // 打印格式化字符串 ".size"、var的名称和var的大小
      println("  .align %d", align);  // 打印格式化字符串 ".align"和align的值
      println("%s:", var->name);  // 打印var的名称和冒号

      Relocation *rel = var->rel;  // 用rel指针指向var的rel成员
      int pos = 0;  // 初始化pos为0
      while (pos < var->ty->size) {  // 当pos小于var的大小时循环
        if (rel && rel->offset == pos) {  // 如果rel存在且rel的offset等于pos
          println("  .quad %s%+ld", *rel->label, rel->addend);  // 打印格式化字符串 ".quad"、rel的label指向的值和rel的addend值
          rel = rel->next;  // 将rel指向下一个节点
          pos += 8;  // 增加pos的值为8
        } else {
          println("  .byte %d", var->init_data[pos++]);  // 打印格式化字符串 ".byte"和var的init_data中的值，然后将pos增加1
        }
      }
      continue;  // 继续下一次迭代
    }
    
    // .bss or .tbss
    // 如果var是线程本地存储
    if (var->is_tls)
      println("  .section .tbss,\"awT\",@nobits");  // 打印字符串 ".section .tbss,\"awT\",@nobits"
    else
      println("  .bss");  // 打印字符串 ".bss"

    println("  .align %d", align);  // 打印格式化字符串 ".align"和align的值
    println("%s:", var->name);  // 打印var的名称和冒号
    println("  .zero %d", var->ty->size);  // 打印格式化字符串 ".zero"和var的ty的size的值
  }
}

static void store_fp(int r, int offset, int sz) {
  switch (sz) {
  case 4:
    println("  movss %%xmm%d, %d(%%rbp)", r, offset); // 将寄存器 xmm%d 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  case 8:
    println("  movsd %%xmm%d, %d(%%rbp)", r, offset); // 将寄存器 xmm%d 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  }
  unreachable(); // 不可达代码，如果sz的值既不是4也不是8，应该永远不会执行到这里
}

static void store_gp(int r, int offset, int sz) {
  switch (sz) {
  case 1:
    println("  mov %s, %d(%%rbp)", argreg8[r], offset); // 将寄存器 argreg8[r] 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  case 2:
    println("  mov %s, %d(%%rbp)", argreg16[r], offset); // 将寄存器 argreg16[r] 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  case 4:
    println("  mov %s, %d(%%rbp)", argreg32[r], offset); // 将寄存器 argreg32[r] 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  case 8:
    println("  mov %s, %d(%%rbp)", argreg64[r], offset); // 将寄存器 argreg64[r] 的值移动到偏移量为 %d(%%rbp) 的内存地址
    return;
  default:
    for (int i = 0; i < sz; i++) {
      println("  mov %s, %d(%%rbp)", argreg8[r], offset + i); // 将寄存器 argreg8[r] 的值移动到偏移量为 %d(%%rbp) 的内存地址
      println("  shr $8, %s", argreg64[r]); // 将寄存器 argreg64[r] 的值右移8位
    }
    return;
  }
}

static void emit_text(Obj *prog) {  // 定义静态函数 emit_text，参数为 prog
  for (Obj *fn = prog; fn; fn = fn->next) {  // 循环遍历 prog 中的每个对象，将当前对象赋值给 fn，直到 fn 为空
    if (!fn->is_function || !fn->is_definition)
      continue; // 如果不是函数或者不是函数定义，则跳过当前循环，进入下一次循环

    // 如果没有人引用"静态内联"函数，则不会发射任何代码。
    if (!fn->is_live)
      continue; // 如果没有引用该函数，则跳过当前循环，进入下一次循环

    if (fn->is_static)
      println("  .local %s", fn->name); // 打印字符串，表示该函数是静态函数，使用指定的格式将函数名插入到字符串中
    else
      println("  .globl %s", fn->name); // 打印字符串，表示该函数是全局函数，使用指定的格式将函数名插入到字符串中

    println("  .text"); // 打印字符串，表示下面的代码是文本段的代码
    println("  .type %s, @function", fn->name); // 打印字符串，使用指定的格式将函数名插入到字符串中，表示该字符串是函数类型的定义
    println("%s:", fn->name); // 打印字符串，使用指定的格式将函数名插入到字符串中，表示函数的开始位置
    current_fn = fn;  // 将当前函数设置为 fn

    // 开场白 序言
    println("  push %%rbp");  // 将 rbp 寄存器的值压入栈中，保存旧的基址指针
    println("  mov %%rsp, %%rbp");  // 将 rsp 寄存器的值移动到 rbp 寄存器中，建立新的基址指针
    println("  sub $%d, %%rsp", fn->stack_size);  // 将栈指针向下移动 fn->stack_size 个字节，为局部变量分配栈空间
    println("  mov %%rsp, %d(%%rbp)", fn->alloca_bottom->offset);  // 将栈指针的值移动到偏移为 fn->alloca_bottom->offset 的位置，用于动态分配的局部变量

    // 如果函数是可变实参函数，则保存参数寄存器
    if (fn->va_area) {
      int gp = 0, fp = 0;  // gp 表示通用寄存器的数量，fp 表示浮点寄存器的数量
      for (Obj *var = fn->params; var; var = var->next) {
        if (is_flonum(var->ty))  // 如果参数的类型是浮点数类型
          fp++;  // 浮点寄存器数量加一
        else
          gp++;  // 通用寄存器数量加一
      }

      int off = fn->va_area->offset;  // 可变参数区域的偏移量

      // va_elem
      println("  movl $%d, %d(%%rbp)", gp * 8, off);          // 将通用寄存器数量乘以 8 的结果移动到偏移为 off 的位置，表示 gp_offset
      println("  movl $%d, %d(%%rbp)", fp * 8 + 48, off + 4); // 将浮点寄存器数量乘以 8 再加上 48 的结果移动到偏移为 off + 4 的位置，表示 fp_offset
      println("  movq %%rbp, %d(%%rbp)", off + 8);            // 将 rbp 寄存器的值移动到偏移为 off + 8 的位置，表示 overflow_arg_area
      println("  addq $16, %d(%%rbp)", off + 8);              // 将偏移为 off + 8 的位置加上 16
      println("  movq %%rbp, %d(%%rbp)", off + 16);           // 将 rbp 寄存器的值移动到偏移为 off + 16 的位置，表示 reg_save_area
      println("  addq $%d, %d(%%rbp)", off + 24, off + 16);   // 将偏移为 off + 24 的位置加上 off + 16

      // __reg_save_area__
      println("  movq %%rdi, %d(%%rbp)", off + 24);   // 将 rdi 寄存器的值移动到偏移为 off + 24 的位置
      println("  movq %%rsi, %d(%%rbp)", off + 32);   // 将 rsi 寄存器的值移动到偏移为 off + 32 的位置
      println("  movq %%rdx, %d(%%rbp)", off + 40);   // 将 rdx 寄存器的值移动到偏移为 off + 40 的位置
      println("  movq %%rcx, %d(%%rbp)", off + 48);   // 将 rcx 寄存器的值移动到偏移为 off + 48 的位置
      println("  movq %%r8, %d(%%rbp)", off + 56);    // 将 r8 寄存器的值移动到偏移为 off + 56 的位置
      println("  movq %%r9, %d(%%rbp)", off + 64);    // 将 r9 寄存器的值移动到偏移为 off + 64 的位置
      println("  movsd %%xmm0, %d(%%rbp)", off + 72); // 将 xmm0 寄存器的值移动到偏移为 off + 72 的位置
      println("  movsd %%xmm1, %d(%%rbp)", off + 80); // 将 xmm1 寄存器的值移动到偏移为 off + 80 的位置
      println("  movsd %%xmm2, %d(%%rbp)", off + 88); // 将 xmm2 寄存器的值移动到偏移为 off + 88 的位置
      println("  movsd %%xmm3, %d(%%rbp)", off + 96); // 将 xmm3 寄存器的值移动到偏移为 off + 96 的位置
      println("  movsd %%xmm4, %d(%%rbp)", off + 104); // 将 xmm4 寄存器的值移动到偏移为 off + 104 的位置
      println("  movsd %%xmm5, %d(%%rbp)", off + 112); // 将 xmm5 寄存器的值移动到偏移为 off + 112 的位置
      println("  movsd %%xmm6, %d(%%rbp)", off + 120); // 将 xmm6 寄存器的值移动到偏移为 off + 120 的位置
      println("  movsd %%xmm7, %d(%%rbp)", off + 128); // 将 xmm7 寄存器的值移动到偏移为 off + 128 的位置
    }

    // 将传递的寄存器实参保存到栈中
    int gp = 0, fp = 0;   // 初始化通用寄存器计数器为 0，浮点寄存器计数器为 0
    for (Obj *var = fn->params; var; var = var->next) {  // 遍历参数链表
      if (var->offset > 0)  // 如果参数的偏移量大于 0，则跳过
        continue;

      Type *ty = var->ty;   // 获取参数的类型信息

      switch (ty->kind) {  // 根据ty的类型进行分支判断
      case TY_STRUCT:  // 如果ty的类型是结构体
      case TY_UNION:  // 或者是联合体
        assert(ty->size <= 16);  // 断言ty的大小不超过16字节
        if (has_flonum(ty, 0, 8, 0))  // 如果ty中包含浮点数，并且位于0到8字节之间
          store_fp(fp++, var->offset, MIN(8, ty->size));  // 将浮点数存储到浮点寄存器中，递增浮点寄存器指针，存储偏移为var->offset，大小为ty的大小和8的最小值
        else
          store_gp(gp++, var->offset, MIN(8, ty->size));  // 否则将非浮点数存储到通用寄存器中，递增通用寄存器指针，存储偏移为var->offset，大小为ty的大小和8的最小值

        if (ty->size > 8) {  // 如果ty的大小大于8字节
          if (has_flonum(ty, 8, 16, 0))  // 如果ty中包含浮点数，并且位于8到16字节之间
            store_fp(fp++, var->offset + 8, ty->size - 8);  // 将浮点数存储到浮点寄存器中，递增浮点寄存器指针，存储偏移为var->offset+8，大小为ty的大小减去8
          else
            store_gp(gp++, var->offset + 8, ty->size - 8);  // 否则将非浮点数存储到通用寄存器中，递增通用寄存器指针，存储偏移为var->offset+8，大小为ty的大小减去8
        }
        break;
      case TY_FLOAT:  // 如果ty的类型是浮点数
      case TY_DOUBLE:  // 或者是双精度浮点数
        store_fp(fp++, var->offset, ty->size);  // 将浮点数存储到浮点寄存器中，递增浮点寄存器指针，存储偏移为var->offset，大小为ty的大小
        break;
      default:
        store_gp(gp++, var->offset, ty->size);  // 将非浮点数存储到通用寄存器中，递增通用寄存器指针，存储偏移为var->offset，大小为ty的大小
      }
    }

    // 发射代码
    gen_stmt(fn->body); // 生成函数体的代码
    assert(depth == 0); // 断言深度为0

    // [https://www.sigbus.info/n1570#5.1.2.2.3p1] C规范为main函数定义了一条特殊规则。
    // 当main函数结束时，相当于返回0，即使对于其他函数，这种行为是未定义的。
    if (strcmp(fn->name, "main") == 0) // 如果函数名与"main"相同
      println("  mov $0, %%rax"); // 输出"mov $0, %%rax"，将0赋给寄存器rax

    // 收场白 后记
    println(".L.return.%s:", fn->name); // 输出".L.return.<函数名>:"，用于函数返回
    println("  mov %%rbp, %%rsp"); // 输出"mov %%rbp, %%rsp"，恢复栈指针
    println("  pop %%rbp"); // 输出"pop %%rbp"，恢复基址指针
    println("  ret"); // 输出"ret"，函数返回
  }
}

void codegen(Obj *prog, FILE *out) { // 生成代码的函数，接受一个Obj类型的参数prog和一个文件指针out作为输入
  output_file = out; // 将全局变量output_file设置为out

  File **files = get_input_files(); // 获取输入文件的数组，并将其赋值给files指针数组
  for (int i = 0; files[i]; i++) // 遍历files数组
    println("  .file %d \"%s\"", files[i]->file_no, files[i]->name); // 输出指令".file <文件编号> "<文件名>""，用于指定文件信息

  assign_lvar_offsets(prog); // 分配局部变量的偏移量
  emit_data(prog); // 生成数据段的代码
  emit_text(prog); // 生成文本段的代码
}

