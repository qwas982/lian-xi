//main.c

#include "han_chibicc.h"

typedef enum {
  FILE_NONE, FILE_C, FILE_ASM, FILE_OBJ, FILE_AR, FILE_DSO,
} FileType;  // 定义一个枚举类型FileType，包含六个取值：FILE_NONE, FILE_C, FILE_ASM, FILE_OBJ, FILE_AR, FILE_DSO

StringArray include_paths;  // 声明一个StringArray类型的变量include_paths
bool opt_fcommon = true;  // 声明一个bool类型的变量opt_fcommon，并初始化为true
bool opt_fpic;  // 声明一个bool类型的变量opt_fpic，未初始化

static FileType opt_x;  // 声明一个静态的FileType类型变量opt_x
static StringArray opt_include;  // 声明一个静态的StringArray类型变量opt_include
static bool opt_E;  // 声明一个静态的bool类型变量opt_E
static bool opt_M;  // 声明一个静态的bool类型变量opt_M
static bool opt_MD;  // 声明一个静态的bool类型变量opt_MD
static bool opt_MMD;  // 声明一个静态的bool类型变量opt_MMD
static bool opt_MP;  // 声明一个静态的bool类型变量opt_MP
static bool opt_S;  // 声明一个静态的bool类型变量opt_S
static bool opt_c;  // 声明一个静态的bool类型变量opt_c
static bool opt_cc1;  // 声明一个静态的bool类型变量opt_cc1
static bool opt_hash_hash_hash;  // 声明一个静态的bool类型变量opt_hash_hash_hash
static bool opt_static;  // 声明一个静态的bool类型变量opt_static
static bool opt_shared;  // 声明一个静态的bool类型变量opt_shared
static char *opt_MF;  // 声明一个静态的char指针类型变量opt_MF
static char *opt_MT;  // 声明一个静态的char指针类型变量opt_MT
static char *opt_o;  // 声明一个静态的char指针类型变量opt_o

static StringArray ld_extra_args;  // 声明一个静态的StringArray类型变量ld_extra_args
static StringArray std_include_paths;  // 声明一个静态的StringArray类型变量std_include_paths

char *base_file;  // 声明一个char指针类型变量base_file
static char *output_file;  // 声明一个静态的char指针类型变量output_file

static StringArray input_paths;  // 声明一个静态的StringArray类型变量input_paths
static StringArray tmpfiles;  // 声明一个静态的StringArray类型变量tmpfiles

static void usage(int status) {
  fprintf(stderr, "chibicc [ -o <path> ] <file>\n"); // 打印使用方法，将错误信息输出到标准错误流(stderr)
  exit(status); // 退出程序，状态码为传入的参数status
}

static bool take_arg(char *arg) {  // 定义一个静态函数take_arg，参数为指向字符的指针arg
  char *x[] = {  // 定义字符串数组x，包含一些特定的选项
    "-o", "-I", "-idirafter", "-include", "-x", "-MF", "-MT", "-Xlinker",
  };

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)  // 使用循环遍历数组x中的元素
    if (!strcmp(arg, x[i]))  // 判断arg是否和数组x中的元素相等
      return true;  // 如果相等，则返回true
  return false;  // 如果不相等，则返回false
}  // 函数结束

static void add_default_include_paths(char *argv0) {    // 定义静态函数 add_default_include_paths，参数为 char* 类型的 argv0
  // 添加默认的包含路径函数，参数为 argv0
  // 我们期望 chibicc 特定的包含文件被安装在相对于 argv[0] 的 ./include 目录下。
  strarray_push(&include_paths, format("%s/include", dirname(strdup(argv0))));  // 将格式化字符串 "%s/include" 和 argv0 的目录名拼接成路径，并将其添加到 include_paths 数组中

  // 添加标准的包含路径
  strarray_push(&include_paths, "/usr/local/include");  // 将 "/usr/local/include" 添加到 include_paths 数组中
  strarray_push(&include_paths, "/usr/include/x86_64-linux-gnu");  // 将 "/usr/include/x86_64-linux-gnu" 添加到 include_paths 数组中
  strarray_push(&include_paths, "/usr/include");  // 将 "/usr/include" 添加到 include_paths 数组中

  // 为了 -MMD 选项，保留标准包含路径的副本
  for (int i = 0; i < include_paths.len; i++)
    strarray_push(&std_include_paths, include_paths.data[i]);  // 将 include_paths 数组中的元素复制到 std_include_paths 数组中，以备后续使用 -MMD 选项
}

static void define(char *str) {  // 定义静态函数 define，参数为指向字符的指针 str
  char *eq = strchr(str, '=');  // 在字符串 str 中查找字符 '=' 的位置，返回指向该位置的指针 eq
  if (eq)  // 如果找到了 '=' 字符
    define_macro(strndup(str, eq - str), eq + 1);  // 调用函数 define_macro，传入参数为从 str 复制的子字符串和等号后面的部分
  else  // 如果没有找到 '=' 字符
    define_macro(str, "1");  // 调用函数 define_macro，传入参数为 str 和字符串 "1"
}

static FileType parse_opt_x(char *s) {
    // 静态函数，用于解析命令行参数 s，并返回对应的文件类型
    if (!strcmp(s, "c"))
        return FILE_C;          // 如果 s 与 "c" 相等，则返回 FILE_C 类型
    if (!strcmp(s, "assembler"))
        return FILE_ASM;        // 如果 s 与 "assembler" 相等，则返回 FILE_ASM 类型
    if (!strcmp(s, "none"))
        return FILE_NONE;       // 如果 s 与 "none" 相等，则返回 FILE_NONE 类型
    error("<command line>: unknown argument for -x: %s", s);
    // 如果参数 s 不匹配任何已知类型，则输出错误信息，并终止程序运行
}

