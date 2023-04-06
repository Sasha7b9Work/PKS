// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32f30x.h>


PinADC pinVolt1(GPIOA, GPIO_PIN_0, ADC_CHANNEL_0);
PinADC pinVolt2(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);
PinADC pinVolt3(GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);

PinADC pinCur1L(GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
PinADC pinCur2L(GPIOA, GPIO_PIN_5, ADC_CHANNEL_5);
PinADC pinCur3L(GPIOA, GPIO_PIN_7, ADC_CHANNEL_7);

PinADC pinCur1H(GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
PinADC pinCur2H(GPIOA, GPIO_PIN_6, ADC_CHANNEL_6);
PinADC pinCur3H(GPIOC, GPIO_PIN_4, ADC_CHANNEL_14);

const uint I2C_ADDR = I2C0;
PinI2C pinI2C_SCL(GPIOB, GPIO_PIN_10);
PinI2C pinI2C_SDA(GPIOB, GPIO_PIN_11);

const uint USART_ADDR = USART1;
PinUSART_TX pinUSART_TX(GPIOD, GPIO_PIN_5);
PinUSART_RX pinUSART_RX(GPIOD, GPIO_PIN_6);


void PinADC::Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}


void PinI2C::Init()
{
    gpio_init(port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_TX::Init()
{
    gpio_init(port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_RX::Init()
{
    gpio_init(port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin);
}
