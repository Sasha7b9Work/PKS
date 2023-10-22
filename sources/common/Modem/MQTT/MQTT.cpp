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
            WAIT_RESPONSE_CIPSEND,      // Ждём приглашения ">"
            SEND_VERSION,
            WAIT_DATA_FOR_SEND,         // Ждём данные для отсылки
            WAIT_CONFIRM                // Ожидаем подтверждения
        };
    };

    StateMQTT::E state = StateMQTT::IDLE;

    // Сбрасывается каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;

    static uint time_connect = 0;

    uint TimeConnect()
    {
        return time_connect;
    }

    // Перейти в режим ожидания ответа
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
    return state >= StateMQTT::SEND_VERSION;
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

            pchar user = "mqttusr";
            pchar password = "tgsdj9e5dc";

            char MQTT_CID[32];
            char GUID[32];

            std::sprintf(MQTT_CID, "-uskd----%s", HAL::GetUID(GUID));

//          LOG_WRITE(MQTT_CID);            // WARNING без этого не соединяется

            SIM800::Transmit::UINT8(0x10);   // маркер пакета на установку соединения

            SIM800::Transmit::UINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + std::strlen(user) + std::strlen(password) + 18));  // Размер передаваемых в MQTT данных
            SIM800::Transmit::UINT8(0x00);                                                                   // \ Размер MQTT_type
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_type));                                          // /
            SIM800::Transmit::RAW(MQTT_type);

            SIM800::Transmit::UINT8(0x05);    // версия протокола
//                                         +------------------ User Name Flag
//                                         |+----------------- Password Flag
//                                         ||    
//                                         ||    +------------ Clean Start
            uint8 connect_flag = BINARY_U8(11000010);

            SIM800::Transmit::UINT8(connect_flag);      // connect flag
            SIM800::Transmit::UINT8(0x00);              // \ keep alive 
            SIM800::Transmit::UINT8(0x3c);              // /

            SIM800::Transmit::UINT8(0x05);              // property length
            SIM800::Transmit::UINT8(0x11);              // Session Expiry Interval identifier
            SIM800::Transmit::UINT8(0x00);              // \ 
            SIM800::Transmit::UINT8(0x00);              // | Session Expiry Interval (10)
            SIM800::Transmit::UINT8(0x01);              // |
            SIM800::Transmit::UINT8(0x2c);              // /

            SIM800::Transmit::UINT8(0x00);
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_CID));
            SIM800::Transmit::RAW(MQTT_CID);

            SIM800::Transmit::UINT8(0x00);
            SIM800::Transmit::UINT8((uint8)std::strlen(user));
            SIM800::Transmit::RAW(user);

            SIM800::Transmit::UINT8(0x00);
            SIM800::Transmit::UINT8((uint8)std::strlen(password));
            SIM800::Transmit::RAW(password);

            SIM800::Transmit::UINT8(0x1A);

            state = StateMQTT::SEND_VERSION;

            meterLastData.Reset();
        }
        break;

    case StateMQTT::SEND_VERSION:
//        if (Sender::SendVersion())
//        {
//            state = StateMQTT::WAIT_DATA_FOR_SEND;
//        }
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
