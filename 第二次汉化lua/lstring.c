/*
** $Id: lstring.c $
** String table (keeps all strings handled by Lua)
** See Copyright Notice in lua.h
*/

#define lstring_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <string.h>

#include "lua.h"

#include "ldebug.h"
#include "ldo.h"
#include "lmem.h"
#include "lobject.h"
#include "lstate.h"
#include "lstring.h"


/*
** Maximum 大小_变量 for string table.
*/
#define 字符串_最大串表_宏名	限制_类型转换_整型_宏名(内存_月亮M_限制N_宏名(限制_最大_整型_宏名, 标签字符串_结*))


/*
** equality for long strings
*/
int 月亮字符串_相等长型串_函 (标签字符串_结 *a, 标签字符串_结 *b) {
  size_t 长度_短变量 = a->u.长串长_短;
  限制_月亮_断言_宏名(a->类标_缩变量 == 对象_月亮_V长型串_宏名 && b->类标_缩变量 == 对象_月亮_V长型串_宏名);
  return (a == b) ||  /* same instance or... */
    ((长度_短变量 == b->u.长串长_短) &&  /* equal length and ... */
     (memcmp(对象_获取串_宏名(a), 对象_获取串_宏名(b), 长度_短变量) == 0));  /* equal 内容_小写 */
}


unsigned int 月亮字符串_哈希_函 (const char *串_变量, size_t l, unsigned int 种籽_圆) {
  unsigned int h = 种籽_圆 ^ 限制_类型转换_无符整型_宏名(l);
  for (; l > 0; l--)
    h ^= ((h<<5) + (h>>2) + 限制_类型转换_字节_宏名(串_变量[l - 1]));
  return h;
}


unsigned int 月亮字符串_哈希长的串_函 (标签字符串_结 *类s_变量) {
  限制_月亮_断言_宏名(类s_变量->类标_缩变量 == 对象_月亮_V长型串_宏名);
  if (类s_变量->额外_变量 == 0) {  /* no 哈希_小写? */
    size_t 长度_短变量 = 类s_变量->u.长串长_短;
    类s_变量->哈希_小写 = 月亮字符串_哈希_函(对象_获取串_宏名(类s_变量), 长度_短变量, 类s_变量->哈希_小写);
    类s_变量->额外_变量 = 1;  /* now it has its 哈希_小写 */
  }
  return 类s_变量->哈希_小写;
}


static void 月字符串_表重新哈希_函 (标签字符串_结 **vect, int 原始大小_变量, int 新大小_变量) {
  int i;
  for (i = 原始大小_变量; i < 新大小_变量; i++)  /* clear new elements */
    vect[i] = NULL;
  for (i = 0; i < 原始大小_变量; i++) {  /* 月表_重新哈希_函 旧_变量 part of the 数组_圆 */
    标签字符串_结 *p = vect[i];
    vect[i] = NULL;
    while (p) {  /* for each string in the 列表_变量 */
      标签字符串_结 *哈希下个_变量 = p->u.哈希下个_变量;  /* 月词法_保存_函 下一个_变量 */
      unsigned int h = 对象_月取模_宏名(p->哈希_小写, 新大小_变量);  /* new position */
      p->u.哈希下个_变量 = vect[h];  /* chain it into 数组_圆 */
      vect[h] = p;
      p = 哈希下个_变量;
    }
  }
}


/*
** Resize the string table. If allocation fails, keep the 当前_圆 大小_变量.
** (This can degrade performance, but any non-zero 大小_变量 should 工作_变量
** correctly.)
*/
void 月亮字符串_调整大小_函 (炉_状态机结 *L, int 新大小_变量) {
  字符串表_结 *临时块_变量 = &G(L)->串的表_缩;
  int 原始大小_变量 = 临时块_变量->大小_变量;
  标签字符串_结 **新向量_变量;
  if (新大小_变量 < 原始大小_变量)  /* shrinking table? */
    月字符串_表重新哈希_函(临时块_变量->哈希_小写, 原始大小_变量, 新大小_变量);  /* depopulate shrinking part */
  新向量_变量 = 内存_月亮M_重新分配向量_宏名(L, 临时块_变量->哈希_小写, 原始大小_变量, 新大小_变量, 标签字符串_结*);
  if (配置_l_可能性低_宏名(新向量_变量 == NULL)) {  /* reallocation failed? */
    if (新大小_变量 < 原始大小_变量)  /* was it shrinking table? */
      月字符串_表重新哈希_函(临时块_变量->哈希_小写, 新大小_变量, 原始大小_变量);  /* restore 到_变量 original 大小_变量 */
    /* leave table as it was */
  }
  else {  /* allocation succeeded */
    临时块_变量->哈希_小写 = 新向量_变量;
    临时块_变量->大小_变量 = 新大小_变量;
    if (新大小_变量 > 原始大小_变量)
      月字符串_表重新哈希_函(新向量_变量, 原始大小_变量, 新大小_变量);  /* 月表_重新哈希_函 for new 大小_变量 */
  }
}


