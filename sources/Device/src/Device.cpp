// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Modem/SIM800.h"
#include "Modem/MQTT/MQTT.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Measurer/Contactors.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include <gd32f30x_rcu.h>


void Device::Init()
{
    Log::Init();

    LOG_WRITE("   Device::Init()   ");

    HAL::Init();
    
    Contactors::Init();

    Display::Init();

    Modem::Init();
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Display::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Modem::Update();

    HAL_PINS::Update();

    Contactors::Serviceability::Verify();
}
