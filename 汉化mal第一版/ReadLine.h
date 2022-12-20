#ifndef INCLUDE_READLINE_H
#define INCLUDE_READLINE_H

#include "String.h"

class ReadLine {
public:
    ReadLine(const 字符串_大写& historyFile);
    ~ReadLine();

    bool 获取_小写(const 字符串_大写& prompt, 字符串_大写& line);

private:
    字符串_大写 m_historyPath;
};

#endif // INCLUDE_READLINE_H
