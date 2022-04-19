/*------------------------------------------------
---------包括 生成.印名 代码生成器.印名------------------
------------------------------------------------*/
#include "定义.h"

/*------------------------------------------------
---------生成.印名 ------------------
------------------------------------------------*/

// Generic code generator
//   (印名)   

// Generate and return a new 标号 number
static int 标号身份 = 1;
int 生成标号(void) {
  return (标号身份++);
}

static void 更新_行(struct 抽象句法树节点 *小节点) {
  // Output the 行_ into the assembly if we've
  // changed the 行_ number in the AST 节点
  if (小节点->行号 != 0 && 行 != 小节点->行号) {
    行 = 小节点->行号;
    代码生成器行号(行);
  }
}

// Generate the code for an IF 语句
// and an optional ELSE clause.
static int 生成若(struct 抽象句法树节点 *小节点, int 环顶部标号, int 环终标号) {
  int 标号假, 标号终;

  // Generate two labels: one for the
  // false compound 语句, and one
  // for the end of the overall IF 语句.
  // When there is no ELSE clause, 标号假 _is_
  // the ending 标号!
  标号假 = 生成标号();
  if (小节点->右)
    标号终 = 生成标号();

  // Generate the condition code followed
  // by a jump to the false 标号.
  生成抽象句法树(小节点->左, 标号假, 无标号, 无标号, 小节点->操作);
  生成自由寄存(无寄存);

  // Generate the true compound 语句
  生成抽象句法树(小节点->中, 无标号, 环顶部标号, 环终标号, 小节点->操作);
  生成自由寄存(无寄存);

  // If there is an optional ELSE clause,
  // generate the jump to 跳过 to the end
  if (小节点->右)
    代码生成器跳(标号终);

  // Now the false 标号
  代码生成器标号(标号假);

  // Optional ELSE clause: generate the
  // false compound 语句 and the
  // end 标号
  if (小节点->右) {
    生成抽象句法树(小节点->右, 无标号, 无标号, 环终标号, 小节点->操作);
    生成自由寄存(无寄存);
    代码生成器标号(标号终);
  }

  return (无寄存);
}

// Generate the code for a WHILE 语句
static int 生成一会儿(struct 抽象句法树节点 *小节点) {
  int 标号开始, 标号终;

  // Generate the start and end labels
  // and output the start 标号
  标号开始 = 生成标号();
  标号终 = 生成标号();
  代码生成器标号(标号开始);

  // Generate the condition code followed
  // by a jump to the end 标号.
  生成抽象句法树(小节点->左, 标号终, 标号开始, 标号终, 小节点->操作);
  生成自由寄存(无寄存);

  // Generate the compound 语句 for the 身体
  生成抽象句法树(小节点->右, 无标号, 标号开始, 标号终, 小节点->操作);
  生成自由寄存(无寄存);

  // Finally output the jump back to the condition,
  // and the end 标号
  代码生成器跳(标号开始);
  代码生成器标号(标号终);
  return (无寄存);
}

// Generate the code for a SWITCH 语句
static int 生成切换(struct 抽象句法树节点 *小节点) {
  int *小情况值, *情况标号;
  int 标号跳顶部, 标号终;
  int 甲, 寄存, 默认标号 = 0, 情况计数 = 0;
  struct 抽象句法树节点 *印名;

  // Create arrays for the case values and associated labels.
  // Ensure that we have at least one position in each array.
  小情况值 = (int *) malloc((小节点->小抽象_整型值 + 1) * sizeof(int));
  情况标号 = (int *) malloc((小节点->小抽象_整型值 + 1) * sizeof(int));

  // Generate labels for the top of the jump table, and the
  // end of the switch 语句. Set a default 标号 for
  // the end of the switch, in case we don't have a default.
  标号跳顶部 = 生成标号();
  标号终 = 生成标号();
  默认标号 = 标号终;

  // Output the code to calculate the switch condition
  寄存 = 生成抽象句法树(小节点->左, 无标号, 无标号, 无标号, 0);
  代码生成器跳(标号跳顶部);
  生成自由寄存(寄存);

  // Walk the 右-孩子 linked 列表 to
  // generate the code for each case
  for (甲 = 0, 印名 = 小节点->右; 印名 != NULL; 甲++, 印名 = 印名->右) {

    // Get a 标号 for this case. Store it
    // and the case 值 in the arrays.
    // Record if it is the default case.
    情况标号[甲] = 生成标号();
    小情况值[甲] = 印名->小抽象_整型值;
    代码生成器标号(情况标号[甲]);
    if (印名->操作 == 抽象_默认)
      默认标号 = 情况标号[甲];
    else
      情况计数++;

    // Generate the case code. Pass in the end 标号 for the breaks.
    // If case has no 身体, we will fall into the following 身体.
    if (印名->左)
      生成抽象句法树(印名->左, 无标号, 无标号, 标号终, 0);
    生成自由寄存(无寄存);
  }

  // Ensure the last case jumps past the switch table
  代码生成器跳(标号终);

  // Now output the switch table and the end 标号.
  代码生成器切换(寄存, 情况计数, 标号跳顶部, 情况标号, 小情况值, 默认标号);
  代码生成器标号(标号终);
  return (无寄存);
}

// Generate the code for an
// 抽象_逻辑与 or 抽象_逻辑或 operation
static int 生成_逻辑与_或(struct 抽象句法树节点 *小节点) {
  // Generate two labels
  int 标号假 = 生成标号();
  int 标号终 = 生成标号();
  int 寄存;

  // Generate the code for the 左 expression
  // followed by the jump to the false 标号
  寄存 = 生成抽象句法树(小节点->左, 无标号, 无标号, 无标号, 0);
  代码生成器布尔(寄存, 小节点->操作, 标号假);
  生成自由寄存(无寄存);

  // Generate the code for the 右 expression
  // followed by the jump to the false 标号
  寄存 = 生成抽象句法树(小节点->右, 无标号, 无标号, 无标号, 0);
  代码生成器布尔(寄存, 小节点->操作, 标号假);
  生成自由寄存(寄存);

  // We didn't jump so set the 右 boolean 值
  if (小节点->操作 == 抽象_逻辑与) {
    代码生成器载入布尔(寄存, 1);
    代码生成器跳(标号终);
    代码生成器标号(标号假);
    代码生成器载入布尔(寄存, 0);
  } else {
    代码生成器载入布尔(寄存, 0);
    代码生成器跳(标号终);
    代码生成器标号(标号假);
    代码生成器载入布尔(寄存, 1);
  }
  代码生成器标号(标号终);
  return (寄存);
}

