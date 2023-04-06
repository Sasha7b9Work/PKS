// 2023/04/05 16:54:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


void Timer::Init()
{
    timer_parameter_struct timer_initpara;

    timer_initpara.prescaler = 8000 - 1;                // „астота 8ћ√ц / 8к√ц = 1к√ц
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = (uint)(-1);
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    timer_init(TIMER1, &timer_initpara);

    timer_enable(TIMER1);
}


uint Timer::TimeMS()
{
    return timer_counter_read(TIMER1);
}
