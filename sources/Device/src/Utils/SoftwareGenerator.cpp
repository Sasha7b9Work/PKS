// 2023/04/07 10:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/SoftwareGenerator.h"
#include <cmath>
#include <cstdlib>


#define M_PI 3.141592653589793238462643383279502884


namespace Generator
{
    void GenerateLineVoltage(Sample[NUM_SAMPLES], float level);
    void GenerateLineCurrent(Sample[NUM_SAMPLES], float level);
    void GenerateSineVoltage(Sample[NUM_SAMPLES], float amplitude);
    void GenerateSineCurrent(Sample[NUM_SAMPLES], float amplitude);
}


void Generator::GenerateVoltage(Sample samples[NUM_SAMPLES])
{
    GenerateSineVoltage(samples, 100.0);
}


void Generator::GenerateCurrent(Sample samples[NUM_SAMPLES])
{
    GenerateSineCurrent(samples, 10.0);
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


void Generator::GenerateSineVoltage(Sample samples[NUM_SAMPLES], float amplitude)
{
    double start_angle = (double)std::rand() / 10.0;

    double dA = 2.0 * M_PI / SAMPLES_ON_PERIOD;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        samples[i].FromVoltage((float)((double)amplitude * std::sin(start_angle + dA * i)));
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
