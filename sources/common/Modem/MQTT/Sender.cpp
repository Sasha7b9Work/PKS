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

    static float voltage[Phase::Count] = { 0.0f, 0.0f, 0.0f };
    static bool need_voltage[Phase::Count] = { true, true, true };

    static float current[Phase::Count] = { 0.0f, 0.0f, 0.0f };
    static bool need_current[Phase::Count] = { true, true, true };

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

    static int ToInt(float value)
    {
        return (int)(value * 10.0f + 0.5f);
    }

    static void SendPhase(Phase::E phase, const FullMeasure &value)
    {
        bool need_power = false;

        static const pchar letters[Phase::Count] = { "a", "b", "c" };

        char topic[32];

        if (need_voltage[phase] || ToInt(voltage[phase]) != ToInt(value.measures[phase].voltage))
        {
            need_power = true;
            voltage[phase] = value.measures[phase].voltage;
            std::sprintf(topic, "base/state/voltage_%s", letters[phase]);
            MQTT::Packet::PublishF(topic, value.measures[phase].voltage);
            need_voltage[phase] = false;
        }

        if (need_current[phase] || ToInt(current[phase]) != ToInt(value.measures[phase].current))
        {
            need_power = true;
            current[phase] = value.measures[phase].current;
            std::sprintf(topic, "base/state/current_%s", letters[phase]);
//            MQTT::Packet::PublishF(topic, value.measures[phase].current);
            need_current[phase] = false;
        }

        if (need_power)
        {
            std::sprintf(topic, "/power_%s", letters[phase]);
//            MQTT::Packet::PublishF(topic, value.measures[phase].GetPower() / 1e3f);
        }
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
    }
    {
        FullMeasure value = meas.GetFullMeasure();

        SendPhase(Phase::A, value);
        SendPhase(Phase::B, value);
        SendPhase(Phase::C, value);
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

    MQTT::Packet::Publish("base/id", HAL::GetUID());

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
