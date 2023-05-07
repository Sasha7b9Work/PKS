// 2023/03/16 16:53:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include <math.h>


struct Averager
{
    static void Clear()
    {
        pointer = 0;
    }

    static void Push(float value)
    {
        if (pointer < NUMBER)
        {
            values[pointer++] = value;
        }
        else
        {
            for (int i = 1; i < NUMBER; i++)
            {
                values[i - 1] = values[i];
            }

            values[NUMBER - 1] = value;
        }
    }

    static float Pop()
    {
        float sum = 0.0f;

        for (int i = 0; i < pointer; i++)
        {
            sum += values[i];
        }

        return sum / (float)pointer;
    }

private:

    static const int NUMBER = 100;

    static float values[NUMBER];

    static int pointer;
};

int Averager::pointer = 0;
float Averager::values[Averager::NUMBER];

namespace Calculator
{
    static int CalculatePeriod(const Sample samples[NUM_SAMPLES]);
}


void PhaseMeasure::Calculate(const Sample samplesVolts[NUM_SAMPLES], const Sample samplesAmpers[NUM_SAMPLES])
{
    int period = Calculator::CalculatePeriod(samplesVolts);

    // Рассчитываем ток

    float currentRMS = 0.0f;

    Averager::Clear();

    for (int i = 0; i < period; i++)
    {
        Averager::Push(samplesAmpers[i].ToCurrent());

        float value = Averager::Pop();

        currentRMS += value * value;
    }

    current = sqrtf(currentRMS / (float)period);

    // Рассчитывем напряжение

    float voltsRMS = 0.0f;

    float max = -1e30f;
    float min = 1e30f;

    Averager::Clear();

    for (int i = 0; i < period; i++)
    {
        Averager::Push(samplesVolts[i].ToVoltage());

        float value = Averager::Pop();

        if (value < min)
        {
            min = value;
        }

        if (value > max)
        {
            max = value;
        }

        voltsRMS += value * value;
    }

    voltage = sqrtf(voltsRMS / (float)period);
}


void PhaseMeasure::CalculateLimits(const Sample samples[NUM_SAMPLES], FullMeasure *result)
{
    float sum = 0.0f;

    float max = 0.0f;

    float min = 1e5f;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        sum += samples[i];

        if (samples[i] > max)
        {
            max = samples[i];
        }

        if (samples[i] < min)
        {
            min = samples[i];
        }
    }

    result->measures[0].voltage = sum / NUM_SAMPLES;
    result->measures[1].voltage = min;
    result->measures[2].voltage = max;
}


int Calculator::CalculatePeriod(const Sample samples[NUM_SAMPLES])
{
    uint sums[NUM_SAMPLES];

    sums[0] = samples[0];

    for (int i = 1; i < NUM_SAMPLES; i++)
    {
        sums[i] = sums[i - 1] + samples[i];
    }

    int min_period = (int)(NUM_SAMPLES - 2.5f * SAMPLES_ON_PERIOD);     // Минимальный рассматриваемый период
    int max_period = (int)(NUM_SAMPLES - 1.0f * SAMPLES_ON_PERIOD - 3);

    uint min_delta = (uint)(-1);
    int period_min_delta = 0;

    for (int period = min_period; period < max_period; period++)      // Рассматриваем периоды от минимального до максимального
    {
        uint min = (uint)(-1);
        uint max = 0;

        for (int start = 0; start < (int)SAMPLES_ON_PERIOD; start++)
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
