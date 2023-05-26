﻿// 2023/05/26 09:27:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SIM800
{
    void Update(pchar);

    bool IsRegistered();

    pchar LevelSignal();

    // Передать с завершающим 0x0d
    void Transmit(pchar);

    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);

    void TransmitUINT8(uint8);

    void Reset();
}
