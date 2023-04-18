// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Utils/SoftwareGenerator.h"
#include <cmath>


namespace Measurer
{
    static FullMeasure Calculate();

    static struct FullMeasure measure;

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
    if (BuffersFull())
    {
        measure = Calculate();

        LOG_WRITE("%f V", measure.measures[0].voltage);

        pos_adc_value = 0;
    }
}


FullMeasure Measurer::LastMeasure()
{
    return measure;
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
        voltA [pos_adc_value] = adc_values[0];
        voltB [pos_adc_value] = adc_values[1];
        voltC [pos_adc_value] = adc_values[2];

        currentA[pos_adc_value] = adc_values[3];
        currentB[pos_adc_value] = adc_values[4];
        currentC[pos_adc_value] = adc_values[5];

        pos_adc_value++;

        if (pos_adc_value == NUM_SAMPLES)
        {
#ifdef EMULATE
            Generator::GenerateVoltage(voltA);
            Generator::GenerateCurrent(currentA);

            Generator::GenerateVoltage(voltB);
            Generator::GenerateCurrent(currentB);

            Generator::GenerateVoltage(voltC);
            Generator::GenerateCurrent(currentC);
#endif

//            LOG_WRITE("time read measure %d ms", meter.ElapsedTime());
        }
    }
}


bool Measurer::BuffersFull()
{
    return pos_adc_value >= NUM_SAMPLES;
}


FullMeasure Measurer::Calculate()
{
    FullMeasure result;

    result.measures[0].Calculate(1, currentA, voltA);
    result.measures[1].Calculate(2, currentB, voltB);
    result.measures[2].Calculate(3, currentC, voltC);

    return result;
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
    return 830.0f * 2.0f;
}


void Sample::FromVoltage(float level)
{
    rel = (uint16)(level / VoltsInSample() + ZERO);
}


void Sample::FromCurrent(float level)
{
    rel = (uint16)(level / AmpersInSample() + ZERO);
}
