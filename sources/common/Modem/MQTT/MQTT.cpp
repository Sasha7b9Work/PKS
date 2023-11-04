// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM800.h"
#include "Modem/MQTT/Sender.h"
#include "Settings/Settings.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    struct StateMQTT
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // ∆дЄм приглашени€ ">"
            SEND_VERSION,
            WAIT_DATA_FOR_SEND,         // ∆дЄм данные дл€ отсылки
            WAIT_CONFIRM                // ќжидаем подтверждени€
        };
    };

    StateMQTT::E state = StateMQTT::IDLE;

    // —брасываетс€ каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;

    static uint time_connect = 0;

    uint TimeConnect()
    {
        return time_connect;
    }

    // ѕерейти в режим ожидани€ ответа
    void ToStateConfirm()
    {
        state = StateMQTT::WAIT_CONFIRM;
    }
}


bool MQTT::InStateWaitData()
{
    return (state == StateMQTT::WAIT_DATA_FOR_SEND);
}


bool MQTT::InStateSendVersion()
{
    return (state == StateMQTT::SEND_VERSION);
}


bool MQTT::InStateIdle()
{
    return state == StateMQTT::IDLE;
}


bool MQTT::InStateRunning()
{
    return state >= StateMQTT::WAIT_DATA_FOR_SEND;
}


void MQTT::Reset()
{
    state = StateMQTT::IDLE;
    meterLastData.Reset();
}


void MQTT::Update(pchar answer)
{
    static TimeMeterMS meter;

    switch (state)
    {
    case StateMQTT::IDLE:
        time_connect = Timer::TimeMS();
        LOG_WRITE("+++ MQTT::IDLE +++");
        SIM800::Transmit::With0D("AT+CIPSEND");
        meter.Reset();
        state = StateMQTT::WAIT_RESPONSE_CIPSEND;
        break;

    case StateMQTT::WAIT_RESPONSE_CIPSEND:
        if (strcmp(answer, ">") == 0)
        {
            char *MQTT_type = "MQTT";

            char MQTT_CID[32];

            std::sprintf(MQTT_CID, "pks-%d%02d-%s", gset.GetNumberSteps(), gset.GetKoeffCurrent(), HAL::GetUID());

            LOG_WRITE(MQTT_CID);            // WARNING без этого не соедин€етс€

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

            state = StateMQTT::SEND_VERSION;

            meterLastData.Reset();
        }
        break;

    case StateMQTT::SEND_VERSION:
        if (Sender::SendVersion())
        {
            state = StateMQTT::WAIT_DATA_FOR_SEND;
        }
        break;

    case StateMQTT::WAIT_DATA_FOR_SEND:
        break;

    case StateMQTT::WAIT_CONFIRM:

        if (std::strcmp(answer, "SEND OK") == 0 ||
            std::strcmp(answer, "SEND FAIL") == 0)
        {
            state = StateMQTT::WAIT_DATA_FOR_SEND;
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


void MQTT::Packet::PublishF(pchar topic, float value)
{
    char buffer[32];

    if (value < 10.0f)
    {
        sprintf(buffer, "%2.1f", value);
    }
    else
    {
        sprintf(buffer, "%d", (int)(value + 0.5f));
    }

    Publish(topic, buffer);
}


void MQTT::CallbackOnReceiveData(pchar answer)
{
    meterLastData.Reset();

    LOG_WRITE((char *)answer);
}
