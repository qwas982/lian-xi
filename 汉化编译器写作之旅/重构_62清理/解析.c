/*------------------------------------------------
------包括 类型.印名 树.印名 语句.印名 表达式.印名 优化.印名-------
------------------------------------------------*/
#include "定义.h"

/*------------------------------------------------
------类型.印名 -------
------------------------------------------------*/

// Types and 类型 handling
//   (印名)   

// Return true if a 类型 is an int 类型
// of any 大小, false otherwise
int 整型类型(int 类型) {
  return (((类型 & 0xf) == 0) && (类型 >= 原语_印刻 && 类型 <= 原语_长型));
}

// Return true if a 类型 is of pointer 类型
int 指针类型(int 类型) {
  return ((类型 & 0xf) != 0);
}

// Given a primitive 类型, return
// the 类型 which is a pointer to it
int 指针_到(int 类型) {
  if ((类型 & 0xf) == 0xf)
    致命数("Unrecognised in 指针_到: 类型", 类型);
  return (类型 + 1);
}

// Given a primitive pointer 类型, return
// the 类型 which it points to
int 值_位置(int 类型) {
  if ((类型 & 0xf) == 0x0)
    致命数("Unrecognised in 值_位置: 类型", 类型);
  return (类型 - 1);
}

// Given a 类型 and a composite 类型 pointer, return
// the 大小 of this 类型 in bytes
int 类型大小(int 类型, struct 符号表 *复合类型) {
  if (类型 == 原语_结构 || 类型 == 原语_联合)
    return (复合类型->大小);
  return (生成基元大小(类型));
}

// Given an AST 树 and a 类型 which we want it to become,
// possibly modify the 树 by widening or scaling so that
// it is compatible with this 类型. Return the original 树
// if no changes occurred, a modified 树, or NULL if the
// 树 is not compatible with the given 类型.
// If this will be part of a binary operation, the AST 操作 is not zero.
struct 抽象句法树节点 *修改_类型(struct 抽象句法树节点 *树, int 右类型,
			    struct 符号表 *右类别类型, int 操作) {
  int 左类型;
  int 左大小, 右大小;

  左类型 = 树->类型;

  // For 抽象_逻辑或 and 抽象_逻辑与, both types have to be int or pointer types
  if (操作==抽象_逻辑或 || 操作==抽象_逻辑与) {
    if (!整型类型(左类型) && !指针类型(左类型))
      return(NULL);
    if (!整型类型(左类型) && !指针类型(右类型))
      return(NULL);
    return (树);
  }

  // XXX No idea on these yet
  if (左类型 == 原语_结构 || 左类型 == 原语_联合)
    致命("Don't know how to do this yet");
  if (右类型 == 原语_结构 || 右类型 == 原语_联合)
    致命("Don't know how to do this yet");

  // Compare scalar int types
  if (整型类型(左类型) && 整型类型(右类型)) {

    // Both types same, nothing to do
    if (左类型 == 右类型)
      return (树);

    // Get the sizes for each 类型
    左大小 = 类型大小(左类型, NULL);
    右大小 = 类型大小(右类型, NULL);

    // The 树'串名 类型 大小 is too big and we can't narrow
    if (左大小 > 右大小)
      return (NULL);

    // Widen to the 右
    if (右大小 > 左大小)
      return (制作抽象句法树一元(抽象_加宽, 右类型, NULL, 树, NULL, 0));
  }

  // For pointers
  if (指针类型(左类型) && 指针类型(右类型)) {
    // We can compare them
    if (操作 >= 抽象_等于 && 操作 <= 抽象_大等)
      return (树);

    // A comparison of the same 类型 for a non-binary operation is OK,
    // or when the 左 树 is of  `void *` 类型.
    if (操作 == 0 && (左类型 == 右类型 || 左类型 == 指针_到(原语_空的)))
      return (树);
  }

  // We can scale only on add and subtract operations
  if (操作 == 抽象_加法 || 操作 == 抽象_减法 ||
      操作 == 抽象_赋值加 || 操作 == 抽象_赋值减) {

    // Left is int 类型, 右 is pointer 类型 and the 大小
    // of the original 类型 is >1: scale the 左
    if (整型类型(左类型) && 指针类型(右类型)) {
      右大小 = 生成基元大小(值_位置(右类型));
      if (右大小 > 1)
	return (制作抽象句法树一元(抽象_比例尺, 右类型, 右类别类型, 树, NULL, 右大小));
      else
	return (树);		// Size 1, no need to scale
    }
  }

  // If we get here, the types are not compatible
  return (NULL);
}

/*------------------------------------------------
------ 树.印名 -------
------------------------------------------------*/

// AST 树 functions
//   (印名)   

// Build and return a generic AST 节点
struct 抽象句法树节点 *制作抽象句法树节点(int 操作, int 类型,
			  struct 符号表 *复合类型,
			  struct 抽象句法树节点 *左,
			  struct 抽象句法树节点 *中,
			  struct 抽象句法树节点 *右,
			  struct 符号表 *符号, int 整型值) {
  struct 抽象句法树节点 *小节点;

  // Malloc a new 抽象句法树节点
  小节点 = (struct 抽象句法树节点 *) malloc(sizeof(struct 抽象句法树节点));
  if (小节点 == NULL)
    致命("Unable to malloc in 制作抽象句法树节点()");

  // Copy in the field values and return it
  小节点->操作 = 操作;
  小节点->类型 = 类型;
  小节点->复合类型 = 复合类型;
  小节点->左 = 左;
  小节点->中 = 中;
  小节点->右 = 右;
  小节点->符号 = 符号;
  小节点->小抽象_整型值 = 整型值;
  小节点->行号= 0;
  return (小节点);
}


// Make an AST leaf 节点
struct 抽象句法树节点 *制作抽象句法树叶(int 操作, int 类型,
			  struct 符号表 *复合类型,
			  struct 符号表 *符号, int 整型值) {
  return (制作抽象句法树节点(操作, 类型, 复合类型, NULL, NULL, NULL, 符号, 整型值));
}

// Make a unary AST 节点: only one 孩子
struct 抽象句法树节点 *制作抽象句法树一元(int 操作, int 类型,
			   struct 符号表 *复合类型,
			   struct 抽象句法树节点 *左,
			   struct 符号表 *符号, int 整型值) {
  return (制作抽象句法树节点(操作, 类型, 复合类型, 左, NULL, NULL, 符号, 整型值));
}

// Generate and return a new 标号 number
// just for AST dumping purposes
static int 转储身份 = 1;
static int 生成转储标号(void) {
  return (转储身份++);
}

