#include "chibicc.h"

类型 *类型_空的_小 = &(类型){类型_空的, 1, 1};
类型 *类型_布尔_小 = &(类型){类型_布尔, 1, 1};

类型 *类型_字符_小 = &(类型){类型_字符, 1, 1};
类型 *类型_短的_小 = &(类型){类型_短的, 2, 2};
类型 *类型_整型_小 = &(类型){类型_整型, 4, 4};
类型 *类型_长的_小 = &(类型){类型_长的, 8, 8};

类型 *类型_无符号字符 = &(类型){类型_字符, 1, 1, true};
类型 *类型_无符号短的 = &(类型){类型_短的, 2, 2, true};
类型 *类型_无符号整型 = &(类型){类型_整型, 4, 4, true};
类型 *类型_无符号长的 = &(类型){类型_长的, 8, 8, true};

类型 *类型_浮点_小 = &(类型){类型_浮点, 4, 4};
类型 *类型_双浮_小 = &(类型){类型_双浮, 8, 8};
类型 *类型_长双浮_小 = &(类型){类型_长双浮, 16, 16};

static 类型 *新_类型_(类型种类 种类, int 大小, int 对齐_短) {
  类型 *类型_缩 = calloc(1, sizeof(类型));
  类型_缩->种类 = 种类;
  类型_缩->大小 = 大小;
  类型_缩->对齐_短 = 对齐_短;
  return 类型_缩;
}

bool 是否_整数_小(类型 *类型_缩) {
  类型种类 k = 类型_缩->种类;
  return k == 类型_布尔 || k == 类型_字符 || k == 类型_短的 ||
         k == 类型_整型  || k == 类型_长的 || k == 类型_枚举;
}

bool 是否_浮点数_小(类型 *类型_缩) {
  return 类型_缩->种类 == 类型_浮点 || 类型_缩->种类 == 类型_双浮 ||
         类型_缩->种类 == 类型_长双浮;
}

bool 是否_数值型_小(类型 *类型_缩) {
  return 是否_整数_小(类型_缩) || 是否_浮点数_小(类型_缩);
}

bool 是否_兼容_小(类型 *t1, 类型 *t2) {
  if (t1 == t2)
    return true;

  if (t1->原创)
    return 是否_兼容_小(t1->原创, t2);

  if (t2->原创)
    return 是否_兼容_小(t1, t2->原创);

  if (t1->种类 != t2->种类)
    return false;

  switch (t1->种类) {
  case 类型_字符:
  case 类型_短的:
  case 类型_整型:
  case 类型_长的:
    return t1->是否_无符号 == t2->是否_无符号;
  case 类型_浮点:
  case 类型_双浮:
  case 类型_长双浮:
    return true;
  case 类型_指针:
    return 是否_兼容_小(t1->基础, t2->基础);
  case 类型_函数: {
    if (!是否_兼容_小(t1->返回_类型, t2->返回_类型))
      return false;
    if (t1->是否_可变参数 != t2->是否_可变参数)
      return false;

    类型 *p1 = t1->形参_短;
    类型 *p2 = t2->形参_短;
    for (; p1 && p2; p1 = p1->下一个, p2 = p2->下一个)
      if (!是否_兼容_小(p1, p2))
        return false;
    return p1 == NULL && p2 == NULL;
  }
  case 类型_数组:
    if (!是否_兼容_小(t1->基础, t2->基础))
      return false;
    return t1->数组_长度 < 0 && t2->数组_长度 < 0 &&
           t1->数组_长度 == t2->数组_长度;
  }
  return false;
}

类型 *复制_类型(类型 *类型_缩) {
  类型 *ret = calloc(1, sizeof(类型));
  *ret = *类型_缩;
  ret->原创 = 类型_缩;
  return ret;
}

类型 *指针_到(类型 *基础) {
  类型 *类型_缩 = 新_类型_(类型_指针, 8, 8);
  类型_缩->基础 = 基础;
  类型_缩->是否_无符号 = true;
  return 类型_缩;
}

类型 *函数_类型_小(类型 *返回_类型) {
  // The C spec disallows sizeof(<正函数_ type>), but
  // GCC allows that and the expression is evaluated to 1.
  类型 *类型_缩 = 新_类型_(类型_函数, 1, 1);
  类型_缩->返回_类型 = 返回_类型;
  return 类型_缩;
}

