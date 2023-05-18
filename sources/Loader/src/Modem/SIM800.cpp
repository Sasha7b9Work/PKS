// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include <cstring>
#include <cstdio>


using namespace Parser;


namespace Modem
{
    void CallbackOnErrorSIM800();
}


namespace Updater
{
    void Update(const String &);
}


namespace SIM800
{
    struct State
    {
        enum E
        {
            START,
            WAIT_ANSWER_ATE0,
            WAIT_ANSWER_GSMBUSY,
            WAIT_ANSWER_CREG,
            WAIT_IP_INITIAL,

            RUNNING_UPDATER
        };
    };

    static State::E state = State::START;

    void Transmit(pchar);
    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    void Update(const String &);

    void Reset();

    static bool ProcessUnsolicited(const String &);

    bool IsRegistered();

    String LevelSignal();

    static TimeMeterMS meterCSQ;

    static String levelSignal("0");
}


bool SIM800::ProcessUnsolicited(const String &answer)
{
    String first_word = Parser::GetWord(answer, 1);

    if (answer == "CLOSED")
    {
        Reset();
        return true;
    }
    else if (first_word == "+CSQ")
    {
        levelSignal = Parser::GetWord(answer, 2);
    }
    else if (answer == "SEND FAIL")
    {
        return true;
    }
    else if (answer == "SEND OK")
    {
        return true;
    }
    else if (first_word == "+IPD")
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
        state = State::WAIT_ANSWER_ATE0;
        meter.Reset();
        levelSignal.Set("0");
        break;

    case State::WAIT_ANSWER_ATE0:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == "OK")
        {
            SIM800::Transmit("AT+GSMBUSY=1");
            state = State::WAIT_ANSWER_GSMBUSY;
            meter.Reset();
        }
        break;

    case State::WAIT_ANSWER_GSMBUSY:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        if (answer == "OK")
        {
            SIM800::Transmit("AT+CREG?");                                       // CREG?
            state = State::WAIT_ANSWER_CREG;
            meter.Reset();
        }
        break;

    case State::WAIT_ANSWER_CREG:
        if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        else
        {
            if (GetWord(answer, 1) == "+CREG")
            {
                int stat = GetWord(answer, 3).c_str()[0] & 0x0f;

                if (stat == 1 ||        // Registered, home network
                    stat == 5)          // Registered, roaming
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
    {
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        int i = 0;
    }
        break;



    case State::RUNNING_UPDATER:
        Updater::Update(answer);
        break;
    }
}


String SIM800::LevelSignal()
{
    return levelSignal;
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