// Given an AST 树, print it out and follow the
// traversal of the 树 that 生成抽象句法树() follows
void 转储抽象句法树(struct 抽象句法树节点 *小节点, int 标号, int 等级) {
  int 标号假, 标号开始, 标号终;
  int 甲;

  switch (小节点->操作) {
    case 抽象_若:
      标号假 = 生成转储标号();
      for (甲 = 0; 甲 < 等级; 甲++)
	fprintf(stdout, " ");
      fprintf(stdout, "抽象_若");
      if (小节点->右) {
	标号终 = 生成转储标号();
	fprintf(stdout, ", end L%d", 标号终);
      }
      fprintf(stdout, "\n");
      转储抽象句法树(小节点->左, 标号假, 等级 + 2);
      转储抽象句法树(小节点->中, 无标号, 等级 + 2);
      if (小节点->右)
	转储抽象句法树(小节点->右, 无标号, 等级 + 2);
      return;
    case 抽象_一会儿:
      标号开始 = 生成转储标号();
      for (甲 = 0; 甲 < 等级; 甲++)
	fprintf(stdout, " ");
      fprintf(stdout, "抽象_一会儿, start L%d\n", 标号开始);
      标号终 = 生成转储标号();
      转储抽象句法树(小节点->左, 标号终, 等级 + 2);
      转储抽象句法树(小节点->右, 无标号, 等级 + 2);
      return;
  }

  // Reset 等级 to -2 for 抽象_胶水
  if (小节点->操作 == 抽象_胶水)
    等级 = -2;

  // General AST 节点 handling
  if (小节点->左)
    转储抽象句法树(小节点->左, 无标号, 等级 + 2);
  if (小节点->右)
    转储抽象句法树(小节点->右, 无标号, 等级 + 2);


  for (甲 = 0; 甲 < 等级; 甲++)
    fprintf(stdout, " ");
  switch (小节点->操作) {
    case 抽象_胶水:
      fprintf(stdout, "\n\n");
      return;
    case 抽象_函数:
      fprintf(stdout, "抽象_函数 %s\n", 小节点->符号->名字);
      return;
    case 抽象_加法:
      fprintf(stdout, "抽象_加法\n");
      return;
    case 抽象_减法:
      fprintf(stdout, "抽象_减法\n");
      return;
    case 抽象_乘法:
      fprintf(stdout, "抽象_乘法\n");
      return;
    case 抽象_除法:
      fprintf(stdout, "抽象_除法\n");
      return;
    case 抽象_等于:
      fprintf(stdout, "抽象_等于\n");
      return;
    case 抽象_不等:
      fprintf(stdout, "抽象_不等\n");
      return;
    case 抽象_小于:
      fprintf(stdout, "抽象_小等\n");
      return;
    case 抽象_大于:
      fprintf(stdout, "抽象_大于\n");
      return;
    case 抽象_小等:
      fprintf(stdout, "抽象_小等\n");
      return;
    case 抽象_大等:
      fprintf(stdout, "抽象_大等\n");
      return;
    case 抽象_整型字面:
      fprintf(stdout, "抽象_整型字面 %d\n", 小节点->小抽象_整型值);
      return;
    case 抽象_串字面:
      fprintf(stdout, "抽象_串字面 rval 标号 L%d\n", 小节点->小抽象_整型值);
      return;
    case 抽象_标识:
      if (小节点->右值)
	fprintf(stdout, "抽象_标识 rval %s\n", 小节点->符号->名字);
      else
	fprintf(stdout, "抽象_标识 %s\n", 小节点->符号->名字);
      return;
    case 抽象_赋值:
      fprintf(stdout, "抽象_赋值\n");
      return;
    case 抽象_加宽:
      fprintf(stdout, "抽象_加宽\n");
      return;
    case 抽象_返回:
      fprintf(stdout, "抽象_返回\n");
      return;
    case 抽象_函数调用:
      fprintf(stdout, "抽象_函数调用 %s\n", 小节点->符号->名字);
      return;
    case 抽象_地址:
      fprintf(stdout, "抽象_地址 %s\n", 小节点->符号->名字);
      return;
    case 抽象_解引用:
      if (小节点->右值)
	fprintf(stdout, "抽象_解引用 rval\n");
      else
	fprintf(stdout, "抽象_解引用\n");
      return;
    case 抽象_比例尺:
      fprintf(stdout, "抽象_比例尺 %d\n", 小节点->小抽象_大小);
      return;
    case 抽象_前置自增:
      fprintf(stdout, "抽象_前置自增 %s\n", 小节点->符号->名字);
      return;
    case 抽象_前置自减:
      fprintf(stdout, "抽象_前置自减 %s\n", 小节点->符号->名字);
      return;
    case 抽象_后置自增:
      fprintf(stdout, "抽象_后置自增\n");
      return;
    case 抽象_后置自减:
      fprintf(stdout, "抽象_后置自减\n");
      return;
    case 抽象_否定:
      fprintf(stdout, "抽象_否定\n");
      return;
    case 抽象_断:
      fprintf(stdout, "抽象_断\n");
      return;
    case 抽象_继续:
      fprintf(stdout, "抽象_继续\n");
      return;
    case 抽象_情况:
      fprintf(stdout, "抽象_情况 %d\n", 小节点->小抽象_整型值);
      return;
    case 抽象_默认:
      fprintf(stdout, "抽象_默认\n");
      return;
    case 抽象_切换:
      fprintf(stdout, "抽象_切换\n");
      return;
    case 抽象_投射:
      fprintf(stdout, "抽象_投射 %d\n", 小节点->类型);
      return;
    case 抽象_赋值加:
      fprintf(stdout, "抽象_赋值加\n");
      return;
    case 抽象_赋值减:
      fprintf(stdout, "抽象_赋值减\n");
      return;
    case 抽象_赋值星号:
      fprintf(stdout, "抽象_赋值星号\n");
      return;
    case 抽象_赋值斜杠:
      fprintf(stdout, "抽象_赋值斜杠\n");
      return;
    case 抽象_到布尔:
      fprintf(stdout, "抽象_到布尔\n");
      return;
    case 抽象_逻辑或:
      fprintf(stdout, "抽象_逻辑或\n");
      return;
    case 抽象_逻辑与:
      fprintf(stdout, "抽象_逻辑与\n");
      return;
    case 抽象_与:
      fprintf(stdout, "抽象_与\n");
      return;
    case 抽象_赋值余数:
      fprintf(stdout, "抽象_赋值余数\n");
      return;
    case 抽象_颠倒:
      fprintf(stdout, "抽象_颠倒\n");
      return;
    case 抽象_逻辑非:
      fprintf(stdout, "抽象_逻辑非\n");
      return;
    case 抽象_左移:
      fprintf(stdout, "抽象_左移\n");
      return;
    case 抽象_余数:
      fprintf(stdout, "抽象_余数\n");
      return;
    case 抽象_或:
      fprintf(stdout, "抽象_或\n");
      return;
    case 抽象_右移:
      fprintf(stdout, "抽象_右移\n");
      return;
    case 抽象_三元运算符:
      fprintf(stdout, "抽象_三元运算符\n");
      return;
    case 抽象_异或:
      fprintf(stdout, "抽象_异或\n");
      return;
    default:
      致命数("Unknown 转储抽象句法树 operator", 小节点->操作);
  }
}

