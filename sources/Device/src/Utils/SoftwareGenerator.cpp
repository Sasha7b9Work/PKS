// 2023/04/07 10:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/SoftwareGenerator.h"
#include <cstdlib>
#include <cmath>


#define M_PI 3.141592653589793238462643383279502884


namespace Generator
{
    void GenerateLineVoltage(Sample[NUM_SAMPLES], float level);
    void GenerateLineCurrent(Sample[NUM_SAMPLES], float level);
    void GenerateSineVoltage(Sample[NUM_SAMPLES], float amplitude, bool noise);
    void GenerateSineCurrent(Sample[NUM_SAMPLES], float amplitude);

    static double GenerateNoise();
}


void Generator::GenerateVoltage(Sample samples[NUM_SAMPLES])
{
    GenerateSineVoltage(samples, 220.0f * std::sqrtf(2.0f), false);
}


void Generator::GenerateCurrent(Sample samples[NUM_SAMPLES])
{
    GenerateSineCurrent(samples, 10.0f);
}


void Generator::GenerateLineVoltage(Sample samples[NUM_SAMPLES], float level)
{
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        samples[i].FromVoltage(level);
    }
}


void Generator::GenerateLineCurrent(Sample samples[NUM_SAMPLES], float level)
{
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        samples[i].FromCurrent(level);
    }
}


void Generator::GenerateSineVoltage(Sample samples[NUM_SAMPLES], float amplitude, bool noise)
{
    double start_angle = (double)std::rand() / 10.0;

    double dA = 2.0 * M_PI / SAMPLES_ON_PERIOD;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        double value = (double)amplitude * std::sin(start_angle + dA * i);

        if (noise)
        {
            value += GenerateNoise();
        }

        samples[i].FromVoltage((float)value);
    }
}


void Generator::GenerateSineCurrent(Sample samples[NUM_SAMPLES], float amplitude)
{
    double start_angle = (double)std::rand() / 10.0;

    double dA = 2.0 * M_PI / SAMPLES_ON_PERIOD;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        samples[i].FromCurrent((float)(amplitude * std::sin(start_angle + dA * i)));
    }
}


double Generator::GenerateNoise()
{
    static double noise = 0.0;
    const double bound = 70.0;

    double step = ((double)(std::rand()) / RAND_MAX - 0.5) * 2.0 * (bound / 20.0f);

    noise += step;

    if (noise > bound)
    {
        noise = bound;
    }
    else if (noise < -bound)
    {
        noise = -bound;
    }

    return noise;
}
