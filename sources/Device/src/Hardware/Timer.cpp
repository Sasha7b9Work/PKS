// 2023/04/05 16:54:23 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include "Hardware/HAL/systick.h"


uint timer_counter = 0;


uint Timer::TimeMS()
{
    return timer_counter;
}


void TimeMeterMS::Reset()
{
    time_reset = timer_counter;
}


uint TimeMeterMS::ElapsedTime()
{
    return timer_counter - time_reset;
}


void Timer::DelayMS(uint ms)
{
    delay_1ms(ms);
}
