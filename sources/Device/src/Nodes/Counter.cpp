// 2023/09/29 08:07:17 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Nodes/Counter.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/Sender.h"
#include "Modem/Modem.h"


namespace Counter
{
    static int counter = 0;
    static uint prev_time = 0;
    static uint prev_time_reset = 0;

    void Reset()
    {
        counter = 0;
        prev_time = 0;
    }

    void Update()
    {
        if ((TIME_MS / 1000) > prev_time)
        {
            prev_time = TIME_MS / 1000;

            if (Sender::SendCounter(counter))
            {
                counter++;
            }
        }

        if (TIME_MS - prev_time_reset > 120000)
        {
            prev_time_reset = TIME_MS;

            Modem::Reset();
        }
    }
}

