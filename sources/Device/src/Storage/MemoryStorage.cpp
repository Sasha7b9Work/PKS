// 2023/09/08 22:14:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/MemoryStorage.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace MemoryStorage
{
    struct RecordData
    {
        RecordData() : number(0) {}

        uint         number;
        Measurements measurements;
        uint         crc;
        BitSet32     control_field;     // Ёто нужно дл€ контрол€ правильности записи

        void Write(const Measurements &, const RecordData *oldest);

        // true, если запись пуста
        bool IsEmpty() const;

        // true, если содержатс€ корректные данные
        bool ContainData() const;

        bool IsErased();

        void Erase();

        bool Read(uint address);

        uint *FirstWord();

        uint CalculateCRC() const;
    };

    struct Page
    {
        Page(uint _startAddress) : startAddress(_startAddress) { }
        bool ExistEmptyRecords() const;
        // —одержит записи с данными
        bool ExistDataRecords() const;
        void Erase() const;
        int Number() const;
        uint Address() const { return startAddress; }
    private:
        uint startAddress;
        RecordData *GetFirstRecord() const;
    };

    static RecordData *PrepreEmptyPlaceForRecord();

    // ¬озвращает указатель на самую старую запись
    static RecordData *GetOldestRec();

    static RecordData *Begin()
    {
        return (RecordData *)BEGIN;
    }

    static RecordData *End()
    {
        return (RecordData *)END;
    }

    // —тереть сектор, которому принадлежит адрес address
    static void ErasePageForAddress(uint address);

    static int NumPageForAddress(uint address);
}


void MemoryStorage::Init()
{
    for (RecordData *address = Begin(); address < End(); address++)
    {
        if (address->IsEmpty() || address->IsErased())
        {
            continue;
        }

        if (!address->ContainData())
        {
            address->Erase();
        }
    }

    for (uint address = BEGIN; address < END; address += HAL_ROM::SIZE_PAGE)
    {
        Page page(address);

        bool exist_empty = page.ExistEmptyRecords();

        bool exist_data = page.ExistDataRecords();

        LOG_WRITE_TRACE("Page %d:%X exist_empty:%d exist_data:%d", page.Number(), page.Address(), exist_empty, exist_data);

        if (!exist_empty && !exist_data)
        {
            page.Erase();
        }
        else
        {
            LOG_WRITE_TRACE("Page %d:%X not erased", page.Number(), page.Address());
        }
    }
}


void MemoryStorage::Append(const Measurements &data)
{
    RecordData *rec = PrepreEmptyPlaceForRecord();

    rec->Write(data, GetOldestRec());
}


void MemoryStorage::RecordData::Write(const Measurements &meas, const RecordData *oldest)
{
    uint address = (uint)this;

    HAL_ROM::WriteUInt(address, (uint)((oldest == nullptr) ? 1 : oldest->number + 1));  // number

    address += sizeof(uint);

    HAL_ROM::WriteData(address, &meas, sizeof(Measurements));                   // Measurements

    address += sizeof(Measurements);

    HAL_ROM::WriteUInt(address, CalculateCRC());                                // crc

    address += sizeof(uint);

    HAL_ROM::WriteUInt(address, 0x00000000);                                    // control_fields
}


MemoryStorage::RecordData *MemoryStorage::PrepreEmptyPlaceForRecord()
{
    for (RecordData *rec = Begin(); rec < End(); rec++)
    {
        if (rec->IsEmpty())
        {
            if (rec + 1 > End())
            {
                ErasePageForAddress((uint)Begin());

                return Begin();
            }

            return rec;
        }
    }

    RecordData *oldest = GetOldestRec();

    if (oldest == nullptr)
    {
        ErasePageForAddress((uint)Begin());

        return Begin();
    }

    ErasePageForAddress((uint)oldest);

    return PrepreEmptyPlaceForRecord();
}


