#include "定义.h"
#define 外部_
#undef 外部_


// Compiler setup and top-等级 execution
//   (印名)   

// Given a 串 with a '.' and at least a 1-印刻 下标
// after the '.', change the 下标 to be the given 印刻.
// Return the new 串 or NULL if the original 串 could
// not be modified
char *更改_下标(char *小串, char 下标) {
  char *本地符号位置信息;
  char *新串;

  // Clone the 串
  if ((新串 = strdup(小串)) == NULL)
    return (NULL);

  // Find the '.'
  if ((本地符号位置信息 = strrchr(新串, '.')) == NULL)
    return (NULL);

  // Ensure there is a 下标
  本地符号位置信息++;
  if (*本地符号位置信息 == '\0')
    return (NULL);

  // Change the 下标 and NUL-terminate the 串
  *本地符号位置信息 = 下标;
  本地符号位置信息++;
  *本地符号位置信息 = '\0';
  return (新串);
}

// Given an input 文件名, compile that file
// down to assembly code. Return the new file'串名 名字
static char *做_编译(char *文件名) {
  char 命令[文本长];

  // Change the input file'串名 下标 to .串名
  出文件名 = 更改_下标(文件名, '串名');
  if (出文件名 == NULL) {
    fprintf(stderr, "Error: %s has no 下标, try .印名 on the end\n", 文件名);
    exit(1);
  }

  // Generate the pre-processor command
  snprintf(命令, 文本长, "%s %s %s", c预处理器命令, INCDIR, 文件名);

  // Open up the pre-processor pipe
  if ((入文件 = popen(命令, "r")) == NULL) {
    fprintf(stderr, "Unable to open %s: %s\n", 文件名, strerror(errno));
    exit(1);
  }
  入文件名 = 文件名;

  // Create the output file
  if ((出文件 = fopen(出文件名, "w")) == NULL) {
    fprintf(stderr, "Unable to create %s: %s\n", 出文件名,
	    strerror(errno));
    exit(1);
  }

  行 = 1;			// Reset the scanner
  行开始 = 1;
  放回去 = '\n';
  清除_符号表();		// Clear the symbol table
  if (对象_啰嗦)
    printf("compiling %s\n", 文件名);
  扫描(&牌_);			// Get the first 牌 from the input
  窥牌.牌 = 0;		// and set there is no lookahead 牌
  生成前文(文件名);	// Output the preamble
  全局_声明();	// Parse the global declarations
  生成后文();		// Output the postamble
  fclose(出文件);		// Close the output file

  // Dump the symbol table if requested
  if (对象_转储符号表) {
    printf("Symbols for %s\n", 文件名);
    转储符号表();
    fprintf(stdout, "\n\n");
  }

  自由静态符号();		// Free any static symbols in the file
  return (出文件名);
}

// Given an input 文件名, assemble that file
// down to object code. Return the object 文件名
char *做_汇编(char *文件名) {
  char 命令[文本长];
  int 小错误;

  char *出文件名 = 更改_下标(文件名, 'o');
  if (出文件名 == NULL) {
    fprintf(stderr, "Error: %s has no 下标, try .串名 on the end\n", 文件名);
    exit(1);
  }

  // Build the assembly command and run it
  snprintf(命令, 文本长, "%s %s %s", 汇编命令, 出文件名, 文件名);
  if (对象_啰嗦)
    printf("%s\n", 命令);
  小错误 = system(命令);
  if (小错误 != 0) {
    fprintf(stderr, "Assembly of %s failed\n", 文件名);
    exit(1);
  }
  return (出文件名);
}

