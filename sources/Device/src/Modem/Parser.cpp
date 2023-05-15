// 2023/4/30 18:04:35 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Parser.h"
#include <cstring>


namespace Parser
{
    int NumSeparators(const String &, int pos[10]);
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


String Parser::GetWordInQuotes(const String &string, int num)
{
    int size = string.Size();
    char *buffer = string.c_str();

    int quote1 = num * 2;

    int pos_quote1 = -1;
    int pos_quote2 = -2;

    for (int i = 0; i < size; i++)
    {
        if (buffer[i] == '\"')
        {
            if (quote1 == 0)
            {
                pos_quote1 = i;
                break;
            }
            quote1--;
        }
    }

    for (int i = pos_quote1 + 1; i < size; i++)
    {
        if (buffer[i] == '\"')
        {
            pos_quote2 = i;
            break;
        }
    }

    if (pos_quote1 >= 0 && pos_quote2 >= 0)
    {
        char data[64];

        char *pointer = data;

        for (int i = pos_quote1 + 1; i < pos_quote2 - pos_quote1; i++)
        {
            *pointer++ = buffer[i];
        }

        *pointer = '\0';

        return String(data);
    }

    return String("");
}


String Parser::GetWord(const String &string, int num)
{
    int pos_start = 0;
    int pos_end = 0;

    if (num == 1)
    {
        pos_start = -1;
    }
    else
    {
        char *p = string.c_str();

        int current_word = 1;

        bool in_word = (*p != ' ') && (*p != ',') && (*p != ':');

        while (current_word < num && *p)
        {
            if (*p == ' ' || *p == ',' || *p == ':')
            {
                if (in_word)
                {
                    in_word = false;
                }
            }
            else
            {
                if (!in_word)
                {
                    current_word++;
                    in_word = true;
                    if (current_word == num)
                    {
                        pos_start = p - string.c_str() - 1;
                        break;
                    }
                }
            }

            p++;
        }

        if (current_word != num)
        {
            return String("");
        }
    }

    pos_end = pos_start + 1;

    char *p = string.c_str() + pos_end;

    while (*p != '\0' && *p != ' ' && *p != ',' && *p != ':')
    {
        pos_end++;
        p++;
    }

    return GetWord(string, pos_start, pos_end);
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
