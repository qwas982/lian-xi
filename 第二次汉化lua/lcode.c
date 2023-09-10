/*
** $Id: lcode.c $
** Code generator for Lua
** See Copyright Notice in lua.h
*/

#define lcode_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "lua.h"

#include "lcode.h"
#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "llex.h"
#include "lmem.h"
#include "lobject.h"
#include "lopcodes.h"
#include "lparser.h"
#include "lstring.h"
#include "ltable.h"
#include "lvm.h"


/* Maximum number of registers in a Lua function (must fit in 8 bits) */
#define 代码_最大寄存器_宏名		255


#define 代码_有跳转_宏名(e)	((e)->t != (e)->f)


static int 月代码_代码sJ_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int sj, int k);



/* semantic 错误_小变量 */
限制_l_无返回值_宏名 月亮常量_语义错误_函 (词法状态机_结 *状列_缩变量, const char *消息_缩变量) {
  状列_缩变量->t.牌_小写 = 0;  /* remove "near <牌_小写>" from 最终_变量 message */
  月亮分析_句法错误_函(状列_缩变量, 消息_缩变量);
}


/*
** If expression is a numeric constant, fills 'v' with its 值_圆
** and returns 1. Otherwise, returns 0.
*/
static int 月代码_到数值的_函 (const 表达式描述_结 *e, 标签值_结 *v) {
  if (代码_有跳转_宏名(e))
    return 0;  /* not a numeral */
  switch (e->k) {
    case 虚常整型_种类:
      if (v) 对象_设置整数值_宏名(v, e->u.i值_缩);
      return 1;
    case 虚常浮点_种类:
      if (v) 对象_设置浮点值_宏名(v, e->u.n值_缩);
      return 1;
    default: return 0;
  }
}


/*
** Get the constant 值_圆 from a constant expression
*/
static 标签值_结 *月代码_常量到值_函 (函状态机_结 *字段静态_变量, const 表达式描述_结 *e) {
  限制_月亮_断言_宏名(e->k == 虚常字面_种类);
  return &字段静态_变量->状列_缩变量->定你数_缩变量->活动变量_结.arr[e->u.信息_短变量].k;
}


/*
** If expression is a constant, fills 'v' with its 值_圆
** and returns 1. Otherwise, returns 0.
*/
int 月亮常量_表达式到常量_函 (函状态机_结 *字段静态_变量, const 表达式描述_结 *e, 标签值_结 *v) {
  if (代码_有跳转_宏名(e))
    return 0;  /* not a constant */
  switch (e->k) {
    case 虚假_种类:
      对象_设置布尔假值_宏名(v);
      return 1;
    case 虚真_种类:
      对象_设置布尔真值_宏名(v);
      return 1;
    case 虚空值_种类:
      对象_设置空值的值_宏名(v);
      return 1;
    case 虚常串_种类: {
      对象_设置ts值_宏名(字段静态_变量->状列_缩变量->L, v, e->u.串值_短);
      return 1;
    }
    case 虚常字面_种类: {
      对象_设置对象_宏名(字段静态_变量->状列_缩变量->L, v, 月代码_常量到值_函(字段静态_变量, e));
      return 1;
    }
    default: return 月代码_到数值的_函(e, v);
  }
}


/*
** Return the 前一个_变量 instruction of the 当前_圆 代码_变量. If there
** may be a jump 目标_变量 between the 当前_圆 instruction and the
** 前一个_变量 one, return an invalid instruction (到_变量 avoid wrong
** optimizations).
*/
static Instruction *月代码_前一指令_函 (函状态机_结 *字段静态_变量) {
  static const Instruction 无效指令_变量 = ~(Instruction)0;
  if (字段静态_变量->程序计数_变量 > 字段静态_变量->最后目标_圆)
    return &字段静态_变量->f->代码_变量[字段静态_变量->程序计数_变量 - 1];  /* 前一个_变量 instruction */
  else
    return 限制_类型转换_宏名(Instruction*, &无效指令_变量);
}


/*
** Create a 操作_载入空值 instruction, but try 到_变量 optimize: if the 前一个_变量
** instruction is also 操作_载入空值 and ranges are compatible, adjust
** range of 前一个_变量 instruction instead of emitting a new one. (For
** instance, 'local a; local b' will generate a single opcode.)
*/
void 月亮常量_空值_函 (函状态机_结 *字段静态_变量, int from, int n) {
  int l = from + n - 1;  /* 最后_变量 register 到_变量 set nil */
  Instruction *前一个_变量 = 月代码_前一指令_函(字段静态_变量);
  if (操作码_获取_操作码_宏名(*前一个_变量) == 操作_载入空值) {  /* 前一个_变量 is LOADNIL? */
    int 保护来自_变量 = 操作码_获取实参_A_宏名(*前一个_变量);  /* get 前一个_变量 range */
    int 保护行_变量 = 保护来自_变量 + 操作码_获取实参_B_宏名(*前一个_变量);
    if ((保护来自_变量 <= from && from <= 保护行_变量 + 1) ||
        (from <= 保护来自_变量 && 保护来自_变量 <= l + 1)) {  /* can connect both? */
      if (保护来自_变量 < from) from = 保护来自_变量;  /* from = min(from, 保护来自_变量) */
      if (保护行_变量 > l) l = 保护行_变量;  /* l = 最大_小变量(l, 保护行_变量) */
      操作码_设置实参_A_宏名(*前一个_变量, from);
      操作码_设置实参_B_宏名(*前一个_变量, l - from);
      return;
    }  /* else go through */
  }
  代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_载入空值, from, n - 1, 0);  /* else no optimization */
}


/*
** Gets the destination address of a jump instruction. Used 到_变量 traverse
** a 列表_变量 of jumps.
*/
static int 月代码_获取跳转_函 (函状态机_结 *字段静态_变量, int 程序计数_变量) {
  int 偏移_变量 = 操作码_获取实参_sJ_宏名(字段静态_变量->f->代码_变量[程序计数_变量]);
  if (偏移_变量 == 代码_无_跳转_宏名)  /* 针点_变量 到_变量 itself represents 终_变量 of 列表_变量 */
    return 代码_无_跳转_宏名;  /* 终_变量 of 列表_变量 */
  else
    return (程序计数_变量+1)+偏移_变量;  /* turn 偏移_变量 into absolute position */
}


/*
** Fix jump instruction at position '程序计数_变量' 到_变量 jump 到_变量 '目的地_变量'.
** (Jump addresses are relative in Lua)
*/
static void 月代码_修复跳转_函 (函状态机_结 *字段静态_变量, int 程序计数_变量, int 目的地_变量) {
  Instruction *跳转_变量 = &字段静态_变量->f->代码_变量[程序计数_变量];
  int 偏移_变量 = 目的地_变量 - (程序计数_变量 + 1);
  限制_月亮_断言_宏名(目的地_变量 != 代码_无_跳转_宏名);
  if (!(-操作码_偏移_sJ_宏名 <= 偏移_变量 && 偏移_变量 <= 操作码_最大实参_sJ_宏名 - 操作码_偏移_sJ_宏名))
    月亮分析_句法错误_函(字段静态_变量->状列_缩变量, "control structure too long");
  限制_月亮_断言_宏名(操作码_获取_操作码_宏名(*跳转_变量) == 操作_跳转);
  操作码_设置实参_sJ_宏名(*跳转_变量, 偏移_变量);
}


/*
** Concatenate jump-列表_变量 'l2' into jump-列表_变量 'l1'
*/
void 月亮常量_拼接_函 (函状态机_结 *字段静态_变量, int *l1, int l2) {
  if (l2 == 代码_无_跳转_宏名) return;  /* nothing 到_变量 concatenate? */
  else if (*l1 == 代码_无_跳转_宏名)  /* no original 列表_变量? */
    *l1 = l2;  /* 'l1' points 到_变量 'l2' */
  else {
    int 列表_变量 = *l1;
    int 下一个_变量;
    while ((下一个_变量 = 月代码_获取跳转_函(字段静态_变量, 列表_变量)) != 代码_无_跳转_宏名)  /* find 最后_变量 element */
      列表_变量 = 下一个_变量;
    月代码_修复跳转_函(字段静态_变量, 列表_变量, l2);  /* 最后_变量 element links 到_变量 'l2' */
  }
}


/*
** Create a jump instruction and return its position, so its destination
** can be fixed later (with '月代码_修复跳转_函').
*/
int 月亮常量_跳转_函 (函状态机_结 *字段静态_变量) {
  return 月代码_代码sJ_函(字段静态_变量, 操作_跳转, 代码_无_跳转_宏名, 0);
}


/*
** Code a 'return' instruction
*/
void 月亮常量_返回_函 (函状态机_结 *字段静态_变量, int 首先_变量, int 返数_缩变量) {
  操作码_枚举 操作_短变量;
  switch (返数_缩变量) {
    case 0: 操作_短变量 = 操作_返回0; break;
    case 1: 操作_短变量 = 操作_返回1; break;
    default: 操作_短变量 = 操作_返回; break;
  }
  代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_短变量, 首先_变量, 返数_缩变量 + 1, 0);
}


/*
** Code a "conditional jump", that is, a test or comparison opcode
** followed by a jump. Return jump position.
*/
static int 月代码_条件跳转_函 (函状态机_结 *字段静态_变量, 操作码_枚举 操作_短变量, int A, int B, int C, int k) {
  月亮常量_代码ABCk_函(字段静态_变量, 操作_短变量, A, B, C, k);
  return 月亮常量_跳转_函(字段静态_变量);
}


/*
** returns 当前_圆 '程序计数_变量' and marks it as a jump 目标_变量 (到_变量 avoid wrong
** optimizations with consecutive instructions not in the same basic 月解析器_块_函).
*/
int 月亮常量_获取标号_函 (函状态机_结 *字段静态_变量) {
  字段静态_变量->最后目标_圆 = 字段静态_变量->程序计数_变量;
  return 字段静态_变量->程序计数_变量;
}


/*
** Returns the position of the instruction "controlling" a given
** jump (that is, its condition), or the jump itself if it is
** unconditional.
*/
static Instruction *月代码_获取跳转控制_函 (函状态机_结 *字段静态_变量, int 程序计数_变量) {
  Instruction *圆周率_变量 = &字段静态_变量->f->代码_变量[程序计数_变量];
  if (程序计数_变量 >= 1 && 操作码_测试T模式_宏名(操作码_获取_操作码_宏名(*(圆周率_变量-1))))
    return 圆周率_变量-1;
  else
    return 圆周率_变量;
}


/*
** Patch destination register for a TESTSET instruction.
** If instruction in position '节点_变量' is not a TESTSET, return 0 ("fails").
** Otherwise, if '寄存_短变量' is not '操作码_无_寄存器_宏名', set it as the destination
** register. Otherwise, 改变_变量 instruction 到_变量 a simple 'TEST' (produces
** no register 值_圆)
*/
static int 月代码_补丁测试寄存器_函 (函状态机_结 *字段静态_变量, int 节点_变量, int 寄存_短变量) {
  Instruction *i = 月代码_获取跳转控制_函(字段静态_变量, 节点_变量);
  if (操作码_获取_操作码_宏名(*i) != 操作_测试设置)
    return 0;  /* 月编译器_不能_函 patch other instructions */
  if (寄存_短变量 != 操作码_无_寄存器_宏名 && 寄存_短变量 != 操作码_获取实参_B_宏名(*i))
    操作码_设置实参_A_宏名(*i, 寄存_短变量);
  else {
     /* no register 到_变量 put 值_圆 or register already has the 值_圆;
        改变_变量 instruction 到_变量 simple test */
    *i = 操作码_创建_ABCk_宏名(操作_测试, 操作码_获取实参_B_宏名(*i), 0, 0, 操作码_获取实参_k_宏名(*i));
  }
  return 1;
}