static char *quote_makefile(char *s) {
  char *buf = calloc(1, strlen(s) * 2 + 1);  // 为buf分配内存，长度为原字符串s的两倍加1

  for (int i = 0, j = 0; s[i]; i++) {  // 遍历字符串s
    switch (s[i]) {
    case '$':
      buf[j++] = '$';  // 如果遇到'$'，在buf中插入两个'$'
      buf[j++] = '$';
      break;
    case '#':
      buf[j++] = '\\';  // 如果遇到'#'，在buf中插入'\'和'#'
      buf[j++] = '#';
      break;
    case ' ':
    case '\t':
      for (int k = i - 1; k >= 0 && s[k] == '\\'; k--)
        buf[j++] = '\\';  // 统计前面连续的'\'个数
      buf[j++] = '\\';  // 在buf中插入'\'和当前字符
      buf[j++] = s[i];
      break;
    default:
      buf[j++] = s[i];  // 其他情况直接将字符复制到buf中
      break;
    }
  }
  return buf;  // 返回处理后的字符串buf
}

static void parse_args(int argc, char **argv) {
	// 确保所有需要实参的命令行选项都有实参。
	for (int i = 1; i < argc; i++)
		if (take_arg(argv[i]))  // 如果当前选项需要参数
			if (!argv[++i])  // 如果下一个参数为空
        	usage(1);  // 调用usage函数显示用法并退出程序
	
	StringArray idirafter = {};  // 声明并初始化一个字符串数组idirafter

	for (int i = 1; i < argc; i++) {  // 从1开始遍历命令行参数，直到argc
		if (!strcmp(argv[i], "-###")) {  // 如果当前参数与"-###"相等
		  opt_hash_hash_hash = true;  // 设置opt_hash_hash_hash为true
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strcmp(argv[i], "-cc1")) {  // 如果当前参数与"-cc1"相等
		  opt_cc1 = true;  // 设置opt_cc1为true
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strcmp(argv[i], "--help"))  // 如果当前参数与"--help"相等
		  usage(0);  // 调用usage函数并传入参数0

		if (!strcmp(argv[i], "-o")) {  // 如果当前参数与"-o"相等
		  opt_o = argv[++i];  // 将下一个参数赋值给opt_o，并将i增加1
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strncmp(argv[i], "-o", 2)) {
		    opt_o = argv[i] + 2;    // 如果参数以"-o"开头，则将指针指向"-o"之后的字符串部分
		    continue;               // 继续下一次循环
		}

		if (!strcmp(argv[i], "-S")) {
		    opt_S = true;           // 如果参数等于"-S"，则将opt_S设置为true
		    continue;               // 继续下一次循环
		}

		if (!strcmp(argv[i], "-fcommon")) {
		    opt_fcommon = true;     // 如果参数等于"-fcommon"，则将opt_fcommon设置为true
		    continue;               // 继续下一次循环
		}

		if (!strcmp(argv[i], "-fno-common")) {
		    opt_fcommon = false;    // 如果参数等于"-fno-common"，则将opt_fcommon设置为false
		    continue;               // 继续下一次循环
		}

		if (!strcmp(argv[i], "-c")) {
		  opt_c = true;  // 如果命令行参数与"-c"相等，则将opt_c设置为true
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strcmp(argv[i], "-E")) {
		  opt_E = true;  // 如果命令行参数与"-E"相等，则将opt_E设置为true
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strncmp(argv[i], "-I", 2)) {
		  strarray_push(&include_paths, argv[i] + 2);  // 将命令行参数中"-I"后面的字符串添加到include_paths数组中
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strcmp(argv[i], "-D")) {
		  define(argv[++i]);  // 如果命令行参数与"-D"相等，则调用define函数，并将下一个参数作为参数传递给define函数
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strncmp(argv[i], "-D", 2)) {
		  define(argv[i] + 2);  // 将命令行参数中"-D"后面的字符串作为参数传递给define函数
		  continue;  // 跳过当前循环，继续下一次循环
		}

		if (!strcmp(argv[i], "-U")) {
		    // 如果参数与"-U"相等
		    undef_macro(argv[++i]); // 调用函数undef_macro并传入下一个参数
		    continue; // 继续下一次循环
		}

		if (!strncmp(argv[i], "-U", 2)) {
		    // 如果参数以"-U"开头
		    undef_macro(argv[i] + 2); // 调用函数undef_macro并传入参数的第三个字符开始的子字符串
		    continue; // 继续下一次循环
		}

		if (!strcmp(argv[i], "-include")) {
		    // 如果参数与"-include"相等
		    strarray_push(&opt_include, argv[++i]); // 将下一个参数添加到opt_include数组中
		    continue; // 继续下一次循环
		}

		if (!strcmp(argv[i], "-x")) {
		    // 如果参数与"-x"相等
		    opt_x = parse_opt_x(argv[++i]); // 调用函数parse_opt_x并传入下一个参数的值，并将返回值赋给opt_x
		    continue; // 继续下一次循环
		}

		if (!strncmp(argv[i], "-x", 2)) {
		    // 如果参数以"-x"开头
		    opt_x = parse_opt_x(argv[i] + 2); // 调用函数parse_opt_x并传入参数的第三个字符开始的子字符串，并将返回值赋给opt_x
		    continue; // 继续下一次循环
		}

		if (!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "-Wl,", 4)) {
		  strarray_push(&input_paths, argv[i]);  // 如果命令行参数以"-l"开头或者以"-Wl,"开头，将该参数添加到input_paths数组中
		  continue;  // 跳过当前迭代，继续下一次迭代
		}

		if (!strcmp(argv[i], "-Xlinker")) {
		  strarray_push(&ld_extra_args, argv[++i]);  // 如果命令行参数与"-Xlinker"相等，将下一个参数添加到ld_extra_args数组中
		  continue;  // 跳过当前迭代，继续下一次迭代
		}

		if (!strcmp(argv[i], "-s")) {
		  strarray_push(&ld_extra_args, "-s");  // 如果命令行参数与"-s"相等，将"-s"添加到ld_extra_args数组中
		  continue;  // 跳过当前迭代，继续下一次迭代
		}

		if (!strcmp(argv[i], "-M")) {
		  opt_M = true;  // 如果命令行参数与"-M"相等，将opt_M设置为true
		  continue;  // 跳过当前迭代，继续下一次迭代
		}

		if (!strcmp(argv[i], "-MF")) {  // 如果命令行参数等于"-MF"
		  opt_MF = argv[++i];  // 将下一个参数赋值给opt_MF
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-MP")) {  // 如果命令行参数等于"-MP"
		  opt_MP = true;  // 将opt_MP设置为true
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-MT")) {  // 如果命令行参数等于"-MT"
		  if (opt_MT == NULL)  // 如果opt_MT为NULL
		    opt_MT = argv[++i];  // 将下一个参数赋值给opt_MT
		  else
		    opt_MT = format("%s %s", opt_MT, argv[++i]);  // 否则将opt_MT和下一个参数格式化为字符串赋值给opt_MT
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-MD")) {  // 如果命令行参数等于"-MD"
		  opt_MD = true;  // 将opt_MD设置为true
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-MQ")) {  // 如果命令行参数等于"-MQ"
		  if (opt_MT == NULL)  // 如果opt_MT为NULL
		    opt_MT = quote_makefile(argv[++i]);  // 将下一个参数经过引号处理后赋值给opt_MT
		  else
		    opt_MT = format("%s %s", opt_MT, quote_makefile(argv[++i]));  // 否则将opt_MT和下一个参数经过引号处理后格式化为字符串赋值给opt_MT
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-MMD")) {  // 如果命令行参数等于"-MMD"
		  opt_MD = opt_MMD = true;  // 将opt_MD和opt_MMD都设置为true
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-fpic") || !strcmp(argv[i], "-fPIC")) {  // 如果命令行参数等于"-fpic"或者"-fPIC"
		  opt_fpic = true;  // 将opt_fpic设置为true
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-cc1-input")) {  // 如果命令行参数等于"-cc1-input"
		  base_file = argv[++i];  // 将下一个参数赋值给base_file
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-cc1-output")) {  // 如果命令行参数等于"-cc1-output"
		  output_file = argv[++i];  // 将下一个参数赋值给output_file
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-idirafter")) {  // 如果命令行参数等于"-idirafter"
		  strarray_push(&idirafter, argv[i++]);  // 将当前参数添加到idirafter数组中，并递增i
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-static")) {  // 如果命令行参数等于"-static"
		  opt_static = true;  // 将opt_static设置为true
		  strarray_push(&ld_extra_args, "-static");  // 将"-static"添加到ld_extra_args数组中
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-shared")) {  // 如果命令行参数等于"-shared"
		  opt_shared = true;  // 将opt_shared设置为true
		  strarray_push(&ld_extra_args, "-shared");  // 将"-shared"添加到ld_extra_args数组中
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-L")) {  // 如果命令行参数等于"-L"
		  strarray_push(&ld_extra_args, "-L");  // 将"-L"添加到ld_extra_args数组中
		  strarray_push(&ld_extra_args, argv[++i]);  // 将下一个参数添加到ld_extra_args数组中，并递增i
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strncmp(argv[i], "-L", 2)) {  // 如果命令行参数以"-L"开头
		  strarray_push(&ld_extra_args, "-L");  // 将"-L"添加到ld_extra_args数组中
		  strarray_push(&ld_extra_args, argv[i] + 2);  // 将参数中从第三个字符开始的部分添加到ld_extra_args数组中
		  continue;  // 跳过当前循环，进入下一次循环
		}

		if (!strcmp(argv[i], "-hashmap-test")) {  // 如果命令行参数等于"-hashmap-test"
		  hashmap_test();  // 调用hashmap_test函数
		  exit(0);  // 退出程序，状态码为0
		}

		// 这些选项目前被忽略
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
		  continue;  // 跳过当前循环，进入下一次循环
		// 如果命令行参数以 "-O"、"-W"、"-g"、"-std=" 开头，
		// 或者等于 "-ffreestanding"、"-fno-builtin"、"-fno-omit-frame-pointer"、"-fno-stack-protector"、"-fno-strict-aliasing"、"-m64"、"-mno-red-zone"、"-w"，
		// 则跳过当前循环，进入下一次循环

		if (argv[i][0] == '-' && argv[i][1] != '\0')  // 如果参数以"-"开头且不是单独的"-"
		  error("unknown argument: %s", argv[i]);  // 报错，显示未知参数信息

		strarray_push(&input_paths, argv[i]);  // 将当前参数添加到input_paths数组中
	}

	for (int i = 0; i < idirafter.len; i++) // 循环遍历idirafter数组
	    strarray_push(&include_paths, idirafter.data[i]); // 将idirafter数组的每个元素添加到include_paths数组中

	if (input_paths.len == 0) // 如果input_paths数组长度为0
		error("no input files"); // 报错：没有输入文件
	
	// -E意味着输入是C宏语言。
	if (opt_E) // 如果opt_E为真
		opt_x = FILE_C; // 将opt_x设置为FILE_C
}

