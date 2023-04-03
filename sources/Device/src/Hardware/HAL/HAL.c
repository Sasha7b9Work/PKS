// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


void HAL_Init(void)
{
    systick_config();
    
    HAL_I2C0_Init();

    HAL_ADC_Init();
}


void HAL_ErrorHandler(void)
{
    while (1)
    {
    }
}
