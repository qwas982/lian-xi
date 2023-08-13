//han_stdatomic.h

#ifndef __STDATOMIC_H  // 如果__STDATOMIC_H宏未定义，则编译以下内容
#define __STDATOMIC_H  // 定义__STDATOMIC_H宏，防止重复包含

#define ATOMIC_BOOL_LOCK_FREE 1  // 表示原子布尔类型的操作是无锁的
#define ATOMIC_CHAR_LOCK_FREE 1  // 表示原子字符类型的操作是无锁的
#define ATOMIC_CHAR16_T_LOCK_FREE 1  // 表示原子char16_t类型的操作是无锁的
#define ATOMIC_CHAR32_T_LOCK_FREE 1  // 表示原子char32_t类型的操作是无锁的
#define ATOMIC_WCHAR_T_LOCK_FREE 1  // 表示原子wchar_t类型的操作是无锁的
#define ATOMIC_SHORT_LOCK_FREE 1  // 表示原子short类型的操作是无锁的
#define ATOMIC_INT_LOCK_FREE 1  // 表示原子int类型的操作是无锁的
#define ATOMIC_LONG_LOCK_FREE 1  // 表示原子long类型的操作是无锁的
#define ATOMIC_LLONG_LOCK_FREE 1  // 表示原子long long类型的操作是无锁的
#define ATOMIC_POINTER_LOCK_FREE 1  // 表示原子指针类型的操作是无锁的


typedef enum {  // 定义枚举类型memory_order
  memory_order_relaxed,  // 松散的内存顺序
  memory_order_consume,  // 消费的内存顺序
  memory_order_acquire,  // 获取的内存顺序
  memory_order_release,  // 释放的内存顺序
  memory_order_acq_rel,  // 获取-释放的内存顺序
  memory_order_seq_cst,  // 顺序一致的内存顺序
} memory_order;  // 枚举类型的名称为memory_order

#define ATOMIC_FLAG_INIT(x) (x)  // 定义宏ATOMIC_FLAG_INIT，用于初始化原子标志
#define atomic_init(addr, val) (*(addr) = (val))  // 定义宏atomic_init，用于初始化原子对象
#define kill_dependency(x) (x)  // 定义宏kill_dependency，用于消除数据依赖性
#define atomic_thread_fence(order)  // 定义宏atomic_thread_fence，用于插入线程屏障
#define atomic_signal_fence(order)  // 定义宏atomic_signal_fence，用于插入信号屏障
#define atomic_is_lock_free(x) 1  // 定义宏atomic_is_lock_free，用于判断原子对象是否无锁

#define atomic_load(addr) (*(addr))  // 定义宏atomic_load，用于读取原子对象的值
#define atomic_store(addr, val) (*(addr) = (val))  // 定义宏atomic_store，用于存储值到原子对象

#define atomic_load_explicit(addr, order) (*(addr))  // 定义宏atomic_load_explicit，用于以指定的内存顺序读取原子对象的值
#define atomic_store_explicit(addr, val, order) (*(addr) = (val))  // 定义宏atomic_store_explicit，用于以指定的内存顺序存储值到原子对象

#define atomic_fetch_add(obj, val) (*(obj) += (val))  // 定义宏atomic_fetch_add，用于原子地将值增加到对象
#define atomic_fetch_sub(obj, val) (*(obj) -= (val))  // 定义宏atomic_fetch_sub，用于原子地将值从对象减去
#define atomic_fetch_or(obj, val) (*(obj) |= (val))  // 定义宏atomic_fetch_or，用于原子地将值逻辑或到对象
#define atomic_fetch_xor(obj, val) (*(obj) ^= (val))  // 定义宏atomic_fetch_xor，用于原子地将值逻辑异或到对象
#define atomic_fetch_and(obj, val) (*(obj) &= (val))  // 定义宏atomic_fetch_and，用于原子地将值逻辑与到对象

