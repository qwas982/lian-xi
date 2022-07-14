/*
 ** TinyASM - 8086/8088 assembler for DOS
 **
 ** by Oscar Toledo G.
 **
 ** Creation date: Oct/01/2019.
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <ctype.h>

#define 调试

char *输入_文件名;
int 行号;

char *输出_文件名;
FILE *输出;

char *列表列出文件名;
FILE *列表列出;

int 汇编器_步骤;
int 默认_开始_地址;
int 开始_地址;
int 地址;
int 第一_时间;

int 指令_地址的;
int 指令_偏移;
int 指令_偏移_宽度;

int 指令_寄存器;

int 指令_值;
int 指令_值2;

#define 最大_大小        256

char 行[最大_大小];
char 部分[最大_大小];
char 名称[最大_大小];
char 表达式_名称[最大_大小];
char 未定义的_名称[最大_大小];
char 全局_标号[最大_大小];
char *前一个_针;
char *p;

char *g;
char 生成的[8];

int 错误的;
int 警告的;
int 字节;
int 改变;
int 改变_号码;

struct 标号 {
    struct 标号 *左;
    struct 标号 *右;
    int 值;
    char 名称[1];
};

struct 标号 *标号_列表;
struct 标号 *最后_标号;
int 未定义的;

extern char *指令_集合[];

char *寄存器1[16] = {
    "AL",
    "CL",
    "DL",
    "BL",
    "AH",
    "CH",
    "DH",
    "BH",
    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI"
};

void 消息();
char *匹配_寄存器(), *匹配_表达式(),
     *匹配_表达式_等级1(), *匹配_表达式_等级2(),
     *匹配_表达式_等级3(), *匹配_表达式_等级4(),
     *匹配_表达式_等级5(), *匹配_表达式_等级6();

#ifdef __DESMET__
/* Work around bug in DeSmet 3.1N runtime: closeall() overflows buffer and clobbers exit 状态 */
#define exit(状态)	_exit(status)
#endif

/*
 ** Define a new 标号
 */
struct 标号 *定义_标号(名称, 值)
    char *名称;
    int 值;
{
    struct 标号 *标号;
    struct 标号 *探索;
    int c;
    
    /* Allocate 标号 */
    标号 = malloc(sizeof(struct 标号) + strlen(名称));
    if (标号 == NULL) {
        fprintf(stderr, "标号内存不足\n");
        exit(1);
        return NULL;
    }
    
    /* Fill 标号 */
    标号->左 = NULL;
    标号->右 = NULL;
    标号->值 = 值;
    strcpy(标号->名称, 名称);
    
    /* Populate binary tree */
    if (标号_列表 == NULL) {
        标号_列表 = 标号;
    } else {
        探索 = 标号_列表;
        while (1) {
            c = strcmp(标号->名称, 探索->名称);
            if (c < 0) {
                if (探索->左 == NULL) {
                    探索->左 = 标号;
                    break;
                }
                探索 = 探索->左;
            } else if (c > 0) {
                if (探索->右 == NULL) {
                    探索->右 = 标号;
                    break;
                }
                探索 = 探索->右;
            }
        }
    }
    return 标号;
}

/*
 ** Find a 标号
 */
struct 标号 *找_标号(名称)
    char *名称;
{
    struct 标号 *探索;
    int c;
    
    /* Follows a binary tree */
    探索 = 标号_列表;
    while (探索 != NULL) {
        c = strcmp(名称, 探索->名称);
        if (c == 0)
            return 探索;
        if (c < 0)
            探索 = 探索->左;
        else
            探索 = 探索->右;
    }
    return NULL;
}

/*
 ** Sort recursively labels (already done by binary tree)
 */
void 排序_标号(节点)
    struct 标号 *节点;
{
    if (节点->左 != NULL)
        排序_标号(节点->左);
    fprintf(列表列出, "%-20s %04x\n", 节点->名称, 节点->值);
    if (节点->右 != NULL)
        排序_标号(节点->右);
}

/*
 ** Avoid spaces in 输入
 */
char *避开_空格(p)
    char *p;
{
    while (isspace(*p))
        p++;
    return p;
}

/*
 ** Match addressing
 */
