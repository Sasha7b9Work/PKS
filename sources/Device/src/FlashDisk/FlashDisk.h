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
};


namespace FlashDisk
{
    void Init();

    void WriteRecord(const Record &);

    void ReadRecord(Record &);
}
