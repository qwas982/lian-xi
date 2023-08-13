#include "han_chibicc.h"

void strarray_push(StringArray *arr, char *s) {
  if (!arr->data) {  // 如果数组为空
    arr->data = calloc(8, sizeof(char *));  // 分配初始容量为8的内存空间
    arr->capacity = 8;  // 设置容量为8
  }

  if (arr->capacity == arr->len) {  // 如果容量等于长度
    arr->data = realloc(arr->data, sizeof(char *) * arr->capacity * 2);  // 重新分配内存空间，容量扩大两倍
    arr->capacity *= 2;  // 更新容量为扩大后的值
    for (int i = arr->len; i < arr->capacity; i++)  // 将新增的位置设置为NULL
      arr->data[i] = NULL;
  }

  arr->data[arr->len++] = s;  // 将字符串添加到数组末尾，并更新长度
}

// 定义一个函数，接受一个 printf 样式的格式字符串，并返回一个格式化后的字符串。
char *format(char *fmt, ...) {
  char *buf; // 用于存储格式化后的字符串
  size_t buflen; // 存储字符串的长度
  FILE *out = open_memstream(&buf, &buflen); // 打开一个内存流，将输出重定向到 buf 中

  va_list ap; // 定义一个 va_list 类型的变量，用于存储可变参数的列表
  va_start(ap, fmt); // 初始化 va_list 变量，将其指向可变参数列表的起始位置
  vfprintf(out, fmt, ap); // 将格式化后的字符串写入内存流中
  va_end(ap); // 结束可变参数的获取
  fclose(out); // 关闭内存流
  return buf; // 返回格式化后的字符串
}
