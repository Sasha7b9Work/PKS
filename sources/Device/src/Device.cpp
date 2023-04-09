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


namespace Device
{
    static void UpdateModem();
    
    static void ProcessMeasure();
}


void Device::Init()
{
    HAL::Init();

    Modem::Init();

//    Display::Init();
}


void Device::Update()
{
    Measurer::Update();

    if (Measurer::MeasureReady())
    {
        ProcessMeasure();
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


void Device::ProcessMeasure()
{
    struct FullMeasure measure = Measurer::GetMeasure();

    static TimeMeterMS meter;

    LOG_WRITE("voltage %f, current %f, power %f", measure.measures[0].voltage, measure.measures[0].current, measure.measures[0].power);

    meter.Reset();
}