/*------------------------------------------------
------ 语句.印名 -------
------------------------------------------------*/

// Parsing of 语句_复
//   (印名)   

// Prototypes
static struct 抽象句法树节点 *单个_语句(void);

// 复合_语句:          // 空的, 例如. no 语句
//      |      语句
//      |      语句 语句_复
//      ;
//
// 语句: 声明
//      |     表达式_语句
//      |     函数_调用
//      |     若_语句
//      |     一会儿_语句
//      |     为_语句
//      |     返回_语句
//      ;


// 若_语句: 若_头部
//      |        若_头部 '否则' 语句
//      ;
//
// 若_头部: '若' '(' true_false_expression ')' 语句  ;
//
// Parse an IF 语句 including any
// optional ELSE clause and return its AST
static struct 抽象句法树节点 *若_语句(void) {
  struct 抽象句法树节点 *抽象句法树条件, *抽象句法树真, *抽象句法树假 = NULL;

  // Ensure we have 'if' '('
  匹配(牌名_若, "若");
  左小括号();

  // Parse the following expression
  // and the ')' following. Force a
  // non-comparison to be boolean
  // the 树'串名 operation is a comparison.
  抽象句法树条件 = 二元表达式(0);
  if (抽象句法树条件->操作 < 抽象_等于 || 抽象句法树条件->操作 > 抽象_大等)
    抽象句法树条件 =
      制作抽象句法树一元(抽象_到布尔, 抽象句法树条件->类型, 抽象句法树条件->复合类型, 抽象句法树条件, NULL, 0);
  右小括号();

  // Get the AST for the 语句
  抽象句法树真 = 单个_语句();

  // If we have an 'else', 跳过 it
  // and get the AST for the 语句
  if (牌_.牌 == 牌名_否则) {
    扫描(&牌_);
    抽象句法树假 = 单个_语句();
  }

  // Build and return the AST for this 语句
  return (制作抽象句法树节点(抽象_若, 原语_全无, NULL, 抽象句法树条件, 抽象句法树真, 抽象句法树假, NULL, 0));
}


// 一会儿_语句: 'while' '(' true_false_expression ')' 语句  ;
//
// Parse a WHILE 语句 and return its AST
static struct 抽象句法树节点 *一会儿_语句(void) {
  struct 抽象句法树节点 *抽象句法树条件, *抽象句法树身体;

  // Ensure we have 'while' '('
  匹配(牌名_一会儿, "一会儿");
  左小括号();

  // Parse the following expression
  // and the ')' following. Force a
  // non-comparison to be boolean
  // the 树'串名 operation is a comparison.
  抽象句法树条件 = 二元表达式(0);
  if (抽象句法树条件->操作 < 抽象_等于 || 抽象句法树条件->操作 > 抽象_大等)
    抽象句法树条件 =
      制作抽象句法树一元(抽象_到布尔, 抽象句法树条件->类型, 抽象句法树条件->复合类型, 抽象句法树条件, NULL, 0);
  右小括号();

  // Get the AST for the 语句.
  // Update the loop depth in the process
  环级++;
  抽象句法树身体 = 单个_语句();
  环级--;

  // Build and return the AST for this 语句
  return (制作抽象句法树节点(抽象_一会儿, 原语_全无, NULL, 抽象句法树条件, NULL, 抽象句法树身体, NULL, 0));
}

// 为_语句: 'for' '(' 表达式_列表 ';'
//                          true_false_expression ';'
//                          表达式_列表 ')' 语句  ;
//
// Parse a FOR 语句 and return its AST
static struct 抽象句法树节点 *为_语句(void) {
  struct 抽象句法树节点 *抽象句法树条件, *抽象句法树身体;
  struct 抽象句法树节点 *抽象句法树前置操作, *抽象句法树后置操作;
  struct 抽象句法树节点 *树;

  // Ensure we have 'for' '('
  匹配(牌名_为, "为");
  左小括号();

  // Get the pre_op expression and the ';'
  抽象句法树前置操作 = 表达式_列表(牌名_分号);
  分号();

  // Get the condition and the ';'.
  // Force a non-comparison to be boolean
  // the 树'串名 operation is a comparison.
  抽象句法树条件 = 二元表达式(0);
  if (抽象句法树条件->操作 < 抽象_等于 || 抽象句法树条件->操作 > 抽象_大等)
    抽象句法树条件 =
      制作抽象句法树一元(抽象_到布尔, 抽象句法树条件->类型, 抽象句法树条件->复合类型, 抽象句法树条件, NULL, 0);
  分号();

  // Get the post_op expression and the ')'
  抽象句法树后置操作 = 表达式_列表(牌名_右小括号);
  右小括号();

  // Get the 语句 which is the 身体
  // Update the loop depth in the process
  环级++;
  抽象句法树身体 = 单个_语句();
  环级--;

  // Glue the 语句 and the postop 树
  树 = 制作抽象句法树节点(抽象_胶水, 原语_全无, NULL, 抽象句法树身体, NULL, 抽象句法树后置操作, NULL, 0);

  // Make a WHILE loop with the condition and this new 身体
  树 = 制作抽象句法树节点(抽象_一会儿, 原语_全无, NULL, 抽象句法树条件, NULL, 树, NULL, 0);

  // And glue the preop 树 to the 抽象_一会儿 树
  return (制作抽象句法树节点(抽象_胶水, 原语_全无, NULL, 抽象句法树前置操作, NULL, 树, NULL, 0));
}

// 返回_语句: 'return' '(' expression ')'  ;
//
// Parse a return 语句 and return its AST
static struct 抽象句法树节点 *返回_语句(void) {
  struct 抽象句法树节点 *树= NULL;

  // Ensure we have 'return'
  匹配(牌名_返回, "返回");

  // See if we have a return 值
  if (牌_.牌 == 牌名_左小括号) {
    // Can't return a 值 if function returns 原语_空的
    if (函数身份->类型 == 原语_空的)
      致命("不能返回来自一个空的函数");

    // Skip the 左 parenthesis
    左小括号();

    // Parse the following expression
    树 = 二元表达式(0);

    // Ensure this is compatible with the function'串名 类型
    树 = 修改_类型(树, 函数身份->类型, 函数身份->复合类型, 0);
    if (树 == NULL)
      致命("Incompatible 类型 to return");

    // Get the ')'
    右小括号();
  }

  // Add on the 抽象_返回 节点
  树 = 制作抽象句法树一元(抽象_返回, 原语_全无, NULL, 树, NULL, 0);

