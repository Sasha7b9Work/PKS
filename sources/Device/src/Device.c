// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "FlashDisk/FlashDisk.h"
#include "Test.h"
#include "gd32f30x_rcu.h"


void Device_Init(void)
{
    HAL_Init();

//    Display_Init();
}


void Device_Update(void)
{
    if (Measurer_MeasureReady())
    {
        struct FullMeasure measure = Measurer_GetMeasure();

        measure = measure;
    }


//    Display_Update();

/*
    Test::Display() ? LOG_WRITE("Display is Ok!") : LOG_ERROR("Display failed");

    Test::Modem() ? LOG_WRITE("Modem is Ok!") : LOG_ERROR("Modem failed");

    Test::Measurer() ? LOG_WRITE("Measurer is Ok!") : LOG_WRITE("Measurer failed");

    Test::FlashDrive() ? LOG_WRITE("FlashDrive is Ok!") : LOG_WRITE("FlashDrive failed");
*/
}
