#include "chibicc.h"

typedef enum {
  文件_全无, 文件_C, 文件_汇编, 文件_对象, 文件_AR, 文件_DSO,
} 文件类型_;

字符串数组 包括_路径;
bool 选项_文件公共的 = true;
bool 选项_文件采摘;

static 文件类型_ 选项_x;
static 字符串数组 选项_包括;
static bool 选项_E;
static bool 选项_M;
static bool 选项_MD;
static bool 选项_MMD;
static bool 选项_MP;
static bool 选项_S;
static bool 选项_c;
static bool 选项_cc1;
static bool 选项_哈希_哈希_哈希;
static bool 选项_静态;
static bool 选项_共享的;
static char *选项_MF;
static char *选项_MT;
static char *选项_o;

static 字符串数组 链接器_额外_实参_短;
static 字符串数组 标准的_包括_路径;

char *基础_文件;
static char *输出_文件;

static 字符串数组 输入_路径;
static 字符串数组 临时文件;

static void 用法_小(int status) {
  fprintf(stderr, "chibicc [ -o <路径> ] <源文件位置>\n");
  exit(status);
}

static bool 拿走_实参(char *arg) {
  char *x[] = {
    "-o", "-I", "-idirafter", "-include", "-x", "-MF", "-MT", "-Xlinker",
  };

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)
    if (!strcmp(arg, x[i]))
      return true;
  return false;
}

static void 添加_默认_包括_路径(char *argv0) {
  // 我们期望安装 chibicc 特定的包括文件
  // to ./include relative to 实参值[0].
  串数组_推(&包括_路径, 格式化("%s/include", dirname(strdup(argv0))));

  // Add standard include paths.
  串数组_推(&包括_路径, "/usr/local/include");
  串数组_推(&包括_路径, "/usr/include/x86_64-linux-gnu");
  串数组_推(&包括_路径, "/usr/include");

  // Keep a copy of the standard include paths for -MMD option.
  for (int i = 0; i < 包括_路径.长度_短; i++)
    串数组_推(&标准的_包括_路径, 包括_路径.数据[i]);
}

static void 定义_小写(char *串_短) {
  char *eq = strchr(串_短, '=');
  if (eq)
    定义_宏(strndup(串_短, eq - 串_短), eq + 1);
  else
    定义_宏(串_短, "1");
}

static 文件类型_ 解析_选项_x(char *s) {
  if (!strcmp(s, "c"))
    return 文件_C;
  if (!strcmp(s, "assembler"))
    return 文件_汇编;
  if (!strcmp(s, "none"))
    return 文件_全无;
  错误("<command 行>: unknown argument for -x: %s", s);
}

static char *引号_制作文件(char *s) {
  char *缓冲_短 = calloc(1, strlen(s) * 2 + 1);

  for (int i = 0, j = 0; s[i]; i++) {
    switch (s[i]) {
    case '$':
      缓冲_短[j++] = '$';
      缓冲_短[j++] = '$';
      break;
    case '#':
      缓冲_短[j++] = '\\';
      缓冲_短[j++] = '#';
      break;
    case ' ':
    case '\t':
      for (int k = i - 1; k >= 0 && s[k] == '\\'; k--)
        缓冲_短[j++] = '\\';
      缓冲_短[j++] = '\\';
      缓冲_短[j++] = s[i];
      break;
    default:
      缓冲_短[j++] = s[i];
      break;
    }
  }
  return 缓冲_短;
}

