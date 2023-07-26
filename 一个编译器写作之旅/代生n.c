#include "定义.h" // 引入自定义头文件 "定义.h"
#include "数据.h" // 引入自定义头文件 "数据.h"
#include "声明.h" // 引入自定义头文件 "声明.h"

// x86-64代码生成器

// 可用寄存器列表及其名称
static int 释放寄存器[4];
static char *寄存器列表[4] = { "r8", "r9", "r10", "r11" };

// 设置所有寄存器为可用
void 释放全部_寄存器(void)
{
  // 将释放寄存器数组的所有元素都设置为1，表示所有寄存器都是可用的
  释放寄存器[0] = 释放寄存器[1] = 释放寄存器[2] = 释放寄存器[3] = 1;
}

// 分配一个可用的寄存器。返回寄存器的编号。
// 如果没有可用寄存器，则终止程序。
static int 分配_寄存器(void)
{
  // 遍历释放寄存器数组中的所有元素，查找第一个可用的寄存器
  for (int i = 0; i < 4; i++) {
    if (释放寄存器[i]) {
      // 如果找到可用寄存器，将其对应的释放寄存器元素设置为0，表示该寄存器已被占用，并返回寄存器的编号
      释放寄存器[i] = 0;
      return (i);
    }
  }
  // 如果没有可用寄存器，使用fprintf()函数向标准错误输出流(stderr)打印一条错误消息"Out of registers!"，并使用exit()函数终止程序
  fprintf(stderr, "寄存器不足!\n");
  exit(1);
}

// 将一个寄存器返回到可用寄存器列表中。
// 检查寄存器是否已经在列表中。
static void 释放_寄存器(int 寄存)
{
  // 如果该寄存器已经是可用的，则说明该寄存器已经被释放过一次，打印一条错误消息并终止程序。
  if (释放寄存器[寄存] != 0) {
    fprintf(stderr, "尝试释放寄存器时出错 %d\n", 寄存);
    exit(1);
  }
  // 将该寄存器的可用状态设置为1，表示该寄存器现在是可用的。
  释放寄存器[寄存] = 1;
}

// 打印汇编前导部分
void 代生预漫步()
{
  // 将所有寄存器设置为可用状态（即所有寄存器都没有被占用）
  释放全部_寄存器();
  // 输出汇编代码到输出文件中
  fputs("\tglobal\tmain\n"
	"\textern\tprintf\n"
	"\tsection\t.text\n"
	"LC0:\tdb\t\"%d\",10,0\n"
	"printint:\n"
	"\tpush\trbp\n"
	"\tmov\trbp, rsp\n"
	"\tsub\trsp, 16\n"
	"\tmov\t[rbp-4], edi\n"
	"\tmov\teax, [rbp-4]\n"
	"\tmov\tesi, eax\n"
	"\tlea	rdi, [rel LC0]\n"
	"\tmov	eax, 0\n"
	"\tcall	printf\n"
	"\tnop\n"
	"\tleave\n"
	"\tret\n"
	"\n"
	"main:\n"
	"\tpush\trbp\n"
	"\tmov	rbp, rsp\n",
    出文件);
}

// 打印汇编后导部分
void 代生后漫步()
{
  // 输出汇编代码到输出文件中
  fputs("\tmov	eax, 0\n"
	"\tpop	rbp\n"
	"\tret\n",
    出文件);
}

// 将整型字面量加载到寄存器中。
// 返回寄存器的编号。
int 代生载入(int 值) {

  // 获取一个可用寄存器。
  int 寄 = 分配_寄存器();

  // 打印汇编代码用于初始化该寄存器。
  fprintf(出文件, "\tmov\t%s, %d\n", 寄存器列表[寄], 值);
  return (寄);
}

// 将两个寄存器相加，并返回结果所在的寄存器的编号。
int 代生加(int 寄1, int 寄2) {
  // 打印汇编代码用于将r2的值加到r1中，并将结果存储在r2中。
  fprintf(出文件, "\tadd\t%s, %s\n", 寄存器列表[寄2], 寄存器列表[寄1]);
  // 将r1寄存器返回到可用寄存器列表中。
  释放_寄存器(寄1);
  return (寄2);
}

// 将第二个寄存器的值从第一个寄存器的值中减去，并返回结果所在的寄存器的编号。
int 代生减(int 寄1, int 寄2) {
  // 打印汇编代码用于将r2的值从r1中减去，并将结果存储在r1中。
  fprintf(出文件, "\tsub\t%s, %s\n", 寄存器列表[寄1], 寄存器列表[寄2]);
  // 将r2寄存器返回到可用寄存器列表中。
  释放_寄存器(寄2);
  return (寄1);
}

// 将两个寄存器相乘，并返回结果所在的寄存器的编号。
int 代生乘(int 寄1, int 寄2) {
  // 打印汇编代码用于将r1和r2相乘，并将结果存储在r2中。
  fprintf(出文件, "\timul\t%s, %s\n", 寄存器列表[寄2], 寄存器列表[寄1]);
  // 将r1寄存器返回到可用寄存器列表中。
  释放_寄存器(寄1);
  return (寄2);
}

// 将第一个寄存器的值除以第二个寄存器的值，返回结果所在的寄存器的编号。
int 代生除(int 寄1, int 寄2) {
  // 将r1寄存器中的值移动到rax寄存器中。
  fprintf(出文件, "\tmov\trax, %s\n", 寄存器列表[寄1]);
  // 将rax寄存器中的值符号扩展到rdx寄存器中。
  fprintf(出文件, "\tcqo\n");
  // 将rdx:rax寄存器对中的值除以r2寄存器中的值，商存储在rax寄存器中，余数存储在rdx寄存器中。
  fprintf(出文件, "\tidiv\t%s\n", 寄存器列表[寄2]);
  // 将rax寄存器中的值移动到r1寄存器中，表示除法的结果。
  fprintf(出文件, "\tmov\t%s, rax\n", 寄存器列表[寄1]);
  // 将r2寄存器返回到可用寄存器列表中。
  释放_寄存器(寄2);
  return (寄1);
}

// 使用给定的寄存器调用printint()函数。
void 代生印整型(int 寄) {
  // 将寄存器中的值移动到rdi寄存器中，作为printint()函数的参数。
  fprintf(出文件, "\tmov\trdi, %s\n", 寄存器列表[寄]);
  // 调用printint()函数。
  fprintf(出文件, "\tcall\tprintint\n");
  // 将寄存器返回到可用寄存器列表中。
  释放_寄存器(寄);
}