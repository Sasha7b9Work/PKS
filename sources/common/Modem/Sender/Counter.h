// 2023/05/25 21:46:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
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

                MQTT::PublishPacket("/counter", buffer);

                meter.SetResponseTime(5000);
            }
        }
    }
}
