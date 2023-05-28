// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/Timer.h"
#include "Measurer/Contactors.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include <gd32f30x_rcu.h>


void Device::Init()
{
    HAL::Init();
    
    Contactors::Init();

    Modem::Init();

    FlashDisk::Init();

    Display::Init();
}


void Device::Update()
{
    char *buffer = Log::buffer;

    static int counter = 0;
    counter++;

    LOG_WRITE("%d", counter);

    HAL_FWDGT::Update();

    Measurer::Update();

    Display::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Modem::Update();

    HAL_PINS::Update();

    Contactors::VerifyCondition();
}
