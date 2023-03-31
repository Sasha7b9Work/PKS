// 2022/6/18 10:53:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <stm32f3xx_hal.h>


void HAL_PINS::Init()
{
#ifndef GUI
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
#endif

    GPIO_InitTypeDef is =                                   //
    {                                                       //
        GPIO_PIN_1  | GPIO_PIN_5,                           //
        GPIO_MODE_OUTPUT_PP,                                //
        GPIO_NOPULL                                         //
    };                                                      //
                                                            //
    HAL_GPIO_Init(GPIOA, &is);                              //
                                                            //
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);   //
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   //
                                                            //
    is.Pin = GPIO_PIN_1;                                    //
                                                            //
    HAL_GPIO_Init(GPIOD, &is);                              //
                                                            //
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);   //

    GPIO_InitTypeDef is2 =
    {
        GPIO_PIN_10 |               // ������ LR
        GPIO_PIN_11,                // ������ SND
        GPIO_MODE_INPUT,
        GPIO_NOPULL,
        GPIO_SPEED_HIGH
    };

    HAL_GPIO_Init(GPIOA, &is2);

    is2.Pin = GPIO_PIN_0;           // ������ LG

    HAL_GPIO_Init(GPIOB, &is2);
}