Measurements *MemoryStorage::GetOldest()
{
    RecordData *result = nullptr;

    for (RecordData *data = (RecordData *)Begin(); data < (RecordData *)End(); data++)
    {
        if (data->ContainData())
        {
            if (!result)
            {
                result = data;
            }
            else
            {
                if (data->number < result->number)
                {
                    result = data;
                }
            }
        }
    }

    return result ? &result->measurements : nullptr;
}


void MemoryStorage::Erase(const Measurements *meas)
{
    for (RecordData *data = (RecordData *)Begin(); data < (RecordData *)End(); data++)
    {
        if (&data->measurements == meas)
        {
            data->Erase();

            return;
        }
    }
}


void MemoryStorage::ErasePageForAddress(uint address)
{
    HAL_ROM::ErasePage(NumPageForAddress(address));
}


int MemoryStorage::NumPageForAddress(uint address)
{
    return (int)((address - HAL_ROM::ADDR_BASE) / HAL_ROM::SIZE_PAGE);
}


bool MemoryStorage::RecordData::IsEmpty() const
{
    uint8 *address = (uint8 *)this;

    uint8 *end = address + sizeof(*this);

    while (address < end)
    {
        if (*address != 0xFF)
        {
            return false;
        }

        address++;
    }

    return true;
}


bool MemoryStorage::RecordData::ContainData() const
{
    uint *pointer = (uint *)this;

    if (*pointer == (uint)-1)
    {
        return false;                   // «десь ничего не записано
    }

    return (crc == CalculateCRC()) &&
           (control_field.bytes[0] == 0x00) &&
           (control_field.bytes[1] == BINARY_U8(01111110));
}


void MemoryStorage::RecordData::Erase()
{
    HAL_ROM::WriteUInt((uint)FirstWord(), 0U);
}


bool MemoryStorage::RecordData::IsErased()
{
    return *FirstWord() == 0U;
}


uint *MemoryStorage::RecordData::FirstWord()
{
    return (uint *)this;
}


uint MemoryStorage::RecordData::CalculateCRC() const
{
    return Math::CalculateCRC((uint)this, sizeof(RecordData) - sizeof(crc) - sizeof(control_field));
}


bool MemoryStorage::RecordData::Read(uint address)
{
    std::memcpy((void *)address, this, sizeof(RecordData));

    return (CalculateCRC() == crc) && (control_field.word == 0);
}


bool MemoryStorage::IsEmpty()
{
    return GetOldest() == nullptr;
}


MemoryStorage::RecordData *MemoryStorage::GetOldestRec()
{
    RecordData *result = nullptr;

    for (RecordData *rec = Begin(); rec < End(); rec++)
    {
        if (rec + 1 <= End())
        {
            if (rec->ContainData())
            {
                if (result == nullptr)
                {
                    result = rec;
                }
                else
                {
                    if (rec->number > result->number)
                    {
                        result = rec;
                    }
                }
            }
        }
    }

    return result;
}


bool MemoryStorage::Page::ExistEmptyRecords() const
{
    uint end = startAddress + HAL_ROM::SIZE_PAGE;

    RecordData *record = GetFirstRecord();

    while ((uint)record < end)
    {
        if (record->IsEmpty())
        {
            return true;
        }

        record++;
    }

    return false;
}


bool MemoryStorage::Page::ExistDataRecords() const
{
    uint end = startAddress + HAL_ROM::SIZE_PAGE;

    RecordData *record = GetFirstRecord();

    while ((uint)record < end)
    {
        if (record->ContainData())
        {
            return true;
        }

        record++;
    }

    return false;
}


MemoryStorage::RecordData *MemoryStorage::Page::GetFirstRecord() const
{
    RecordData *record = Begin();

    while ((uint)record < startAddress)
    {
        record++;
    }

    return record;
}


void MemoryStorage::Page::Erase() const
{
    int num_page = Number();

    LOG_WRITE_TRACE("Erase page %d address %X", num_page, Address());

    HAL_ROM::ErasePage(num_page);
}


int MemoryStorage::Page::Number() const
{
    return (int)((startAddress - HAL_ROM::ADDR_STORAGE) / HAL_ROM::SIZE_PAGE);
}
