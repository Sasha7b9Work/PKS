// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


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

    void Update(const String &);

    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // уникальное имя устройства в сети MQTT

    // пакет на публикацию
    static void PublishPacket(const char *MQTT_topic, const char *MQTT_messege);

    void Reset();

    void CallbackOnReceiveData();

    namespace Send
    {
        // Сбрасывается каждый раз при поступлении данынх
        static TimeMeterMS meterLastData;
        static void Measure(pchar name, float value);
        void Measure(const FullMeasure &);
        void GP(int num, bool state);
        void Contactors(const bool[27]);
        static bool gp[3] = { false, false, false };
        static bool need_gp[3] = { false, false, false };
        // Если true - надо передавать измерение
        static bool need_measure = false;
        static FullMeasure measure;
        static bool state_contactors[27];               // Состояние каждого контактора
        static bool need_send_state_contactors[27] =    // true, если нужно передавать состояние конактора
        {
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true,
            true, true, true, true, true, true, true, true, true
        };
        static bool all_connectos_ok = true;            // false, если хоть один контактор неисправен
        static bool need_send_all_contactors = true;    // true, если нужно передавать all_connectos_ok

        static void SendRequest()
        {
            SIM800::Transmit("AT+CIPSEND");
        }
    }

    // Присоединён к серверу MQTT
    bool IsConnected();
}


void MQTT::Update(const String &answer)
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
        if (answer == ">")
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

//            // MQTT логин
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_user));
//            SIM800::TransmitRAW(MQTT_user);
//
//            // MQTT пароль
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_pass));
//            SIM800::TransmitRAW(MQTT_pass);

//            PublishPacket("base/state/voltage_c", "12.24");

// маркер завершения пакета
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

        if (answer == ">")
        {
            {
                if (Send::need_send_all_contactors)
                {
                    PublishPacket("/base/state/state_contactors", Send::all_connectos_ok ? "1" : "0");
                    Send::need_send_all_contactors = false;
                }

                static const char *const names[27] =
                {
                    "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9",
                    "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
                    "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9"
                };

                char buffer[100];

                for (int i = 0; i < 27; i++)
                {
                    if (Send::need_send_state_contactors[i])
                    {
                        std::sprintf(buffer, "/base/cont/KM%s", names[i]);
                        PublishPacket(buffer, Send::state_contactors[i] ? "1" : "0");
                        Send::need_send_state_contactors[i] = false;
                    }
                }
            }
            if (Send::need_gp[0] || Send::need_gp[1] || Send::need_gp[2])
            {
                char name[20] = "base/state/gp0";

                for (int i = 0; i < 3; i++)
                {
                    if (Send::need_gp[i])
                    {
                        name[13] = (char)((i + 1) | 0x30);

                        PublishPacket(name, Send::gp[i] ? "1" : "0");

                        Send::need_gp[i] = false;
                    }
                }
            }
            if (Send::need_measure)
            {
                static int counter = 0;

                char buffer[32];
                std::sprintf(buffer, "%d", counter++);

                PublishPacket("base/state/counter", buffer);

                if (!Send::measure.is_bad[0])
                {
                    Send::Measure("base/state/voltage_a", Send::measure.measures[0].voltage);
                    Send::Measure("base/state/current_a", Send::measure.measures[0].current * 1000.0f);
                }

                if (!Send::measure.is_bad[1])
                {
                    Send::Measure("base/state/voltage_b", Send::measure.measures[1].voltage);
                    Send::Measure("base/state/current_b", Send::measure.measures[1].current * 1000.0f);
                }

                if (!Send::measure.is_bad[2])
                {
                    Send::Measure("base/state/voltage_c", Send::measure.measures[2].voltage);
                    Send::Measure("base/state/current_c", Send::measure.measures[2].current * 1000.0f);
                }

                Send::need_measure = false;
            }
            if(need_ping)
            {
                SIM800::TransmitUINT8(0xC0);
                SIM800::TransmitUINT8(0x00);
                need_ping = false;
            }

            SIM800::TransmitUINT8(0x1A);
        }

        break;
    }
}


bool MQTT::IsConnected()
{
    return (state == State::RUNNING);
}


void MQTT::Send::Measure(pchar name, float value)
{
    char buffer[32];
    std::sprintf(buffer, "%d", (int)(value + 0.5f));
    for (uint i = 0; i < std::strlen(buffer); i++)
    {
        if (buffer[i] == ',')
        {
            buffer[i] = '.';
        }
    }
    PublishPacket(name, buffer);
}


void MQTT::Send::Measure(const FullMeasure &meas)
{
    if (state != State::RUNNING)
    {
        return;
    }

    static TimeMeterMS meterLastMeasure;

    static bool first = true;

    if (meterLastMeasure.ElapsedTime() < 1000 && !first)
    {
        return;
    }

    first = false;

    meterLastMeasure.Reset();

    measure = meas;

    need_measure = true;

    SendRequest();
}


void MQTT::Send::Contactors(const bool st_contactors[27])
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 5000)
    {
        return;
    }

    meter.Reset();

    bool need_request = false;

    bool connectos_ok = true;

    for (int i = 0; i < 27; i++)
    {
        bool new_state = st_contactors[i];

        if (i == 0)
        {
            new_state = false;
        }

        if (new_state != state_contactors[i])
        {
            state_contactors[i] = new_state;
            need_send_state_contactors[i] = true;
            need_request = true;
        }

        if (!state_contactors[i])
        {
            connectos_ok = false;
        }
    }

    if (need_request)
    {
        all_connectos_ok = connectos_ok;

        need_send_all_contactors = true;

        if (state == State::RUNNING)
        {
            SendRequest();
        }
    }
}


void MQTT::Send::GP(int num, bool is_low)
{
    if (state != State::RUNNING)
    {
        return;
    }

    gp[num - 1] = is_low;

    bool need_request = !need_gp[0] && !need_gp[1] && !need_gp[2];

    need_gp[num - 1] = true;

    if (need_request)
    {
        SendRequest();
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


void MQTT::CallbackOnReceiveData()
{
    Send::meterLastData.Reset();
}
