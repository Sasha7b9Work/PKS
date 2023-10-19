// 2023/09/08 22:14:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/MemoryStorage.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include <cstring>


namespace MemoryStorage
{
    static const uint BEGIN = 0x8000000 + 200 * 1024;
    static const uint END = 0x8000000 + 210 * 1024;

    struct RecordData
    {
        int          number;
        Measurements measurements;
        uint         crc;
        uint         control_field;     // Ёто нужно дл€ контрол€ правильности записи

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
            RecordData data;

            data.number = _number;
            std::memcpy(&data.measurements, &meas, sizeof(Measurements));
            data.crc = Math::CalculateCRC(&data, sizeof(uint) + sizeof(Measurements));
            data.control_field = 0;

            HAL_ROM::WriteData((uint)Begin(), (const void *)&data, sizeof(data));
        }

        bool IsEmpty()                          // —юда может быть произведена запись
        {
            uint *address = (uint *)this;

            while (address < (uint *)End())
            {
                if (*address != (uint)(-1))
                {
                    return false;
                }
            }

            return true;
        }

        bool IsErased()                         // «апись стЄрта
        {
            return number == 0;
        }

        bool IsValid()
        {
            if (number == (uint)-1 || number == 0 || control_field != 0)
            {
                return false;
            }

            return Math::CalculateCRC(&measurements, sizeof(measurements)) == crc;
        }

        void Erase()
        {
            HAL_ROM::WriteUInt((uint)Begin(), 0);
        }
    };

    struct Page
    {
        void Init(int num_page)
        {
            startAddress = BEGIN + HAL_ROM::SIZE_PAGE * num_page;
            LOG_WRITE_TRACE("Init start %X, begin %X", startAddress, BEGIN);
        }

        RecordData *FirstRecord()
        {
            return (RecordData *)startAddress;
        }

        int GetRecordsCount() const
        {
            return HAL_ROM::SIZE_PAGE / sizeof(RecordData);
        }

        RecordData *LastRecord()
        {
            return FirstRecord() + GetRecordsCount();

        }

        void Prepare()
        {
            RecordData *record = FirstRecord();

            while (record < LastRecord())
            {
                if (record->IsEmpty() || record->IsErased())
                {
                    continue;
                }

                if (!record->IsValid())
                {
                    record->Erase();
                }
            }
        }

        // ¬озвращает первую пригодную дл€ записи страницу или nullptr, если все зан€ты
        static Page *GetFirstForRecord();

        // ¬озвращает страницу с самой старой записью
        static Page *GetWithOldestRecord();

        RecordData *GetFirstEmptyRecord()
        {
            RecordData *record = FirstRecord();

            RecordData *last = LastRecord();

            while (record < last)
            {
                if (record->IsEmpty())
                {
                    return record;
                }
            }

            return nullptr;
        }

        RecordData *Append(const Measurements &measurements, char *file, int line);

        void Erase(char *file, int line)
        {
            LOG_WRITE_TRACE("Erase() from %s:%d", file, line);

            int num_page = (int)((startAddress - HAL_ROM::ADDR_BASE) / HAL_ROM::SIZE_PAGE);

            LOG_WRITE_TRACE("start %X, base %X", startAddress, HAL_ROM::ADDR_BASE);

            LOG_WRITE_TRACE("Erase %d page", num_page);

            HAL_ROM::ErasePage(num_page, __FILE__, __LINE__);
        }

        void *Begin()
        {
            return (void *)startAddress;
        }

        bool IsEmpty()
        {
            RecordData *record = FirstRecord();

            while (record < LastRecord())
            {
                if (record->IsEmpty())
                {
                    continue;;
                }

                return false;
            }

            return true;
        }

        int GetLastNumber()
        {
            RecordData *record = FirstRecord();

            int result = 0;

            while (record < LastRecord())
            {
                if (record->IsValid())
                {
                    result = record->number;
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
        int number = 0;

        for (int i = 0; i < NUM_PAGES; i++)
        {
            Page &page = pages[i];

            if (page.IsEmpty())
            {
                continue;
            }

            if (page.GetLastNumber() > number)
            {
                number = page.GetLastNumber();
                result = &page;
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

    RecordData *Page::Append(const Measurements &measurements, char *file, int line)
    {
        LOG_WRITE_TRACE("Page::Append() from %s:%d", file, line);

        LOG_WRITE_TRACE("startAddress = %X", startAddress);

        RecordData *record = GetFirstEmptyRecord();

        if (!record)
        {
            Erase(__FILE__, __LINE__);

            record = (RecordData *)Begin();
        }

        record->Write(GetNewNumber(), measurements);

        return record;
    }
}


void MemoryStorage::Init()
{
    LOG_WRITE(" ");
    LOG_WRITE_TRACE("MemoryStorage::Init()");

    for (int i = 0; i < NUM_PAGES; i++)
    {
        pages[i].Init(i);
    }

//    for (int i = 0; i < NUM_PAGES; i++)
//    {
//        pages[i].Prepare();
//    }

    for (int i = 0; i < NUM_PAGES; i++)
    {
        pages[i].Erase(__FILE__, __LINE__);
    }
}


void MemoryStorage::Erase(const Measurements *)
{

}


void *MemoryStorage::Append(const Measurements &meas)
{
    Page *page = Page::GetFirstForRecord();

    LOG_WRITE_TRACE("GetFirstForRecord() = %X", page);

    if (page)
    {
        page->Append(meas, __FILE__, __LINE__);
    }

    page = Page::GetWithOldestRecord();

    LOG_WRITE_TRACE("page %X", page);

    return page->Append(meas, __FILE__, __LINE__);
}


Measurements *MemoryStorage::GetOldest()
{
    return nullptr;
}


void MemoryStorage::Test()
{
    while (true)
    {
        Measurements meas;

        meas.SetFullMeasure(Measurer::Measure1Min());

        void *address = Append(meas);

        LOG_WRITE("New measure append to address %X", address);

        Timer::DelayMS(1000);
    }
}