/*
** Clear API string cache. (Entries 月编译器_不能_函 be 空容器_变量, so fill them with
** a non-collectable string.)
*/
void 月亮字符串_清除缓存_函 (全局_状态机结 *g) {
  int i, j;
  for (i = 0; i < 串缓存_N大写; i++)
    for (j = 0; j < 串缓存_M大写; j++) {
      if (垃圾回收_是否白色_宏名(g->串缓存_短圆[i][j]))  /* will entry be collected? */
        g->串缓存_短圆[i][j] = g->内存错误消息_短;  /* replace it with something fixed */
    }
}


/*
** Initialize the string table and the string cache
*/
void 月亮字符串_初始的_函 (炉_状态机结 *L) {
  全局_状态机结 *g = G(L);
  int i, j;
  字符串表_结 *临时块_变量 = &G(L)->串的表_缩;
  临时块_变量->哈希_小写 = 内存_月亮M_新向量_宏名(L, 限制_最小串表大小_宏名, 标签字符串_结*);
  月字符串_表重新哈希_函(临时块_变量->哈希_小写, 0, 限制_最小串表大小_宏名);  /* clear 数组_圆 */
  临时块_变量->大小_变量 = 限制_最小串表大小_宏名;
  /* pre-create memory-错误_小变量 message */
  g->内存错误消息_短 = 字符串_月亮S_新字面_宏名(L, 字符串_内存错误消息_宏名);
  月亮编译_修复_函(L, 状态机_对象到垃圾回收对象_宏名(g->内存错误消息_短));  /* it should never be collected */
  for (i = 0; i < 串缓存_N大写; i++)  /* fill cache with 有效_变量 strings */
    for (j = 0; j < 串缓存_M大写; j++)
      g->串缓存_短圆[i][j] = g->内存错误消息_短;
}



/*
** creates a new string object
*/
static 标签字符串_结 *月字符串_创建串对象_函 (炉_状态机结 *L, size_t l, int tag, unsigned int h) {
  标签字符串_结 *类s_变量;
  垃回对象_结 *o;
  size_t 总大小_变量;  /* 总数_变量 大小_变量 of 标签字符串_结 object */
  总大小_变量 = 字符串_大写l字符串_宏名(l);
  o = 月亮编译_新对象_函(L, tag, 总大小_变量);
  类s_变量 = 状态机_垃圾回收对象到t串_宏名(o);
  类s_变量->哈希_小写 = h;
  类s_变量->额外_变量 = 0;
  对象_获取串_宏名(类s_变量)[l] = '\0';  /* ending 0 */
  return 类s_变量;
}


标签字符串_结 *月亮字符串_创建长型串对象_函 (炉_状态机结 *L, size_t l) {
  标签字符串_结 *类s_变量 = 月字符串_创建串对象_函(L, l, 对象_月亮_V长型串_宏名, G(L)->种籽_圆);
  类s_变量->u.长串长_短 = l;
  return 类s_变量;
}


void 月亮字符串_移除_函 (炉_状态机结 *L, 标签字符串_结 *类s_变量) {
  字符串表_结 *临时块_变量 = &G(L)->串的表_缩;
  标签字符串_结 **p = &临时块_变量->哈希_小写[对象_月取模_宏名(类s_变量->哈希_小写, 临时块_变量->大小_变量)];
  while (*p != 类s_变量)  /* find 前一个_变量 element */
    p = &(*p)->u.哈希下个_变量;
  *p = (*p)->u.哈希下个_变量;  /* remove element from its 列表_变量 */
  临时块_变量->使用数_缩--;
}


static void 月字符串_增长串表_函 (炉_状态机结 *L, 字符串表_结 *临时块_变量) {
  if (配置_l_可能性低_宏名(临时块_变量->使用数_缩 == 限制_最大_整型_宏名)) {  /* too many strings? */
    月亮编译_完全垃圾回收_函(L, 1);  /* try 到_变量 free some... */
    if (临时块_变量->使用数_缩 == 限制_最大_整型_宏名)  /* still too many? */
      内存_月亮M_错误_宏名(L);  /* 月编译器_不能_函 even create a message... */
  }
  if (临时块_变量->大小_变量 <= 字符串_最大串表_宏名 / 2)  /* can grow string table? */
    月亮字符串_调整大小_函(L, 临时块_变量->大小_变量 * 2);
}