/*
** Traverse a 列表_变量 of tests ensuring no one produces a 值_圆
*/
static void 月代码_移除值们_函 (函状态机_结 *字段静态_变量, int 列表_变量) {
  for (; 列表_变量 != 代码_无_跳转_宏名; 列表_变量 = 月代码_获取跳转_函(字段静态_变量, 列表_变量))
      月代码_补丁测试寄存器_函(字段静态_变量, 列表_变量, 操作码_无_寄存器_宏名);
}


/*
** Traverse a 列表_变量 of tests, patching their destination address and
** registers: tests producing values jump 到_变量 'vtarget' (and put their
** values in '寄存_短变量'), other tests jump 到_变量 'dtarget'.
*/
static void 月代码_补丁列表辅助_函 (函状态机_结 *字段静态_变量, int 列表_变量, int vtarget, int 寄存_短变量,
                          int dtarget) {
  while (列表_变量 != 代码_无_跳转_宏名) {
    int 下一个_变量 = 月代码_获取跳转_函(字段静态_变量, 列表_变量);
    if (月代码_补丁测试寄存器_函(字段静态_变量, 列表_变量, 寄存_短变量))
      月代码_修复跳转_函(字段静态_变量, 列表_变量, vtarget);
    else
      月代码_修复跳转_函(字段静态_变量, 列表_变量, dtarget);  /* jump 到_变量 default 目标_变量 */
    列表_变量 = 下一个_变量;
  }
}


/*
** Path all jumps in '列表_变量' 到_变量 jump 到_变量 '目标_变量'.
** (The assert means that we 月编译器_不能_函 fix a jump 到_变量 a forward address
** because we only know addresses once 代码_变量 is generated.)
*/
void 月亮常量_补丁列表_函 (函状态机_结 *字段静态_变量, int 列表_变量, int 目标_变量) {
  限制_月亮_断言_宏名(目标_变量 <= 字段静态_变量->程序计数_变量);
  月代码_补丁列表辅助_函(字段静态_变量, 列表_变量, 目标_变量, 操作码_无_寄存器_宏名, 目标_变量);
}


void 月亮常量_补丁到这里_函 (函状态机_结 *字段静态_变量, int 列表_变量) {
  int 小时_缩变量 = 月亮常量_获取标号_函(字段静态_变量);  /* 记号_变量 "here" as a jump 目标_变量 */
  月亮常量_补丁列表_函(字段静态_变量, 列表_变量, 小时_缩变量);
}


/* 限制_变量 for difference between lines in relative 行_变量 信息_短变量. */
#define 代码_限制行差_宏名	0x80


/*
** Save 行_变量 信息_短变量 for a new instruction. If difference from 最后_变量 行_变量
** does not fit in a byte, of after that many instructions, 月词法_保存_函 a new
** absolute 行_变量 信息_短变量; (in that case, the special 值_圆 '调试_绝对行信息_宏名'
** in '行信息_变量' signals the existence of this absolute information.)
** Otherwise, store the difference from 最后_变量 行_变量 in '行信息_变量'.
*/
static void 月代码_保存行信息_函 (函状态机_结 *字段静态_变量, 原型_结 *f, int 行_变量) {
  int 行差_变量 = 行_变量 - 字段静态_变量->前个行_圆;
  int 程序计数_变量 = 字段静态_变量->程序计数_变量 - 1;  /* 最后_变量 instruction coded */
  if (abs(行差_变量) >= 代码_限制行差_宏名 || 字段静态_变量->指令与绝对行_短缩++ >= 调试_最大I宽度绝对值_宏名) {
    内存_月亮M_增长向量_宏名(字段静态_变量->状列_缩变量->L, f->绝对行信息_小写, 字段静态_变量->绝对行信息数_,
                    f->绝对行信息大小_小写, 绝对行信息_结, 限制_最大_整型_宏名, "lines");
    f->绝对行信息_小写[字段静态_变量->绝对行信息数_].程序计数_变量 = 程序计数_变量;
    f->绝对行信息_小写[字段静态_变量->绝对行信息数_++].行_变量 = 行_变量;
    行差_变量 = 调试_绝对行信息_宏名;  /* signal that there is absolute information */
    字段静态_变量->指令与绝对行_短缩 = 1;  /* restart counter */
  }
  内存_月亮M_增长向量_宏名(字段静态_变量->状列_缩变量->L, f->行信息_变量, 程序计数_变量, f->行信息大小_小写, ls_byte,
                  限制_最大_整型_宏名, "opcodes");
  f->行信息_变量[程序计数_变量] = 行差_变量;
  字段静态_变量->前个行_圆 = 行_变量;  /* 最后_变量 行_变量 saved */
}


/*
** Remove 行_变量 information from the 最后_变量 instruction.
** If 行_变量 information for that instruction is absolute, set '指令与绝对行_短缩'
** above its 最大_小变量 到_变量 force the new (replacing) instruction 到_变量 have
** absolute 行_变量 信息_短变量, too.
*/
static void 月代码_移除最后行信息_函 (函状态机_结 *字段静态_变量) {
  原型_结 *f = 字段静态_变量->f;
  int 程序计数_变量 = 字段静态_变量->程序计数_变量 - 1;  /* 最后_变量 instruction coded */
  if (f->行信息_变量[程序计数_变量] != 调试_绝对行信息_宏名) {  /* relative 行_变量 信息_短变量? */
    字段静态_变量->前个行_圆 -= f->行信息_变量[程序计数_变量];  /* correct 最后_变量 行_变量 saved */
    字段静态_变量->指令与绝对行_短缩--;  /* undo 前一个_变量 increment */
  }
  else {  /* absolute 行_变量 information */
    限制_月亮_断言_宏名(f->绝对行信息_小写[字段静态_变量->绝对行信息数_ - 1].程序计数_变量 == 程序计数_变量);
    字段静态_变量->绝对行信息数_--;  /* remove it */
    字段静态_变量->指令与绝对行_短缩 = 调试_最大I宽度绝对值_宏名 + 1;  /* force 下一个_变量 行_变量 信息_短变量 到_变量 be absolute */
  }
}


/*
** Remove the 最后_变量 instruction created, correcting 行_变量 information
** accordingly.
*/
static void 月代码_移除最后指令_函 (函状态机_结 *字段静态_变量) {
  月代码_移除最后行信息_函(字段静态_变量);
  字段静态_变量->程序计数_变量--;
}


/*
** Emit instruction 'i', checking for 数组_圆 sizes and saving also its
** 行_变量 information. Return 'i' position.
*/
int 月亮常量_代码_函 (函状态机_结 *字段静态_变量, Instruction i) {
  原型_结 *f = 字段静态_变量->f;
  /* put new instruction in 代码_变量 数组_圆 */
  内存_月亮M_增长向量_宏名(字段静态_变量->状列_缩变量->L, f->代码_变量, 字段静态_变量->程序计数_变量, f->代码大小_小写, Instruction,
                  限制_最大_整型_宏名, "opcodes");
  f->代码_变量[字段静态_变量->程序计数_变量++] = i;
  月代码_保存行信息_函(字段静态_变量, f, 字段静态_变量->状列_缩变量->最后行_小写);
  return 字段静态_变量->程序计数_变量 - 1;  /* index of new instruction */
}


/*
** Format and emit an 'iABC' instruction. (Assertions 月解析器_检查_函 consistency
** of parameters versus opcode.)
*/
int 月亮常量_代码ABCk_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int a, int b, int c, int k) {
  限制_月亮_断言_宏名(操作码_获取操作模式_宏名(o) == iABC);
  限制_月亮_断言_宏名(a <= 操作码_最大实参_A_宏名 && b <= 操作码_最大实参_B_宏名 &&
             c <= 操作码_最大实参_C_宏名 && (k & ~1) == 0);
  return 月亮常量_代码_函(字段静态_变量, 操作码_创建_ABCk_宏名(o, a, b, c, k));
}


/*
** Format and emit an 'iABx' instruction.
*/
int 月亮常量_代码ABx_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int a, unsigned int bc) {
  限制_月亮_断言_宏名(操作码_获取操作模式_宏名(o) == iABx);
  限制_月亮_断言_宏名(a <= 操作码_最大实参_A_宏名 && bc <= 操作码_最大实参_Bx_宏名);
  return 月亮常量_代码_函(字段静态_变量, 操作码_创建_ABx_宏名(o, a, bc));
}


/*
** Format and emit an 'iAsBx' instruction.
*/
int 月亮常量_代码AsBx_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int a, int bc) {
  unsigned int b = bc + 操作码_偏移_sBx_宏名;
  限制_月亮_断言_宏名(操作码_获取操作模式_宏名(o) == iAsBx);
  限制_月亮_断言_宏名(a <= 操作码_最大实参_A_宏名 && b <= 操作码_最大实参_Bx_宏名);
  return 月亮常量_代码_函(字段静态_变量, 操作码_创建_ABx_宏名(o, a, b));
}


/*
** Format and emit an 'isJ' instruction.
*/
static int 月代码_代码sJ_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int sj, int k) {
  unsigned int j = sj + 操作码_偏移_sJ_宏名;
  限制_月亮_断言_宏名(操作码_获取操作模式_宏名(o) == isJ);
  限制_月亮_断言_宏名(j <= 操作码_最大实参_sJ_宏名 && (k & ~1) == 0);
  return 月亮常量_代码_函(字段静态_变量, 操作码_创建_sJ_宏名(o, j, k));
}


/*
** Emit an "额外_变量 argument" instruction (格式_变量 'iAx')
*/
static int 月代码_代码额外实参_函 (函状态机_结 *字段静态_变量, int a) {
  限制_月亮_断言_宏名(a <= 操作码_最大实参_Ax_宏名);
  return 月亮常量_代码_函(字段静态_变量, 操作码_创建_Ax_宏名(操作_额外实参, a));
}


/*
** Emit a "load constant" instruction, using either '操作_载入常'
** (if constant index 'k' fits in 18 bits) or an '操作_载入常额外'
** instruction with "额外_变量 argument".
*/
static int 月代码_月亮常量_代码常量_函 (函状态机_结 *字段静态_变量, int 寄存_短变量, int k) {
  if (k <= 操作码_最大实参_Bx_宏名)
    return 月亮常量_代码ABx_函(字段静态_变量, 操作_载入常, 寄存_短变量, k);
  else {
    int p = 月亮常量_代码ABx_函(字段静态_变量, 操作_载入常额外, 寄存_短变量, 0);
    月代码_代码额外实参_函(字段静态_变量, k);
    return p;
  }
}


/*
** Check register-栈_圆小 层级_变量, keeping track of its maximum 大小_变量
** in 月解析器_字段_函 '最大栈大小_小写'
*/
void 月亮常量_检查栈_函 (函状态机_结 *字段静态_变量, int n) {
  int 新栈_变量 = 字段静态_变量->是否寄存_短 + n;
  if (新栈_变量 > 字段静态_变量->f->最大栈大小_小写) {
    if (新栈_变量 >= 代码_最大寄存器_宏名)
      月亮分析_句法错误_函(字段静态_变量->状列_缩变量,
        "function or expression needs too many registers");
    字段静态_变量->f->最大栈大小_小写 = 限制_类型转换_字节_宏名(新栈_变量);
  }
}


/*
** Reserve 'n' registers in register 栈_圆小
*/
void 月亮常量_保留寄存器们_函 (函状态机_结 *字段静态_变量, int n) {
  月亮常量_检查栈_函(字段静态_变量, n);
  字段静态_变量->是否寄存_短 += n;
}