static void 解析_实参_(int 实参计数, char **实参值) {
  // Make sure that all command 行 options that take an argument
  // have an argument.
  for (int i = 1; i < 实参计数; i++)
    if (拿走_实参(实参值[i]))
      if (!实参值[++i])
        用法_小(1);

  字符串数组 idirafter = {};

  for (int i = 1; i < 实参计数; i++) {
    if (!strcmp(实参值[i], "-###")) {
      选项_哈希_哈希_哈希 = true;
      continue;
    }

    if (!strcmp(实参值[i], "-cc1")) {
      选项_cc1 = true;
      continue;
    }

    if (!strcmp(实参值[i], "--help"))
      用法_小(0);

    if (!strcmp(实参值[i], "-o")) {
      选项_o = 实参值[++i];
      continue;
    }

    if (!strncmp(实参值[i], "-o", 2)) {
      选项_o = 实参值[i] + 2;
      continue;
    }

    if (!strcmp(实参值[i], "-S")) {
      选项_S = true;
      continue;
    }

    if (!strcmp(实参值[i], "-fcommon")) {
      选项_文件公共的 = true;
      continue;
    }

    if (!strcmp(实参值[i], "-fno-common")) {
      选项_文件公共的 = false;
      continue;
    }

    if (!strcmp(实参值[i], "-c")) {
      选项_c = true;
      continue;
    }

    if (!strcmp(实参值[i], "-E")) {
      选项_E = true;
      continue;
    }

    if (!strncmp(实参值[i], "-I", 2)) {
      串数组_推(&包括_路径, 实参值[i] + 2);
      continue;
    }

    if (!strcmp(实参值[i], "-D")) {
      定义_小写(实参值[++i]);
      continue;
    }

    if (!strncmp(实参值[i], "-D", 2)) {
      定义_小写(实参值[i] + 2);
      continue;
    }

    if (!strcmp(实参值[i], "-U")) {
      未定义_宏(实参值[++i]);
      continue;
    }

    if (!strncmp(实参值[i], "-U", 2)) {
      未定义_宏(实参值[i] + 2);
      continue;
    }

    if (!strcmp(实参值[i], "-include")) {
      串数组_推(&选项_包括, 实参值[++i]);
      continue;
    }

    if (!strcmp(实参值[i], "-x")) {
      选项_x = 解析_选项_x(实参值[++i]);
      continue;
    }

    if (!strncmp(实参值[i], "-x", 2)) {
      选项_x = 解析_选项_x(实参值[i] + 2);
      continue;
    }

    if (!strncmp(实参值[i], "-l", 2) || !strncmp(实参值[i], "-Wl,", 4)) {
      串数组_推(&输入_路径, 实参值[i]);
      continue;
    }

    if (!strcmp(实参值[i], "-Xlinker")) {
      串数组_推(&链接器_额外_实参_短, 实参值[++i]);
      continue;
    }

    if (!strcmp(实参值[i], "-s")) {
      串数组_推(&链接器_额外_实参_短, "-s");
      continue;
    }

    if (!strcmp(实参值[i], "-M")) {
      选项_M = true;
      continue;
    }

    if (!strcmp(实参值[i], "-MF")) {
      选项_MF = 实参值[++i];
      continue;
    }

    if (!strcmp(实参值[i], "-MP")) {
      选项_MP = true;
      continue;
    }

    if (!strcmp(实参值[i], "-MT")) {
      if (选项_MT == NULL)
        选项_MT = 实参值[++i];
      else
        选项_MT = 格式化("%s %s", 选项_MT, 实参值[++i]);
      continue;
    }

    if (!strcmp(实参值[i], "-MD")) {
      选项_MD = true;
      continue;
    }

    if (!strcmp(实参值[i], "-MQ")) {
      if (选项_MT == NULL)
        选项_MT = 引号_制作文件(实参值[++i]);
      else
        选项_MT = 格式化("%s %s", 选项_MT, 引号_制作文件(实参值[++i]));
      continue;
    }

    if (!strcmp(实参值[i], "-MMD")) {
      选项_MD = 选项_MMD = true;
      continue;
    }

    if (!strcmp(实参值[i], "-fpic") || !strcmp(实参值[i], "-fPIC")) {
      选项_文件采摘 = true;
      continue;
    }

    if (!strcmp(实参值[i], "-cc1-输入")) {
      基础_文件 = 实参值[++i];
      continue;
    }

    if (!strcmp(实参值[i], "-cc1-输出_")) {
      输出_文件 = 实参值[++i];
      continue;
    }

    if (!strcmp(实参值[i], "-idirafter")) {
      串数组_推(&idirafter, 实参值[i++]);
      continue;
    }

    if (!strcmp(实参值[i], "-static")) {
      选项_静态 = true;
      串数组_推(&链接器_额外_实参_短, "-static");
      continue;
    }

    if (!strcmp(实参值[i], "-shared")) {
      选项_共享的 = true;
      串数组_推(&链接器_额外_实参_短, "-shared");
      continue;
    }

    if (!strcmp(实参值[i], "-L")) {
      串数组_推(&链接器_额外_实参_短, "-L");
      串数组_推(&链接器_额外_实参_短, 实参值[++i]);
      continue;
    }

    if (!strncmp(实参值[i], "-L", 2)) {
      串数组_推(&链接器_额外_实参_短, "-L");
      串数组_推(&链接器_额外_实参_短, 实参值[i] + 2);
      continue;
    }

    if (!strcmp(实参值[i], "-hashmap-test")) {
      哈希映射_测试();
      exit(0);
    }

    // These options are ignored for now.
    if (!strncmp(实参值[i], "-O", 2) ||
        !strncmp(实参值[i], "-W", 2) ||
        !strncmp(实参值[i], "-g", 2) ||
        !strncmp(实参值[i], "-std=", 5) ||
        !strcmp(实参值[i], "-ffreestanding") ||
        !strcmp(实参值[i], "-fno-builtin") ||
        !strcmp(实参值[i], "-fno-omit-frame-pointer") ||
        !strcmp(实参值[i], "-fno-stack-protector") ||
        !strcmp(实参值[i], "-fno-strict-aliasing") ||
        !strcmp(实参值[i], "-m64") ||
        !strcmp(实参值[i], "-mno-red-zone") ||
        !strcmp(实参值[i], "-w"))
      continue;

    if (实参值[i][0] == '-' && 实参值[i][1] != '\0')
      错误("unknown argument: %s", 实参值[i]);

    串数组_推(&输入_路径, 实参值[i]);
  }

  for (int i = 0; i < idirafter.长度_短; i++)
    串数组_推(&包括_路径, idirafter.数据[i]);

  if (输入_路径.长度_短 == 0)
    错误("no 输入 files");

  // -E implies that the 输入 is the C macro language.
  if (选项_E)
    选项_x = 文件_C;
}

