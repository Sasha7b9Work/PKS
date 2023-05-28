// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/Mutex.h"
#include <cstring>


template<class T, int capacity>
class Buffer
{
public:

    Buffer() : size(0) {}; //-V730

    T *Data() { return buffer; }

    const T *DataConst() const { return buffer; }

    T *Last()
    {
        return buffer + Size();
    }

    // Возвращает количество элементов в буфере
    int Size() const
    {
        return size;
    }

    void Clear()
    {
        size = 0;
    }

    void Append(const void *data, int _size)
    {
        if (Size() + _size > capacity)
        {
        }
        else
        {
            std::memcpy(&buffer[size], data, (uint)_size);
            size += _size;
        }
    }

    void Append(T symbol)
    {
        if (size < capacity)
        {
            buffer[size++] = symbol;
        }
        else
        {
            LOG_WRITE("buffer is full");
        }
    }

    // Удалить первых n элементов
    void RemoveFirst(int n)
    {
        std::memmove(buffer, buffer + n, n * sizeof(T));
        size -= n;
    }

    T &operator[](uint i)
    {
        if ((int)i >= 0 && (int)i < Size())
        {
            return buffer[i];
        }

        static T null(0);

        return null;
    }

    T &operator[](int i)
    {
        if (i >= 0 && i < Size())
        {
            return buffer[i];
        }

        static T null(0);

        return null;
    }

    Mutex mutex;

protected:

    int size;

    T buffer[capacity];
};
