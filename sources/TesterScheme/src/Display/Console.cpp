// 2023/09/17 20:07:32 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Console.h"
#include "Display/Display.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


namespace Console
{
    static const int MAX_LINES = 6;
    static int num_lines = 0;
    static char lines[MAX_LINES][128];
}


bool Console::IsEmpty()
{
    return num_lines == 0;
}


void Console::AppendLine(pchar line)
{
    if (num_lines == MAX_LINES)
    {
        for (int i = 1; i < MAX_LINES; i++)
        {
            std::strcpy(&lines[i - 1][0], &lines[i][0]);
        }

        num_lines = MAX_LINES - 1;
    }

    std::strcpy(&lines[num_lines++][0], line);
}


void Console::AppendLineF(char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    AppendLine(message);
}


void Console::Draw()
{
    int y = 0;

    for (int i = 0; i < num_lines; i++)
    {
        Display::WriteString(0, y, lines[i]);

        y += 63 / MAX_LINES;
    }
}
