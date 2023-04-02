// 2022/6/17 0:37:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace HAL_ADC
{
    static ADC_HandleTypeDef handle;
}


void HAL_ADC::Init()
{
//#ifndef GUI
//    __HAL_RCC_ADC1_CLK_ENABLE();
//#endif
//
//    GPIO_InitTypeDef is =
//    {
//        GPIO_PIN_0,
//        GPIO_MODE_ANALOG,
//        0,
//        0
//    };
//
//    HAL_GPIO_Init(GPIOA, &is);
//
//    handle.Instance = ADC1;
//    handle.Init.ScanConvMode = ADC_SCAN_DISABLE;
//    handle.Init.ContinuousConvMode = DISABLE;
//    handle.Init.DiscontinuousConvMode = DISABLE;
//    handle.Init.ExternalTrigConv = ADC_SOFTWARE_START;
//    handle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
//    handle.Init.NbrOfConversion = 1;
//
//    HAL_ADC_Init(&handle);
//
//    ADC_ChannelConfTypeDef sConfig =
//    {
//        ADC_CHANNEL_0,
//        ADC_REGULAR_RANK_1,
//        ADC_SAMPLETIME_1CYCLE_5
//    };
//
//    HAL_ADC_ConfigChannel(&handle, &sConfig);
//
//    HAL_ADCEx_Calibration_Start(&handle);
}


void HAL_ADC::Update()
{
    HAL_ADC_Start(&handle);

    HAL_ADC_PollForConversion(&handle, 10);
}


float HAL_ADC::GetVoltage(int, int)
{
    return 0.0f;
}
