// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include "Modules/SIM800C/SIM800C.h"
#include <stm32f1xx_hal.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>


namespace Device
{

}


void Device::Init()
{
    HAL::Init();

    Timer::Init();

    Timer::Delay(500);

    SIM800C::Init();
}


void Device::Update()
{
}
