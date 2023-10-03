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
        BitSet32     control_field;     // Это нужно для контроля правильности записи

        void Write(const Measurements &, const RecordData *oldest);

        // true, если запись пуста
        bool IsEmpty() const;

        // true, если содержатся корректные данные
        bool IsValid() const;

        bool IsErased();

        void Erase();

        bool Read(uint address);

        uint *FirstWord();

        uint CalculateCRC() const;
    };

    static RecordData *PrepreEmptyPlaceForRecord();

    // Возвращает указатель на самую старую запись
    static RecordData *GetOldestRec();

    static RecordData *Begin()
    {
        return (RecordData *)BEGIN;
    }

    static RecordData *End()
    {
        return (RecordData *)END;
    }

    // Стереть сектор, которому принадлежит адрес address
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

        if (!address->IsValid())
        {
            address->Erase();
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
        if (data->IsValid())
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


bool MemoryStorage::RecordData::IsValid() const
{
    uint *pointer = (uint *)this;

    if (*pointer == (uint)-1)
    {
        return false;                   // Здесь ничего не записано
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
            if (rec->IsValid())
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
