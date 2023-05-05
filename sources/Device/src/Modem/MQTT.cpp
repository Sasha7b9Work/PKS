// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <cstring>


namespace SIM800
{
    uint Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitRAW(pchar);
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

//    static const char MQTT_type[15] = "MQIsdp";             // тип протокола НЕ ТРОГАТЬ!
    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // уникальное имя устройства в сети MQTT
    static const char *MQTT_user = "";                   // api.cloudmqtt.com > Details > User
    static const char *MQTT_pass = "";                   // api.cloudmqtt.com > Details > Password

    // пакет на публикацию
    static void PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15]);

    // пакет подписки на топик
    static void SubscribePacket(const char MQTT_topic[15]);

    static void Reset();
}


void MQTT::Update(const String &answer)
{
    const uint DEFAULT_TIME = 10000;

    TimeMeterMS meter;

    if (meterPing.ElapsedTime() > 20000)
    {
        meter.Reset();

        SIM800::Transmit("AT+CIPSEND");

        need_ping = true;
    }

    if (answer == ">" && need_ping)
    {
        SIM800::TransmitUINT8(0xC0);
        SIM800::TransmitUINT8(0x00);
        need_ping = false;
        return;
    }

    switch (state)
    {
    case State::IDLE:
        SIM800::Transmit("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == ">")
        {
            SIM800::TransmitUINT8(0x10);   // маркер пакета на установку соединения
            SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 14));
//            SIM800::TransmitUINT8(0x22);

            // тип протокола
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_type));
            SIM800::TransmitRAW(MQTT_type);

            // просто так нужно
            SIM800::TransmitUINT8(0x05);    // версия протокола
            SIM800::TransmitUINT8(0x00);    // connect flag
            SIM800::TransmitUINT8(0x00);    // \ keep alive 
            SIM800::TransmitUINT8(0x3c);    // /

            SIM800::TransmitUINT8(0x05);    // property lenth
            SIM800::TransmitUINT8(0x11);    // 
            SIM800::TransmitUINT8(0x00);    // \ 
            SIM800::TransmitUINT8(0x00);    // | sexxion expiry interval
            SIM800::TransmitUINT8(0x00);    // |
            SIM800::TransmitUINT8(0x00);    // /

            // MQTT  идентификатор устройства
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_CID));
            SIM800::TransmitRAW(MQTT_CID);

            // MQTT логин
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_user));
//            SIM800::TransmitRAW(MQTT_user);
//
//            // MQTT пароль
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_pass));
//            SIM800::TransmitRAW(MQTT_pass);

            //    // пакет публикации
            //    PublishPacket("C5/status", "Подключено");
            //
            //    // пакет подписки на присылаемые команды
            //    SubscribePacket("C5/comand");
            //
            //    // пакет подписки на присылаемые значения таймера
            //    SubscribePacket("C5/settimer");

                // маркер завершения пакета
            SIM800::TransmitUINT8(0x1A);

            state = State::RUNNING;
        }
        break;

    case State::RUNNING:
        break;
    }
}


void MQTT::Reset()
{

}


void  MQTT::PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15])
{
    SIM800::TransmitUINT8(0x30);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit(MQTT_topic); // топик
    SIM800::Transmit(MQTT_messege);
}


void MQTT::SubscribePacket(const char MQTT_topic[15])
{
    // сумма пакета
    SIM800::TransmitUINT8(0x82);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + 5));

    // просто так нужно
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8(0x01);
    SIM800::TransmitUINT8(0);

    // топик
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit(MQTT_topic);

    SIM800::TransmitUINT8(0);
}
