// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


/*
*   Задействованные ресурсы
*   ADC0     - измерение входных параметров
*   DMA0_CH0 - АЦП
*   TIM0_CH0 - АЦП
*   TIM1     - таймер мс
*/


void HAL::Init()
{
    systick_config();
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);

    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    rcu_periph_clock_enable(RCU_GPIOE);
    rcu_periph_clock_enable(RCU_GPIOF);

    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_I2C0);
    rcu_periph_clock_enable(RCU_I2C1);
    rcu_periph_clock_enable(RCU_TIMER0);    // ADC
    rcu_periph_clock_enable(RCU_TIMER1);    // Timer
    rcu_periph_clock_enable(RCU_USART1);    // Log
    rcu_periph_clock_enable(RCU_UART3);     // GPRS
    rcu_periph_clock_enable(RCU_AF);

    HAL_I2C::Init();

    HAL_ADC::Init();

    HAL_USART_GPRS::Init();

#ifdef DEV_BOARD
    HAL_USART_LOG::Init();
#endif
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
