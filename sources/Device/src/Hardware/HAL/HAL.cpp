// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


void HAL::Init(void)
{
    systick_config();
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    
    HAL_I2C0_Init();

    HAL_ADC_Init();
}


void HAL::ErrorHandler(void)
{
    while (1)
    {
    }
}
