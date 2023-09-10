/*
** $Id: 月词法_月的词法_函.c $
** Lexical Analyzer
** See Copyright Notice in lua.h
*/

#define llex_c
#define 应程接_月亮_内核_宏名

#include "lprefix.h"


#include <locale.h>
#include <string.h>

#include "lua.h"

#include "lctype.h"
#include "ldebug.h"
#include "ldo.h"
#include "lgc.h"
#include "llex.h"
#include "lobject.h"
#include "lparser.h"
#include "lstate.h"
#include "lstring.h"
#include "ltable.h"
#include "lzio.h"



#define 下一个_变量(状列_缩变量)	(状列_缩变量->当前_圆 = 入出流_流获取字符_宏名(状列_缩变量->z))



#define 词法_当前是否换行_宏名(状列_缩变量)	(状列_缩变量->当前_圆 == '\n' || 状列_缩变量->当前_圆 == '\r')


/* ORDER 保留_枚举 */
static const char *const 月亮X_牌们_变量 [] = {
    "与", "破断", "做", "否则", "否若",
    "终", "假", "为", "函数", "去到", "若",
    "在内", "本地", "空值", "非", "或", "重复",
    "返回", "那么", "真", "直到", "一会儿",
    "//", "..", "...", "==", ">=", "<=", "~=",
    "<<", ">>", "::", "<eof>",
    "<数目>", "<整数>", "<名称>", "<字符串>"
};


#define 词法_保存_与_下一个_宏名(状列_缩变量) (月词法_保存_函(状列_缩变量, 状列_缩变量->当前_圆), 下一个_变量(状列_缩变量))


static 限制_l_无返回值_宏名 月词法_词法错误_函 (词法状态机_结 *状列_缩变量, const char *消息_缩变量, int 牌_小写);


static void 月词法_保存_函 (词法状态机_结 *状列_缩变量, int c) {
  M缓冲区_结 *b = 状列_缩变量->缓冲_变量;
  if (入出流_月亮Z_缓冲长度_宏名(b) + 1 > 入出流_月亮Z_缓冲区大小_宏名(b)) {
    size_t 新大小_变量;
    if (入出流_月亮Z_缓冲区大小_宏名(b) >= 限制_最大_大小_宏名/2)
      月词法_词法错误_函(状列_缩变量, "lexical element too long", 0);
    新大小_变量 = 入出流_月亮Z_缓冲区大小_宏名(b) * 2;
    入出流_月亮Z_调整缓冲区大小_宏名(状列_缩变量->L, b, 新大小_变量);
  }
  b->缓冲区_变量[入出流_月亮Z_缓冲长度_宏名(b)++] = 限制_类型转换_印刻_宏名(c);
}


void 月亮分析_初始的_函 (炉_状态机结 *L) {
  int i;
  标签字符串_结 *e = 字符串_月亮S_新字面_宏名(L, 词法_月亮_环境_宏名);  /* create 环境_短变量 名称_变量 */
  月亮编译_修复_函(L, 状态机_对象到垃圾回收对象_宏名(e));  /* never collect this 名称_变量 */
  for (i=0; i<词法_数目_保留字_宏名; i++) {
    标签字符串_结 *类s_变量 = 月亮字符串_新_函(L, 月亮X_牌们_变量[i]);
    月亮编译_修复_函(L, 状态机_对象到垃圾回收对象_宏名(类s_变量));  /* reserved words are never collected */
    类s_变量->额外_变量 = 限制_类型转换_字节_宏名(i+1);  /* reserved word */
  }
}


const char *月亮分析_牌到串_函 (词法状态机_结 *状列_缩变量, int 牌_小写) {
  if (牌_小写 < 首个_保留) {  /* single-byte symbols? */
    if (c类型_l是否打印_宏名(牌_小写))
      return 月亮对象_推入格式化字符串_函(状列_缩变量->L, "'%c'", 牌_小写);
    else  /* control character */
      return 月亮对象_推入格式化字符串_函(状列_缩变量->L, "'<\\%d>'", 牌_小写);
  }
  else {
    const char *s = 月亮X_牌们_变量[牌_小写 - 首个_保留];
    if (牌_小写 < 终结牌_流终)  /* fixed 格式_变量 (symbols and reserved words)? */
      return 月亮对象_推入格式化字符串_函(状列_缩变量->L, "'%s'", s);
    else  /* names, strings, and numerals */
      return s;
  }
}