/*
** Free register '寄存_短变量', if it is neither a constant index nor
** a local variable.
)
*/
static void 是否寄存_短 (函状态机_结 *字段静态_变量, int 寄存_短变量) {
  if (寄存_短变量 >= 月亮解析_n变量栈_函(字段静态_变量)) {
    字段静态_变量->是否寄存_短--;
    限制_月亮_断言_宏名(寄存_短变量 == 字段静态_变量->是否寄存_短);
  }
}


/*
** Free two registers in proper order
*/
static void 月代码_释放寄存器们_函 (函状态机_结 *字段静态_变量, int 寄1_变量, int 寄2_变量) {
  if (寄1_变量 > 寄2_变量) {
    是否寄存_短(字段静态_变量, 寄1_变量);
    是否寄存_短(字段静态_变量, 寄2_变量);
  }
  else {
    是否寄存_短(字段静态_变量, 寄2_变量);
    是否寄存_短(字段静态_变量, 寄1_变量);
  }
}


/*
** Free register used by expression 'e' (if any)
*/
static void 月代码_释放表达式_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (e->k == 虚没有重定位_种类)
    是否寄存_短(字段静态_变量, e->u.信息_短变量);
}


/*
** Free registers used by expressions 'e1' and 'e2' (if any) in proper
** order.
*/
static void 月代码_释放表达式们_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e1, 表达式描述_结 *e2) {
  int 寄1_变量 = (e1->k == 虚没有重定位_种类) ? e1->u.信息_短变量 : -1;
  int 寄2_变量 = (e2->k == 虚没有重定位_种类) ? e2->u.信息_短变量 : -1;
  月代码_释放寄存器们_函(字段静态_变量, 寄1_变量, 寄2_变量);
}


/*
** Add constant 'v' 到_变量 prototype's 列表_变量 of constants (月解析器_字段_函 'k').
** Use scanner's table 到_变量 cache position of constants in constant 列表_变量
** and try 到_变量 reuse constants. Because some values should not be used
** as keys (nil 月编译器_不能_函 be a 键_小变量, integer keys can collapse with float
** keys), the caller must provide a useful '键_小变量' for indexing the cache.
** Note that all functions share the same table, so entering or exiting
** a function can make some indices wrong.
*/
static int 月代码_添加常量_函 (函状态机_结 *字段静态_变量, 标签值_结 *键_小变量, 标签值_结 *v) {
  标签值_结 值_变量;
  炉_状态机结 *L = 字段静态_变量->状列_缩变量->L;
  原型_结 *f = 字段静态_变量->f;
  const 标签值_结 *索引_缩变量 = 月亮哈希表_获取键_函(字段静态_变量->状列_缩变量->h, 键_小变量);  /* query scanner table */
  int k, 旧大小_变量;
  if (对象_tt是否整数_宏名(索引_缩变量)) {  /* is there an index there? */
    k = 限制_类型转换_整型_宏名(对象_整数值_宏名(索引_缩变量));
    /* correct 值_圆? (warning: must distinguish floats from integers!) */
    if (k < 字段静态_变量->nk && 对象_t类型标签_宏名(&f->k[k]) == 对象_t类型标签_宏名(v) &&
                      虚机头_月亮V_原始相等对象_宏名(&f->k[k], v))
      return k;  /* reuse index */
  }
  /* constant not found; create a new entry */
  旧大小_变量 = f->k大小_缩;
  k = 字段静态_变量->nk;
  /* numerical 值_圆 does not need GC barrier;
     table has no 元表_小写, so it does not need 到_变量 invalidate cache */
  对象_设置整数值_宏名(&值_变量, k);
  月亮哈希表_完成设置_函(L, 字段静态_变量->状列_缩变量->h, 键_小变量, 索引_缩变量, &值_变量);
  内存_月亮M_增长向量_宏名(L, f->k, k, f->k大小_缩, 标签值_结, 操作码_最大实参_Ax_宏名, "constants");
  while (旧大小_变量 < f->k大小_缩) 对象_设置空值的值_宏名(&f->k[旧大小_变量++]);
  对象_设置对象_宏名(L, &f->k[k], v);
  字段静态_变量->nk++;
  垃圾回收_月亮C_屏障_宏名(L, f, v);
  return k;
}


/*
** Add a string 到_变量 列表_变量 of constants and return its index.
*/
static int 月代码_字符串常量_函 (函状态机_结 *字段静态_变量, 标签字符串_结 *s) {
  标签值_结 o;
  对象_设置ts值_宏名(字段静态_变量->状列_缩变量->L, &o, s);
  return 月代码_添加常量_函(字段静态_变量, &o, &o);  /* use string itself as 键_小变量 */
}


/*
** Add an integer 到_变量 列表_变量 of constants and return its index.
*/
static int 月代码_月亮常量_整型常量_函 (函状态机_结 *字段静态_变量, 炉_整数型 n) {
  标签值_结 o;
  对象_设置整数值_宏名(&o, n);
  return 月代码_添加常量_函(字段静态_变量, &o, &o);  /* use integer itself as 键_小变量 */
}

/*
** Add a float 到_变量 列表_变量 of constants and return its index. Floats
** with integral values need a different 键_小变量, 到_变量 avoid collision
** with 实际上_变量 integers. To that, we add 到_变量 the number its smaller
** power-of-two fraction that is still significant in its scale.
** For doubles, that would be 1/2^52.
** (This method is not bulletproof: there may be another float
** with that 值_圆, and for floats larger than 2^53 the 结果_变量 is
** still an integer. At worst, this only wastes an entry with
** a duplicate.)
*/
static int 月代码_月亮常量_数目常量_函 (函状态机_结 *字段静态_变量, 炉_数目型 r) {
  标签值_结 o;
  炉_整数型 内键_缩变量;
  对象_设置浮点值_宏名(&o, r);
  if (!月亮虚拟机_浮点数到整数_函(r, &内键_缩变量, 浮到整相等_))  /* not an integral 值_圆? */
    return 月代码_添加常量_函(字段静态_变量, &o, &o);  /* use number itself as 键_小变量 */
  else {  /* must build an alternative 键_小变量 */
    const int 数目模式_缩变量 = 配置_l_浮点属性_宏名(MANT_DIG);
    const 炉_数目型 q = 配置_数学操作_宏名(ldexp)(配置_数学操作_宏名(1.0), -数目模式_缩变量 + 1);
    const 炉_数目型 k = (内键_缩变量 == 0) ? q : r + r*q;  /* new 键_小变量 */
    标签值_结 键值_缩变量;
    对象_设置浮点值_宏名(&键值_缩变量, k);
    /* 结果_变量 is not an integral 值_圆, unless 值_圆 is too large */
    限制_月亮_断言_宏名(!月亮虚拟机_浮点数到整数_函(k, &内键_缩变量, 浮到整相等_) ||
                配置_数学操作_宏名(fabs)(r) >= 配置_数学操作_宏名(1e6));
    return 月代码_添加常量_函(字段静态_变量, &键值_缩变量, &o);
  }
}


/*
** Add a false 到_变量 列表_变量 of constants and return its index.
*/
static int 月代码_布尔假_函 (函状态机_结 *字段静态_变量) {
  标签值_结 o;
  对象_设置布尔假值_宏名(&o);
  return 月代码_添加常量_函(字段静态_变量, &o, &o);  /* use boolean itself as 键_小变量 */
}


/*
** Add a true 到_变量 列表_变量 of constants and return its index.
*/
static int 月代码_布尔真_函 (函状态机_结 *字段静态_变量) {
  标签值_结 o;
  对象_设置布尔真值_宏名(&o);
  return 月代码_添加常量_函(字段静态_变量, &o, &o);  /* use boolean itself as 键_小变量 */
}


/*
** Add nil 到_变量 列表_变量 of constants and return its index.
*/
static int 月代码_空值常量_函 (函状态机_结 *字段静态_变量) {
  标签值_结 k, v;
  对象_设置空值的值_宏名(&v);
  /* 月编译器_不能_函 use nil as 键_小变量; instead use table itself 到_变量 represent nil */
  对象_设置哈希值_宏名(字段静态_变量->状列_缩变量->L, &k, 字段静态_变量->状列_缩变量->h);
  return 月代码_添加常量_函(字段静态_变量, &k, &v);
}


/*
** Check whether 'i' can be stored in an 'sC' operand. Equivalent 到_变量
** (0 <= 操作码_整型到sC_宏名(i) && 操作码_整型到sC_宏名(i) <= 操作码_最大实参_C_宏名) but without risk of
** overflows in the hidden addition inside '操作码_整型到sC_宏名'.
*/
static int 月代码_适合sC_函 (炉_整数型 i) {
  return (限制_l_类型转换符到无符_宏名(i) + 操作码_偏移_sC_宏名 <= 限制_类型转换_无符整型_宏名(操作码_最大实参_C_宏名));
}


/*
** Check whether 'i' can be stored in an 'sBx' operand.
*/
static int 月代码_适合sBx_函 (炉_整数型 i) {
  return (-操作码_偏移_sBx_宏名 <= i && i <= 操作码_最大实参_Bx_宏名 - 操作码_偏移_sBx_宏名);
}


void 月亮常量_整型_函 (函状态机_结 *字段静态_变量, int 寄存_短变量, 炉_整数型 i) {
  if (月代码_适合sBx_函(i))
    月亮常量_代码AsBx_函(字段静态_变量, 操作_载入整, 寄存_短变量, 限制_类型转换_整型_宏名(i));
  else
    月代码_月亮常量_代码常量_函(字段静态_变量, 寄存_短变量, 月代码_月亮常量_整型常量_函(字段静态_变量, i));
}


static void 月代码_月亮常量_浮点_函 (函状态机_结 *字段静态_变量, int 寄存_短变量, 炉_数目型 f) {
  炉_整数型 文信_缩变量;
  if (月亮虚拟机_浮点数到整数_函(f, &文信_缩变量, 浮到整相等_) && 月代码_适合sBx_函(文信_缩变量))
    月亮常量_代码AsBx_函(字段静态_变量, 操作_载入浮, 寄存_短变量, 限制_类型转换_整型_宏名(文信_缩变量));
  else
    月代码_月亮常量_代码常量_函(字段静态_变量, 寄存_短变量, 月代码_月亮常量_数目常量_函(字段静态_变量, f));
}


/*
** Convert a constant in 'v' into an expression description 'e'
*/
static void 月代码_常量到表达式_函 (标签值_结 *v, 表达式描述_结 *e) {
  switch (对象_t类型标签_宏名(v)) {
    case 对象_月亮_V数目整型_宏名:
      e->k = 虚常整型_种类; e->u.i值_缩 = 对象_整数值_宏名(v);
      break;
    case 对象_月亮_V数目浮点_宏名:
      e->k = 虚常浮点_种类; e->u.n值_缩 = 对象_浮点值_宏名(v);
      break;
    case 对象_月亮_V假_宏名:
      e->k = 虚假_种类;
      break;
    case 对象_月亮_V真_宏名:
      e->k = 虚真_种类;
      break;
    case 对象_月亮_V空值_宏名:
      e->k = 虚空值_种类;
      break;
    case 对象_月亮_V短型串_宏名:  case 对象_月亮_V长型串_宏名:
      e->k = 虚常串_种类; e->u.串值_短 = 对象_ts值_宏名(v);
      break;
    default: 限制_月亮_断言_宏名(0);
  }
}


