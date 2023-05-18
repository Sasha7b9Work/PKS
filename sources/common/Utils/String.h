// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"
#include <cstdarg>


class String // -V690
{
public:
    explicit String();
    explicit String(pchar);
             String(const String &);
    ~String();

    void Set(pchar);

    char *c_str() const { return buffer; };

    void Free();

    int Size() const;

    bool IsEmpty() const { return Size() == 0; }

    char &operator[](int i) const;

    bool operator==(pchar) const;

    bool operator!=(pchar) const;

    String operator=(const String &rhs)
    {
        Set(rhs.c_str());

        return String(*this);
    }

private:

    char *buffer;
};