static FILE *open_file(char *path) {    // 定义一个静态函数open_file，接受一个参数path作为文件路径
  if (!path || strcmp(path, "-") == 0) // 如果path为空指针或者path与"-"相等，则返回stdout指针，表示标准输出
    return stdout;

  FILE *out = fopen(path, "w"); // 定义一个文件指针out，并将打开路径为path的文件以写入模式打开
  if (!out)
    error("cannot open output file: %s: %s", path, strerror(errno)); // 如果打开文件失败，则输出错误信息
  return out; // 返回文件指针out
}

static bool endswith(char *p, char *q) {
  int len1 = strlen(p); // 获取字符串p的长度
  int len2 = strlen(q); // 获取字符串q的长度
  return (len1 >= len2) && !strcmp(p + len1 - len2, q); // 返回判断字符串p是否以字符串q结尾的结果
}

// 替换文件扩展名
static char *replace_extn(char *tmpl, char *extn) {
  char *filename = basename(strdup(tmpl)); // 提取模板路径中的文件名部分
  char *dot = strrchr(filename, '.'); // 查找文件名中最后一个点的位置
  if (dot)
    *dot = '\0'; // 如果找到点，则将其替换为字符串结束符，截断文件扩展名部分
  return format("%s%s", filename, extn); // 格式化新的文件名，将文件名和新扩展名连接起来
}

