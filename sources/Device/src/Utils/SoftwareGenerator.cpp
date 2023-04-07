// 2023/04/07 10:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/SoftwareGenerator.h"


namespace Generator
{
    static void GenerateLineZero(Sample[NUM_POINTS]);
    static void GenerateLineMax(Sample[NUM_POINTS]);
    static void GenerateLineMin(Sample[NUM_POINTS]);
}


void Generator::GenerateVoltage(Sample samples[NUM_POINTS])
{
    GenerateLineMax(samples);
}


void Generator::GenerateCurrent(Sample samples[NUM_POINTS])
{
    GenerateLineMax(samples);
}


void Generator::GenerateLineZero(Sample samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = Sample::ZERO;
    }
}


void Generator::GenerateLineMax(Sample samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = Sample::MAX;
    }
}


void Generator::GenerateLineMin(Sample samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = Sample::MIN;
    }
}
