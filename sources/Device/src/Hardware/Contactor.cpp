// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Contactor.h"
#include "Hardware/HAL/HAL.h"


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

    static PinOUT *pins[Cout] =
    {
        &pinOutMX0,
        &pinOutMX1,
        &pinOutMX2,
        &pinOutMX3,
        &pinOutMX4,
        &pinOutKMA1,
        &pinOutKMA2,
        &pinOutKMA3,
        &pinOutKMA4,
        &pinOutKMA5,
        &pinOutKMA6,
        &pinOutKMA7,
        &pinOutKMA8,
        &pinOutKMA9,
        &pinOutKMB1,
        &pinOutKMB2,
        &pinOutKMB3,
        &pinOutKMB4,
        &pinOutKMB5,
        &pinOutKMB6,
        &pinOutKMB7,
        &pinOutKMB8,
        &pinOutKMB9,
        &pinOutKMC1,
        &pinOutKMC2,
        &pinOutKMC3,
        &pinOutKMC4,
        &pinOutKMC5,
        &pinOutKMC6,
        &pinOutKMC7,
        &pinOutKMC8,
        &pinOutKMC9
    };

    void Enable(E);

    void Disable(E);
}


void Contactor::Init()
{
    for (int i = 0; i < Cout; i++)
    {
        pins[i]->Init();
    }
}


void Contactor::Enable(E pin)
{
    pins[pin]->Set();
}


void Contactor::Disable(E pin)
{
    pins[pin]->Reset();
}


void Contactor::Update(const FullMeasure &measure)
{

}