static void cleanup(void) {                              // 定义了一个静态函数 cleanup，无返回值，无参数
  for (int i = 0; i < tmpfiles.len; i++) {               // 循环遍历 tmpfiles 数组
    unlink(tmpfiles.data[i]);                            // 删除 tmpfiles 数组中的文件
  }
}

static char *create_tmpfile(void) {                               // 定义了一个静态函数 create_tmpfile，返回类型为 char 指针，无参数
  char *path = strdup("/tmp/chibicc-XXXXXX");                    // 为 path 分配内存并将字符串 "/tmp/chibicc-XXXXXX" 复制给它
  int fd = mkstemp(path);                                         // 使用 mkstemp 创建一个临时文件，返回文件描述符并赋值给 fd
  if (fd == -1)                                                   // 如果文件描述符为 -1，表示创建临时文件失败
    error("mkstemp failed: %s", strerror(errno));                 // 输出错误信息并终止程序，显示失败原因
  close(fd);                                                      // 关闭文件描述符

  strarray_push(&tmpfiles, path);                                 // 将 path 添加到 tmpfiles 数组中
  return path;                                                    // 返回 path 指针
}

static void run_subprocess(char **argv) {                          // 定义了一个静态函数 run_subprocess，无返回值，参数为 char 指针的数组
  // 如果 -### 被指定，则转储子进程的命令行
  if (opt_hash_hash_hash) {                                        // 如果 opt_hash_hash_hash 为真，则执行以下代码块
    fprintf(stderr, "%s", argv[0]);                                // 输出 argv[0] 到标准错误流
    for (int i = 1; argv[i]; i++)                                  // 遍历 argv 数组，从索引 1 开始
      fprintf(stderr, " %s", argv[i]);                             // 输出每个元素到标准错误流
    fprintf(stderr, "\n");                                         // 输出换行符到标准错误流
  }
	
	// 子进程。运行一个新命令
  if (fork() == 0) {                                               // 当前进程进行分叉，如果返回值为 0，则表示当前进程为子进程
    execvp(argv[0], argv);                                         // 在子进程中执行给定的命令
    fprintf(stderr, "exec failed: %s: %s\n", argv[0], strerror(errno));    // 如果执行失败，输出错误信息到标准错误流，并终止子进程
    _exit(1);
  }

  // 等待子进程完成
  int status;                                                      // 声明一个整型变量 status 用于存储子进程的退出状态
  while (wait(&status) > 0);                                       // 等待子进程结束
  if (status != 0)                                                 // 如果子进程的退出状态不为 0
    exit(1);                                                       // 终止当前进程
}

static void run_cc1(int argc, char **argv, char *input, char *output) {  // 定义了一个静态函数 run_cc1，无返回值，参数为整型 argc，字符指针的数组 argv，字符指针 input 和 output
  char **args = calloc(argc + 10, sizeof(char *));  // 为args分配内存，大小为(argc + 10) * sizeof(char *)字节，并将其指针赋给args
  memcpy(args, argv, argc * sizeof(char *));  // 将argv的内容复制到args中，复制长度为argc * sizeof(char *)字节
  args[argc++] = "-cc1";  // 在args数组的末尾添加"-cc1"字符串，并将argc的值自增1

  if (input) {
    args[argc++] = "-cc1-input";  // 如果input非空，将"-cc1-input"字符串添加到args数组末尾，并将argc的值自增1
    args[argc++] = input;  // 将input的指针赋给args数组的下一个位置，并将argc的值自增1
  }

  if (output) {
    args[argc++] = "-cc1-output";  // 如果output非空，将"-cc1-output"字符串添加到args数组末尾，并将argc的值自增1
    args[argc++] = output;  // 将output的指针赋给args数组的下一个位置，并将argc的值自增1
  }

  run_subprocess(args);  // 调用run_subprocess函数，并将args作为参数传递给它
}