  // Get the ';'
  分号();
  return (树);
}

// 断_语句: 'break' ;
//
// Parse a break 语句 and return its AST
static struct 抽象句法树节点 *断_语句(void) {

  if (环级 == 0 && 切换级 == 0)
    致命("no loop or switch to break out from");
  扫描(&牌_);
  分号();
  return (制作抽象句法树叶(抽象_断, 原语_全无, NULL, NULL, 0));
}

// 继续_语句: 'continue' ;
//
// Parse a continue 语句 and return its AST
static struct 抽象句法树节点 *继续_语句(void) {

  if (环级 == 0)
    致命("no loop to continue to");
  扫描(&牌_);
  分号();
  return (制作抽象句法树叶(抽象_继续, 原语_全无, NULL, NULL, 0));
}

// Parse a switch 语句 and return its AST
static struct 抽象句法树节点 *切换_语句(void) {
  struct 抽象句法树节点 *左, *身体, *小节点, *印名;
  struct 抽象句法树节点 *情况树 = NULL, *情况尾部;
  int 在环内 = 1, 情况计数 = 0;
  int 看到默认 = 0;
  int 抽象句法树操作, 情况值;

  // Skip the 'switch' and '('
  扫描(&牌_);
  左小括号();

  // Get the switch expression, the ')' and the '{'
  左 = 二元表达式(0);
  右小括号();
  左大括号();

  // Ensure that this is of int 类型
  if (!整型类型(左->类型))
    致命("Switch expression is not of integer 类型");

  // Build an 抽象_切换 subtree with the expression as
  // the 孩子
  小节点 = 制作抽象句法树一元(抽象_切换, 原语_全无, NULL, 左, NULL, 0);

  // Now parse the cases
  切换级++;
  while (在环内) {
    switch (牌_.牌) {
	// Leave the loop when we hit a '}'
      case 牌名_右大括号:
	if (情况计数 == 0)
	  致命("No cases in switch");
	在环内 = 0;
	break;
      case 牌名_情况:
      case 牌名_默认:
	// Ensure this isn't after a previous 'default'
	if (看到默认)
	  致命("case or default after existing default");

	// Set the AST operation. Scan the case 值 if required
	if (牌_.牌 == 牌名_默认) {
	  抽象句法树操作 = 抽象_默认;
	  看到默认 = 1;
	  扫描(&牌_);
	} else {
	  抽象句法树操作 = 抽象_情况;
	  扫描(&牌_);
	  左 = 二元表达式(0);

	  // Ensure the case 值 is an integer literal
	  if (左->操作 != 抽象_整型字面)
	    致命("Expecting integer literal for case 值");
	  情况值 = 左->小抽象_整型值;

	  // Walk the 列表 of existing case values to ensure
	  // that there isn't a duplicate case 值
	  for (印名 = 情况树; 印名 != NULL; 印名 = 印名->右)
	    if (情况值 == 印名->小抽象_整型值)
	      致命("Duplicate case 值");
	}

	// Scan the ':' and increment the 情况计数
	匹配(牌名_冒号, ":");
	情况计数++;

	// If the 下一个 牌 is a 牌名_情况, the existing case will fall
	// into the 下一个 case. Otherwise, parse the case 身体.
	if (牌_.牌 == 牌名_情况)
	  身体 = NULL;
	else
	  身体 = 复合_语句(1);

	// Build a sub-树 with any compound 语句 as the 左 孩子
	// and link it in to the growing 抽象_情况 树
	if (情况树 == NULL) {
	  情况树 = 情况尾部 =
	    制作抽象句法树一元(抽象句法树操作, 原语_全无, NULL, 身体, NULL, 情况值);
	} else {
	  情况尾部->右 =
	    制作抽象句法树一元(抽象句法树操作, 原语_全无, NULL, 身体, NULL, 情况值);
	  情况尾部 = 情况尾部->右;
	}
	break;
      default:
	致命符("Unexpected 牌 in switch", 牌_.牌串);
    }
  }
  切换级--;

  // We have a sub-树 with the cases and any default. Put the
  // case count into the 抽象_切换 节点 and attach the case 树.
  小节点->小抽象_整型值 = 情况计数;
  小节点->右 = 情况树;
  右大括号();

  return (小节点);
}

// Parse a single 语句 and return its AST.
static struct 抽象句法树节点 *单个_语句(void) {
  struct 抽象句法树节点 *小语句;
  struct 符号表 *复合类型;
  int 行号= 行;

  switch (牌_.牌) {
    case 牌名_分号:
      // An empty 语句
      分号();
      break;
    case 牌名_左大括号:
      // We have a '{', so this is a compound 语句
      左大括号();
      小语句 = 复合_语句(0);
      小语句->行号= 行号;
      右大括号();
      return (小语句);
    case 牌名_标识:
      // We have to see if the 标识符 matches a typedef.
      // If not, treat it as an expression.
      // Otherwise, fall down to the 解析_类型() call.
      if (找类型定义(文本) == NULL) {
	小语句 = 二元表达式(0);
        小语句->行号= 行号;
	分号();
	return (小语句);
      }
    case 牌名_印刻:
    case 牌名_整型:
    case 牌名_长型:
    case 牌名_结构:
    case 牌名_联合:
    case 牌名_枚举:
    case 牌名_类型定义:
      // The beginning of a variable declaration 列表.
      声明_列表(&复合类型, 类别_本地, 牌名_分号, 牌名_文件终, &小语句);
      分号();
      return (小语句);		// Any assignments from the declarations
    case 牌名_若:
      小语句= 若_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_一会儿:
      小语句= 一会儿_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_为:
      小语句= 为_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_返回:
      小语句= 返回_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_断:
      小语句= 断_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_继续:
      小语句= 继续_语句(); 小语句->行号= 行号; return(小语句);
    case 牌名_切换:
      小语句= 切换_语句(); 小语句->行号= 行号; return(小语句);
    default:
      // For now, see if this is an expression.
      // This catches assignment 语句_复.
      小语句 = 二元表达式(0);
      小语句->行号= 行号;
      分号();
      return (小语句);
  }
  return (NULL);		// Keep -Wall happy
}

// Parse a compound 语句
// and return its AST. If 在切换内 is true,
// we look for a '}', 'case' or 'default' 牌
// to end the parsing. Otherwise, look for
// just a '}' to end the parsing.
struct 抽象句法树节点 *复合_语句(int 在切换内) {
  struct 抽象句法树节点 *左 = NULL;
  struct 抽象句法树节点 *树;

