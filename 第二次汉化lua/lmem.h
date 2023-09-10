/*
** $Id: lmem.h $
** Interface 到_变量 Memory Manager
** See Copyright Notice in lua.h
*/

#ifndef lmem_h
#define lmem_h


#include <stddef.h>

#include "llimits.h"
#include "lua.h"


#define 内存_月亮M_错误_宏名(L)	月亮调度_抛出_函(L, 月头_月亮_内存错误_宏名)


/*
** This macro tests whether it is safe 到_变量 multiply 'n' by the 大小_变量 of
** type 't' without overflows. Because 'e' is always constant, it avoids
** the runtime division 串库_最大_大小T_宏名/(e).
** (The macro is somewhat complex 到_变量 avoid warnings:  The 'sizeof'
** comparison avoids a runtime comparison when overflow 月编译器_不能_函 occur.
** The compiler should be able 到_变量 optimize the real test by itself, but
** when it does it, it may give a warning about "comparison is always
** false due 到_变量 limited range of 数据_变量 type"; the +1 tricks the compiler,
** avoiding this warning but also this optimization.)
*/
#define 内存_月亮M_测试大小_宏名(n,e)  \
	(sizeof(n) >= sizeof(size_t) && 限制_类型转换_大小t_宏名((n)) + 1 > 串库_最大_大小T_宏名/(e))

#define 内存_月亮M_检查大小_宏名(L,n,e)  \
	(内存_月亮M_测试大小_宏名(n,e) ? 月亮内存_太大_函(L) : 限制_类型转换_空的_宏名(0))


/*
** Computes the minimum between 'n' and '串库_最大_大小T_宏名/sizeof(t)', so that
** the 结果_变量 is not larger than 'n' and 月编译器_不能_函 overflow a 'size_t'
** when multiplied by the 大小_变量 of type 't'. (Assumes that 'n' is an
** 'int' or 'unsigned int' and that 'int' is not larger than 'size_t'.)
*/
#define 内存_月亮M_限制N_宏名(n,t)  \
  ((限制_类型转换_大小t_宏名(n) <= 串库_最大_大小T_宏名/sizeof(t)) ? (n) :  \
     限制_类型转换_无符整型_宏名((串库_最大_大小T_宏名/sizeof(t))))


/*
** Arrays of chars do not need any test
*/
#define 内存_月亮M_重新分配v印刻_宏名(L,b,on,n)  \
  限制_类型转换_印刻指针_宏名(月亮内存_安全重新分配_函(L, (b), (on)*sizeof(char), (n)*sizeof(char)))

#define 内存_月亮M_释放内存_宏名(L, b, s)	月亮内存_释放_函(L, (b), (s))
#define 内存_月亮M_释放_宏名(L, b)		月亮内存_释放_函(L, (b), sizeof(*(b)))
#define 内存_月亮M_释放数组_宏名(L, b, n)   月亮内存_释放_函(L, (b), (n)*sizeof(*(b)))

#define 内存_月亮M_新_宏名(L,t)		限制_类型转换_宏名(t*, 月亮内存_内存分配_函(L, sizeof(t), 0))
#define 内存_月亮M_新向量_宏名(L,n,t)	限制_类型转换_宏名(t*, 月亮内存_内存分配_函(L, (n)*sizeof(t), 0))
#define 内存_月亮M_新向量已检查_宏名(L,n,t) \
  (内存_月亮M_检查大小_宏名(L,n,sizeof(t)), 内存_月亮M_新向量_宏名(L,n,t))

#define 内存_月亮M_新对象_宏名(L,tag,s)	月亮内存_内存分配_函(L, (s), tag)

#define 内存_月亮M_增长向量_宏名(L,v,nelems,大小_变量,t,限制_变量,e) \
	((v)=限制_类型转换_宏名(t *, 月亮内存_增长辅助_函(L,v,nelems,&(大小_变量),sizeof(t), \
                         内存_月亮M_限制N_宏名(限制_变量,t),e)))

#define 内存_月亮M_重新分配向量_宏名(L, v,oldn,n,t) \
   (限制_类型转换_宏名(t *, 月亮内存_重新分配_函(L, v, 限制_类型转换_大小t_宏名(oldn) * sizeof(t), \
                                  限制_类型转换_大小t_宏名(n) * sizeof(t))))

#define 内存_月亮M_缩放向量_宏名(L,v,大小_变量,字段静态_变量,t) \
   ((v)=限制_类型转换_宏名(t *, 月亮内存_收缩向量_函(L, v, &(大小_变量), 字段静态_变量, sizeof(t))))

配置_月亮I_函_宏名 限制_l_无返回值_宏名 月亮内存_太大_函 (炉_状态机结 *L);

/* not 到_变量 be called directly */
配置_月亮I_函_宏名 void *月亮内存_重新分配_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 旧大小_变量,
                                                          size_t 大小_变量);
配置_月亮I_函_宏名 void *月亮内存_安全重新分配_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 旧大小_变量,
                                                              size_t 大小_变量);
配置_月亮I_函_宏名 void 月亮内存_释放_函 (炉_状态机结 *L, void *月解析器_块_函, size_t 原始大小_变量);
配置_月亮I_函_宏名 void *月亮内存_增长辅助_函 (炉_状态机结 *L, void *月解析器_块_函, int nelems,
                               int *大小_变量, int size_elem, int 限制_变量,
                               const char *什么_变量);
配置_月亮I_函_宏名 void *月亮内存_收缩向量_函 (炉_状态机结 *L, void *月解析器_块_函, int *nelem,
                                    int final_n, int size_elem);
配置_月亮I_函_宏名 void *月亮内存_内存分配_函 (炉_状态机结 *L, size_t 大小_变量, int tag);

#endif