/*
** Fix an expression 到_变量 return the number of results '结果数目_变量'.
** 'e' must be a multi-返回_短变量 expression (function call or vararg).
*/
void 月亮常量_设置返回值_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e, int 结果数目_变量) {
  Instruction *程序计数_变量 = &代码_获取指令_宏名(字段静态_变量, e);
  if (e->k == 虚调用_种类)  /* expression is an 打开_圆 function call? */
    操作码_设置实参_C_宏名(*程序计数_变量, 结果数目_变量 + 1);
  else {
    限制_月亮_断言_宏名(e->k == 虚变量实参_种类);
    操作码_设置实参_C_宏名(*程序计数_变量, 结果数目_变量 + 1);
    操作码_设置实参_A_宏名(*程序计数_变量, 字段静态_变量->是否寄存_短);
    月亮常量_保留寄存器们_函(字段静态_变量, 1);
  }
}


/*
** Convert a 虚常串_种类 到_变量 a 虚常_种类
*/
static void 月代码_串到常量_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  限制_月亮_断言_宏名(e->k == 虚常串_种类);
  e->u.信息_短变量 = 月代码_字符串常量_函(字段静态_变量, e->u.串值_短);
  e->k = 虚常_种类;
}


/*
** Fix an expression 到_变量 return one 结果_变量.
** If expression is not a multi-返回_短变量 expression (function call or
** vararg), it already returns one 结果_变量, so nothing needs 到_变量 be done.
** Function calls become 虚没有重定位_种类 expressions (as its 结果_变量 comes
** fixed in the 基本_变量 register of the call), while vararg expressions
** become 虚重定位_种类 (as 操作_变量实参 puts its results 哪儿_变量 it wants).
** (Calls are created returning one 结果_变量, so that does not need
** 到_变量 be fixed.)
*/
void 月亮常量_设置一个返回_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (e->k == 虚调用_种类) {  /* expression is an 打开_圆 function call? */
    /* already returns 1 值_圆 */
    限制_月亮_断言_宏名(操作码_获取实参_C_宏名(代码_获取指令_宏名(字段静态_变量, e)) == 2);
    e->k = 虚没有重定位_种类;  /* 结果_变量 has fixed position */
    e->u.信息_短变量 = 操作码_获取实参_A_宏名(代码_获取指令_宏名(字段静态_变量, e));
  }
  else if (e->k == 虚变量实参_种类) {
    操作码_设置实参_C_宏名(代码_获取指令_宏名(字段静态_变量, e), 2);
    e->k = 虚重定位_种类;  /* can relocate its simple 结果_变量 */
  }
}


/*
** Ensure that expression 'e' is not a variable (nor a <const>).
** (Expression still may have jump lists.)
*/
void 月亮常量_卸货变量们_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  switch (e->k) {
    case 虚常字面_种类: {
      月代码_常量到表达式_函(月代码_常量到值_函(字段静态_变量, e), e);
      break;
    }
    case 虚本地_种类: {  /* already in a register */
      e->u.信息_短变量 = e->u.变_短变量.寄索引_缩短;
      e->k = 虚没有重定位_种类;  /* becomes a non-relocatable 值_圆 */
      break;
    }
    case 虚上值_种类: {  /* move 值_圆 到_变量 some (pending) register */
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_获取上值, 0, e->u.信息_短变量, 0);
      e->k = 虚重定位_种类;
      break;
    }
    case 虚索引上值_种类: {
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_获取表上值内, 0, e->u.ind.t, e->u.ind.索引_缩变量);
      e->k = 虚重定位_种类;
      break;
    }
    case 虚索引整_种类: {
      是否寄存_短(字段静态_变量, e->u.ind.t);
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_获取整, 0, e->u.ind.t, e->u.ind.索引_缩变量);
      e->k = 虚重定位_种类;
      break;
    }
    case 虚索引串_种类: {
      是否寄存_短(字段静态_变量, e->u.ind.t);
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_获取字段, 0, e->u.ind.t, e->u.ind.索引_缩变量);
      e->k = 虚重定位_种类;
      break;
    }
    case 虚已索引_种类: {
      月代码_释放寄存器们_函(字段静态_变量, e->u.ind.t, e->u.ind.索引_缩变量);
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_获取表, 0, e->u.ind.t, e->u.ind.索引_缩变量);
      e->k = 虚重定位_种类;
      break;
    }
    case 虚变量实参_种类: case 虚调用_种类: {
      月亮常量_设置一个返回_函(字段静态_变量, e);
      break;
    }
    default: break;  /* there is one 值_圆 available (somewhere) */
  }
}


/*
** Ensure expression 值_圆 is in register '寄存_短变量', making 'e' a
** non-relocatable expression.
** (Expression still may have jump lists.)
*/
static void 月代码_卸货到寄存器_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e, int 寄存_短变量) {
  月亮常量_卸货变量们_函(字段静态_变量, e);
  switch (e->k) {
    case 虚空值_种类: {
      月亮常量_空值_函(字段静态_变量, 寄存_短变量, 1);
      break;
    }
    case 虚假_种类: {
      代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_载入假, 寄存_短变量, 0, 0);
      break;
    }
    case 虚真_种类: {
      代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_载入真, 寄存_短变量, 0, 0);
      break;
    }
    case 虚常串_种类: {
      月代码_串到常量_函(字段静态_变量, e);
    }  /* FALLTHROUGH */
    case 虚常_种类: {
      月代码_月亮常量_代码常量_函(字段静态_变量, 寄存_短变量, e->u.信息_短变量);
      break;
    }
    case 虚常浮点_种类: {
      月代码_月亮常量_浮点_函(字段静态_变量, 寄存_短变量, e->u.n值_缩);
      break;
    }
    case 虚常整型_种类: {
      月亮常量_整型_函(字段静态_变量, 寄存_短变量, e->u.i值_缩);
      break;
    }
    case 虚重定位_种类: {
      Instruction *程序计数_变量 = &代码_获取指令_宏名(字段静态_变量, e);
      操作码_设置实参_A_宏名(*程序计数_变量, 寄存_短变量);  /* instruction will put 结果_变量 in '寄存_短变量' */
      break;
    }
    case 虚没有重定位_种类: {
      if (寄存_短变量 != e->u.信息_短变量)
        代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_移, 寄存_短变量, e->u.信息_短变量, 0);
      break;
    }
    default: {
      限制_月亮_断言_宏名(e->k == 虚跳转_种类);
      return;  /* nothing 到_变量 do... */
    }
  }
  e->u.信息_短变量 = 寄存_短变量;
  e->k = 虚没有重定位_种类;
}


/*
** Ensure expression 值_圆 is in a register, making 'e' a
** non-relocatable expression.
** (Expression still may have jump lists.)
*/
static void 月代码_卸货到任意寄存器_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (e->k != 虚没有重定位_种类) {  /* no fixed register yet? */
    月亮常量_保留寄存器们_函(字段静态_变量, 1);  /* get a register */
    月代码_卸货到寄存器_函(字段静态_变量, e, 字段静态_变量->是否寄存_短-1);  /* put 值_圆 there */
  }
}


static int 月代码_代码_加载布尔_函 (函状态机_结 *字段静态_变量, int A, 操作码_枚举 操作_短变量) {
  月亮常量_获取标号_函(字段静态_变量);  /* those instructions may be jump targets */
  return 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_短变量, A, 0, 0);
}


/*
** 月解析器_检查_函 whether 列表_变量 has any jump that do not produce a 值_圆
** or produce an inverted 值_圆
*/
static int 月代码_需要_值_函 (函状态机_结 *字段静态_变量, int 列表_变量) {
  for (; 列表_变量 != 代码_无_跳转_宏名; 列表_变量 = 月代码_获取跳转_函(字段静态_变量, 列表_变量)) {
    Instruction i = *月代码_获取跳转控制_函(字段静态_变量, 列表_变量);
    if (操作码_获取_操作码_宏名(i) != 操作_测试设置) return 1;
  }
  return 0;  /* not found */
}


/*
** Ensures 最终_变量 expression 结果_变量 (which includes results from its
** jump lists) is in register '寄存_短变量'.
** If expression has jumps, need 到_变量 patch these jumps either 到_变量
** its 最终_变量 position or 到_变量 "load" instructions (for those tests
** that do not produce values).
*/
static void 月代码_表达式到寄存器_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e, int 寄存_短变量) {
  月代码_卸货到寄存器_函(字段静态_变量, e, 寄存_短变量);
  if (e->k == 虚跳转_种类)  /* expression itself is a test? */
    月亮常量_拼接_函(字段静态_变量, &e->t, e->u.信息_短变量);  /* put this jump in 't' 列表_变量 */
  if (代码_有跳转_宏名(e)) {
    int 最终_变量;  /* position after whole expression */
    int 来自_位置_变量 = 代码_无_跳转_宏名;  /* position of an eventual LOAD false */
    int 到_位置_变量 = 代码_无_跳转_宏名;  /* position of an eventual LOAD true */
    if (月代码_需要_值_函(字段静态_变量, e->t) || 月代码_需要_值_函(字段静态_变量, e->f)) {
      int 函跳_缩变量 = (e->k == 虚跳转_种类) ? 代码_无_跳转_宏名 : 月亮常量_跳转_函(字段静态_变量);
      来自_位置_变量 = 月代码_代码_加载布尔_函(字段静态_变量, 寄存_短变量, 操作_载入假跳过);  /* skip 下一个_变量 指令_短变量. */
      到_位置_变量 = 月代码_代码_加载布尔_函(字段静态_变量, 寄存_短变量, 操作_载入真);
      /* jump around these booleans if 'e' is not a test */
      月亮常量_补丁到这里_函(字段静态_变量, 函跳_缩变量);
    }
    最终_变量 = 月亮常量_获取标号_函(字段静态_变量);
    月代码_补丁列表辅助_函(字段静态_变量, e->f, 最终_变量, 寄存_短变量, 来自_位置_变量);
    月代码_补丁列表辅助_函(字段静态_变量, e->t, 最终_变量, 寄存_短变量, 到_位置_变量);
  }
  e->f = e->t = 代码_无_跳转_宏名;
  e->u.信息_短变量 = 寄存_短变量;
  e->k = 虚没有重定位_种类;
}


/*
** Ensures 最终_变量 expression 结果_变量 is in 下一个_变量 available register.
*/
void 月亮常量_表达式到下一个寄存器_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  月亮常量_卸货变量们_函(字段静态_变量, e);
  月代码_释放表达式_函(字段静态_变量, e);
  月亮常量_保留寄存器们_函(字段静态_变量, 1);
  月代码_表达式到寄存器_函(字段静态_变量, e, 字段静态_变量->是否寄存_短 - 1);
}


/*
** Ensures 最终_变量 expression 结果_变量 is in some (any) register
** and return that register.
*/
int 月亮常量_表达式到任意寄存器_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  月亮常量_卸货变量们_函(字段静态_变量, e);
  if (e->k == 虚没有重定位_种类) {  /* expression already has a register? */
    if (!代码_有跳转_宏名(e))  /* no jumps? */
      return e->u.信息_短变量;  /* 结果_变量 is already in a register */
    if (e->u.信息_短变量 >= 月亮解析_n变量栈_函(字段静态_变量)) {  /* 寄存_短变量. is not a local? */
      月代码_表达式到寄存器_函(字段静态_变量, e, e->u.信息_短变量);  /* put 最终_变量 结果_变量 in it */
      return e->u.信息_短变量;
    }
    /* else expression has jumps and 月编译器_不能_函 改变_变量 its register
       到_变量 hold the jump values, because it is a local variable.
       Go through 到_变量 the default case. */
  }
  月亮常量_表达式到下一个寄存器_函(字段静态_变量, e);  /* default: use 下一个_变量 available register */
  return e->u.信息_短变量;
}


