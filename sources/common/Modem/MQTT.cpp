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
#include "Modem/Sender/ContactorsOK.h"
#include "Modem/Sender/GP.h"
#include "Modem/SIM800.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    namespace Packet
    {
        static int counter = 0;

        void Reset()
        {
            counter = 0;
        }

        int Count()
        {
            return counter;
        }
    }

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

    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // уникальное имя устройства в сети MQTT

    // Сбрасывается каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;
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

            meterLastData.Reset();

            meterPing.Reset();

            HAL_PINS::SendState();
        }
        break;

    case State::RUNNING:

        if (strcmp(answer, ">") == 0)
        {
            Sender::Counter::OnEventSend();

//            Sender::LevelContactors::OnEventSend();
//
//            Sender::ContactorsIsOK::OnEventSend();
//
//            Sender::GP::OnEventSend();
//
//            Sender::Measure::OnEventSend();
//
//            if (need_ping)
//            {
//                SIM800::TransmitUINT8(0xC0);
//                SIM800::TransmitUINT8(0x00);
//                need_ping = false;
//            }

            if (Packet::Count())
            {
                SIM800::TransmitUINT8(0x1A);
            }
            else
            {
                SIM800::TransmitUINT8(0x1B);        // ESC - отказ
            }
        }

        Sender::Counter::OnStateRunning();

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

        break;
    }
}


void MQTT::Reset()
{
    state = State::IDLE;
}


void  MQTT::Packet::Publish(const char *MQTT_topic, const char *MQTT_messege)
{
    SIM800::TransmitUINT8(0x30);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::TransmitRAW(MQTT_topic);
    SIM800::TransmitRAW(MQTT_messege);

    counter++;
}


void MQTT::CallbackOnReceiveData(pchar)
{
    meterLastData.Reset();
}


void MQTT::Send::SendRequest()
{
    if (state == State::RUNNING)
    {
        SIM800::Transmit("AT+CIPSEND");
    }
}
