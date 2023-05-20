// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include <cstring>
#include <cstdio>


using namespace Parser;
using namespace std;


namespace Updater
{
    void Update(pchar);
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
            WAIT_ANSWER_CGATT,
            WAIT_IP_INITIAL,
            RUNNING_UPDATER
        };
    };

    static State::E state = State::START;

    void Transmit(pchar);
    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    void Update(pchar);

    static bool ProcessUnsolicited(pchar);

    bool IsRegistered();

    pchar LevelSignal();

    static TimeMeterMS meterCSQ;

    static char levelSignal[16] = { '0', '\0' };

    static void Reset()
    {
        state = State::START;
        Modem::Reset();
    }
}


bool SIM800::ProcessUnsolicited(pchar answer)
{
    pchar first_word = Parser::GetWord(answer, 1);

    if (strcmp(answer, "CLOSED") == 0)
    {
        Reset();
        return true;
    }
    else if (strcmp(first_word, "+CSQ") == 0)
    {
        strcpy(levelSignal, Parser::GetWord(answer, 2));
        return true;
    }
    else if (strcmp(answer, "SEND FAIL") == 0)
    {
        return true;
    }
    else if (strcmp(answer, "SEND OK") == 0)
    {
        return true;
    }
    else if (strcmp(first_word, "+IPD") == 0)
    {
        return false;
    }

    return false;
}


void SIM800::Update(pchar answer)
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
        strcpy(levelSignal, "0");
        break;

    case State::WAIT_ANSWER_ATE0:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
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
        else if (strcmp(answer, "OK") == 0)
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
            Timer::DelayMS(1);
            if (strcmp(GetWord(answer, 1), "+CREG") == 0)
            {
                int stat = GetWord(answer, 3)[0] & 0x0f;

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
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(GetWord(answer, 3), "INITIAL") == 0)
        {
            state = State::RUNNING_UPDATER;
        }
        break;

    case State::WAIT_ANSWER_CGATT:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (strcmp(answer, "OK") == 0)
        {
            state = State::RUNNING_UPDATER;
        }
        break;

    case State::RUNNING_UPDATER:
        Updater::Update(answer);
        break;
    }
}


pchar SIM800::LevelSignal()
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
