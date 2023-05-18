// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


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

    rcu_periph_clock_enable(RCU_USART1);    // Log
    rcu_periph_clock_enable(RCU_UART3);     // GPRS

    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER1);    // Для опроса контакторв
    rcu_periph_clock_enable(RCU_TIMER5);

    HAL_USART_GPRS::Init();

    HAL_PINS::Init();

    //  HAL_WWDGT::Init();

#ifdef ENABLE_LOG
    HAL_USART_LOG::Init();
#endif
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
