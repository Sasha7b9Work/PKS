// 2023/04/09 14:54:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Contactor
{
    enum E
    {
        MX0,
        MX1,
        MX2,
        MX3,
        MX4,
        KMA1,
        KMA2,
        KMA3,
        KMA4,
        KMA5,
        KMA6,
        KMA7,
        KMA8,
        KMA9,
        KMB1,
        KMB2,
        KMB3,
        KMB4,
        KMB5,
        KMB6,
        KMB7,
        KMB8,
        KMB9,
        KMC1,
        KMC2,
        KMC3,
        KMC4,
        KMC5,
        KMC6,
        KMC7,
        KMC8,
        KMC9,
        Cout
    };

    void Init();

    void Enable(E);

    void Disable(E);
}
