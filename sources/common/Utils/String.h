// 2023/11/04 19:38:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstdarg>
#include <cstdio>
#include <cstring>


class String
{
public:
    String() : buffer(nullptr)
    {
        buffer = nullptr;
    }

    String(char *format, ...) : buffer(nullptr)
    {
        char data[2048];

        std::va_list args;
        va_start(args, format);
        std::vsprintf(data, format, args);
        va_end(args);

        Set(data);
    }

    String(const String &string)
    {
        Clear();

        Set(string.c_str());
    }

    ~String()
    {
        Clear();
    }

    void Clear()
    {
        if (buffer)
        {
            delete buffer;
            buffer = nullptr;
        }
    }

    void Set(pchar string)
    {
        Clear();

        buffer = new char[std::strlen(string) + 1];
        std::strcpy(buffer, string);
    }

    char *c_str() const
    {
        return buffer;
    }

    operator char *()
    {
        return buffer;
    }

private:
    char *buffer;
};
