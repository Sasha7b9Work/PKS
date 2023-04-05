// 2023/03/16 16:53:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Calculator.h"
#include <math.h>


#define VOLTS_IN_SAMPLE (380.0f / 2047.0f)

#define AMPERS_IN_SAMPLE (10.0f / 2047.0f)


static float SampleToVoltage(uint16 sample)
{
    return ((float)sample - 2047.0f) * VOLTS_IN_SAMPLE;
}


static float SampleToCurrent(uint16 sample)
{
    return ((float)sample - 2047.0f) * AMPERS_IN_SAMPLE;
}


static int CalculatePeriod(const uint16 samples[NUM_POINTS])
{
    uint sums[NUM_POINTS];

    sums[0] = samples[0];

    for (int i = 1; i < NUM_POINTS; i++)
    {
        sums[i] = sums[i - 1] + samples[i];
    }

    int min_period = (int)(NUM_POINTS - 1.5f * POINTS_ON_PERIOD);     // Минимальный рассматриваемый период
    int max_period = (int)(NUM_POINTS - 0.5f * POINTS_ON_PERIOD);

    uint min_delta = (uint)(-1);
    int period_min_delta = 0;

    for (int period = min_period; period < max_period; period++)      // Рассматриваем периоды от минимального до максимального
    {
        uint min = (uint)(-1);
        uint max = 0;

        for (int start = 0; start < (NUM_POINTS - period); start++)
        {
            uint sum = sums[start + period] - sums[start];

            if (sum < min) { min = sum; }

            if (sum > max) { max = sum; }
        }

        uint delta = max - min;

        if (delta < min_delta)
        {
            min_delta = delta;
            period_min_delta = period;
        }
    }

    return period_min_delta;
}


float Calculator::CalculateCurrentRMS(const uint16 samples[NUM_POINTS])
{
    int period = CalculatePeriod(samples);

    float result = 0.0f;

    for (int i = 0; i < period; i++)
    {
        float current = SampleToCurrent(samples[i]);

        result += current * current;
    }

    return sqrtf(result / (float)period);
}


float Calculator::CalculateVoltageRMS(const uint16 samples[NUM_POINTS])
{
    int period = CalculatePeriod(samples);

    float result = 0.0f;

    for (int i = 0; i < period; i++)
    {
        float voltage = SampleToVoltage(samples[i]);

        result += voltage * voltage;
    }

    return sqrtf(result / (float)period);
}
