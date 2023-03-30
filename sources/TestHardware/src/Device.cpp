// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "Calculator/Calculator.h"
#include "FlashDisk/FlashDisk.h"
#include "Test.h"


void Device::Init()
{
    HAL::Init();

    Timer::Init();

    Timer::Delay(500);

    Display::Init();

    FlashDisk::Init();

    Measurer::Init();

    Modem::Init();
}


void Device::Update()
{
    if (Test::Display())
    {
        LOG_WRITE("Display is Ok!");
    }
    else
    {
        LOG_ERROR("Display failed");
    }

    if (Test::Modem())
    {
        LOG_WRITE("Modem is Ok!");
    }
    else
    {
        LOG_ERROR("Modem failed");
    }
}
