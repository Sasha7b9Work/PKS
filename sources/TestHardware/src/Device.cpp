// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/Modem/Modem.h"
#include "Modules/Display/Display.h"
#include "Modules/Measurer/Measurer.h"
#include "Blocks/Calculator/Calculator.h"
#include "Modules/FlashDisk/FlashDisk.h"


namespace Device
{

}


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

}