// 打印标记到标准输出。用于选项-E。
static void print_tokens(Token *tok) {
  FILE *out = open_file(opt_o ? opt_o : "-");  // 打开文件，若opt_o非空则打开对应文件，否则打开标准输出

  int line = 1;  // 初始化行数为1
  for (; tok->kind != TK_EOF; tok = tok->next) {  // 循环遍历标记链表，直到遇到TK_EOF标记
    if (line > 1 && tok->at_bol)  // 如果行数大于1且当前标记位于行首
      fprintf(out, "\n");  // 在输出文件中打印换行符
    if (tok->has_space && !tok->at_bol)  // 如果标记前有空格且不位于行首
      fprintf(out, " ");  // 在输出文件中打印一个空格
    fprintf(out, "%.*s", tok->len, tok->loc);  // 在输出文件中打印标记的内容
    line++;  // 行数加1
  }
  fprintf(out, "\n");  // 在输出文件中打印换行符
}

static bool in_std_include_path(char *path) {
  for (int i = 0; i < std_include_paths.len; i++) {  // 循环遍历std_include_paths数组中的元素
    char *dir = std_include_paths.data[i];  // 获取当前元素的指针并赋给dir变量
    int len = strlen(dir);  // 获取dir字符串的长度，并赋给len变量
    if (strncmp(dir, path, len) == 0 && path[len] == '/')  // 如果dir和path的前len个字符相同且path的第len个字符是'/'，则返回true
      return true;
  }
  return false;  // 如果循环结束后仍未找到匹配项，则返回false
}

// 如果 -M 选项被指定，则编译器将输入文件列表写到标准输出，以便“make”命令可以读取。
// 这个功能用于自动化文件依赖管理。
static void print_dependencies(void) {
  char *path;  // 声明一个指向字符的指针变量path
  if (opt_MF)
    path = opt_MF;  // 如果opt_MF非空，则将opt_MF的值赋给path
  else if (opt_MD)
    path = replace_extn(opt_o ? opt_o : base_file, ".d");  // 如果opt_MD非空，则将opt_o或base_file替换扩展名为".d"的结果赋给path
  else if (opt_o)
    path = opt_o;  // 如果opt_o非空，则将opt_o的值赋给path
  else
    path = "-";  // 否则，将"-"赋给path

  FILE *out = open_file(path);  // 打开文件，文件路径由path指定，并将文件指针赋给out
  if (opt_MT)
    fprintf(out, "%s:", opt_MT);  // 如果opt_MT非空，则在输出文件中打印opt_MT的值后面加上冒号
  else
    fprintf(out, "%s:", quote_makefile(replace_extn(base_file, ".o")));  // 否则，在输出文件中打印将base_file的扩展名替换为".o"后的结果，并使用quote_makefile函数处理之后加上冒号

  File **files = get_input_files();  // 调用get_input_files函数获取输入文件列表，并将其赋给files

  for (int i = 0; files[i]; i++) {  // 循环遍历files数组中的元素
    if (opt_MMD && in_std_include_path(files[i]->name))  // 如果opt_MMD为真且files[i]->name在标准包含路径中，则跳过当前迭代
      continue;
    fprintf(out, " \\\n  %s", files[i]->name);  // 在输出文件中打印换行符和当前文件名
  }

  fprintf(out, "\n\n");  // 在输出文件中打印两个换行符

  if (opt_MP) {
    for (int i = 1; files[i]; i++) {  // 循环遍历files数组中的元素，从索引1开始
      if (opt_MMD && in_std_include_path(files[i]->name))  // 如果opt_MMD为真且files[i]->name在标准包含路径中，则跳过当前迭代
        continue;
      fprintf(out, "%s:\n\n", quote_makefile(files[i]->name));  // 在输出文件中打印当前文件名，并在末尾打印两个换行符
    }
  }
}

static Token *must_tokenize_file(char *path) {
  Token *tok = tokenize_file(path);  // 调用tokenize_file函数对指定文件进行标记化，并将结果赋给tok
  if (!tok)
    error("%s: %s", path, strerror(errno));  // 如果tok为空，则报错并打印错误信息，其中包括文件路径和错误信息
  return tok;  // 返回tok
}

static Token *append_tokens(Token *tok1, Token *tok2) {
  if (!tok1 || tok1->kind == TK_EOF)  // 如果tok1为空或tok1的种类是TK_EOF，则直接返回tok2
    return tok2;

  Token *t = tok1;  // 声明一个指向Token的指针变量t，并将tok1赋给它
  while (t->next->kind != TK_EOF)  // 循环遍历t链表，直到找到TK_EOF标记
    t = t->next;  // 将t指向下一个标记
  t->next = tok2;  // 将tok2连接到t链表的末尾
  return tok1;  // 返回tok1
}

