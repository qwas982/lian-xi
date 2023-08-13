//han_type.c

#include "han_chibicc.h"

Type *ty_void = &(Type){TY_VOID, 1, 1};               // 定义指向 Type 结构体的指针 ty_void，指向 TY_VOID 类型的对象，大小为 1 字节，对齐为 1 字节
Type *ty_bool = &(Type){TY_BOOL, 1, 1};               // 定义指向 Type 结构体的指针 ty_bool，指向 TY_BOOL 类型的对象，大小为 1 字节，对齐为 1 字节

Type *ty_char = &(Type){TY_CHAR, 1, 1};               // 定义指向 Type 结构体的指针 ty_char，指向 TY_CHAR 类型的对象，大小为 1 字节，对齐为 1 字节
Type *ty_short = &(Type){TY_SHORT, 2, 2};             // 定义指向 Type 结构体的指针 ty_short，指向 TY_SHORT 类型的对象，大小为 2 字节，对齐为 2 字节
Type *ty_int = &(Type){TY_INT, 4, 4};                 // 定义指向 Type 结构体的指针 ty_int，指向 TY_INT 类型的对象，大小为 4 字节，对齐为 4 字节
Type *ty_long = &(Type){TY_LONG, 8, 8};               // 定义指向 Type 结构体的指针 ty_long，指向 TY_LONG 类型的对象，大小为 8 字节，对齐为 8 字节

Type *ty_uchar = &(Type){TY_CHAR, 1, 1, true};        // 定义指向 Type 结构体的指针 ty_uchar，指向 TY_CHAR 类型的对象，大小为 1 字节，对齐为 1 字节，无符号类型
Type *ty_ushort = &(Type){TY_SHORT, 2, 2, true};      // 定义指向 Type 结构体的指针 ty_ushort，指向 TY_SHORT 类型的对象，大小为 2 字节，对齐为 2 字节，无符号类型
Type *ty_uint = &(Type){TY_INT, 4, 4, true};          // 定义指向 Type 结构体的指针 ty_uint，指向 TY_INT 类型的对象，大小为 4 字节，对齐为 4 字节，无符号类型
Type *ty_ulong = &(Type){TY_LONG, 8, 8, true};        // 定义指向 Type 结构体的指针 ty_ulong，指向 TY_LONG 类型的对象，大小为 8 字节，对齐为 8 字节，无符号类型

Type *ty_float = &(Type){TY_FLOAT, 4, 4};             // 定义指向 Type 结构体的指针 ty_float，指向 TY_FLOAT 类型的对象，大小为 4 字节，对齐为 4 字节
Type *ty_double = &(Type){TY_DOUBLE, 8, 8};           // 定义指向 Type 结构体的指针 ty_double，指向 TY_DOUBLE 类型的对象，大小为 8 字节，对齐为 8 字节
Type *ty_ldouble = &(Type){TY_LDOUBLE, 16, 16};       // 定义指向 Type 结构体的指针 ty_ldouble，指向 TY_LDOUBLE 类型的对象，大小为 16 字节，对齐为 16 字节

static Type *new_type(TypeKind kind, int size, int align) {  // 定义静态函数 new_type，返回 Type 类型的指针，接受 kind、size 和 align 作为参数
  Type *ty = calloc(1, sizeof(Type));                       // 使用 calloc 在内存中分配 Type 的空间，并将其初始化为 0
  ty->kind = kind;                                          // 将 kind 赋值给 ty 结构体的成员变量
  ty->size = size;                                          // 将 size 赋值给 ty 结构体的成员变量
  ty->align = align;                                        // 将 align 赋值给 ty 结构体的成员变量
  return ty;                                                // 返回指向新创建的 Type 对象的指针
}

