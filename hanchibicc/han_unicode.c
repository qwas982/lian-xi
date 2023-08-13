//han_unicode.c

#include "han_chibicc.h"

// 将给定字符编码为UTF-8。
int encode_utf8(char *buf, uint32_t c) {
  if (c <= 0x7F) {  // 如果字符在0x00至0x7F范围内（单字节表示）
    buf[0] = c;  // 将字符直接存储在buf的第一个位置
    return 1;  // 返回编码后的字节数（1字节）
  }

  if (c <= 0x7FF) {  // 如果字符在0x80至0x7FF范围内（双字节表示）
    buf[0] = 0b11000000 | (c >> 6);  // 第一个字节的前两位为0b110，后五位为字符c的前5位
    buf[1] = 0b10000000 | (c & 0b00111111);  // 第二个字节的前两位为0b10，后六位为字符c的后6位
    return 2;  // 返回编码后的字节数（2字节）
  }

  if (c <= 0xFFFF) {  // 如果字符在0x800至0xFFFF范围内（三字节表示）
    buf[0] = 0b11100000 | (c >> 12);  // 第一个字节的前三位为0b1110，后四位为字符c的前4位
    buf[1] = 0b10000000 | ((c >> 6) & 0b00111111);  // 第二个字节的前两位为0b10，后六位为字符c的中间6位
    buf[2] = 0b10000000 | (c & 0b00111111);  // 第三个字节的前两位为0b10，后六位为字符c的后6位
    return 3;  // 返回编码后的字节数（3字节）
  }

  buf[0] = 0b11110000 | (c >> 18);  // 第一个字节的前四位为0b11110，后三位为字符c的前3位
  buf[1] = 0b10000000 | ((c >> 12) & 0b00111111);  // 第二个字节的前两位为0b10，后六位为字符c的中间6位
  buf[2] = 0b10000000 | ((c >> 6) & 0b00111111);  // 第三个字节的前两位为0b10，后六位为字符c的中间6位
  buf[3] = 0b10000000 | (c & 0b00111111);  // 第四个字节的前两位为0b10，后六位为字符c的后6位
  return 4;  // 返回编码后的字节数（4字节）
}

// 从源文件中读取一个UTF-8编码的Unicode码点。
// 我们假设源文件始终采用UTF-8编码。
//
// UTF-8是一种可变宽度编码，其中一个码点用1至4个字节编码。
// 单字节UTF-8码点与ASCII相同。非ASCII字符使用多个字节进行编码。
uint32_t decode_utf8(char **new_pos, char *p) {
  if ((unsigned char)*p < 128) {  // 如果字符的最高位为0，表示单字节编码
    *new_pos = p + 1;  // 更新new_pos指针，指向下一个字符的位置
    return *p;  // 返回当前字符的Unicode码点
  }

  char *start = p;  // 记录起始位置，用于错误处理
  int len;  // 记录UTF-8编码的字节数
  uint32_t c;  // 用于存储解码后的Unicode码点

  if ((unsigned char)*p >= 0b11110000) {  // 如果字符的最高4位为0b1111，表示4字节编码
    len = 4;  // 字节数为4
    c = *p & 0b111;  // 取出第一个字节的低3位作为初始码点值
  } else if ((unsigned char)*p >= 0b11100000) {  // 如果字符的最高3位为0b111，表示3字节编码
    len = 3;  // 字节数为3
    c = *p & 0b1111;  // 取出第一个字节的低4位作为初始码点值
  } else if ((unsigned char)*p >= 0b11000000) {  // 如果字符的最高2位为0b11，表示2字节编码
    len = 2;  // 字节数为2
    c = *p & 0b11111;  // 取出第一个字节的低5位作为初始码点值
  } else {  // 否则，表示编码错误
    error_at(start, "invalid UTF-8 sequence");  // 报告无效的UTF-8序列错误
  }

  for (int i = 1; i < len; i++) {  // 遍历后续字节
    if ((unsigned char)p[i] >> 6 != 0b10)  // 检查字节的最高2位是否为0b10，若不是则表示编码错误
      error_at(start, "invalid UTF-8 sequence");  // 报告无效的UTF-8序列错误
    c = (c << 6) | (p[i] & 0b111111);  // 将后续字节的低6位添加到码点c的低6位
  }

  *new_pos = p + len;  // 更新new_pos指针，指向下一个字符的位置
  return c;  // 返回解码后的Unicode码点
}

static bool in_range(uint32_t *range, uint32_t c) {
  for (int i = 0; range[i] != -1; i += 2)
    if (range[i] <= c && c <= range[i + 1])
      return true;  // 如果码点c在指定的范围内，则返回true
  return false;  // 如果码点c不在指定的范围内，则返回false
}

// [https://www.sigbus.info/n1570#D] C11允许在标识符中使用不仅仅是ASCII字符，
//还可以使用一些位于特定Unicode范围内的多字节字符。

// 这个函数返回true，如果给定的字符可以作为标识符的第一个字符。

