/*
** $Id: lopcodes.h $
** Opcodes for Lua virtual machine
** See Copyright Notice in lua.h
*/

#ifndef lopcodes_h
#define lopcodes_h

#include "llimits.h"


/*===========================================================================
  We assume that instructions are unsigned 32-bit integers.
  All instructions have an opcode in the 首先_变量 7 bits.
  Instructions can have the following formats:

        3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0
        1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
iABC          C(8)     |      B(8)     |k|     A(8)      |   Op(7)     |
iABx                Bx(17)               |     A(8)      |   Op(7)     |
iAsBx              sBx (signed)(17)      |     A(8)      |   Op(7)     |
iAx                           Ax(25)                     |   Op(7)     |
isJ                           sJ (signed)(25)            |   Op(7)     |

  A signed argument is represented in excess K: the represented 值_圆 is
  the written unsigned 值_圆 minus K, 哪儿_变量 K is half the maximum for the
  corresponding unsigned argument.
===========================================================================*/


enum OpMode {iABC, iABx, iAsBx, iAx, isJ};  /* basic instruction formats */


/*
** 大小_变量 and position of opcode arguments.
*/
#define 操作码_大小_C_宏名		8
#define 操作码_大小_B_宏名		8
#define 操作码_大小_Bx_宏名		(操作码_大小_C_宏名 + 操作码_大小_B_宏名 + 1)
#define 操作码_大小_A_宏名		8
#define 操作码_大小_Ax_宏名		(操作码_大小_Bx_宏名 + 操作码_大小_A_宏名)
#define 操作码_大小_sJ_宏名		(操作码_大小_Bx_宏名 + 操作码_大小_A_宏名)

#define 操作码_大小_OP_宏名		7

#define 操作码_位置_OP_宏名		0

#define 操作码_位置_A_宏名		(操作码_位置_OP_宏名 + 操作码_大小_OP_宏名)
#define 操作码_位置_k_宏名		(操作码_位置_A_宏名 + 操作码_大小_A_宏名)
#define 操作码_位置_B_宏名		(操作码_位置_k_宏名 + 1)
#define 操作码_位置_C_宏名		(操作码_位置_B_宏名 + 操作码_大小_B_宏名)

#define 操作码_位置_Bx_宏名		操作码_位置_k_宏名

#define 操作码_位置_Ax_宏名		操作码_位置_A_宏名

#define 操作码_位置_sJ_宏名		操作码_位置_A_宏名


/*
** 限制们_变量 for opcode arguments.
** we use (signed) 'int' 到_变量 manipulate most arguments,
** so they must fit in ints.
*/

/* Check whether type 'int' has at least 'b' bits ('b' < 32) */
#define 操作码_L_整型有多少位_宏名(b)		((UINT_MAX >> ((b) - 1)) >= 1)


#if 操作码_L_整型有多少位_宏名(操作码_大小_Bx_宏名)
#define 操作码_最大实参_Bx_宏名	((1<<操作码_大小_Bx_宏名)-1)
#else
#define 操作码_最大实参_Bx_宏名	限制_最大_整型_宏名
#endif

#define 操作码_偏移_sBx_宏名	(操作码_最大实参_Bx_宏名>>1)         /* 'sBx' is signed */


#if 操作码_L_整型有多少位_宏名(操作码_大小_Ax_宏名)
#define 操作码_最大实参_Ax_宏名	((1<<操作码_大小_Ax_宏名)-1)
#else
#define 操作码_最大实参_Ax_宏名	限制_最大_整型_宏名
#endif

#if 操作码_L_整型有多少位_宏名(操作码_大小_sJ_宏名)
#define 操作码_最大实参_sJ_宏名	((1 << 操作码_大小_sJ_宏名) - 1)
#else
#define 操作码_最大实参_sJ_宏名	限制_最大_整型_宏名
#endif

#define 操作码_偏移_sJ_宏名	(操作码_最大实参_sJ_宏名 >> 1)