static const char *月词法_文本牌_函 (词法状态机_结 *状列_缩变量, int 牌_小写) {
  switch (牌_小写) {
    case 终结牌_名称: case 终结牌_字符串:
    case 终结牌_浮点: case 终结牌_整型:
      月词法_保存_函(状列_缩变量, '\0');
      return 月亮对象_推入格式化字符串_函(状列_缩变量->L, "'%s'", 入出流_月亮Z_缓冲区_宏名(状列_缩变量->缓冲_变量));
    default:
      return 月亮分析_牌到串_函(状列_缩变量, 牌_小写);
  }
}


static 限制_l_无返回值_宏名 月词法_词法错误_函 (词法状态机_结 *状列_缩变量, const char *消息_缩变量, int 牌_小写) {
  消息_缩变量 = 月亮全局_添加信息_函(状列_缩变量->L, 消息_缩变量, 状列_缩变量->源_圆, 状列_缩变量->行数目_小写);
  if (牌_小写)
    月亮对象_推入格式化字符串_函(状列_缩变量->L, "%s near %s", 消息_缩变量, 月词法_文本牌_函(状列_缩变量, 牌_小写));
  月亮调度_抛出_函(状列_缩变量->L, 月头_月亮_句法错误_宏名);
}


限制_l_无返回值_宏名 月亮分析_句法错误_函 (词法状态机_结 *状列_缩变量, const char *消息_缩变量) {
  月词法_词法错误_函(状列_缩变量, 消息_缩变量, 状列_缩变量->t.牌_小写);
}


/*
** Creates a new string and anchors it in scanner's table so that it
** will not be collected until the 终_变量 of the compilation; by that time
** it should be anchored somewhere. It also internalizes long strings,
** ensuring there is only one copy of each unique string.  The table
** here is used as a set: the string enters as the 键_小变量, while its 值_圆
** is irrelevant. We use the string itself as the 值_圆 only because it
** is a 标签值_结 readily available. Later, the 代码_变量 generation can 改变_变量
** this 值_圆.
*/
标签字符串_结 *月亮分析_新字符串_函 (词法状态机_结 *状列_缩变量, const char *串_变量, size_t l) {
  炉_状态机结 *L = 状列_缩变量->L;
  标签字符串_结 *类s_变量 = 月亮字符串_新长串_函(L, 串_变量, l);  /* create new string */
  const 标签值_结 *o = 月亮哈希表_获取串键_函(状列_缩变量->h, 类s_变量);
  if (!对象_tt是否空值_宏名(o))  /* string already present? */
    类s_变量 = 对象_键串值_宏名(表_节点来自值_宏名(o));  /* get saved copy */
  else {  /* not in use yet */
    标签值_结 *栈变_变量 = 对象_s到v_宏名(L->顶部_变量.p++);  /* reserve 栈_圆小 空间_圆 for string */
    对象_设置ts值_宏名(L, 栈变_变量, 类s_变量);  /* temporarily 锚点_变量 the string */
    月亮哈希表_完成设置_函(L, 状列_缩变量->h, 栈变_变量, o, 栈变_变量);  /* t[string] = string */
    /* table is not a 元表_小写, so it does not need 到_变量 invalidate cache */
    垃圾回收_月亮C_检查GC_宏名(L);
    L->顶部_变量.p--;  /* remove string from 栈_圆小 */
  }
  return 类s_变量;
}


/*
** increment 行_变量 number and skips 新行_小变量 sequence (any of
** \n, \r, \n\r, or \r\n)
*/
static void 月词法_递增行号_函 (词法状态机_结 *状列_缩变量) {
  int 旧_变量 = 状列_缩变量->当前_圆;
  限制_月亮_断言_宏名(词法_当前是否换行_宏名(状列_缩变量));
  下一个_变量(状列_缩变量);  /* skip '\n' or '\r' */
  if (词法_当前是否换行_宏名(状列_缩变量) && 状列_缩变量->当前_圆 != 旧_变量)
    下一个_变量(状列_缩变量);  /* skip '\n\r' or '\r\n' */
  if (++状列_缩变量->行数目_小写 >= 限制_最大_整型_宏名)
    月词法_词法错误_函(状列_缩变量, "chunk has too many lines", 0);
}