  while (1) {
    // Leave if we've hit the end 牌. We do this first to allow
    // an empty compound 语句
    if (牌_.牌 == 牌名_右大括号)
      return (左);
    if (在切换内 && (牌_.牌 == 牌名_情况 || 牌_.牌 == 牌名_默认))
      return (左);

    // Parse a single 语句
    树 = 单个_语句();

    // For each new 树, either save it in 左
    // if 左 is empty, or glue the 左 and the
    // new 树 together
    if (树 != NULL) {
      if (左 == NULL)
	左 = 树;
      else
	左 = 制作抽象句法树节点(抽象_胶水, 原语_全无, NULL, 左, NULL, 树, NULL, 0);
    }
  }
  return (NULL);		// Keep -Wall happy
}

/*------------------------------------------------
------ 表达式.印名 -------
------------------------------------------------*/

// Parsing of expressions
//   (印名)   

// 表达式_列表: <null>
//        | 表达式
//        | 表达式 ',' 表达式_列表
//        ;

// Parse a 列表 of zero or more 逗号-separated expressions and
// return an AST composed of 抽象_胶水 nodes with the 左-hand 孩子
// being the sub-树 of previous expressions (or NULL) and the 右-hand
// 孩子 being the 下一个 expression. Each 抽象_胶水 节点 will have 大小 field
// set to the number of expressions in the 树 at this point. If no
// expressions are parsed, NULL is returned
struct 抽象句法树节点 *表达式_列表(int 终牌) {
  struct 抽象句法树节点 *树 = NULL;
  struct 抽象句法树节点 *孩子 = NULL;
  int 表达式计数 = 0;

  // Loop until the end 牌
  while (牌_.牌 != 终牌) {

    // Parse the 下一个 expression and increment the expression count
    孩子 = 二元表达式(0);
    表达式计数++;

    // Build an 抽象_胶水 AST 节点 with the previous 树 as the 左 孩子
    // and the new expression as the 右 孩子. Store the expression count.
    树 =
      制作抽象句法树节点(抽象_胶水, 原语_全无, NULL, 树, NULL, 孩子, NULL, 表达式计数);

    // Stop when we reach the end 牌
    if (牌_.牌 == 终牌)
      break;

    // Must have a ',' at this point
    匹配(牌名_逗号, ",");
  }

  // Return the 树 of expressions
  return (树);
}

// Parse a function call and return its AST
static struct 抽象句法树节点 *函数调用(void) {
  struct 抽象句法树节点 *树;
  struct 符号表 *函数指针;

  // Check that the 标识符 has been defined as a function,
  // then make a leaf 节点 for it.
  if ((函数指针 = 找符号(文本)) == NULL || 函数指针->结构化类型 != 结构化_函数) {
    致命符("没声明函数", 文本);
  }

  // Get the '('
  左小括号();

  // Parse the argument expression 列表
  树 = 表达式_列表(牌名_右小括号);

  // XXX Check 类型 of each argument against the function'串名 prototype

  // Build the function call AST 节点. Store the
  // function'串名 return 类型 as this 节点'串名 类型.
  // Also record the function'串名 symbol-身份
  树 =
    制作抽象句法树一元(抽象_函数调用, 函数指针->类型, 函数指针->复合类型, 树, 函数指针, 0);

  // Get the ')'
  右小括号();
  return (树);
}

// Parse the index into an array and return an AST 树 for it
static struct 抽象句法树节点 *数组_访问(struct 抽象句法树节点 *左) {
  struct 抽象句法树节点 *右;

  // Check that the sub-树 is a pointer
  if (!指针类型(左->类型))
    致命("非一数组或指针");

  // Get the '['
  扫描(&牌_);

  // Parse the following expression
  右 = 二元表达式(0);

  // Get the ']'
  匹配(牌名_右中括号, "]");

  // Ensure that this is of int 类型
  if (!整型类型(右->类型))
    致命("Array index is not of integer 类型");

  // Make the 左 树 an 右值
  左->右值 = 1;

  // Scale the index by the 大小 of the element'串名 类型
  右 = 修改_类型(右, 左->类型, 左->复合类型, 抽象_加法);

  // Return an AST 树 where the array'串名 base has the 偏移 added to it,
  // and dereference the element. Still an lvalue at this point.
  左 =
    制作抽象句法树节点(抽象_加法, 左->类型, 左->复合类型, 左, NULL, 右, NULL, 0);
  左 =
    制作抽象句法树一元(抽象_解引用, 值_位置(左->类型), 左->复合类型, 左, NULL, 0);
  return (左);
}

// Parse the 成员 reference of a struct or union
// and return an AST 树 for it. If 和指针一起 is true,
// the access is through a pointer to the 成员.
static struct 抽象句法树节点 *成员_访问(struct 抽象句法树节点 *左, int 和指针一起) {
  struct 抽象句法树节点 *右;
  struct 符号表 *类型指针;
  struct 符号表 *小成员;

  // Check that the 左 AST 树 is a pointer to struct or union
  if (和指针一起 && 左->类型 != 指针_到(原语_结构)
      && 左->类型 != 指针_到(原语_联合))
    致命("Expression is not a pointer to a struct/union");

  // Or, check that the 左 AST 树 is a struct or union.
  // If so, change it from an 抽象_标识 to an 抽象_地址 so that
  // we get the base address, not the 值 at this address.
  if (!和指针一起) {
    if (左->类型 == 原语_结构 || 左->类型 == 原语_联合)
      左->操作 = 抽象_地址;
    else
      致命("Expression is not a struct/union");
  }

  // Get the details of the composite 类型
  类型指针 = 左->复合类型;

  // Skip the '.' or '->' 牌 and get the 成员'串名 名字
  扫描(&牌_);
  标识();

  // Find the matching 成员'串名 名字 in the 类型
  // Die if we can't find it
  for (小成员 = 类型指针->成员; 小成员 != NULL; 小成员 = 小成员->下一个)
    if (!strcmp(小成员->名字, 文本))
      break;
  if (小成员 == NULL)
    致命符("No 成员 found in struct/union: ", 文本);

  // Make the 左 树 an 右值
  左->右值 = 1;

  // Build an 抽象_整型字面 节点 with the 偏移
  右 = 制作抽象句法树叶(抽象_整型字面, 原语_整型, NULL, NULL, 小成员->符号表_本地符号位置信息);

  // Add the 成员'串名 偏移 to the base of the struct/union
  // and dereference it. Still an lvalue at this point
  左 =
    制作抽象句法树节点(抽象_加法, 指针_到(小成员->类型), 小成员->复合类型, 左, NULL, 右, NULL,
	      0);
  左 = 制作抽象句法树一元(抽象_解引用, 小成员->类型, 小成员->复合类型, 左, NULL, 0);
  return (左);
}

// Parse a parenthesised expression and
// return an AST 节点 representing it.
static struct 抽象句法树节点 *小括号_表达式(int 上牌优) {
  struct 抽象句法树节点 *小节点;
  int 类型 = 0;
  struct 符号表 *复合类型 = NULL;

