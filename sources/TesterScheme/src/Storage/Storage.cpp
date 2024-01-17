// 2023/09/14 11:48:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Storage/Storage.h"
#include "Measurer/Measurer.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"


Measurements::Measurements()
{
    for (int i = 0; i < Phase::Count; i++)
    {
        volts[i] = 0;
        currents[i] = 0;
    }
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

    static void GetStateContactors(Measurements &);

    static void GetMeasures(Measurements &);

    // Собрать очередное измерение
    void CollectMeasure(Measurements &);
}


void Storage::Init()
{
}


void Storage::Update()
{
//    static TimeMeterMS meter;
//
//    if (meter.IsFinished())
//    {
//        meter.SetResponseTime(TIME_UPDATE_SENSORS);
//
//        static Measurements measurements;
//
//        CollectMeasure(measurements);
//
//        Sender::SendMeasures(measurements);
//    }
}


void Storage::CollectMeasure(Measurements &measurements)
{
    GetStateContactors(measurements);

    GetMeasures(measurements);

    measurements.counter = counter++;
}


void Storage::GetStateContactors(Measurements &meas)
{
    Contactors::Test::GetCountersBad(meas.bads);
}


void Storage::GetMeasures(Measurements &meas)
{
    meas.SetFullMeasure(Measurer::Measure5Sec());
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


int Measurements::GetBad(Phase::E phase, int num) const
{
    int index = num + phase * 8;

    return bads[index];
}
