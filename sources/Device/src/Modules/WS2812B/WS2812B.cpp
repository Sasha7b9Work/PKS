// 2022/6/15 22:18:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modules/WS2812B/WS2812B.h"
#include "Hardware/Timer.h"
#include "Hardware/Communicator.h" 
#include <stm32f1xx_hal.h> 


namespace WS2812B
{
    uint dT0H = 5;      // В тактах
    uint dT0L = 13;     // В тактах
    uint dT1H = 13;     // В тактах
    uint dT1L = 5;      // В тактах

    void FireRED();
    void FireGREEN();
    void FireBLUE();
    void FireBLACK();

    void Fire(uint8 red, uint8 green, uint8 blue);

    namespace DLED
    {
        void Init()
        {
            GPIO_InitTypeDef is =
            {
                GPIO_PIN_1,
                GPIO_MODE_OUTPUT_PP,
                GPIO_PULLUP,
                GPIO_SPEED_HIGH
            };

            HAL_GPIO_Init(GPIOB, &is);

            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
        }

        // Записать полную последовательность бит в четыре индикатора
        // 96 бит = 4 индикатора по 24 бита
        // data представляет собой массив количества тактов, в которые нужно передёргивать из 0 в 1 и обратно.
        // data[0] - начало положительного импульса 0-го бита
        // data[1] - окончание положительного импульса 0-го бита
        // data[2], data[3] - --//-- 1-го бита
        // data[4], data[5] - --//-- 2-го бита
        // В массиве не 96 * 2 элементов
        // 24 - бит на индикатор, 2 - интервалов на бит, 4 - количество индикаторов, +4 - после 24 бит каждого индикатора идёт завершающий ноль
        static void WriteFullSequency(const uint data[24 * 2 * 4 + 4]);

        // Записать управляющую последовательность для одного индикатора. Здесь на 1 элемент больше, чем нужно переключений - завершается нулём
        static void WriteOneIndicator(const uint data[24 * 2 + 1]);
    }
}


void WS2812B::Init()
{
    DLED::Init();
}


void WS2812B::Update()
{
    bool stateLR = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET;
    bool stateLG = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET;

    if (!stateLR && stateLG)
    {
        FireRED();
    }
    else if (stateLR && !stateLG)
    {
        FireGREEN();
    }
    else if (!stateLR && !stateLG)
    {
        FireBLUE();
    }
    else if (stateLR && stateLG)
    {
        FireBLACK();
    }
}


void WS2812B::FireRED()
{
    Fire(255, 0, 0);
//    Fire(127, 68, 0);      // Оранжевый
}


void WS2812B::FireGREEN()
{
    Fire(0, 255, 0);
//    Fire(63, 191, 255);      // Синий
}


void WS2812B::FireBLUE()
{
    Fire(0, 0, 255);
}


void WS2812B::FireBLACK()
{
    Fire(0, 0, 0);
}


void WS2812B::Fire(uint8 red, uint8 green, uint8 blue)
{
    /*
    * Цвет хранится в формате GRB
    */

// Записать текущий счётчик времени в массив и увеличить его на delta
#define WRITE_TIME(delta)   *time++ = delta;

    uint value = (uint)((green << 16) | (red << 8) | blue);

    uint bits[24 * 2 * 4 + 4];
    uint *time = &bits[0];

    for (int indicator = 0; indicator < 4; indicator++)
    {
        uint col = (value << 8);

        for (int i = 0; i < 24; i++)
        {
            if (col & 0x80000000)       // единица
            {
                WRITE_TIME(dT1H);
                WRITE_TIME(dT1L);
            }
            else                        // ноль
            {
                WRITE_TIME(dT0H);
                WRITE_TIME(dT0L);
            }

            col <<= 1;
        }

        *time++ = 0;
    }

//    USART2->CR1 &= ~USART_CR1_RXNEIE;       // Запрещаем прерывания по приёму

    __disable_irq();

    __DSB();
    __ISB();

    DLED::WriteFullSequency(bits);
    
    __enable_irq();

//    USART2->CR1 |= USART_CR1_RXNEIE;        // Разрешаем прерывания на приём
}