  // Beginning of a parenthesised expression, 跳过 the '('.
  扫描(&牌_);

  // If the 牌 after is a 类型 标识符, this is a cast expression
  switch (牌_.牌) {
  case 牌名_标识:
    // We have to see if the 标识符 matches a typedef.
    // If not, treat it as an expression.
    if (找类型定义(文本) == NULL) {
      小节点 = 二元表达式(0);	// 上牌优 is zero as expression inside ( )
      break;
    }
  case 牌名_空的:
  case 牌名_印刻:
  case 牌名_整型:
  case 牌名_长型:
  case 牌名_结构:
  case 牌名_联合:
  case 牌名_枚举:
    // Get the 类型 inside the parentheses
    类型 = 解析_投射(&复合类型);

    // Skip the closing ')' and then parse the following expression
    右小括号();

  default:
    小节点 = 二元表达式(上牌优);		// Scan in the expression. We pass in 上牌优
				// as the cast doesn't change the
				// expression'串名 precedence
  }

  // We now have at least an expression in 小节点, and possibly a non-zero 类型
  // in 类型 if there was a cast. Skip the closing ')' if there was no cast.
  if (类型 == 0)
    右小括号();
  else
    // Otherwise, make a unary AST 节点 for the cast
    小节点 = 制作抽象句法树一元(抽象_投射, 类型, 复合类型, 小节点, NULL, 0);
  return (小节点);
}

// Parse a 基元 因子 and return an
// AST 节点 representing it.
static struct 抽象句法树节点 *基元(int 上牌优) {
  struct 抽象句法树节点 *小节点;
  struct 符号表 *枚举指针;
  struct 符号表 *变量指针;
  int 身份;
  int 类型 = 0;
  int 大小, 类别;
  struct 符号表 *复合类型;

  switch (牌_.牌) {
  case 牌名_静态:
  case 牌名_外部:
    致命("Compiler doesn't support static or extern local declarations");
  case 牌名_求大小:
    // Skip the 牌名_求大小 and ensure we have a 左 parenthesis
    扫描(&牌_);
    if (牌_.牌 != 牌名_左小括号)
      致命("Left parenthesis expected after sizeof");
    扫描(&牌_);

    // Get the 类型 inside the parentheses
    类型 = 解析_星号(解析_类型(&复合类型, &类别));

    // Get the 类型'串名 大小
    大小 = 类型大小(类型, 复合类型);
    右小括号();

    // Make a leaf 节点 int literal with the 大小
    return (制作抽象句法树叶(抽象_整型字面, 原语_整型, NULL, NULL, 大小));

  case 牌名_整型字面:
    // For an 整型字面 牌, make a leaf AST 节点 for it.
    // Make it a 原语_印刻 if it'串名 within the 原语_印刻 range
    if (牌_.整型值 >= 0 && 牌_.整型值 < 256)
      小节点 = 制作抽象句法树叶(抽象_整型字面, 原语_印刻, NULL, NULL, 牌_.整型值);
    else
      小节点 = 制作抽象句法树叶(抽象_整型字面, 原语_整型, NULL, NULL, 牌_.整型值);
    break;

  case 牌名_串字面:
    // For a STRLIT 牌, generate the assembly for it.
    身份 = 生成全局串(文本, 0);

    // For successive STRLIT tokens, 追加 their contents
    // to this one
    while (1) {
      扫描(&窥牌);
      if (窥牌.牌 != 牌名_串字面) break;
      生成全局串(文本, 1);
      扫描(&牌_);	// To 跳过 it properly
    }

    // Now make a leaf AST 节点 for it. 身份 is the 串'串名 标号.
    生成全局串终();
    小节点 = 制作抽象句法树叶(抽象_串字面, 指针_到(原语_印刻), NULL, NULL, 身份);
    break;

  case 牌名_标识:
    // If the 标识符 matches an enum 值,
    // return an 抽象_整型字面 节点
    if ((枚举指针 = 找枚举值(文本)) != NULL) {
      小节点 = 制作抽象句法树叶(抽象_整型字面, 原语_整型, NULL, NULL, 枚举指针->符号表_本地符号位置信息);
      break;
    }

    // See if this 标识符 exists as a symbol. For arrays, set 右值 to 1.
    if ((变量指针 = 找符号(文本)) == NULL)
      致命符("Unknown variable or function", 文本);
    switch (变量指针->结构化类型) {
    case 结构化_可变的:
      小节点 = 制作抽象句法树叶(抽象_标识, 变量指针->类型, 变量指针->复合类型, 变量指针, 0);
      break;
    case 结构化_数组:
      小节点 = 制作抽象句法树叶(抽象_地址, 变量指针->类型, 变量指针->复合类型, 变量指针, 0);
      小节点->右值 = 1;
      break;
    case 结构化_函数:
      // Function call, see if the 下一个 牌 is a 左 parenthesis
      扫描(&牌_);
      if (牌_.牌 != 牌名_左小括号)
	致命符("Function 名字 used without parentheses", 文本);
      return (函数调用());
    default:
      致命符("Identifier not a scalar or array variable", 文本);
    }

    break;

  case 牌名_左小括号:
    return (小括号_表达式(上牌优));

  default:
    致命符("Expecting a 基元 expression, got 牌", 牌_.牌串);
  }

  // Scan in the 下一个 牌 and return the leaf 节点
  扫描(&牌_);
  return (小节点);
}

// Parse a 后缀 expression and return
// an AST 节点 representing it. The
// 标识符 is already in 文本.
static struct 抽象句法树节点 *后缀(int 上牌优) {
  struct 抽象句法树节点 *小节点;

  // Get the 基元 expression
  小节点 = 基元(上牌优);

  // Loop until there are no more 后缀 operators
  while (1) {
    switch (牌_.牌) {
    case 牌名_左中括号:
      // An array reference
      小节点 = 数组_访问(小节点);
      break;

    case 牌名_句号:
      // Access into a struct or union
      小节点 = 成员_访问(小节点, 0);
      break;

    case 牌名_箭头:
      // Pointer access into a struct or union
      小节点 = 成员_访问(小节点, 1);
      break;

    case 牌名_自增:
      // Post-increment: 跳过 over the 牌
      if (小节点->右值 == 1)
	致命("Cannot ++ on 右值");
      扫描(&牌_);

      // Can't do it twice
      if (小节点->操作 == 抽象_后置自增 || 小节点->操作 == 抽象_后置自减)
	致命("Cannot ++ and/or -- more than once");

      // and change the AST operation
      小节点->操作 = 抽象_后置自增;
      break;

    case 牌名_自减:
      // Post-decrement: 跳过 over the 牌
      if (小节点->右值 == 1)
	致命("Cannot -- on 右值");
      扫描(&牌_);

      // Can't do it twice
      if (小节点->操作 == 抽象_后置自增 || 小节点->操作 == 抽象_后置自减)
	致命("Cannot ++ and/or -- more than once");

      // and change the AST operation
      小节点->操作 = 抽象_后置自减;
      break;

    default:
      return (小节点);
    }
  }

