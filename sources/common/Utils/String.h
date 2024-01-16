// 2023/11/04 19:38:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>


template<int capacity = 64>
class String
{
public:
    String() : pointer(0)
    {
    }

    String(char *format, ...)
    {
        std::va_list args;
        va_start(args, format);
        std::vsprintf(buffer, format, args);
        va_end(args);

        pointer = std::strlen(buffer);
    }

    pchar c_str() const
    {
        return buffer;
    }

private:
    char buffer[capacity];
    uint pointer;
};