void WS2812B::DLED::WriteFullSequency(const uint data[24 * 2 * 4 + 4])
{
    for (int i = 0; i < 4; i++)
    {
        if (USART2->SR & USART_SR_RXNE)
        {
            Communicator::AppendByte((uint8)USART2->DR);
        }

        WriteOneIndicator(data + i * (24 * 2 + 1));         // 1 - это завершающий ноль. На каждый индикатор идёт 49 элементов массива, а не 48

        if (USART2->SR & USART_SR_RXNE)
        {
            Communicator::AppendByte((uint8)USART2->DR);
        }
    }
}

#ifdef WIN32
#pragma warning(push, 0)
#pragma warning disable
#endif


void WS2812B::DLED::WriteOneIndicator(const uint data[24 * 2 + 1])
{
#define PIN_SET      GPIOB->BSRR = GPIO_PIN_1
#define PIN_RESET    GPIOB->BSRR = (uint)(GPIO_PIN_1 << 16)

#define WRITE_BIT   asm("mov counter, *data++");    \
                    PIN_SET;                        \
                    while(--counter) {}             \
                    asm("mov counter, *data++");    \
                    PIN_RESET;                      \
                    while(--counter) {}

    uint counter = 0;

    WRITE_BIT;  // 1 //-V760
    WRITE_BIT;  // 2
    WRITE_BIT;  // 3
    WRITE_BIT;  // 4
    WRITE_BIT;  // 5
    WRITE_BIT;  // 6
    WRITE_BIT;  // 7
    WRITE_BIT;  // 8
    WRITE_BIT;  // 9
    WRITE_BIT;  // 10
    WRITE_BIT;  // 11
    WRITE_BIT;  // 12
    WRITE_BIT;  // 13
    WRITE_BIT;  // 14
    WRITE_BIT;  // 15
    WRITE_BIT;  // 16
    WRITE_BIT;  // 17
    WRITE_BIT;  // 18
    WRITE_BIT;  // 19
    WRITE_BIT;  // 20
    WRITE_BIT;  // 21
    WRITE_BIT;  // 22
    WRITE_BIT;  // 23
    WRITE_BIT;  // 24
}


/*
void WS2812B::DLED::WriteOneIndicator(const uint data[24 * 2 + 1])
{

#define PIN_SET      GPIOB->BSRR = GPIO_PIN_1
#define PIN_RESET    GPIOB->BSRR = (uint)(GPIO_PIN_1 << 16)

#define WRITE_HI    asm("mov dat, *data++");        \
                    while (TIM2->CNT < dat) {}      \
                    PIN_SET;

#define WRITE_LOW   asm("mov dat, *data++;");        \
                    while (TIM2->CNT < dat) {}      \
                    PIN_RESET;

#define WRITE_BIT   WRITE_HI; WRITE_LOW;


    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->CNT = 0;
    TIM2->CR1 |= TIM_CR1_CEN;

    uint dat = 0;

    WRITE_BIT;  // 1
    WRITE_BIT;  // 2
    WRITE_BIT;  // 3
    WRITE_BIT;  // 4
    WRITE_BIT;  // 5
    WRITE_BIT;  // 6
    WRITE_BIT;  // 7
    WRITE_BIT;  // 8
    WRITE_BIT;  // 9
    WRITE_BIT;  // 10
    WRITE_BIT;  // 11
    WRITE_BIT;  // 12
    WRITE_BIT;  // 13
    WRITE_BIT;  // 14
    WRITE_BIT;  // 15
    WRITE_BIT;  // 16
    WRITE_BIT;  // 17
    WRITE_BIT;  // 18
    WRITE_BIT;  // 19
    WRITE_BIT;  // 20
    WRITE_BIT;  // 21
    WRITE_BIT;  // 22
    WRITE_BIT;  // 23
    WRITE_BIT;  // 24
}
*/

#ifdef WIN32
#pragma warning pop
#endif
