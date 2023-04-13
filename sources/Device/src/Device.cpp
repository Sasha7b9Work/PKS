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
#include "Hardware/Contactor.h"
#include <gd32f30x_rcu.h>


namespace Device
{
    static void UpdateModem();
}


void Device::Init()
{
    HAL::Init();

    Modem::Init();

    Contactor::Init();

    FlashDisk::Init();

//    Display::Init();
}


void Device::Update()
{
    Measurer::Update();

    if (Measurer::MeasureReady())
    {
        volatile FullMeasure measure = Measurer::GetMeasure();
//        Contactor::Update(measure);
    }

    Updater::Update();

    UpdateModem();

//    Display::Update();
}


void Device::UpdateModem()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 1000)
    {
        meter.Reset();

        pchar answer = Modem::LastAnswer();
        LOG_WRITE("modem %s", answer);

        Modem::Transmit("AT+IPR?");
    }
}