static FILE *打开_文件_小写(char *路径) {
  if (!路径 || strcmp(路径, "-") == 0)
    return stdout;

  FILE * 出_小写 = fopen(路径, "w");
  if (! 出_小写)
    错误("cannot open 输出_ 源文件位置: %s: %s", 路径, strerror(errno));
  return  出_小写;
}

static bool 以什么结束(char *p, char *q) {
  int len1 = strlen(p);
  int len2 = strlen(q);
  return (len1 >= len2) && !strcmp(p + len1 - len2, q);
}

// Replace 源文件位置 extension
static char *替换_扩展(char *tmpl, char *extn) {
  char *文件名 = basename(strdup(tmpl));
  char *dot = strrchr(文件名, '.');
  if (dot)
    *dot = '\0';
  return 格式化("%s%s", 文件名, extn);
}

static void 清除(void) {
  for (int i = 0; i < 临时文件.长度_短; i++)
    unlink(临时文件.数据[i]);
}

static char *创建_临时文件(void) {
  char *路径 = strdup("/tmp/chibicc-XXXXXX");
  int fd = mkstemp(路径);
  if (fd == -1)
    错误("mkstemp failed: %s", strerror(errno));
  close(fd);

  串数组_推(&临时文件, 路径);
  return 路径;
}

static void 跑_子进程(char **实参值) {
  // If -### is given, dump the subprocess's command 行.
  if (选项_哈希_哈希_哈希) {
    fprintf(stderr, "%s", 实参值[0]);
    for (int i = 1; 实参值[i]; i++)
      fprintf(stderr, " %s", 实参值[i]);
    fprintf(stderr, "\n");
  }

  if (fork() == 0) {
    // Child process. Run a 新的 command.
    execvp(实参值[0], 实参值);
    fprintf(stderr, "exec failed: %s: %s\n", 实参值[0], strerror(errno));
    _exit(1);
  }

  // Wait for the child process to finish.
  int status;
  while (wait(&status) > 0);
  if (status != 0)
    exit(1);
}

static void 跑_cc1(int 实参计数, char **实参值, char *输入, char *输出_) {
  char **实参_短 = calloc(实参计数 + 10, sizeof(char *));
  memcpy(实参_短, 实参值, 实参计数 * sizeof(char *));
  实参_短[实参计数++] = "-cc1";

  if (输入) {
    实参_短[实参计数++] = "-cc1-输入";
    实参_短[实参计数++] = 输入;
  }

  if (输出_) {
    实参_短[实参计数++] = "-cc1-输出_";
    实参_短[实参计数++] = 输出_;
  }

  跑_子进程(实参_短);
}

