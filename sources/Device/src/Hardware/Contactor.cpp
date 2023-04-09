// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Contactor.h"
#include "Hardware/HAL/HAL.h"


namespace Contactor
{
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
