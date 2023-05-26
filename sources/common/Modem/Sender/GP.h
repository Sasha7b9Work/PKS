// 2023/05/26 08:35:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT.h"


namespace Sender
{
    namespace GP
    {
        static bool value[3] = { false, false, false };
        static bool need[3] = { false, false, false };

        inline void Send(int num, bool is_low)
        {
            if (!MQTT::InStateRunning())
            {
                return;
            }

            value[num - 1] = is_low;

            bool need_request = !need[0] && !need[1] && !need[2];

            need[num - 1] = true;

            if (need_request)
            {
                MQTT::Send::SendRequest();
            }
        }

        inline void OnEventSend()
        {
            if (need[0] || need[1] || need[2])
            {
                char name[20] = "/gp/_0";

                for (int i = 0; i < 3; i++)
                {
                    if (need[i])
                    {
                        name[6] = (char)((i + 1) | 0x30);

                        MQTT::Packet::Publish(name, value[i] ? "1" : "0");

                        need[i] = false;
                    }
                }
            }
        }
    }
}
