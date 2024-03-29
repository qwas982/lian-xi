#include "定义.h" // 引入自定义头文件 "定义.h"
#include "数据.h" // 引入自定义头文件 "数据.h"
#include "声明.h" // 引入自定义头文件 "声明.h"


// 抽象句法树解释器


// 抽象句法树运算符列表
static char *抽象句法树操作[] = { "+", "-", "*", "/" };

// 给定一个抽象句法树，解释其中的运算符并返回最终值。
int 解释抽象句法树(struct 抽象句法树节点 *节点指针) {
  int 左子树值, 右子树值;

  // 获取左子树和右子树的值
  if (节点指针->左子树)
    左子树值 = 解释抽象句法树(节点指针->左子树);
  if (节点指针->右子树)
    右子树值 = 解释抽象句法树(节点指针->右子树);

  // 调试：打印即将执行的操作
  // if (节点指针->操作 == 抽象_整型字面)
  //   printf("整数 %d\n", 节点指针->整型值);
  // else
  //   printf("%d %s %d\n", 左子树值, 抽象句法树操作[节点指针->操作], 右子树值);

  switch (节点指针->操作) {
    case 抽象_加:
      return (左子树值 + 右子树值);
    case 抽象_减:
      return (左子树值 - 右子树值);
    case 抽象_乘:
      return (左子树值 * 右子树值);
    case 抽象_除:
      return (左子树值 / 右子树值);
    case 抽象_整型字面:
      return (节点指针->整型值);
    default:
      fprintf(stderr, "未知的抽象句法树运算符 %d\n", 节点指针->操作);
      exit(1);
  }
}