// 2024/01/16 11:51:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


template<int capacity>
class RingBuffer
{
public:

    RingBuffer() : pos_in(0), pos_out(0) { }

    bool IsEmpty() const
    {
        return pos_in == pos_out;
    }

    void Push(char symbol)
    {
        buffer[pos_in++] = symbol;

        if (pos_in == capacity)
        {
            pos_in = 0;
        }
    }

    char Pop()
    {
        return buffer[pos_out++];
    }

private:
    char buffer[capacity];
    int pos_in;                 // В эту позицию нужно ложить символ
    int pos_out;                // Из этой позиции нужно брать символ
};
