﻿// 2023/04/07 10:58:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/SoftwareGenerator.h"


namespace Generator
{
    static void GenerateLineNull(uint16[NUM_POINTS]);
    static void GenerateLineMax(uint16[NUM_POINTS]);
    static void GenerateLineMin(uint16[NUM_POINTS]);
}


void Generator::GenerateVoltage(uint16 samples[NUM_POINTS])
{
    GenerateLineMax(samples);
}


void Generator::GenerateCurrent(uint16 samples[NUM_POINTS])
{
    GenerateLineMax(samples);
}


void Generator::GenerateLineNull(uint16 samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = 2047;
    }
}


void Generator::GenerateLineMax(uint16 samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = 4095;
    }
}


void Generator::GenerateLineMin(uint16 samples[NUM_POINTS])
{
    for (int i = 0; i < NUM_POINTS; i++)
    {
        samples[i] = 0;
    }
}