bool is_integer(Type *ty) {  // 判断类型 ty 是否为整数类型
  TypeKind k = ty->kind;     // 获取类型 ty 的种类
  return k == TY_BOOL || k == TY_CHAR || k == TY_SHORT ||  // 返回类型种类是否为 TY_BOOL、TY_CHAR、TY_SHORT、
         k == TY_INT  || k == TY_LONG || k == TY_ENUM;     // TY_INT、TY_LONG 或 TY_ENUM
}

bool is_flonum(Type *ty) {  // 判断类型 ty 是否为浮点数类型
  return ty->kind == TY_FLOAT || ty->kind == TY_DOUBLE ||  // 返回类型种类是否为 TY_FLOAT、TY_DOUBLE 或 TY_LDOUBLE
         ty->kind == TY_LDOUBLE;
}

bool is_numeric(Type *ty) {  // 判断类型 ty 是否为数值类型（整数或浮点数）
  return is_integer(ty) || is_flonum(ty);  // 返回类型是否为整数类型或浮点数类型
}

bool is_compatible(Type *t1, Type *t2) {  // 判断类型 t1 和 t2 是否兼容
  if (t1 == t2)
    return true;  // 如果 t1 和 t2 是同一个类型，返回 true

  if (t1->origin)
    return is_compatible(t1->origin, t2);  // 如果 t1 存在 origin，则递归判断 origin 和 t2 是否兼容

  if (t2->origin)
    return is_compatible(t1, t2->origin);  // 如果 t2 存在 origin，则递归判断 t1 和 origin 是否兼容

  if (t1->kind != t2->kind)
    return false;  // 如果 t1 和 t2 的种类不同，返回 false

  switch (t1->kind) {  // 根据 t1 的类型种类进行判断
  case TY_CHAR:     // 如果 t1 的类型种类为 TY_CHAR
  case TY_SHORT:    // 或者为 TY_SHORT
  case TY_INT:      // 或者为 TY_INT
  case TY_LONG:     // 或者为 TY_LONG
    return t1->is_unsigned == t2->is_unsigned;  // 返回 t1 和 t2 是否都为无符号类型
  case TY_FLOAT:    // 如果 t1 的类型种类为 TY_FLOAT
  case TY_DOUBLE:   // 或者为 TY_DOUBLE
  case TY_LDOUBLE:  // 或者为 TY_LDOUBLE
    return true;    // 返回 true
  case TY_PTR:      // 如果 t1 的类型种类为 TY_PTR
    return is_compatible(t1->base, t2->base);  // 返回 t1 和 t2 的指针基类型是否兼容
  case TY_FUNC: {   // 如果 t1 的类型种类为 TY_FUNC
    if (!is_compatible(t1->return_ty, t2->return_ty))
      return false;  // 如果 t1 和 t2 的返回类型不兼容，返回 false
    if (t1->is_variadic != t2->is_variadic)
      return false;  // 如果 t1 和 t2 的可变参数性质不相同，返回 false

    Type *p1 = t1->params;  // 分别指向 t1 和 t2 的参数列表
    Type *p2 = t2->params;
    for (; p1 && p2; p1 = p1->next, p2 = p2->next)
      if (!is_compatible(p1, p2))
        return false;  // 如果参数列表中的对应参数不兼容，返回 false
    return p1 == NULL && p2 == NULL;  // 如果两个参数列表都遍历完了，则返回 true，否则返回 false
  }
  case TY_ARRAY:  // 如果 t1 的类型种类为 TY_ARRAY
    if (!is_compatible(t1->base, t2->base))
      return false;  // 如果 t1 和 t2 的数组基类型不兼容，返回 false
    return t1->array_len < 0 && t2->array_len < 0 &&  // 如果 t1 和 t2 的数组长度都小于 0
           t1->array_len == t2->array_len;           // 并且数组长度相同，返回 true
  }
  return false;  // 默认情况下，返回 false
}

Type *copy_type(Type *ty) {  // 复制类型 ty 并返回新的类型指针
  Type *ret = calloc(1, sizeof(Type));  // 分配内存空间用于存储新的类型
  *ret = *ty;  // 将 ty 的内容复制到 ret
  ret->origin = ty;  // 设置 ret 的 origin 为 ty
  return ret;  // 返回新的类型指针 ret
}