#define 操作码_最大实参_A_宏名	((1<<操作码_大小_A_宏名)-1)
#define 操作码_最大实参_B_宏名	((1<<操作码_大小_B_宏名)-1)
#define 操作码_最大实参_C_宏名	((1<<操作码_大小_C_宏名)-1)
#define 操作码_偏移_sC_宏名	(操作码_最大实参_C_宏名 >> 1)

#define 操作码_整型到sC_宏名(i)	((i) + 操作码_偏移_sC_宏名)
#define 操作码_sC到整型_宏名(i)	((i) - 操作码_偏移_sC_宏名)


/* creates a 掩码_变量 with 'n' 1 bits at position 'p' */
#define 操作码_掩码1_宏名(n,p)	((~((~(Instruction)0)<<(n)))<<(p))

/* creates a 掩码_变量 with 'n' 0 bits at position 'p' */
#define 操作码_掩码0_宏名(n,p)	(~操作码_掩码1_宏名(n,p))

/*
** the following macros help 到_变量 manipulate instructions
*/

#define 操作码_获取_操作码_宏名(i)	(限制_类型转换_宏名(操作码_枚举, ((i)>>操作码_位置_OP_宏名) & 操作码_掩码1_宏名(操作码_大小_OP_宏名,0)))
#define 操作码_设置_操作码_宏名(i,o)	((i) = (((i)&操作码_掩码0_宏名(操作码_大小_OP_宏名,操作码_位置_OP_宏名)) | \
		((限制_类型转换_宏名(Instruction, o)<<操作码_位置_OP_宏名)&操作码_掩码1_宏名(操作码_大小_OP_宏名,操作码_位置_OP_宏名))))

#define 操作码_检查操作m_宏名(i,m)	(操作码_获取操作模式_宏名(操作码_获取_操作码_宏名(i)) == m)


#define 操作码_获取实参_宏名(i,位置_缩变量,大小_变量)	(限制_类型转换_整型_宏名(((i)>>(位置_缩变量)) & 操作码_掩码1_宏名(大小_变量,0)))
#define 操作码_设置实参_宏名(i,v,位置_缩变量,大小_变量)	((i) = (((i)&操作码_掩码0_宏名(大小_变量,位置_缩变量)) | \
                ((限制_类型转换_宏名(Instruction, v)<<位置_缩变量)&操作码_掩码1_宏名(大小_变量,位置_缩变量))))

#define 操作码_获取实参_A_宏名(i)	操作码_获取实参_宏名(i, 操作码_位置_A_宏名, 操作码_大小_A_宏名)
#define 操作码_设置实参_A_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_A_宏名, 操作码_大小_A_宏名)

#define 操作码_获取实参_B_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iABC), 操作码_获取实参_宏名(i, 操作码_位置_B_宏名, 操作码_大小_B_宏名))
#define 操作码_获取实参_sB_宏名(i)	操作码_sC到整型_宏名(操作码_获取实参_B_宏名(i))
#define 操作码_设置实参_B_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_B_宏名, 操作码_大小_B_宏名)

#define 操作码_获取实参_C_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iABC), 操作码_获取实参_宏名(i, 操作码_位置_C_宏名, 操作码_大小_C_宏名))
#define 操作码_获取实参_sC_宏名(i)	操作码_sC到整型_宏名(操作码_获取实参_C_宏名(i))
#define 操作码_设置实参_C_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_C_宏名, 操作码_大小_C_宏名)

#define 操作码_测试实参_k_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iABC), (限制_类型转换_整型_宏名(((i) & (1u << 操作码_位置_k_宏名)))))
#define 操作码_获取实参_k_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iABC), 操作码_获取实参_宏名(i, 操作码_位置_k_宏名, 1))
#define 操作码_设置实参_k_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_k_宏名, 1)

#define 操作码_获取实参_Bx_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iABx), 操作码_获取实参_宏名(i, 操作码_位置_Bx_宏名, 操作码_大小_Bx_宏名))
#define 操作码_设置实参_Bx_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_Bx_宏名, 操作码_大小_Bx_宏名)

#define 操作码_获取实参_Ax_宏名(i)	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iAx), 操作码_获取实参_宏名(i, 操作码_位置_Ax_宏名, 操作码_大小_Ax_宏名))
#define 操作码_设置实参_Ax_宏名(i,v)	操作码_设置实参_宏名(i, v, 操作码_位置_Ax_宏名, 操作码_大小_Ax_宏名)

