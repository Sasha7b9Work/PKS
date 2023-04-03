// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


void HAL_Init()
{
    systick_config();
    
    HAL_I2C0_Init();
}


void HAL_ErrorHandler()
{
    while (1)
    {
    }
}
