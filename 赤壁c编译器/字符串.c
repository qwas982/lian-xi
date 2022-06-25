#include "chibicc.h"

void 串数组_推(字符串数组 *数组_短, char *s) {
  if (!数组_短->数据) {
    数组_短->数据 = calloc(8, sizeof(char *));
    数组_短->容量 = 8;
  }

  if (数组_短->容量 == 数组_短->长度_短) {
    数组_短->数据 = realloc(数组_短->数据, sizeof(char *) * 数组_短->容量 * 2);
    数组_短->容量 *= 2;
    for (int i = 数组_短->长度_短; i < 数组_短->容量; i++)
      数组_短->数据[i] = NULL;
  }

  数组_短->数据[数组_短->长度_短++] = s;
}

// Takes a printf-style 格式化 string and returns a formatted string.
char *格式化(char *fmt, ...) {
  char *缓冲_短;
  size_t 缓冲长度;
  FILE *out = open_memstream(&缓冲_短, &缓冲长度);

  va_list ap;
  va_start(ap, fmt);
  vfprintf(out, fmt, ap);
  va_end(ap);
  fclose(out);
  return 缓冲_短;
}