#define 操作码_获取实参_sBx_宏名(i)  \
	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, iAsBx), 操作码_获取实参_宏名(i, 操作码_位置_Bx_宏名, 操作码_大小_Bx_宏名) - 操作码_偏移_sBx_宏名)
#define 操作码_设置实参_sBx_宏名(i,b)	操作码_设置实参_Bx_宏名((i),限制_类型转换_无符整型_宏名((b)+操作码_偏移_sBx_宏名))

#define 操作码_获取实参_sJ_宏名(i)  \
	限制_检查_表达式_宏名(操作码_检查操作m_宏名(i, isJ), 操作码_获取实参_宏名(i, 操作码_位置_sJ_宏名, 操作码_大小_sJ_宏名) - 操作码_偏移_sJ_宏名)
#define 操作码_设置实参_sJ_宏名(i,j) \
	操作码_设置实参_宏名(i, 限制_类型转换_无符整型_宏名((j)+操作码_偏移_sJ_宏名), 操作码_位置_sJ_宏名, 操作码_大小_sJ_宏名)


#define 操作码_创建_ABCk_宏名(o,a,b,c,k)	((限制_类型转换_宏名(Instruction, o)<<操作码_位置_OP_宏名) \
			| (限制_类型转换_宏名(Instruction, a)<<操作码_位置_A_宏名) \
			| (限制_类型转换_宏名(Instruction, b)<<操作码_位置_B_宏名) \
			| (限制_类型转换_宏名(Instruction, c)<<操作码_位置_C_宏名) \
			| (限制_类型转换_宏名(Instruction, k)<<操作码_位置_k_宏名))

#define 操作码_创建_ABx_宏名(o,a,bc)	((限制_类型转换_宏名(Instruction, o)<<操作码_位置_OP_宏名) \
			| (限制_类型转换_宏名(Instruction, a)<<操作码_位置_A_宏名) \
			| (限制_类型转换_宏名(Instruction, bc)<<操作码_位置_Bx_宏名))

#define 操作码_创建_Ax_宏名(o,a)		((限制_类型转换_宏名(Instruction, o)<<操作码_位置_OP_宏名) \
			| (限制_类型转换_宏名(Instruction, a)<<操作码_位置_Ax_宏名))

#define 操作码_创建_sJ_宏名(o,j,k)	((限制_类型转换_宏名(Instruction, o) << 操作码_位置_OP_宏名) \
			| (限制_类型转换_宏名(Instruction, j) << 操作码_位置_sJ_宏名) \
			| (限制_类型转换_宏名(Instruction, k) << 操作码_位置_k_宏名))


#if !defined(操作码_最大索引RK_宏名)  /* (for debugging only) */
#define 操作码_最大索引RK_宏名	操作码_最大实参_B_宏名
#endif


/*
** invalid register that fits in 8 bits
*/
#define 操作码_无_寄存器_宏名		操作码_最大实参_A_宏名


/*
** R[x] - register
** K[x] - constant (in constant table)
** RK(x) == if k(i) then K[x] else R[x]
*/


/*
** Grep "ORDER OP" if you 改变_变量 these enums. Opcodes 已记号_变量 with a (*)
** has 额外_变量 descriptions in the notes after the enumeration.
*/