// Print tokens to stdout. Used for -E.
static void 打印_牌_(牌 *牌_短) {
  FILE * 出_小写 = 打开_文件_小写(选项_o ? 选项_o : "-");

  int 行 = 1;
  for (; 牌_短->种类 != 牌_缩_文件终; 牌_短 = 牌_短->下一个) {
    if (行 > 1 && 牌_短->位置_行开始)
      fprintf( 出_小写, "\n");
    if (牌_短->有_空格字符 && !牌_短->位置_行开始)
      fprintf( 出_小写, " ");
    fprintf( 出_小写, "%.*s", 牌_短->长度_短, 牌_短->位置_短);
    行++;
  }
  fprintf( 出_小写, "\n");
}

static bool 是否在_标准_包括_路径(char *路径) {
  for (int i = 0; i < 标准的_包括_路径.长度_短; i++) {
    char *dir = 标准的_包括_路径.数据[i];
    int 长度_短 = strlen(dir);
    if (strncmp(dir, 路径, 长度_短) == 0 && 路径[长度_短] == '/')
      return true;
  }
  return false;
}

// If -M options is given, the compiler write a list of 输入 files to
// stdout in a 格式化 that "make" command can read. This feature is
// 已用 to automate 源文件位置 dependency management.
static void 打印_依赖性(void) {
  char *路径;
  if (选项_MF)
    路径 = 选项_MF;
  else if (选项_MD)
    路径 = 替换_扩展(选项_o ? 选项_o : 基础_文件, ".d");
  else if (选项_o)
    路径 = 选项_o;
  else
    路径 = "-";

  FILE * 出_小写 = 打开_文件_小写(路径);
  if (选项_MT)
    fprintf( 出_小写, "%s:", 选项_MT);
  else
    fprintf( 出_小写, "%s:", 引号_制作文件(替换_扩展(基础_文件, ".o")));

  文件_大写 **files = 取_输入_文件();

  for (int i = 0; files[i]; i++) {
    if (选项_MMD && 是否在_标准_包括_路径(files[i]->名字))
      continue;
    fprintf( 出_小写, " \\\n  %s", files[i]->名字);
  }

  fprintf( 出_小写, "\n\n");

  if (选项_MP) {
    for (int i = 1; files[i]; i++) {
      if (选项_MMD && 是否在_标准_包括_路径(files[i]->名字))
        continue;
      fprintf( 出_小写, "%s:\n\n", 引号_制作文件(files[i]->名字));
    }
  }
}

static 牌 *必须_化为牌_文件(char *路径) {
  牌 *牌_短 = 化为牌_文件(路径);
  if (!牌_短)
    错误("%s: %s", 路径, strerror(errno));
  return 牌_短;
}

static 牌 *追加_牌_(牌 *tok1, 牌 *tok2) {
  if (!tok1 || tok1->种类 == 牌_缩_文件终)
    return tok2;

  牌 *t = tok1;
  while (t->下一个->种类 != 牌_缩_文件终)
    t = t->下一个;
  t->下一个 = tok2;
  return tok1;
}

static void cc1(void) {
  牌 *牌_短 = NULL;

  // Process -include option
  for (int i = 0; i < 选项_包括.长度_短; i++) {
    char *incl = 选项_包括.数据[i];

    char *路径;
    if (文件_存在(incl)) {
      路径 = incl;
    } else {
      路径 = 搜索_包括_路径(incl);
      if (!路径)
        错误("-include: %s: %s", incl, strerror(errno));
    }

    牌 *tok2 = 必须_化为牌_文件(路径);
    牌_短 = 追加_牌_(牌_短, tok2);
  }

  // Tokenize and 解析.
  牌 *tok2 = 必须_化为牌_文件(基础_文件);
  牌_短 = 追加_牌_(牌_短, tok2);
  牌_短 = 预处理(牌_短);

  // If -M or -MD are given, print 源文件位置 dependencies.
  if (选项_M || 选项_MD) {
    打印_依赖性();
    if (选项_M)
      return;
  }

  // If -E is given, print  出_小写 preprocessed C code as a result.
  if (选项_E) {
    打印_牌_(牌_短);
    return;
  }

  对象_短 *程序_短 = 解析(牌_短);

  // Open a temporary 输出_ buffer.
  char *缓冲_短;
  size_t 缓冲长度;
  FILE *输出_缓冲_短 = open_memstream(&缓冲_短, &缓冲长度);

  // Traverse the AST to emit assembly.
  代码生成(程序_短, 输出_缓冲_短);
  fclose(输出_缓冲_短);

  // Write the asembly text to a 源文件位置.
  FILE * 出_小写 = 打开_文件_小写(输出_文件);
  fwrite(缓冲_短, 缓冲长度, 1, out);
  fclose( 出_小写);
}

