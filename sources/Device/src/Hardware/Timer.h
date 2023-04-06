// 2023/04/05 16:54:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Timer
{
    void Init();

    uint TimeMS();

    void OnCallbackInterrupt();
}


struct TimeMeterMS
{
    TimeMeterMS()
    {
        Reset();
    }
    void Reset();
    uint ElapsedTime();
private:
    uint time_reset;
};
