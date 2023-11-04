// 2022/11/06 19:23:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstring>


template<class T>
class Averager //-V730
{
public:
    Averager() : num_elements(0) {}
    T Push(T value)
    {
        if (num_elements == SIZE_BUFFER)
        {
            if (SIZE_BUFFER != 1)
            {
                std::memmove(buffer, buffer + 1, sizeof(T) * (SIZE_BUFFER - 1));
            }
            num_elements--;
        }
        buffer[num_elements++] = value;

        return Get();
    }

    T Pop(int index)
    {
        return buffer[index];
    }

    T Get()
    {
        T sum = 0;

        for (int i = 0; i < num_elements; i++)
        {
            sum += buffer[i];
        }

        return (T)(sum / (T)num_elements);
    }
    int NumElements() const { return num_elements; }
    void Reset() { num_elements = 0; }
private:
    static const int SIZE_BUFFER = 10;
    T buffer[SIZE_BUFFER];
    int num_elements;
};
