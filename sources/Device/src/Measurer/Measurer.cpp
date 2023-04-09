// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Utils/SoftwareGenerator.h"
#include <cmath>


namespace Measurer
{
    static void Calculate();

    static struct FullMeasure measure;

    static bool measure_ready = false;          // true, если измерение готово. Устанавливается в 0 после считывания

    static Sample currentA[NUM_SAMPLES];
    static Sample currentB[NUM_SAMPLES];
    static Sample currentC[NUM_SAMPLES];

    static Sample voltA[NUM_SAMPLES];
    static Sample voltB[NUM_SAMPLES];
    static Sample voltC[NUM_SAMPLES];

    static int16 pos_adc_value = 0;             // Позиция текущих считываемых значений
}


void Measurer::Update()
{
    if (BuffersFull() && !measure_ready)
    {
        TimeMeterMS meterUpdate;
        Calculate();
//        LOG_WRITE("time calculate %d ms", meterUpdate.ElapsedTime());

        measure_ready = true;
    }
}


void Measurer::AppendMeasures(uint16 adc_values[6])
{
    static TimeMeterMS meter;

    if (pos_adc_value == 0)
    {
        meter.Reset();
    }

    if (pos_adc_value < NUM_SAMPLES)
    {
        currentA[pos_adc_value] = adc_values[0];
        currentB[pos_adc_value] = adc_values[1];
        currentC[pos_adc_value] = adc_values[2];

        voltA[pos_adc_value] = adc_values[3];
        voltB[pos_adc_value] = adc_values[4];
        voltC[pos_adc_value] = adc_values[5];

        pos_adc_value++;

        if (pos_adc_value == NUM_SAMPLES)
        {
#ifdef DEV_BOARD
            Generator::GenerateVoltage(voltA);
            Generator::GenerateCurrent(currentA);
#endif

            LOG_WRITE("time read measure %d ms", meter.ElapsedTime());
        }
    }
}


bool Measurer::BuffersFull()
{
    return pos_adc_value >= NUM_SAMPLES;
}


void Measurer::Calculate()
{
    measure.measures[0].Calculate(currentA, voltA);
    measure.measures[1].Calculate(currentB, voltB);
    measure.measures[2].Calculate(currentC, voltC);
}


bool Measurer::MeasureReady()
{
    return measure_ready;
}


FullMeasure Measurer::GetMeasure()
{
    measure_ready = 0;
    pos_adc_value = 0;

    return measure;
}


float Sample::ToVoltage() const
{
    return ((float)rel - (float)ZERO) * VoltsInSample();
}


float Sample::ToCurrent() const
{
    return ((float)rel - (float)ZERO) * AmpersInSample();
}


float Sample::AmplitudeCurrent() const
{
    return 10.0f * 2.0f;
}


float Sample::AmplitudeVoltage() const
{
    return 400.0f * 2.0f;
}


void Sample::FromVoltage(float level)
{
    rel = (uint16)(level / VoltsInSample() + ZERO);
}


void Sample::FromCurrent(float level)
{
    rel = (uint16)(level / AmpersInSample() + ZERO);
}