/*
** Checks whether short string exists and reuses it or creates a new one.
*/
static 标签字符串_结 *月字符串_内部短串_函 (炉_状态机结 *L, const char *串_变量, size_t l) {
  标签字符串_结 *类s_变量;
  全局_状态机结 *g = G(L);
  字符串表_结 *临时块_变量 = &g->串的表_缩;
  unsigned int h = 月亮字符串_哈希_函(串_变量, l, g->种籽_圆);
  标签字符串_结 **列表_变量 = &临时块_变量->哈希_小写[对象_月取模_宏名(h, 临时块_变量->大小_变量)];
  限制_月亮_断言_宏名(串_变量 != NULL);  /* otherwise 'memcmp'/'memcpy' are undefined */
  for (类s_变量 = *列表_变量; 类s_变量 != NULL; 类s_变量 = 类s_变量->u.哈希下个_变量) {
    if (l == 类s_变量->短串长_小写 && (memcmp(串_变量, 对象_获取串_宏名(类s_变量), l * sizeof(char)) == 0)) {
      /* found! */
      if (垃圾回收_是否死亡_宏名(g, 类s_变量))  /* dead (but not collected yet)? */
        垃圾回收_改变为白色_宏名(类s_变量);  /* resurrect it */
      return 类s_变量;
    }
  }
  /* else must create a new string */
  if (临时块_变量->使用数_缩 >= 临时块_变量->大小_变量) {  /* need 到_变量 grow string table? */
    月字符串_增长串表_函(L, 临时块_变量);
    列表_变量 = &临时块_变量->哈希_小写[对象_月取模_宏名(h, 临时块_变量->大小_变量)];  /* 月表_重新哈希_函 with new 大小_变量 */
  }
  类s_变量 = 月字符串_创建串对象_函(L, l, 对象_月亮_V短型串_宏名, h);
  memcpy(对象_获取串_宏名(类s_变量), 串_变量, l * sizeof(char));
  类s_变量->短串长_小写 = 限制_类型转换_字节_宏名(l);
  类s_变量->u.哈希下个_变量 = *列表_变量;
  *列表_变量 = 类s_变量;
  临时块_变量->使用数_缩++;
  return 类s_变量;
}


/*
** new string (with explicit length)
*/
标签字符串_结 *月亮字符串_新长串_函 (炉_状态机结 *L, const char *串_变量, size_t l) {
  if (l <= 限制_月亮I_最大短型长度_宏名)  /* short string? */
    return 月字符串_内部短串_函(L, 串_变量, l);
  else {
    标签字符串_结 *类s_变量;
    if (配置_l_可能性低_宏名(l >= (限制_最大_大小_宏名 - sizeof(标签字符串_结))/sizeof(char)))
      月亮内存_太大_函(L);
    类s_变量 = 月亮字符串_创建长型串对象_函(L, l);
    memcpy(对象_获取串_宏名(类s_变量), 串_变量, l * sizeof(char));
    return 类s_变量;
  }
}


/*
** Create or reuse a zero-terminated string, 首先_变量 checking in the
** cache (using the string address as a 键_小变量). The cache can contain
** only zero-terminated strings, so it is safe 到_变量 use 'strcmp' 到_变量
** 月解析器_检查_函 hits.
*/
标签字符串_结 *月亮字符串_新_函 (炉_状态机结 *L, const char *串_变量) {
  unsigned int i = 限制_指针到无符整型_宏名(串_变量) % 串缓存_N大写;  /* 哈希_小写 */
  int j;
  标签字符串_结 **p = G(L)->串缓存_短圆[i];
  for (j = 0; j < 串缓存_M大写; j++) {
    if (strcmp(串_变量, 对象_获取串_宏名(p[j])) == 0)  /* hit? */
      return p[j];  /* that is it */
  }
  /* normal route */
  for (j = 串缓存_M大写 - 1; j > 0; j--)
    p[j] = p[j - 1];  /* move out 最后_变量 element */
  /* new element is 首先_变量 in the 列表_变量 */
  p[0] = 月亮字符串_新长串_函(L, 串_变量, strlen(串_变量));
  return p[0];
}


用户数据_结 *月亮字符串_新用户数据_函 (炉_状态机结 *L, size_t s, int 用户值数_缩) {
  用户数据_结 *u;
  int i;
  垃回对象_结 *o;
  if (配置_l_可能性低_宏名(s > 限制_最大_大小_宏名 - 对象_用户数据内存偏移_宏名(用户值数_缩)))
    月亮内存_太大_函(L);
  o = 月亮编译_新对象_函(L, 对象_月亮_V用户数据_宏名, 对象_用户数据大小_宏名(用户值数_缩, s));
  u = 状态机_垃圾回收对象到u_宏名(o);
  u->长度_短变量 = s;
  u->用户值数_缩 = 用户值数_缩;
  u->元表_小写 = NULL;
  for (i = 0; i < 用户值数_缩; i++)
    对象_设置空值的值_宏名(&u->上值_缩变量[i].上值_缩变量);
  return u;
}

