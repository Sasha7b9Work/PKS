// 2023/05/26 08:22:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT.h"
#include <cstdio>


namespace Sender
{
    namespace LevelContactors
    {
        static int value[Phase::Count] = { -10, -9, -8 };
        static bool need[Phase::Count] = { true, true, true };

        inline void Send(int level[Phase::Count])
        {
            bool need_request = false;

            for (int i = 0; i < Phase::Count; i++)
            {
                if (level[i] != value[i])
                {
                    value[i] = level[i];
                    need[i] = true;
                    need_request = true;
                }
            }

            if (need_request)
            {
                MQTT::Send::SendRequest();
            }
        }

        inline void OnEventSend()
        {
            static const char *const names[Phase::Count] = { "A", "B", "C" };

            char buffer_name[32];
            char buffer_value[32];

            for (int i = 0; i < Phase::Count; i++)
            {
                if (need[i])
                {
                    need[i] = false;
                    std::sprintf(buffer_name, "/base/cont/level%s", names[i]);
                    std::sprintf(buffer_value, "%d", -value[i]);
                    MQTT::PublishPacket(buffer_name, buffer_value);
                }
            }
        }
    }
}
