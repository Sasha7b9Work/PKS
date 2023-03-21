// 2023/03/21 11:47:45 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include <stm32f1xx_hal.h>

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *)
{
    return HAL_OK;
}


HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *, uint32_t)
{
    return HAL_OK;
}


uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *)
{
    return 0;
}


HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *, ADC_ChannelConfTypeDef *)
{
    return HAL_OK;
}
