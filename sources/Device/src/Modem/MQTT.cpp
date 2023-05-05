﻿// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <cstring>


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

    // пакет подписки на топик
    static void SubscribePacket(const char MQTT_topic[15]);

    void Reset();

    void SendMeasure(const FullMeasure &);

    void CallbackOnReceiveData();

    // Сбрасывается каждый раз при поступлении данынх
    static TimeMeterMS meterLastData;
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
        }
        break;

    case State::RUNNING:

        if (meterLastData.ElapsedTime() > 30000)
        {
            SIM800::Reset();
        }

        if (meterPing.ElapsedTime() > 20000)
        {
            meterPing.Reset();
            SIM800::Transmit("AT+CIPSEND");
            need_ping = true;
        }

        if (answer == ">")
        {
            if (need_measure)
            {
                PublishPacket("base/state/voltage_c", "12");
//                PublishPacket("base/state/text", "message1");
//                PublishPacket("base/state/voltage_c", "25235");
                need_measure = false;
            }
            else if(need_ping)
            {
                SIM800::TransmitUINT8(0xC0);
                SIM800::TransmitUINT8(0x00);
                SIM800::TransmitUINT8(0x1A);
                need_ping = false;
            }
        }

        break;
    }
}


void MQTT::SendMeasure(const FullMeasure &meas)
{
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


void MQTT::Reset()
{
    state = State::IDLE;
}


void  MQTT::PublishPacket(const char *MQTT_topic, const char *MQTT_messege)
{
//    0000   04 27 58 55 4e 58 a4 97 b1 e4 8a 95 08 00 45 00   .'XUNX........E.
//    0010   00 42 b2 ac 40 00 80 06 4c df 64 7d cc e9 59 6c.B..@...L.d}..Yl
//    0020   70 57 2c a0 07 5b 03 b6 e5 2e cb aa 6d 2f 50 18   pW, ..[......m / P.
//    0030   01 00 34 62 00 00 30 18 00 14 62 61 73 65 2f 73   ..4b..0...base / s
//    0040   74 61 74 65 2f 76 6f 6c 74 61 67 65 5f 63 31 32   tate / voltage_c12


//    0000   04 27 58 55 4e 58 a4 97 b1 e4 8a 95 08 00 45 00   .'XUNX........E.
//    0010   00 42 b2 ac 40 00 80 06 4c df 64 7d cc e9 59 6c.B..@...L.d}..Yl
//    0020   70 57 2c a0 07 5b 03 b6 e5 2e cb aa 6d 2f 50 18   pW, ..[......m / P.
//    0030   01 00 34 62 00 00    
    
//  30
//  18
//  00
//  14
//  62 61 73 65 2f 73 74 61 74 65 2f 76 6f 6c 74 61 67 65 5f 63 base/state/voltage_c
//  31 32                                                       12

    SIM800::TransmitUINT8(0x30);
    TimeMeterMS().Wait(100);
//    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::TransmitUINT8(0x18);
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit(MQTT_topic); // топик 
    SIM800::Transmit(MQTT_messege);
    SIM800::TransmitUINT8(0x1A);
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


void MQTT::CallbackOnReceiveData()
{
    meterLastData.Reset();
}
