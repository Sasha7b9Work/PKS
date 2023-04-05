// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32f30x.h>


extern PinADC pinVolt1(GPIOA, GPIO_PIN_0, ADC_CHANNEL_0);
extern PinADC pinVolt2(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);
extern PinADC pinVolt3(GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);

extern PinADC pinCur1L(GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
extern PinADC pinCur2L(GPIOA, GPIO_PIN_5, ADC_CHANNEL_5);
extern PinADC pinCur3L(GPIOA, GPIO_PIN_7, ADC_CHANNEL_7);

extern PinADC pinCur1H(GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
extern PinADC pinCur2H(GPIOA, GPIO_PIN_6, ADC_CHANNEL_6);
extern PinADC pinCur3H(GPIOC, GPIO_PIN_4, ADC_CHANNEL_14);


void PinADC::Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}

