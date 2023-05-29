// 2023/05/21 11:08:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace SIM800
{
    void Update(pchar);

    bool IsRegistered();

    pchar LevelSignal();

    void Transmit(pchar);

    void Transmit(pchar format, pchar);

    void Transmit(pchar format, int);
}
