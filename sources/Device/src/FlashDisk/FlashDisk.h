// 2023/03/30 10:41:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL.h"


struct FullMeasure;


namespace FlashDisk
{
    struct Memory;
}


union DataInfo
{
    DataInfo() : transmited(0) { }
    uint empty;
    struct
    {
        uint transmited : 1;
    };
};


struct Record
{
    static const int SIZE = 40;
    static const int MAX_NUM;

    uint       hash;
    uint       number;
    PackedTime time;
    float      ampl[3];
    float      curr[3];
    DataInfo   info;

    Record(const FullMeasure &);

    // В эту запись ничего не записывали
    bool IsEmpty() const { return hash == (uint)-1; }

    // Запись стёрта
    bool IsErased() const { return hash == 0; }

    // Запись хранит значения
    bool IsFill() const { return !IsEmpty() && !IsErased(); }

    void *GetBuffer() const { return (void *)this; }

    int GetSize() const { return sizeof(*this); }

    uint CalculateHash();

private:

    friend struct FlashDisk::Memory;

    static uint num_oldest;      // Номер самой старой записи
    static uint num_newest;      // Номер последней записи

    // Адрес начала записи
    static uint AddressBegin(uint num_record);
};


namespace FlashDisk
{
    void Init();

    void WriteRecord(const Record &);

    void ReadRecord(Record &);

    // Возвращает номер последней записи
    int NumberLastRecord();
}
