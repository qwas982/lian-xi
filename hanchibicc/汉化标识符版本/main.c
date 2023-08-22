#include "chibicc.h"

typedef enum {
  FILE_NONE, FILE_C, FILE_ASM, FILE_OBJ, FILE_AR, FILE_DSO,
} FileType;

字符串数组 包含_路径们_小写;
bool 优化_标志共同 = true;
bool 优化_标志位置独立代码;

static FileType opt_x;
static 字符串数组 opt_include;
static bool opt_E;
static bool opt_M;
static bool opt_MD;
static bool opt_MMD;
static bool opt_MP;
static bool opt_S;
static bool opt_c;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static bool opt_static;
static bool opt_shared;
static char *opt_MF;
static char *opt_MT;
static char *opt_o;

static 字符串数组 ld_extra_args;
static 字符串数组 std_include_paths;

char *基本_文件;
static char *output_file;

static 字符串数组 input_paths;
static 字符串数组 tmpfiles;

static void usage(int status) {
  fprintf(stderr, "chibicc [ -o <path> ] <文件_小写>\n");
  exit(status);
}

static bool take_arg(char *arg) {
  char *x[] = {
    "-o", "-I", "-idirafter", "-include", "-x", "-MF", "-MT", "-Xlinker",
  };

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)
    if (!strcmp(arg, x[i]))
      return true;
  return false;
}

static void add_default_include_paths(char *argv0) {
  // We expect that chibicc-specific include files are installed
  // to ./include relative to argv[0].
  串数组_推(&包含_路径们_小写, 格式化_函("%s/include", dirname(strdup(argv0))));

  // Add standard include paths.
  串数组_推(&包含_路径们_小写, "/usr/local/include");
  串数组_推(&包含_路径们_小写, "/usr/include/x86_64-linux-gnu");
  串数组_推(&包含_路径们_小写, "/usr/include");

  // Keep a copy of the standard include paths for -MMD option.
  for (int i = 0; i < 包含_路径们_小写.长度_短; i++)
    串数组_推(&std_include_paths, 包含_路径们_小写.数据_小写[i]);
}

static void define(char *串_小写_短) {
  char *eq = strchr(串_小写_短, '=');
  if (eq)
    定义_宏(strndup(串_小写_短, eq - 串_小写_短), eq + 1);
  else
    定义_宏(串_小写_短, "1");
}

static FileType parse_opt_x(char *s) {
  if (!strcmp(s, "c"))
    return FILE_C;
  if (!strcmp(s, "assembler"))
    return FILE_ASM;
  if (!strcmp(s, "none"))
    return FILE_NONE;
  错误_小写("<command 行号_小写>: unknown argument for -x: %s", s);
}

static char *quote_makefile(char *s) {
  char *buf = calloc(1, strlen(s) * 2 + 1);

  for (int i = 0, j = 0; s[i]; i++) {
    switch (s[i]) {
    case '$':
      buf[j++] = '$';
      buf[j++] = '$';
      break;
    case '#':
      buf[j++] = '\\';
      buf[j++] = '#';
      break;
    case ' ':
    case '\t':
      for (int k = i - 1; k >= 0 && s[k] == '\\'; k--)
        buf[j++] = '\\';
      buf[j++] = '\\';
      buf[j++] = s[i];
      break;
    default:
      buf[j++] = s[i];
      break;
    }
  }
  return buf;
}

