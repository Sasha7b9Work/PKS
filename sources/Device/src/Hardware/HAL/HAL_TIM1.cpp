// 2023/5/8 19:15:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_TIM1::Init()
{
    /* -----------------------------------------------------------------------
TIMER2CLK is 2KHz
TIMER1 channel0 duty cycle = (2000 / 4000) * 100% = 50%
----------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER1);

    timer_deinit(TIMER1);

    /* TIMER configuration */
    timer_initpara.prescaler = 59999;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 3999;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);

    /* auto-reload preload enable */
    timer_enable(TIMER1);
}
