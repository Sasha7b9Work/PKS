// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "FlashDisk/FlashDisk.h"
#include "Test.h"
#include "Utils/Updater.h"
#include "Hardware/Timer.h"
#include <gd32f30x_rcu.h>


void Device::Init()
{
    HAL::Init();

    Timer::Init();
}


void Device::Update()
{
    Measurer::Update();

    if (Measurer::MeasureReady())
    {
        struct FullMeasure measure = Measurer::GetMeasure();

        measure = measure;
    }

    Updater::Update();
}
