// 2023/03/16 16:53:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Calculator.h"


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
    uint period_min_delta = 0;

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


float Calculator_CalculateCurrent(const uint16 samples[NUM_POINTS])
{
    int period = CalculatePeriod(samples);

    return 0.0f;
}


float Calculator_CalculateVoltage(const uint16 samples[NUM_POINTS])
{
    return 0.0f;
}
