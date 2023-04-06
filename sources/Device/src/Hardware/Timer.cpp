// 2023/04/05 16:54:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


namespace Timer
{
    static uint timeMS = 0;
}


void Timer::Init()
{
    nvic_irq_enable(TIMER1_IRQn, 0, 0);

    timer_parameter_struct timer_initpara;

    timer_initpara.prescaler = 1;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = (uint)(-1);
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    timer_init(TIMER1, &timer_initpara);

    timer_interrupt_enable(TIMER1, TIMER_INT_UP);

    timer_enable(TIMER1);
}


uint Timer::TimeMS()
{
    return timeMS;
}


void Timer::OnCallbackInterrupt()
{
    timeMS++;
}


void TimeMeterMS::Reset()
{
    time_reset = Timer::timeMS;
}


uint TimeMeterMS::ElapsedTime()
{
    return Timer::timeMS - time_reset;
}
