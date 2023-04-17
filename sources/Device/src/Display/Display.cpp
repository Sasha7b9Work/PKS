// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"


#include "Display/Font/font10_7.inc"


namespace Display
{
    static const uint SIZE_BUFFER = 1024;
    static uint8 buffer[SIZE_BUFFER];

    static int cursorX = 0;
    static int cursorY = 0;

    static FontDef Font_7x10 = { 7, 10, Font7x10 };
} 


void Display::Init()
{
    SSD1306::Init();
}


void Display::Update()
{
    SSD1306::WriteBuffer(buffer);
}



void Display::DrawPixel(int x, int y, uint8 color)
{
    if (x >= WIDTH || y >= HEIGHT)
    {
        return;
    };

    if (color)
    {
        buffer[x + (y / 8) * WIDTH] |= 1 << (y % 8);
    }
    else
    {
        buffer[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
    }
}


void Display::SetCursor(int x, int y)
{
    cursorX = x;
    cursorY = y;
}


char Display::WriteString(char *str, const FontDef &font)
{
    while (*str)
    {
        if (WriteChar(*str, font) != *str)
        {
            return *str;
        };

        str++;
    };

    return *str;
}


char Display::WriteChar(char ch, const FontDef &font)
{
    if (ch < 32 || ch > 126)
    {
        return 0;
    }

    if (WIDTH < (cursorX + font.width) || HEIGHT < (cursorY + font.height))
    {
        return 0;
    };

    for (int i = 0; i < font.height; i++)
    {
        int b = font.data[(ch - 32) * font.height + i];

        for (int j = 0; j < font.width; j++)
        {
            if ((b << j) & 0x8000)
            {
                DrawPixel(cursorX + j, cursorY + i, 1);
            }
            else
            {
                DrawPixel(cursorX + j, cursorY + i, 0);
            };
        };
    };

    cursorX += font.width;

    return ch;
}
