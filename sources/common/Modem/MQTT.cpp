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
#include "Modem/Sender/ContactorsIsOK.h"
#include "Modem/Sender/GP.h"
#include "Modem/SIM800.h"
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
            WAIT_RESPONSE_CIPSEND,      // ���� ����������� ">"
            RUNNING
        };
    };

    static State::E state = State::IDLE;

    static TimeMeterMS meterPing;

    // ���� nned_ping == true, �� �������� ������� �����
    static bool need_ping = false;

    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // ���������� ��� ���������� � ���� MQTT

    // ������������ ������ ��� ��� ����������� ������
    static TimeMeterMS meterLastData;
}


bool MQTT::InStateRunning()
{
    return state == State::RUNNING;
}


void MQTT::Update(pchar answer)
{
    static TimeMeterMS meter;

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
            SIM800::TransmitUINT8(0x10);   // ������ ������ �� ��������� ����������
            SIM800::TransmitUINT8(0x1c);
//            SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 14));

            // ��� ���������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_type));
            SIM800::TransmitRAW(MQTT_type);

            // ������ ��� �����
            SIM800::TransmitUINT8(0x04);    // ������ ���������
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

            Sender::Counter::Reset();
        }
        break;

    case State::RUNNING:

        if (strcmp(answer, ">") == 0)
        {
            bool sending = false;

            if (Sender::Counter::OnEventSend())
            {
                sending = true;
            }

            if (Sender::Measure::OnEventSend())
            {
                sending = true;
            }

//            Sender::LevelContactors::OnEventSend();
//
//            Sender::ContactorsIsOK::OnEventSend();
//
//            Sender::GP::OnEventSend();
//
//            if (need_ping)
//            {
//                SIM800::TransmitUINT8(0xC0);
//                SIM800::TransmitUINT8(0x00);
//                need_ping = false;
//            }

            SIM800::TransmitUINT8(sending ? (uint8)0x1A : (uint8)0x1B);
        }
        else
        {
            if (meter.ElapsedTime() > 1000)
            {
                SIM800::Transmit("AT+CIPSEND");

                meter.Reset();
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
}


void MQTT::CallbackOnReceiveData(pchar)
{
    meterLastData.Reset();
}
