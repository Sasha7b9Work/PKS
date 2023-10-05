// 2023/09/28 08:56:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/SIM800.h"
#include "Hardware/Timer.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <cstdio>


namespace MQTT
{
    void ToStateConfirm();
}


namespace Sender
{
    namespace Request
    {
        static char topic[64] = { '\0' };
        static char message[32] = { '\0' };

        void Clear()
        {
            topic[0] = '\0';
            message[0] = '\0';
        }

        bool IsEmpty()
        {
            return topic[0] == '\0';
        }

        void Set(pchar _topic, int value)
        {
            std::strcpy(topic, _topic);
            std::sprintf(message, "%d", value);
        }

        void SendFinalSequence(bool not_empty)
        {
            SIM800::Transmit::UINT8(not_empty ? (uint8)0x1A : (uint8)0x1B);
        }

        void Send()
        {
            if (topic[0])
            {
                MQTT::Packet::Publish(topic, message);
            }

            SendFinalSequence(topic[0] != '\0');
        }
    }

    static char last_received = 0;          // Последний принятый символ

    void CallbackOnReceiveChar(char symbol)
    {
        last_received = symbol;
    }


    static bool gp[Phase::Count] = { false, false, false };
    static bool need_gp[Phase::Count] = { true, true, true };

    static int states[Phase::Count][9];
    static bool need_states[Phase::Count][9];

    static bool all_states = true;
    static bool need_all_states = true;

    static int levels[Phase::Count] = { 0, 0, 0 };
    static bool need_levels[Phase::Count] = { true, true, true };

    void Reset()
    {
        for (int i = 0; i < Phase::Count; i++)
        {
            gp[i] = false;
            need_gp[i] = true;

            for (int km = 0; km < 9; km++)
            {
                states[i][km] = 2;
                need_states[i][km] = true;
            }

            levels[i] = 0;
            need_levels[i] = true;
        }

        all_states = true;
        need_all_states = true;
    }
}


bool Sender::SendMeasures(const Measurements &meas)
{
    last_received = 0;

    if (!MQTT::InStateWaitData())
    {
        return false;
    }

    TimeMeterMS meter;

    SIM800::Transmit::With0D("AT+CIPSEND");

    while (last_received != '>')
    {
        if (meter.ElapsedTime() > 20)
        {
            return false;
        }
    }

    MQTT::ToStateConfirm();

    MQTT::Packet::Publish("base/state/counter", (int)meas.counter);

    {
        for (int phase = Phase::A; phase < Phase::Count; phase++)
        {
            bool new_gp = meas.flags.GetGP(Phase::A);

            if (need_gp[phase] || new_gp != gp[phase])
            {
                need_gp[phase] = false;

                gp[phase] = new_gp;

                char topic[32];
                std::sprintf(topic, "/base/state/gp%d", phase + 1);

                MQTT::Packet::Publish(topic, new_gp ? "1" : "0");
            }
        }
    }

    {
        bool good = true;       // Признак того, что исправны все контакторы

        for (int phase = Phase::A; phase < Phase::Count; phase++)
        {
            for (int i = 0; i < 8; i++)
            {
                char topic[32] = { '\0' };

                static const pchar names[Phase::Count] = { "A", "B", "C" };

                std::sprintf(topic, "base/cont/KM%s%d", names[phase], i + 1);

                int state = meas.flags.GetKM((Phase::E)phase, i);

                if (need_states[phase][i] || state != states[phase][i])
                {
                    need_states[phase][i] = false;

                    states[phase][i] = state;

                    MQTT::Packet::Publish(topic, state);
                }

                if (state == -1)
                {
                    good = false;
                }
            }
        }

        MQTT::Packet::Publish("/base/state/dc100v", meas.flags.Get100V() ? "1" : "0");

        if (need_all_states || good != all_states)
        {
            need_all_states = false;

            all_states = good;

            MQTT::Packet::Publish("/base/state/state_contactors", good ? "1" : "0");
        }
    }

    {
        static const char *const names[Phase::Count] = { "A", "B", "C" };

        for (int i = 0; i < Phase::Count; i++)
        {
            char topic[32];

            std::sprintf(topic, "base/cont/level%s", names[i]);

            int level = -meas.flags.GetLevelRele((Phase::E)i);

            if (need_levels[i] || level != levels[i])
            {
                need_levels[i] = false;
                levels[i] = level;

                MQTT::Packet::Publish(topic, level);
            }
        }
    }

    {
        FullMeasure value = meas.GetFullMeasure();

        MQTT::Packet::PublishF("base/state/voltage_a", value.measures[0].voltage);
        MQTT::Packet::PublishF("base/state/current_a", value.measures[0].current);
        MQTT::Packet::PublishF("/power_a", value.measures[0].GetPower() / 1e3f);

        MQTT::Packet::PublishF("base/state/voltage_b", value.measures[1].voltage);
        MQTT::Packet::PublishF("base/state/current_b", value.measures[1].current);
        MQTT::Packet::PublishF("/power_b", value.measures[1].GetPower() / 1e3f);

        MQTT::Packet::PublishF("base/state/voltage_c", value.measures[2].voltage);
        MQTT::Packet::PublishF("base/state/current_c", value.measures[2].current);
        MQTT::Packet::PublishF("/power_c", value.measures[2].GetPower() / 1e3f);
    }

    Request::SendFinalSequence(true);

    Request::Clear();

    return true;
}


bool Sender::SendVersion()
{
    last_received = 0;

    if (!MQTT::InStateSendVersion())
    {
        return false;
    }

    char buffer[32];

    std::sprintf(buffer, "v%d:%d:%d", VERSION, gset.GetNumberSteps(), gset.GetKoeffCurrent());

    TimeMeterMS meter;

    SIM800::Transmit::With0D("AT+CIPSEND");

    while (last_received != '>')
    {
        if (meter.ElapsedTime() > 20)
        {
            return false;
        }
    }

    MQTT::Packet::Publish("/versionSW", buffer);

    MQTT::Packet::Publish("base/id", HAL::GetUID(buffer));

    MQTT::Packet::Publish("/last/reset", "-");

    if (_GET_BIT(GL::_RCU_RSTSCK, 28))
    {
        MQTT::Packet::Publish("/last/reset", "Software");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 31))
    {
        MQTT::Packet::Publish("/last/reset", "Low power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 30))
    {
        MQTT::Packet::Publish("/last/reset", "Watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 29))
    {
        MQTT::Packet::Publish("/last/reset", "Free watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 27))
    {
        MQTT::Packet::Publish("/last/reset", "Power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 26))
    {
        MQTT::Packet::Publish("/last/reset", "External pin");
    }

    Request::SendFinalSequence(true);

    Request::Clear();

    return true;
}
