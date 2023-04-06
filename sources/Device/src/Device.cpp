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


static void UpdateLEDS();


void Device::Init()
{
    HAL::Init();

    Timer::Init();

    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);
}


void Device::Update()
{
    Measurer::Update();

    if (Measurer::MeasureReady())
    {
        struct FullMeasure measure = Measurer::GetMeasure();

        measure = measure;

        HAL_ADC::Start();
    }

    Updater::Update();

    UpdateLEDS();
}


static void UpdateLEDS()
{
    static bool fired = true;
    static uint next_time = 1000;

    uint time = Timer::TimeMS();

    if (time >= next_time)
    {
        fired = !fired;

        if (fired)
        {
            gpio_bit_set(GPIOA, GPIO_PIN_6);
            gpio_bit_reset(GPIOA, GPIO_PIN_7);
        }
        else
        {
            gpio_bit_reset(GPIOA, GPIO_PIN_6);
            gpio_bit_set(GPIOA, GPIO_PIN_7);
        }
    }
}
