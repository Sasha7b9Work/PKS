// 2023/03/30 10:55:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Modules/SSD1306/SSD1306.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <cstdlib>


namespace SSD1306
{
#define SSD1306_ADDR              0x3C

#define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
#define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
#define SSD1306_DATA              0xC0  // Continuation bit=1, D/C=1; 1100 0000
#define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000


#define SIZE_BUFFER  1024
    static uint8 buffer[SIZE_BUFFER];

    static void SendCommand(uint8);
}


void SSD1306::SendCommand(uint8 command)
{
    HAL_I2C::Write8(SSD1306_COMMAND);
    HAL_I2C::Write8(command);
}


void SSD1306::Init()
{
    SendCommand(0xAE); // Set display OFF
 
    SendCommand(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
    SendCommand(0x80); // Display Clock Divide Ratio / OSC Frequency 
 
    SendCommand(0xA8); // Set Multiplex Ratio
    SendCommand(0x3F); // Multiplex Ratio for 128x64 (64-1)
 
    SendCommand(0xD3); // Set Display Offset
    SendCommand(0x00); // Display Offset
 
    SendCommand(0x40); // Set Display Start Line
 
    SendCommand(0x8D); // Set Charge Pump
    SendCommand(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)
 
    SendCommand(0xA1); // Set Segment Re-Map
    SendCommand(0xC8); // Set Com Output Scan Direction
 
    SendCommand(0xDA); // Set COM Hardware Configuration
    SendCommand(0x12); // COM Hardware Configuration
 
    SendCommand(0x81); // Set Contrast
    SendCommand(0xCF); // Contrast
 
    SendCommand(0xD9); // Set Pre-Charge Period
    SendCommand(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)
 
    SendCommand(0xDB); // Set VCOMH Deselect Level
    SendCommand(0x40); // VCOMH Deselect Level
 
    SendCommand(0xA4); // Set all pixels OFF
    SendCommand(0xA6); // Set display not inverted
    SendCommand(0xAF); // Set display On
}


void SSD1306::WriteBuffer()
{
    HAL_I2C::Write8(SSD1306_DATA_STREAM);

    HAL_I2C::Write(buffer, SIZE_BUFFER);
}
