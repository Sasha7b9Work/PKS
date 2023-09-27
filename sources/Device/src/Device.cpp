// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Storage/Storage.h"
#include "Measurer/Contactors.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/Timer.h"


void Device::Init()
{
    Log::Init();

    HAL::Init();

    Contactors::Init();

    Storage::Init();

    Display::Init();

    Modem::Init();
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Contactors::Serviceability::Verify();

    HAL_PINS::Update();

    Storage::Update();

    Display::Update();

    static int counter = 0;
    static uint prev_time = 0;

    if ((TIME_MS / 1000) > prev_time)
    {
        prev_time = TIME_MS / 1000;

        MQTT::Send::Counter(counter++);
    }

    Modem::Update();
}
