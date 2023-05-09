// 2023/04/13 12:00:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Math
{
    template<class T>
    T CircularIncrease(T value, T min, T max)
    {
        T result = value + 1;

        return (result < max) ? result : min;
    }

    template<class T>
    T Limitation(T value, T min, T max)
    {
        if (value < min)
        {
            return min;
        }

        if (value > max)
        {
            return max;
        }

        return value;
    }
}
