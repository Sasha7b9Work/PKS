// 2023/4/30 18:04:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Parser.h"
#include <cstring>


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
