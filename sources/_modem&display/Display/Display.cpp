// 2023/03/29 20:05:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Display/Display.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"

extern FontDef Font_7x10;

void Display::Init()
{
    SSD1306::Init();

    SSD1306::DrawBitmap(0,0,garfield_128x64,128,64);
	  SSD1306::UpdateScreen();

	  uint8 y = 0;
	  SSD1306::Fill();

    #ifdef SSD1306_INCLUDE_FONT_16x26
    ssd1306_SetCursor(2, y);
    ssd1306_WriteString("Font 16x26", Font_16x26, White);
    y += 26;
    #endif

    #ifdef SSD1306_INCLUDE_FONT_11x18
    ssd1306_SetCursor(2, y);
    ssd1306_WriteString("Font 11x18", Font_11x18, White);
    y += 18;
    #endif

	  SSD1306::SetCursor(2, y);
    SSD1306::WriteString("Font 7x10", Font_7x10);

    #ifdef SSD1306_INCLUDE_FONT_6x8
    ssd1306_SetCursor(2, y);
    ssd1306_WriteString("Font 6x8", Font_6x8, White);
    #endif

    SSD1306::UpdateScreen();
}


void Display::Update()
{
    static int counter = 0;
    
    counter++;
    
    if(counter > 1000)
    {
        counter = 0;
    }
    
    SSD1306::UpdateScreen();
}


void Display::Fill(void)
{
    SSD1306::Fill();
}