  return (NULL);		// Keep -Wall happy
}


// Convert a binary operator 牌 into a binary AST operation.
// We rely on a 1:1 mapping from 牌 to AST operation
static int 二元抽象句法树操作(int 牌类型) {
  if (牌类型 > 牌名_文件终 && 牌类型 <= 牌名_余数)
    return (牌类型);
  致命符("Syntax error, 牌", 牌名串[牌类型]);
  return (0);			// Keep -Wall happy
}

// Return true if a 牌 is 右-associative,
// false otherwise.
static int 右关联(int 牌类型) {
  if (牌类型 >= 牌名_赋值 && 牌类型 <= 牌名_赋值斜杠)
    return (1);
  return (0);
}

// Operator precedence for each 牌. Must
// 匹配 up with the order of tokens in 定义.h
static int 操作优先级[] = {
  0, 10, 10,			// 牌名_文件终, 牌名_赋值, 牌名_赋值加,
  10, 10,			// 牌名_赋值减, 牌名_赋值星号,
  10, 10,			// 牌名_赋值斜杠, 牌名_赋值余数,
  15,				// 牌名_问号,
  20, 30,			// 牌名_逻辑或, 牌名_逻辑与
  40, 50, 60,			// 牌名_或, 牌名_异或, 牌名_与 
  70, 70,			// 牌名_等于, 牌名_不等
  80, 80, 80, 80,		// 牌名_小于, 牌名_大于, 牌名_小等, 牌名_大等
  90, 90,			// 牌名_左移, 牌名_右移
  100, 100,			// 牌名_加, 牌名_减
  110, 110, 110			// 牌名_星号, 牌名_斜杠, 牌名_余数
};

// Check that we have a binary operator and
// return its precedence.
static int 操作_优先级(int 牌类型) {
  int 小优先级;
  if (牌类型 > 牌名_余数)
    致命符("牌_ with no precedence in 操作_优先级:", 牌名串[牌类型]);
  小优先级 = 操作优先级[牌类型];
  if (小优先级 == 0)
    致命符("Syntax error, 牌", 牌名串[牌类型]);
  return (小优先级);
}

// 前缀_表达式: 后缀_表达式
//     | '*'  前缀_表达式
//     | '&'  前缀_表达式
//     | '-'  前缀_表达式
//     | '++' 前缀_表达式
//     | '--' 前缀_表达式
//     ;

// Parse a 前缀 expression and return 
// a sub-树 representing it.
static struct 抽象句法树节点 *前缀(int 上牌优) {
  struct 抽象句法树节点 *树;
  switch (牌_.牌) {
  case 牌名_与:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // Ensure that it'串名 an 标识符
    if (树->操作 != 抽象_标识)
      致命("& operator must be followed by an 标识符");

    // Prevent '&' being performed on an array
    if (树->符号->结构化类型 == 结构化_数组)
      致命("& operator cannot be performed on an array");

    // Now change the operator to 抽象_地址 and the 类型 to
    // a pointer to the original 类型
    树->操作 = 抽象_地址;
    树->类型 = 指针_到(树->类型);
    break;
  case 牌名_星号:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression.
    // Make it an 右值
    扫描(&牌_);
    树 = 前缀(上牌优);
    树->右值= 1;

    // Ensure the 树'串名 类型 is a pointer
    if (!指针类型(树->类型))
      致命("* operator must be followed by an expression of pointer 类型");

    // Prepend an 抽象_解引用 operation to the 树
    树 =
      制作抽象句法树一元(抽象_解引用, 值_位置(树->类型), 树->复合类型, 树, NULL, 0);
    break;
  case 牌名_减:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // Prepend a 抽象_否定 operation to the 树 and
    // make the 孩子 an 右值. Because chars are unsigned,
    // also widen this if needed to int so that it'串名 signed
    树->右值 = 1;
    if (树->类型 == 原语_印刻)
      树->类型 = 原语_整型;
    树 = 制作抽象句法树一元(抽象_否定, 树->类型, 树->复合类型, 树, NULL, 0);
    break;
  case 牌名_颠倒:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // Prepend a 抽象_颠倒 operation to the 树 and
    // make the 孩子 an 右值.
    树->右值 = 1;
    树 = 制作抽象句法树一元(抽象_颠倒, 树->类型, 树->复合类型, 树, NULL, 0);
    break;
  case 牌名_逻辑非:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // Prepend a 抽象_逻辑非 operation to the 树 and
    // make the 孩子 an 右值.
    树->右值 = 1;
    树 = 制作抽象句法树一元(抽象_逻辑非, 树->类型, 树->复合类型, 树, NULL, 0);
    break;
  case 牌名_自增:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // For now, ensure it'串名 an 标识符
    if (树->操作 != 抽象_标识)
      致命("++ operator must be followed by an 标识符");

    // Prepend an 抽象_前置自增 operation to the 树
    树 = 制作抽象句法树一元(抽象_前置自增, 树->类型, 树->复合类型, 树, NULL, 0);
    break;
  case 牌名_自减:
    // Get the 下一个 牌 and parse it
    // recursively as a 前缀 expression
    扫描(&牌_);
    树 = 前缀(上牌优);

    // For now, ensure it'串名 an 标识符
    if (树->操作 != 抽象_标识)
      致命("-- operator must be followed by an 标识符");

    // Prepend an 抽象_前置自减 operation to the 树
    树 = 制作抽象句法树一元(抽象_前置自减, 树->类型, 树->复合类型, 树, NULL, 0);
    break;
  default:
    树 = 后缀(上牌优);
  }
  return (树);
}

// Return an AST 树 whose root is a binary operator.
// Parameter 上牌优 is the previous 牌'串名 precedence.
struct 抽象句法树节点 *二元表达式(int 上牌优) {
  struct 抽象句法树节点 *左, *右;
  struct 抽象句法树节点 *左临时, *右临时;
  int 抽象句法树操作;
  int 牌类型;

  // Get the 树 on the 左.
  // Fetch the 下一个 牌 at the same time.
  左 = 前缀(上牌优);

  // If we hit one of several terminating tokens, return just the 左 节点
  牌类型 = 牌_.牌;
  if (牌类型 == 牌名_分号 || 牌类型 == 牌名_右小括号 ||
      牌类型 == 牌名_右中括号 || 牌类型 == 牌名_逗号 ||
      牌类型 == 牌名_冒号 || 牌类型 == 牌名_右大括号) {
    左->右值 = 1;
    return (左);
  }

