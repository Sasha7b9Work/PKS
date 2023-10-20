// 2023/09/08 22:14:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/MemoryStorage.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include <cstring>
#include <cstdlib>


namespace MemoryStorage
{
    static const uint BEGIN = 0x8000000 + 200 * 1024;
    static const uint END = 0x8000000 + 250 * 1024;

    struct Record
    {
        int          number;
        Measurements measurements;
        uint         crc;
        uint         control_field;     // Это нужно для контроля правильности записи

        void *Begin()
        {
            return (void *)this;
        }

        void *End()
        {
            return (void *)(this + 1);
        }

        void Write(int _number, const Measurements &meas)
        {
            Record data;

            data.number = _number;
            std::memcpy(&data.measurements, &meas, sizeof(Measurements));
            data.crc = Math::CalculateCRC(&data, sizeof(number) + sizeof(Measurements));
            data.control_field = 0;

            HAL_ROM::WriteData((uint)Begin(), (const void *)&data, sizeof(data));
        }

        bool IsEmpty()                          // Сюда может быть произведена запись
        {
            for (uint *address = (uint *)this; address < (uint *)End(); address++)
            {
                if (*address != (uint)(-1))
                {
                    return false;
                }
            }

            return true;
        }

        bool IsErased()                         // Запись стёрта
        {
            return number == 0;
        }

        bool IsValid()
        {
            if (number == (uint)-1 || number == 0 || control_field != 0)
            {
                return false;
            }

            return Math::CalculateCRC(&number, sizeof(measurements) + sizeof(number)) == crc;
        }

        void Erase()
        {
            HAL_ROM::WriteUInt((uint)Begin(), 0);
        }

        static Record *Oldest();

        static Record *Newest();
    };

    struct Page
    {
        void Init(int num_page)
        {
            startAddress = BEGIN + HAL_ROM::SIZE_PAGE * num_page;
        }

        Record *FirstRecord()
        {
            return (Record *)startAddress;
        }

        int GetMaxRecordsCount() const
        {
            return HAL_ROM::SIZE_PAGE / sizeof(Record);
        }

        int GetRecordsCount()
        {
            int result = 0;

            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsValid())
                {
                    result++;
                }
            }

            return result;
        }

        Record *LastRecord()
        {
            return FirstRecord() + GetMaxRecordsCount();

        }

        void Prepare()
        {
            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsEmpty() || record->IsErased())
                {
                    continue;
                }

                if (!record->IsValid())
                {
                    LOG_WRITE_TRACE("record %X                          is corrupted", record);
                    record->Erase();
                }
            }
        }

        // Возвращает первую пригодную для записи страницу или nullptr, если все заняты
        static Page *GetFirstForRecord();

        // Возвращает страницу с самой старой записью
        static Page *GetWithOldestRecord();

        Record *GetFirstEmptyRecord()
        {
            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsEmpty())
                {
                    return record;
                }
            }

            return nullptr;
        }

        Record *Append(const Measurements &measurements);

        void Erase()
        {
            int num_page = (int)((startAddress - HAL_ROM::ADDR_BASE) / HAL_ROM::SIZE_PAGE);

            HAL_ROM::ErasePage(num_page);
        }

        void *Begin()
        {
            return (void *)startAddress;
        }

        bool IsEmpty()
        {
            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsEmpty())
                {
                    continue;
                }

                return false;
            }

            return true;
        }

        int GetLastNumber()
        {
            int result = 0;

            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsValid() && record->number > result)
                {
                    result = record->number;
                }
            }

            return result;
        }

        Record *GetOldestRecord()
        {
            Record *result = nullptr;

            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsValid())
                {
                    if (!result || (record->number < result->number))
                    {
                        result = record;
                    }
                }
            }

            return result;
        }

        Record *GetNewestRecord()
        {
            Record *result = nullptr;

            for (Record *record = FirstRecord(); record < LastRecord(); record++)
            {
                if (record->IsValid())
                {
                    if (!result || (record->number > result->number))
                    {
                        result = record;
                    }
                }
            }

            return result;
        }

    private:

        uint startAddress;
    };

    static const int NUM_PAGES = (END - BEGIN) / HAL_ROM::SIZE_PAGE;

    static Page pages[NUM_PAGES];

    static int GetNewNumber()
    {
        int result = 0;

        for (int i = 0; i < NUM_PAGES; i++)
        {
            Page &page = pages[i];

            if (page.IsEmpty())
            {
                continue;
            }

            if (page.GetLastNumber() > result)
            {
                result = page.GetLastNumber();
            }
        }

        return result + 1;
    }

    Page *Page::GetWithOldestRecord()
    {
        Page *result = nullptr;

        for (int i = 0; i < NUM_PAGES; i++)
        {
            Page &page = pages[i];

            if (page.IsEmpty())
            {
                continue;
            }

            int last_number = page.GetLastNumber();

            if (last_number != 0)
            {
                if (!result || (last_number < result->GetLastNumber()))
                {
                    result = &page;
                }
            }
        }

        return result;
    }

    Page *Page::GetFirstForRecord()
    {
        for (int i = 0; i < NUM_PAGES; i++)
        {
            if (pages[i].GetFirstEmptyRecord())
            {
                return &pages[i];
            }
        }

        return nullptr;
    }

    Record *Page::Append(const Measurements &measurements)
    {
        Record *record = GetFirstEmptyRecord();

        if (!record)
        {
            Erase();

            record = (Record *)Begin();
        }

        record->Write(GetNewNumber(), measurements);

        LOG_WRITE_TRACE("record for write %X, number %d", record, record->number);

        return record;
    }

    // Проверить все сектора на предмет повреждённых записей и стереть их
    static void Prepare();

    static int NumberOldestRecord();
    static int NumberNewestRecord();

    static int GetRecordsCount();
}