typedef enum {
/*----------------------------------------------------------------------
  名称_变量		实参们_短变量	description
------------------------------------------------------------------------*/
操作_移,/*	A B	R[A] := R[B]					*/
操作_载入整,/*	A sBx	R[A] := sBx					*/
操作_载入浮,/*	A sBx	R[A] := (炉_数目型)sBx				*/
操作_载入常,/*	A Bx	R[A] := K[Bx]					*/
操作_载入常额外,/*	A	R[A] := K[额外_变量 实参_短变量]				*/
操作_载入假,/*	A	R[A] := false					*/
操作_载入假跳过,/*A	R[A] := false; 程序计数_变量++	(*)			*/
操作_载入真,/*	A	R[A] := true					*/
操作_载入空值,/*	A B	R[A], R[A+1], ..., R[A+B] := nil		*/
操作_获取上值,/*	A B	R[A] := UpValue[B]				*/
操作_设置上值,/*	A B	UpValue[B] := R[A]				*/

操作_获取表上值内,/*	A B C	R[A] := UpValue[B][K[C]:string]			*/
操作_获取表,/*	A B C	R[A] := R[B][R[C]]				*/
操作_获取整,/*	A B C	R[A] := R[B][C]					*/
操作_获取字段,/*	A B C	R[A] := R[B][K[C]:string]			*/

操作_设置表上值内,/*	A B C	UpValue[A][K[B]:string] := RK(C)		*/
操作_设置表,/*	A B C	R[A][R[B]] := RK(C)				*/
操作_设置整,/*	A B C	R[A][B] := RK(C)				*/
操作_设置字段,/*	A B C	R[A][K[B]:string] := RK(C)			*/

操作_新表,/*	A B C k	R[A] := {}					*/

操作_自身,/*	A B C	R[A+1] := R[B]; R[A] := R[B][RK(C):string]	*/

操作_加整,/*	A B sC	R[A] := R[B] + sC				*/

操作_加常,/*	A B C	R[A] := R[B] + K[C]:number			*/
操作_减常,/*	A B C	R[A] := R[B] - K[C]:number			*/
操作_乘常,/*	A B C	R[A] := R[B] * K[C]:number			*/
操作_取模常,/*	A B C	R[A] := R[B] % K[C]:number			*/
操作_幂运算常,/*	A B C	R[A] := R[B] ^ K[C]:number			*/
操作_除常,/*	A B C	R[A] := R[B] / K[C]:number			*/
操作_整数除常,/*	A B C	R[A] := R[B] // K[C]:number			*/

操作_位与常,/*	A B C	R[A] := R[B] & K[C]:integer			*/
操作_位或常,/*	A B C	R[A] := R[B] | K[C]:integer			*/
操作_位异或常,/*	A B C	R[A] := R[B] ~ K[C]:integer			*/

操作_右移整,/*	A B sC	R[A] := R[B] >> sC				*/
操作_左移整,/*	A B sC	R[A] := sC << R[B]				*/

操作_加,/*	A B C	R[A] := R[B] + R[C]				*/
操作_减,/*	A B C	R[A] := R[B] - R[C]				*/
操作_乘,/*	A B C	R[A] := R[B] * R[C]				*/
操作_取模,/*	A B C	R[A] := R[B] % R[C]				*/
操作_幂运算,/*	A B C	R[A] := R[B] ^ R[C]				*/
操作_除,/*	A B C	R[A] := R[B] / R[C]				*/
操作_整数除,/*	A B C	R[A] := R[B] // R[C]				*/

操作_位与,/*	A B C	R[A] := R[B] & R[C]				*/
操作_位或,/*	A B C	R[A] := R[B] | R[C]				*/
操作_位异或,/*	A B C	R[A] := R[B] ~ R[C]				*/
操作_左移,/*	A B C	R[A] := R[B] << R[C]				*/
操作_右移,/*	A B C	R[A] := R[B] >> R[C]				*/

操作_元方法二元,/*	A B C	call C metamethod over R[A] and R[B]	(*)	*/
操作_元方法二元整,/*	A sB C k	call C metamethod over R[A] and sB	*/
操作_元方法二元常,/*	A B C k		call C metamethod over R[A] and K[B]	*/

操作_一元减号,/*	A B	R[A] := -R[B]					*/
操作_位非,/*	A B	R[A] := ~R[B]					*/
操作_非,/*	A B	R[A] := not R[B]				*/
操作_长度,/*	A B	R[A] := #R[B] (length operator)			*/

操作_拼接,/*	A B	R[A] := R[A].. ... ..R[A + B - 1]		*/

操作_关闭,/*	A	关闭_圆 all 上值们_小写 >= R[A]			*/
操作_待关闭,/*	A	记号_变量 variable A "到_变量 be closed"			*/
操作_跳转,/*	sJ	程序计数_变量 += sJ					*/
操作_相等,/*	A B k	if ((R[A] == R[B]) ~= k) then 程序计数_变量++		*/
操作_小于,/*	A B k	if ((R[A] <  R[B]) ~= k) then 程序计数_变量++		*/
操作_小等,/*	A B k	if ((R[A] <= R[B]) ~= k) then 程序计数_变量++		*/

操作_相等常,/*	A B k	if ((R[A] == K[B]) ~= k) then 程序计数_变量++		*/
操作_相等整,/*	A sB k	if ((R[A] == sB) ~= k) then 程序计数_变量++		*/
操作_小于整,/*	A sB k	if ((R[A] < sB) ~= k) then 程序计数_变量++			*/
操作_小等整,/*	A sB k	if ((R[A] <= sB) ~= k) then 程序计数_变量++		*/
操作_大于整,/*	A sB k	if ((R[A] > sB) ~= k) then 程序计数_变量++			*/
操作_大等整,/*	A sB k	if ((R[A] >= sB) ~= k) then 程序计数_变量++		*/

操作_测试,/*	A k	if (not R[A] == k) then 程序计数_变量++			*/
操作_测试设置,/*	A B k	if (not R[B] == k) then 程序计数_变量++ else R[A] := R[B] (*) */

操作_调用,/*	A B C	R[A], ... ,R[A+C-2] := R[A](R[A+1], ... ,R[A+B-1]) */
操作_尾调用,/*	A B C k	return R[A](R[A+1], ... ,R[A+B-1])		*/

操作_返回,/*	A B C k	return R[A], ... ,R[A+B-2]	(see note)	*/
操作_返回0,/*		return						*/
操作_返回1,/*	A	return R[A]					*/

操作_为环,/*	A Bx	update counters; if 环_变量 continues then 程序计数_变量-=Bx; */
操作_为预备,/*	A Bx	<月解析器_检查_函 values and prepare counters>;
                        if not 到_变量 run then 程序计数_变量+=Bx+1;			*/

操作_泛型为预备,/*	A Bx	create 上值_圆 for R[A + 3]; 程序计数_变量+=Bx		*/
操作_泛型为调用,/*	A C	R[A+4], ... ,R[A+3+C] := R[A](R[A+1], R[A+2]);	*/
操作_泛型为环,/*	A Bx	if R[A+2] ~= nil then { R[A]=R[A+2]; 程序计数_变量 -= Bx }	*/

操作_设置列表,/*	A B C k	R[A][C+i] := R[A+i], 1 <= i <= B		*/

操作_闭包,/*	A Bx	R[A] := closure(KPROTO[Bx])			*/

操作_变量实参,/*	A C	R[A], R[A+1], ..., R[A+C-2] = vararg		*/

操作_变量实参预备,/*A	(adjust vararg parameters)			*/

操作_额外实参/*	Ax	额外_变量 (larger) argument for 前一个_变量 opcode	*/
} 操作码_枚举;


