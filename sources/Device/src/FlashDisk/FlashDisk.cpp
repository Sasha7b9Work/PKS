// 2023/03/30 10:42:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include "Measurer/Measurer.h"
#include "Utils/Math.h"


uint Record::num_oldest = (uint)(-1);
uint Record::num_newest = (uint)(-1);


/*
*   Записи сохраняются в сектора
*/


namespace FlashDisk
{
    static const int SIZE = 1024 * 1024;
    static const int NUM_SECTORS = 16;                      // Количество секторов
//    static const int RECORDS_IN_SECTOR = Sector::SIZE / Record::SIZE;
//    static const int NUM_RECORDS = RECORDS_IN_SECTOR * NUM_SECTORS;

    struct Memory                   // 8 MB
    {
        static bool IsEmpty();
        static void Write(const Record &);
        static void Write(uint number_record, const Record &);

        Sector sectors[NUM_SECTORS];
    };
}


const int Record::MAX_NUM = FlashDisk::SIZE / SIZE;  // Столько записей помещается в памяти


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
        uint num_record = Math::CircularIncrease(Record::num_newest, 0U, (uint)(Record::MAX_NUM - 1));

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
    uint address = Record::SIZE * number_record;

    M25P80::Write(address, record.GetBuffer(), record.GetSize());
}


void FlashDisk::ReadRecord(Record &)
{
}


uint Record::AddressBegin(uint num_record)
{
    return num_record * Record::SIZE;
}


Record::Record(const FullMeasure &measure)
{
    ampl[0] = measure.measures[0].voltage;
    ampl[1] = measure.measures[1].voltage;
    ampl[2] = measure.measures[2].voltage;

    curr[0] = measure.measures[0].current;
    curr[1] = measure.measures[1].current;
    curr[2] = measure.measures[2].current;

    number = FlashDisk::NumberLastRecord() + 1U;

    time = HAL_RTC::CurrentTime();

    info = DataInfo();

    hash = CalculateHash();
}


uint Record::CalculateHash()
{
    uint8 *first_byte = (uint8 *)&number;
    uint8 *last_byte = (uint8 *)&info + sizeof(DataInfo);

    uint result = 0;

    for (uint8 *pointer = first_byte; pointer < last_byte; pointer++)
    {
        result = *pointer++ + (result << 6) + (result << 16) - result;
    }

    return result;
}


int FlashDisk::NumberLastRecord()
{
    return -1;
}
