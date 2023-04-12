// 2023/03/30 10:41:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


union DataInfo
{
    uint empty;
    struct
    {
        uint transmited : 1;
    };
};


struct Record
{
    uint       crc;
    PackedTime time;
    float      ampl[3];
    float      curr[3];
    DataInfo   info;

    // В эту запись ничего не записывали
    bool IsEmpty() const { return crc == (uint)-1; }

    // Запись стёрта
    bool IsErased() const { return crc == 0; }

    // Запись хранит значения
    bool IsFill() const { return !IsEmpty() && !IsErased(); }
};


namespace FlashDisk
{
    void Init();

    void WriteRecord(const Record &);

    void ReadRecord(Record &);
}
