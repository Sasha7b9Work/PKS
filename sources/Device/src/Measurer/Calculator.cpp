// 2023/03/16 16:53:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Calculator.h"
#include <math.h>


namespace Calculator
{
    static int CalculatePeriod(const Sample samples[NUM_SAMPLES]);
}


float Calculator::CalculateCurrentRMS(const Sample samples[NUM_SAMPLES])
{
    int period = CalculatePeriod(samples);

    float result = 0.0f;

    for (int i = 0; i < period; i++)
    {
        float current = samples[i].ToCurrent();

        result += current * current;
    }

    return sqrtf(result / (float)period);
}


float Calculator::CalculateVoltageRMS(const Sample samples[NUM_SAMPLES])
{
    int period = CalculatePeriod(samples);

    float result = 0.0f;

    for (int i = 0; i < period; i++)
    {
        float voltage = Sample(samples[i]).ToVoltage();

        result += voltage * voltage;
    }

    return sqrtf(result / (float)period);
}


int Calculator::CalculatePeriod(const Sample samples[NUM_SAMPLES])
{
    uint sums[NUM_SAMPLES];

    sums[0] = samples[0];

    for (int i = 1; i < NUM_SAMPLES; i++)
    {
        sums[i] = sums[i - 1] + samples[i];
    }

    int min_period = (int)(NUM_SAMPLES - 1.5f * SAMPLES_ON_PERIOD);     // Минимальный рассматриваемый период
    int max_period = (int)(NUM_SAMPLES - 0.5f * SAMPLES_ON_PERIOD);

    uint min_delta = (uint)(-1);
    int period_min_delta = 0;

    for (int period = min_period; period < max_period; period++)      // Рассматриваем периоды от минимального до максимального
    {
        uint min = (uint)(-1);
        uint max = 0;

        for (int start = 0; start < (NUM_SAMPLES - period); start++)
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