#define 操作码_数目_操作码_宏名	((int)(操作_额外实参) + 1)



/*===========================================================================
  Notes:

  (*) Opcode 操作_载入假跳过 is used 到_变量 convert a condition 到_变量 a boolean
  值_圆, in a 代码_变量 equivalent 到_变量 (not 条件_变量 ? false : true).  (It
  produces false and skips the 下一个_变量 instruction producing true.)

  (*) Opcodes 操作_元方法二元 and variants follow each arithmetic and
  bitwise opcode. If the operation succeeds, it skips this 下一个_变量
  opcode. Otherwise, this opcode calls the corresponding metamethod.

  (*) Opcode 操作_测试设置 is used in short-circuit expressions that need
  both 到_变量 jump and 到_变量 produce a 值_圆, such as (a = b or c).

  (*) In 操作_调用, if (B == 0) then B = 顶部_变量 - A. If (C == 0), then
  '顶部_变量' is set 到_变量 last_result+1, so 下一个_变量 打开_圆 instruction (操作_调用,
  操作_返回*, 操作_设置列表) may use '顶部_变量'.

  (*) In 操作_变量实参, if (C == 0) then use 实际上_变量 number of varargs and
  set 顶部_变量 (like in 操作_调用 with C == 0).

  (*) In 操作_返回, if (B == 0) then return 上_小变量 到_变量 '顶部_变量'.

  (*) In 操作_载入常额外 and 操作_新表, the 下一个_变量 instruction is always
  操作_额外实参.

  (*) In 操作_设置列表, if (B == 0) then real B = '顶部_变量'; if k, then
  real C = 编译器_额外实参_宏名 _ C (the bits of 编译器_额外实参_宏名 concatenated with the
  bits of C).

  (*) In 操作_新表, B is log2 of the 哈希_小写 大小_变量 (which is always a
  power of 2) plus 1, or zero for 大小_变量 zero. If not k, the 数组_圆 大小_变量
  is C. Otherwise, the 数组_圆 大小_变量 is 编译器_额外实参_宏名 _ C.

  (*) For comparisons, k specifies 什么_变量 condition the test should accept
  (true or false).

  (*) In 操作_元方法二元整/操作_元方法二元常, k means the arguments were flipped
   (the constant is the 首先_变量 operand).

  (*) All 'skips' (程序计数_变量++) assume that 下一个_变量 instruction is a jump.

  (*) In instructions 操作_返回/操作_尾调用, 'k' specifies that the
  function builds 上值们_小写, which may need 到_变量 be closed. C > 0 means
  the function is vararg, so that its '函_短变量' must be corrected before
  returning; in this case, (C - 1) is its number of fixed parameters.

  (*) In comparisons with an immediate operand, C signals whether the
  original operand was a float. (It must be corrected in case of
  metamethods.)

===========================================================================*/


