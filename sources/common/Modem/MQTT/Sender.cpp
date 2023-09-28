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
}


bool Sender::SendCounter(int counter)
{
    last_received = 0;

    if (!MQTT::InStateRunning())
    {
        return false;
    }

    Request::Set("base/state/counter", counter);

    TimeMeterMS meter;

    SIM800::Transmit::With0D("AT+CIPSEND");

    while (last_received != '>')
    {
        if (meter.ElapsedTime() > 20)
        {
            return false;
        }
    }

    Request::Send();
    Request::Clear();

    return true;
}


bool Sender::SendMeasures(const Measurements &meas)
{
    last_received = 0;

    if (!MQTT::InStateRunning())
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

    {
        MQTT::Packet::Publish("/base/state/gp1", meas.flags.GetGP(Phase::A) ? "1" : "0");
        MQTT::Packet::Publish("/base/state/gp2", meas.flags.GetGP(Phase::B) ? "1" : "0");
        MQTT::Packet::Publish("/base/state/gp3", meas.flags.GetGP(Phase::C) ? "1" : "0");
    }

    {
        for (int phase = Phase::A; phase < Phase::Count; phase++)
        {
            for (int i = 0; i < 9; i++)
            {
                char topic[32] = { '\0' };

                static const pchar names[Phase::Count] = { "A", "B", "C" };

                std::sprintf(topic, "base/cont/KM%s%d", names[phase], i + 1);

                LOG_WRITE("KM %d %d = %d", phase, i, meas.flags.GetKM((Phase::E)phase, i));

                MQTT::Packet::Publish(topic, meas.flags.GetKM((Phase::E)phase, i));
            }
        }

        MQTT::Packet::Publish("/base/state/dc100v", meas.flags.Get100V() ? "1" : "0");
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
