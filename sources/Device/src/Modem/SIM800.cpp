// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include <cstring>


using namespace Parser;


namespace Modem
{
    void CallbackOnErrorSIM800();
}


namespace MQTT
{
    void Update(const String &);
    void SendMeasure(const FullMeasure &);
}


namespace SIM800
{
    struct State
    {
        enum E
        {
            START,
            WAIT_ATE0,
            WAIT_GSMBUSY,
            WAIT_CREG,
            WAIT_IP_INITIAL,
            WAIT_CSTT,
            WAIT_IP_START,
            WAIT_CIICR,
            WAIT_IP_GPRSACT,
            WAIT_CIFSR,
            WAIT_IP_STATUS,
            WAIT_TCP_CONNECT,
            WAIT_CIPHEAD,
            RUNNING_MQTT,
        };
    };

    static State::E state = State::START;

    // Возращает время до получения ответа
    void Transmit(pchar);
    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    void Update(const String &);

    static void Reset();

    static bool ProcessUnsolicited(const String &);

    void SendMeasure(const FullMeasure &);
}


bool SIM800::ProcessUnsolicited(const String &answer)
{
    if (answer == "CLOSED")
    {
        Reset();
        return true;
    }
    else if (answer == "SEND FAIL")
    {
        return true;
    }
    else if (answer == "SEND OK")
    {
        return true;
    }
    else if (Parser::GetWord(answer, 1) == "+IPD")
    {
        return true;
    }
    else if (answer.c_str()[0] == '>')
    {
        return false;
    }

    return false;
}


void SIM800::Update(const String &answer)
{
    if (ProcessUnsolicited(answer))
    {
        return;
    }

    const uint DEFAULT_TIME = 10000;

    static TimeMeterMS meter;

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        state = State::WAIT_ATE0;
        meter.Reset();
        break;

    case State::WAIT_ATE0:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+GSMBUSY=1");
            state = State::WAIT_GSMBUSY;
            meter.Reset();
        }

        break;

    case State::WAIT_GSMBUSY:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+CREG?");
            state = State::WAIT_CREG;
            meter.Reset();
        }
        break;

    case State::WAIT_CREG:
        if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        else
        {
            if (GetWord(answer, 1) == "+CREG")
            {
                int stat = GetWord(answer, 3).c_str()[0] & 0x0f;

                if (stat == 1 ||    // Registered, home network
                    stat == 5)      // Registered, roaming
                {
                    SIM800::Transmit("AT+CIPSTATUS");
                    state = State::WAIT_IP_INITIAL;
                    meter.Reset();
                }
                else
                {
                    SIM800::Transmit("AT+CREG?");
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "INITIAL")
        {
            SIM800::Transmit("AT+CSTT=\"internet\",\"\",\"\"");
            state = State::WAIT_CSTT;
            meter.Reset();
        }
        break;

    case State::WAIT_CSTT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else
        {
            if (GetWord(answer, 1) == "OK")
            {
                state = State::WAIT_IP_START;
                meter.Reset();
                SIM800::Transmit("AT+CIPSTATUS");
            }
            else if (GetWord(answer, 1) == "ERROR")
            {
                Reset();
            }
        }
        break;

    case State::WAIT_IP_START:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "START")
        {
            state = State::WAIT_CIICR;
            meter.Reset();
            SIM800::Transmit("AT+CIICR");
        }
        break;

    case State::WAIT_CIICR:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 1) == "OK")
        {
            state = State::WAIT_IP_GPRSACT;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTATUS");
        }
        break;

    case State::WAIT_IP_GPRSACT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "GPRSACT")
        {
            state = State::WAIT_CIFSR;
            meter.Reset();
            SIM800::Transmit("AT+CIFSR");
        }
        break;

    case State::WAIT_CIFSR:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 1) != "OK")
        {
            // Здесь получаем IP-адрес
            state = State::WAIT_IP_STATUS;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTATUS");
        }
        break;

    case State::WAIT_IP_STATUS:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (GetWord(answer, 3) == "STATUS")
        {
            state = State::WAIT_TCP_CONNECT;
            meter.Reset();
            SIM800::Transmit("AT+CIPSTART=\"TCP\",\"dev.rightech.io\",\"1883\"");
        }
        break;

    case State::WAIT_TCP_CONNECT:
        if (meter.ElapsedTime() > 160000)
        {
            Reset();
        }
        else if (GetWord(answer, 1) == "ALREADY" || GetWord(answer, 2) == "OK")
        {
            state = State::WAIT_CIPHEAD;
            meter.Reset();
            SIM800::Transmit("AT+CIPHEAD=1");
        }
        else if (GetWord(answer, 2) == "FAIL")
        {
            Reset();
        }
        break;

    case State::WAIT_CIPHEAD:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            meter.Reset();
            state = State::RUNNING_MQTT;
        }
        else if (answer == "ERROR")
        {
            Reset();
        }

        break;

    case State::RUNNING_MQTT:
        MQTT::Update(answer);
        break;
    }
}


void SIM800::Transmit(pchar message)
{
    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::TransmitRAW(pchar message)
{
    HAL_USART_GPRS::Transmit(message);
}


void SIM800::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::Reset()
{
    state = State::START;
    Modem::CallbackOnErrorSIM800();
}


void SIM800::SendMeasure(const FullMeasure &measure)
{
    MQTT::SendMeasure(measure);
}
