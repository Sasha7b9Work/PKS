// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/Sender/Counter.h"
#include "Modem/Sender/Measure.h"
#include "Modem/Sender/LevelContactors.h"
#include "Modem/Sender/GP.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace SIM800
{
    uint Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitRAW(pchar);
    void Reset();
}


namespace MQTT
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // Ждеём приглашения ">"
            RUNNING
        };
    };

    static State::E state = State::IDLE;

    static TimeMeterMS meterPing;

    // Если nned_ping == true, то посылаем команду пинга
    static bool need_ping = false;

    void Update(pchar);

    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // уникальное имя устройства в сети MQTT

    void Reset();

    void CallbackOnReceiveData(pchar);

    namespace Send
    {
        void StateContactors(const bool[NUM_PINS_MX]);
        static bool state_contactors[NUM_PINS_MX] =              // Состояние каждого контактора
        {
            false, false, false, false, false, false, false, false, false,
            false, false, false, false, false, false, false, false, false,
            false, false, false, false, false, false, false, false, false,
            false
        };
        static bool need_send_state_contactors[NUM_PINS_MX] =    // true, если нужно передавать состояние конактора
        {
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true
        };

        // Сбрасывается каждый раз при поступлении данынх
        static TimeMeterMS meterLastData;

        static void SendAllToMQTT();
    }

    // Присоединён к серверу MQTT
    bool IsConnected();
}


bool MQTT::InStateRunning()
{
    return state == State::RUNNING;
}


void MQTT::Update(pchar answer)
{
    TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        SIM800::Transmit("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (strcmp(answer, ">") == 0)
        {
            SIM800::TransmitUINT8(0x10);   // маркер пакета на установку соединения
            SIM800::TransmitUINT8(0x1c);
//            SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 14));

            // тип протокола
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_type));
            SIM800::TransmitRAW(MQTT_type);

            // просто так нужно
            SIM800::TransmitUINT8(0x04);    // версия протокола
            SIM800::TransmitUINT8(0x02);    // connect flag
            SIM800::TransmitUINT8(0x00);    // \ keep alive 
            SIM800::TransmitUINT8(0x3c);    // /

            SIM800::TransmitUINT8(0x00);    // property lenth
            SIM800::TransmitUINT8(0x10);    // 
            SIM800::TransmitRAW(MQTT_CID);

            SIM800::TransmitUINT8(0x1A);

            state = State::RUNNING;

            Send::meterLastData.Reset();

            meterPing.Reset();

            HAL_PINS::SendState();
        }
        break;

    case State::RUNNING:

        if (Send::meterLastData.ElapsedTime() > 30000)
        {
//            SIM800::Reset();
        }

        if (meterPing.ElapsedTime() > 20000)
        {
            meterPing.Reset();
//            SIM800::Transmit("AT+CIPSEND");
//            need_ping = true;
        }

        if (strcmp(answer, ">") == 0)
        {
            Send::SendAllToMQTT();
        }

//        Sender::Counter::OnStateRunning();

        break;
    }
}


bool MQTT::IsConnected()
{
    return (state == State::RUNNING);
}


void MQTT::Send::StateContactors(const bool st_contactors[NUM_PINS_MX])
{
    static bool need_request = false;

    if (state == State::RUNNING)
    {
        need_request = false;
    }

    for (int i = 0; i < NUM_PINS_MX; i++)
    {
        bool new_state = st_contactors[i];

        if (new_state != state_contactors[i])
        {
            state_contactors[i] = new_state;
            need_send_state_contactors[i] = true;
            need_request = true;
        }
    }

    if (state != State::RUNNING)
    {
        return;
    }

    if (need_request)
    {
        if (state == State::RUNNING)
        {
            static TimeMeterMS meter;
            if (meter.ElapsedTime() > 1000)
            {
                meter.Reset();
                SendRequest();
            }
        }
    }
}


void MQTT::Reset()
{
    state = State::IDLE;
}


void  MQTT::PublishPacket(const char *MQTT_topic, const char *MQTT_messege)
{
    SIM800::TransmitUINT8(0x30);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::TransmitRAW(MQTT_topic);
    SIM800::TransmitRAW(MQTT_messege);
}


void MQTT::CallbackOnReceiveData(pchar)
{
    Send::meterLastData.Reset();
}


void MQTT::Send::SendAllToMQTT()
{
    Sender::LevelContactors::OnEventSend();

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
            if (Send::need_send_state_contactors[i])
            {
                if (i == 27)
                {
                    PublishPacket("base/state/dc100v", Send::state_contactors[i] ? "1" : "0");
                    sended = true;
                }
                else
                {
                    std::sprintf(buffer, "/cont/KM%s", names[i]);
                    PublishPacket(buffer, Send::state_contactors[i] ? "1" : "0");
                    sended = true;
                }
                Send::need_send_state_contactors[i] = false;
            }
            if (!Send::state_contactors[i])
            {
                all_is_ok = false;
            }
        }
        if (sended)
        {
            PublishPacket("/base/state/state_contactors", all_is_ok ? "1" : "0");
        }
    }

    Sender::GP::OnEventSend();

//    Sender::Counter::OnEventSend();

    Sender::Measure::OnEventSend();

    if (need_ping)
    {
        SIM800::TransmitUINT8(0xC0);
        SIM800::TransmitUINT8(0x00);
        need_ping = false;
    }

    SIM800::TransmitUINT8(0x1A);
}


void MQTT::Send::SendRequest()
{
    if (state == State::RUNNING)
    {
        SIM800::Transmit("AT+CIPSEND");
    }
}
