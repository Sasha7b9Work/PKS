// 2023/05/26 12:02:52 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Counter.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT.h"
#include <cstdio>


namespace Sender
{
    namespace Counter
    {
        static int value = -1;

        void Reset()
        {
            value = 0;
        }

        bool SendToSIM800()
        {
            MQTT::Packet::Publish("base/state/counter", value++);

            return true;
        }
    }
}