static void cc1(void) {
  Token *tok = NULL;  // 声明一个指向Token的指针变量tok，并将其初始化为空

  // 处理-include选项
  for (int i = 0; i < opt_include.len; i++) {  // 循环遍历opt_include数组中的元素
    char *incl = opt_include.data[i];  // 获取当前元素的值并赋给incl变量

    char *path;  // 声明一个指向字符的指针变量path
    if (file_exists(incl)) {  // 如果incl文件存在
      path = incl;  // 将incl赋给path
    } else {
      path = search_include_paths(incl);  // 否则，调用search_include_paths函数在包含路径中搜索incl文件，并将结果赋给path
      if (!path)
        error("-include: %s: %s", incl, strerror(errno));  // 如果path为空，则报错并打印错误信息，其中包括incl和错误信息
    }

    Token *tok2 = must_tokenize_file(path);  // 调用must_tokenize_file函数对path文件进行标记化，并将结果赋给tok2
    tok = append_tokens(tok, tok2);  // 将tok2连接到tok链表的末尾，并将结果赋给tok
  }

  // 化为牌与解析.
  Token *tok2 = must_tokenize_file(base_file);  // 调用must_tokenize_file函数对base_file文件进行标记化，并将结果赋给tok2
  tok = append_tokens(tok, tok2);  // 将tok2连接到tok链表的末尾，并将结果赋给tok
  tok = preprocess(tok);  // 对tok链表进行预处理

  // 如果 -M 或 -MD 被指定，则打印文件依赖项。
  if (opt_M || opt_MD) {  // 如果给出了-M选项或-MD选项
    print_dependencies();  // 打印文件依赖关系
    if (opt_M)
      return;  // 如果是-M选项，则直接返回
  }

  // 如果 -E 被指定，则打印出预处理后的 C 代码作为结果。
  if (opt_E) {  // 如果给出了-E选项
    print_tokens(tok);  // 打印预处理后的C代码
    return;  // 返回
  }

  Obj *prog = parse(tok);  // 调用parse函数对tok链表进行解析，并将结果赋给prog

  // 打开一个临时输出缓冲区。
  char *buf;  // 声明一个指向字符的指针变量buf
  size_t buflen;  // 声明一个size_t类型的变量buflen
  FILE *output_buf = open_memstream(&buf, &buflen);  // 打开一个临时内存流文件，并将其指针赋给output_buf，同时将buf和buflen的地址传递给open_memstream函数

  // 遍历 AST，发射汇编。
  codegen(prog, output_buf);  // 调用codegen函数遍历AST并将汇编代码输出到output_buf中
  fclose(output_buf);  // 关闭output_buf文件流

  // 将汇编文本写入一个文件中。
  FILE *out = open_file(output_file);  // 打开一个输出文件流，并将其指针赋给out
  fwrite(buf, buflen, 1, out);  // 将buf中的汇编代码写入out文件
  fclose(out);  // 关闭out文件流
}

static void assemble(char *input, char *output) {
  char *cmd[] = {"as", "-c", input, "-o", output, NULL};  // 声明一个字符串数组cmd，存储as命令及其参数
  run_subprocess(cmd);  // 调用run_subprocess函数运行子进程，执行cmd中的命令
}

static char *find_file(char *pattern) {
  char *path = NULL;  // 声明一个指向字符的指针变量path，并将其初始化为空
  glob_t buf = {};  // 声明一个glob_t类型的变量buf，并将其初始化为0
  glob(pattern, 0, NULL, &buf);  // 使用glob函数匹配文件名模式pattern，并将匹配结果存储在buf中
  if (buf.gl_pathc > 0)
    path = strdup(buf.gl_pathv[buf.gl_pathc - 1]);  // 如果匹配结果不为空，则将最后一个匹配的路径存储在path中（复制一份）
  globfree(&buf);  // 释放buf占用的资源
  return path;  // 返回path
}

// 判断给定文件是否存在,若存在返回真
bool file_exists(char *path) {
  struct stat st;  // 声明一个struct stat类型的变量st，用于存储文件信息
  return !stat(path, &st);  // 调用stat函数获取文件的信息，并将结果返回，如果文件存在则返回真，否则返回假
}

static char *find_libpath(void) {
  if (file_exists("/usr/lib/x86_64-linux-gnu/crti.o"))  // 如果"/usr/lib/x86_64-linux-gnu/crti.o"文件存在
    return "/usr/lib/x86_64-linux-gnu";  // 返回"/usr/lib/x86_64-linux-gnu"
  if (file_exists("/usr/lib64/crti.o"))  // 如果"/usr/lib64/crti.o"文件存在
    return "/usr/lib64";  // 返回"/usr/lib64"
  error("library path is not found");  // 报错，打印错误信息"library path is not found"
}

static char *find_gcc_libpath(void) {
  char *paths[] = {
    "/usr/lib/gcc/x86_64-linux-gnu/*/crtbegin.o",  // gcc库路径模式1
    "/usr/lib/gcc/x86_64-pc-linux-gnu/*/crtbegin.o",  // gcc库路径模式2，用于Gentoo
    "/usr/lib/gcc/x86_64-redhat-linux/*/crtbegin.o",  // gcc库路径模式3，用于Fedora
  };

  for (int i = 0; i < sizeof(paths) / sizeof(*paths); i++) {  // 遍历路径数组
    char *path = find_file(paths[i]);  // 调用find_file函数查找匹配路径
    if (path)
      return dirname(path);  // 如果找到匹配路径，则返回其所在目录名
  }

  error("gcc library path is not found");  // 报错，打印错误信息"gcc library path is not found"
}

