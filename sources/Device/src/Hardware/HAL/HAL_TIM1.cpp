// 2023/5/8 19:15:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_TIM1::Init()
{
    /* -----------------------------------------------------------------------
TIMER2CLK is 2KHz
TIMER5 channel0 duty cycle = (2000 / 4000) * 100% = 50%
----------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER5);

    /* TIMER configuration */
    timer_initpara.prescaler = 5999;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 399;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    timer_struct_para_init(&timer_initpara);

    timer_init(TIMER5, &timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER5);

    timer_update_event_enable(TIMER5);

    /* auto-reload preload enable */
    timer_enable(TIMER5);
}