类型 *数组_阵列(类型 *基础, int 长度_短) {
  类型 *类型_缩 = 新_类型_(类型_数组, 基础->大小 * 长度_短, 基础->对齐_短);
  类型_缩->基础 = 基础;
  类型_缩->数组_长度 = 长度_短;
  return 类型_缩;
}

类型 *变长数组_阵列(类型 *基础, 节点 *长度_短) {
  类型 *类型_缩 = 新_类型_(类型_变长数组, 8, 8);
  类型_缩->基础 = 基础;
  类型_缩->变长数组_长度 = 长度_短;
  return 类型_缩;
}

类型 *枚举_类型_小(void) {
  return 新_类型_(类型_枚举, 4, 4);
}

类型 *结构_类型_小(void) {
  return 新_类型_(类型_结构, 0, 1);
}

static 类型 *取_公共的_类型(类型 *ty1, 类型 *ty2) {
  if (ty1->基础)
    return 指针_到(ty1->基础);

  if (ty1->种类 == 类型_函数)
    return 指针_到(ty1);
  if (ty2->种类 == 类型_函数)
    return 指针_到(ty2);

  if (ty1->种类 == 类型_长双浮 || ty2->种类 == 类型_长双浮)
    return 类型_长双浮_小;
  if (ty1->种类 == 类型_双浮 || ty2->种类 == 类型_双浮)
    return 类型_双浮_小;
  if (ty1->种类 == 类型_浮点 || ty2->种类 == 类型_浮点)
    return 类型_浮点_小;

  if (ty1->大小 < 4)
    ty1 = 类型_整型_小;
  if (ty2->大小 < 4)
    ty2 = 类型_整型_小;

  if (ty1->大小 != ty2->大小)
    return (ty1->大小 < ty2->大小) ? ty2 : ty1;

  if (ty2->是否_无符号)
    return ty2;
  return ty1;
}

// For many 二元_小写 operators, we implicitly promote operands so that
// both operands have the same type. Any integral type smaller than
// int is always promoted to int. If the type of one operand is larger
// than the other's (e.g. "long" vs. "int"), the smaller operand will
// be promoted to 匹配 with the other.
//
// This operation is called the "usual arithmetic conversion".
static void 通常的_算术_转换_短(节点 **左手边, 节点 **右手边) {
  类型 *类型_缩 = 取_公共的_类型((*左手边)->类型_缩, (*右手边)->类型_缩);
  *左手边 = 新_类型转换(*左手边, 类型_缩);
  *右手边 = 新_类型转换(*右手边, 类型_缩);
}

