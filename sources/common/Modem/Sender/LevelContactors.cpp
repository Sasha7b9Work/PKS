// 2023/05/26 11:56:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/LevelContactors.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT.h"
#include <cstdio>


namespace Sender
{
    namespace LevelContactors
    {
        static int value[Phase::Count] = { -10, -9, -8 };
        static bool need[Phase::Count] = { true, true, true };

        void Send(const int level[Phase::Count])
        {
            for (int i = 0; i < Phase::Count; i++)
            {
                if (level[i] != value[i])
                {
                    value[i] = level[i];
                    need[i] = true;
                }
            }

            static TimeMeterMS meter;

            if ((need[0] || need[1] || need[2]) && meter.IsFinished())
            {
                MQTT::Request::Send();

                meter.SetResponseTime(500);
            }
        }

        void OnEventSend()
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
                    MQTT::Packet::Publish(buffer_name, buffer_value);
                }
            }
        }
    }
}

