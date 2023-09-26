// 2023/05/26 11:43:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Measure.h"
#include "Hardware/Timer.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    namespace Measure
    {
        static FullMeasure value;
        static bool need = false;

        void Send(const FullMeasure &meas)
        {
            value = meas;

            need = true;
        }

        bool SendToSIM800()
        {
            bool result = need;

            if (need)
            {
                if (value.is_good[0])
                {
                    MQTT::Packet::PublishF("base/state/voltage_a", value.measures[0].voltage);
                    MQTT::Packet::PublishF("base/state/current_a", value.measures[0].current);
                    MQTT::Packet::PublishF("/power_a", value.measures[0].GetPower() / 1e3f);
                }

                if (value.is_good[1])
                {
                    MQTT::Packet::PublishF("base/state/voltage_b", value.measures[1].voltage);
                    MQTT::Packet::PublishF("base/state/current_b", value.measures[1].current);
                    MQTT::Packet::PublishF("/power_b", value.measures[1].GetPower() / 1e3f);
                }

                if (value.is_good[2])
                {
                    MQTT::Packet::PublishF("base/state/voltage_c", value.measures[2].voltage);
                    MQTT::Packet::PublishF("base/state/current_c", value.measures[2].current);
                    MQTT::Packet::PublishF("/power_c", value.measures[2].GetPower() / 1e3f);
                }

                need = false;
            }

            return result;
        }
    }
}