void 月亮分析_设置输入_函 (炉_状态机结 *L, 词法状态机_结 *状列_缩变量, 入出流_结 *z, 标签字符串_结 *源_圆,
                    int firstchar) {
  状列_缩变量->t.牌_小写 = 0;
  状列_缩变量->L = L;
  状列_缩变量->当前_圆 = firstchar;
  状列_缩变量->看向前_小写.牌_小写 = 终结牌_流终;  /* no look-ahead 牌_小写 */
  状列_缩变量->z = z;
  状列_缩变量->字段静态_变量 = NULL;
  状列_缩变量->行数目_小写 = 1;
  状列_缩变量->最后行_小写 = 1;
  状列_缩变量->源_圆 = 源_圆;
  状列_缩变量->环境名_短 = 字符串_月亮S_新字面_宏名(L, 词法_月亮_环境_宏名);  /* get 环境_短变量 名称_变量 */
  入出流_月亮Z_调整缓冲区大小_宏名(状列_缩变量->L, 状列_缩变量->缓冲_变量, 限制_月亮_最小缓冲区_宏名);  /* initialize 缓冲区_变量 */
}



/*
** =======================================================
** LEXICAL ANALYZER
** =======================================================
*/


static int 月词法_检查_下一个1_函 (词法状态机_结 *状列_缩变量, int c) {
  if (状列_缩变量->当前_圆 == c) {
    下一个_变量(状列_缩变量);
    return 1;
  }
  else return 0;
}


/*
** Check whether 当前_圆 char is in set 'set' (with two chars) and
** saves it
*/
static int 月词法_检查_下一个2_函 (词法状态机_结 *状列_缩变量, const char *set) {
  限制_月亮_断言_宏名(set[2] == '\0');
  if (状列_缩变量->当前_圆 == set[0] || 状列_缩变量->当前_圆 == set[1]) {
    词法_保存_与_下一个_宏名(状列_缩变量);
    return 1;
  }
  else return 0;
}


/* 配置_月亮_数目_宏名 */
/*
** This function is quite liberal in 什么_变量 it accepts, as '月亮对象_字符串到数目_函'
** will reject ill-formed numerals. Roughly, it accepts the following
** pattern:
**
**   %d(%x|%.|([Ee][+-]?))* | 0[Xx](%x|%.|([Pp][+-]?))*
**
** The only tricky part is 到_变量 accept [+-] only after a 有效_变量 exponent
** 记号_变量, 到_变量 avoid reading '3-4' or '0xe+1' as a single number.
**
** The caller might have already read an initial 点_变量.
*/
static int 月词法_读_数值的_函 (词法状态机_结 *状列_缩变量, 语义信息_联 *语义信息_小写) {
  标签值_结 对象_变量;
  const char *指数_短变量 = "Ee";
  int 首先_变量 = 状列_缩变量->当前_圆;
  限制_月亮_断言_宏名(c类型_l是否数字_宏名(状列_缩变量->当前_圆));
  词法_保存_与_下一个_宏名(状列_缩变量);
  if (首先_变量 == '0' && 月词法_检查_下一个2_函(状列_缩变量, "xX"))  /* hexadecimal? */
    指数_短变量 = "Pp";
  for (;;) {
    if (月词法_检查_下一个2_函(状列_缩变量, 指数_短变量))  /* exponent 记号_变量? */
      月词法_检查_下一个2_函(状列_缩变量, "-+");  /* optional exponent sign */
    else if (c类型_l是否十六进制数字_宏名(状列_缩变量->当前_圆) || 状列_缩变量->当前_圆 == '.')  /* '%x|%.' */
      词法_保存_与_下一个_宏名(状列_缩变量);
    else break;
  }
  if (c类型_l是否l字母_宏名(状列_缩变量->当前_圆))  /* is numeral touching a letter? */
    词法_保存_与_下一个_宏名(状列_缩变量);  /* force an 错误_小变量 */
  月词法_保存_函(状列_缩变量, '\0');
  if (月亮对象_字符串到数目_函(入出流_月亮Z_缓冲区_宏名(状列_缩变量->缓冲_变量), &对象_变量) == 0)  /* 格式_变量 错误_小变量? */
    月词法_词法错误_函(状列_缩变量, "malformed number", 终结牌_浮点);
  if (对象_tt是否整数_宏名(&对象_变量)) {
    语义信息_小写->i = 对象_整数值_宏名(&对象_变量);
    return 终结牌_整型;
  }
  else {
    限制_月亮_断言_宏名(对象_tt是否浮点_宏名(&对象_变量));
    语义信息_小写->r = 对象_浮点值_宏名(&对象_变量);
    return 终结牌_浮点;
  }
}