Type *pointer_to(Type *base) {  // 创建指向基类型 base 的指针类型并返回类型指针
  Type *ty = new_type(TY_PTR, 8, 8);  // 创建一个类型种类为 TY_PTR 的类型指针 ty
  ty->base = base;  // 设置 ty 的基类型为 base
  ty->is_unsigned = true;  // 设置 ty 的无符号属性为 true
  return ty;  // 返回类型指针 ty
}

Type *func_type(Type *return_ty) {  // 创建函数类型并返回类型指针
  // C 规范禁止使用 sizeof(<function type>)，但是 GCC 允许，并且该表达式的结果为 1。
  Type *ty = new_type(TY_FUNC, 1, 1);  // 创建一个类型种类为 TY_FUNC 的类型指针 ty，大小为 1 字节
  ty->return_ty = return_ty;  // 设置 ty 的返回类型为 return_ty
  return ty;  // 返回类型指针 ty
}

Type *array_of(Type *base, int len) {  // 创建一个基类型为 base，长度为 len 的数组类型并返回类型指针
  Type *ty = new_type(TY_ARRAY, base->size * len, base->align);  // 创建一个类型种类为 TY_ARRAY 的类型指针 ty，大小为 base 的大小乘以 len，对齐方式与 base 相同
  ty->base = base;  // 设置 ty 的基类型为 base
  ty->array_len = len;  // 设置 ty 的数组长度为 len
  return ty;  // 返回类型指针 ty
}

Type *vla_of(Type *base, Node *len) {  // 创建一个基类型为 base，长度由 len 决定的可变长度数组类型并返回类型指针
  Type *ty = new_type(TY_VLA, 8, 8);  // 创建一个类型种类为 TY_VLA 的类型指针 ty，大小为 8 字节
  ty->base = base;  // 设置 ty 的基类型为 base
  ty->vla_len = len;  // 设置 ty 的可变长度数组长度为 len
  return ty;  // 返回类型指针 ty
}

Type *enum_type(void) {  // 创建一个枚举类型并返回类型指针
  return new_type(TY_ENUM, 4, 4);  // 创建一个类型种类为 TY_ENUM 的类型指针，大小为 4 字节
}

Type *struct_type(void) {  // 创建一个结构体类型并返回类型指针
  return new_type(TY_STRUCT, 0, 1);  // 创建一个类型种类为 TY_STRUCT 的类型指针，大小为 0 字节，对齐方式为 1
}

static Type *get_common_type(Type *ty1, Type *ty2) {  // 获取两个类型 ty1 和 ty2 的公共类型
  if (ty1->base)
    return pointer_to(ty1->base);  // 如果 ty1 是指针类型，则返回指向 ty1 基类型的指针类型

  if (ty1->kind == TY_FUNC)
    return pointer_to(ty1);  // 如果 ty1 是函数类型，则返回指向 ty1 的指针类型
  if (ty2->kind == TY_FUNC)
    return pointer_to(ty2);  // 如果 ty2 是函数类型，则返回指向 ty2 的指针类型

  if (ty1->kind == TY_LDOUBLE || ty2->kind == TY_LDOUBLE)
    return ty_ldouble;  // 如果 ty1 或 ty2 是长双类型（TY_LDOUBLE），则返回 ty_ldouble 类型
  if (ty1->kind == TY_DOUBLE || ty2->kind == TY_DOUBLE)
    return ty_double;  // 如果 ty1 或 ty2 是双精度类型（TY_DOUBLE），则返回 ty_double 类型
  if (ty1->kind == TY_FLOAT || ty2->kind == TY_FLOAT)
    return ty_float;  // 如果 ty1 或 ty2 是单精度类型（TY_FLOAT），则返回 ty_float 类型

  if (ty1->size < 4)
    ty1 = ty_int;  // 如果 ty1 的大小小于 4，则将 ty1 设置为 ty_int
  if (ty2->size < 4)
    ty2 = ty_int;  // 如果 ty2 的大小小于 4，则将 ty2 设置为 ty_int

  if (ty1->size != ty2->size)
    return (ty1->size < ty2->size) ? ty2 : ty1;  // 如果 ty1 的大小与 ty2 的大小不相等，则返回大小较大的类型

  if (ty2->is_unsigned)
    return ty2;  // 如果 ty2 是无符号类型，则返回 ty2
  return ty1;  // 否则返回 ty1
}

