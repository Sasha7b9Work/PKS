// 2023/09/14 11:48:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Measurer/Measurer.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/MQTT/Sender/Sender.h"
#include "Hardware/Timer.h"


namespace Storage
{

}


void Storage::Init()
{

}


void Storage::Update()
{
    static TimeMeterMS meter;

    if (meter.IsFinished())
    {
        meter.SetResponseTime(1000);

        Sender::Measure::Send(Measurer::Measure5Sec());
    }
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


void Measurements::SetFullMeasure(FullMeasure &meas)
{
    for (int i = 0; i < Phase::Count; i++)
    {
        volts[i] = (uint16)(meas.measures[i].voltage * 10.0f + 0.5f);
        currents[i] = (uint16)(meas.measures[i].current * 10.0f + 0.5f);
    }
}


FullMeasure Measurements::GetFullMeasure() const
{
    FullMeasure meas;

    for (int i = 0; i < Phase::Count; i++)
    {
        meas.measures[i].voltage = volts[i] / 10.0f;
        meas.measures[i].current = currents[i] / 10.0f;
    }

    return meas;
}
