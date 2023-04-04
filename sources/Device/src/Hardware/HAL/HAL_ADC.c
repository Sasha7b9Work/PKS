// 2023/04/03 19:52:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>
#include <systick.h>


/*
    ADC012_IN0      23  PA0
    ADC012_IN1      24  PA1
    ADC012_IN2      25  PA2
    ADC012_IN3      26  PA3
    ADC012_IN4      29  PA4
    ADC012_IN5      30  PA5
    ADC012_IN6      31  PA6
    ADC012_IN7      32  PA7
    ADC012_IN8      35  PB0
    ADC012_IN9      36  PB1
    ADC012_IN10     15  PC0
    ADC012_IN11     16  PC1
    ADC012_IN12     17  PC2
    ADC012_IN13     18  PC3
    ADC012_IN14     33  PC4
    ADC012_IN15     34  PC5
*/


void HAL_ADC_Init()
{

}


void HAL_ADC_Update()
{

}


float HAL_ADC_GetVoltage(int num, int ch)
{
    return 0.0f;
}
