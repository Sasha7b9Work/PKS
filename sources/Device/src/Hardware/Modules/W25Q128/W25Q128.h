// 2023/04/07 17:06:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "FlashDisk/FlashDisk.h"


// Описывает сектор
struct Sector                   // 4 kB
{
    static const int SIZE = 4 * 1024;

    Sector(uint _number);
    uint Number() const { return number; }
    // Адрес байта, следующего за последним байтом сектора
    uint End() const;
    // Возвращает сектор, содержащий address
    static Sector ForAddress(uint address);
private:
    uint number;    // Номер сектора
    uint begin;     // Начало сектора
    uint end;       // Конец сектора (адрес за последним байтом сектора)
};


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

    void Write(uint address, void *buffer, int size);

    void Read(uint address, void *buffer, int size);
}
