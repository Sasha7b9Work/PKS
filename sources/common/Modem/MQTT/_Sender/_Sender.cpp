// 2023/05/29 15:29:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/_Sender/_Sender.h"
#include "Modem/MQTT/MQTT.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Settings/Settings.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    static TimeMeterMS meter;

    // По этому таймеру будем посылать пинги
    static TimeMeterMS meterPing;

    // Если true - нужно посылать пинг
    static bool need_ping = false;

    bool SendAll(pchar);
}


void Sender::Reset()
{
    StringState::Reset();

    meter.SetResponseTime(0);
}


void Sender::ResetMeter()
{
    meter.SetResponseTime(0);
}


void Sender::Update(pchar )
{

}


bool Sender::SendAll(pchar answer)
{
    if (meterPing.ElapsedTime() > 20000)
    {
        meterPing.Reset();
        need_ping = true;
    }

    if (std::strcmp(answer, ">") == 0)
    {
        bool sending = false;

        if (need_ping)
        {
            need_ping = false;
            SIM800::Transmit::UINT8(0xC0);
            SIM800::Transmit::UINT8(0x00);
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
        if (Sender::StateContactors::SendToSIM800())
        {
            sending = true;
        }

        SIM800::Transmit::UINT8(sending ? (uint8)0x1A : (uint8)0x1B);

        if (sending)
        {
            LOG_WRITE("Send %d ms after connect", Timer::TimeMS() - MQTT::TimeConnect());

            SIM800::Transmit::With0D("AT+CIPSEND");
        }

        return true;
    }
    else
    {
        if (meter.IsFinished())
        {
            SIM800::Transmit::With0D("AT+CIPSEND");

            meter.SetResponseTime(1000);
        }
    }

    return false;
}