/*
** Ensures 最终_变量 expression 结果_变量 is either in a register
** or in an 上值_圆.
*/
void 月亮常量_表达式到任意寄存器更新_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (e->k != 虚上值_种类 || 代码_有跳转_宏名(e))
    月亮常量_表达式到任意寄存器_函(字段静态_变量, e);
}


/*
** Ensures 最终_变量 expression 结果_变量 is either in a register
** or it is a constant.
*/
void 月亮常量_表达式到值_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (代码_有跳转_宏名(e))
    月亮常量_表达式到任意寄存器_函(字段静态_变量, e);
  else
    月亮常量_卸货变量们_函(字段静态_变量, e);
}


/*
** Try 到_变量 make 'e' a K expression with an index in the range of R/K
** indices. Return true iff succeeded.
*/
static int 月代码_月亮常量_表达式到常量_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (!代码_有跳转_宏名(e)) {
    int 信息_短变量;
    switch (e->k) {  /* move constants 到_变量 'k' */
      case 虚真_种类: 信息_短变量 = 月代码_布尔真_函(字段静态_变量); break;
      case 虚假_种类: 信息_短变量 = 月代码_布尔假_函(字段静态_变量); break;
      case 虚空值_种类: 信息_短变量 = 月代码_空值常量_函(字段静态_变量); break;
      case 虚常整型_种类: 信息_短变量 = 月代码_月亮常量_整型常量_函(字段静态_变量, e->u.i值_缩); break;
      case 虚常浮点_种类: 信息_短变量 = 月代码_月亮常量_数目常量_函(字段静态_变量, e->u.n值_缩); break;
      case 虚常串_种类: 信息_短变量 = 月代码_字符串常量_函(字段静态_变量, e->u.串值_短); break;
      case 虚常_种类: 信息_短变量 = e->u.信息_短变量; break;
      default: return 0;  /* not a constant */
    }
    if (信息_短变量 <= 操作码_最大索引RK_宏名) {  /* does constant fit in 'argC'? */
      e->k = 虚常_种类;  /* make expression a 'K' expression */
      e->u.信息_短变量 = 信息_短变量;
      return 1;
    }
  }
  /* else, expression doesn't fit; leave it unchanged */
  return 0;
}


/*
** Ensures 最终_变量 expression 结果_变量 is in a 有效_变量 R/K index
** (that is, it is either in a register or in 'k' with an index
** in the range of R/K indices).
** Returns 1 iff expression is K.
*/
int 月亮常量_表达式到RK_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  if (月代码_月亮常量_表达式到常量_函(字段静态_变量, e))
    return 1;
  else {  /* not a constant in the 右_圆 range: put it in a register */
    月亮常量_表达式到任意寄存器_函(字段静态_变量, e);
    return 0;
  }
}


static void 月代码_代码ABRK_函 (函状态机_结 *字段静态_变量, 操作码_枚举 o, int a, int b,
                      表达式描述_结 *ec) {
  int k = 月亮常量_表达式到RK_函(字段静态_变量, ec);
  月亮常量_代码ABCk_函(字段静态_变量, o, a, b, ec->u.信息_短变量, k);
}


/*
** Generate 代码_变量 到_变量 store 结果_变量 of expression 'ex' into variable '变_短变量'.
*/
void 月亮常量_存储变量_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *变_短变量, 表达式描述_结 *ex) {
  switch (变_短变量->k) {
    case 虚本地_种类: {
      月代码_释放表达式_函(字段静态_变量, ex);
      月代码_表达式到寄存器_函(字段静态_变量, ex, 变_短变量->u.变_短变量.寄索引_缩短);  /* compute 'ex' into proper place */
      return;
    }
    case 虚上值_种类: {
      int e = 月亮常量_表达式到任意寄存器_函(字段静态_变量, ex);
      代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_设置上值, e, 变_短变量->u.信息_短变量, 0);
      break;
    }
    case 虚索引上值_种类: {
      月代码_代码ABRK_函(字段静态_变量, 操作_设置表上值内, 变_短变量->u.ind.t, 变_短变量->u.ind.索引_缩变量, ex);
      break;
    }
    case 虚索引整_种类: {
      月代码_代码ABRK_函(字段静态_变量, 操作_设置整, 变_短变量->u.ind.t, 变_短变量->u.ind.索引_缩变量, ex);
      break;
    }
    case 虚索引串_种类: {
      月代码_代码ABRK_函(字段静态_变量, 操作_设置字段, 变_短变量->u.ind.t, 变_短变量->u.ind.索引_缩变量, ex);
      break;
    }
    case 虚已索引_种类: {
      月代码_代码ABRK_函(字段静态_变量, 操作_设置表, 变_短变量->u.ind.t, 变_短变量->u.ind.索引_缩变量, ex);
      break;
    }
    default: 限制_月亮_断言_宏名(0);  /* invalid 变_短变量 种类_变量 到_变量 store */
  }
  月代码_释放表达式_函(字段静态_变量, ex);
}


/*
** Emit SELF instruction (convert expression 'e' into 'e:键_小变量(e,').
*/
void 月亮常量_自身_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e, 表达式描述_结 *键_小变量) {
  int 扩正则_缩变量;
  月亮常量_表达式到任意寄存器_函(字段静态_变量, e);
  扩正则_缩变量 = e->u.信息_短变量;  /* register 哪儿_变量 'e' was placed */
  月代码_释放表达式_函(字段静态_变量, e);
  e->u.信息_短变量 = 字段静态_变量->是否寄存_短;  /* 基本_变量 register for op_self */
  e->k = 虚没有重定位_种类;  /* self expression has a fixed register */
  月亮常量_保留寄存器们_函(字段静态_变量, 2);  /* function and 'self' produced by op_self */
  月代码_代码ABRK_函(字段静态_变量, 操作_自身, e->u.信息_短变量, 扩正则_缩变量, 键_小变量);
  月代码_释放表达式_函(字段静态_变量, 键_小变量);
}


/*
** Negate condition 'e' (哪儿_变量 'e' is a comparison).
*/
static void 月代码_否定条件_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  Instruction *程序计数_变量 = 月代码_获取跳转控制_函(字段静态_变量, e->u.信息_短变量);
  限制_月亮_断言_宏名(操作码_测试T模式_宏名(操作码_获取_操作码_宏名(*程序计数_变量)) && 操作码_获取_操作码_宏名(*程序计数_变量) != 操作_测试设置 &&
                                           操作码_获取_操作码_宏名(*程序计数_变量) != 操作_测试);
  操作码_设置实参_k_宏名(*程序计数_变量, (操作码_获取实参_k_宏名(*程序计数_变量) ^ 1));
}


/*
** Emit instruction 到_变量 jump if 'e' is '条件_变量' (that is, if '条件_变量'
** is true, 代码_变量 will jump if 'e' is true.) Return jump position.
** Optimize when 'e' is 'not' something, inverting the condition
** and removing the 'not'.
*/
static int 月代码_在条件上跳转_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e, int 条件_变量) {
  if (e->k == 虚重定位_种类) {
    Instruction ie = 代码_获取指令_宏名(字段静态_变量, e);
    if (操作码_获取_操作码_宏名(ie) == 操作_非) {
      月代码_移除最后指令_函(字段静态_变量);  /* remove 前一个_变量 操作_非 */
      return 月代码_条件跳转_函(字段静态_变量, 操作_测试, 操作码_获取实参_B_宏名(ie), 0, 0, !条件_变量);
    }
    /* else go through */
  }
  月代码_卸货到任意寄存器_函(字段静态_变量, e);
  月代码_释放表达式_函(字段静态_变量, e);
  return 月代码_条件跳转_函(字段静态_变量, 操作_测试设置, 操作码_无_寄存器_宏名, e->u.信息_短变量, 0, 条件_变量);
}


/*
** Emit 代码_变量 到_变量 go through if 'e' is true, jump otherwise.
*/
void 月亮常量_去若真_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  int 程序计数_变量;  /* 程序计数_变量 of new jump */
  月亮常量_卸货变量们_函(字段静态_变量, e);
  switch (e->k) {
    case 虚跳转_种类: {  /* condition? */
      月代码_否定条件_函(字段静态_变量, e);  /* jump when it is false */
      程序计数_变量 = e->u.信息_短变量;  /* 月词法_保存_函 jump position */
      break;
    }
    case 虚常_种类: case 虚常浮点_种类: case 虚常整型_种类: case 虚常串_种类: case 虚真_种类: {
      程序计数_变量 = 代码_无_跳转_宏名;  /* always true; do nothing */
      break;
    }
    default: {
      程序计数_变量 = 月代码_在条件上跳转_函(字段静态_变量, e, 0);  /* jump when false */
      break;
    }
  }
  月亮常量_拼接_函(字段静态_变量, &e->f, 程序计数_变量);  /* insert new jump in false 列表_变量 */
  月亮常量_补丁到这里_函(字段静态_变量, e->t);  /* true 列表_变量 jumps 到_变量 here (到_变量 go through) */
  e->t = 代码_无_跳转_宏名;
}


/*
** Emit 代码_变量 到_变量 go through if 'e' is false, jump otherwise.
*/
void 月亮常量_去若假_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  int 程序计数_变量;  /* 程序计数_变量 of new jump */
  月亮常量_卸货变量们_函(字段静态_变量, e);
  switch (e->k) {
    case 虚跳转_种类: {
      程序计数_变量 = e->u.信息_短变量;  /* already jump if true */
      break;
    }
    case 虚空值_种类: case 虚假_种类: {
      程序计数_变量 = 代码_无_跳转_宏名;  /* always false; do nothing */
      break;
    }
    default: {
      程序计数_变量 = 月代码_在条件上跳转_函(字段静态_变量, e, 1);  /* jump if true */
      break;
    }
  }
  月亮常量_拼接_函(字段静态_变量, &e->t, 程序计数_变量);  /* insert new jump in 't' 列表_变量 */
  月亮常量_补丁到这里_函(字段静态_变量, e->f);  /* false 列表_变量 jumps 到_变量 here (到_变量 go through) */
  e->f = 代码_无_跳转_宏名;
}


/*
** Code 'not e', doing constant folding.
*/
static void 月代码_代码非_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  switch (e->k) {
    case 虚空值_种类: case 虚假_种类: {
      e->k = 虚真_种类;  /* true == not nil == not false */
      break;
    }
    case 虚常_种类: case 虚常浮点_种类: case 虚常整型_种类: case 虚常串_种类: case 虚真_种类: {
      e->k = 虚假_种类;  /* false == not "x" == not 0.5 == not 1 == not true */
      break;
    }
    case 虚跳转_种类: {
      月代码_否定条件_函(字段静态_变量, e);
      break;
    }
    case 虚重定位_种类:
    case 虚没有重定位_种类: {
      月代码_卸货到任意寄存器_函(字段静态_变量, e);
      月代码_释放表达式_函(字段静态_变量, e);
      e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_非, 0, e->u.信息_短变量, 0);
      e->k = 虚重定位_种类;
      break;
    }
    default: 限制_月亮_断言_宏名(0);  /* 月编译器_不能_函 happen */
  }
  /* interchange true and false lists */
  { int 临时_变量 = e->f; e->f = e->t; e->t = 临时_变量; }
  月代码_移除值们_函(字段静态_变量, e->f);  /* values are useless when negated */
  月代码_移除值们_函(字段静态_变量, e->t);
}


