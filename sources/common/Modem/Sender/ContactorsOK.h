// 2023/05/26 09:08:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/MQTT.h"
#include "Hardware/Timer.h"


namespace Sender
{
    namespace ContactorsIsOK
    {
        static bool value[NUM_PINS_MX] =              // Состояние каждого контактора
        {
            false, false, false, false, false, false, false, false, false,
            false, false, false, false, false, false, false, false, false,
            false, false, false, false, false, false, false, false, false,
            false
        };
        static bool need[NUM_PINS_MX] =    // true, если нужно передавать состояние конактора
        {
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true
        };

        inline void Send(const bool state[NUM_PINS_MX])
        {
            bool need_request = false;

            for (int i = 0; i < NUM_PINS_MX; i++)
            {
                if (state[i] != value[i])
                {
                    value[i] = state[i];
                    need[i] = true;
                    need_request = true;
                }
            }

            if (need_request)
            {
                MQTT::Send::SendRequest();
            }
        }

        inline void OnEventSend()
        {
            static const char *const names[NUM_PINS_MX] =
            {
                "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9",
                "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
                "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9"
            };

            char buffer[100];

            bool sended = false;
            bool all_is_ok = true;

            for (int i = 0; i < NUM_PINS_MX; i++)
            {
                if (need[i])
                {
                    if (i == 27)
                    {
                        MQTT::Packet::Publish("base/state/dc100v", value[i] ? "1" : "0");
                        sended = true;
                    }
                    else
                    {
                        std::sprintf(buffer, "/cont/KM%s", names[i]);
                        MQTT::Packet::Publish(buffer, value[i] ? "1" : "0");
                        sended = true;
                    }
                    need[i] = false;
                }

                if (!value[i])
                {
                    all_is_ok = false;
                }
            }
            if (sended)
            {
                MQTT::Packet::Publish("/base/state/state_contactors", all_is_ok ? "1" : "0");
            }
        }
    }
}
