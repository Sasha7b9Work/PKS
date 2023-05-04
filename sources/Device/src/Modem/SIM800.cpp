// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Commands.h"
#include "Modem/Parser.h"
#include <cstring>


using namespace Parser;


namespace Modem
{
    void CallbackOnErrorSIM800();
}


namespace MQTT
{
    void Connect();
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

            WAIT_REGISTRATION,

            RUNNING
        };
    };

    static State::E state = State::START;

    // Возращает время до получения ответа
    void Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitUINT(uint);

    void Update(const String &);

    static void Reset();
}


void SIM800::Update(const String &answer)
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        state = State::WAIT_ATE0;
        meter.Reset();
        break;

    case State::WAIT_ATE0:
        if (meter.ElapsedTime() > 10000)
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
        if (meter.ElapsedTime() > 10000)
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
            if (answer.Size() != 0)
            {
                String word = GetWord(answer, 3);

                if (word.Size() != 0)
                {
                    int stat = word.c_str()[0] & 0x0f;

                    if (stat == 0 ||    // Not registered, MT is not currently searching a new operator to register to
                        stat == 2 ||    // Not registered, but MT is currently searching a new operator to register to
                        stat == 3 ||    // Registration denied
                        stat == 4)      // Unknown
                    {
                        SIM800::Transmit("AT+CREG?");
                    }
                    else
                    {
                        state = State::WAIT_IP_INITIAL;
                    }
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:


        break;

    case State::WAIT_REGISTRATION:
        if (!Command::WaitCIPSTATUS("IP INITIAL"))
        {
            Reset();
        }
        //            else if (!SIM800::TransmitAndWaitAnswer("AT+CSTT=\"internet\",\"\",\"\"", "OK"))
        //            {
        //                Reset();
        //            }

        TimeMeterMS().Wait(5000);

        //            if (!SIM800::TransmitAndWaitAnswer("AT+CIICR", "OK"))
        //            {
        //                Reset();
        //            }

        TimeMeterMS().Wait(5000);

        SIM800::Transmit("AT+CIFSR");

        TimeMeterMS().Wait(5000);

        if (!Command::ConnectToTCP())
        {
            Reset();
        }
        else
        {
            MQTT::Connect();

            state = State::RUNNING;
        }
        if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        break;

    case State::RUNNING:
        break;
    }
}


void SIM800::Transmit(pchar message)
{
    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::TransmitUINT(uint value)
{
    HAL_USART_GPRS::Transmit(&value, 4);
}


void SIM800::Reset()
{
    state = State::START;
    Modem::CallbackOnErrorSIM800();
}
