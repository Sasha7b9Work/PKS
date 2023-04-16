// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"


void Display::Init()
{
    SSD1306::Init();
}


void Display::Update()
{
    static enum Color color = Black;
    
    static int counter = 0;
    
    counter++;
    
    if(counter > 1000)
    {
        counter = 0;
        
        color = (color == Black) ? White : Black;
    }

    SSD1306::WriteBuffer();
}
