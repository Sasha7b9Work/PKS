// 2023/03/30 10:55:38 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

#ifndef SSD1306_H
#define SSD1306_H

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0

typedef struct {
	const uint8 FontWidth;  /*!< Font width in pixels */
	uint8 FontHeight;   		/*!< Font height in pixels */
	const uint16 *data; 		/*!< Pointer to data font data array */
} FontDef;

namespace SSD1306
{
    void SendCommand(uint8 command);
	
    void SendData(uint8* buffer, uint8 buff_size);
	
  	void Init();

    void UpdateScreen();
	
    void Fill();
	
    void DrawPixel(uint8 x, uint8 y, uint8 color);
	
    void DrawBitmap(uint8 x, uint8 y, const unsigned char* bitmap, uint8 w, uint8 h);
	
    void SetCursor(uint8 x, uint8 y);
	
    char WriteString(char* str, FontDef Font);
	
    char WriteChar(char ch, FontDef Font);
}

#endif
