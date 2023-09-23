// 2023/09/14 11:48:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


struct Measurements
{
    uint number;
    FullMeasure measure;
};


namespace Storage
{
    void Init();

    void Update();
}
