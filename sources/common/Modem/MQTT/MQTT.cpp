// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/MQTT.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM800.h"
#include "Modem/MQTT/_Sender/_Sender.h"
#include "Settings/Settings.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>


using namespace std;


namespace MQTT
{
    namespace Request
    {
        static char topic[64] = { '\0' };
        static char message[32] = { '\0' };

        void Clear()
        {
            topic[0] = '\0';
            message[0] = '\0';
        }

        bool IsEmpty()
        {
            return topic[0] == '\0';
        }

        void Set(pchar _topic, int value)
        {
            std::strcpy(topic, _topic);
            std::sprintf(message, "%d", value);
        }

        void SendFinalSequence(bool not_empty)
        {
            SIM800::Transmit::UINT8(not_empty ? (uint8)0x1A : (uint8)0x1B);
        }

        void Send()
        {
            if (topic[0])
            {
                MQTT::Packet::Publish(topic, message);
            }

            SendFinalSequence(topic[0] != '\0');
        }
    }

    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // ��� ����������� ">"
            SEND_VERSION,
            WAIT_DATA_FOR_SEND          // ��� ������ ��� �������
        };
    };

    static State::E state = State::IDLE;

    // ������������ ������ ��� ��� ����������� ������
    static TimeMeterMS meterLastData;

    static uint time_connect = 0;

    uint TimeConnect()
    {
        return time_connect;
    }

    static char last_received = 0;          // ��������� �������� ������
}


void MQTT::CallbackOnReceiveChar(char symbol)
{
    last_received = symbol;
}


bool MQTT::InStateRunning()
{
    return (state == State::WAIT_DATA_FOR_SEND);
}


bool MQTT::InStateIdle()
{
    return state == State::IDLE;
}


void MQTT::Reset()
{
    state = State::IDLE;
    meterLastData.Reset();
}


void MQTT::Update(pchar answer)
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        Request::Clear();
        time_connect = Timer::TimeMS();
        LOG_WRITE("MQTT start connect %d ms", time_connect);
        Sender::Reset();
        LOG_WRITE("+++ MQTT::IDLE +++");
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

            std::sprintf(MQTT_CID, "pks-%d%02d-%s", gset.GetNumberSteps(), gset.GetKoeffCurrent(), HAL::GetUID(guid));

            LOG_WRITE(MQTT_CID);            // WARNING ��� ����� �� �����������

            SIM800::Transmit::UINT8(0x10);   // ������ ������ �� ��������� ����������
            SIM800::Transmit::UINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 8));

            // ��� ���������
            SIM800::Transmit::UINT8(0x00);
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_type));
            SIM800::Transmit::RAW(MQTT_type);

            // ������ ��� �����
            SIM800::Transmit::UINT8(0x04);    // ������ ���������
            SIM800::Transmit::UINT8(0x02);    // connect flag
            SIM800::Transmit::UINT8(0x00);    // \ keep alive 
            SIM800::Transmit::UINT8(0x3c);    // /

            SIM800::Transmit::UINT8(0x00);    // property lenth
            SIM800::Transmit::UINT8((uint8)std::strlen(MQTT_CID));
            SIM800::Transmit::RAW(MQTT_CID);

            SIM800::Transmit::UINT8(0x1A);

            state = State::SEND_VERSION;

            meterLastData.Reset();

            HAL_PINS::SendState();
        }
        break;

    case State::SEND_VERSION:
        if (Send::Version())
        {
            state = State::WAIT_DATA_FOR_SEND;
        }
        break;

    case State::WAIT_DATA_FOR_SEND:
        break;
    }
}


bool MQTT::Send::Counter(int counter)
{
    last_received = 0;

    if (MQTT::state != State::WAIT_DATA_FOR_SEND)
    {
        return false;
    }

    Request::Set("base/state/counter", counter);

    TimeMeterMS meter;

    SIM800::Transmit::With0D("AT+CIPSEND");

    while (last_received != '>')
    {
        if (meter.ElapsedTime() > 20)
        {
            return false;
        }
    }

    Request::Send();
    Request::Clear();

    return true;
}


bool MQTT::Send::Version()
{
    last_received = 0;

    if (MQTT::state != State::SEND_VERSION)
    {
        return false;
    }

    char buffer[32];

    std::sprintf(buffer, "v%d:%d:%d", VERSION, gset.GetNumberSteps(), gset.GetKoeffCurrent());

    TimeMeterMS meter;

    SIM800::Transmit::With0D("AT+CIPSEND");

    while (last_received != '>')
    {
        if (meter.ElapsedTime() > 20)
        {
            return false;
        }
    }

    MQTT::Packet::Publish("/versionSW", buffer);

    MQTT::Packet::Publish("base/id", HAL::GetUID(buffer));

    MQTT::Packet::Publish("/last/reset", "-");

    if (_GET_BIT(GL::_RCU_RSTSCK, 28))
    {
        MQTT::Packet::Publish("/last/reset", "Software");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 31))
    {
        MQTT::Packet::Publish("/last/reset", "Low power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 30))
    {
        MQTT::Packet::Publish("/last/reset", "Watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 29))
    {
        MQTT::Packet::Publish("/last/reset", "Free watchdog");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 27))
    {
        MQTT::Packet::Publish("/last/reset", "Power");
    }
    if (_GET_BIT(GL::_RCU_RSTSCK, 26))
    {
        MQTT::Packet::Publish("/last/reset", "External pin");
    }

    Request::SendFinalSequence(true);

    Request::Clear();

    return true;
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
