// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"


void Display_Init(void)
{
    SSD1306_Init();
}


void Display_Update(void)
{
    static enum Color color = Black;
    
    static int counter = 0;
    
    counter++;
    
    if(counter > 1000)
    {
        counter = 0;
        
        color = (color == Black) ? White : Black;
    }
    
    Display::Fill(color);
}


void Display::Fill(enum Color color)
{
    SSD1306_Fill((color == White) ? 0xff : 0x00);
}
