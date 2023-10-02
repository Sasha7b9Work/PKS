// 2023/09/14 11:48:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Measurer/Measurer.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/MQTT/Sender.h"
#include "Hardware/Timer.h"
#include "Storage/MemoryStorage.h"


Measurements::Measurements()
{
    for (int i = 0; i < Phase::Count; i++)
    {
        volts[i] = 0;
        currents[i] = 0;
    }

    flags.bits = 0;
    flags.levelA = 0;
    flags.levelB = 0;
    flags.levelC = 0;
}


MeasurementsReady::MeasurementsReady()
{
    for (int i = 0; i < Phase::Count; i++)
    {
        volts[i] = true;
        currents[i] = true;
        gp[i] = true;
        stage[i] = true;

        for (int j = 0; j < 9; j++)
        {
            km[i][j] = true;
        }
    }

    _100V = true;
}


namespace Storage
{
    static uint counter = 0;

    void Reset()
    {
        counter = 0;
    }

    static void GetPinsGP(Measurements &);

    static void GetStateContactors(Measurements &);

    static void GetLevelsContactors(Measurements &);

    static void GetMeasures(Measurements &);

    // Собрать очередное измерение
    static bool CollectMeasure(Measurements &);

    // Послать очередное измерение
    static void SendMeasure();
}


void Storage::Init()
{
//    MemoryStorage::Init();
}


void Storage::Update()
{
    static Measurements measurements;

    if (CollectMeasure(measurements))
    {
        Sender::SendMeasures(measurements);

//        MemoryStorage::Append(measurements);
    }

//    SendMeasure();
}


bool Storage::CollectMeasure(Measurements &measurements)
{
    static TimeMeterMS meter;

    if (!meter.IsFinished())
    {
        return false;
    }

    GetPinsGP(measurements);

    GetStateContactors(measurements);

    GetLevelsContactors(measurements);

    GetMeasures(measurements);

    measurements.counter = counter++;

    meter.SetResponseTime(TIME_UPDATE_MS);

    return true;
}


void Storage::SendMeasure()
{
    static TimeMeterMS meter;

    if (meter.IsFinished())
    {
        const Measurements *meas = MemoryStorage::GetOldest();

        LOG_WRITE_TRACE("address meas = %X", meas);

        if (Sender::SendMeasures(*meas))
        {
            MemoryStorage::Erase(meas);

            meter.SetResponseTime(100);
        }
    }
}


void Storage::GetPinsGP(Measurements &meas)
{
    bool is_hi[Phase::Count];

    HAL_PINS_GP::Update(is_hi);

    for (int i = 0; i < Phase::Count; i++)
    {
        meas.flags.SetGP((Phase::E)i, is_hi[i]);
    }
}


void Storage::GetStateContactors(Measurements &meas)
{
    int states[NUM_PINS_MX];

    Contactors::Serviceability::GetStates(states);

    for (int phase = 0; phase < Phase::Count; phase++)
    {
        for (int i = 0; i < 9; i++)
        {
            meas.flags.SetKM((Phase::E)phase, i, states[phase * 9 + i]);
        }
    }

    meas.flags.Set100V(states[27] != 0);
}


void Storage::GetLevelsContactors(Measurements &meas)
{
    int levels[Phase::Count];

    Contactors::GetLevels(levels);

    for (int i = 0; i < Phase::Count; i++)
    {
        meas.flags.SetLevelRele((Phase::E)i, levels[i]);
    }
}


void Storage::GetMeasures(Measurements &meas)
{
    meas.SetFullMeasure(Measurer::Measure5Sec());
}


int Measurements::Flags::ShiftBitsStateKM(Phase::E phase, int num) const
{
    return num * 2 + (int)phase * 9 * 2;
}


int Measurements::Flags::NumBitGP(Phase::E phase) const
{
    return 55 + (int)phase;
}


void Measurements::Flags::SetKM(Phase::E phase, int num, int state)
{
    int shift = ShiftBitsStateKM(phase, num);

    if (state < 0)
    {
        bits |= (uint64)0x3 << shift;
    }
    else
    {
        bits &= ~((uint64)0x3 << shift);

        if (state > 0)
        {
            bits |= ((uint64)0x1 << shift);
        }
    }
}


int Measurements::Flags::GetKM(Phase::E phase, int num) const
{
    int shift = ShiftBitsStateKM(phase, num);

    int value = (int)(0x3 & (bits >> shift));

    static const int states[4] = { 0, 1, 0, -1 };

    return states[value];
}


void Measurements::Flags::SetGP(Phase::E phase, bool state)
{
    if (state)
    {
        _SET_BIT_U64(bits, NumBitGP(phase));
    }
    else
    {
        _CLEAR_BIT_U64(bits, NumBitGP(phase));
    }
}


bool Measurements::Flags::GetGP(Phase::E phase) const
{
    return (_GET_BIT_U64(bits, NumBitGP(phase)) != 0);
}


void Measurements::Flags::Set100V(bool state)
{
    if (state)
    {
        _SET_BIT_U64(bits, 54);
    }
    else
    {
        _CLEAR_BIT_U64(bits, 54);
    }
}


bool Measurements::Flags::Get100V() const
{
    return _GET_BIT(bits, 54) != 0;
}


void Measurements::Flags::SetLevelRele(Phase::E phase, int state)
{
    if (phase == Phase::A)
    {
        levelA = state;
    }
    else if (phase == Phase::B)
    {
        levelB = state;
    }
    else
    {
        levelC = state;
    }
}


int Measurements::Flags::GetLevelRele(Phase::E phase) const
{
    if (phase == Phase::A)
    {
        return levelA;
    }
    else if (phase == Phase::B)
    {
        return levelB;
    }

    return levelC;
}


void Measurements::SetFullMeasure(const FullMeasure &meas)
{
    for (int i = 0; i < Phase::Count; i++)
    {
        if (meas.is_good[i])
        {
            volts[i] = (uint16)(meas.measures[i].voltage * 10.0f + 0.5f);
            currents[i] = (uint16)(meas.measures[i].current * 10.0f + 0.5f);
        }
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
