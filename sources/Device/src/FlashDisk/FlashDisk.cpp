// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Modules/W25Q128/W25Q128.h"


namespace FlashDisk
{
    static const int SIZE = 128 / 8 * 1024 * 1024;
    static const int SIZE_SECTOR = 4 * 1024;
    static const int RECORDS_IN_SECTOR = SIZE_SECTOR / sizeof(Record);
    static const int NUM_SECTORS = 256 * 16;

    struct Sector                   // 4 kB
    {
        // Стёртый сектор хранит 
        uint info[7];

        Record records[RECORDS_IN_SECTOR];
    };

    struct Memory                   // 8 MB
    {
        Sector sectors[NUM_SECTORS];
    };


    // Если 0, то места для запси нет
    static uint FindEmptyRecord();

    // Стереть самую старую запись
    static void EraseOldRecord();
}


void FlashDisk::Init()
{

}


void FlashDisk::WriteRecord(const Record &record)
{
    uint address = FindEmptyRecord();

    if (address == 0)
    {
        EraseOldRecord();

        address = FindEmptyRecord();
    }

    W25Q80DV::WriteRecord(address, record);
}


void FlashDisk::ReadRecord(Record &)
{
}


uint FlashDisk::FindEmptyRecord()
{
    return 0;
}
