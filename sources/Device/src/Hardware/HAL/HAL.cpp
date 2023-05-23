// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


/*
*   ��������������� �������
*   ADC0     - ��������� ������� ����������
*   DMA0_CH0 - ���
*   TIM0_CH0 - ���
*   TIM1     - ������ ��
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

    rcu_periph_clock_enable(RCU_ADC0);      // ���� ����������
    rcu_periph_clock_enable(RCU_DMA0);      // ���� ����������
    rcu_periph_clock_enable(RCU_I2C0);      // Display
    rcu_periph_clock_enable(RCU_I2C1);
    rcu_periph_clock_enable(RCU_SPI2);      // Ext Flash
    rcu_periph_clock_enable(RCU_TIMER0);    // ADC
    rcu_periph_clock_enable(RCU_USART1);    // Log
    rcu_periph_clock_enable(RCU_UART3);     // GPRS
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER1);    // ��� ������ ����������
    rcu_periph_clock_enable(RCU_TIMER5);
    rcu_periph_clock_enable(RCU_WWDGT);

    HAL_I2C::Init();

    HAL_ADC::Init();

    HAL_USART_GPRS::Init();

    HAL_SPI::Init();
    
    HAL_RTC::Init();

    HAL_PINS::Init();

    HAL_WWDGT::Init();

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