/*
** Check whether expression 'e' is a small literal string
*/
static int 月代码_是否常量串_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e) {
  return (e->k == 虚常_种类 && !代码_有跳转_宏名(e) && e->u.信息_短变量 <= 操作码_最大实参_B_宏名 &&
          对象_tt是否短型字符串_宏名(&字段静态_变量->f->k[e->u.信息_短变量]));
}

/*
** Check whether expression 'e' is a literal integer.
*/
int 月亮常量_是否为整数常量_函 (表达式描述_结 *e) {
  return (e->k == 虚常整型_种类 && !代码_有跳转_宏名(e));
}


/*
** Check whether expression 'e' is a literal integer in
** proper range 到_变量 fit in register C
*/
static int 月代码_是否C整型_函 (表达式描述_结 *e) {
  return 月亮常量_是否为整数常量_函(e) && (限制_l_类型转换符到无符_宏名(e->u.i值_缩) <= 限制_l_类型转换符到无符_宏名(操作码_最大实参_C_宏名));
}


/*
** Check whether expression 'e' is a literal integer in
** proper range 到_变量 fit in register sC
*/
static int 月代码_是否SC整型_函 (表达式描述_结 *e) {
  return 月亮常量_是否为整数常量_函(e) && 月代码_适合sC_函(e->u.i值_缩);
}


/*
** Check whether expression 'e' is a literal integer or float in
** proper range 到_变量 fit in a register (sB or sC).
*/
static int 月代码_是否SC数目_函 (表达式描述_结 *e, int *圆周率_变量, int *是否浮点_变量) {
  炉_整数型 i;
  if (e->k == 虚常整型_种类)
    i = e->u.i值_缩;
  else if (e->k == 虚常浮点_种类 && 月亮虚拟机_浮点数到整数_函(e->u.n值_缩, &i, 浮到整相等_))
    *是否浮点_变量 = 1;
  else
    return 0;  /* not a number */
  if (!代码_有跳转_宏名(e) && 月代码_适合sC_函(i)) {
    *圆周率_变量 = 操作码_整型到sC_宏名(限制_类型转换_整型_宏名(i));
    return 1;
  }
  else
    return 0;
}


/*
** Create expression 't[k]'. 't' must have its 最终_变量 结果_变量 already in a
** register or 上值_圆. Upvalues can only be indexed by literal strings.
** Keys can be literal strings in the constant table or arbitrary
** values in registers.
*/
void 月亮常量_已索引_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *t, 表达式描述_结 *k) {
  if (k->k == 虚常串_种类)
    月代码_串到常量_函(字段静态_变量, k);
  限制_月亮_断言_宏名(!代码_有跳转_宏名(t) &&
             (t->k == 虚本地_种类 || t->k == 虚没有重定位_种类 || t->k == 虚上值_种类));
  if (t->k == 虚上值_种类 && !月代码_是否常量串_函(字段静态_变量, k))  /* 上值_圆 indexed by non 'Kstr'? */
    月亮常量_表达式到任意寄存器_函(字段静态_变量, t);  /* put it in a register */
  if (t->k == 虚上值_种类) {
    t->u.ind.t = t->u.信息_短变量;  /* 上值_圆 index */
    t->u.ind.索引_缩变量 = k->u.信息_短变量;  /* literal string */
    t->k = 虚索引上值_种类;
  }
  else {
    /* register index of the table */
    t->u.ind.t = (t->k == 虚本地_种类) ? t->u.变_短变量.寄索引_缩短: t->u.信息_短变量;
    if (月代码_是否常量串_函(字段静态_变量, k)) {
      t->u.ind.索引_缩变量 = k->u.信息_短变量;  /* literal string */
      t->k = 虚索引串_种类;
    }
    else if (月代码_是否C整型_函(k)) {
      t->u.ind.索引_缩变量 = 限制_类型转换_整型_宏名(k->u.i值_缩);  /* int. constant in proper range */
      t->k = 虚索引整_种类;
    }
    else {
      t->u.ind.索引_缩变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, k);  /* register */
      t->k = 虚已索引_种类;
    }
  }
}


/*
** Return false if folding can raise an 错误_小变量.
** Bitwise operations need operands convertible 到_变量 integers; division
** operations 月编译器_不能_函 have 0 as divisor.
*/
static int 月代码_有效操作_函 (int 操作_短变量, 标签值_结 *变1_缩变量, 标签值_结 *变2_缩变量) {
  switch (操作_短变量) {
    case 月头_月亮_操作按位与_宏名: case 月头_月亮_操作按位或_宏名: case 月头_月亮_操作按位异或_宏名:
    case 月头_月亮_操作左移_宏名: case 月头_月亮_操作右移_宏名: case 月头_月亮_操作按位非_宏名: {  /* conversion errors */
      炉_整数型 i;
      return (月亮虚拟机_非安全到整数_函(变1_缩变量, &i, 虚机头_月亮_向下取整数目到整数_宏名) &&
              月亮虚拟机_非安全到整数_函(变2_缩变量, &i, 虚机头_月亮_向下取整数目到整数_宏名));
    }
    case 月头_月亮_操作除法_宏名: case 月头_月亮_操作整数除_宏名: case 月头_月亮_操作取模_宏名:  /* division by 0 */
      return (对象_数目值_宏名(变2_缩变量) != 0);
    default: return 1;  /* everything else is 有效_变量 */
  }
}


/*
** Try 到_变量 "constant-fold" an operation; return 1 iff successful.
** (In this case, 'e1' has the 最终_变量 结果_变量.)
*/
static int 月代码_常量折叠_函 (函状态机_结 *字段静态_变量, int 操作_短变量, 表达式描述_结 *e1,
                                        const 表达式描述_结 *e2) {
  标签值_结 变1_缩变量, 变2_缩变量, 结果_短变量;
  if (!月代码_到数值的_函(e1, &变1_缩变量) || !月代码_到数值的_函(e2, &变2_缩变量) || !月代码_有效操作_函(操作_短变量, &变1_缩变量, &变2_缩变量))
    return 0;  /* non-numeric operands or not safe 到_变量 fold */
  月亮对象_原始算术_函(字段静态_变量->状列_缩变量->L, 操作_短变量, &变1_缩变量, &变2_缩变量, &结果_短变量);  /* does operation */
  if (对象_tt是否整数_宏名(&结果_短变量)) {
    e1->k = 虚常整型_种类;
    e1->u.i值_缩 = 对象_整数值_宏名(&结果_短变量);
  }
  else {  /* folds neither NaN nor 0.0 (到_变量 avoid problems with -0.0) */
    炉_数目型 n = 对象_浮点值_宏名(&结果_短变量);
    if (限制_月亮i_数目是否nan_宏名(n) || n == 0)
      return 0;
    e1->k = 虚常浮点_种类;
    e1->u.n值_缩 = n;
  }
  return 1;
}


/*
** Convert a 二元操作者_枚举 到_变量 an 操作码_枚举  (ORDER OPR - ORDER OP)
*/
l_sinline 操作码_枚举 月代码_二元操作r到操作_函 (二元操作者_枚举 opr, 二元操作者_枚举 baser, 操作码_枚举 基本_变量) {
  限制_月亮_断言_宏名(baser <= opr &&
            ((baser == 操作者_加法 && opr <= 操作者_右移) ||
             (baser == 操作者_小于 && opr <= 操作者_小等)));
  return 限制_类型转换_宏名(操作码_枚举, (限制_类型转换_整型_宏名(opr) - 限制_类型转换_整型_宏名(baser)) + 限制_类型转换_整型_宏名(基本_变量));
}


/*
** Convert a 一元操作者_枚举 到_变量 an 操作码_枚举  (ORDER OPR - ORDER OP)
*/
l_sinline 操作码_枚举 月代码_一元操作r到操作_函 (一元操作者_枚举 opr) {
  return 限制_类型转换_宏名(操作码_枚举, (限制_类型转换_整型_宏名(opr) - 限制_类型转换_整型_宏名(操作者_减号)) +
                                       限制_类型转换_整型_宏名(操作_一元减号));
}


/*
** Convert a 二元操作者_枚举 到_变量 a tag method  (ORDER OPR - ORDER TM)
*/
l_sinline 标方符_枚举 月代码_二元操作r到标签方法_函 (二元操作者_枚举 opr) {
  限制_月亮_断言_宏名(操作者_加法 <= opr && opr <= 操作者_右移);
  return 限制_类型转换_宏名(标方符_枚举, (限制_类型转换_整型_宏名(opr) - 限制_类型转换_整型_宏名(操作者_加法)) + 限制_类型转换_整型_宏名(标方_加大写));
}


/*
** Emit 代码_变量 for unary expressions that "produce values"
** (everything but 'not').
** Expression 到_变量 produce 最终_变量 结果_变量 will be encoded in 'e'.
*/
static void 月代码_代码一元表达式值_函 (函状态机_结 *字段静态_变量, 操作码_枚举 操作_短变量, 表达式描述_结 *e, int 行_变量) {
  int r = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e);  /* opcodes operate only on registers */
  月代码_释放表达式_函(字段静态_变量, e);
  e->u.信息_短变量 = 代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_短变量, 0, r, 0);  /* generate opcode */
  e->k = 虚重定位_种类;  /* all those operations are relocatable */
  月亮常量_修正行号_函(字段静态_变量, 行_变量);
}


/*
** Emit 代码_变量 for binary expressions that "produce values"
** (everything but logical operators 'and'/'or' and comparison
** operators).
** Expression 到_变量 produce 最终_变量 结果_变量 will be encoded in 'e1'.
*/
static void 月代码_完成二元表达式值_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e1, 表达式描述_结 *e2,
                             操作码_枚举 操作_短变量, int 变2_缩变量, int 折叠_变量, int 行_变量,
                             操作码_枚举 mmop, 标方符_枚举 事件_变量) {
  int 变1_缩变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e1);
  int 程序计数_变量 = 月亮常量_代码ABCk_函(字段静态_变量, 操作_短变量, 0, 变1_缩变量, 变2_缩变量, 0);
  月代码_释放表达式们_函(字段静态_变量, e1, e2);
  e1->u.信息_短变量 = 程序计数_变量;
  e1->k = 虚重定位_种类;  /* all those operations are relocatable */
  月亮常量_修正行号_函(字段静态_变量, 行_变量);
  月亮常量_代码ABCk_函(字段静态_变量, mmop, 变1_缩变量, 变2_缩变量, 事件_变量, 折叠_变量);  /* 到_变量 call metamethod */
  月亮常量_修正行号_函(字段静态_变量, 行_变量);
}


/*
** Emit 代码_变量 for binary expressions that "produce values" over
** two registers.
*/
static void 月代码_代码二元表达式值_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                           表达式描述_结 *e1, 表达式描述_结 *e2, int 行_变量) {
  操作码_枚举 操作_短变量 = 月代码_二元操作r到操作_函(opr, 操作者_加法, 操作_加);
  int 变2_缩变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e2);  /* make sure 'e2' is in a register */
  /* 'e1' must be already in a register or it is a constant */
  限制_月亮_断言_宏名((虚空值_种类 <= e1->k && e1->k <= 虚常串_种类) ||
             e1->k == 虚没有重定位_种类 || e1->k == 虚重定位_种类);
  限制_月亮_断言_宏名(操作_加 <= 操作_短变量 && 操作_短变量 <= 操作_右移);
  月代码_完成二元表达式值_函(字段静态_变量, e1, e2, 操作_短变量, 变2_缩变量, 0, 行_变量, 操作_元方法二元, 月代码_二元操作r到标签方法_函(opr));
}