// 例如，¾（U+00BE）是一个有效的标识符，
//因为在0x00BE-0x00C0范围内的字符是允许的，而⟘（U+27D8）和'　'（U+3000，全角空格）是不允许的，因为它们超出了范围。
bool is_ident1(uint32_t c) {
  static uint32_t range[] = {
    '_', '_', 'a', 'z', 'A', 'Z', '$', '$',  // 允许的字符：下划线、小写字母、大写字母、美元符号
    0x00A8, 0x00A8, 0x00AA, 0x00AA, 0x00AD, 0x00AD, 0x00AF, 0x00AF,  // 允许的Unicode码点范围
    0x00B2, 0x00B5, 0x00B7, 0x00BA, 0x00BC, 0x00BE, 0x00C0, 0x00D6,
    0x00D8, 0x00F6, 0x00F8, 0x00FF, 0x0100, 0x02FF, 0x0370, 0x167F,
    0x1681, 0x180D, 0x180F, 0x1DBF, 0x1E00, 0x1FFF, 0x200B, 0x200D,
    0x202A, 0x202E, 0x203F, 0x2040, 0x2054, 0x2054, 0x2060, 0x206F,
    0x2070, 0x20CF, 0x2100, 0x218F, 0x2460, 0x24FF, 0x2776, 0x2793,
    0x2C00, 0x2DFF, 0x2E80, 0x2FFF, 0x3004, 0x3007, 0x3021, 0x302F,
    0x3031, 0x303F, 0x3040, 0xD7FF, 0xF900, 0xFD3D, 0xFD40, 0xFDCF,
    0xFDF0, 0xFE1F, 0xFE30, 0xFE44, 0xFE47, 0xFFFD,
    0x10000, 0x1FFFD, 0x20000, 0x2FFFD, 0x30000, 0x3FFFD, 0x40000, 0x4FFFD,
    0x50000, 0x5FFFD, 0x60000, 0x6FFFD, 0x70000, 0x7FFFD, 0x80000, 0x8FFFD,
    0x90000, 0x9FFFD, 0xA0000, 0xAFFFD, 0xB0000, 0xBFFFD, 0xC0000, 0xCFFFD,
    0xD0000, 0xDFFFD, 0xE0000, 0xEFFFD, -1,  // 结束标志
  };

  return in_range(range, c);  // 调用in_range函数判断字符c是否在允许的范围内
}

// 如果给定的字符可以作为标识符的非首字符，则返回true。
bool is_ident2(uint32_t c) {
  static uint32_t range[] = {
    '0', '9', '$', '$', 0x0300, 0x036F, 0x1DC0, 0x1DFF, 0x20D0, 0x20FF,  // 允许的字符范围：数字、美元符号、Unicode范围
    0xFE20, 0xFE2F, -1,  // 结束标志
  };

  return is_ident1(c) || in_range(range, c);  // 调用is_ident1函数判断字符c是否为标识符的第一个字符，或者调用in_range函数判断字符c是否在允许的范围内
}