char *匹配_地址的(p, 宽度)
    char *p;
    int 宽度;
{
    int 寄存;
    int 寄存2;
    char *p2;
    int *位数;
    
    位数 = &指令_地址的;
    指令_偏移 = 0;
    指令_偏移_宽度 = 0;
    
    p = 避开_空格(p);
    if (*p == '[') {
        p = 避开_空格(p + 1);
        p2 = 匹配_寄存器(p, 16, &寄存);
        if (p2 != NULL) {
            p = 避开_空格(p2);
            if (*p == ']') {
                p++;
                if (寄存 == 3) {   /* BX */
                    *位数 = 0x07;
                } else if (寄存 == 5) {  /* BP */
                    *位数 = 0x46;
                    指令_偏移 = 0;
                    指令_偏移_宽度 = 1;
                } else if (寄存 == 6) {  /* SI */
                    *位数 = 0x04;
                } else if (寄存 == 7) {  /* DI */
                    *位数 = 0x05;
                } else {    /* Not valid */
                    return NULL;
                }
            } else if (*p == '+' || *p == '-') {
                if (*p == '+') {
                    p = 避开_空格(p + 1);
                    p2 = 匹配_寄存器(p, 16, &寄存2);
                } else {
                    p2 = NULL;
                }
                if (p2 != NULL) {
                    if ((寄存 == 3 && 寄存2 == 6) || (寄存 == 6 && 寄存2 == 3)) {   /* BX+SI / SI+BX */
                        *位数 = 0x00;
                    } else if ((寄存 == 3 && 寄存2 == 7) || (寄存 == 7 && 寄存2 == 3)) {    /* BX+DI / DI+BX */
                        *位数 = 0x01;
                    } else if ((寄存 == 5 && 寄存2 == 6) || (寄存 == 6 && 寄存2 == 5)) {    /* BP+SI / SI+BP */
                        *位数 = 0x02;
                    } else if ((寄存 == 5 && 寄存2 == 7) || (寄存 == 7 && 寄存2 == 5)) {    /* BP+DI / DI+BP */
                        *位数 = 0x03;
                    } else {    /* Not valid */
                        return NULL;
                    }
                    p = 避开_空格(p2);
                    if (*p == ']') {
                        p++;
                    } else if (*p == '+' || *p == '-') {
                        p2 = 匹配_表达式(p, &指令_偏移);
                        if (p2 == NULL)
                            return NULL;
                        p = 避开_空格(p2);
                        if (*p != ']')
                            return NULL;
                        p++;
                        if (指令_偏移 >= -0x80 && 指令_偏移 <= 0x7f) {
                            指令_偏移_宽度 = 1;
                            *位数 |= 0x40;
                        } else {
                            指令_偏移_宽度 = 2;
                            *位数 |= 0x80;
                        }
                    } else {    /* Syntax error */
                        return NULL;
                    }
                } else {
                    if (寄存 == 3) {   /* BX */
                        *位数 = 0x07;
                    } else if (寄存 == 5) {  /* BP */
                        *位数 = 0x06;
                    } else if (寄存 == 6) {  /* SI */
                        *位数 = 0x04;
                    } else if (寄存 == 7) {  /* DI */
                        *位数 = 0x05;
                    } else {    /* Not valid */
                        return NULL;
                    }
                    p2 = 匹配_表达式(p, &指令_偏移);
                    if (p2 == NULL)
                        return NULL;
                    p = 避开_空格(p2);
                    if (*p != ']')
                        return NULL;
                    p++;
                    if (指令_偏移 >= -0x80 && 指令_偏移 <= 0x7f) {
                        指令_偏移_宽度 = 1;
                        *位数 |= 0x40;
                    } else {
                        指令_偏移_宽度 = 2;
                        *位数 |= 0x80;
                    }
                }
            } else {    /* Syntax error */
                return NULL;
            }
        } else {    /* No valid register, try expression (absolute addressing) */
            p2 = 匹配_表达式(p, &指令_偏移);
            if (p2 == NULL)
                return NULL;
            p = 避开_空格(p2);
            if (*p != ']')
                return NULL;
            p++;
            *位数 = 0x06;
            指令_偏移_宽度 = 2;
        }
    } else {    /* Register */
        p = 匹配_寄存器(p, 宽度, &寄存);
        if (p == NULL)
            return NULL;
        *位数 = 0xc0 | 寄存;
    }
    return p;
}

/*
 ** Check for a 标号 character
 */
int 是否标号(c)
    int c;
{
    return isalpha(c) || isdigit(c) || c == '_' || c == '.';
}

/*
 ** Match register
 */
char *匹配_寄存器(p, 宽度, 值)
    char *p;
    int 宽度;
    int *值;
{
    char 寄存[3];
    int c;
    
    p = 避开_空格(p);
    if (!isalpha(p[0]) || !isalpha(p[1]) || 是否标号(p[2]))
        return NULL;
    寄存[0] = p[0];
    寄存[1] = p[1];
    寄存[2] = '\0';
    if (宽度 == 8) {   /* 8-bit */
        for (c = 0; c < 8; c++)
            if (strcmp(寄存, 寄存器1[c]) == 0)
                break;
        if (c < 8) {
            *值 = c;
            return p + 2;
        }
    } else {    /* 16-bit */
        for (c = 0; c < 8; c++)
            if (strcmp(寄存, 寄存器1[c + 8]) == 0)
                break;
        if (c < 8) {
            *值 = c;
            return p + 2;
        }
    }
    return NULL;
}

/*
 ** Read character for string or character literal
 */
char *读_字符(p, c)
    char *p;
    int *c;
{
    if (*p == '\\') {
        p++;
        if (*p == '\'') {
            *c = '\'';
            p++;
        } else if (*p == '\"') {
            *c = '"';
            p++;
        } else if (*p == '\\') {
            *c = '\\';
            p++;
        } else if (*p == 'a') {
            *c = 0x07;
            p++;
        } else if (*p == 'b') {
            *c = 0x08;
            p++;
        } else if (*p == 't') {
            *c = 0x09;
            p++;
        } else if (*p == 'n') {
            *c = 0x0a;
            p++;
        } else if (*p == 'v') {
            *c = 0x0b;
            p++;
        } else if (*p == 'f') {
            *c = 0x0c;
            p++;
        } else if (*p == 'r') {
            *c = 0x0d;
            p++;
        } else if (*p == 'e') {
            *c = 0x1b;
            p++;
        } else if (*p >= '0' && *p <= '7') {
            *c = 0;
            while (*p >= '0' && *p <= '7') {
                *c = *c * 8 + (*p - '0');
                p++;
            }
        } else {
            p--;
            消息(1, "字符串内的错误转义");
        }
    } else {
        *c = *p;
        p++;
    }
    return p;
}

/*
 ** Match expression (top tier)
 */
