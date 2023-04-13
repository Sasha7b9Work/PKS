// 2023/04/07 17:11:37 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


template<class T, int size_buffer>
class Buffer
{
public:

    Buffer() : size(size_buffer) {}; //-V730

    Buffer(int _size) : size(size_buffer) //-V730
    {
        Realloc(_size);
    }

    Buffer(int _size, T value) : size(size_buffer)
    {
        Realloc(_size);
        Fill(value);
    }

    void Fill(T value)
    {
        for (int i = 0; i < size; i++)
        {
            buffer[i] = value;
        }
    }

    T *Data() { return buffer; }

    const T *DataConst() const { return buffer; }

    void Realloc(int _size)
    {
        size = _size;

        if (size > size_buffer)
        {
            size = size_buffer;
        }
    }

    void ReallocAndFill(int _size, T value)
    {
        Realloc(_size);

        for (int i = 0; i < size; i++)
        {
            buffer[i] = value;
        }
    }

    // Перевыделить память и заполнить её из buffer
    void ReallocFromBuffer(const T *in, int _size)
    {
        size = _size;

        if (size > size_buffer)
        {
            size = size_buffer;
        }

        for (int i = 0; i < size; i++)
        {
            buffer[i] = in[i];
        }
    }

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