// Generate the code to copy the arguments of a
// function call to its parameters, then call the
// function itself. Return the register that holds 
// the function'串名 return 值.
static int 生成_函数调用(struct 抽象句法树节点 *小节点) {
  struct 抽象句法树节点 *胶水树 = 小节点->左;
  int 寄存;
  int 实参数目 = 0;

  // Save the registers before we copy the arguments
  代码生成器溢出寄存();

  // If there is a 列表 of arguments, walk this 列表
  // from the last argument (右-hand 孩子) to the
  // first
  while (胶水树) {
    // Calculate the expression'串名 值
    寄存 = 生成抽象句法树(胶水树->右, 无标号, 无标号, 无标号, 胶水树->操作);
    // Copy this into the 小节点'th function parameter: 大小 is 1, 2, 3, ...
    代码生成器复制实参(寄存, 胶水树->小抽象_大小);
    // Keep the first (highest) number of arguments
    if (实参数目 == 0)
      实参数目 = 胶水树->小抽象_大小;
    胶水树 = 胶水树->左;
  }

  // Call the function, clean up the stack (based on 实参数目),
  // and return its result
  return (代码生成器调用(小节点->符号, 实参数目));
}

// Generate code for a ternary expression
static int 生成_三元运算符(struct 抽象句法树节点 *小节点) {
  int 标号假, 标号终;
  int 寄存, 表达式寄存;

  // Generate two labels: one for the
  // false expression, and one for the
  // end of the overall expression
  标号假 = 生成标号();
  标号终 = 生成标号();

  // Generate the condition code followed
  // by a jump to the false 标号.
  生成抽象句法树(小节点->左, 标号假, 无标号, 无标号, 小节点->操作);
  // 生成自由寄存(无寄存);

  // Get a register to hold the result of the two expressions
  寄存 = 代码生成器分配寄存();

  // Generate the true expression and the false 标号.
  // Move the expression result into the known register.
  表达式寄存 = 生成抽象句法树(小节点->中, 无标号, 无标号, 无标号, 小节点->操作);
  代码生成器移(表达式寄存, 寄存);
  代码生成器自由寄存(表达式寄存);
  代码生成器跳(标号终);
  代码生成器标号(标号假);

  // Generate the false expression and the end 标号.
  // Move the expression result into the known register.
  表达式寄存 = 生成抽象句法树(小节点->右, 无标号, 无标号, 无标号, 小节点->操作);
  代码生成器移(表达式寄存, 寄存);
  代码生成器自由寄存(表达式寄存);
  代码生成器标号(标号终);
  return (寄存);
}