/*
** Code binary operators with immediate operands.
*/
static void 月代码_代码二元索引_函 (函状态机_结 *字段静态_变量, 操作码_枚举 操作_短变量,
                       表达式描述_结 *e1, 表达式描述_结 *e2, int 折叠_变量, int 行_变量,
                       标方符_枚举 事件_变量) {
  int 变2_缩变量 = 操作码_整型到sC_宏名(限制_类型转换_整型_宏名(e2->u.i值_缩));  /* immediate operand */
  限制_月亮_断言_宏名(e2->k == 虚常整型_种类);
  月代码_完成二元表达式值_函(字段静态_变量, e1, e2, 操作_短变量, 变2_缩变量, 折叠_变量, 行_变量, 操作_元方法二元整, 事件_变量);
}


/*
** Code binary operators with K operand.
*/
static void 月代码_代码二元常量_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                      表达式描述_结 *e1, 表达式描述_结 *e2, int 折叠_变量, int 行_变量) {
  标方符_枚举 事件_变量 = 月代码_二元操作r到标签方法_函(opr);
  int 变2_缩变量 = e2->u.信息_短变量;  /* K index */
  操作码_枚举 操作_短变量 = 月代码_二元操作r到操作_函(opr, 操作者_加法, 操作_加常);
  月代码_完成二元表达式值_函(字段静态_变量, e1, e2, 操作_短变量, 变2_缩变量, 折叠_变量, 行_变量, 操作_元方法二元常, 事件_变量);
}


/* Try 到_变量 代码_变量 a binary operator negating its second operand.
** For the metamethod, 2nd operand must keep its original 值_圆.
*/
static int 月代码_完成二元表达式否定_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e1, 表达式描述_结 *e2,
                             操作码_枚举 操作_短变量, int 行_变量, 标方符_枚举 事件_变量) {
  if (!月亮常量_是否为整数常量_函(e2))
    return 0;  /* not an integer constant */
  else {
    炉_整数型 索2_缩变量 = e2->u.i值_缩;
    if (!(月代码_适合sC_函(索2_缩变量) && 月代码_适合sC_函(-索2_缩变量)))
      return 0;  /* not in the proper range */
    else {  /* operating a small integer constant */
      int 变2_缩变量 = 限制_类型转换_整型_宏名(索2_缩变量);
      月代码_完成二元表达式值_函(字段静态_变量, e1, e2, 操作_短变量, 操作码_整型到sC_宏名(-变2_缩变量), 0, 行_变量, 操作_元方法二元整, 事件_变量);
      /* correct metamethod argument */
      操作码_设置实参_B_宏名(字段静态_变量->f->代码_变量[字段静态_变量->程序计数_变量 - 1], 操作码_整型到sC_宏名(变2_缩变量));
      return 1;  /* successfully coded */
    }
  }
}


static void 月代码_交换表达式们_函 (表达式描述_结 *e1, 表达式描述_结 *e2) {
  表达式描述_结 临时_变量 = *e1; *e1 = *e2; *e2 = 临时_变量;  /* swap 'e1' and 'e2' */
}


/*
** Code binary operators with no constant operand.
*/
static void 月代码_代码二元无常量_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                        表达式描述_结 *e1, 表达式描述_结 *e2, int 折叠_变量, int 行_变量) {
  if (折叠_变量)
    月代码_交换表达式们_函(e1, e2);  /* back 到_变量 original order */
  月代码_代码二元表达式值_函(字段静态_变量, opr, e1, e2, 行_变量);  /* use standard operators */
}


/*
** Code arithmetic operators ('+', '-', ...). If second operand is a
** constant in the proper range, use variant opcodes with K operands.
*/
static void 月代码_代码算术_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                       表达式描述_结 *e1, 表达式描述_结 *e2, int 折叠_变量, int 行_变量) {
  if (月代码_到数值的_函(e2, NULL) && 月代码_月亮常量_表达式到常量_函(字段静态_变量, e2))  /* K operand? */
    月代码_代码二元常量_函(字段静态_变量, opr, e1, e2, 折叠_变量, 行_变量);
  else  /* 'e2' is neither an immediate nor a K operand */
    月代码_代码二元无常量_函(字段静态_变量, opr, e1, e2, 折叠_变量, 行_变量);
}


/*
** Code commutative operators ('+', '*'). If 首先_变量 operand is a
** numeric constant, 改变_变量 order of operands 到_变量 try 到_变量 use an
** immediate or K operator.
*/
static void 月代码_代码交换律_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 操作_短变量,
                             表达式描述_结 *e1, 表达式描述_结 *e2, int 行_变量) {
  int 折叠_变量 = 0;
  if (月代码_到数值的_函(e1, NULL)) {  /* is 首先_变量 operand a numeric constant? */
    月代码_交换表达式们_函(e1, e2);  /* 改变_变量 order */
    折叠_变量 = 1;
  }
  if (操作_短变量 == 操作者_加法 && 月代码_是否SC整型_函(e2))  /* immediate operand? */
    月代码_代码二元索引_函(字段静态_变量, 操作_加整, e1, e2, 折叠_变量, 行_变量, 标方_加大写);
  else
    月代码_代码算术_函(字段静态_变量, 操作_短变量, e1, e2, 折叠_变量, 行_变量);
}


/*
** Code bitwise operations; they are all commutative, so the function
** tries 到_变量 put an integer constant as the 2nd operand (a K operand).
*/
static void 月代码_代码位运算_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                         表达式描述_结 *e1, 表达式描述_结 *e2, int 行_变量) {
  int 折叠_变量 = 0;
  if (e1->k == 虚常整型_种类) {
    月代码_交换表达式们_函(e1, e2);  /* 'e2' will be the constant operand */
    折叠_变量 = 1;
  }
  if (e2->k == 虚常整型_种类 && 月代码_月亮常量_表达式到常量_函(字段静态_变量, e2))  /* K operand? */
    月代码_代码二元常量_函(字段静态_变量, opr, e1, e2, 折叠_变量, 行_变量);
  else  /* no constants */
    月代码_代码二元无常量_函(字段静态_变量, opr, e1, e2, 折叠_变量, 行_变量);
}


/*
** Emit 代码_变量 for order comparisons. When using an immediate operand,
** '是否浮点_变量' tells whether the original 值_圆 was a float.
*/
static void 月代码_代码秩序_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr, 表达式描述_结 *e1, 表达式描述_结 *e2) {
  int 寄1_变量, 寄2_变量;
  int 图像_缩变量;
  int 是否浮点_变量 = 0;
  操作码_枚举 操作_短变量;
  if (月代码_是否SC数目_函(e2, &图像_缩变量, &是否浮点_变量)) {
    /* use immediate operand */
    寄1_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e1);
    寄2_变量 = 图像_缩变量;
    操作_短变量 = 月代码_二元操作r到操作_函(opr, 操作者_小于, 操作_小于整);
  }
  else if (月代码_是否SC数目_函(e1, &图像_缩变量, &是否浮点_变量)) {
    /* transform (A < B) 到_变量 (B > A) and (A <= B) 到_变量 (B >= A) */
    寄1_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e2);
    寄2_变量 = 图像_缩变量;
    操作_短变量 = 月代码_二元操作r到操作_函(opr, 操作者_小于, 操作_大于整);
  }
  else {  /* regular case, compare two registers */
    寄1_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e1);
    寄2_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e2);
    操作_短变量 = 月代码_二元操作r到操作_函(opr, 操作者_小于, 操作_小于);
  }
  月代码_释放表达式们_函(字段静态_变量, e1, e2);
  e1->u.信息_短变量 = 月代码_条件跳转_函(字段静态_变量, 操作_短变量, 寄1_变量, 寄2_变量, 是否浮点_变量, 1);
  e1->k = 虚跳转_种类;
}


/*
** Emit 代码_变量 for equality comparisons ('==', '~=').
** 'e1' was already put as RK by '月亮常量_中缀_函'.
*/
static void 月代码_代码相等_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr, 表达式描述_结 *e1, 表达式描述_结 *e2) {
  int 寄1_变量, 寄2_变量;
  int 图像_缩变量;
  int 是否浮点_变量 = 0;  /* not 已需要_变量 here, but kept for symmetry */
  操作码_枚举 操作_短变量;
  if (e1->k != 虚没有重定位_种类) {
    限制_月亮_断言_宏名(e1->k == 虚常_种类 || e1->k == 虚常整型_种类 || e1->k == 虚常浮点_种类);
    月代码_交换表达式们_函(e1, e2);
  }
  寄1_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e1);  /* 1st expression must be in register */
  if (月代码_是否SC数目_函(e2, &图像_缩变量, &是否浮点_变量)) {
    操作_短变量 = 操作_相等整;
    寄2_变量 = 图像_缩变量;  /* immediate operand */
  }
  else if (月亮常量_表达式到RK_函(字段静态_变量, e2)) {  /* 2nd expression is constant? */
    操作_短变量 = 操作_相等常;
    寄2_变量 = e2->u.信息_短变量;  /* constant index */
  }
  else {
    操作_短变量 = 操作_相等;  /* will compare two registers */
    寄2_变量 = 月亮常量_表达式到任意寄存器_函(字段静态_变量, e2);
  }
  月代码_释放表达式们_函(字段静态_变量, e1, e2);
  e1->u.信息_短变量 = 月代码_条件跳转_函(字段静态_变量, 操作_短变量, 寄1_变量, 寄2_变量, 是否浮点_变量, (opr == 操作者_相等));
  e1->k = 虚跳转_种类;
}


/*
** Apply prefix operation '操作_短变量' 到_变量 expression 'e'.
*/
void 月亮常量_前缀_函 (函状态机_结 *字段静态_变量, 一元操作者_枚举 opr, 表达式描述_结 *e, int 行_变量) {
  static const 表达式描述_结 错标_缩变量 = {虚常整型_种类, {0}, 代码_无_跳转_宏名, 代码_无_跳转_宏名};
  月亮常量_卸货变量们_函(字段静态_变量, e);
  switch (opr) {
    case 操作者_减号: case 操作者_位非:  /* use '错标_缩变量' as fake 2nd operand */
      if (月代码_常量折叠_函(字段静态_变量, opr + 月头_月亮_操作负号_宏名, e, &错标_缩变量))
        break;
      /* else */ /* FALLTHROUGH */
    case 操作者_长度:
      月代码_代码一元表达式值_函(字段静态_变量, 月代码_一元操作r到操作_函(opr), e, 行_变量);
      break;
    case 操作者_非: 月代码_代码非_函(字段静态_变量, e); break;
    default: 限制_月亮_断言_宏名(0);
  }
}


/*
** Process 1st operand 'v' of binary operation '操作_短变量' before reading
** 2nd operand.
*/
void 月亮常量_中缀_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 操作_短变量, 表达式描述_结 *v) {
  月亮常量_卸货变量们_函(字段静态_变量, v);
  switch (操作_短变量) {
    case 操作者_与: {
      月亮常量_去若真_函(字段静态_变量, v);  /* go ahead only if 'v' is true */
      break;
    }
    case 操作者_或: {
      月亮常量_去若假_函(字段静态_变量, v);  /* go ahead only if 'v' is false */
      break;
    }
    case 操作者_拼接: {
      月亮常量_表达式到下一个寄存器_函(字段静态_变量, v);  /* operand must be on the 栈_圆小 */
      break;
    }
    case 操作者_加法: case 操作者_减法:
    case 操作者_乘法: case 操作者_除法: case 操作者_整数除:
    case 操作者_取模: case 操作者_幂运算:
    case 操作者_位与: case 操作者_位或: case 操作者_位异或:
    case 操作者_左移: case 操作者_右移: {
      if (!月代码_到数值的_函(v, NULL))
        月亮常量_表达式到任意寄存器_函(字段静态_变量, v);
      /* else keep numeral, which may be folded or used as an immediate
         operand */
      break;
    }
    case 操作者_相等: case 操作者_不等: {
      if (!月代码_到数值的_函(v, NULL))
        月亮常量_表达式到RK_函(字段静态_变量, v);
      /* else keep numeral, which may be an immediate operand */
      break;
    }
    case 操作者_小于: case 操作者_小等:
    case 操作者_大于: case 操作者_大等: {
      int 虚假_变量, 虚假2_变量;
      if (!月代码_是否SC数目_函(v, &虚假_变量, &虚假2_变量))
        月亮常量_表达式到任意寄存器_函(字段静态_变量, v);
      /* else keep numeral, which may be an immediate operand */
      break;
    }
    default: 限制_月亮_断言_宏名(0);
  }
}