/*
** read a sequence '[=*[' or ']=*]', leaving the 最后_变量 bracket. If
** sequence is well formed, return its number of '='s + 2; otherwise,
** return 1 if it is a single bracket (no '='s and no 2nd bracket);
** otherwise (an unfinished '[==...') return 0.
*/
static size_t 月词法_跳过_分隔符_函 (词法状态机_结 *状列_缩变量) {
  size_t 计数_变量 = 0;
  int s = 状列_缩变量->当前_圆;
  限制_月亮_断言_宏名(s == '[' || s == ']');
  词法_保存_与_下一个_宏名(状列_缩变量);
  while (状列_缩变量->当前_圆 == '=') {
    词法_保存_与_下一个_宏名(状列_缩变量);
    计数_变量++;
  }
  return (状列_缩变量->当前_圆 == s) ? 计数_变量 + 2
         : (计数_变量 == 0) ? 1
         : 0;
}


static void 月词法_读_长型_字符串_函 (词法状态机_结 *状列_缩变量, 语义信息_联 *语义信息_小写, size_t 分隔_变量) {
  int 行_变量 = 状列_缩变量->行数目_小写;  /* initial 行_变量 (for 错误_小变量 message) */
  词法_保存_与_下一个_宏名(状列_缩变量);  /* skip 2nd '[' */
  if (词法_当前是否换行_宏名(状列_缩变量))  /* string starts with a 新行_小变量? */
    月词法_递增行号_函(状列_缩变量);  /* skip it */
  for (;;) {
    switch (状列_缩变量->当前_圆) {
      case EOZ: {  /* 错误_小变量 */
        const char *什么_变量 = (语义信息_小写 ? "string" : "comment");
        const char *消息_缩变量 = 月亮对象_推入格式化字符串_函(状列_缩变量->L,
                     "unfinished long %s (starting at 行_变量 %d)", 什么_变量, 行_变量);
        月词法_词法错误_函(状列_缩变量, 消息_缩变量, 终结牌_流终);
        break;  /* 到_变量 avoid warnings */
      }
      case ']': {
        if (月词法_跳过_分隔符_函(状列_缩变量) == 分隔_变量) {
          词法_保存_与_下一个_宏名(状列_缩变量);  /* skip 2nd ']' */
          goto endloop;
        }
        break;
      }
      case '\n': case '\r': {
        月词法_保存_函(状列_缩变量, '\n');
        月词法_递增行号_函(状列_缩变量);
        if (!语义信息_小写) 入出流_月亮Z_重置缓冲区_宏名(状列_缩变量->缓冲_变量);  /* avoid wasting 空间_圆 */
        break;
      }
      default: {
        if (语义信息_小写) 词法_保存_与_下一个_宏名(状列_缩变量);
        else 下一个_变量(状列_缩变量);
      }
    }
  } endloop:
  if (语义信息_小写)
    语义信息_小写->类s_变量 = 月亮分析_新字符串_函(状列_缩变量, 入出流_月亮Z_缓冲区_宏名(状列_缩变量->缓冲_变量) + 分隔_变量,
                                     入出流_月亮Z_缓冲长度_宏名(状列_缩变量->缓冲_变量) - 2 * 分隔_变量);
}


static void 月词法_转义检查_函 (词法状态机_结 *状列_缩变量, int c, const char *消息_缩变量) {
  if (!c) {
    if (状列_缩变量->当前_圆 != EOZ)
      词法_保存_与_下一个_宏名(状列_缩变量);  /* add 当前_圆 到_变量 缓冲区_变量 for 错误_小变量 message */
    月词法_词法错误_函(状列_缩变量, 消息_缩变量, 终结牌_字符串);
  }
}


static int 月词法_获取十六进制_函 (词法状态机_结 *状列_缩变量) {
  词法_保存_与_下一个_宏名(状列_缩变量);
  月词法_转义检查_函 (状列_缩变量, c类型_l是否十六进制数字_宏名(状列_缩变量->当前_圆), "hexadecimal 数字_变量 expected");
  return 月亮对象_十六进制值_函(状列_缩变量->当前_圆);
}


