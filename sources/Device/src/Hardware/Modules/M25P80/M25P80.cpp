﻿// 2023/04/07 17:06:22 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <cstring>


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


namespace M25P80
{
    // Записывает uin8, а затем младшие 3 байта из второго значения
    void Write32bit(uint8, uint);

    bool IsBusy();

    void WaitRelease();

    // Произвести запись в пределах одного сектора. Size не может быть больше 256 байт
    static void WriteToSector(uint address, uint8 *buffer, int size);
}


void M25P80::EraseSector(uint num_sector)
{
}


void M25P80::WriteToSector(uint address, uint8 *buffer, int size)
{
    uint8 data[256 + 4];

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

    HAL_SPI::Write(data, size + 4);
}


void M25P80::Write(uint address, void *buffer, int size)
{
}


void M25P80::Read(uint address, void *buffer, int size)
{
}


void M25P80::Write32bit(uint8 command, uint bits24)
{
    uint8 data[4];

    BitSet32 bs(bits24);

    data[0] = command;
    data[1] = bs.byte[2];
    data[2] = bs.byte[1];
    data[3] = bs.byte[0];

    HAL_SPI::Write(data, 4);
}


bool M25P80::IsBusy()
{
    static const uint8 out[2] = { READ_STATUS_1, 0 };
    static uint8 in[2] = { 0, 0 };

    HAL_SPI::WriteRead(out, in, 2);

    return (in[1] & 0x01);
}


void M25P80::WaitRelease()
{
    TimeMeterMS meter;

    while (IsBusy() && (meter.ElapsedTime() < 100))
    {
    }
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


bool Sector::AddressBelong(uint address) const
{
    return (address >= begin) && (address < end);
}


void M25P80::WriteByte(uint8 byte)
{

}


uint8 M25P80::ReadByte()
{
    return 0;
}
