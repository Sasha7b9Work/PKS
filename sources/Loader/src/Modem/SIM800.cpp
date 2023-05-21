// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM800.h"
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
            WAIT_IP_INITIAL,
            RUNNING_UPDATER
        };

        static TimeMeterMS meter;

        static void Set(E);
    };

    TimeMeterMS State::meter;

    static State::E state = State::START;

    void State::Set(E new_state)
    {
        state = new_state;
        meter.Reset();
    }

    static void Reset()
    {
        state = State::START;
        Modem::Reset();
    }

    static bool MeterIsRunning(uint time)
    {
        if (State::meter.ElapsedTime() <= time)
        {
            return true;
        }

        Reset();

        return false;
    }


    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    void Update(pchar);

    static bool ProcessUnsolicited(pchar);

    bool IsRegistered();

    pchar LevelSignal();

    static TimeMeterMS meterCSQ;

    static char levelSignal[16] = { '0', '\0' };
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

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        State::Set(State::WAIT_ANSWER_ATE0);
        strcpy(levelSignal, "0");
        break;

    case State::WAIT_ANSWER_ATE0:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_ANSWER_GSMBUSY);
                SIM800::Transmit("AT+GSMBUSY=1");
            }
        }
        break;

    case State::WAIT_ANSWER_GSMBUSY:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_ANSWER_CREG);
                SIM800::Transmit("AT+CREG?");                                       // CREG?
            }
        }
        break;

    case State::WAIT_ANSWER_CREG:
        if(MeterIsRunning(30000))
        {
            Timer::DelayMS(1);
            if (strcmp(GetWord(answer, 1), "+CREG") == 0)
            {
                int stat = GetWord(answer, 3)[0] & 0x0f;

                if (stat == 1 ||        // Registered, home network
                    stat == 5)          // Registered, roaming
                {
                    State::Set(State::WAIT_IP_INITIAL);
                    SIM800::Transmit("AT+CIPSTATUS");
                }
                else
                {
                    SIM800::Transmit("AT+CREG?");
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(GetWord(answer, 3), "INITIAL") == 0)
            {
                state = State::RUNNING_UPDATER;
            }
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
