#include "Validation.h"

int checkArgsIs(const char* name, int expected, int got)
{
    MAL_CHECK(got == expected,
           "\"%s\" 期望 %d 实参%s, %d 已提供",
           name, expected, PLURAL(expected), got);
    return got;
}

int checkArgsBetween(const char* name, int min, int max, int got)
{
    MAL_CHECK((got >= min) && (got <= max),
           "\"%s\" 期望介于 %d 与 %d 实参%s 之间, %d 已提供",
           name, min, max, PLURAL(max), got);
    return got;
}

int checkArgsAtLeast(const char* name, int min, int got)
{
    MAL_CHECK(got >= min,
           "\"%s\" 期望至少 %d 实参%s, %d 已提供",
           name, min, PLURAL(min), got);
    return got;
}

int checkArgsEven(const char* name, int got)
{
    MAL_CHECK(got % 2 == 0,
           "\"%s\" 期望一个实参的偶数数字, %d 已提供",
           name, got);
    return got;
}
