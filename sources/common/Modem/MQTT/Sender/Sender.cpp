// 2023/05/29 15:29:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Sender.h"
#include "Modem/MQTT/Sender/Counter.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    static bool versionSW_is_sended = false;

    static TimeMeterMS meter;
}


void Sender::Reset()
{
    Counter::Reset();

    StringState::Reset();

    versionSW_is_sended = false;

    meter.SetResponseTime(5000);
}


bool Sender::SendToSIM800()
{
    static TimeMeterMS _meter;

    if (!versionSW_is_sended)
    {
        if (_meter.ElapsedTime() > 3000)
        {
            char buffer[32];

            std::sprintf(buffer, "%d : %dst", VERSION, NUM_STEPS);

            MQTT::Packet::Publish("/versionSW", buffer);
            versionSW_is_sended = true;

            MQTT::Packet::Publish("base/id", HAL::GetUID(buffer));

            return true;
        }
    }

    return false;
}


void Sender::ResetMeter()
{
    meter.SetResponseTime(0);
}


bool Sender::SendAll(pchar answer)
{
    if (std::strcmp(answer, ">") == 0)
    {
        bool sending = false;

        if (Sender::SendToSIM800())
        {
            sending = true;
        }

        if (Sender::StringState::SendToSIM800())
        {
            sending = true;
        }

        if (Sender::Measure::SendToSIM800())
        {
            sending = true;
        }

        if (Sender::LevelContactors::SendToSIM800())
        {
            sending = true;
        }

        if (Sender::ContactorsIsOK::SendToSIM800())
        {
            sending = true;
        }

        if (Sender::GP::SendToSIM800())
        {
            sending = true;
        }

        if (sending)
        {
            Sender::Counter::SendToSIM800();
        }

//            if (need_ping)
//            {
//                SIM800::Transmit::UINT8(0xC0);
//                SIM800::Transmit::UINT8(0x00);
//                need_ping = false;
//            }

        SIM800::Transmit::UINT8(sending ? (uint8)0x1A : (uint8)0x1B);

        return true;
    }
    else
    {
        if (meter.IsFinished())
        {
            SIM800::Transmit::With0D("AT+CIPSEND");

            meter.SetResponseTime(10000);
        }
    }

    return false;
}
