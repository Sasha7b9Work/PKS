// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Display/SSD1306.h"
#include "Measurer/Measurer.h"
#include "Modem/Modem.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/SIM800.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Display/Console.h"
#include "Measurer/Contactors.h"
#include "Utils/String.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "Display/Font/font10_7.inc"


namespace Display
{
    static const uint SIZE_BUFFER = 64 * 128 / 8;
    static uint8 buffer[SIZE_BUFFER];

    static int cursorX = 0;
    static int cursorY = 0;

    static FontDef font_10x7 = { 7, 10, Font10x7 };

#ifdef DEVICE
    static void WriteMeasures(int i);
#else
#endif
}


namespace Font
{
    extern TypeFont::E type;
}


void Display::Init()
{
    SSD1306::Init();

    Fill(1);

    Timer::DelayMS(250);

    Fill(2);

    Timer::DelayMS(250);

    Fill(0);

    Timer::DelayMS(250);
}


#ifdef DEVICE
void Display::Update()
{
    FullMeasure measure = Measurer::LastMeasure();

    BeginScene();

    for (int i = 0; i < 3; i++)
    {
        WriteMeasures(i);
    }

    char message[32];

    WriteString(72, 54, HAL::GetUID());

    WriteString(32, 54, message);

    if (Modem::Mode::Power())
    {
        WriteString(5, 0, "POW");
    }

    if (SIM800::IsRegistered())
    {
        WriteString(35, 0, "REG");
    }

    if (MQTT::InStateRunning())
    {
        WriteString(65, 0, "MQTT");

        WriteString(100, 0, SIM800::LevelSignal());
    }

    SSD1306::WriteBuffer(buffer);
}


void Display::WriteMeasures(int i)
{
#define Y() (13 + i * 13)

    char message[30];

    bool need_bad_info = !Contactors::Serviceability::AllIsOK((Phase::E)i) && (((Timer::TimeMS() / 1000 / 5) % 2) == 0);

    if (need_bad_info)
    {
        for (int num = 0; num < 8; num++)
        {
            WriteString(0, Y(), "Defect");

            if (Contactors::Serviceability::GetState((Phase::E)i, num) == -1)
            {
                std::sprintf(message, "%d", num + 1);

                WriteString(num * 10 + 50, Y(), message);
            }
        }
    }
    else
    {
        FullMeasure measure = Measurer::LastMeasure();

        std::sprintf(message, "%d", -Contactors::GetLevel((Phase::E)i));

        WriteString(0, Y(), message);

        std::sprintf(message, "%4.1f", measure.measures[i].voltage);

        WriteString(20, Y(), message);

        std::sprintf(message, "%4.1f", measure.measures[i].current);

        WriteString(80, Y(), message);
    }
}


#else


void Display::Update()
{
    BeginScene();

    if (Console::IsEmpty())
    {
        char message[64];

        std::sprintf(message, "Upgrade v %d", VERSION);

        Display::WriteString(0, 0, message);
    }
    else
    {
        Console::Draw();
    }

    SSD1306::WriteBuffer(buffer);
}


#endif


void Display::BeginScene()
{
    std::memset(buffer, 0, SIZE_BUFFER);
}


void Display::Fill(int color)
{
    if (color == 1)
    {
        std::memset(buffer, 0xFF, SIZE_BUFFER);
    }
    else if(color == 0)
    {
        std::memset(buffer, 0x00, SIZE_BUFFER);
    }
    else if (color == 2)
    {
        for (int i = 0; i < SIZE_BUFFER; i++)
        {
            buffer[i] = (uint8)std::rand();
        }
    }

    SSD1306::WriteBuffer(buffer);
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


char Display::WriteString(int x, int y, pchar str)
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
