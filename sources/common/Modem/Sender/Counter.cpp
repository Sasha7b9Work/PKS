// 2023/05/26 12:02:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Counter.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT.h"
#include <cstdio>


namespace Sender
{
    namespace Counter
    {
        static int value = 0;
        static bool need = false;
        static TimeMeterMS meter;

        void Reset()
        {
            value = 0;
        }

        void OnStateRunning()
        {
            if (meter.IsFinished())
            {
                need = true;
                MQTT::Send::SendRequest();
            }
        }

        void OnEventSend()
        {
            if (need)
            {
                need = false;

                char buffer[32];
                std::sprintf(buffer, "%d", ++value);

                MQTT::Packet::Publish("/base/state/counter", buffer);

                meter.SetResponseTime(5000);
            }
        }
    }
}
