// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>
#include <cstdlib>
#include <stdio.h>


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

    static FullMeasure measure;

    static bool gp[3] = { false, false, false };
    static bool need_gp[3] = { false, false, false };

    // Если true - надо передавать измерение
    static bool need_measure = false;

    void Update(const String &);

//    static const char MQTT_type[15] = "MQIsdp";             // тип протокола НЕ ТРОГАТЬ!
    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // уникальное имя устройства в сети MQTT
    static const char *MQTT_user = "";                   // api.cloudmqtt.com > Details > User
    static const char *MQTT_pass = "";                   // api.cloudmqtt.com > Details > Password

    // пакет на публикацию
    static void PublishPacket(const char *MQTT_topic, const char *MQTT_messege);

    void Reset();

    void CallbackOnReceiveData();

    // Сбрасывается каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;

    static void SendMeasure(pchar name, float value);

    void SendMeasure(const FullMeasure &);
    void SendGP(int num, bool state);
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

            meterLastData.Reset();

            meterPing.Reset();

            HAL_PINS::SendState();
        }
        break;

    case State::RUNNING:

        if (meterLastData.ElapsedTime() > 30000)
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
            if (need_gp[0] || need_gp[1] || need_gp[2])
            {
                char name[20] = "base/state/gp0";

                for (int i = 0; i < 3; i++)
                {
                    if (need_gp[i])
                    {
                        name[13] = (char)((i + 1) | 0x30);

                        PublishPacket(name, gp[i] ? "1" : "0");

                        need_gp[i] = false;
                    }
                }
            }
            if (need_measure)
            {
                static int counter = (int)std::rand();
                   ;

                char buffer[32];
                sprintf(buffer, "%d", counter++);

                PublishPacket("base/state/counter", buffer);

                SendMeasure("base/state/voltage_a", measure.measures[0].voltage);
                SendMeasure("base/state/voltage_b", measure.measures[1].voltage);
                SendMeasure("base/state/voltage_c", measure.measures[2].voltage);
                SendMeasure("base/state/current_a", measure.measures[0].current * 1000.0f);
                SendMeasure("base/state/current_b", measure.measures[1].current * 1000.0f);
                SendMeasure("base/state/current_c", measure.measures[2].current * 1000.0f);

                need_measure = false;
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


void MQTT::SendMeasure(pchar name, float value)
{
    char buffer[32];
    sprintf(buffer, "%d", (int)value);
    for (uint i = 0; i < std::strlen(buffer); i++)
    {
        if (buffer[i] == ',')
        {
            buffer[i] = '.';
        }
    }
    PublishPacket(name, buffer);
}


void MQTT::SendMeasure(const FullMeasure &meas)
{
    if (state != State::RUNNING)
    {
        return;
    }

    static TimeMeterMS meterLastMeasure;

    if (meterLastMeasure.ElapsedTime() < 5000)
    {
        return;
    }

    meterLastMeasure.Reset();

    measure = meas;

    need_measure = true;

    SIM800::Transmit("AT+CIPSEND");
}


void MQTT::SendGP(int num, bool is_low)
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
        SIM800::Transmit("AT+CIPSEND");
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
    meterLastData.Reset();
}
