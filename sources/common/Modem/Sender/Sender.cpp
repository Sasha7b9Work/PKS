// 2023/05/29 15:29:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Sender.h"
#include "Modem/Sender/Counter.h"
#include "Modem/MQTT.h"


namespace Sender
{
    static bool versionSW = false;
}


void Sender::Reset()
{
    Counter::Reset();
}


bool Sender::SendToSIM800()
{
    if (!versionSW)
    {
        MQTT::Packet::Publish("/versionSW", VERSION);

        versionSW = true;

        return true;
    }

    return false;
}