// Given an AST, an optional 标号, and the AST 操作
// of the parent, generate assembly code recursively.
// Return the register 身份 with the 树'串名 final 值.
int 生成抽象句法树(struct 抽象句法树节点 *小节点, int 若标号, int 环顶部标号,
	   int 环终标号, int 父节点抽象句法树操作) {
  int 左寄存 = 无寄存, 右寄存 = 无寄存;

  // Empty 树, do nothing
  if (小节点 == NULL)
    return (无寄存);

  // Update the 行_ number in the output
  更新_行(小节点);

  // We have some specific AST 节点 handling at the top
  // so that we don't evaluate the 孩子 sub-trees immediately
  switch (小节点->操作) {
  case 抽象_若:
    return (生成若(小节点, 环顶部标号, 环终标号));
  case 抽象_一会儿:
    return (生成一会儿(小节点));
  case 抽象_切换:
    return (生成切换(小节点));
  case 抽象_函数调用:
    return (生成_函数调用(小节点));
  case 抽象_三元运算符:
    return (生成_三元运算符(小节点));
  case 抽象_逻辑或:
    return (生成_逻辑与_或(小节点));
  case 抽象_逻辑与:
    return (生成_逻辑与_或(小节点));
  case 抽象_胶水:
    // Do each 孩子 语句, and free the
    // registers after each 孩子
    if (小节点->左 != NULL)
      生成抽象句法树(小节点->左, 若标号, 环顶部标号, 环终标号, 小节点->操作);
    生成自由寄存(无寄存);
    if (小节点->右 != NULL)
      生成抽象句法树(小节点->右, 若标号, 环顶部标号, 环终标号, 小节点->操作);
    生成自由寄存(无寄存);
    return (无寄存);
  case 抽象_函数:
    // Generate the function'串名 preamble before the code
    // in the 孩子 sub-树
    代码生成器函前文(小节点->符号);
    生成抽象句法树(小节点->左, 无标号, 无标号, 无标号, 小节点->操作);
    代码生成器后文(小节点->符号);
    return (无寄存);
  }

  // General AST 节点 handling below

  // Get the 左 and 右 sub-树 values
  if (小节点->左)
    左寄存 = 生成抽象句法树(小节点->左, 无标号, 无标号, 无标号, 小节点->操作);
  if (小节点->右)
    右寄存 = 生成抽象句法树(小节点->右, 无标号, 无标号, 无标号, 小节点->操作);

  switch (小节点->操作) {
  case 抽象_加法:
    return (代码生成器加(左寄存, 右寄存));
  case 抽象_减法:
    return (代码生成器减(左寄存, 右寄存));
  case 抽象_乘法:
    return (代码生成器乘(左寄存, 右寄存));
  case 抽象_除法:
    return (代码生成器除法取余(左寄存, 右寄存, 抽象_除法));
  case 抽象_余数:
    return (代码生成器除法取余(左寄存, 右寄存, 抽象_余数));
  case 抽象_与:
    return (代码生成器与(左寄存, 右寄存));
  case 抽象_或:
    return (代码生成器或(左寄存, 右寄存));
  case 抽象_异或:
    return (代码生成器异或(左寄存, 右寄存));
  case 抽象_左移:
    return (代码生成器左移(左寄存, 右寄存));
  case 抽象_右移:
    return (代码生成器右移(左寄存, 右寄存));
  case 抽象_等于:
  case 抽象_不等:
  case 抽象_小于:
  case 抽象_大于:
  case 抽象_小等:
  case 抽象_大等:
    // If the parent AST 节点 is an 抽象_若, 抽象_一会儿 or 抽象_三元运算符,
    // generate a compare followed by a jump. Otherwise, compare
    // registers and set one to 1 or 0 based on the comparison.
    if (父节点抽象句法树操作 == 抽象_若 || 父节点抽象句法树操作 == 抽象_一会儿 ||
	父节点抽象句法树操作 == 抽象_三元运算符)
      return (代码生成器对比与跳
	      (小节点->操作, 左寄存, 右寄存, 若标号, 小节点->左->类型));
    else
      return (代码生成器对比与设置(小节点->操作, 左寄存, 右寄存, 小节点->左->类型));
  case 抽象_整型字面:
    return (代码生成器载入整型(小节点->小抽象_整型值, 小节点->类型));
  case 抽象_串字面:
    return (代码生成器载入全局串(小节点->小抽象_整型值));
  case 抽象_标识:
    // Load our 值 if we are an 右值
    // or we are being dereferenced
    if (小节点->右值 || 父节点抽象句法树操作 == 抽象_解引用) {
      return (代码生成器载入变量(小节点->符号, 小节点->操作));
    } else
      return (无寄存);
  case 抽象_赋值加:
  case 抽象_赋值减:
  case 抽象_赋值星号:
  case 抽象_赋值斜杠:
  case 抽象_赋值余数:
  case 抽象_赋值:

    // For the '+=' and friends operators, generate suitable code
    // and get the register with the result. Then take the 左 孩子,
    // make it the 右 孩子 so that we can fall into the assignment code.
    switch (小节点->操作) {
    case 抽象_赋值加:
      左寄存 = 代码生成器加(左寄存, 右寄存);
      小节点->右 = 小节点->左;
      break;
    case 抽象_赋值减:
      左寄存 = 代码生成器减(左寄存, 右寄存);
      小节点->右 = 小节点->左;
      break;
    case 抽象_赋值星号:
      左寄存 = 代码生成器乘(左寄存, 右寄存);
      小节点->右 = 小节点->左;
      break;
    case 抽象_赋值斜杠:
      左寄存 = 代码生成器除法取余(左寄存, 右寄存, 抽象_除法);
      小节点->右 = 小节点->左;
      break;
    case 抽象_赋值余数:
      左寄存 = 代码生成器除法取余(左寄存, 右寄存, 抽象_余数);
      小节点->右 = 小节点->左;
      break;
    }

    // Now into the assignment code
    // Are we assigning to an 标识符 or through a pointer?
    switch (小节点->右->操作) {
    case 抽象_标识:
      if (小节点->右->符号->类别 == 类别_全局 ||
	  小节点->右->符号->类别 == 类别_外部 ||
	  小节点->右->符号->类别 == 类别_静态)
	return (代码生成器存全局(左寄存, 小节点->右->符号));
      else
	return (代码生成器存本地(左寄存, 小节点->右->符号));
    case 抽象_解引用:
      return (代码生成器存解引用(左寄存, 右寄存, 小节点->右->类型));
    default:
      致命数("Can't 抽象_赋值 in 生成抽象句法树(), 操作", 小节点->操作);
    }
  case 抽象_加宽:
    // Widen the 孩子'串名 类型 to the parent'串名 类型
    return (代码生成器加宽(左寄存, 小节点->左->类型, 小节点->类型));
  case 抽象_返回:
    代码生成器返回(左寄存, 函数身份);
    return (无寄存);
  case 抽象_地址:
    // If we have a symbol, get its address. Otherwise,
    // the 左 register already has the address because
    // it'串名 a 成员 access
    if (小节点->符号 != NULL)
      return (代码生成器地址(小节点->符号));
    else
      return (左寄存);
  case 抽象_解引用:
    // If we are an 右值, dereference to get the 值 we point at,
    // otherwise leave it for 抽象_赋值 to store through the pointer
    if (小节点->右值)
      return (代码生成器解引用(左寄存, 小节点->左->类型));
    else
      return (左寄存);
  case 抽象_比例尺:
    // Small optimisation: use shift if the
    // scale 值 is a known power of two
    switch (小节点->小抽象_大小) {
    case 2:
      return (代码生成器左移常量(左寄存, 1));
    case 4:
      return (代码生成器左移常量(左寄存, 2));
    case 8:
      return (代码生成器左移常量(左寄存, 3));
    default:
      // Load a register with the 大小 and
      // multiply the 左寄存 by this 大小
      右寄存 = 代码生成器载入整型(小节点->小抽象_大小, 原语_整型);
      return (代码生成器乘(左寄存, 右寄存));
    }
  case 抽象_后置自增:
  case 抽象_后置自减:
    // Load and decrement the variable'串名 值 into a register
    // and post increment/decrement it
    return (代码生成器载入变量(小节点->符号, 小节点->操作));
  case 抽象_前置自增:
  case 抽象_前置自减:
    // Load and decrement the variable'串名 值 into a register
    // and pre increment/decrement it
    return (代码生成器载入变量(小节点->左->符号, 小节点->操作));
  case 抽象_否定:
    return (代码生成器否定(左寄存));
  case 抽象_颠倒:
    return (代码生成器颠倒(左寄存));
  case 抽象_逻辑非:
    return (代码生成器逻辑非(左寄存));
  case 抽象_到布尔:
    // If the parent AST 节点 is an 抽象_若 or 抽象_一会儿, generate
    // a compare followed by a jump. Otherwise, set the register
    // to 0 or 1 based on it'串名 zeroeness or non-zeroeness
    return (代码生成器布尔(左寄存, 父节点抽象句法树操作, 若标号));
  case 抽象_断:
    代码生成器跳(环终标号);
    return (无寄存);
  case 抽象_继续:
    代码生成器跳(环顶部标号);
    return (无寄存);
  case 抽象_投射:
    return (左寄存);		// Not much to do
  default:
    致命数("Unknown AST operator", 小节点->操作);
  }
  return (无寄存);		// Keep -Wall happy
}

void 生成前文(char *文件名) {
  代码生成器前文(文件名);
}
void 生成后文() {
  代码生成器后文();
}
void 生成自由寄存(int 保留寄存) {
  代码生成器自由全部寄存(保留寄存);
}
void 生成全局符号(struct 符号表 *节点) {
  代码生成器全局符号(节点);
}

// Generate a global 串.
// If 追加 is true, 追加 to
// previous 生成全局串() call.
int 生成全局串(char *串值, int 追加) {
  int 小标号 = 生成标号();
  代码生成器全局串(小标号, 串值, 追加);
  return (小标号);
}
void 生成全局串终(void) {
  代码生成器全局串终();
}
int 生成基元大小(int 类型) {
  return (代码生成器基元大小(类型));
}
int 生成对齐(int 类型, int 偏移, int 指向) {
  return (代码生成器对齐(类型, 偏移, 指向));
}

/*------------------------------------------------
--------- 代码生成器.印名------------------
------------------------------------------------*/

// Code generator for x86-64
//   (印名)   

// Flag to say which section were are outputting in to
enum { 无_段, 文本_段, 数据_段 } 当前段 = 无_段;

// Switch to the text segment
void 代码生成器文本段() {
  if (当前段 != 文本_段) {
    fputs("\t.文本\n", 出文件);
    当前段 = 文本_段;
  }
}