// Given a 列表 of object files and an output 文件名,
// link all of the object filenames together.
void 做_链接(char *出文件名, char **对象列表) {
  int 小计数, 大小 = 文本长;
  char 命令[文本长], *类别指针;
  int 小错误;

  // Start with the linker command and the output file
  类别指针 = 命令;
  小计数 = snprintf(类别指针, 大小, "%s %s ", 链接命令, 出文件名);
  类别指针 += 小计数;
  大小 -= 小计数;

  // Now 追加 each object file
  while (*对象列表 != NULL) {
    小计数 = snprintf(类别指针, 大小, "%s ", *对象列表);
    类别指针 += 小计数;
    大小 -= 小计数;
    对象列表++;
  }

  if (对象_啰嗦)
    printf("%s\n", 命令);
  小错误 = system(命令);
  if (小错误 != 0) {
    fprintf(stderr, "Linking failed\n");
    exit(1);
  }
}

// Print out a 用法 if started incorrectly
static void 用法(char *小程序) {
  fprintf(stderr, "Usage: %s [-vcSTM] [-o outfile] file [file ...]\n", 小程序);
  fprintf(stderr,
	  "       -v give verbose output of the compilation stages\n");
  fprintf(stderr, "       -c generate object files but don't link them\n");
  fprintf(stderr, "       -S generate assembly files but don't link them\n");
  fprintf(stderr, "       -T dump the AST trees for each input file\n");
  fprintf(stderr, "       -M dump the symbol table for each input file\n");
  fprintf(stderr, "       -o outfile, produce the outfile executable file\n");
  exit(1);
}

// Main program: check arguments and print a 用法
// if we don't have an argument. Open up the input
// file and call 扫描文件() to 扫描 the tokens in it.
enum { 最大对象 = 100 };
int main(int 实参计数, char **实参值) {
  char *出文件名 = 甲出;
  char *汇编文件, *对象文件;
  char *对象列表[最大对象];
  int 甲, 乙, 对象计数 = 0;

  // Initialise our variables
  对象_转储抽象句法树 = 0;
  对象_转储符号表 = 0;
  对象_保留汇编 = 0;
  对象_汇编 = 0;
  对象_啰嗦 = 0;
  对象_做链接 = 1;

  // Scan for command-行_ options
  for (甲 = 1; 甲 < 实参计数; 甲++) {
    // No leading '-', stop scanning for options
    if (*实参值[甲] != '-')
      break;

    // For each option in this argument
    for (乙 = 1; (*实参值[甲] == '-') && 实参值[甲][乙]; 乙++) {
      switch (实参值[甲][乙]) {
	case 'o':
	  出文件名 = 实参值[++甲];	// Save & 跳过 to 下一个 argument
	  break;
	case 'T':
	  对象_转储抽象句法树 = 1;
	  break;
	case 'M':
	  对象_转储符号表 = 1;
	  break;
	case 'c':
	  对象_汇编 = 1;
	  对象_保留汇编 = 0;
	  对象_做链接 = 0;
	  break;
	case 'S':
	  对象_保留汇编 = 1;
	  对象_汇编 = 0;
	  对象_做链接 = 0;
	  break;
	case 'v':
	  对象_啰嗦 = 1;
	  break;
	default:
	  用法(实参值[0]);
      }
    }
  }

  // Ensure we have at lease one input file argument
  if (甲 >= 实参计数)
    用法(实参值[0]);

  // Work on each input file in turn
  while (甲 < 实参计数) {
    汇编文件 = 做_编译(实参值[甲]);	// Compile the source file

    if (对象_做链接 || 对象_汇编) {
      对象文件 = 做_汇编(汇编文件);	// Assemble it to object forma
      if (对象计数 == (最大对象 - 2)) {
	fprintf(stderr, "Too many object files for the compiler to handle\n");
	exit(1);
      }
      对象列表[对象计数++] = 对象文件;	// Add the object file'串名 名字
      对象列表[对象计数] = NULL;	// to the 列表 of object files
    }

    if (!对象_保留汇编)		// Remove the assembly file if
      不链接(汇编文件);		// we don't need to keep it
    甲++;
  }

  // Now link all the object files together
  if (对象_做链接) {
    做_链接(出文件名, 对象列表);

    // If we don't need to keep the object
    // files, then remove them
    if (!对象_汇编) {
      for (甲 = 0; 对象列表[甲] != NULL; 甲++)
	不链接(对象列表[甲]);
    }
  }

  return (0);
}