#define atomic_fetch_add_explicit(obj, val, order) (*(obj) += (val))  // 定义宏atomic_fetch_add_explicit，用于以指定的内存顺序原子地将值增加到对象
#define atomic_fetch_sub_explicit(obj, val, order) (*(obj) -= (val))  // 定义宏atomic_fetch_sub_explicit，用于以指定的内存顺序原子地将值从对象减去
#define atomic_fetch_or_explicit(obj, val, order) (*(obj) |= (val))  // 定义宏atomic_fetch_or_explicit，用于以指定的内存顺序原子地将值逻辑或到对象
#define atomic_fetch_xor_explicit(obj, val, order) (*(obj) ^= (val))  // 定义宏atomic_fetch_xor_explicit，用于以指定的内存顺序原子地将值逻辑异或到对象
#define atomic_fetch_and_explicit(obj, val, order) (*(obj) &= (val))  // 定义宏atomic_fetch_and_explicit，用于以指定的内存顺序原子地将值逻辑与到对象

#define atomic_compare_exchange_weak(p, old, new) \  // 定义宏atomic_compare_exchange_weak，使用弱比较和交换操作来原子地比较并交换指针的值
  __builtin_compare_and_swap((p), (old), (new))

#define atomic_compare_exchange_strong(p, old, new) \  // 定义宏atomic_compare_exchange_strong，使用强比较和交换操作来原子地比较并交换指针的值
  __builtin_compare_and_swap((p), (old), (new))

#define atomic_exchange(obj, val) __builtin_atomic_exchange((obj), (val))  // 定义宏atomic_exchange，用于原子地交换对象的值并返回原来的值
#define atomic_exchange_explicit(obj, val, order) __builtin_atomic_exchange((obj), (val))  // 定义宏atomic_exchange_explicit，用于以指定的内存顺序原子地交换对象的值并返回原来的值

#define atomic_flag_test_and_set(obj) atomic_exchange((obj), 1)  // 定义宏atomic_flag_test_and_set，用于原子地设置标志并返回之前的值
#define atomic_flag_test_and_set_explicit(obj, order) atomic_exchange((obj), 1)  // 定义宏atomic_flag_test_and_set_explicit，用于以指定的内存顺序原子地设置标志并返回之前的值
#define atomic_flag_clear(obj) (*(obj) = 0)  // 定义宏atomic_flag_clear，用于清除标志
#define atomic_flag_clear_explicit(obj, order) (*(obj) = 0)  // 定义宏atomic_flag_clear_explicit，用于以指定的内存顺序清除标志

