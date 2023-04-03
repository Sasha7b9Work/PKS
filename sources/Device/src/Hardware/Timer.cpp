// 2022/6/10 9:08:19 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


namespace Timer
{
}


void Timer::Init()
{

}


uint Timer::CurrentTime()
{
    return 0;
}


void Timer::Delay(uint)
{
}


TimeMeterMS::TimeMeterMS()
{
    Reset();
}


void TimeMeterMS::Reset()
{
    time_reset = TIME_MS;
    time_pause = 0;
}


void TimeMeterMS::WaitFor(uint ms)
{
    while (ElapsedMS() < ms)
    {
    }
}


void TimeMeterMS::Pause()
{
    time_pause = TIME_MS;
}


void TimeMeterMS::Continue()
{
    time_reset += (TIME_MS - time_pause);
}


uint TimeMeterMS::ElapsedMS()
{
    return TIME_MS - time_reset;
}


void TimeMeterUS::Reset()
{
}


void TimeMeterUS::WaitFor(uint us)
{
    while (ElapsedUS() < us)
    {
    }
}


uint TimeMeterUS::ElapsedUS()
{
    return 0;
}


void TimeMeterTics::Reset()
{
}


void TimeMeterTics::WaitFor(uint)
{
}