/*
** masks for instruction properties. The 格式_变量 is:
** bits 0-2: 操作_短变量 模块_变量
** bit 3: instruction set register A
** bit 4: operator is a test (下一个_变量 instruction must be a jump)
** bit 5: instruction uses 'L->顶部_变量' set by 前一个_变量 instruction (when B == 0)
** bit 6: instruction sets 'L->顶部_变量' for 下一个_变量 instruction (when C == 0)
** bit 7: instruction is an MM instruction (call a metamethod)
*/

配置_月亮I_D声明_宏名(const 路_字节型 月亮P_操作模式_变量[操作码_数目_操作码_宏名];)

#define 操作码_获取操作模式_宏名(m)	(限制_类型转换_宏名(enum OpMode, 月亮P_操作模式_变量[m] & 7))
#define 操作码_测试A模式_宏名(m)	(月亮P_操作模式_变量[m] & (1 << 3))
#define 操作码_测试T模式_宏名(m)	(月亮P_操作模式_变量[m] & (1 << 4))
#define 操作码_测试IT模式_宏名(m)	(月亮P_操作模式_变量[m] & (1 << 5))
#define 操作码_测试OT模式_宏名(m)	(月亮P_操作模式_变量[m] & (1 << 6))
#define 操作码_测试MM模式_宏名(m)	(月亮P_操作模式_变量[m] & (1 << 7))

/* "out 顶部_变量" (set 顶部_变量 for 下一个_变量 instruction) */
#define 操作码_是否OT_宏名(i)  \
	((操作码_测试OT模式_宏名(操作码_获取_操作码_宏名(i)) && 操作码_获取实参_C_宏名(i) == 0) || \
          操作码_获取_操作码_宏名(i) == 操作_尾调用)

/* "in 顶部_变量" (uses 顶部_变量 from 前一个_变量 instruction) */
#define 操作码_是否IT_宏名(i)		(操作码_测试IT模式_宏名(操作码_获取_操作码_宏名(i)) && 操作码_获取实参_B_宏名(i) == 0)

#define 操作码_操作模式_宏名(mm,ot,it,t,a,m)  \
    (((mm) << 7) | ((ot) << 6) | ((it) << 5) | ((t) << 4) | ((a) << 3) | (m))


/* number of 列表_变量 items 到_变量 accumulate before a SETLIST instruction */
#define 操作码_字段_每次_刷新_宏名	50

#endif
