// 2023/03/30 10:55:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"
#include "Hardware/HAL/HAL.h"
#include <string.h>
#include <stdlib.h>


#define SSD1306_ADDR              0x3C

#define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
#define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
#define SSD1306_DATA              0xC0  // Continuation bit=1, D/C=1; 1100 0000
#define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000


#define CACHE_SIZE_MEM  1024
static uint8 buffer[CACHE_SIZE_MEM];


static void SSD1306_SendCommand(uint8 command)
{
    HAL_I2C0::Write8(SSD1306_COMMAND);
    HAL_I2C0::Write8(command);
}


void SSD1306_Init(void)
{
    SSD1306_SendCommand(0xAE); // Set display OFF
 
    SSD1306_SendCommand(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
    SSD1306_SendCommand(0x80); // Display Clock Divide Ratio / OSC Frequency 
 
    SSD1306_SendCommand(0xA8); // Set Multiplex Ratio
    SSD1306_SendCommand(0x3F); // Multiplex Ratio for 128x64 (64-1)
 
    SSD1306_SendCommand(0xD3); // Set Display Offset
    SSD1306_SendCommand(0x00); // Display Offset
 
    SSD1306_SendCommand(0x40); // Set Display Start Line
 
    SSD1306_SendCommand(0x8D); // Set Charge Pump
    SSD1306_SendCommand(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)
 
    SSD1306_SendCommand(0xA1); // Set Segment Re-Map
    SSD1306_SendCommand(0xC8); // Set Com Output Scan Direction
 
    SSD1306_SendCommand(0xDA); // Set COM Hardware Configuration
    SSD1306_SendCommand(0x12); // COM Hardware Configuration
 
    SSD1306_SendCommand(0x81); // Set Contrast
    SSD1306_SendCommand(0xCF); // Contrast
 
    SSD1306_SendCommand(0xD9); // Set Pre-Charge Period
    SSD1306_SendCommand(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)
 
    SSD1306_SendCommand(0xDB); // Set VCOMH Deselect Level
    SSD1306_SendCommand(0x40); // VCOMH Deselect Level
 
    SSD1306_SendCommand(0xA4); // Set all pixels OFF
    SSD1306_SendCommand(0xA6); // Set display not inverted
    SSD1306_SendCommand(0xAF); // Set display On
}


static void SSD1306_Update(void)
{
    HAL_I2C0::Write8(SSD1306_DATA_STREAM);
    
    HAL_I2C0::Write(buffer, CACHE_SIZE_MEM);
}   


void SSD1306_Fill(uint color)
{
    memset(buffer, (uint8)color, CACHE_SIZE_MEM);
    
    SSD1306_Update();
}