static int 月词法_读十六进制转义_函 (词法状态机_结 *状列_缩变量) {
  int r = 月词法_获取十六进制_函(状列_缩变量);
  r = (r << 4) + 月词法_获取十六进制_函(状列_缩变量);
  入出流_月亮Z_缓冲移除_宏名(状列_缩变量->缓冲_变量, 2);  /* remove saved chars from 缓冲区_变量 */
  return r;
}


static unsigned long 月词法_读utf8转义_函 (词法状态机_结 *状列_缩变量) {
  unsigned long r;
  int i = 4;  /* chars 到_变量 be removed: '\', 'u', '{', and 首先_变量 数字_变量 */
  词法_保存_与_下一个_宏名(状列_缩变量);  /* skip 'u' */
  月词法_转义检查_函(状列_缩变量, 状列_缩变量->当前_圆 == '{', "missing '{'");
  r = 月词法_获取十六进制_函(状列_缩变量);  /* must have at least one 数字_变量 */
  while (限制_类型转换_空的_宏名(词法_保存_与_下一个_宏名(状列_缩变量)), c类型_l是否十六进制数字_宏名(状列_缩变量->当前_圆)) {
    i++;
    月词法_转义检查_函(状列_缩变量, r <= (0x7FFFFFFFu >> 4), "UTF-8 值_圆 too large");
    r = (r << 4) + 月亮对象_十六进制值_函(状列_缩变量->当前_圆);
  }
  月词法_转义检查_函(状列_缩变量, 状列_缩变量->当前_圆 == '}', "missing '}'");
  下一个_变量(状列_缩变量);  /* skip '}' */
  入出流_月亮Z_缓冲移除_宏名(状列_缩变量->缓冲_变量, i);  /* remove saved chars from 缓冲区_变量 */
  return r;
}


static void 月词法_utf8转义_函 (词法状态机_结 *状列_缩变量) {
  char 缓冲_变量[对象_UTF8缓冲大小_宏名];
  int n = 月亮对象_utf8转义_函(缓冲_变量, 月词法_读utf8转义_函(状列_缩变量));
  for (; n > 0; n--)  /* add '缓冲_变量' 到_变量 string */
    月词法_保存_函(状列_缩变量, 缓冲_变量[对象_UTF8缓冲大小_宏名 - n]);
}


static int 月词法_读十进制转义_函 (词法状态机_结 *状列_缩变量) {
  int i;
  int r = 0;  /* 结果_变量 accumulator */
  for (i = 0; i < 3 && c类型_l是否数字_宏名(状列_缩变量->当前_圆); i++) {  /* read 上_小变量 到_变量 3 digits */
    r = 10*r + 状列_缩变量->当前_圆 - '0';
    词法_保存_与_下一个_宏名(状列_缩变量);
  }
  月词法_转义检查_函(状列_缩变量, r <= UCHAR_MAX, "decimal escape too large");
  入出流_月亮Z_缓冲移除_宏名(状列_缩变量->缓冲_变量, i);  /* remove read digits from 缓冲区_变量 */
  return r;
}