void 添加_类型_小(节点 *节点_小) {
  if (!节点_小 || 节点_小->类型_缩)
    return;

  添加_类型_小(节点_小->左手边);
  添加_类型_小(节点_小->右手边);
  添加_类型_小(节点_小->条件_短);
  添加_类型_小(节点_小->那么);
  添加_类型_小(节点_小->否则);
  添加_类型_小(节点_小->初始);
  添加_类型_小(节点_小->增_短);

  for (节点 *n = 节点_小->体; n; n = n->下一个)
    添加_类型_小(n);
  for (节点 *n = 节点_小->实参_短; n; n = n->下一个)
    添加_类型_小(n);

  switch (节点_小->种类) {
  case 抽象节点_整数:
    节点_小->类型_缩 = 类型_整型_小;
    return;
  case 抽象节点_加:
  case 抽象节点_减:
  case 抽象节点_乘:
  case 抽象节点_除:
  case 抽象节点_余数:
  case 抽象节点_位与:
  case 抽象节点_位或:
  case 抽象节点_位异或:
    通常的_算术_转换_短(&节点_小->左手边, &节点_小->右手边);
    节点_小->类型_缩 = 节点_小->左手边->类型_缩;
    return;
  case 抽象节点_负: {
    类型 *类型_缩 = 取_公共的_类型(类型_整型_小, 节点_小->左手边->类型_缩);
    节点_小->左手边 = 新_类型转换(节点_小->左手边, 类型_缩);
    节点_小->类型_缩 = 类型_缩;
    return;
  }
  case 抽象节点_赋值:
    if (节点_小->左手边->类型_缩->种类 == 类型_数组)
      错误_牌_短(节点_小->左手边->牌_短, "not an lvalue");
    if (节点_小->左手边->类型_缩->种类 != 类型_结构)
      节点_小->右手边 = 新_类型转换(节点_小->右手边, 节点_小->左手边->类型_缩);
    节点_小->类型_缩 = 节点_小->左手边->类型_缩;
    return;
  case 抽象节点_等于:
  case 抽象节点_不等:
  case 抽象节点_小于:
  case 抽象节点_小等:
    通常的_算术_转换_短(&节点_小->左手边, &节点_小->右手边);
    节点_小->类型_缩 = 类型_整型_小;
    return;
  case 抽象节点_函数调用:
    节点_小->类型_缩 = 节点_小->函数_类型->返回_类型;
    return;
  case 抽象节点_非:
  case 抽象节点_逻辑或:
  case 抽象节点_逻辑与:
    节点_小->类型_缩 = 类型_整型_小;
    return;
  case 抽象节点_位非:
  case 抽象节点_左移:
  case 抽象节点_右移:
    节点_小->类型_缩 = 节点_小->左手边->类型_缩;
    return;
  case 抽象节点_变量:
  case 抽象节点_变长数组_指针:
    节点_小->类型_缩 = 节点_小->变量_短->类型_缩;
    return;
  case 抽象节点_条件:
    if (节点_小->那么->类型_缩->种类 == 类型_空的 || 节点_小->否则->类型_缩->种类 == 类型_空的) {
      节点_小->类型_缩 = 类型_空的_小;
    } else {
      通常的_算术_转换_短(&节点_小->那么, &节点_小->否则);
      节点_小->类型_缩 = 节点_小->那么->类型_缩;
    }
    return;
  case 抽象节点_逗号:
    节点_小->类型_缩 = 节点_小->右手边->类型_缩;
    return;
  case 抽象节点_成员:
    节点_小->类型_缩 = 节点_小->成员_小->类型_缩;
    return;
  case 抽象节点_地址: {
    类型 *类型_缩 = 节点_小->左手边->类型_缩;
    if (类型_缩->种类 == 类型_数组)
      节点_小->类型_缩 = 指针_到(类型_缩->基础);
    else
      节点_小->类型_缩 = 指针_到(类型_缩);
    return;
  }
  case 抽象节点_解引用:
    if (!节点_小->左手边->类型_缩->基础)
      错误_牌_短(节点_小->牌_短, "invalid pointer dereference");
    if (节点_小->左手边->类型_缩->基础->种类 == 类型_空的)
      错误_牌_短(节点_小->牌_短, "dereferencing a void pointer");

    节点_小->类型_缩 = 节点_小->左手边->类型_缩->基础;
    return;
  case 抽象节点_语句_表达式:
    if (节点_小->体) {
      节点 *语句_缩 = 节点_小->体;
      while (语句_缩->下一个)
        语句_缩 = 语句_缩->下一个;
      if (语句_缩->种类 == 抽象节点_表达式_语句) {
        节点_小->类型_缩 = 语句_缩->左手边->类型_缩;
        return;
      }
    }
    错误_牌_短(节点_小->牌_短, "statement expression returning void is not supported");
    return;
  case 抽象节点_标号_值:
    节点_小->类型_缩 = 指针_到(类型_空的_小);
    return;
  case 抽象节点_对比与交换:
    添加_类型_小(节点_小->对比与交换_地址);
    添加_类型_小(节点_小->对比与交换_旧);
    添加_类型_小(节点_小->对比与交换_新);
    节点_小->类型_缩 = 类型_布尔_小;

    if (节点_小->对比与交换_地址->类型_缩->种类 != 类型_指针)
      错误_牌_短(节点_小->对比与交换_地址->牌_短, "pointer expected");
    if (节点_小->对比与交换_旧->类型_缩->种类 != 类型_指针)
      错误_牌_短(节点_小->对比与交换_旧->牌_短, "pointer expected");
    return;
  case 抽象节点_互换:
    if (节点_小->左手边->类型_缩->种类 != 类型_指针)
      错误_牌_短(节点_小->对比与交换_地址->牌_短, "pointer expected");
    节点_小->类型_缩 = 节点_小->左手边->类型_缩->基础;
    return;
  }
}
