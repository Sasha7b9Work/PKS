// 2023/03/16 16:53:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Calculator.h"
#include "Measurer/Measurer.h"


namespace Calculator
{
    float CalculateCurrentRMS(const uint16 samples[NUM_POINTS]);

    float CalculateVoltageRMS(const uint16 samples[NUM_POINTS]);
}
