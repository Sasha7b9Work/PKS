// 2023/03/21 09:58:21 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Display/Font/Font.h"


namespace Display
{
    static const int WIDTH = 128;
    static const int HEIGHT = 64;

    void Init();

    void Update();

    void BeginScene();

    void DrawPixel(int x, int y, int color);

    char WriteString(int x, int y, char *str);

    char WriteChar(char ch);
}