static void run_linker(StringArray *inputs, char *output) {
  StringArray arr = {};  // 声明一个StringArray结构体变量arr，并初始化为空

  strarray_push(&arr, "ld");  // 将"ld"字符串推入arr
  strarray_push(&arr, "-o");  // 将"-o"字符串推入arr
  strarray_push(&arr, output);  // 将output字符串推入arr
  strarray_push(&arr, "-m");  // 将"-m"字符串推入arr
  strarray_push(&arr, "elf_x86_64");  // 将"elf_x86_64"字符串推入arr

  char *libpath = find_libpath();  // 调用find_libpath函数获取库路径并赋值给libpath
  char *gcc_libpath = find_gcc_libpath();  // 调用find_gcc_libpath函数获取gcc库路径并赋值给gcc_libpath

  if (opt_shared) {  // 如果opt_shared为真
    strarray_push(&arr, format("%s/crti.o", libpath));  // 根据libpath拼接路径，并将结果推入arr
    strarray_push(&arr, format("%s/crtbeginS.o", gcc_libpath));  // 根据gcc_libpath拼接路径，并将结果推入arr
  } else {
    strarray_push(&arr, format("%s/crt1.o", libpath));  // 根据libpath拼接路径，并将结果推入arr
    strarray_push(&arr, format("%s/crti.o", libpath));  // 根据libpath拼接路径，并将结果推入arr
    strarray_push(&arr, format("%s/crtbegin.o", gcc_libpath));  // 根据gcc_libpath拼接路径，并将结果推入arr
  }

  strarray_push(&arr, format("-L%s", gcc_libpath));  // 将"-L"和gcc_libpath拼接成新的字符串，并将结果推入arr
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");  // 将"-L/usr/lib/x86_64-linux-gnu"字符串推入arr
  strarray_push(&arr, "-L/usr/lib64");  // 将"-L/usr/lib64"字符串推入arr
  strarray_push(&arr, "-L/lib64");  // 将"-L/lib64"字符串推入arr
  strarray_push(&arr, "-L/usr/lib/x86_64-linux-gnu");  // 将"-L/usr/lib/x86_64-linux-gnu"字符串推入arr
  strarray_push(&arr, "-L/usr/lib/x86_64-pc-linux-gnu");  // 将"-L/usr/lib/x86_64-pc-linux-gnu"字符串推入arr
  strarray_push(&arr, "-L/usr/lib/x86_64-redhat-linux");  // 将"-L/usr/lib/x86_64-redhat-linux"字符串推入arr
  strarray_push(&arr, "-L/usr/lib");  // 将"-L/usr/lib"字符串推入arr
  strarray_push(&arr, "-L/lib");  // 将"-L/lib"字符串推入arr

  if (!opt_static) {  // 如果opt_static为假
    strarray_push(&arr, "-dynamic-linker");  // 将"-dynamic-linker"字符串推入arr
    strarray_push(&arr, "/lib64/ld-linux-x86-64.so.2");  // 将"/lib64/ld-linux-x86-64.so.2"字符串推入arr
  }

  for (int i = 0; i < ld_extra_args.len; i++)  // 遍历ld_extra_args数组
    strarray_push(&arr, ld_extra_args.data[i]);  // 将ld_extra_args数组元素推入arr

  for (int i = 0; i < inputs->len; i++)  // 遍历inputs指针所指向的数组
    strarray_push(&arr, inputs->data[i]);  // 将inputs数组元素推入arr

  if (opt_static) {  // 如果opt_static为真
    strarray_push(&arr, "--start-group");  // 将"--start-group"字符串推入arr
    strarray_push(&arr, "-lgcc");  // 将"-lgcc"字符串推入arr
    strarray_push(&arr, "-lgcc_eh");  // 将"-lgcc_eh"字符串推入arr
    strarray_push(&arr, "-lc");  // 将"-lc"字符串推入arr
    strarray_push(&arr, "--end-group");  // 将"--end-group"字符串推入arr
  } else {
    strarray_push(&arr, "-lc");  // 将"-lc"字符串推入arr
    strarray_push(&arr, "-lgcc");  // 将"-lgcc"字符串推入arr
    strarray_push(&arr, "--as-needed");  // 将"--as-needed"字符串推入arr
    strarray_push(&arr, "-lgcc_s");  // 将"-lgcc_s"字符串推入arr
    strarray_push(&arr, "--no-as-needed");  // 将"--no-as-needed"字符串推入arr
  }

  if (opt_shared)  // 如果opt_shared为真
    strarray_push(&arr, format("%s/crtendS.o", gcc_libpath));  // 根据gcc_libpath拼接路径，并将结果推入arr
  else
    strarray_push(&arr, format("%s/crtend.o", gcc_libpath));  // 根据gcc_libpath拼接路径，并将结果推入arr

  strarray_push(&arr, format("%s/crtn.o", libpath));  // 根据libpath拼接路径，并将结果推入arr
  strarray_push(&arr, NULL);  // 将NULL指针推入arr

  run_subprocess(arr.data);  // 调用run_subprocess函数，传递arr.data作为参数
}

static FileType get_file_type(char *filename) {
  if (opt_x != FILE_NONE)  // 如果opt_x不等于FILE_NONE
    return opt_x;  // 返回opt_x的值

  if (endswith(filename, ".a"))  // 如果文件名以".a"结尾
    return FILE_AR;  // 返回FILE_AR类型
  if (endswith(filename, ".so"))  // 如果文件名以".so"结尾
    return FILE_DSO;  // 返回FILE_DSO类型
  if (endswith(filename, ".o"))  // 如果文件名以".o"结尾
    return FILE_OBJ;  // 返回FILE_OBJ类型
  if (endswith(filename, ".c"))  // 如果文件名以".c"结尾
    return FILE_C;  // 返回FILE_C类型
  if (endswith(filename, ".s"))  // 如果文件名以".s"结尾
    return FILE_ASM;  // 返回FILE_ASM类型

  error("<command line>: unknown file extension: %s", filename);  // 报错，未知的文件扩展名
}