// Switch to the 数据 segment
void 代码生成器数据段() {
  if (当前段 != 数据_段) {
    fputs("\t.数据\n", 出文件);
    当前段 = 数据_段;
  }
}

// Given a scalar 类型 值, return the
// 大小 of the 类型 in bytes.
int 代码生成器基元大小(int 类型) {
  if (指针类型(类型))
    return (8);
  switch (类型) {
  case 原语_印刻:
    return (1);
  case 原语_整型:
    return (4);
  case 原语_长型:
    return (8);
  default:
    致命数("Bad 类型 in 代码生成器基元大小:", 类型);
  }
  return (0);			// Keep -Wall happy
}

// Given a scalar 类型, an existing memory 偏移
// (which hasn't been allocated to anything yet)
// and a 指向 (1 is up, -1 is down), calculate
// and return a suitably aligned memory 偏移
// for this scalar 类型. This could be the original
// 偏移, or it could be above/below the original
int 代码生成器对齐(int 类型, int 偏移, int 指向) {
  int 对齐;

  // We don't need to do this on x86-64, but let'串名
  // align chars on any 偏移 and align ints/pointers
  // on a 4-byte 对齐
  switch (类型) {
  case 原语_印刻:
    break;
  default:
    // Align whatever we have now on a 4-byte 对齐.
    // I put the generic code here so it can be reused elsewhere.
    对齐 = 4;
    偏移 = (偏移 + 指向 * (对齐 - 1)) & ~(对齐 - 1);
  }
  return (偏移);
}

// Position of 下一个 local variable relative to stack base pointer.
// We store the 偏移 as positive to make aligning the stack pointer easier
static int 本地偏移;

// Position of stack pointer 偏移 relative to stack base pointer.
// We need this to ensure it is aligned on a 16-byte boundary.
static int 栈偏移;

// Create the position of a new local variable.
static int 新本地偏移(int 大小) {
  // Decrement the 偏移 by a minimum of 4 bytes
  // and allocate on the stack
  本地偏移 += (大小 > 4) ? 大小 : 4;
  return (-本地偏移);
}

// List of available registers and their names.
// We need a 列表 of byte and doubleword registers, too.
// The 列表 also includes the registers used to
// hold function parameters
#define 自由寄存器数目 4
#define 第一个形参寄存器 9		// Position of first parameter register
static int 自由寄存[自由寄存器数目];
static char *寄存列表[] =
  { "%r10", "%r11", "%r12", "%r13", "%r9", "%r8", "%rcx", "%rdx", "%rsi",
  "%rdi"
};

// We also need the 8-bit and 32-bit register names
static char *二号寄存器列表[] =
  { "%r10b", "%r11b", "%r12b", "%r13b", "%r9b", "%r8b", "%cl", "%dl", "%sil",
  "%dil"
};

static char *三号寄存器列表[] =
  { "%r10d", "%r11d", "%r12d", "%r13d", "%r9d", "%r8d", "%ecx", "%edx",
  "%esi", "%edi"
};

// Push and pop a register on/off the stack
static void 推寄存(int 小寄存) {
  fprintf(出文件, "\tpushq\t%s\n", 寄存列表[小寄存]);
}

static void 弹寄存(int 小寄存) {
  fprintf(出文件, "\tpopq\t%s\n", 寄存列表[小寄存]);
}


// Set all registers as available.
// But if 寄存 is positive, don't free that one.
void 代码生成器自由全部寄存(int 保留寄存) {
  int 甲;
  // fprintf(出文件, "# freeing all registers\n");
  for (甲 = 0; 甲 < 自由寄存器数目; 甲++)
    if (甲 != 保留寄存)
      自由寄存[甲] = 1;
}

// When we need to spill a register, we choose
// the following register and then cycle through
// the remaining registers. The 溢出寄存 increments
// continually, so we need to take a modulo 自由寄存器数目
// on it.
static int 溢出寄存 = 0;

// Allocate a free register. Return the number of
// the register. Die if no available registers.
int 代码生成器分配寄存(void) {
  int 寄存;

  for (寄存 = 0; 寄存 < 自由寄存器数目; 寄存++) {
    if (自由寄存[寄存]) {
      自由寄存[寄存] = 0;
      // fprintf(出文件, "# allocated register %s\n", 寄存列表[寄存]);
      return (寄存);
    }
  }

  // We have no registers, so we must spill one
  寄存 = (溢出寄存 % 自由寄存器数目);
  溢出寄存++;
  // fprintf(出文件, "# spilling 寄存 %s\n", 寄存列表[寄存]);
  推寄存(寄存);
  return (寄存);
}

// Return a register to the 列表 of available registers.
// Check to see if it'串名 not already there.
void 代码生成器自由寄存(int 寄存) {
  if (自由寄存[寄存] != 0) {
    // fprintf(出文件, "# error trying to free register %s\n", 寄存列表[寄存]);
    致命数("Error trying to free register", 寄存);
  }
  // If this was a spilled register, get it back
  if (溢出寄存 > 0) {
    溢出寄存--;
    寄存 = (溢出寄存 % 自由寄存器数目);
    // fprintf(出文件, "# unspilling 寄存 %s\n", 寄存列表[寄存]);
    弹寄存(寄存);
  } else {
    // fprintf(出文件, "# freeing 寄存 %s\n", 寄存列表[寄存]);
    自由寄存[寄存] = 1;
  }
}

// Spill all registers on the stack
void 代码生成器溢出寄存(void) {
  int 甲;

  for (甲 = 0; 甲 < 自由寄存器数目; 甲++)
    推寄存(甲);
}

// Unspill all registers from the stack
static void 代码生成器未溢出寄存(void) {
  int 甲;

  for (甲 = 自由寄存器数目 - 1; 甲 >= 0; 甲--)
    弹寄存(甲);
}

// Print out the assembly preamble
// for one output file
void 代码生成器前文(char *文件名) {
  代码生成器自由全部寄存(无寄存);
  代码生成器文本段();
  fprintf(出文件, "\t.file 1 ");
  fputc('"', 出文件);
  fprintf(出文件, "%s", 文件名);
  fputc('"', 出文件);
  fputc('\n', 出文件);
  fprintf(出文件,
	  "# internal switch(expr) routine\n"
	  "# %%rsi = switch table, %%rax = expr\n"
	  "# from SubC: http://www.t3x.org/subc/\n"
	  "\n"
	  "__switch:\n"
	  "        pushq   %%rsi\n"
	  "        movq    %%rdx, %%rsi\n"
	  "        movq    %%rax, %%rbx\n"
	  "        cld\n"
	  "        lodsq\n"
	  "        movq    %%rax, %%rcx\n"
	  "__next:\n"
	  "        lodsq\n"
	  "        movq    %%rax, %%rdx\n"
	  "        lodsq\n"
	  "        cmpq    %%rdx, %%rbx\n"
	  "        jnz     __no\n"
	  "        popq    %%rsi\n"
	  "        jmp     *%%rax\n"
	  "__no:\n"
	  "        loop    __next\n"
	  "        lodsq\n"
	  "        popq    %%rsi\n" "        jmp     *%%rax\n\n");
}

