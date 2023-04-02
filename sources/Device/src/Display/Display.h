// 2023/03/21 09:58:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct Color
{
    enum E
    {
        Black,
        White,
        Count
    };


    static const uint16 colors[Count];
};


namespace Display
{
    static const int WIDTH = 128;
    static const int HEIGHT = 64;

    void Init();
}


#define BLUE_FROM_COLOR(color)  ((color) & 0x1f)
#define GREEN_FROM_COLOR(color) (((color) >> 5) & 0x3f)
#define RED_FROM_COLOR(color)   (((color) >> 11) & 0x1f)
