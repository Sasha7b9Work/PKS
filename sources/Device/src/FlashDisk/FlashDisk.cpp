// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Modules/W25Q128/W25Q128.h"


uint Record::num_oldest = (uint)(-1);
uint Record::num_newest = (uint)(-1);


namespace FlashDisk
{
    static const int SIZE = 128 / 8 * 1024 * 1024;
//    static const int NUM_SECTORS = 256 * 16;                // Количество секторов
    static const int NUM_RECORDS = SIZE / sizeof(Record);   // Столько записей помещается в памяти

    struct Memory                   // 8 MB
    {
        static bool IsEmpty();
        static void Write(const Record &);
        static void Write(uint number_record, const Record &);

        Record records[NUM_RECORDS];
//        Sector sectors[NUM_SECTORS];
    };
}


void FlashDisk::Init()
{

}


void FlashDisk::WriteRecord(const Record &record)
{
    Memory::Write(record);
}


bool FlashDisk::Memory::IsEmpty()
{
    return Record::num_newest == (uint)(-1);
}


void FlashDisk::Memory::Write(const Record &record)
{
    if (IsEmpty())
    {
        Write(0, record);
        Record::num_oldest = 0;
        Record::num_newest = 0;
    }
    else
    {
        // Номер записи, куда будем сохранять
        uint num_record = Record::num_newest + 1;

        if (num_record > Record::num_oldest)            // Если сохраняемая запись находится за самой старой - нормальный порядок
        {

        }
        else
        {
            if (num_record == Record::num_oldest)
            {

            }
            else
            {

            }
        }
    }
}


void FlashDisk::Memory::Write(uint number_record, const Record &record)
{
    uint address = sizeof(Record) * number_record;

    W25Q80DV::Write(address, record.GetBuffer(), record.GetSize());
}


void FlashDisk::ReadRecord(Record &)
{
}
