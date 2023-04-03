// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


void HAL::Init()
{
    systick_config();
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
