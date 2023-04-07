// 2023/04/07 10:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/SoftwareGenerator.h"


namespace Generator
{
    static void GenerateLineVoltage(Sample[NUM_SAMPLES], float level);
    static void GenerateLineCurrent(Sample[NUM_SAMPLES], float level);
    static void GenerateSineVoltage(Sample[NUM_SAMPLES], float amplitude);
    static void GenerateSineCurrent(Sample[NUM_SAMPLES], float amplitude);
}


void Generator::GenerateVoltage(Sample samples[NUM_SAMPLES])
{
    GenerateLineVoltage(samples, 220.0f);
}


void Generator::GenerateCurrent(Sample samples[NUM_SAMPLES])
{
    GenerateLineCurrent(samples, -5.0f);
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

}


void Generator::GenerateSineCurrent(Sample samples[NUM_SAMPLES], float amplitude)
{

}