char *匹配_表达式(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级1(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '|') {    /* Binary OR */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级1(p, 值);
            if (p == NULL)
                return NULL;
            *值 |= 值1;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *匹配_表达式_等级1(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级2(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '^') {    /* Binary XOR */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级2(p, 值);
            if (p == NULL)
                return NULL;
            *值 ^= 值1;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *匹配_表达式_等级2(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级3(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '&') {    /* Binary AND */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级3(p, 值);
            if (p == NULL)
                return NULL;
            *值 &= 值1;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *匹配_表达式_等级3(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级4(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '<' && p[1] == '<') { /* Shift to 左 */
            p += 2;
            值1 = *值;
            p = 匹配_表达式_等级4(p, 值);
            if (p == NULL)
                return NULL;
            *值 = 值1 << *值;
        } else if (*p == '>' && p[1] == '>') {  /* Shift to 右 */
            p += 2;
            值1 = *值;
            p = 匹配_表达式_等级4(p, 值);
            if (p == NULL)
                return NULL;
            *值 = 值1 >> *值;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *匹配_表达式_等级4(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级5(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '+') {    /* Add operator */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级5(p, 值);
            if (p == NULL)
                return NULL;
            *值 = 值1 + *值;
        } else if (*p == '-') { /* Subtract operator */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级5(p, 值);
            if (p == NULL)
                return NULL;
            *值 = 值1 - *值;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression
 */
char *匹配_表达式_等级5(p, 值)
    char *p;
    int *值;
{
    int 值1;
    
    p = 匹配_表达式_等级6(p, 值);
    if (p == NULL)
        return NULL;
    while (1) {
        p = 避开_空格(p);
        if (*p == '*') {    /* Multiply operator */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级6(p, 值);
            if (p == NULL)
                return NULL;
            *值 = 值1 * *值;
        } else if (*p == '/') { /* Division operator */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级6(p, 值);
            if (p == NULL)
                return NULL;
            if (*值 == 0) {
                if (汇编器_步骤 == 2)
                    消息(1, "被零除");
                *值 = 1;
            }
            *值 = (unsigned) 值1 / *值;
        } else if (*p == '%') { /* Modulo operator */
            p++;
            值1 = *值;
            p = 匹配_表达式_等级6(p, 值);
            if (p == NULL)
                return NULL;
            if (*值 == 0) {
                if (汇编器_步骤 == 2)
                    消息(1, "以零为模数");
                *值 = 1;
            }
            *值 = 值1 % *值;
        } else {
            return p;
        }
    }
}

/*
 ** Match expression (bottom tier)
 */
char *匹配_表达式_等级6(p, 值)
    char *p;
    int *值;
{
    int 号码;
    int c;
    char *p2;
    struct 标号 *标号;
    
    p = 避开_空格(p);
    if (*p == '(') {    /* Handle parenthesized expressions */
        p++;
        p = 匹配_表达式(p, 值);
        if (p == NULL)
            return NULL;
        p = 避开_空格(p);
        if (*p != ')')
            return NULL;
        p++;
        return p;
    }
    if (*p == '-') {    /* Simple negation */
        p++;
        p = 匹配_表达式_等级6(p, 值);
        if (p == NULL)
            return NULL;
        *值 = -*值;
        return p;
    }
    if (*p == '+') {    /* Unary */
        p++;
        p = 匹配_表达式_等级6(p, 值);
        if (p == NULL)
            return NULL;
        return p;
    }
    if (p[0] == '0' && tolower(p[1]) == 'b') {  /* Binary */
        p += 2;
        号码 = 0;
        while (p[0] == '0' || p[0] == '1' || p[0] == '_') {
            if (p[0] != '_') {
                号码 <<= 1;
                if (p[0] == '1')
                    号码 |= 1;
            }
            p++;
        }
        *值 = 号码;
        return p;
    }
    if (p[0] == '0' && tolower(p[1]) == 'x' && isxdigit(p[2])) {	/* Hexadecimal */
        p += 2;
        号码 = 0;
        while (isxdigit(p[0])) {
            c = toupper(p[0]);
            c = c - '0';
            if (c > 9)
                c -= 7;
            号码 = (号码 << 4) | c;
            p++;
        }
        *值 = 号码;
        return p;
    }
    if (p[0] == '$' && isdigit(p[1])) {	/* Hexadecimal */
        /* This is nasm syntax, notice no letter is allowed after $ */
        /* So it's preferrable to use prefix 0x for hexadecimal */
        p += 1;
        号码 = 0;
        while (isxdigit(p[0])) {
            c = toupper(p[0]);
            c = c - '0';
            if (c > 9)
                c -= 7;
            号码 = (号码 << 4) | c;
            p++;
        }
        *值 = 号码;
        return p;
    }
    if (p[0] == '\'') { /* Character constant */
        p++;
        p = 读_字符(p, 值);
        if (p[0] != '\'') {
            消息(1, "遗漏缩写符号");
        } else {
            p++;
        }
        return p;
    }
    if (isdigit(*p)) {   /* Decimal */
        号码 = 0;
        while (isdigit(p[0])) {
            c = p[0] - '0';
            号码 = 号码 * 10 + c;
            p++;
        }
        *值 = 号码;
        return p;
    }
    if (*p == '$' && p[1] == '$') { /* Start 地址 */
        p += 2;
        *值 = 开始_地址;
        return p;
    }
    if (*p == '$') { /* Current 地址 */
        p++;
        *值 = 地址;
        return p;
    }
    if (isalpha(*p) || *p == '_' || *p == '.') { /* Label */
        if (*p == '.') {
            strcpy(表达式_名称, 全局_标号);
            p2 = 表达式_名称;
            while (*p2)
                p2++;
        } else {
            p2 = 表达式_名称;
        }
        while (isalpha(*p) || isdigit(*p) || *p == '_' || *p == '.')
            *p2++ = *p++;
        *p2 = '\0';
        for (c = 0; c < 16; c++)
            if (strcmp(表达式_名称, 寄存器1[c]) == 0)
                return NULL;
        标号 = 找_标号(表达式_名称);
        if (标号 == NULL) {
            *值 = 0;
            未定义的++;
            strcpy(未定义的_名称, 表达式_名称);
        } else {
            *值 = 标号->值;
        }
        return p;
    }
    return NULL;
}

/*
 ** Emit one 字节 to 输出
 */
void 发射_字节(int 字节)
{
    char buf[1];
    
    if (汇编器_步骤 == 2) {
        if (g != NULL && g < 生成的 + sizeof(生成的))
            *g++ = 字节;
        buf[0] = 字节;
        /* Cannot use fputc because DeSmet C expands to CR LF */
        fwrite(buf, 1, 1, 输出);
        字节++;
    }
    地址++;
}

/*
 ** Search for a 匹配 with instruction
 */
char *匹配(p, 模式, 解码)
    char *p;
    char *模式;
    char *解码;
{
    char *p2;
    int c;
    int d;
    int bit;
    int qualifier;
    char *基本;
    
    未定义的 = 0;
    while (*模式) {
/*        fputc(*模式, stdout);*/
        if (*模式 == '%') {	/* Special */
            模式++;
            if (*模式 == 'd') {  /* Addressing */
                模式++;
                qualifier = 0;
                if (memcmp(p, "WORD", 4) == 0 && !isalpha(p[4])) {
                    p = 避开_空格(p + 4);
                    if (*p != '[')
                        return NULL;
                    qualifier = 16;
                } else if (memcmp(p, "BYTE", 4) == 0 && !isalpha(p[4])) {
                    p = 避开_空格(p + 4);
                    if (*p != '[')
                        return NULL;
                    qualifier = 8;
                }
                if (*模式 == 'w') {
                    模式++;
                    if (qualifier != 16 && 匹配_寄存器(p, 16, &d) == 0)
                        return NULL;
                } else if (*模式 == 'b') {
                    模式++;
                    if (qualifier != 8 && 匹配_寄存器(p, 8, &d) == 0)
                        return NULL;
                } else {
                    if (qualifier == 8 && *模式 != '8')
                        return NULL;
                    if (qualifier == 16 && *模式 != '1')
                        return NULL;
                }
                if (*模式 == '8') {
                    模式++;
                    p2 = 匹配_地址的(p, 8);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else if (*模式 == '1' && 模式[1] == '6') {
                    模式 += 2;
                    p2 = 匹配_地址的(p, 16);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else {
                    return NULL;
                }
            } else if (*模式 == 'r') {   /* Register */
                模式++;
                if (*模式 == '8') {
                    模式++;
                    p2 = 匹配_寄存器(p, 8, &指令_寄存器);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else if (*模式 == '1' && 模式[1] == '6') {
                    模式 += 2;
                    p2 = 匹配_寄存器(p, 16, &指令_寄存器);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else {
                    return NULL;
                }
            } else if (*模式 == 'i') {   /* Immediate */
                模式++;
                if (*模式 == '8') {
                    模式++;
                    p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else if (*模式 == '1' && 模式[1] == '6') {
                    模式 += 2;
                    p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else {
                    return NULL;
                }
            } else if (*模式 == 'a') {   /* Address for jump */
                模式++;
                if (*模式 == '8') {
                    模式++;
                    p = 避开_空格(p);
                    qualifier = 0;
                    if (memcmp(p, "SHORT", 5) == 0 && isspace(p[5])) {
                        p += 5;
                        qualifier = 1;
                    }
                    p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    if (qualifier == 0) {
                        c = 指令_值 - (地址 + 2);
                        if (未定义的 == 0 && (c < -128 || c > 127) && memcmp(解码, "xeb", 3) == 0)
                            return NULL;
                    }
                    p = p2;
                } else if (*模式 == '1' && 模式[1] == '6') {
                    模式 += 2;
                    p = 避开_空格(p);
                    if (memcmp(p, "SHORT", 5) == 0 && isspace(p[5]))
                        p2 = NULL;
                    else
                        p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else {
                    return NULL;
                }
            } else if (*模式 == 's') {   /* Signed immediate */
                模式++;
                if (*模式 == '8') {
                    模式++;
                    p = 避开_空格(p);
                    qualifier = 0;
                    if (memcmp(p, "BYTE", 4) == 0 && isspace(p[4])) {
                        p += 4;
                        qualifier = 1;
                    }
                    p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    if (qualifier == 0) {
                        c = 指令_值;
                        if (未定义的 != 0)
                            return NULL;
                        if (未定义的 == 0 && (c < -128 || c > 127))
                            return NULL;
                    }
                    p = p2;
                } else {
                    return NULL;
                }
            } else if (*模式 == 'f') {   /* FAR pointer */
                模式++;
                if (*模式 == '3' && 模式[1] == '2') {
                    模式 += 2;
                    p2 = 匹配_表达式(p, &指令_值2);
                    if (p2 == NULL)
                        return NULL;
                    if (*p2 != ':')
                        return NULL;
                    p = p2 + 1;
                    p2 = 匹配_表达式(p, &指令_值);
                    if (p2 == NULL)
                        return NULL;
                    p = p2;
                } else {
                    return NULL;
                }
            } else {
                return NULL;
            }
            continue;
        }
        if (toupper(*p) != *模式)
            return NULL;
        p++;
        if (*模式 == ',')    /* Allow spaces after comma */
            p = 避开_空格(p);
        模式++;
    }

    /*
     ** Instruction properly matched, now generate binary
     */
    基本 = 解码;
    while (*解码) {
        解码 = 避开_空格(解码);
        if (解码[0] == 'x') { /* Byte */
            c = toupper(解码[1]);
            c -= '0';
            if (c > 9)
                c -= 7;
            d = toupper(解码[2]);
            d -= '0';
            if (d > 9)
                d -= 7;
            c = (c << 4) | d;
            发射_字节(c);
            解码 += 3;
        } else {    /* Binary */
            if (*解码 == 'b')
                解码++;
            bit = 0;
            c = 0;
            d = 0;
            while (bit < 8) {
                if (解码[0] == '0') { /* Zero */
                    解码++;
                    bit++;
                } else if (解码[0] == '1') {  /* One */
                    c |= 0x80 >> bit;
                    解码++;
                    bit++;
                } else if (解码[0] == '%') {  /* Special */
                    解码++;
                    if (解码[0] == 'r') { /* Register field */
                        解码++;
                        if (解码[0] == '8')
                            解码++;
                        else if (解码[0] == '1' && 解码[1] == '6')
                            解码 += 2;
                        c |= 指令_寄存器 << (5 - bit);
                        bit += 3;
                    } else if (解码[0] == 'd') {  /* Addressing field */
                        if (解码[1] == '8')
                            解码 += 2;
                        else
                            解码 += 3;
                        if (bit == 0) {
                            c |= 指令_地址的 & 0xc0;
                            bit += 2;
                        } else {
                            c |= 指令_地址的 & 0x07;
                            bit += 3;
                            d = 1;
                        }
                    } else if (解码[0] == 'i' || 解码[0] == 's') {
                        if (解码[1] == '8') {
                            解码 += 2;
                            c = 指令_值;
                            break;
                        } else {
                            解码 += 3;
                            c = 指令_值;
                            指令_偏移 = 指令_值 >> 8;
                            指令_偏移_宽度 = 1;
                            d = 1;
                            break;
                        }
                    } else if (解码[0] == 'a') {
                        if (解码[1] == '8') {
                            解码 += 2;
                            c = 指令_值 - (地址 + 1);
                            if (汇编器_步骤 == 2 && (c < -128 || c > 127))
                                消息(1, "短跳太长");
                            break;
                        } else {
                            解码 += 3;
                            c = 指令_值 - (地址 + 2);
                            指令_偏移 = c >> 8;
                            指令_偏移_宽度 = 1;
                            d = 1;
                            break;
                        }
                    } else if (解码[0] == 'f') {
                        解码 += 3;
                        发射_字节(指令_值);
                        c = 指令_值 >> 8;
                        指令_偏移 = 指令_值2;
                        指令_偏移_宽度 = 2;
                        d = 1;
                        break;
                    } else {
                        fprintf(stderr, "解码: 内部错误 2\n");
                    }
                } else {
                    fprintf(stderr, "解码: 内部错误 1 (%s)\n", 基本);
                    break;
                }
            }
            发射_字节(c);
            if (d == 1) {
                d = 0;
                if (指令_偏移_宽度 >= 1) {
                    发射_字节(指令_偏移);
                }
                if (指令_偏移_宽度 >= 2) {
                    发射_字节(指令_偏移 >> 8);
                }
            }
        }
    }
    if (汇编器_步骤 == 2) {
        if (未定义的) {
            fprintf(stderr, "错误: 未定义的标号 '%s' 在行 %d\n", 未定义的_名称, 行号);
        }
    }
    return p;
}

/*
 ** Make a string lowercase
 */
void 到_小写(p)
    char *p;
{
    while (*p) {
        *p = tolower(*p);
        p++;
    }
}

/*
 ** Separate a portion of entry up to the first space
 */
void 分隔(void)
{
    char *p2;
    
    while (*p && isspace(*p))
        p++;
    前一个_针 = p;
    p2 = 部分;
    while (*p && !isspace(*p) && *p != ';')
        *p2++ = *p++;
    *p2 = '\0';
    while (*p && isspace(*p))
        p++;
}

/*
 ** Check for end of 行
 */
void 检查_终(p)
    char *p;
{
    p = 避开_空格(p);
    if (*p && *p != ';') {
        fprintf(stderr, "错误：第 %d 行末尾的多余字符\n", 行号);
        错误的++;
    }
}

/*
 ** Generate a 消息
 */
void 消息(error, 消息)
    int error;
    char *消息;
{
    if (error) {
        fprintf(stderr, "错误: %s 在行 %d\n", 消息, 行号);
        错误的++;
    } else {
        fprintf(stderr, "警告: %s 在行 %d\n", 消息, 行号);
        警告的++;
    }
    if (列表列出 != NULL) {
        if (error) {
            fprintf(列表列出, "错误: %s 在行 %d\n", 消息, 行号);
        } else {
            fprintf(列表列出, "警告: %s 在行 %d\n", 消息, 行号);
        }
    }
}

/*
 ** Process an instruction
 */
void 加工_指令()
{
    char *p2 = NULL;
    char *p3;
    int c;
    
    if (strcmp(部分, "DB") == 0) {  /* Define 字节 */
        while (1) {
            p = 避开_空格(p);
            if (*p == '"') {    /* ASCII text */
                p++;
                while (*p && *p != '"') {
                    p = 读_字符(p, &c);
                    发射_字节(c);
                }
                if (*p) {
                    p++;
                } else {
                    fprintf(stderr, "错误：第 %d 行未终止的字符串\n", 行号);
                }
            } else {
                p2 = 匹配_表达式(p, &指令_值);
                if (p2 == NULL) {
                    fprintf(stderr, "错误: 不良表达式在行 %d\n", 行号);
                    break;
                }
                发射_字节(指令_值);
                p = p2;
            }
            p = 避开_空格(p);
            if (*p == ',') {
                p++;
                continue;
            }
            检查_终(p);
            break;
        }
        return;
    }
    if (strcmp(部分, "DW") == 0) {  /* Define word */
        while (1) {
            p2 = 匹配_表达式(p, &指令_值);
            if (p2 == NULL) {
                fprintf(stderr, "错误: 不良表达式在行 %d\n", 行号);
                break;
            }
            发射_字节(指令_值);
            发射_字节(指令_值 >> 8);
            p = 避开_空格(p2);
            if (*p == ',') {
                p++;
                continue;
            }
            检查_终(p);
            break;
        }
        return;
    }
    while (部分[0]) {   /* Match against instruction set */
        c = 0;
        while (指令_集合[c] != NULL) {
            if (strcmp(部分, 指令_集合[c]) == 0) {
                p2 = 指令_集合[c];
                while (*p2++) ;
                p3 = p2;
                while (*p3++) ;
                
                p2 = 匹配(p, p2, p3);
                if (p2 != NULL) {
                    p = p2;
                    break;
                }
            }
            c++;
        }
        if (指令_集合[c] == NULL) {
            char m[25 + 最大_大小];
            
            sprintf(m, "未定义指令“%s %s”", 部分, p);
            消息(1, m);
            break;
        } else {
            p = p2;
            分隔();
        }
    }
}

/*
 ** Reset current 地址.
 ** Called anytime the assembler needs to generate code.
 */
void 复位_地址()
{
    地址 = 开始_地址 = 默认_开始_地址;
}

/*
 ** Include a binary file
 */
void 包括二进制(文名称)
    char *文名称;
{
    FILE *输入;
    char buf[256];
    int 大小;
    int i;
    
    输入 = fopen(文名称, "r");
    if (输入 == NULL) {
        sprintf(buf, "错误：无法打开“%s”进行输入", 文名称);
        消息(1, buf);
        return;
    }
    
    while (大小 = fread(buf, 1, 30, 输入)) {
        for (i = 0; i < 大小; i++) {
            发射_字节(buf[i]);
        }
    }
    
    fclose(输入);
}

/*
 ** Do an assembler step
 */
void 搞_汇编(文名称)
    char *文名称;
{
    FILE *输入;
    char *p2;
    char *p3;
    char *p文名称;
    int 等级;
    int 避开_等级;
    int 次数;
    int 基本;
    int p行;
    int 包括;
    int 对齐;

    输入 = fopen(文名称, "r");
    if (输入 == NULL) {
        fprintf(stderr, "错误：无法打开“%s”进行输入\n", 文名称);
        错误的++;
        return;
    }

    p文名称 = 输入_文件名;
    p行 = 行号;
    输入_文件名 = 文名称;
    等级 = 0;
    避开_等级 = -1;
    全局_标号[0] = '\0';
    行号 = 0;
    基本 = 0;
    while (fgets(行, sizeof(行), 输入)) {
        行号++;
        p = 行;
        while (*p) {
            if (*p == '\'' && *(p - 1) != '\\') {
                p++;
                while (*p && *p != '\'' && *(p - 1) != '\\')
                    p++;
            } else if (*p == '"' && *(p - 1) != '\\') {
                p++;
                while (*p && *p != '"' && *(p - 1) != '\\')
                    p++;
            } else if (*p == ';') {
                while (*p)
                    p++;
                break;
            }
            *p = toupper(*p);
            p++;
        }
        if (p > 行 && *(p - 1) == '\n')
            p--;
        *p = '\0';

        基本 = 地址;
        g = 生成的;
        包括 = 0;

        while (1) {
            p = 行;
            分隔();
            if (部分[0] == '\0' && (*p == '\0' || *p == ';'))    /* Empty 行 */
                break;
            if (部分[0] != '\0' && 部分[strlen(部分) - 1] == ':') {	/* Label */
                部分[strlen(部分) - 1] = '\0';
                if (部分[0] == '.') {
                    strcpy(名称, 全局_标号);
                    strcat(名称, 部分);
                } else {
                    strcpy(名称, 部分);
                    strcpy(全局_标号, 名称);
                }
                分隔();
                if (避开_等级 == -1 || 等级 < 避开_等级) {
                    if (strcmp(部分, "EQU") == 0) {
                        p2 = 匹配_表达式(p, &指令_值);
                        if (p2 == NULL) {
                            消息(1, "不良表达式");
                        } else {
                            if (汇编器_步骤 == 1) {
                                if (找_标号(名称)) {
                                    char m[18 + 最大_大小];
                                    
                                    sprintf(m, "重新定义标号“%s”", 名称);
                                    消息(1, m);
                                } else {
                                    最后_标号 = 定义_标号(名称, 指令_值);
                                }
                            } else {
                                最后_标号 = 找_标号(名称);
                                if (最后_标号 == NULL) {
                                    char m[33 + 最大_大小];
                                    
                                    sprintf(m, "不一致，未找到标号“%s”", 名称);
                                    消息(1, m);
                                } else {
                                    if (最后_标号->值 != 指令_值) {
#ifdef 调试
/*                                        fprintf(stderr, "Woops: 标号 '%s' changed 值 from %04x to %04x\n", 最后_标号->名称, 最后_标号->值, 指令_值);*/
#endif
                                        改变 = 1;
                                    }
                                    最后_标号->值 = 指令_值;
                                }
                            }
                            检查_终(p2);
                        }
                        break;
                    }
                    if (第一_时间 == 1) {
#ifdef 调试
                        /*                        fprintf(stderr, "First time '%s' 在 行 %d\n", 行, 行号);*/
#endif
                        第一_时间 = 0;
                        复位_地址();
                    }
                    if (汇编器_步骤 == 1) {
                        if (找_标号(名称)) {
                            char m[18 + 最大_大小];
                            
                            sprintf(m, "重新定义标号“%s”", 名称);
                            消息(1, m);
                        } else {
                            最后_标号 = 定义_标号(名称, 地址);
                        }
                    } else {
                        最后_标号 = 找_标号(名称);
                        if (最后_标号 == NULL) {
                            char m[33 + 最大_大小];
                            
                            sprintf(m, "不一致，未找到标号“%s”", 名称);
                            消息(1, m);
                        } else {
                            if (最后_标号->值 != 地址) {
#ifdef 调试
/*                                fprintf(stderr, "Woops: 标号 '%s' changed 值 from %04x to %04x\n", 最后_标号->名称, 最后_标号->值, 地址);*/
#endif
                                改变 = 1;
                            }
                            最后_标号->值 = 地址;
                        }
                        
                    }
                }
            }
            if (strcmp(部分, "%IF") == 0) {
                等级++;
                if (避开_等级 != -1 && 等级 >= 避开_等级)
                    break;
                未定义的 = 0;
                p = 匹配_表达式(p, &指令_值);
                if (p == NULL) {
                    消息(1, "不良表达式");
                } else if (未定义的) {
                    消息(1, "未定义的标号");
                }
                if (指令_值 != 0) {
                    ;
                } else {
                    避开_等级 = 等级;
                }
                检查_终(p);
                break;
            }
            if (strcmp(部分, "%IFDEF") == 0) {
                等级++;
                if (避开_等级 != -1 && 等级 >= 避开_等级)
                    break;
                分隔();
                if (找_标号(部分) != NULL) {
                    ;
                } else {
                    避开_等级 = 等级;
                }
                检查_终(p);
                break;
            }
            if (strcmp(部分, "%IFNDEF") == 0) {
                等级++;
                if (避开_等级 != -1 && 等级 >= 避开_等级)
                    break;
                分隔();
                if (找_标号(部分) == NULL) {
                    ;
                } else {
                    避开_等级 = 等级;
                }
                检查_终(p);
                break;
            }
            if (strcmp(部分, "%ELSE") == 0) {
                if (避开_等级 != -1 && 等级 > 避开_等级)
                    break;
                if (避开_等级 == 等级) {
                    避开_等级 = -1;
                } else if (避开_等级 == -1) {
                    避开_等级 = 等级;
                }
                检查_终(p);
                break;
            }
            if (strcmp(部分, "%ENDIF") == 0) {
                if (避开_等级 == 等级)
                    避开_等级 = -1;
                等级--;
                检查_终(p);
                break;
            }
            if (避开_等级 != -1 && 等级 >= 避开_等级) {
#ifdef 调试
                /*fprintf(stderr, "Avoiding '%s'\n", 行);*/
#endif
                break;
            }
            if (strcmp(部分, "USE16") == 0) {
                break;
            }
            if (strcmp(部分, "CPU") == 0) {
                p = 避开_空格(p);
                if (memcmp(p, "8086", 4) != 0)
                    消息(1, "请求的处理器不受支持");
                break;
            }
            if (strcmp(部分, "%INCLUDE") == 0) {
                分隔();
                检查_终(p);
                if (部分[0] != '"' || 部分[strlen(部分) - 1] != '"') {
                    消息(1, "%include 上缺少引号");
                    break;
                }
                包括 = 1;
                break;
            }
            if (strcmp(部分, "INCBIN") == 0) {
                分隔();
                检查_终(p);
                if (部分[0] != '"' || 部分[strlen(部分) - 1] != '"') {
                    消息(1, "在包括二进制上缺少引号");
                    break;
                }
                包括 = 2;
                break;
            }
            if (strcmp(部分, "ORG") == 0) {
                p = 避开_空格(p);
                未定义的 = 0;
                p2 = 匹配_表达式(p, &指令_值);
                if (p2 == NULL) {
                    消息(1, "不良表达式");
                } else if (未定义的) {
                    消息(1, "不能使用未定义的标号");
                } else {
                    if (第一_时间 == 1) {
                        第一_时间 = 0;
                        地址 = 指令_值;
                        开始_地址 = 指令_值;
                        基本 = 地址;
                    } else {
                        if (指令_值 < 地址) {
                            消息(1, "后向地址");
                        } else {
                            while (地址 < 指令_值)
                                发射_字节(0);
                            
                        }
                    }
                    检查_终(p2);
                }
                break;
            }
            if (strcmp(部分, "ALIGN") == 0) {
                p = 避开_空格(p);
                未定义的 = 0;
                p2 = 匹配_表达式(p, &指令_值);
                if (p2 == NULL) {
                    消息(1, "不良表达式");
                } else if (未定义的) {
                    消息(1, "不能使用未定义的标号");
                } else {
                    对齐 = 地址 / 指令_值;
                    对齐 = 对齐 * 指令_值;
                    对齐 = 对齐 + 指令_值;
		    while (地址 < 对齐)
		        发射_字节(0x90);
                    检查_终(p2);
                }
                break;
            }
            if (第一_时间 == 1) {
#ifdef 调试
                /*fprintf(stderr, "First time '%s' 在 行 %d\n", 行, 行号);*/
#endif
                第一_时间 = 0;
                复位_地址();
            }
            次数 = 1;
            if (strcmp(部分, "TIMES") == 0) {
                未定义的 = 0;
                p2 = 匹配_表达式(p, &指令_值);
                if (p2 == NULL) {
                    消息(1, "不良表达式");
                    break;
                }
                if (未定义的) {
                    消息(1, "非常量表达式");
                    break;
                }
                次数 = 指令_值;
                p = p2;
                分隔();
            }
            基本 = 地址;
            g = 生成的;
            p3 = 前一个_针;
            while (次数) {
                p = p3;
                分隔();
                加工_指令();
                次数--;
            }
            break;
        }
        if (汇编器_步骤 == 2 && 列表列出 != NULL) {
            if (第一_时间)
                fprintf(列表列出, "      ");
            else
                fprintf(列表列出, "%04X  ", 基本);
            p = 生成的;
            while (p < g) {
                fprintf(列表列出, "%02X", *p++ & 255);
            }
            while (p < 生成的 + sizeof(生成的)) {
                fprintf(列表列出, "  ");
                p++;
            }
            fprintf(列表列出, "  %05d %s\n", 行号, 行);
        }
        if (包括 == 1) {
            部分[strlen(部分) - 1] = '\0';
            搞_汇编(部分 + 1);
        }
        if (包括 == 2) {
            部分[strlen(部分) - 1] = '\0';
            包括二进制(部分 + 1);
        }
    }
    fclose(输入);
    行号 = p行;
    输入_文件名 = p文名称;
}

/*
 ** Main program
 */
int main(实参计数, 实参值)
    int 实参计数;
    char *实参值[];
{
    int c;
    int d;
    char *p;
    char *ifname;
    
    /*
     ** If ran without arguments then show usage
     */
    if (实参计数 == 1) {
        fprintf(stderr, "与 nasm 句法兼容的汇编器\n");
        fprintf(stderr, "典型用法:\n");
        fprintf(stderr, "小汇编(8位16位) -f bin 输入.asm -o 输入.bin\n");
        exit(1);
    }
    
    /*
     ** Start to collect arguments
     */
    ifname = NULL;
    输出_文件名 = NULL;
    列表列出文件名 = NULL;
    默认_开始_地址 = 0;
    c = 1;
    while (c < 实参计数) {
        if (实参值[c][0] == '-') {    /* All arguments start with dash */
            d = tolower(实参值[c][1]);
            if (d == 'f') { /* Format */
                c++;
                if (c >= 实参计数) {
                    fprintf(stderr, "错误：-f 没有实参\n");
                    exit(1);
                } else {
                    到_小写(实参值[c]);
                    if (strcmp(实参值[c], "bin") == 0) {
                        默认_开始_地址 = 0;
                    } else if (strcmp(实参值[c], "com") == 0) {
                        默认_开始_地址 = 0x0100;
                    } else {
                        fprintf(stderr, "错误：-f 仅支持 'bin'、'com'（它是 '%s'）\n", 实参值[c]);
                        exit(1);
                    }
                    c++;
                }
            } else if (d == 'o') {  /* Object file 名称 */
                c++;
                if (c >= 实参计数) {
                    fprintf(stderr, "错误：-o 没有实参\n");
                    exit(1);
                } else if (输出_文件名 != NULL) {
                    fprintf(stderr, "错误：已经存在 -o 实参\n");
                    exit(1);
                } else {
                    输出_文件名 = 实参值[c];
                    c++;
                }
            } else if (d == 'l') {  /* Listing file 名称 */
                c++;
                if (c >= 实参计数) {
                    fprintf(stderr, "错误：-l 没有实参\n");
                    exit(1);
                } else if (列表列出文件名 != NULL) {
                    fprintf(stderr, "错误：已经存在 -l 实参\n");
                    exit(1);
                } else {
                    列表列出文件名 = 实参值[c];
                    c++;
                }
            } else if (d == 'd') {  /* Define 标号 */
                p = 实参值[c] + 2;
                while (*p && *p != '=') {
                    *p = toupper(*p);
                    p++;
                }
                if (*p == '=') {
                    *p++ = 0;
                    未定义的 = 0;
                    p = 匹配_表达式(p, &指令_值);
                    if (p == NULL) {
                        fprintf(stderr, "错误：标号定义错误\n");
                        exit(1);
                    } else if (未定义的) {
                        fprintf(stderr, "错误：非常量标号定义\n");
                        exit(1);
                    } else {
                        定义_标号(实参值[c] + 2, 指令_值);
                    }
                }
                c++;
            } else {
                fprintf(stderr, "错误：未知实参 %s\n", 实参值[c]);
                exit(1);
            }
        } else {
            if (ifname != NULL) {
                fprintf(stderr, "错误：多个输入文件名: %s\n", 实参值[c]);
                exit(1);
            } else {
                ifname = 实参值[c];
            }
            c++;
        }
    }
    
    if (ifname == NULL) {
        fprintf(stderr, "未提供输入文件名\n");
        exit(1);
    }
    
    /*
     ** Do first step of assembly
     */
    汇编器_步骤 = 1;
    第一_时间 = 1;
    搞_汇编(ifname);
    if (!错误的) {
        
        /*
         ** Do second step of assembly and generate final 输出
         */
        if (输出_文件名 == NULL) {
            fprintf(stderr, "未提供输出文件名\n");
            exit(1);
        }
        改变_号码 = 0;
        do {
            改变 = 0;
            if (列表列出文件名 != NULL) {
                列表列出 = fopen(列表列出文件名, "w");
                if (列表列出 == NULL) {
                    fprintf(stderr, "错误：无法打开 '%s' 作为列表文件\n", 输出_文件名);
                    exit(1);
                }
            }
            输出 = fopen(输出_文件名, "wb");
            if (输出 == NULL) {
                fprintf(stderr, "错误：无法打开 '%s' 作为输出文件\n", 输出_文件名);
                exit(1);
            }
            汇编器_步骤 = 2;
            第一_时间 = 1;
            搞_汇编(ifname);
            
            if (列表列出 != NULL && 改变 == 0) {
                fprintf(列表列出, "\n%05d 发现错误\n", 错误的);
                fprintf(列表列出, "%05d 发现警告\n\n", 警告的);
                fprintf(列表列出, "%05d 程序字节\n\n", 字节);
                if (标号_列表 != NULL) {
                    fprintf(列表列出, "%-20s 值/地址\n\n", "标号");
                    排序_标号(标号_列表);
                }
            }
            fclose(输出);
            if (列表列出文件名 != NULL)
                fclose(列表列出);
            if (改变) {
                改变_号码++;
                if (改变_号码 == 5) {
                    fprintf(stderr, "中止：无法稳定移动标号\n");
                    错误的++;
                }
            }
            if (错误的) {
                remove(输出_文件名);
                if (列表列出文件名 != NULL)
                    remove(列表列出文件名);
                exit(1);
            }
        } while (改变) ;

        exit(0);
    }

    exit(1);
}
