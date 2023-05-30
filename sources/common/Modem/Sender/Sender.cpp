// 2023/05/29 15:29:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Sender.h"
#include "Modem/Sender/Counter.h"
#include "Modem/MQTT.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cstdio>


namespace Sender
{
    static bool versionSW_is_sended = false;
}


void Sender::Reset()
{
    Counter::Reset();

    versionSW_is_sended = false;
}


bool Sender::SendToSIM800()
{
    static TimeMeterMS meter;

    if (!versionSW_is_sended)
    {
        if (meter.ElapsedTime() > 3000)
        {
            MQTT::Packet::Publish("/versionSW", VERSION);
            versionSW_is_sended = true;

            MQTT::Packet::Publish("base/id", (int)HAL::GetUID());

            return true;
        }
    }

    if (meter.ElapsedTime() > 60000)
    {
        meter.Reset();

        MQTT::Packet::Publish("/test/string", "test");

        return true;
    }

    return false;
}
