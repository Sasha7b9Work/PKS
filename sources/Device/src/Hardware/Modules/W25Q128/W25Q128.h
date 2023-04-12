// 2023/04/07 17:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FlashDisk/FlashDisk.h"


namespace W25Q80DV
{
    // Размер буфера ограничен 1024 байтами
    void Write1024bytes(const uint8 *buffer, int size);

    // Размер буфера ограничен 1024 байтами
    void Read1024bytes(uint8 *buffer, int size);

    void ReadID();

    bool Test();

    // Возвращает значение, считанное во время теста
    uint8 TestValue();

    void WriteRecord(uint address, const Record &);

    void ReadRecord(uint address, Record &);
}