// 对于许多二元运算符，我们会隐式地提升操作数，使得两个操作数具有相同的类型。
// 任何小于 int 的整数类型都会被提升为 int。
//如果一个操作数的类型比另一个操作数的类型更大（例如 "long" 和 "int"），较小的操作数将被提升以与另一个操作数匹配。
//
// 这个操作被称为 "通常的算术转换"。
static void usual_arith_conv(Node **lhs, Node **rhs) {  // 执行通常的算术转换，将左操作数和右操作数转换为相同的类型
  Type *ty = get_common_type((*lhs)->ty, (*rhs)->ty);  // 获取左操作数和右操作数的公共类型
  *lhs = new_cast(*lhs, ty);  // 将左操作数转换为公共类型
  *rhs = new_cast(*rhs, ty);  // 将右操作数转换为公共类型
}

void add_type(Node *node) {  // 添加节点的类型信息
  if (!node || node->ty)
    return;  // 如果节点为空或已有类型信息，则返回

  add_type(node->lhs);  // 添加左子节点的类型信息
  add_type(node->rhs);  // 添加右子节点的类型信息
  add_type(node->cond);  // 添加条件表达式的类型信息
  add_type(node->then);  // 添加then子节点的类型信息
  add_type(node->els);  // 添加else子节点的类型信息
  add_type(node->init);  // 添加初始化表达式的类型信息
  add_type(node->inc);  // 添加递增表达式的类型信息

  for (Node *n = node->body; n; n = n->next)
    add_type(n);  // 添加循环体内节点的类型信息
  for (Node *n = node->args; n; n = n->next)
    add_type(n);  // 添加函数参数节点的类型信息

  switch (node->kind) {  // 根据节点的类型进行处理
  case ND_NUM:
    node->ty = ty_int;  // 数字节点的类型为整型
    return;
  case ND_ADD:
  case ND_SUB:
  case ND_MUL:
  case ND_DIV:
  case ND_MOD:
  case ND_BITAND:
  case ND_BITOR:
  case ND_BITXOR:
    usual_arith_conv(&node->lhs, &node->rhs);  // 执行通常的算术转换
    node->ty = node->lhs->ty;  // 结果节点的类型与左操作数节点的类型相同
    return;
  case ND_NEG: {
    Type *ty = get_common_type(ty_int, node->lhs->ty);  // 获取通常的算术转换后的类型
    node->lhs = new_cast(node->lhs, ty);  // 将左操作数节点转换为通常的算术转换后的类型
    node->ty = ty;  // 结果节点的类型为通常的算术转换后的类型
    return;
  }
  case ND_ASSIGN:  // 赋值运算符
	  if (node->lhs->ty->kind == TY_ARRAY)
	    error_tok(node->lhs->tok, "not an lvalue");  // 左侧不是可赋值的值
	  if (node->lhs->ty->kind != TY_STRUCT)
	    node->rhs = new_cast(node->rhs, node->lhs->ty);  // 进行类型转换
	  node->ty = node->lhs->ty;  // 结果节点的类型与左侧节点的类型相同
	  return;
  case ND_EQ:
  case ND_NE:
  case ND_LT:
  case ND_LE:
    usual_arith_conv(&node->lhs, &node->rhs);  // 执行通常的算术转换
    node->ty = ty_int;  // 结果节点的类型为整型
    return;
  case ND_FUNCALL:  // 函数调用
    node->ty = node->func_ty->return_ty;  // 结果节点的类型为函数返回值的类型
    return;
  case ND_NOT:
  case ND_LOGOR:
  case ND_LOGAND:
    node->ty = ty_int;  // 结果节点的类型为整型
    return;
  case ND_BITNOT:
  case ND_SHL:
  case ND_SHR:
    node->ty = node->lhs->ty;  // 结果节点的类型与左侧节点的类型相同
    return;
  case ND_VAR:
  case ND_VLA_PTR:
    node->ty = node->var->ty;  // 结果节点的类型为变量的类型
    return;
	case ND_COND:  // 条件表达式
	  if (node->then->ty->kind == TY_VOID || node->els->ty->kind == TY_VOID) {
	    node->ty = ty_void;  // 如果then或else分支的类型为void，则结果节点的类型为void
	  } else {
	    usual_arith_conv(&node->then, &node->els);  // 执行通常的算术转换
	    node->ty = node->then->ty;  // 结果节点的类型与then分支节点的类型相同
	  }
	  return;
	case ND_COMMA:  // 逗号运算符
	  node->ty = node->rhs->ty;  // 结果节点的类型为右侧节点的类型
	  return;
	case ND_MEMBER:  // 结构体成员访问
	  node->ty = node->member->ty;  // 结果节点的类型为成员变量的类型
	  return;
	case ND_ADDR: {
	  Type *ty = node->lhs->ty;  // 获取左侧节点的类型
	  if (ty->kind == TY_ARRAY)
	    node->ty = pointer_to(ty->base);  // 如果左侧节点的类型为数组，则结果节点的类型为指向数组元素的指针类型
	  else
	    node->ty = pointer_to(ty);  // 否则结果节点的类型为指向左侧节点类型的指针类型
	  return;
	}
	case ND_DEREF:  // 解引用操作符
	  if (!node->lhs->ty->base)
	    error_tok(node->tok, "invalid pointer dereference");  // 无效的指针解引用
	  if (node->lhs->ty->base->kind == TY_VOID)
	    error_tok(node->tok, "dereferencing a void pointer");  // 解引用一个void指针

	  node->ty = node->lhs->ty->base;  // 结果节点的类型为左侧节点的基础类型
	  return;
	case ND_STMT_EXPR:  // 语句表达式
	  if (node->body) {
	    Node *stmt = node->body;
	    while (stmt->next)
	      stmt = stmt->next;
	    if (stmt->kind == ND_EXPR_STMT) {
	      node->ty = stmt->lhs->ty;  // 结果节点的类型为最后一个表达式语句节点的类型
	      return;
	    }
	  }
	  error_tok(node->tok, "statement expression returning void is not supported");  // 不支持返回void的语句表达式
	  return;
	case ND_LABEL_VAL:  // 标签值
	  node->ty = pointer_to(ty_void);  // 结果节点的类型为指向void类型的指针类型
	  return;
	case ND_CAS:  // 比较并交换
	  add_type(node->cas_addr);  // 添加地址节点的类型信息
	  add_type(node->cas_old);  // 添加旧值节点的类型信息
	  add_type(node->cas_new);  // 添加新值节点的类型信息
	  node->ty = ty_bool;  // 结果节点的类型为布尔类型

	  if (node->cas_addr->ty->kind != TY_PTR)
	    error_tok(node->cas_addr->tok, "pointer expected");  // 期望指针类型的地址节点
	  if (node->cas_old->ty->kind != TY_PTR)
	    error_tok(node->cas_old->tok, "pointer expected");  // 期望指针类型的旧值节点
	  return;
	case ND_EXCH:  // 交换操作
	  if (node->lhs->ty->kind != TY_PTR)
	    error_tok(node->cas_addr->tok, "pointer expected");  // 期望指针类型的左侧节点
	  node->ty = node->lhs->ty->base;  // 结果节点的类型为左侧节点的基础类型
	  return;
	}
}	  	    