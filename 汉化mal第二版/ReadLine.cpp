#include "ReadLine.h"
#include "String.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <readline/tilde.h>

ReadLine::ReadLine(const 字符串_大写& historyFile)
: m_historyPath(copyAndFree(tilde_expand(historyFile.c_str())))
{
    read_history(m_historyPath.c_str());
}

ReadLine::~ReadLine()
{
}

bool ReadLine::get(const 字符串_大写& prompt, 字符串_大写& out)
{
    char *line = readline(prompt.c_str());
    if (line == NULL) {
        return false;
    }
    add_history(line); // Add input to in-memory history
    append_history(1, m_historyPath.c_str());

    out = line;
    free(line);

    return true;
}
