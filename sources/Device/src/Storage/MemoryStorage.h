// 2023/09/08 22:13:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Storage/Storage.h"


struct StructData
{
    friend struct MemoryStorage;

    StructData() {}

    Measurements meas;
    uint         crc;
    BitSet32     control_field;     // Это нужно для контроля правильности записи

private:

    // true, если запись пуста
    bool IsEmpty() const;

    // true, если содержатся корректные данные
    bool IsValid() const;

    void Erase();

    bool Write(uint address) const;

    bool IsErased();

    bool Read(uint address);

    uint *FirstWord();

    uint CalculateCRC() const;
};


struct MemoryStorage
{
    static const uint BEGIN = 0x8000000 + 200 * 1024;
    static const uint END = 0x8000000 + 250 * 1024;

    static void Init();

    static void Append(const StructData &);

    // Возвращает указатель на самую старую структуру данных (которая считана раньше всех). После использования нужно вызвать Erase()
    // с этим указателем, чтобы стереть структуру из хранилища
    static StructData *GetOldest();

    static void Erase(StructData *);

private:

    static void Write(uint address, uint value);

    static StructData *FindFirstEmpty();

    static void EraseFull();
};