// 返回在固定宽度字体中显示给定字符所需的列数。
//
// 基于 https://www.cl.cam.ac.uk/~mgk25/ucs/wcwidth.c
static int char_width(uint32_t c) {     // 定义一个静态整型函数char_width，参数为一个无符号32位整数c
  static uint32_t range1[] = {     // 声明一个静态的无符号32位整数数组range1
    0x0000, 0x001F, 0x007f, 0x00a0, 0x0300, 0x036F, 0x0483, 0x0486,    // 数组range1的元素包含一系列十六进制数值，用逗号分隔
    0x0488, 0x0489, 0x0591, 0x05BD, 0x05BF, 0x05BF, 0x05C1, 0x05C2,
    0x05C4, 0x05C5, 0x05C7, 0x05C7, 0x0600, 0x0603, 0x0610, 0x0615,
    0x064B, 0x065E, 0x0670, 0x0670, 0x06D6, 0x06E4, 0x06E7, 0x06E8,
    0x06EA, 0x06ED, 0x070F, 0x070F, 0x0711, 0x0711, 0x0730, 0x074A,
    0x07A6, 0x07B0, 0x07EB, 0x07F3, 0x0901, 0x0902, 0x093C, 0x093C,
    0x0941, 0x0948, 0x094D, 0x094D, 0x0951, 0x0954, 0x0962, 0x0963,
    0x0981, 0x0981, 0x09BC, 0x09BC, 0x09C1, 0x09C4, 0x09CD, 0x09CD,
    0x09E2, 0x09E3, 0x0A01, 0x0A02, 0x0A3C, 0x0A3C, 0x0A41, 0x0A42,
    0x0A47, 0x0A48, 0x0A4B, 0x0A4D, 0x0A70, 0x0A71, 0x0A81, 0x0A82,
    0x0ABC, 0x0ABC, 0x0AC1, 0x0AC5, 0x0AC7, 0x0AC8, 0x0ACD, 0x0ACD,
    0x0AE2, 0x0AE3, 0x0B01, 0x0B01, 0x0B3C, 0x0B3C, 0x0B3F, 0x0B3F,
    0x0B41, 0x0B43, 0x0B4D, 0x0B4D, 0x0B56, 0x0B56, 0x0B82, 0x0B82,
    0x0BC0, 0x0BC0, 0x0BCD, 0x0BCD, 0x0C3E, 0x0C40, 0x0C46, 0x0C48,
    0x0C4A, 0x0C4D, 0x0C55, 0x0C56, 0x0CBC, 0x0CBC, 0x0CBF, 0x0CBF,
    0x0CC6, 0x0CC6, 0x0CCC, 0x0CCD, 0x0CE2, 0x0CE3, 0x0D41, 0x0D43,
    0x0D4D, 0x0D4D, 0x0DCA, 0x0DCA, 0x0DD2, 0x0DD4, 0x0DD6, 0x0DD6,
    0x0E31, 0x0E31, 0x0E34, 0x0E3A, 0x0E47, 0x0E4E, 0x0EB1, 0x0EB1,
    0x0EB4, 0x0EB9, 0x0EBB, 0x0EBC, 0x0EC8, 0x0ECD, 0x0F18, 0x0F19,
    0x0F35, 0x0F35, 0x0F37, 0x0F37, 0x0F39, 0x0F39, 0x0F71, 0x0F7E,
    0x0F80, 0x0F84, 0x0F86, 0x0F87, 0x0F90, 0x0F97, 0x0F99, 0x0FBC,
    0x0FC6, 0x0FC6, 0x102D, 0x1030, 0x1032, 0x1032, 0x1036, 0x1037,
    0x1039, 0x1039, 0x1058, 0x1059, 0x1160, 0x11FF, 0x135F, 0x135F,
    0x1712, 0x1714, 0x1732, 0x1734, 0x1752, 0x1753, 0x1772, 0x1773,
    0x17B4, 0x17B5, 0x17B7, 0x17BD, 0x17C6, 0x17C6, 0x17C9, 0x17D3,
    0x17DD, 0x17DD, 0x180B, 0x180D, 0x18A9, 0x18A9, 0x1920, 0x1922,
    0x1927, 0x1928, 0x1932, 0x1932, 0x1939, 0x193B, 0x1A17, 0x1A18,
    0x1B00, 0x1B03, 0x1B34, 0x1B34, 0x1B36, 0x1B3A, 0x1B3C, 0x1B3C,
    0x1B42, 0x1B42, 0x1B6B, 0x1B73, 0x1DC0, 0x1DCA, 0x1DFE, 0x1DFF,
    0x200B, 0x200F, 0x202A, 0x202E, 0x2060, 0x2063, 0x206A, 0x206F,
    0x20D0, 0x20EF, 0x302A, 0x302F, 0x3099, 0x309A, 0xA806, 0xA806,
    0xA80B, 0xA80B, 0xA825, 0xA826, 0xFB1E, 0xFB1E, 0xFE00, 0xFE0F,
    0xFE20, 0xFE23, 0xFEFF, 0xFEFF, 0xFFF9, 0xFFFB, 0x10A01, 0x10A03,
    0x10A05, 0x10A06, 0x10A0C, 0x10A0F, 0x10A38, 0x10A3A, 0x10A3F, 0x10A3F,
    0x1D167, 0x1D169, 0x1D173, 0x1D182, 0x1D185, 0x1D18B, 0x1D1AA, 0x1D1AD,
    0x1D242, 0x1D244, 0xE0001, 0xE0001, 0xE0020, 0xE007F, 0xE0100, 0xE01EF,
    -1,
  };

  if (in_range(range1, c))	// 检查c是否在range1范围内
    return 0;	// 如果在范围内，返回0

  static uint32_t range2[] = {
    0x1100, 0x115F, 0x2329, 0x2329, 0x232A, 0x232A, 0x2E80, 0x303E,
    0x3040, 0xA4CF, 0xAC00, 0xD7A3, 0xF900, 0xFAFF, 0xFE10, 0xFE19,
    0xFE30, 0xFE6F, 0xFF00, 0xFF60, 0xFFE0, 0xFFE6, 0x1F000, 0x1F644,
    0x20000, 0x2FFFD, 0x30000, 0x3FFFD, -1,
  };	// 声明并初始化一个静态的无符号32位整数数组range2，用于保存一系列范围值

  if (in_range(range2, c))	// 检查c是否在range2范围内
    return 2;	// 如果在范围内，返回2

  return 1;	// 如果不在任何范围内，返回1
}

int display_width(char *p, int len) {	// 返回以固定宽度字体显示给定字符串所需的列数。
  char *start = p;	// 声明一个字符指针start，指向p的位置
  int w = 0;	// 声明一个整数w，用于记录宽度，初始值为0

  while (p - start < len) {	// 当p与start之间的差值小于len时循环执行
    uint32_t c = decode_utf8(&p, p);	// 调用decode_utf8函数，解码p指向的UTF-8字符，将解码结果赋值给无符号32位整数c
    w += char_width(c);	// 将解码字符的宽度加到w上
  }

  return w;	// 返回宽度w
}