void MemoryStorage::Init()
{
    LOG_WRITE(" ");
    LOG_WRITE_TRACE("MemoryStorage::Init()");

    for (int i = 0; i < NUM_PAGES; i++)
    {
        pages[i].Init(i);
    }

    Prepare();

    for (int i = 0; i < NUM_PAGES; i++)
    {
        pages[i].Erase();
    }
}


void MemoryStorage::Erase(const Measurements *)
{

}


void *MemoryStorage::Append(const Measurements &meas)
{
    Page *page = Page::GetFirstForRecord();

    if (!page)
    {
        page = Page::GetWithOldestRecord();
    }

    LOG_WRITE("page for write %X", page->Begin());

    return page->Append(meas);
}


Measurements *MemoryStorage::GetOldest()
{
    return nullptr;
}


namespace MemoryStorage
{
    void AppendMeasure()
    {
        Measurements meas;

        meas.SetFullMeasure(Measurer::Measure1Min());

        Append(meas);
    }
}


void MemoryStorage::Prepare()
{
    for (int i = 0; i < NUM_PAGES; i++)
    {
        pages[i].Prepare();
    }
}


MemoryStorage::Record *MemoryStorage::Record::Oldest()
{
    Record *result = nullptr;

    for (int i = 0; i < NUM_PAGES; i++)
    {
        Record *oldest = pages[i].GetOldestRecord();

        if (oldest)
        {
            if (!result || oldest->number < result->number)
            {
                result = oldest;
            }
        }
    }

    return result;
}


MemoryStorage::Record *MemoryStorage::Record::Newest()
{
    Record *result = nullptr;

    for (int i = 0; i < NUM_PAGES; i++)
    {
        Record *newest = pages[i].GetNewestRecord();

        if (newest)
        {
            if (!result || newest->number > result->number)
            {
                result = newest;
            }
        }
    }

    return result;
}


void MemoryStorage::Test()
{
    for (int i = 0; i < 1200; i++)
    {
        AppendMeasure();
    }

//    for (int i = 0; i < 100; i++)
//    {
//        uint value = 0;
//        uint address = 0;
//
//        while (value == 0)
//        {
//            address = BEGIN + std::rand() % (END - BEGIN);
//
//            address &= 0xFFFFFFFFFC;
//
//            value = *((uint *)address);
//        }
//
//        value = 0;
//
//        LOG_WRITE("all records %d", GetRecordsCount());
//
//        LOG_WRITE("write %u to %X", value, address);
//
//        HAL_ROM::WriteUInt(address, value);
//
//        LOG_WRITE("after %d", GetRecordsCount());
//
//        Prepare();
//
//        Timer::DelayMS(100);
//    }
}


int MemoryStorage::NumberOldestRecord()
{
    Record *record = Record::Oldest();

    return record ? record->number : 0;
}


int MemoryStorage::NumberNewestRecord()
{
    Record *record = Record::Newest();

    return record ? record->number : 0;
}


int MemoryStorage::GetRecordsCount()
{
    int result = 0;

    for (int i = 0; i < NUM_PAGES; i++)
    {
        result += pages[i].GetRecordsCount();
    }

    return result;
}