static void 月词法_读_字符串_函 (词法状态机_结 *状列_缩变量, int del, 语义信息_联 *语义信息_小写) {
  词法_保存_与_下一个_宏名(状列_缩变量);  /* keep delimiter (for 错误_小变量 messages) */
  while (状列_缩变量->当前_圆 != del) {
    switch (状列_缩变量->当前_圆) {
      case EOZ:
        月词法_词法错误_函(状列_缩变量, "unfinished string", 终结牌_流终);
        break;  /* 到_变量 avoid warnings */
      case '\n':
      case '\r':
        月词法_词法错误_函(状列_缩变量, "unfinished string", 终结牌_字符串);
        break;  /* 到_变量 avoid warnings */
      case '\\': {  /* escape sequences */
        int c;  /* 最终_变量 character 到_变量 be saved */
        词法_保存_与_下一个_宏名(状列_缩变量);  /* keep '\\' for 错误_小变量 messages */
        switch (状列_缩变量->当前_圆) {
          case 'a': c = '\a'; goto read_save;
          case 'b': c = '\b'; goto read_save;
          case 'f': c = '\f'; goto read_save;
          case 'n': c = '\n'; goto read_save;
          case 'r': c = '\r'; goto read_save;
          case 't': c = '\t'; goto read_save;
          case 'v': c = '\v'; goto read_save;
          case 'x': c = 月词法_读十六进制转义_函(状列_缩变量); goto read_save;
          case 'u': 月词法_utf8转义_函(状列_缩变量);  goto no_save;
          case '\n': case '\r':
            月词法_递增行号_函(状列_缩变量); c = '\n'; goto only_save;
          case '\\': case '\"': case '\'':
            c = 状列_缩变量->当前_圆; goto read_save;
          case EOZ: goto no_save;  /* will raise an 错误_小变量 下一个_变量 环_变量 */
          case 'z': {  /* zap following span of spaces */
            入出流_月亮Z_缓冲移除_宏名(状列_缩变量->缓冲_变量, 1);  /* remove '\\' */
            下一个_变量(状列_缩变量);  /* skip the 'z' */
            while (c类型_l是否空格_宏名(状列_缩变量->当前_圆)) {
              if (词法_当前是否换行_宏名(状列_缩变量)) 月词法_递增行号_函(状列_缩变量);
              else 下一个_变量(状列_缩变量);
            }
            goto no_save;
          }
          default: {
            月词法_转义检查_函(状列_缩变量, c类型_l是否数字_宏名(状列_缩变量->当前_圆), "invalid escape sequence");
            c = 月词法_读十进制转义_函(状列_缩变量);  /* digital escape '\ddd' */
            goto only_save;
          }
        }
       read_save:
         下一个_变量(状列_缩变量);
         /* go through */
       only_save:
         入出流_月亮Z_缓冲移除_宏名(状列_缩变量->缓冲_变量, 1);  /* remove '\\' */
         月词法_保存_函(状列_缩变量, c);
         /* go through */
       no_save: break;
      }
      default:
        词法_保存_与_下一个_宏名(状列_缩变量);
    }
  }
  词法_保存_与_下一个_宏名(状列_缩变量);  /* skip delimiter */
  语义信息_小写->类s_变量 = 月亮分析_新字符串_函(状列_缩变量, 入出流_月亮Z_缓冲区_宏名(状列_缩变量->缓冲_变量) + 1,
                                   入出流_月亮Z_缓冲长度_宏名(状列_缩变量->缓冲_变量) - 2);
}