int main(int argc, char **argv) {
  atexit(cleanup);  // 注册cleanup函数，在程序结束时执行清理操作
  init_macros();  // 初始化宏定义
  parse_args(argc, argv);  // 解析命令行参数

  if (opt_cc1) {  // 如果opt_cc1为真
    add_default_include_paths(argv[0]);  // 添加默认的包含路径
    cc1();  // 调用cc1函数
    return 0;  // 返回0表示正常退出
  }

  if (input_paths.len > 1 && opt_o && (opt_c || opt_S | opt_E))  // 如果输入文件路径数量大于1且同时指定了-o和'-c,' '-S'或'-E'
    error("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");  // 报错，不能在多个文件上同时使用'-o'和'-c,' '-S'或'-E'

  StringArray ld_args = {};  // 声明并初始化一个StringArray类型的变量ld_args

  for (int i = 0; i < input_paths.len; i++) {  // 遍历input_paths数组
    char *input = input_paths.data[i];  // 获取input_paths数组中的元素

    if (!strncmp(input, "-l", 2)) {  // 如果input字符串以"-l"开头的前两个字符相等
      strarray_push(&ld_args, input);  // 将input字符串推入ld_args数组
      continue;  // 跳过当前循环，继续下一次循环
    }

    if (!strncmp(input, "-Wl,", 4)) {  // 如果input字符串以"-Wl,"开头的前四个字符相等
      char *s = strdup(input + 4);  // 复制input字符串中从第五个字符开始的部分，并将结果赋值给s
      char *arg = strtok(s, ",");  // 使用逗号作为分隔符，将s字符串切割成多个部分，并将第一个部分赋值给arg
      while (arg) {  // 当arg不为NULL时循环
        strarray_push(&ld_args, arg);  // 将arg字符串推入ld_args数组
        arg = strtok(NULL, ",");  // 继续获取下一个部分赋值给arg
      }
      continue;  // 跳过当前循环，继续下一次循环
    }

    char *output;
    if (opt_o)  // 如果opt_o不为NULL
      output = opt_o;  // 将opt_o赋值给output
    else if (opt_S)  // 如果opt_S为真
      output = replace_extn(input, ".s");  // 将input的扩展名替换为".s"，并将结果赋值给output
    else
      output = replace_extn(input, ".o");  // 将input的扩展名替换为".o"，并将结果赋值给output

    FileType type = get_file_type(input);  // 调用get_file_type函数获取文件类型，并将结果赋值给type

    // 处理 .o 或 .a
    if (type == FILE_OBJ || type == FILE_AR || type == FILE_DSO) {  // 如果type等于FILE_OBJ、FILE_AR或FILE_DSO中的任何一个
      strarray_push(&ld_args, input);  // 将input字符串推入ld_args数组
      continue;  // 跳过当前循环，继续下一次循环
    }

    // 处理 .s
    if (type == FILE_ASM) {  // 如果type等于FILE_ASM
      if (!opt_S)  // 如果opt_S为假
        assemble(input, output);  // 调用assemble函数，将input和output作为参数
      continue;  // 跳过当前循环，继续下一次循环
    }

    assert(type == FILE_C);  // 断言type等于FILE_C

    // 只进行预处理
    if (opt_E || opt_M) {  // 如果opt_E或opt_M为真
      run_cc1(argc, argv, input, NULL);  // 调用run_cc1函数，将argc、argv、input和NULL作为参数
      continue;  // 跳过当前循环，继续下一次循环
    }

    // 编译
    if (opt_S) {  // 如果opt_S为真
      run_cc1(argc, argv, input, output);  // 调用run_cc1函数，将argc、argv、input和output作为参数
      continue;  // 跳过当前循环，继续下一次循环
    }

    // 编译和汇编
    if (opt_c) {  // 如果opt_c为真
      char *tmp = create_tmpfile();  // 创建一个临时文件，并将文件名赋值给tmp
      run_cc1(argc, argv, input, tmp);  // 调用run_cc1函数，将argc、argv、input和tmp作为参数
      assemble(tmp, output);  // 调用assemble函数，将tmp和output作为参数
      continue;  // 跳过当前循环，继续下一次循环
    }

    // 编译、汇编和链接
    char *tmp1 = create_tmpfile();  // 创建一个临时文件，并将文件名赋值给tmp1
    char *tmp2 = create_tmpfile();  // 创建一个临时文件，并将文件名赋值给tmp2
    run_cc1(argc, argv, input, tmp1);  // 调用run_cc1函数，将argc、argv、input和tmp1作为参数
    assemble(tmp1, tmp2);  // 调用assemble函数，将tmp1和tmp2作为参数
    strarray_push(&ld_args, tmp2);  // 将tmp2字符串推入ld_args数组
    continue;  // 跳过当前循环，继续下一次循环
  }

  if (ld_args.len > 0)  // 如果ld_args数组的长度大于0
    run_linker(&ld_args, opt_o ? opt_o : "a.out");  // 调用run_linker函数，将ld_args数组和opt_o或"a.out"作为参数
  return 0;  // 返回0
}