// Nothing to do for the end of a file
void 代码生成器后文() {
}

// Print out a function preamble
void 代码生成器函前文(struct 符号表 *符号) {
  char *名字 = 符号->名字;
  struct 符号表 *小形参, *小本地变量;
  int 小计数;
  int 形参偏移 = 16;		// Any pushed params start at this stack 偏移
  int 形参寄存 = 第一个形参寄存器;	// Index to the first param register in above 寄存 lists

  // Output in the text segment, reset local 偏移
  代码生成器文本段();
  本地偏移 = 0;

  // Output the function start, save the %rsp and %rsp
  if (符号->类别 == 类别_全局)
    fprintf(出文件, "\t.全局\t%s\n" "\t.类型\t%s, @function\n", 名字, 名字);
  fprintf(出文件, "%s:\n" "\tpushq\t%%rbp\n" "\tmovq\t%%rsp, %%rbp\n", 名字);

  // Copy any in-register parameters to the stack, up to six of them
  // The remaining parameters are already on the stack
  for (小形参 = 符号->成员, 小计数 = 1; 小形参 != NULL; 小形参 = 小形参->下一个, 小计数++) {
    if (小计数 > 6) {
      小形参->符号表_本地符号位置信息 = 形参偏移;
      形参偏移 += 8;
    } else {
      小形参->符号表_本地符号位置信息 = 新本地偏移(小形参->大小);
      代码生成器存本地(形参寄存--, 小形参);
    }
  }

  // For the remainder, if they are a parameter then they are
  // already on the stack. If only a local, make a stack position.
  for (小本地变量 = 本地头; 小本地变量 != NULL; 小本地变量 = 小本地变量->下一个) {
    小本地变量->符号表_本地符号位置信息 = 新本地偏移(小本地变量->大小);
  }

  // Align the stack pointer to be a multiple of 16
  // less than its previous 值
  栈偏移 = (本地偏移 + 15) & ~15;
  fprintf(出文件, "\taddq\t$%d,%%rsp\n", -栈偏移);
}

// Print out a function postamble
void 代码生成器后文(struct 符号表 *符号) {
  代码生成器标号(符号->符号表_终标号);
  fprintf(出文件, "\taddq\t$%d,%%rsp\n", 栈偏移);
  fputs("\tpopq	%rbp\n" "\tret\n", 出文件);
  代码生成器自由全部寄存(无寄存);
}

// Load an integer literal 值 into a register.
// Return the number of the register.
// For x86-64, we don't need to worry about the 类型.
int 代码生成器载入整型(int 值, int 类型) {
  // Get a new register
  int 小寄存 = 代码生成器分配寄存();

  fprintf(出文件, "\tmovq\t$%d, %s\n", 值, 寄存列表[小寄存]);
  return (小寄存);
}

