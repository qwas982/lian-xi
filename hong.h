/*
宏替换关键字

转载出处:

C语言中32个关键字详解
https://zhuanlan.zhihu.com/p/37908790

*由ANSI标准定义的C语言关键字共32个:

auto double int struct break else long switch

case enum register typedef char extern return union

const float short unsigned continue for signed void

default goto sizeof volatile do if while static

根据关键字的作用，可以将关键字分为数据类型关键字和流程控制关键字两大类。



一、数据类型关键字
A基本数据类型（5个）

void：声明函数无返回值或无参数，声明无类型指针，显式丢弃运算结果
char：字符型类型数据，属于整型数据的一种
int：整型数据，通常为编译器指定的机器字长
float：单精度浮点型数据，属于浮点数据的一种
double：双精度浮点型数据，属于浮点数据的一种
B类型修饰关键字（4个）

short：修饰int，短整型数据，可省略被修饰的int。
long：修饰int，长整形数据，可省略被修饰的int。
signed：修饰整型数据，有符号数据类型
unsigned：修饰整型数据，无符号数据类型
C复杂类型关键字（5个）

struct：结构体声明
union：共用体声明
enum：枚举声明
typedef：声明类型别名
sizeof：得到特定类型或特定类型变量的大小
D存储级别关键字（6个）

auto：指定为自动变量，由编译器自动分配及释放。通常在栈上分配
static：指定为静态变量，分配在静态变量区，修饰函数时，指定函数作用域为文件内部
register：指定为寄存器变量，建议编译器将变量存储到寄存器中使用，也可以修饰函数形参，建议编译器通过寄存器而不是堆栈传递参数
extern：指定对应变量为外部变量，即在另外的目标文件中定义，可以认为是约定由另外文件声明的对象的一个“引用“
const：与volatile合称“cv特性”，指定变量不可被当前线程/进程改变（但有可能被系统或其他线程/进程改变）
volatile：与const合称“cv特性”，指定变量的值有可能会被系统或其他进程/线程改变，强制编译器每次从内存中取得该变量的值
二、流程控制关键字
A跳转结构（4个）

return：用在函数体中，返回特定值（或者是void值，即不返回值）
continue：结束当前循环，开始下一轮循环
break：跳出当前循环或switch结构
goto：无条件跳转语句
B分支结构（5个）

if：条件语句
else：条件语句否定分支（与if连用）
switch：开关语句（多重分支语句）
case：开关语句中的分支标记
default：开关语句中的“其他”分治，可选。
C循环结构（3个）

for：for循环结构，for(1;2;3)4;的执行顺序为1->2->4->3->2...循环，其中2为循环条件
do：do循环结构，do 1 while(2);的执行顺序是1->2->1...循环，2为循环条件
while：while循环结构，while(1) 2;的执行顺序是1->2->1...循环，1为循环条件
以上循环语句，当循环条件表达式为真则继续循环，为假则跳出循环。
*
*/

/*
一、数据类型关键字
A基本数据类型（5个）
*/

//void：声明函数无返回值或无参数，声明无类型指针，显式丢弃运算结果
#define 空 void

//char：字符型类型数据，属于整型数据的一种
#define 印刻 char

//int：整型数据，通常为编译器指定的机器字长
#define 整型 int

//float：单精度浮点型数据，属于浮点数据的一种
#define 浮点 float

//double：双精度浮点型数据，属于浮点数据的一种
#define 双浮 double

/*
B类型修饰关键字（4个）
*/

//short：修饰int，短整型数据，可省略被修饰的int。
#define 短 short

//long：修饰int，长整形数据，可省略被修饰的int。
#define 长 long

//signed：修饰整型数据，有符号数据类型
#define 符号 signed

//unsigned：修饰整型数据，无符号数据类型
#define 无符 unsigned

/*
C复杂类型关键字（5个）
*/

//struct：结构体声明
#define 结构 struct

//union：共用体声明
#define 联合 union

//enum：枚举声明
#define 枚举 enum

//typedef：声明类型别名
#define 类型定义 typedef

//sizeof：得到特定类型或特定类型变量的大小
#define 求大小 sizeof

/*
D存储级别关键字（6个）
*/

//auto：指定为自动变量，由编译器自动分配及释放。通常在栈上分配
#define 自动 auto

//static：指定为静态变量，分配在静态变量区，
//修饰函数时，指定函数作用域为文件内部
#define 静态 static

//register：指定为寄存器变量，建议编译器将变量存储到寄存器中使用，
//也可以修饰函数形参，建议编译器通过寄存器而不是堆栈传递参数
#define 寄存器 register

//extern：指定对应变量为外部变量，即在另外的目标文件中定义，
//可以认为是约定由另外文件声明的对象的一个“引用“
#define 外部 extern

//const：与volatile合称“cv特性”，指定变量不可被当前线程/进程改变
//（但有可能被系统或其他线程/进程改变）
#define 常量 const

//volatile：与const合称“cv特性”，指定变量的值有可能会被系统或其他进程/线程改变，
//强制编译器每次从内存中取得该变量的值
#define 易失 volatile

/*
二、流程控制关键字
A跳转结构（4个）
*/

//return：用在函数体中，返回特定值（或者是void值，即不返回值）
#define 返回 return

//continue：结束当前循环，开始下一轮循环
#define 继续 continue

//break：跳出当前循环或switch结构
#define 断 break

//goto：无条件跳转语句
#define 去到 goto

/*
B分支结构（5个）
*/

//if：条件语句
#define 若 if

//else：条件语句否定分支（与if连用）
#define 否则 else

//switch：开关语句（多重分支语句）
#define 切换 switch

//case：开关语句中的分支标记
#define 情况 case

//default：开关语句中的“其他”分治，可选。
#define 默认 default

/*
C循环结构（3个）
*/


//for：for循环结构，for(1;2;3)4;的执行顺序为1->2->4->3->2...循环，
//其中2为循环条件
#define 环 for

//do：do循环结构，do 1 while(2);的执行顺序是1->2->1...循环，2为循环条件
#define 做 do

//while：while循环结构，while(1) 2;的执行顺序是1->2->1...循环，1为循环条件
#define 一会儿 while

