#include "chibicc.h"

void 串数组_推(字符串数组 *数组_短, char *s) {
  if (!数组_短->数据_小写) {
    数组_短->数据_小写 = calloc(8, sizeof(char *));
    数组_短->容量 = 8;
  }

  if (数组_短->容量 == 数组_短->长度_短) {
    数组_短->数据_小写 = realloc(数组_短->数据_小写, sizeof(char *) * 数组_短->容量 * 2);
    数组_短->容量 *= 2;
    for (int i = 数组_短->长度_短; i < 数组_短->容量; i++)
      数组_短->数据_小写[i] = NULL;
  }

  数组_短->数据_小写[数组_短->长度_短++] = s;
}

// Takes a printf-style 格式化_函 string and returns a formatted string.
char *格式化_函(char *格式_缩写, ...) {
  char *buf;
  size_t buflen;
  FILE *out = open_memstream(&buf, &buflen);

  va_list ap;
  va_start(ap, 格式_缩写);
  vfprintf(out, 格式_缩写, ap);
  va_end(ap);
  fclose(out);
  return buf;
}