static int 月词法_月的词法_函 (词法状态机_结 *状列_缩变量, 语义信息_联 *语义信息_小写) {
  入出流_月亮Z_重置缓冲区_宏名(状列_缩变量->缓冲_变量);
  for (;;) {
    switch (状列_缩变量->当前_圆) {
      case '\n': case '\r': {  /* 行_变量 breaks */
        月词法_递增行号_函(状列_缩变量);
        break;
      }
      case ' ': case '\f': case '\t': case '\v': {  /* spaces */
        下一个_变量(状列_缩变量);
        break;
      }
      case '-': {  /* '-' or '--' (comment) */
        下一个_变量(状列_缩变量);
        if (状列_缩变量->当前_圆 != '-') return '-';
        /* else is a comment */
        下一个_变量(状列_缩变量);
        if (状列_缩变量->当前_圆 == '[') {  /* long comment? */
          size_t 分隔_变量 = 月词法_跳过_分隔符_函(状列_缩变量);
          入出流_月亮Z_重置缓冲区_宏名(状列_缩变量->缓冲_变量);  /* '月词法_跳过_分隔符_函' may dirty the 缓冲区_变量 */
          if (分隔_变量 >= 2) {
            月词法_读_长型_字符串_函(状列_缩变量, NULL, 分隔_变量);  /* skip long comment */
            入出流_月亮Z_重置缓冲区_宏名(状列_缩变量->缓冲_变量);  /* 前一个_变量 call may dirty the 缓冲_变量. */
            break;
          }
        }
        /* else short comment */
        while (!词法_当前是否换行_宏名(状列_缩变量) && 状列_缩变量->当前_圆 != EOZ)
          下一个_变量(状列_缩变量);  /* skip until 终_变量 of 行_变量 (or 终_变量 of file) */
        break;
      }
      case '[': {  /* long string or simply '[' */
        size_t 分隔_变量 = 月词法_跳过_分隔符_函(状列_缩变量);
        if (分隔_变量 >= 2) {
          月词法_读_长型_字符串_函(状列_缩变量, 语义信息_小写, 分隔_变量);
          return 终结牌_字符串;
        }
        else if (分隔_变量 == 0)  /* '[=...' missing second bracket? */
          月词法_词法错误_函(状列_缩变量, "invalid long string delimiter", 终结牌_字符串);
        return '[';
      }
      case '=': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '=')) return 终结牌_相等;  /* '==' */
        else return '=';
      }
      case '<': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '=')) return 终结牌_小于;  /* '<=' */
        else if (月词法_检查_下一个1_函(状列_缩变量, '<')) return 终结牌_左移;  /* '<<' */
        else return '<';
      }
      case '>': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '=')) return 终结牌_大于;  /* '>=' */
        else if (月词法_检查_下一个1_函(状列_缩变量, '>')) return 终结牌_右移;  /* '>>' */
        else return '>';
      }
      case '/': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '/')) return 终结牌_整数除;  /* '//' */
        else return '/';
      }
      case '~': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '=')) return 终结牌_不等;  /* '~=' */
        else return '~';
      }
      case ':': {
        下一个_变量(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, ':')) return 终结牌_双冒号;  /* '::' */
        else return ':';
      }
      case '"': case '\'': {  /* short literal strings */
        月词法_读_字符串_函(状列_缩变量, 状列_缩变量->当前_圆, 语义信息_小写);
        return 终结牌_字符串;
      }
      case '.': {  /* '.', '..', '...', or number */
        词法_保存_与_下一个_宏名(状列_缩变量);
        if (月词法_检查_下一个1_函(状列_缩变量, '.')) {
          if (月词法_检查_下一个1_函(状列_缩变量, '.'))
            return 终结牌_省略;   /* '...' */
          else return 终结牌_拼接;   /* '..' */
        }
        else if (!c类型_l是否数字_宏名(状列_缩变量->当前_圆)) return '.';
        else return 月词法_读_数值的_函(状列_缩变量, 语义信息_小写);
      }
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9': {
        return 月词法_读_数值的_函(状列_缩变量, 语义信息_小写);
      }
      case EOZ: {
        return 终结牌_流终;
      }
      default: {
        if (c类型_l是否l字母_宏名(状列_缩变量->当前_圆)) {  /* identifier or reserved word? */
          标签字符串_结 *类s_变量;
          do {
            词法_保存_与_下一个_宏名(状列_缩变量);
          } while (c类型_l是否l字母或数字_宏名(状列_缩变量->当前_圆));
          类s_变量 = 月亮分析_新字符串_函(状列_缩变量, 入出流_月亮Z_缓冲区_宏名(状列_缩变量->缓冲_变量),
                                  入出流_月亮Z_缓冲长度_宏名(状列_缩变量->缓冲_变量));
          语义信息_小写->类s_变量 = 类s_变量;
          if (字符串_是否保留字_宏名(类s_变量))  /* reserved word? */
            return 类s_变量->额外_变量 - 1 + 首个_保留;
          else {
            return 终结牌_名称;
          }
        }
        else {  /* single-char tokens ('+', '*', '%', '{', '}', ...) */
          int c = 状列_缩变量->当前_圆;
          下一个_变量(状列_缩变量);
          return c;
        }
      }
    }
  }
}


void 月亮分析_下一个_函 (词法状态机_结 *状列_缩变量) {
  状列_缩变量->最后行_小写 = 状列_缩变量->行数目_小写;
  if (状列_缩变量->看向前_小写.牌_小写 != 终结牌_流终) {  /* is there a look-ahead 牌_小写? */
    状列_缩变量->t = 状列_缩变量->看向前_小写;  /* use this one */
    状列_缩变量->看向前_小写.牌_小写 = 终结牌_流终;  /* and discharge it */
  }
  else
    状列_缩变量->t.牌_小写 = 月词法_月的词法_函(状列_缩变量, &状列_缩变量->t.语义信息_小写);  /* read 下一个_变量 牌_小写 */
}


int 月亮分析_看向前_函 (词法状态机_结 *状列_缩变量) {
  限制_月亮_断言_宏名(状列_缩变量->看向前_小写.牌_小写 == 终结牌_流终);
  状列_缩变量->看向前_小写.牌_小写 = 月词法_月的词法_函(状列_缩变量, &状列_缩变量->看向前_小写.语义信息_小写);
  return 状列_缩变量->看向前_小写.牌_小写;
}