static void 汇编_(char *输入, char *输出_) {
  char *cmd[] = {"as", "-c", 输入, "-o", 输出_, NULL};
  跑_子进程(cmd);
}

static char *找_文件_(char *pattern) {
  char *路径 = NULL;
  glob_t 缓冲_短 = {};
  glob(pattern, 0, NULL, &缓冲_短);
  if (缓冲_短.gl_pathc > 0)
    路径 = strdup(缓冲_短.gl_pathv[缓冲_短.gl_pathc - 1]);
  globfree(&缓冲_短);
  return 路径;
}

// Returns true if a given 源文件位置 exists.
bool 文件_存在(char *路径) {
  struct stat st;
  return !stat(路径, &st);
}

static char *找_库路径(void) {
  if (文件_存在("/usr/lib/x86_64-linux-gnu/crti.o"))
    return "/usr/lib/x86_64-linux-gnu";
  if (文件_存在("/usr/lib64/crti.o"))
    return "/usr/lib64";
  错误("library 路径 is not found");
}

static char *找_gcc_库路径(void) {
  char *paths[] = {
    "/usr/lib/gcc/x86_64-linux-gnu/*/crtbegin.o",
    "/usr/lib/gcc/x86_64-pc-linux-gnu/*/crtbegin.o", // For Gentoo
    "/usr/lib/gcc/x86_64-redhat-linux/*/crtbegin.o", // For Fedora
  };

  for (int i = 0; i < sizeof(paths) / sizeof(*paths); i++) {
    char *路径 = 找_文件_(paths[i]);
    if (路径)
      return dirname(路径);
  }

  错误("gcc library 路径 is not found");
}

static void 跑_链接器_全(字符串数组 *输入_复, char *输出_) {
  字符串数组 数组_短 = {};

  串数组_推(&数组_短, "ld");
  串数组_推(&数组_短, "-o");
  串数组_推(&数组_短, 输出_);
  串数组_推(&数组_短, "-m");
  串数组_推(&数组_短, "elf_x86_64");

  char *库路径_ = 找_库路径();
  char *gcc_库路径_ = 找_gcc_库路径();

  if (选项_共享的) {
    串数组_推(&数组_短, 格式化("%s/crti.o", 库路径_));
    串数组_推(&数组_短, 格式化("%s/crtbeginS.o", gcc_库路径_));
  } else {
    串数组_推(&数组_短, 格式化("%s/crt1.o", 库路径_));
    串数组_推(&数组_短, 格式化("%s/crti.o", 库路径_));
    串数组_推(&数组_短, 格式化("%s/crtbegin.o", gcc_库路径_));
  }

  串数组_推(&数组_短, 格式化("-L%s", gcc_库路径_));
  串数组_推(&数组_短, "-L/usr/lib/x86_64-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib64");
  串数组_推(&数组_短, "-L/lib64");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-pc-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-redhat-linux");
  串数组_推(&数组_短, "-L/usr/lib");
  串数组_推(&数组_短, "-L/lib");

  if (!选项_静态) {
    串数组_推(&数组_短, "-dynamic-linker");
    串数组_推(&数组_短, "/lib64/ld-linux-x86-64.so.2");
  }

  for (int i = 0; i < 链接器_额外_实参_短.长度_短; i++)
    串数组_推(&数组_短, 链接器_额外_实参_短.数据[i]);

  for (int i = 0; i < 输入_复->长度_短; i++)
    串数组_推(&数组_短, 输入_复->数据[i]);

  if (选项_静态) {
    串数组_推(&数组_短, "--开启-group");
    串数组_推(&数组_短, "-lgcc");
    串数组_推(&数组_短, "-lgcc_eh");
    串数组_推(&数组_短, "-lc");
    串数组_推(&数组_短, "--终-group");
  } else {
    串数组_推(&数组_短, "-lc");
    串数组_推(&数组_短, "-lgcc");
    串数组_推(&数组_短, "--as-needed");
    串数组_推(&数组_短, "-lgcc_s");
    串数组_推(&数组_短, "--no-as-needed");
  }

  if (选项_共享的)
    串数组_推(&数组_短, 格式化("%s/crtendS.o", gcc_库路径_));
  else
    串数组_推(&数组_短, 格式化("%s/crtend.o", gcc_库路径_));

  串数组_推(&数组_短, 格式化("%s/crtn.o", 库路径_));
  串数组_推(&数组_短, NULL);

  跑_子进程(数组_短.数据);
}

