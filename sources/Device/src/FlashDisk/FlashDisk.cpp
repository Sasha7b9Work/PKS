// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Modules/W25Q128/W25Q128.h"


int Record::num_oldest = (uint)(-1);
int Record::num_newest = (uint)(-1);


namespace FlashDisk
{
    static const int SIZE = 128 / 8 * 1024 * 1024;
    static const int SIZE_SECTOR = 4 * 1024;
    static const int NUM_SECTORS = 256 * 16;                // Количество секторов
    static const int NUM_RECORDS = SIZE / sizeof(Record);   // Столько записей помещается в памяти

    struct Sector                   // 4 kB
    {
        Sector(uint _number);
        uint Number() const { return number; }
    private:
        uint number;    // Номер сектора
        uint begin;     // Начало сектора
        uint end;       // Конец сектора (адрес за последним байтом сектора)
    };

    struct Memory                   // 8 MB
    {
        static void Write(const Record &);

        Sector sectors[NUM_SECTORS];
    };
}


FlashDisk::Sector::Sector(uint _number) : number(_number)
{
    begin = _number * SIZE_SECTOR;
    end = begin + SIZE_SECTOR;
}


void FlashDisk::Init()
{

}


void FlashDisk::WriteRecord(const Record &record)
{
    Memory::Write(record);
}


void FlashDisk::Memory::Write(const Record &record)
{
    if (Record::num_oldest == (uint)(-1))
    {

    }
}


void FlashDisk::ReadRecord(Record &)
{
}