/*
** Create 代码_变量 for '(e1 .. e2)'.
** For '(e1 .. e2.1 .. e2.2)' (which is '(e1 .. (e2.1 .. e2.2))',
** because concatenation is 右_圆 associative), merge both CONCATs.
*/
static void 月代码_代码拼接_函 (函状态机_结 *字段静态_变量, 表达式描述_结 *e1, 表达式描述_结 *e2, int 行_变量) {
  Instruction *ie2 = 月代码_前一指令_函(字段静态_变量);
  if (操作码_获取_操作码_宏名(*ie2) == 操作_拼接) {  /* is 'e2' a concatenation? */
    int n = 操作码_获取实参_B_宏名(*ie2);  /* # of elements concatenated in 'e2' */
    限制_月亮_断言_宏名(e1->u.信息_短变量 + 1 == 操作码_获取实参_A_宏名(*ie2));
    月代码_释放表达式_函(字段静态_变量, e2);
    操作码_设置实参_A_宏名(*ie2, e1->u.信息_短变量);  /* correct 首先_变量 element ('e1') */
    操作码_设置实参_B_宏名(*ie2, n + 1);  /* will concatenate one 更多_变量 element */
  }
  else {  /* 'e2' is not a concatenation */
    代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_拼接, e1->u.信息_短变量, 2, 0);  /* new concat opcode */
    月代码_释放表达式_函(字段静态_变量, e2);
    月亮常量_修正行号_函(字段静态_变量, 行_变量);
  }
}


/*
** Finalize 代码_变量 for binary operation, after reading 2nd operand.
*/
void 月亮常量_后缀_函 (函状态机_结 *字段静态_变量, 二元操作者_枚举 opr,
                  表达式描述_结 *e1, 表达式描述_结 *e2, int 行_变量) {
  月亮常量_卸货变量们_函(字段静态_变量, e2);
  if (代码_折叠二元操作_宏名(opr) && 月代码_常量折叠_函(字段静态_变量, opr + 月头_月亮_操作加法_宏名, e1, e2))
    return;  /* done by folding */
  switch (opr) {
    case 操作者_与: {
      限制_月亮_断言_宏名(e1->t == 代码_无_跳转_宏名);  /* 列表_变量 closed by '月亮常量_中缀_函' */
      月亮常量_拼接_函(字段静态_变量, &e2->f, e1->f);
      *e1 = *e2;
      break;
    }
    case 操作者_或: {
      限制_月亮_断言_宏名(e1->f == 代码_无_跳转_宏名);  /* 列表_变量 closed by '月亮常量_中缀_函' */
      月亮常量_拼接_函(字段静态_变量, &e2->t, e1->t);
      *e1 = *e2;
      break;
    }
    case 操作者_拼接: {  /* e1 .. e2 */
      月亮常量_表达式到下一个寄存器_函(字段静态_变量, e2);
      月代码_代码拼接_函(字段静态_变量, e1, e2, 行_变量);
      break;
    }
    case 操作者_加法: case 操作者_乘法: {
      月代码_代码交换律_函(字段静态_变量, opr, e1, e2, 行_变量);
      break;
    }
    case 操作者_减法: {
      if (月代码_完成二元表达式否定_函(字段静态_变量, e1, e2, 操作_加整, 行_变量, 标方_减大写))
        break; /* coded as (寄1_变量 + -I) */
      /* ELSE */
    }  /* FALLTHROUGH */
    case 操作者_除法: case 操作者_整数除: case 操作者_取模: case 操作者_幂运算: {
      月代码_代码算术_函(字段静态_变量, opr, e1, e2, 0, 行_变量);
      break;
    }
    case 操作者_位与: case 操作者_位或: case 操作者_位异或: {
      月代码_代码位运算_函(字段静态_变量, opr, e1, e2, 行_变量);
      break;
    }
    case 操作者_左移: {
      if (月代码_是否SC整型_函(e1)) {
        月代码_交换表达式们_函(e1, e2);
        月代码_代码二元索引_函(字段静态_变量, 操作_左移整, e1, e2, 1, 行_变量, 标方_左移大写);  /* I << 寄2_变量 */
      }
      else if (月代码_完成二元表达式否定_函(字段静态_变量, e1, e2, 操作_右移整, 行_变量, 标方_左移大写)) {
        /* coded as (寄1_变量 >> -I) */;
      }
      else  /* regular case (two registers) */
       月代码_代码二元表达式值_函(字段静态_变量, opr, e1, e2, 行_变量);
      break;
    }
    case 操作者_右移: {
      if (月代码_是否SC整型_函(e2))
        月代码_代码二元索引_函(字段静态_变量, 操作_右移整, e1, e2, 0, 行_变量, 标方_右移大写);  /* 寄1_变量 >> I */
      else  /* regular case (two registers) */
        月代码_代码二元表达式值_函(字段静态_变量, opr, e1, e2, 行_变量);
      break;
    }
    case 操作者_相等: case 操作者_不等: {
      月代码_代码相等_函(字段静态_变量, opr, e1, e2);
      break;
    }
    case 操作者_大于: case 操作者_大等: {
      /* '(a > b)' <=> '(b < a)';  '(a >= b)' <=> '(b <= a)' */
      月代码_交换表达式们_函(e1, e2);
      opr = 限制_类型转换_宏名(二元操作者_枚举, (opr - 操作者_大于) + 操作者_小于);
    }  /* FALLTHROUGH */
    case 操作者_小于: case 操作者_小等: {
      月代码_代码秩序_函(字段静态_变量, opr, e1, e2);
      break;
    }
    default: 限制_月亮_断言_宏名(0);
  }
}


/*
** Change 行_变量 information associated with 当前_圆 position, by removing
** 前一个_变量 信息_短变量 and adding it again with new 行_变量.
*/
void 月亮常量_修正行号_函 (函状态机_结 *字段静态_变量, int 行_变量) {
  月代码_移除最后行信息_函(字段静态_变量);
  月代码_保存行信息_函(字段静态_变量, 字段静态_变量->f, 行_变量);
}


void 月亮常量_设置表大小_函 (函状态机_结 *字段静态_变量, int 程序计数_变量, int 奖励_变量, int 数组大小_缩变量, int hsize) {
  Instruction *指令_短变量 = &字段静态_变量->f->代码_变量[程序计数_变量];
  int 寄b_变量 = (hsize != 0) ? 月亮对象_向上取整对数2_函(hsize) + 1 : 0;  /* 哈希_小写 大小_变量 */
  int 额外_变量 = 数组大小_缩变量 / (操作码_最大实参_C_宏名 + 1);  /* higher bits of 数组_圆 大小_变量 */
  int 寄c_变量 = 数组大小_缩变量 % (操作码_最大实参_C_宏名 + 1);  /* lower bits of 数组_圆 大小_变量 */
  int k = (额外_变量 > 0);  /* true iff needs 额外_变量 argument */
  *指令_短变量 = 操作码_创建_ABCk_宏名(操作_新表, 奖励_变量, 寄b_变量, 寄c_变量, k);
  *(指令_短变量 + 1) = 操作码_创建_Ax_宏名(操作_额外实参, 额外_变量);
}


/*
** Emit a SETLIST instruction.
** '基本_变量' is register that keeps table;
** 'nelems' is #table plus those 到_变量 be stored now;
** '到存储_小写' is number of values (in registers '基本_变量 + 1',...) 到_变量 add 到_变量
** table (or 月头_月亮_多返回_宏名 到_变量 add 上_小变量 到_变量 栈_圆小 顶部_变量).
*/
void 月亮常量_设置列表_函 (函状态机_结 *字段静态_变量, int 基本_变量, int nelems, int 到存储_小写) {
  限制_月亮_断言_宏名(到存储_小写 != 0 && 到存储_小写 <= 操作码_字段_每次_刷新_宏名);
  if (到存储_小写 == 月头_月亮_多返回_宏名)
    到存储_小写 = 0;
  if (nelems <= 操作码_最大实参_C_宏名)
    代码_月亮K_编码ABC_宏名(字段静态_变量, 操作_设置列表, 基本_变量, 到存储_小写, nelems);
  else {
    int 额外_变量 = nelems / (操作码_最大实参_C_宏名 + 1);
    nelems %= (操作码_最大实参_C_宏名 + 1);
    月亮常量_代码ABCk_函(字段静态_变量, 操作_设置列表, 基本_变量, 到存储_小写, nelems, 1);
    月代码_代码额外实参_函(字段静态_变量, 额外_变量);
  }
  字段静态_变量->是否寄存_短 = 基本_变量 + 1;  /* free registers with 列表_变量 values */
}


/*
** return the 最终_变量 目标_变量 of a jump (skipping jumps 到_变量 jumps)
*/
static int 月代码_最终目标_函 (Instruction *代码_变量, int i) {
  int 计数_变量;
  for (计数_变量 = 0; 计数_变量 < 100; 计数_变量++) {  /* avoid infinite loops */
    Instruction 程序计数_变量 = 代码_变量[i];
    if (操作码_获取_操作码_宏名(程序计数_变量) != 操作_跳转)
      break;
     else
       i += 操作码_获取实参_sJ_宏名(程序计数_变量) + 1;
  }
  return i;
}


/*
** Do a 最终_变量 pass over the 代码_变量 of a function, doing small peephole
** optimizations and adjustments.
*/
void 月亮常量_完成_函 (函状态机_结 *字段静态_变量) {
  int i;
  原型_结 *p = 字段静态_变量->f;
  for (i = 0; i < 字段静态_变量->程序计数_变量; i++) {
    Instruction *程序计数_变量 = &p->代码_变量[i];
    限制_月亮_断言_宏名(i == 0 || 操作码_是否OT_宏名(*(程序计数_变量 - 1)) == 操作码_是否IT_宏名(*程序计数_变量));
    switch (操作码_获取_操作码_宏名(*程序计数_变量)) {
      case 操作_返回0: case 操作_返回1: {
        if (!(字段静态_变量->需要关闭_圆 || p->是否_变量实参短))
          break;  /* no 额外_变量 工作_变量 */
        /* else use 操作_返回 到_变量 do the 额外_变量 工作_变量 */
        操作码_设置_操作码_宏名(*程序计数_变量, 操作_返回);
      }  /* FALLTHROUGH */
      case 操作_返回: case 操作_尾调用: {
        if (字段静态_变量->需要关闭_圆)
          操作码_设置实参_k_宏名(*程序计数_变量, 1);  /* signal that it needs 到_变量 关闭_圆 */
        if (p->是否_变量实参短)
          操作码_设置实参_C_宏名(*程序计数_变量, p->形参数_小写 + 1);  /* signal that it is vararg */
        break;
      }
      case 操作_跳转: {
        int 目标_变量 = 月代码_最终目标_函(p->代码_变量, i);
        月代码_修复跳转_函(字段静态_变量, i, 目标_变量);
        break;
      }
      default: break;
    }
  }
}
