// 2023/05/26 11:43:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Measure.h"
#include "Hardware/Timer.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    namespace Measure
    {
        static FullMeasure value;
        static bool need = false;
        static TimeMeterMS meter;

        void Send(const FullMeasure &meas)
        {
            if (!meter.IsFinished())
            {
                return;
            }

            meter.SetResponseTime(60000);

            value = meas;

            need = true;
        }

        static void SendMeasure(pchar name, float voltage)
        {
            char buffer[32];
            std::sprintf(buffer, "%d", (int)(voltage + 0.5f));
            for (uint i = 0; i < std::strlen(buffer); i++)
            {
                if (buffer[i] == ',')
                {
                    buffer[i] = '.';
                }
            }
            MQTT::Packet::Publish(name, buffer);
        }

        bool SendToSIM800()
        {
            bool result = need;

            if (need)
            {
                if (value.is_good[0])
                {
                    SendMeasure("base/state/voltage_a", value.measures[0].voltage);
                    SendMeasure("base/state/current_a", value.measures[0].current * 1000.0f);
                }

                if (value.is_good[1])
                {
                    SendMeasure("base/state/voltage_b", value.measures[1].voltage);
                    SendMeasure("base/state/current_b", value.measures[1].current * 1000.0f);
                }

                if (value.is_good[2])
                {
                    SendMeasure("base/state/voltage_c", value.measures[2].voltage);
                    SendMeasure("base/state/current_c", value.measures[2].current * 1000.0f);
                }

                need = false;
            }

            return result;
        }
    }
}