static void parse_args(int argc, char **argv) {
  // Make sure that all command 行号_小写 options that take an argument
  // have an argument.
  for (int i = 1; i < argc; i++)
    if (take_arg(argv[i]))
      if (!argv[++i])
        usage(1);

  字符串数组 idirafter = {};

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-###")) {
      opt_hash_hash_hash = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
      continue;
    }

    if (!strcmp(argv[i], "--help"))
      usage(0);

    if (!strcmp(argv[i], "-o")) {
      opt_o = argv[++i];
      continue;
    }

    if (!strncmp(argv[i], "-o", 2)) {
      opt_o = argv[i] + 2;
      continue;
    }

    if (!strcmp(argv[i], "-S")) {
      opt_S = true;
      continue;
    }

    if (!strcmp(argv[i], "-fcommon")) {
      优化_标志共同 = true;
      continue;
    }

    if (!strcmp(argv[i], "-fno-common")) {
      优化_标志共同 = false;
      continue;
    }

    if (!strcmp(argv[i], "-c")) {
      opt_c = true;
      continue;
    }

    if (!strcmp(argv[i], "-E")) {
      opt_E = true;
      continue;
    }

    if (!strncmp(argv[i], "-I", 2)) {
      串数组_推(&包含_路径们_小写, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-D")) {
      define(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-D", 2)) {
      define(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-U")) {
      未定义_宏(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-U", 2)) {
      未定义_宏(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-include")) {
      串数组_推(&opt_include, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-x")) {
      opt_x = parse_opt_x(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-x", 2)) {
      opt_x = parse_opt_x(argv[i] + 2);
      continue;
    }

    if (!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "-Wl,", 4)) {
      串数组_推(&input_paths, argv[i]);
      continue;
    }

    if (!strcmp(argv[i], "-Xlinker")) {
      串数组_推(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-s")) {
      串数组_推(&ld_extra_args, "-s");
      continue;
    }

    if (!strcmp(argv[i], "-M")) {
      opt_M = true;
      continue;
    }

    if (!strcmp(argv[i], "-MF")) {
      opt_MF = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-MP")) {
      opt_MP = true;
      continue;
    }

    if (!strcmp(argv[i], "-MT")) {
      if (opt_MT == NULL)
        opt_MT = argv[++i];
      else
        opt_MT = 格式化_函("%s %s", opt_MT, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-MD")) {
      opt_MD = true;
      continue;
    }

    if (!strcmp(argv[i], "-MQ")) {
      if (opt_MT == NULL)
        opt_MT = quote_makefile(argv[++i]);
      else
        opt_MT = 格式化_函("%s %s", opt_MT, quote_makefile(argv[++i]));
      continue;
    }

    if (!strcmp(argv[i], "-MMD")) {
      opt_MD = opt_MMD = true;
      continue;
    }

    if (!strcmp(argv[i], "-fpic") || !strcmp(argv[i], "-fPIC")) {
      优化_标志位置独立代码 = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1-input")) {
      基本_文件 = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-cc1-output")) {
      output_file = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-idirafter")) {
      串数组_推(&idirafter, argv[i++]);
      continue;
    }

    if (!strcmp(argv[i], "-static")) {
      opt_static = true;
      串数组_推(&ld_extra_args, "-static");
      continue;
    }

    if (!strcmp(argv[i], "-shared")) {
      opt_shared = true;
      串数组_推(&ld_extra_args, "-shared");
      continue;
    }

    if (!strcmp(argv[i], "-L")) {
      串数组_推(&ld_extra_args, "-L");
      串数组_推(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-L", 2)) {
      串数组_推(&ld_extra_args, "-L");
      串数组_推(&ld_extra_args, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-hashmap-test")) {
      哈希映射_测试();
      exit(0);
    }

    // These options are ignored for now.
    if (!strncmp(argv[i], "-O", 2) ||
        !strncmp(argv[i], "-W", 2) ||
        !strncmp(argv[i], "-g", 2) ||
        !strncmp(argv[i], "-std=", 5) ||
        !strcmp(argv[i], "-ffreestanding") ||
        !strcmp(argv[i], "-fno-builtin") ||
        !strcmp(argv[i], "-fno-omit-frame-pointer") ||
        !strcmp(argv[i], "-fno-stack-protector") ||
        !strcmp(argv[i], "-fno-strict-aliasing") ||
        !strcmp(argv[i], "-m64") ||
        !strcmp(argv[i], "-mno-red-zone") ||
        !strcmp(argv[i], "-w"))
      continue;

    if (argv[i][0] == '-' && argv[i][1] != '\0')
      错误_小写("unknown argument: %s", argv[i]);

    串数组_推(&input_paths, argv[i]);
  }

  for (int i = 0; i < idirafter.长度_短; i++)
    串数组_推(&包含_路径们_小写, idirafter.数据_小写[i]);

  if (input_paths.长度_短 == 0)
    错误_小写("no input files / 无输入文件");

  // -E implies that the input is the C macro language.
  if (opt_E)
    opt_x = FILE_C;
}

static FILE *open_file(char *path) {
  if (!path || strcmp(path, "-") == 0)
    return stdout;

  FILE *out = fopen(path, "w");
  if (!out)
    错误_小写("cannot open output 文件_小写: %s: %s", path, strerror(errno));
  return out;
}

static bool endswith(char *p, char *q) {
  int len1 = strlen(p);
  int len2 = strlen(q);
  return (len1 >= len2) && !strcmp(p + len1 - len2, q);
}

// Replace 文件_小写 extension
static char *replace_extn(char *tmpl, char *extn) {
  char *文件名_小写 = basename(strdup(tmpl));
  char *dot = strrchr(文件名_小写, '.');
  if (dot)
    *dot = '\0';
  return 格式化_函("%s%s", 文件名_小写, extn);
}

static void cleanup(void) {
  for (int i = 0; i < tmpfiles.长度_短; i++)
    unlink(tmpfiles.数据_小写[i]);
}

static char *create_tmpfile(void) {
  char *path = strdup("/tmp/chibicc-XXXXXX");
  int fd = mkstemp(path);
  if (fd == -1)
    错误_小写("mkstemp failed: %s", strerror(errno));
  close(fd);

  串数组_推(&tmpfiles, path);
  return path;
}

static void run_subprocess(char **argv) {
  // If -### is given, dump the subprocess's command 行号_小写.
  if (opt_hash_hash_hash) {
    fprintf(stderr, "%s", argv[0]);
    for (int i = 1; argv[i]; i++)
      fprintf(stderr, " %s", argv[i]);
    fprintf(stderr, "\n");
  }

  if (fork() == 0) {
    // Child process. Run a new command.
    execvp(argv[0], argv);
    fprintf(stderr, "exec failed: %s: %s\n", argv[0], strerror(errno));
    _exit(1);
  }

  // Wait for the child process to finish.
  int status;
  while (wait(&status) > 0);
  if (status != 0)
    exit(1);
}

static void run_cc1(int argc, char **argv, char *input, char *output) {
  char **实参们_短 = calloc(argc + 10, sizeof(char *));
  memcpy(实参们_短, argv, argc * sizeof(char *));
  实参们_短[argc++] = "-cc1";

  if (input) {
    实参们_短[argc++] = "-cc1-input";
    实参们_短[argc++] = input;
  }

  if (output) {
    实参们_短[argc++] = "-cc1-output";
    实参们_短[argc++] = output;
  }

  run_subprocess(实参们_短);
}

// Print tokens to stdout. Used for -E.
static void print_tokens(牌 *牌_短_小写) {
  FILE *out = open_file(opt_o ? opt_o : "-");

  int 行号_小写 = 1;
  for (; 牌_短_小写->种类_小写 != 牌_文件终; 牌_短_小写 = 牌_短_小写->下一个_小写) {
    if (行号_小写 > 1 && 牌_短_小写->在_行开头)
      fprintf(out, "\n");
    if (牌_短_小写->有_空格 && !牌_短_小写->在_行开头)
      fprintf(out, " ");
    fprintf(out, "%.*s", 牌_短_小写->长度_短, 牌_短_小写->定位_小写_短);
    行号_小写++;
  }
  fprintf(out, "\n");
}

static bool in_std_include_path(char *path) {
  for (int i = 0; i < std_include_paths.长度_短; i++) {
    char *dir = std_include_paths.数据_小写[i];
    int 长度_短 = strlen(dir);
    if (strncmp(dir, path, 长度_短) == 0 && path[长度_短] == '/')
      return true;
  }
  return false;
}

// If -M options is given, the compiler write a list of input files to
// stdout in a 格式化_函 that "make" command can read. This feature is
// 已用_小写 to automate 文件_小写 dependency management.
static void print_dependencies(void) {
  char *path;
  if (opt_MF)
    path = opt_MF;
  else if (opt_MD)
    path = replace_extn(opt_o ? opt_o : 基本_文件, ".d");
  else if (opt_o)
    path = opt_o;
  else
    path = "-";

  FILE *out = open_file(path);
  if (opt_MT)
    fprintf(out, "%s:", opt_MT);
  else
    fprintf(out, "%s:", quote_makefile(replace_extn(基本_文件, ".o")));

  文件_大写 **files = 获取_输入_文件们();

  for (int i = 0; files[i]; i++) {
    if (opt_MMD && in_std_include_path(files[i]->名称_小写))
      continue;
    fprintf(out, " \\\n  %s", files[i]->名称_小写);
  }

  fprintf(out, "\n\n");

  if (opt_MP) {
    for (int i = 1; files[i]; i++) {
      if (opt_MMD && in_std_include_path(files[i]->名称_小写))
        continue;
      fprintf(out, "%s:\n\n", quote_makefile(files[i]->名称_小写));
    }
  }
}

static 牌 *must_tokenize_file(char *path) {
  牌 *牌_短_小写 = 化为牌_文件(path);
  if (!牌_短_小写)
    错误_小写("%s: %s", path, strerror(errno));
  return 牌_短_小写;
}

static 牌 *append_tokens(牌 *tok1, 牌 *tok2) {
  if (!tok1 || tok1->种类_小写 == 牌_文件终)
    return tok2;

  牌 *t = tok1;
  while (t->下一个_小写->种类_小写 != 牌_文件终)
    t = t->下一个_小写;
  t->下一个_小写 = tok2;
  return tok1;
}

static void cc1(void) {
  牌 *牌_短_小写 = NULL;

  // Process -include option
  for (int i = 0; i < opt_include.长度_短; i++) {
    char *incl = opt_include.数据_小写[i];

    char *path;
    if (文件_存在们(incl)) {
      path = incl;
    } else {
      path = 搜索_包含_路径们(incl);
      if (!path)
        错误_小写("-include: %s: %s", incl, strerror(errno));
    }

    牌 *tok2 = must_tokenize_file(path);
    牌_短_小写 = append_tokens(牌_短_小写, tok2);
  }

  // Tokenize and 解析_小写.
  牌 *tok2 = must_tokenize_file(基本_文件);
  牌_短_小写 = append_tokens(牌_短_小写, tok2);
  牌_短_小写 = 预处理_小写(牌_短_小写);

  // If -M or -MD are given, print 文件_小写 dependencies.
  if (opt_M || opt_MD) {
    print_dependencies();
    if (opt_M)
      return;
  }

  // If -E is given, print out preprocessed C code as a result.
  if (opt_E) {
    print_tokens(牌_短_小写);
    return;
  }

  对象_缩_大写 *prog = 解析_小写(牌_短_小写);

  // Open a temporary output buffer.
  char *buf;
  size_t buflen;
  FILE *output_buf = open_memstream(&buf, &buflen);

  // Traverse the AST to emit assembly.
  代码生成_短(prog, output_buf);
  fclose(output_buf);

  // Write the asembly text to a 文件_小写.
  FILE *out = open_file(output_file);
  fwrite(buf, buflen, 1, out);
  fclose(out);
}

static void assemble(char *input, char *output) {
  char *cmd[] = {"as", "-c", input, "-o", output, NULL};
  run_subprocess(cmd);
}

static char *find_file(char *pattern) {
  char *path = NULL;
  glob_t buf = {};
  glob(pattern, 0, NULL, &buf);
  if (buf.gl_pathc > 0)
    path = strdup(buf.gl_pathv[buf.gl_pathc - 1]);
  globfree(&buf);
  return path;
}

// Returns true if a given 文件_小写 exists.
bool 文件_存在们(char *path) {
  struct stat st;
  return !stat(path, &st);
}

static char *find_libpath(void) {
  if (文件_存在们("/usr/lib/x86_64-linux-gnu/crti.o"))
    return "/usr/lib/x86_64-linux-gnu";
  if (文件_存在们("/usr/lib64/crti.o"))
    return "/usr/lib64";
  错误_小写("library path is not found");
}

static char *find_gcc_libpath(void) {
  char *paths[] = {
    "/usr/lib/gcc/x86_64-linux-gnu/*/crtbegin.o",
    "/usr/lib/gcc/x86_64-pc-linux-gnu/*/crtbegin.o", // For Gentoo
    "/usr/lib/gcc/x86_64-redhat-linux/*/crtbegin.o", // For Fedora
  };

  for (int i = 0; i < sizeof(paths) / sizeof(*paths); i++) {
    char *path = find_file(paths[i]);
    if (path)
      return dirname(path);
  }

  错误_小写("gcc library path is not found");
}

static void run_linker(字符串数组 *inputs, char *output) {
  字符串数组 数组_短 = {};

  串数组_推(&数组_短, "ld");
  串数组_推(&数组_短, "-o");
  串数组_推(&数组_短, output);
  串数组_推(&数组_短, "-m");
  串数组_推(&数组_短, "elf_x86_64");

  char *libpath = find_libpath();
  char *gcc_libpath = find_gcc_libpath();

  if (opt_shared) {
    串数组_推(&数组_短, 格式化_函("%s/crti.o", libpath));
    串数组_推(&数组_短, 格式化_函("%s/crtbeginS.o", gcc_libpath));
  } else {
    串数组_推(&数组_短, 格式化_函("%s/crt1.o", libpath));
    串数组_推(&数组_短, 格式化_函("%s/crti.o", libpath));
    串数组_推(&数组_短, 格式化_函("%s/crtbegin.o", gcc_libpath));
  }

  串数组_推(&数组_短, 格式化_函("-L%s", gcc_libpath));
  串数组_推(&数组_短, "-L/usr/lib/x86_64-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib64");
  串数组_推(&数组_短, "-L/lib64");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-pc-linux-gnu");
  串数组_推(&数组_短, "-L/usr/lib/x86_64-redhat-linux");
  串数组_推(&数组_短, "-L/usr/lib");
  串数组_推(&数组_短, "-L/lib");

  if (!opt_static) {
    串数组_推(&数组_短, "-dynamic-linker");
    串数组_推(&数组_短, "/lib64/ld-linux-x86-64.so.2");
  }

  for (int i = 0; i < ld_extra_args.长度_短; i++)
    串数组_推(&数组_短, ld_extra_args.数据_小写[i]);

  for (int i = 0; i < inputs->长度_短; i++)
    串数组_推(&数组_短, inputs->数据_小写[i]);

  if (opt_static) {
    串数组_推(&数组_短, "--开起-group");
    串数组_推(&数组_短, "-lgcc");
    串数组_推(&数组_短, "-lgcc_eh");
    串数组_推(&数组_短, "-lc");
    串数组_推(&数组_短, "--终_小写-group");
  } else {
    串数组_推(&数组_短, "-lc");
    串数组_推(&数组_短, "-lgcc");
    串数组_推(&数组_短, "--as-needed");
    串数组_推(&数组_短, "-lgcc_s");
    串数组_推(&数组_短, "--no-as-needed");
  }

  if (opt_shared)
    串数组_推(&数组_短, 格式化_函("%s/crtendS.o", gcc_libpath));
  else
    串数组_推(&数组_短, 格式化_函("%s/crtend.o", gcc_libpath));

  串数组_推(&数组_短, 格式化_函("%s/crtn.o", libpath));
  串数组_推(&数组_短, NULL);

  run_subprocess(数组_短.数据_小写);
}

static FileType get_file_type(char *文件名_小写) {
  if (opt_x != FILE_NONE)
    return opt_x;

  if (endswith(文件名_小写, ".a"))
    return FILE_AR;
  if (endswith(文件名_小写, ".so"))
    return FILE_DSO;
  if (endswith(文件名_小写, ".o"))
    return FILE_OBJ;
  if (endswith(文件名_小写, ".c"))
    return FILE_C;
  if (endswith(文件名_小写, ".s"))
    return FILE_ASM;

  错误_小写("<command 行号_小写>: unknown 文件_小写 extension: %s", 文件名_小写);
}

int main(int argc, char **argv) {
  atexit(cleanup);
  初始_宏们();
  parse_args(argc, argv);

  if (opt_cc1) {
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }

  if (input_paths.长度_短 > 1 && opt_o && (opt_c || opt_S | opt_E))
    错误_小写("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");

  字符串数组 ld_args = {};

  for (int i = 0; i < input_paths.长度_短; i++) {
    char *input = input_paths.数据_小写[i];

    if (!strncmp(input, "-l", 2)) {
      串数组_推(&ld_args, input);
      continue;
    }

    if (!strncmp(input, "-Wl,", 4)) {
      char *s = strdup(input + 4);
      char *arg = strtok(s, ",");
      while (arg) {
        串数组_推(&ld_args, arg);
        arg = strtok(NULL, ",");
      }
      continue;
    }

    char *output;
    if (opt_o)
      output = opt_o;
    else if (opt_S)
      output = replace_extn(input, ".s");
    else
      output = replace_extn(input, ".o");

    FileType type = get_file_type(input);

    // Handle .o or .a
    if (type == FILE_OBJ || type == FILE_AR || type == FILE_DSO) {
      串数组_推(&ld_args, input);
      continue;
    }

    // Handle .s
    if (type == FILE_ASM) {
      if (!opt_S)
        assemble(input, output);
      continue;
    }

    assert(type == FILE_C);

    // Just 预处理_小写
    if (opt_E || opt_M) {
      run_cc1(argc, argv, input, NULL);
      continue;
    }

    // Compile
    if (opt_S) {
      run_cc1(argc, argv, input, output);
      continue;
    }

    // Compile and assemble
    if (opt_c) {
      char *tmp = create_tmpfile();
      run_cc1(argc, argv, input, tmp);
      assemble(tmp, output);
      continue;
    }

    // Compile, assemble and link
    char *tmp1 = create_tmpfile();
    char *tmp2 = create_tmpfile();
    run_cc1(argc, argv, input, tmp1);
    assemble(tmp1, tmp2);
    串数组_推(&ld_args, tmp2);
    continue;
  }

  if (ld_args.长度_短 > 0)
    run_linker(&ld_args, opt_o ? opt_o : "a.out");
  return 0;
}
