// 2023/05/21 10:21:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Programmer
{
    // Подгтовка к программированию - стирание памяти
    void Prepare();

    // С этого адреса будет начинаться запись
    void SetStartAddress(uint);

    void WriteBytes(void *data, int size);

    // Столько байт уже записано
    int WrittenBytes();

    uint CalculateCRC(uint address, int size);
}
