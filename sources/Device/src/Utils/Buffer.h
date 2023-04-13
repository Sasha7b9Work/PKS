// 2023/04/07 17:11:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


template<class T, int size_buffer>
class Buffer
{
public:

    Buffer() : size(size_buffer) {}; //-V730

    T *Data() { return buffer; }

    T &operator[](int i)
    {
        if (i >= 0 && i < size_buffer)
        {
            return buffer[i];
        }

        static T null(0);

        return null;
    }

protected:

    int size;

    T buffer[size_buffer];
};
