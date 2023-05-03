// 2023/4/30 18:04:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Parser.h"
#include <cstring>


namespace Parser
{
    static int NumSeparators(const String &, int pos[10]);
}


int Parser::NumberSymbols(pchar string, char symbol)
{
    int result = 0;

    const char *p = string;

    while (*p)
    {
        if (*p == symbol)
        {
            result++;
        }
        p++;
    }

    return result;
}


int Parser::NumberSymbols(const String &string, char symbol)
{
    return NumberSymbols(string.c_str(), symbol);
}


int Parser::PositionSymbol(pchar string, char symbol, int num)
{
    int size = (int)std::strlen(string);

    for (int i = 0; i < size; i++)
    {
        if (string[i] == symbol)
        {
            num--;

            if (num == 0)
            {
                return i;
            }
        }
    }

    return -1;
}


int Parser::PositionSymbol(const String &string, char symbol, int num)
{
    return PositionSymbol(string.c_str(), symbol, num);
}


pchar Parser::GetWord(pchar string, char out[32], int pos_start, int pos_end)
{
    if (pos_end - pos_start <= 0)
    {
        LOG_WRITE("Wrong arguments");

        out[0] = '\0';
    }
    else
    {
        char *p = out;

        for (int i = pos_start + 1; i < pos_end; i++)
        {
            *p++ = string[i];
        }

        *p = '\0';
    }

    return out;
}


String Parser::GetWord(const String &string, int pos_start, int pos_end)
{
    char word[32];

    return String(GetWord(string.c_str(), word, pos_start, pos_end));
}


String Parser::GetWord(const String &string, int num)
{
    int pos[10];

    int num_sep = NumSeparators(string, pos);

    if (num_sep < num)
    {
        return String("");
    }

    if (num == 1)
    {
        return GetWord(string, -1, pos[0]);
    }

    return GetWord(string, pos[num - 2], pos[num - 1]);
}


int Parser::NumSeparators(const String &string, int pos[10])
{
    int size = string.Size();

    char *p = string.c_str();

    int num_sep = 0;

    for (int i = 0; i < size; i++)
    {
        if (*p == ' ' || *p == ',' || *p == ':')
        {
            pos[num_sep++] = i;
        }
        p++;
    }

    pos[num_sep++] = size;

    return num_sep;
}
