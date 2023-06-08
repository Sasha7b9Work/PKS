// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM800.h"
#include "Modem/MQTT/Sender/Sender.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // ∆деЄм приглашени€ ">"
            RUNNING
        };
    };

    static State::E state = State::IDLE;

    static bool need_reset = false;

    void Reset()
    {
        need_reset = true;
    }

    static TimeMeterMS meterPing;

    // —брасываетс€ каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;
}


bool MQTT::InStateRunning()
{
    return state == State::RUNNING;
}


void MQTT::Update(pchar answer)
{
    if (need_reset)
    {
        need_reset = false;

        state = State::IDLE;
    }

    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        LOG_WRITE("+++MQTT::IDLE+++");
        SIM800::Transmit::With0D("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (strcmp(answer, ">") == 0)
        {
            char *MQTT_type = "MQTT";

            char guid[32];
            char MQTT_CID[32];
            std::sprintf(MQTT_CID, "mqttpks-%s", HAL::GetUID(guid));

            SIM800::Transmit::UINT8(0x10);   // маркер пакета на установку соединени€
            SIM800::Transmit::UINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 8));

            // тип протокола
            SIM800::Transmit::UINT8(0x00);
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_type));
            SIM800::Transmit::RAW(MQTT_type);

            // просто так нужно
            SIM800::Transmit::UINT8(0x04);    // верси€ протокола
            SIM800::Transmit::UINT8(0x02);    // connect flag
            SIM800::Transmit::UINT8(0x00);    // \ keep alive 
            SIM800::Transmit::UINT8(0x3c);    // /

            SIM800::Transmit::UINT8(0x00);    // property lenth
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_CID));
            SIM800::Transmit::RAW(MQTT_CID);

            SIM800::Transmit::UINT8(0x1A);

            state = State::RUNNING;

            meterLastData.Reset();

            meterPing.Reset();

            HAL_PINS::SendState();

            Sender::Reset();
        }
        break;

    case State::RUNNING:

        if (!Sender::SendAll(answer))
        {
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
        }

        break;
    }
}


void  MQTT::Packet::Publish(pchar MQTT_topic, pchar MQTT_messege)
{
    SIM800::Transmit::UINT8(0x30);
    SIM800::Transmit::UINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::Transmit::UINT8(0);
    SIM800::Transmit::UINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit::RAW(MQTT_topic);
    SIM800::Transmit::RAW(MQTT_messege);
}


void MQTT::Packet::Publish(pchar topic, int value)
{
    char buffer[32];
    sprintf(buffer, "%d", value);
    Publish(topic, buffer);
}


void MQTT::CallbackOnReceiveData(pchar)
{
    meterLastData.Reset();
}