  // While the precedence of this 牌 is more than that of the
  // previous 牌 precedence, or it'串名 右 associative and
  // equal to the previous 牌'串名 precedence
  while ((操作_优先级(牌类型) > 上牌优) ||
	 (右关联(牌类型) && 操作_优先级(牌类型) == 上牌优)) {
    // Fetch in the 下一个 integer literal
    扫描(&牌_);

    // Recursively call 二元表达式() with the
    // precedence of our 牌 to build a sub-树
    右 = 二元表达式(操作优先级[牌类型]);

    // Determine the operation to be performed on the sub-trees
    抽象句法树操作 = 二元抽象句法树操作(牌类型);

    switch (抽象句法树操作) {
    case 抽象_三元运算符:
      // Ensure we have a ':' 牌, 扫描 in the expression after it
      匹配(牌名_冒号, ":");
      左临时 = 二元表达式(0);

      // Build and return the AST for this 语句. Use the middle
      // expression'串名 类型 as the return 类型. XXX We should also
      // consider the third expression'串名 类型.
      return (制作抽象句法树节点
	      (抽象_三元运算符, 右->类型, 右->复合类型, 左, 右, 左临时,
	       NULL, 0));

    case 抽象_赋值:
      // Assignment
      // Make the 右 树 into an 右值
      右->右值 = 1;

      // Ensure the 右'串名 类型 matches the 左
      右 = 修改_类型(右, 左->类型, 左->复合类型, 0);
      if (右 == NULL)
	致命("Incompatible expression in assignment");

      // Make an assignment AST 树. However, switch
      // 左 and 右 around, so that the 右 expression'串名 
      // code will be generated before the 左 expression
      左临时 = 左;
      左 = 右;
      右 = 左临时;
      break;

    default:
      // We are not doing a ternary or assignment, so both trees should
      // be rvalues. Convert both trees into 右值 if they are lvalue trees
      左->右值 = 1;
      右->右值 = 1;

      // Ensure the two types are compatible by trying
      // to modify each 树 to 匹配 the other'串名 类型.
      左临时 = 修改_类型(左, 右->类型, 右->复合类型, 抽象句法树操作);
      右临时 = 修改_类型(右, 左->类型, 左->复合类型, 抽象句法树操作);
      if (左临时 == NULL && 右临时 == NULL)
	致命("Incompatible types in binary expression");
      if (左临时 != NULL)
	左 = 左临时;
      if (右临时 != NULL)
	右 = 右临时;
    }

    // Join that sub-树 with ours. Convert the 牌
    // into an AST operation at the same time.
    左 =
      制作抽象句法树节点(二元抽象句法树操作(牌类型), 左->类型, 左->复合类型, 左, NULL,
		右, NULL, 0);

    // Some operators produce an int result regardless of their operands
    switch (二元抽象句法树操作(牌类型)) {
    case 抽象_逻辑或:
    case 抽象_逻辑与:
    case 抽象_等于:
    case 抽象_不等:
    case 抽象_小于:
    case 抽象_大于:
    case 抽象_小等:
    case 抽象_大等:
      左->类型 = 原语_整型;
    }

    // Update the details of the current 牌.
    // If we hit a terminating 牌, return just the 左 节点
    牌类型 = 牌_.牌;
    if (牌类型 == 牌名_分号 || 牌类型 == 牌名_右小括号 ||
	牌类型 == 牌名_右中括号 || 牌类型 == 牌名_逗号 ||
	牌类型 == 牌名_冒号 || 牌类型 == 牌名_右大括号) {
      左->右值 = 1;
      return (左);
    }
  }

  // Return the 树 we have when the precedence
  // is the same or lower
  左->右值 = 1;
  return (左);
}

/*------------------------------------------------
------ 优化.印名-------
------------------------------------------------*/

// AST Tree Optimisation Code
//   (印名)   

// Fold an AST 树 with a binary operator
// and two 抽象_整型字面 children. Return either 
// the original 树 or a new leaf 节点.
static struct 抽象句法树节点 *折叠2(struct 抽象句法树节点 *小节点) {
  int 小值, 左值, 右值;

  // Get the values from each 孩子
  左值 = 小节点->左->小抽象_整型值;
  右值 = 小节点->右->小抽象_整型值;

  // Perform some of the binary operations.
  // For any AST 操作 we can't do, return
  // the original 树.
  switch (小节点->操作) {
    case 抽象_加法:
      小值 = 左值 + 右值;
      break;
    case 抽象_减法:
      小值 = 左值 - 右值;
      break;
    case 抽象_乘法:
      小值 = 左值 * 右值;
      break;
    case 抽象_除法:
      // Don't try to divide by zero.
      if (右值 == 0)
	return (小节点);
      小值 = 左值 / 右值;
      break;
    default:
      return (小节点);
  }

  // Return a leaf 节点 with the new 值
  return (制作抽象句法树叶(抽象_整型字面, 小节点->类型, NULL, NULL, 小值));
}

// Fold an AST 树 with a unary operator
// and one 整型字面 children. Return either 
// the original 树 or a new leaf 节点.
static struct 抽象句法树节点 *折叠1(struct 抽象句法树节点 *小节点) {
  int 小值;

  // Get the 孩子 值. Do the
  // operation if recognised.
  // Return the new leaf 节点.
  小值 = 小节点->左->小抽象_整型值;
  switch (小节点->操作) {
    case 抽象_加宽:
      break;
    case 抽象_颠倒:
      小值 = ~小值;
      break;
    case 抽象_逻辑非:
      小值 = !小值;
      break;
    default:
      return (小节点);
  }

  // Return a leaf 节点 with the new 值
  return (制作抽象句法树叶(抽象_整型字面, 小节点->类型, NULL, NULL, 小值));
}

// Attempt to do constant folding on
// the AST 树 with the root 节点 小节点
static struct 抽象句法树节点 *折叠(struct 抽象句法树节点 *小节点) {

  if (小节点 == NULL)
    return (NULL);

  // Fold on the 左 孩子, then
  // do the same on the 右 孩子
  小节点->左 = 折叠(小节点->左);
  小节点->右 = 折叠(小节点->右);

  // If both children are A_INTLITs, do a 折叠2()
  if (小节点->左 && 小节点->左->操作 == 抽象_整型字面) {
    if (小节点->右 && 小节点->右->操作 == 抽象_整型字面)
      小节点 = 折叠2(小节点);
    else
      // If only the 左 is 抽象_整型字面, do a 折叠1()
      小节点 = 折叠1(小节点);
  }

  // Return the possibly modified 树
  return (小节点);
}

// Optimise an AST 树 by
// constant folding in all sub-trees
struct 抽象句法树节点 *优化(struct 抽象句法树节点 *小节点) {
  小节点 = 折叠(小节点);
  return (小节点);
}