// Load a 值 from a variable into a register.
// Return the number of the register. If the
// operation is pre- or post-increment/decrement,
// also perform this action.
int 代码生成器载入变量(struct 符号表 *符号, int 操作) {
  int 小寄存, 后置寄存, 偏移 = 1;

  // Get a new register
  小寄存 = 代码生成器分配寄存();

  // If the symbol is a pointer, use the 大小
  // of the 类型 that it points to as any
  // increment or decrement. If not, it'串名 one.
  if (指针类型(符号->类型))
    偏移 = 类型大小(值_位置(符号->类型), 符号->复合类型);

  // Negate the 偏移 for decrements
  if (操作 == 抽象_前置自减 || 操作 == 抽象_后置自减)
    偏移 = -偏移;

  // If we have a pre-operation
  if (操作 == 抽象_前置自增 || 操作 == 抽象_前置自减) {
    // Load the symbol'串名 address
    if (符号->类别 == 类别_本地 || 符号->类别 == 类别_形参)
      fprintf(出文件, "\tleaq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息, 寄存列表[小寄存]);
    else
      fprintf(出文件, "\tleaq\t%s(%%rip), %s\n", 符号->名字, 寄存列表[小寄存]);

    // and change the 值 at that address
    switch (符号->大小) {
    case 1:
      fprintf(出文件, "\taddb\t$%d,(%s)\n", 偏移, 寄存列表[小寄存]);
      break;
    case 4:
      fprintf(出文件, "\taddl\t$%d,(%s)\n", 偏移, 寄存列表[小寄存]);
      break;
    case 8:
      fprintf(出文件, "\taddq\t$%d,(%s)\n", 偏移, 寄存列表[小寄存]);
      break;
    }
  }

  // Now load the output register with the 值
  if (符号->类别 == 类别_本地 || 符号->类别 == 类别_形参) {
    switch (符号->大小) {
    case 1:
      fprintf(出文件, "\tmovzbq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息, 寄存列表[小寄存]);
      break;
    case 4:
      fprintf(出文件, "\tmovslq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息, 寄存列表[小寄存]);
      break;
    case 8:
      fprintf(出文件, "\tmovq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息, 寄存列表[小寄存]);
    }
  } else {
    switch (符号->大小) {
    case 1:
      fprintf(出文件, "\tmovzbq\t%s(%%rip), %s\n", 符号->名字, 寄存列表[小寄存]);
      break;
    case 4:
      fprintf(出文件, "\tmovslq\t%s(%%rip), %s\n", 符号->名字, 寄存列表[小寄存]);
      break;
    case 8:
      fprintf(出文件, "\tmovq\t%s(%%rip), %s\n", 符号->名字, 寄存列表[小寄存]);
    }
  }

  // If we have a post-operation, get a new register
  if (操作 == 抽象_后置自增 || 操作 == 抽象_后置自减) {
    后置寄存 = 代码生成器分配寄存();

    // Load the symbol'串名 address
    if (符号->类别 == 类别_本地 || 符号->类别 == 类别_形参)
      fprintf(出文件, "\tleaq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息,
	      寄存列表[后置寄存]);
    else
      fprintf(出文件, "\tleaq\t%s(%%rip), %s\n", 符号->名字,
	      寄存列表[后置寄存]);

    // and change the 值 at that address
    switch (符号->大小) {
    case 1:
      fprintf(出文件, "\taddb\t$%d,(%s)\n", 偏移, 寄存列表[后置寄存]);
      break;
    case 4:
      fprintf(出文件, "\taddl\t$%d,(%s)\n", 偏移, 寄存列表[后置寄存]);
      break;
    case 8:
      fprintf(出文件, "\taddq\t$%d,(%s)\n", 偏移, 寄存列表[后置寄存]);
      break;
    }

    // Finally, free the register
    代码生成器自由寄存(后置寄存);
  }

  // Return the register with the 值
  return (小寄存);
}

// Given the 标号 number of a global 串,
// load its address into a new register
int 代码生成器载入全局串(int 标号) {
  // Get a new register
  int 小寄存 = 代码生成器分配寄存();
  fprintf(出文件, "\tleaq\tL%d(%%rip), %s\n", 标号, 寄存列表[小寄存]);
  return (小寄存);
}

// Add two registers together and return
// the number of the register with the result
int 代码生成器加(int 寄存1, int 寄存2) {
  fprintf(出文件, "\taddq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Subtract the second register from the first and
// return the number of the register with the result
int 代码生成器减(int 寄存1, int 寄存2) {
  fprintf(出文件, "\tsubq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Multiply two registers together and return
// the number of the register with the result
int 代码生成器乘(int 寄存1, int 寄存2) {
  fprintf(出文件, "\timulq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Divide or modulo the first register by the second and
// return the number of the register with the result
int 代码生成器除法取余(int 寄存1, int 寄存2, int 操作) {
  fprintf(出文件, "\tmovq\t%s,%%rax\n", 寄存列表[寄存1]);
  fprintf(出文件, "\tcqo\n");
  fprintf(出文件, "\tidivq\t%s\n", 寄存列表[寄存2]);
  if (操作 == 抽象_除法)
    fprintf(出文件, "\tmovq\t%%rax,%s\n", 寄存列表[寄存1]);
  else
    fprintf(出文件, "\tmovq\t%%rdx,%s\n", 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Bitwise AND two registers
int 代码生成器与(int 寄存1, int 寄存2) {
  fprintf(出文件, "\tandq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Bitwise OR two registers
int 代码生成器或(int 寄存1, int 寄存2) {
  fprintf(出文件, "\torq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Bitwise XOR two registers
int 代码生成器异或(int 寄存1, int 寄存2) {
  fprintf(出文件, "\txorq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Shift 左 寄存1 by 寄存2 bits
int 代码生成器左移(int 寄存1, int 寄存2) {
  fprintf(出文件, "\tmovb\t%s, %%cl\n", 二号寄存器列表[寄存2]);
  fprintf(出文件, "\tshlq\t%%cl, %s\n", 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Shift 右 寄存1 by 寄存2 bits
int 代码生成器右移(int 寄存1, int 寄存2) {
  fprintf(出文件, "\tmovb\t%s, %%cl\n", 二号寄存器列表[寄存2]);
  fprintf(出文件, "\tshrq\t%%cl, %s\n", 寄存列表[寄存1]);
  代码生成器自由寄存(寄存2);
  return (寄存1);
}

// Negate a register'串名 值
int 代码生成器否定(int 小寄存) {
  fprintf(出文件, "\tnegq\t%s\n", 寄存列表[小寄存]);
  return (小寄存);
}

// Invert a register'串名 值
int 代码生成器颠倒(int 小寄存) {
  fprintf(出文件, "\tnotq\t%s\n", 寄存列表[小寄存]);
  return (小寄存);
}

// Logically negate a register'串名 值
int 代码生成器逻辑非(int 小寄存) {
  fprintf(出文件, "\ttest\t%s, %s\n", 寄存列表[小寄存], 寄存列表[小寄存]);
  fprintf(出文件, "\tsete\t%s\n", 二号寄存器列表[小寄存]);
  fprintf(出文件, "\tmovzbq\t%s, %s\n", 二号寄存器列表[小寄存], 寄存列表[小寄存]);
  return (小寄存);
}

// Load a boolean 值 (only 0 or 1)
// into the given register
void 代码生成器载入布尔(int 小寄存, int 小值) {
  fprintf(出文件, "\tmovq\t$%d, %s\n", 小值, 寄存列表[小寄存]);
}

// Convert an integer 值 to a boolean 值. Jump if
// it'串名 an IF, WHILE, LOGAND or LOGOR operation
int 代码生成器布尔(int 小寄存, int 操作, int 标号) {
  fprintf(出文件, "\ttest\t%s, %s\n", 寄存列表[小寄存], 寄存列表[小寄存]);
  switch (操作) {
  case 抽象_若:
  case 抽象_一会儿:
  case 抽象_逻辑与:
    fprintf(出文件, "\tje\tL%d\n", 标号);
    break;
  case 抽象_逻辑或:
    fprintf(出文件, "\tjne\tL%d\n", 标号);
    break;
  default:
    fprintf(出文件, "\tsetnz\t%s\n", 二号寄存器列表[小寄存]);
    fprintf(出文件, "\tmovzbq\t%s, %s\n", 二号寄存器列表[小寄存], 寄存列表[小寄存]);
  }
  return (小寄存);
}

// Call a function with the given symbol 身份.
// Pop off any arguments pushed on the stack.
// Return the register with the result
int 代码生成器调用(struct 符号表 *符号, int 实参数目) {
  int 出寄存;

  // Call the function
  fprintf(出文件, "\tcall\t%s@PLT\n", 符号->名字);

  // Remove any arguments pushed on the stack
  if (实参数目 > 6)
    fprintf(出文件, "\taddq\t$%d, %%rsp\n", 8 * (实参数目 - 6));

  // Unspill all the registers
  代码生成器未溢出寄存();

  // Get a new register and copy the return 值 into it
  出寄存 = 代码生成器分配寄存();
  fprintf(出文件, "\tmovq\t%%rax, %s\n", 寄存列表[出寄存]);
  return (出寄存);
}

// Given a register with an argument 值,
// copy this argument into the 实参本地符号位置信息'th
// parameter in preparation for a future function call.
// Note that 实参本地符号位置信息 is 1, 2, 3, 4, ..., never zero.
void 代码生成器复制实参(int 小寄存, int 实参本地符号位置信息) {

  // If this is above the sixth argument, simply push the
  // register on the stack. We rely on being called with
  // successive arguments in the correct order for x86-64
  if (实参本地符号位置信息 > 6) {
    fprintf(出文件, "\tpushq\t%s\n", 寄存列表[小寄存]);
  } else {
    // Otherwise, copy the 值 into one of the six registers
    // used to hold parameter values
    fprintf(出文件, "\tmovq\t%s, %s\n", 寄存列表[小寄存],
	    寄存列表[第一个形参寄存器 - 实参本地符号位置信息 + 1]);
  }
  代码生成器自由寄存(小寄存);
}

// Shift a register 左 by a constant
int 代码生成器左移常量(int 小寄存, int 小值) {
  fprintf(出文件, "\tsalq\t$%d, %s\n", 小值, 寄存列表[小寄存]);
  return (小寄存);
}

// Store a register'串名 值 into a variable
int 代码生成器存全局(int 小寄存, struct 符号表 *符号) {

  if (代码生成器基元大小(符号->类型) == 8) {
    fprintf(出文件, "\tmovq\t%s, %s(%%rip)\n", 寄存列表[小寄存], 符号->名字);
  } else
    switch (符号->类型) {
    case 原语_印刻:
      fprintf(出文件, "\tmovb\t%s, %s(%%rip)\n", 二号寄存器列表[小寄存], 符号->名字);
      break;
    case 原语_整型:
      fprintf(出文件, "\tmovl\t%s, %s(%%rip)\n", 三号寄存器列表[小寄存], 符号->名字);
      break;
    default:
      致命数("Bad 类型 in 代码生成器存全局:", 符号->类型);
    }
  return (小寄存);
}

// Store a register'串名 值 into a local variable
int 代码生成器存本地(int 小寄存, struct 符号表 *符号) {

  if (代码生成器基元大小(符号->类型) == 8) {
    fprintf(出文件, "\tmovq\t%s, %d(%%rbp)\n", 寄存列表[小寄存], 符号->符号表_本地符号位置信息);
  } else
    switch (符号->类型) {
    case 原语_印刻:
      fprintf(出文件, "\tmovb\t%s, %d(%%rbp)\n", 二号寄存器列表[小寄存], 符号->符号表_本地符号位置信息);
      break;
    case 原语_整型:
      fprintf(出文件, "\tmovl\t%s, %d(%%rbp)\n", 三号寄存器列表[小寄存], 符号->符号表_本地符号位置信息);
      break;
    default:
      致命数("Bad 类型 in 代码生成器存本地:", 符号->类型);
    }
  return (小寄存);
}

// Generate a global symbol but not functions
void 代码生成器全局符号(struct 符号表 *节点) {
  int 大小, 类型;
  int 初始值;
  int 甲;

  if (节点 == NULL)
    return;
  if (节点->结构化类型 == 结构化_函数)
    return;

  // Get the 大小 of the variable (or its elements if an array)
  // and the 类型 of the variable
  if (节点->结构化类型 == 结构化_数组) {
    大小 = 类型大小(值_位置(节点->类型), 节点->复合类型);
    类型 = 值_位置(节点->类型);
  } else {
    大小 = 节点->大小;
    类型 = 节点->类型;
  }

  // Generate the global identity and the 标号
  代码生成器数据段();
  if (节点->类别 == 类别_全局)
    fprintf(出文件, "\t.全局\t%s\n", 节点->名字);
  fprintf(出文件, "%s:\n", 节点->名字);

  // Output space for one or more elements
  for (甲 = 0; 甲 < 节点->元素个数; 甲++) {

    // Get any initial 值
    初始值 = 0;
    if (节点->初始列表 != NULL)
      初始值 = 节点->初始列表[甲];

    // Generate the space for this 类型
    switch (大小) {
    case 1:
      fprintf(出文件, "\t.byte\t%d\n", 初始值);
      break;
    case 4:
      fprintf(出文件, "\t.long\t%d\n", 初始值);
      break;
    case 8:
      // Generate the pointer to a 串 literal. Treat a zero 值
      // as actually zero, not the 标号 L0
      if (节点->初始列表 != NULL && 类型 == 指针_到(原语_印刻)
	  && 初始值 != 0)
	fprintf(出文件, "\t.quad\tL%d\n", 初始值);
      else
	fprintf(出文件, "\t.quad\t%d\n", 初始值);
      break;
    default:
      for (甲 = 0; 甲 < 大小; 甲++)
	fprintf(出文件, "\t.byte\t0\n");
    }
  }
}

// Generate a global 串 and its start 标号.
// Don't output the 标号 if 追加 is true.
void 代码生成器全局串(int 小标号, char *串值, int 追加) {
  char *类别指针;
  if (!追加)
    代码生成器标号(小标号);
  for (类别指针 = 串值; *类别指针; 类别指针++) {
    fprintf(出文件, "\t.byte\t%d\n", *类别指针);
  }
}

// NUL terminate a global 串
void 代码生成器全局串终(void) {
  fprintf(出文件, "\t.byte\t0\n");
}

// List of comparison instructions,
// in AST order: 抽象_等于, 抽象_不等, 抽象_小于, 抽象_大于, 抽象_小等, 抽象_大等
static char *对比列表[] =
  { "sete", "setne", "setl", "setg", "setle", "setge" };

// Compare two registers and set if true.
int 代码生成器对比与设置(int 抽象句法树操作, int 寄存1, int 寄存2, int 类型) {
  int 大小 = 代码生成器基元大小(类型);


  // Check the range of the AST operation
  if (抽象句法树操作 < 抽象_等于 || 抽象句法树操作 > 抽象_大等)
    致命("Bad 抽象句法树操作 in 代码生成器对比与设置()");

  switch (大小) {
  case 1:
    fprintf(出文件, "\tcmpb\t%s, %s\n", 二号寄存器列表[寄存2], 二号寄存器列表[寄存1]);
    break;
  case 4:
    fprintf(出文件, "\tcmpl\t%s, %s\n", 三号寄存器列表[寄存2], 三号寄存器列表[寄存1]);
    break;
  default:
    fprintf(出文件, "\tcmpq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  }

  fprintf(出文件, "\t%s\t%s\n", 对比列表[抽象句法树操作 - 抽象_等于], 二号寄存器列表[寄存2]);
  fprintf(出文件, "\tmovzbq\t%s, %s\n", 二号寄存器列表[寄存2], 寄存列表[寄存2]);
  代码生成器自由寄存(寄存1);
  return (寄存2);
}

// Generate a 标号
void 代码生成器标号(int 小标号) {
  fprintf(出文件, "L%d:\n", 小标号);
}

// Generate a jump to a 标号
void 代码生成器跳(int 小标号) {
  fprintf(出文件, "\tjmp\tL%d\n", 小标号);
}

// List of inverted jump instructions,
// in AST order: 抽象_等于, 抽象_不等, 抽象_小于, 抽象_大于, 抽象_小等, 抽象_大等
static char *颠倒对比列表[] = { "jne", "je", "jge", "jle", "jg", "jl" };

// Compare two registers and jump if false.
int 代码生成器对比与跳(int 抽象句法树操作, int 寄存1, int 寄存2, int 标号, int 类型) {
  int 大小 = 代码生成器基元大小(类型);

  // Check the range of the AST operation
  if (抽象句法树操作 < 抽象_等于 || 抽象句法树操作 > 抽象_大等)
    致命("Bad 抽象句法树操作 in 代码生成器对比与设置()");

  switch (大小) {
  case 1:
    fprintf(出文件, "\tcmpb\t%s, %s\n", 二号寄存器列表[寄存2], 二号寄存器列表[寄存1]);
    break;
  case 4:
    fprintf(出文件, "\tcmpl\t%s, %s\n", 三号寄存器列表[寄存2], 三号寄存器列表[寄存1]);
    break;
  default:
    fprintf(出文件, "\tcmpq\t%s, %s\n", 寄存列表[寄存2], 寄存列表[寄存1]);
  }

  fprintf(出文件, "\t%s\tL%d\n", 颠倒对比列表[抽象句法树操作 - 抽象_等于], 标号);
  代码生成器自由寄存(寄存1);
  代码生成器自由寄存(寄存2);
  return (无寄存);
}

// Widen the 值 in the register from the old
// to the new 类型, and return a register with
// this new 值
int 代码生成器加宽(int 小寄存, int 旧类型, int 新类型) {
  // Nothing to do
  return (小寄存);
}

// Generate code to return a 值 from a function
void 代码生成器返回(int 寄存, struct 符号表 *符号) {

  // Only return a 值 if we have a 值 to return
  if (寄存 != 无寄存) {
    // Deal with pointers here as we can't put them in
    // the switch 语句
    if (指针类型(符号->类型))
      fprintf(出文件, "\tmovq\t%s, %%rax\n", 寄存列表[寄存]);
    else {
      // Generate code depending on the function'串名 类型
      switch (符号->类型) {
      case 原语_印刻:
	fprintf(出文件, "\tmovzbl\t%s, %%eax\n", 二号寄存器列表[寄存]);
	break;
      case 原语_整型:
	fprintf(出文件, "\tmovl\t%s, %%eax\n", 三号寄存器列表[寄存]);
	break;
      case 原语_长型:
	fprintf(出文件, "\tmovq\t%s, %%rax\n", 寄存列表[寄存]);
	break;
      default:
	致命数("Bad function 类型 in 代码生成器返回:", 符号->类型);
      }
    }
  }

  代码生成器跳(符号->符号表_终标号);
}

// Generate code to load the address of an
// 标识符 into a variable. Return a new register
int 代码生成器地址(struct 符号表 *符号) {
  int 小寄存 = 代码生成器分配寄存();

  if (符号->类别 == 类别_全局 ||
      符号->类别 == 类别_外部 || 符号->类别 == 类别_静态)
    fprintf(出文件, "\tleaq\t%s(%%rip), %s\n", 符号->名字, 寄存列表[小寄存]);
  else
    fprintf(出文件, "\tleaq\t%d(%%rbp), %s\n", 符号->符号表_本地符号位置信息, 寄存列表[小寄存]);
  return (小寄存);
}

// Dereference a pointer to get the 值
// it points at into the same register
int 代码生成器解引用(int 小寄存, int 类型) {
  // Get the 类型 that we are pointing to
  int 新类型 = 值_位置(类型);
  // Now get the 大小 of this 类型
  int 大小 = 代码生成器基元大小(新类型);

  switch (大小) {
  case 1:
    fprintf(出文件, "\tmovzbq\t(%s), %s\n", 寄存列表[小寄存], 寄存列表[小寄存]);
    break;
  case 4:
    fprintf(出文件, "\tmovslq\t(%s), %s\n", 寄存列表[小寄存], 寄存列表[小寄存]);
    break;
  case 8:
    fprintf(出文件, "\tmovq\t(%s), %s\n", 寄存列表[小寄存], 寄存列表[小寄存]);
    break;
  default:
    致命数("Can't 代码生成器解引用 on 类型:", 类型);
  }
  return (小寄存);
}

// Store through a dereferenced pointer
int 代码生成器存解引用(int 寄存1, int 寄存2, int 类型) {
  // Get the 大小 of the 类型
  int 大小 = 代码生成器基元大小(类型);

  switch (大小) {
  case 1:
    fprintf(出文件, "\tmovb\t%s, (%s)\n", 二号寄存器列表[寄存1], 寄存列表[寄存2]);
    break;
  case 4:
    fprintf(出文件, "\tmovl\t%s, (%s)\n", 三号寄存器列表[寄存1], 寄存列表[寄存2]);
    break;
  case 8:
    fprintf(出文件, "\tmovq\t%s, (%s)\n", 寄存列表[寄存1], 寄存列表[寄存2]);
    break;
  default:
    致命数("Can't cgstoderef on 类型:", 类型);
  }
  return (寄存1);
}

// Generate a switch jump table and the code to
// load the registers and call the switch() code
void 代码生成器切换(int 寄存, int 情况计数, int 顶部标号,
	      int *情况标号, int *小情况值, int 默认标号) {
  int 甲, 标号;

  // Get a 标号 for the switch table
  标号 = 生成标号();
  代码生成器标号(标号);

  // Heuristic. If we have no cases, create one case
  // which points to the default case
  if (情况计数 == 0) {
    小情况值[0] = 0;
    情况标号[0] = 默认标号;
    情况计数 = 1;
  }
  // Generate the switch jump table.
  fprintf(出文件, "\t.quad\t%d\n", 情况计数);
  for (甲 = 0; 甲 < 情况计数; 甲++)
    fprintf(出文件, "\t.quad\t%d, L%d\n", 小情况值[甲], 情况标号[甲]);
  fprintf(出文件, "\t.quad\tL%d\n", 默认标号);

  // Load the specific registers
  代码生成器标号(顶部标号);
  fprintf(出文件, "\tmovq\t%s, %%rax\n", 寄存列表[寄存]);
  fprintf(出文件, "\tleaq\tL%d(%%rip), %%rdx\n", 标号);
  fprintf(出文件, "\tjmp\t__switch\n");
}

// Move 值 between registers
void 代码生成器移(int 寄存1, int 寄存2) {
  fprintf(出文件, "\tmovq\t%s, %s\n", 寄存列表[寄存1], 寄存列表[寄存2]);
}

// Output a gdb directive to say on which
// source code 行_ number the following
// assembly code came from
void 代码生成器行号(int 行_) {
  // fprintf(出文件, "\t.loc 1 %d 0\n", 行_);
}
