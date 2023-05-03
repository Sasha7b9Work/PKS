// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Buffer.h"
#include "Utils/String.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>


String::String() : buffer(nullptr)
{
    Set("");
}


String::String(pchar text) : buffer(nullptr)
{
    Set(text);
}


String::String(const String &text) : buffer(nullptr)
{
    Set(text.c_str());
}


void String::Set(pchar text)
{
    if (buffer && text[0] == '\0')
    {
        buffer[0] = '\0';

        return;
    }

    Free();

    buffer = (char *)std::malloc(std::strlen(text) + 1);

    std::strcpy(buffer, text);
}


String::~String()
{
    std::free(buffer);
}


void String::Free()
{
    if(buffer)
    {
        std::free(buffer);
        buffer = nullptr;
    }
}


int String::Size() const
{
    if (buffer == nullptr)
    {
        return 0;
    }

    return static_cast<int>(std::strlen(buffer));
}


char &String::operator[](int i) const
{
    static char result = 0;

    if (buffer == nullptr || Size() < i)
    {
        return result;
    }

    return buffer[i];
}


bool String::operator==(pchar rhs) const
{
    return std::strcmp(buffer, rhs) == 0;
}


bool String::operator!=(pchar rhs) const
{
    return std::strcmp(buffer, rhs) != 0;
}
