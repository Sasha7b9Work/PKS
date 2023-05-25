// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
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

    // пакет на публикацию
    static void PublishPacket(const char *MQTT_topic, const char *MQTT_messege);

    void Reset();

    void CallbackOnReceiveData(pchar);

    namespace Send
    {
        //------------------------------------------------------------------------- counter
        static int counter = 0;
        static bool need_counter = false;
        static TimeMeterMS meter_counter;

        //------------------------------------------------------------------------
        void Measure(const FullMeasure &);
        static FullMeasure measure;
        static bool need_measure = false;           // Если true - надо передавать измерение

        //--------------------------------------------------------------------------
        void GP(int num, bool state);
        static bool gp[3] = { false, false, false };
        static bool need_gp[3] = { false, false, false };

        //--------------------------------------------------------------------------
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

        //------------------------------------------------------------------------------
        void LevelContactors(int[Phase::Count]);
        static int level_contactor[Phase::Count] = { 0, 0, 0 };
        static bool need_level_contactor[Phase::Count] = { true, true, true };

        // Сбрасывается каждый раз при поступлении данынх
        static TimeMeterMS meterLastData;

        static void Measure(pchar name, float value);

        static void SendRequest()
        {
            if (state == State::RUNNING)
            {
                SIM800::Transmit("AT+CIPSEND");
            }
        }

        static void SendAllToMQTT();
    }

    // Присоединён к серверу MQTT
    bool IsConnected();
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

        if (strcmp(answer, ">") == 0)
        {
            Send::SendAllToMQTT();
        }

        if (Send::meter_counter.IsFinished())
        {
            Send::need_counter = true;
            Send::SendRequest();
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

    static TimeMeterMS meter;

    static bool first = true;

    if (meter.ElapsedTime() < 60000 && !first)
    {
        return;
    }

    first = false;

    meter.Reset();

    measure = meas;

    need_measure = true;

    SendRequest();
}


void MQTT::Send::LevelContactors(int level[Phase::Count])
{
    bool need_request = false;

    for (int i = 0; i < Phase::Count; i++)
    {
        if (level[i] != Send::level_contactor[i])
        {
            Send::level_contactor[i] = level[i];
            Send::need_level_contactor[i] = true;
            need_request = true;
        }
    }

    if (need_request && state == State::RUNNING)
    {
        SendRequest();
    }
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


void MQTT::CallbackOnReceiveData(pchar)
{
    Send::meterLastData.Reset();
}


void MQTT::Send::SendAllToMQTT()
{
    {
        static const char *const names[Phase::Count] = { "A", "B", "C" };

        char buffer_name[32];
        char buffer_value[32];

        for (int i = 0; i < Phase::Count; i++)
        {
            if (Send::need_level_contactor[i])
            {
                Send::need_level_contactor[i] = false;
                std::sprintf(buffer_name, "/base/cont/level%s", names[i]);
                std::sprintf(buffer_value, "%d", -Send::level_contactor[i]);
                PublishPacket(buffer_name, buffer_value);
            }
        }
    }
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
    if (Send::need_gp[0] || Send::need_gp[1] || Send::need_gp[2])
    {
        char name[20] = "/gp/_0";

        for (int i = 0; i < 3; i++)
        {
            if (Send::need_gp[i])
            {
                name[6] = (char)((i + 1) | 0x30);

                PublishPacket(name, Send::gp[i] ? "1" : "0");

                Send::need_gp[i] = false;
            }
        }
    }
    if (Send::need_counter)
    {
        char buffer[32];
        std::sprintf(buffer, "%d", counter++);

        PublishPacket("/counter", buffer);

        meter_counter.SetResponseTime(1000);
    }
    if (Send::need_measure)
    {
        if (Send::measure.is_good[0])
        {
            Send::Measure("/voltage/a", Send::measure.measures[0].voltage);
            Send::Measure("/current/a", Send::measure.measures[0].current * 1000.0f);
        }

        if (Send::measure.is_good[1])
        {
            Send::Measure("/voltage/b", Send::measure.measures[1].voltage);
            Send::Measure("/current/b", Send::measure.measures[1].current * 1000.0f);
        }

        if (Send::measure.is_good[2])
        {
            Send::Measure("/voltage/c", Send::measure.measures[2].voltage);
            Send::Measure("/current/c", Send::measure.measures[2].current * 1000.0f);
        }

        Send::need_measure = false;
    }
    if (need_ping)
    {
        SIM800::TransmitUINT8(0xC0);
        SIM800::TransmitUINT8(0x00);
        need_ping = false;
    }

    SIM800::TransmitUINT8(0x1A);
}
