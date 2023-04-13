// 2023/04/07 17:06:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Modules/W25Q128/W25Q128.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Utils/Buffer.h"


/*
*   Block 0
*   Sector 0
*   Адреса 000000h - 0000FFh
*/

//                            page
#define PROGRAM_PAGE  0x02  /* 34 */
#define READ_DATA     0x03  /* 26 */
#define WRITE_DISABLE 0x04  /* 23 */
#define READ_STATUS_1 0x05  /* 24 */
#define WRITE_ENABLE  0x06  /* 22 */
#define SECTOR_ERASE  0x20  /* 36 */


namespace W25Q80DV
{
    // Записывает uin8, а затем младшие 3 байта из второго значения
    void Write32bit(uint8, uint);

    bool IsBusy();

    void WaitRelease();

    uint8 test_value = 0;

    // Произвести запись в пределах одного сектора. Size не может быть больше 256 байт
    static void WriteToSector(uint address, uint8 *buffer, int size);
}


void W25Q80DV::Write1024bytes(const uint8 *buffer, int size)
{
    WaitRelease();

    HAL_SPI::Write(WRITE_ENABLE);             // Write enable

    Write32bit(SECTOR_ERASE, 0x000000);                             // Sector erase

    HAL_SPI::Write(WRITE_DISABLE);            // Write disable

    WaitRelease();

    HAL_SPI::Write(WRITE_ENABLE);             // Write enable

    Buffer<uint8, 1024> data;

    data[0] = PROGRAM_PAGE; //-V525
    data[1] = 0;                // \ 
    data[2] = 0;                // | Адрес
    data[3] = 0;                // /

    for (int i = 0; i < size; i++)
    {
        data[4 + i] = buffer[i];
    }

    //                                                       команда   адрес
    HAL_SPI::Write(data.Data(), size +    1    +   3);     // Page program

    HAL_SPI::Write(WRITE_DISABLE);              // Write disable
}


void W25Q80DV::WriteToSector(uint address, uint8 *buffer, int size)
{
    Buffer<uint8, 256 + 4> data;

    BitSet32 bs(address);

    data[0] = PROGRAM_PAGE;
    data[1] = bs.byte[2];
    data[2] = bs.byte[1];
    data[1] = bs.byte[0];

    for (int i = 0; i < size; i++)
    {
        data[i + 4] = buffer[i];
    }

    WaitRelease();

    HAL_SPI::Write(data.Data(), size + 4);
}


void W25Q80DV::Write(uint address, void *buffer, int size)
{
    WaitRelease();

    HAL_SPI::Write(WRITE_ENABLE);

    uint8 *bufU8 = (uint8 *)buffer;

    while (size > 0)
    {
        Sector sector = Sector::ForAddress(address);

        uint last_address = sector.End();

        int portion = (size <= (int)(last_address - address)) ? size : (int)(last_address - address);

        WriteToSector(address, bufU8, portion);

        bufU8 += portion;
        address += portion;
        size -= portion;
    }

    WaitRelease();

    HAL_SPI::Write(WRITE_DISABLE);
}


void W25Q80DV::Read1024bytes(uint8 *buffer, int size)
{
    WaitRelease();

    Buffer<uint8, 1024> out;

    out[0] = READ_DATA; //-V525
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;

    Buffer<uint8, 1024> in;

    HAL_SPI::WriteRead(out.Data(), in.Data(), size + 1 + 3);

    for (int i = 0; i < size; i++)
    {
        buffer[i] = in[4 + i];
    }
}


void W25Q80DV::Read(uint address, void *buffer, int size)
{
    uint8 *bufU8 = (uint8 *)buffer;

    Buffer<uint8, 1024 + 4> out;
    Buffer<uint8, 1024 + 4> in;

    out[0] = READ_DATA;

    BitSet32 bs(address);

    while (size > 0)
    {
        int portion = (size <= 1024) ? size : 1024;

        out[1] = bs.byte[2];
        out[2] = bs.byte[1];
        out[3] = bs.byte[0];

        WaitRelease();

        HAL_SPI::WriteRead(out.Data(), in.Data(), portion + 4);

        for (int i = 0; i < portion; i++)
        {
            *bufU8++ = in[i + 4];
        }

        size -= portion;
        address += portion;
    }
}


void W25Q80DV::Write32bit(uint8 command, uint bits24)
{
    uint8 data[4];

    data[0] = command;
    data[1] = (uint8)(bits24 >> 16);
    data[2] = (uint8)(bits24 >> 8);
    data[3] = (uint8)(bits24);

    HAL_SPI::Write(data, 4);
}


bool W25Q80DV::IsBusy()
{
    static const uint8 out[2] = { READ_STATUS_1, 0 };
    static uint8 in[2] = { 0, 0 };

    HAL_SPI::WriteRead(out, in, 2);

    return (in[1] & 0x01);
}


void W25Q80DV::WaitRelease()
{
    TimeMeterMS meter;

    while (IsBusy() && (meter.ElapsedTime() < 100))
    {
    }
}


void W25Q80DV::ReadID()
{
    uint8 out[6] = { 0x90, 0, 0, 0, 0, 0 };
    uint8 in[6] = { 0, 0, 0, 0, 0, 0 };

    HAL_SPI::WriteRead(out, in, 6);
}


bool W25Q80DV::Test()
{
    uint8 out[256] = { 0x55 }; //-V1009
    uint8 in[256] = { 0x00 };

    Write1024bytes(out, 1);

    Read1024bytes(in, 1);

    test_value = in[0];

    return (std::memcmp(out, in, 1) == 0); //-V1086
}


uint8 W25Q80DV::TestValue()
{
    return test_value;
}


Sector::Sector(uint _number) : number(_number)
{
    begin = _number * Sector::SIZE;
    end = begin + Sector::SIZE;
}


Sector Sector::ForAddress(uint address)
{
    uint num_sector = address / Sector::SIZE;

    return Sector(num_sector);
}


uint Sector::End() const
{
    return (Number() + 1) * Sector::SIZE;
}
