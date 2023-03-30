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
    Test::Display() ? LOG_WRITE("Display is Ok!") : LOG_ERROR("Display failed");

    Test::Modem() ? LOG_WRITE("Modem is Ok!") : LOG_ERROR("Modem failed");

    Test::Measurer() ? LOG_WRITE("Measurer is Ok!") : LOG_WRITE("Measurer failed");
}
