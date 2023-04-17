// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"
#include "Measurer/Measurer.h"
#include <cstring>
#include <cstdio>

#include "Display/Font/font10_7.inc"


namespace Display
{
    static const uint SIZE_BUFFER = 1024;
    static uint8 buffer[SIZE_BUFFER];

    static int cursorX = 0;
    static int cursorY = 0;

    static FontDef font_10x7 = { 7, 10, Font10x7 };

    static void WriteVoltage(int i);

    static void WriteAmpere(int i);
}


namespace Font
{
    extern TypeFont::E type;
}


void Display::Init()
{
    SSD1306::Init();
}


void Display::Update()
{
    FullMeasure measure = Measurer::LastMeasure();

    BeginScene();

    for (int i = 0; i < 3; i++)
    {
        WriteVoltage(i);

        WriteAmpere(i);
    }

    SSD1306::WriteBuffer(buffer);
}


void Display::WriteVoltage(int i)
{
    static int counter = 0;
    counter++;

    FullMeasure measure = Measurer::LastMeasure();

    char message[30];

    std::sprintf(message, "%3.1f V", measure.measures[i].voltage + (float)counter);

    WriteString(10, 10 + i * 20, message);
}


void Display::WriteAmpere(int i)
{
    static int counter = 0;
    counter++;

    FullMeasure measure = Measurer::LastMeasure();

    char message[30];

    std::sprintf(message, "%3.1f A", measure.measures[i].current + (float)counter);

    WriteString(64, 10 + i * 20, message);

}


void Display::BeginScene()
{
    std::memset(buffer, 0, SIZE_BUFFER);
}


void Display::DrawPixel(int x, int y, int color)
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


char Display::WriteString(int x, int y, char *str)
{
    cursorX = x;
    cursorY = y;

    while (*str)
    {
        if (WriteChar(*str) != *str)
        {
            return *str;
        };

        str++;
    };

    return *str;
}


char Display::WriteChar(char ch)
{
    if (Font::type == TypeFont::_5 || Font::type == TypeFont::_7)
    {
        uint8 symbol = (uint8)ch;

        int height = Font::GetHeight();
        int width = Font::GetWidth(symbol);

        for (int row = 0; row < height; row++)
        {
            for (int col = 0; col < width; col++)
            {
                if (Font::GetBit(symbol, row, col))
                {
                    DrawPixel(cursorX + col, cursorY + row, 1);
                }
            }
        }

        cursorX += width + 1;
    }
    else if (Font::type == TypeFont::_10)
    {
        if (ch < 32 || ch > 126)
        {
            return 0;
        }

        if (WIDTH < (cursorX + font_10x7.width) || HEIGHT < (cursorY + font_10x7.height))
        {
            return 0;
        };

        for (int i = 0; i < font_10x7.height; i++)
        {
            int b = font_10x7.data[(ch - 32) * font_10x7.height + i];

            for (int j = 0; j < font_10x7.width; j++)
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

        cursorX += font_10x7.width;
    }

    return ch;
}
