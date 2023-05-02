// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Utils/SoftwareGenerator.h"
#include <cmath>


namespace Measurer
{
    static struct FullMeasure measure;
    static struct FullMeasure measure5Sec;
    static struct FullMeasure measure1Min;


    static Sample voltA[NUM_SAMPLES];
    static Sample voltB[NUM_SAMPLES];
    static Sample voltC[NUM_SAMPLES];

    static Sample currentA[NUM_SAMPLES];
    static Sample currentB[NUM_SAMPLES];
    static Sample currentC[NUM_SAMPLES];

    static int16 pos_adc_value = 0;             // Позиция текущих считываемых значений

    static FullMeasure Calculate();
    static void Calculate5Sec(const FullMeasure &);
    static void Calculate1Min(const FullMeasure &);
}


void Measurer::Update()
{
    if (BuffersFull())
    {
        measure = Calculate();

        Calculate5Sec(measure);

        Calculate1Min(measure);

        LOG_WRITE("%f V", measure.measures[0].voltage);

        pos_adc_value = 0;
    }
}


FullMeasure Measurer::LastMeasure()
{
    return measure;
}


FullMeasure Measurer::Measure5Sec()
{
    return measure5Sec;
}


FullMeasure Measurer::Measure1Min()
{
    return measure1Min;
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

    uint16 bufferA[32];

    for (int i = 0; i < 32; i++)
    {
        bufferA[i] = voltA[i];
    }


    result.measures[0].Calculate(voltA, currentA);

    uint16 bufferB[32];

    for (int i = 0; i < 32; i++)
    {
        bufferB[i] = voltB[i];
    }


    result.measures[1].Calculate(voltB, currentB);

    uint16 buffer[32];

    for (int i = 0; i < 32; i++)
    {
        buffer[i] = voltC[i];
    }

    result.measures[2].Calculate(voltC, currentC);

    PhaseMeasure aveVoltageC;
    aveVoltageC.CalculateLimits(voltC);

    PhaseMeasure aveAmpersC;
    aveAmpersC.CalculateLimits(currentC);

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


void Measurer::Calculate5Sec(const FullMeasure &meas)
{
    static TimeMeterMS meter;

    static int counter = 0;

    static FullMeasure result;

    if (meter.ElapsedTime() >= 5000)
    {
        result.Average(counter);

        measure5Sec = result;

        result.Clear();

        meter.Reset();

        counter = 0;
    }

    result.AppendMeasure(meas);
}


void Measurer::Calculate1Min(const FullMeasure &meas)
{
    static TimeMeterMS meter;

    static int counter = 0;

    static FullMeasure result;

    if (meter.ElapsedTime() >= 1000 * 60)
    {
        result.Average(counter);

        measure1Min = result;

        result.Clear();

        meter.Reset();

        counter = 0;
    }

    result.AppendMeasure(meas);
}


void FullMeasure::Clear()
{
    for (int i = 0; i < 3; i++)
    {
        measures[i].voltage = 0.0f;
        measures[i].current = 0.0f;
    }
}


void FullMeasure::AppendMeasure(const FullMeasure &meas)
{
    for (int i = 0; i < 3; i++)
    {
        measures[i].voltage += meas.measures[i].voltage;
        measures[i].current += meas.measures[i].current;
    }
}


void FullMeasure::Average(int number)
{
    for (int i = 0; i < 3; i++)
    {
        measures[i].voltage /= (float)number;
        measures[i].current /= (float)number;
    }
}
