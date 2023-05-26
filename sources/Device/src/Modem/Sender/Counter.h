// 2023/05/25 21:46:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include <cstdio>


namespace Sender
{
    namespace Counter
    {
        int value = 0;
        bool need = false;
        TimeMeterMS meter;

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
                std::sprintf(buffer, "%d", value++);

                MQTT::PublishPacket("/counter", buffer);

                meter.SetResponseTime(5000);
            }
        }
    }
}