static 文件类型_ 取_文件_类型_(char *文件名) {
  if (选项_x != 文件_全无)
    return 选项_x;

  if (以什么结束(文件名, ".a"))
    return 文件_AR;
  if (以什么结束(文件名, ".so"))
    return 文件_DSO;
  if (以什么结束(文件名, ".o"))
    return 文件_对象;
  if (以什么结束(文件名, ".c"))
    return 文件_C;
  if (以什么结束(文件名, ".s"))
    return 文件_汇编;

  错误("<command 行>: unknown 源文件位置 extension: %s", 文件名);
}

int main(int 实参计数, char **实参值) {
  atexit(清除);
  初始_宏();
  解析_实参_(实参计数, 实参值);

  if (选项_cc1) {
    添加_默认_包括_路径(实参值[0]);
    cc1();
    return 0;
  }

  if (输入_路径.长度_短 > 1 && 选项_o && (选项_c || 选项_S | 选项_E))
    错误("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");

  字符串数组 链接器_实参 = {};

  for (int i = 0; i < 输入_路径.长度_短; i++) {
    char *输入 = 输入_路径.数据[i];

    if (!strncmp(输入, "-l", 2)) {
      串数组_推(&链接器_实参, 输入);
      continue;
    }

    if (!strncmp(输入, "-Wl,", 4)) {
      char *s = strdup(输入 + 4);
      char *arg = strtok(s, ",");
      while (arg) {
        串数组_推(&链接器_实参, arg);
        arg = strtok(NULL, ",");
      }
      continue;
    }

    char *输出_;
    if (选项_o)
      输出_ = 选项_o;
    else if (选项_S)
      输出_ = 替换_扩展(输入, ".s");
    else
      输出_ = 替换_扩展(输入, ".o");

    文件类型_ type = 取_文件_类型_(输入);

    // Handle .o or .a
    if (type == 文件_对象 || type == 文件_AR || type == 文件_DSO) {
      串数组_推(&链接器_实参, 输入);
      continue;
    }

    // Handle .s
    if (type == 文件_汇编) {
      if (!选项_S)
        汇编_(输入, 输出_);
      continue;
    }

    assert(type == 文件_C);

    // Just 预处理
    if (选项_E || 选项_M) {
      跑_cc1(实参计数, 实参值, 输入, NULL);
      continue;
    }

    // Compile
    if (选项_S) {
      跑_cc1(实参计数, 实参值, 输入, 输出_);
      continue;
    }

    // Compile and 汇编_
    if (选项_c) {
      char *tmp = 创建_临时文件();
      跑_cc1(实参计数, 实参值, 输入, tmp);
      汇编_(tmp, 输出_);
      continue;
    }

    // Compile, 汇编_ and link
    char *tmp1 = 创建_临时文件();
    char *tmp2 = 创建_临时文件();
    跑_cc1(实参计数, 实参值, 输入, tmp1);
    汇编_(tmp1, tmp2);
    串数组_推(&链接器_实参, tmp2);
    continue;
  }

  if (链接器_实参.长度_短 > 0)
    跑_链接器_全(&链接器_实参, 选项_o ? 选项_o : "a.out");
  return 0;
}