typedef _Atomic _Bool atomic_flag;  // 定义类型别名atomic_flag，表示原子布尔类型
typedef _Atomic _Bool atomic_bool;  // 定义类型别名atomic_bool，表示原子布尔类型
typedef _Atomic char atomic_char;  // 定义类型别名atomic_char，表示原子字符类型
typedef _Atomic signed char atomic_schar;  // 定义类型别名atomic_schar，表示原子有符号字符类型
typedef _Atomic unsigned char atomic_uchar;  // 定义类型别名atomic_uchar，表示原子无符号字符类型
typedef _Atomic short atomic_short;  // 定义类型别名atomic_short，表示原子短整型类型
typedef _Atomic unsigned short atomic_ushort;  // 定义类型别名atomic_ushort，表示原子无符号短整型类型
typedef _Atomic int atomic_int;  // 定义类型别名atomic_int，表示原子整型类型
typedef _Atomic unsigned int atomic_uint;  // 定义类型别名atomic_uint，表示原子无符号整型类型
typedef _Atomic long atomic_long;  // 定义类型别名atomic_long，表示原子长整型类型
typedef _Atomic unsigned long atomic_ulong;  // 定义类型别名atomic_ulong，表示原子无符号长整型类型
typedef _Atomic long long atomic_llong;  // 定义类型别名atomic_llong，表示原子长长整型类型
typedef _Atomic unsigned long long atomic_ullong;  // 定义类型别名atomic_ullong，表示原子无符号长长整型类型
typedef _Atomic unsigned short atomic_char16_t;  // 定义类型别名atomic_char16_t，表示原子无符号16位字符类型
typedef _Atomic unsigned atomic_char32_t;  // 定义类型别名atomic_char32_t，表示原子无符号32位字符类型
typedef _Atomic unsigned atomic_wchar_t;  // 定义类型别名atomic_wchar_t，表示原子宽字符类型
typedef _Atomic signed char atomic_int_least8_t;  // 定义类型别名atomic_int_least8_t，表示原子最少8位有符号整型类型
typedef _Atomic unsigned char atomic_uint_least8_t;  // 定义类型别名atomic_uint_least8_t，表示原子最少8位无符号整型类型
typedef _Atomic short atomic_int_least16_t;  // 定义类型别名atomic_int_least16_t，表示原子最少16位有符号整型类型
typedef _Atomic unsigned short atomic_uint_least16_t;  // 定义类型别名atomic_uint_least16_t，表示原子最少16位无符号整型类型
typedef _Atomic int atomic_int_least32_t;  // 定义类型别名atomic_int_least32_t，表示原子最少32位有符号整型类型
typedef _Atomic unsigned int atomic_uint_least32_t;  // 定义类型别名atomic_uint_least32_t，表示原子最少32位无符号整型类型
typedef _Atomic long atomic_int_least64_t;  // 定义类型别名atomic_int_least64_t，表示原子最少64位有符号整型类型

typedef _Atomic unsigned long atomic_uint_least64_t;  // 定义类型别名atomic_uint_least64_t，表示原子最少64位无符号整型类型
typedef _Atomic signed char atomic_int_fast8_t;  // 定义类型别名atomic_int_fast8_t，表示原子最快的8位有符号整型类型
typedef _Atomic unsigned char atomic_uint_fast8_t;  // 定义类型别名atomic_uint_fast8_t，表示原子最快的8位无符号整型类型
typedef _Atomic short atomic_int_fast16_t;  // 定义类型别名atomic_int_fast16_t，表示原子最快的16位有符号整型类型
typedef _Atomic unsigned short atomic_uint_fast16_t;  // 定义类型别名atomic_uint_fast16_t，表示原子最快的16位无符号整型类型
typedef _Atomic int atomic_int_fast32_t;  // 定义类型别名atomic_int_fast32_t，表示原子最快的32位有符号整型类型
typedef _Atomic unsigned int atomic_uint_fast32_t;  // 定义类型别名atomic_uint_fast32_t，表示原子最快的32位无符号整型类型
typedef _Atomic long atomic_int_fast64_t;  // 定义类型别名atomic_int_fast64_t，表示原子最快的64位有符号整型类型
typedef _Atomic unsigned long atomic_uint_fast64_t;  // 定义类型别名atomic_uint_fast64_t，表示原子最快的64位无符号整型类型
typedef _Atomic long atomic_intptr_t;  // 定义类型别名atomic_intptr_t，表示原子指针大小的有符号整型类型
typedef _Atomic unsigned long atomic_uintptr_t;  // 定义类型别名atomic_uintptr_t，表示原子指针大小的无符号整型类型
typedef _Atomic unsigned long atomic_size_t;  // 定义类型别名atomic_size_t，表示原子size_t类型
typedef _Atomic long atomic_ptrdiff_t;  // 定义类型别名atomic_ptrdiff_t，表示原子ptrdiff_t类型
typedef _Atomic long atomic_intmax_t;  // 定义类型别名atomic_intmax_t，表示原子intmax_t类型
typedef _Atomic unsigned long atomic_uintmax_t;  // 定义类型别名atomic_uintmax_t，表示原子uintmax_t类型

#endif  // 结束条件编译