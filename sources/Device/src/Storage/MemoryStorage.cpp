// 2023/09/08 22:14:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/MemoryStorage.h"
#include "Utils/Math.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


struct StructData
{
    StructData() : number(0) {}

    uint         number;
    Measurements meas;
    uint         crc;
    BitSet32     control_field;     // ��� ����� ��� �������� ������������ ������

    void Write(const Measurements &);

    // true, ���� ������ �����
    bool IsEmpty() const;

    // true, ���� ���������� ���������� ������
    bool IsValid() const;

    void Erase();

    bool Write(uint address) const;

    bool IsErased();

    bool Read(uint address);

    uint *FirstWord();

    uint CalculateCRC() const;
};


namespace MemoryStorage
{
    static StructData *FindFirstEmpty();

    static void EraseFull();
}


void MemoryStorage::Init()
{
    for (StructData *address = (StructData *)BEGIN; (uint)address < END; address++)
    {
        if (address->IsEmpty())
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
    StructData *address = FindFirstEmpty();

    if (!address)
    {
        EraseFull();

        address = (StructData *)BEGIN;
    }

    address->Write(data);
}


Measurements *MemoryStorage::GetOldest()
{
//    StructData *result = nullptr;
//
//    for (StructData *data = (StructData *)BEGIN; data < (StructData *)END; data++)
//    {
//        if (data && data->IsValid())
//        {
//            if (!result)
//            {
//                result = data;
//            }
//            else
//            {
//                if (data->number < result->number)
//                {
//                    result = data;
//                }
//            }
//        }
//    }
//
//    return result;

    return nullptr;
}


void MemoryStorage::Erase(const Measurements * /*data*/)
{
//    data->Erase();
}


void MemoryStorage::EraseFull()
{
    int start_page = (BEGIN - HAL_ROM::ADDR_BASE) / HAL_ROM::SIZE_PAGE;

    int end_page = (END - HAL_ROM::ADDR_BASE) / HAL_ROM::SIZE_PAGE;

    for (int page = start_page; page < end_page; page++)
    {
        HAL_ROM::ErasePage(page);
    }
}


StructData *MemoryStorage::FindFirstEmpty()
{
    for (StructData *data = (StructData *)BEGIN; data < (StructData *)END; data++)
    {
        if (data && data->IsEmpty())
        {
            return data;
        }
    }

    return nullptr;
}


bool StructData::IsEmpty() const
{
    uint8 *address = (uint8 *)this;

    uint8 *end = address + sizeof(*this);

    while (address < end)
    {
        if (*address != 0xFF)
        {
            return false;
        }

        end++;
    }

    return true;
}


bool StructData::IsValid() const
{
    uint *pointer = (uint *)this;

    if (*pointer == (uint)-1)
    {
        return true;
    }

    return (crc == CalculateCRC()) &&
           (control_field.bytes[0] == 0x00) &&
           (control_field.bytes[1] == BINARY_U8(01111110));
}


void StructData::Erase()
{
    HAL_ROM::WriteUInt((uint)FirstWord(), 0U);
}


bool StructData::IsErased()
{
    return *FirstWord() == 0U;
}


uint *StructData::FirstWord()
{
    return (uint *)this;
}


uint StructData::CalculateCRC() const
{
    return Math::CalculateCRC((uint)this, sizeof(StructData) - sizeof(crc) - sizeof(control_field));
}


bool StructData::Write(uint address) const
{
    HAL_ROM::WriteData(address, this, (int)sizeof(StructData));

    return std::memcmp((void *)this, (void *)address, sizeof(StructData)) == 0;
}


bool StructData::Read(uint address)
{
    std::memcpy((void *)address, this, sizeof(StructData));

    return (CalculateCRC() == crc) && (control_field.word == 0);
}


bool MemoryStorage::IsEmpty()
{
    return GetOldest() == nullptr;
}
