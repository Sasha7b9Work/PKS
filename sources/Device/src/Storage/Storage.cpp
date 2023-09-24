// 2023/09/14 11:48:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Measurer/Measurer.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"


void Storage::Init()
{
    Contactors::Init();
}


void Storage::Update()
{
    Measurer::Update();

    Contactors::Serviceability::Verify();

    Contactors::Update(Measurer::Measure5Sec());

    HAL_PINS::Update();
}


int Measurements::Flags::NumBitKM(Phase::E phase, int num) const
{
    return num + (int)phase * 9;
}


int Measurements::Flags::NumBItGP(Phase::E phase) const
{
    return 28 + (int)phase;
}


void Measurements::Flags::SetKM(Phase::E phase, int num, bool state)
{
    if (state)
    {
        _SET_BIT(bits, NumBitKM(phase, num));
    }
    else
    {
        _CLEAR_BIT(bits, NumBitKM(phase, num));
    }
}


bool Measurements::Flags::GetKM(Phase::E phase, int num) const
{
    return (_GET_BIT(bits, NumBitKM(phase, num)) != 0);
}


void Measurements::Flags::SetGP(Phase::E phase, bool state)
{
    if (state)
    {
        _SET_BIT(bits, NumBItGP(phase));
    }
    else
    {
        _CLEAR_BIT(bits, NumBItGP(phase));
    }
}


bool Measurements::Flags::GetGP(Phase::E phase) const
{
    return (_GET_BIT(bits, NumBItGP(phase)) != 0);
}


void Measurements::Flags::Set100V(bool state)
{
    if (state)
    {
        _SET_BIT(bits, 27);
    }
    else
    {
        _CLEAR_BIT(bits, 27);
    }
}


bool Measurements::Flags::Get100V() const
{
    return _GET_BIT(bits, 27) != 0;
}


void Measurements::Flags::SetStateRele(Phase::E phase, int state)
{
    if (phase == Phase::A)
    {
        stateA = state;
    }
    else if (phase == Phase::B)
    {
        stateB = state;
    }
    else
    {
        stateC = state;
    }
}


int Measurements::Flags::GetStateRele(Phase::E phase)
{
    if (phase == Phase::A)
    {
        return stateA;
    }
    else if (phase == Phase::B)
    {
        return stateB;
    }

    return stateC;
}
