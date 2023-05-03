// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Commands.h"
#include <cstring>


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
    const int MAX_LENGTH_ANSWERR = 128;

    struct State
    {
        enum E
        {
            START,
            WAIT_REGISTRATION,

            RUNNING
        };
    };

    static State::E state = State::START;

    namespace Answer
    {
        static char buffer[MAX_LENGTH_ANSWERR] = { '\0' };
        static int pointer = 0;
        static bool ready = false;

        static void Clear()
        {
            pointer = 0;
            ready = false;
            buffer[0] = '\0';
        }

        static void Push(char symbol)
        {
            if (symbol == 0x0a)
            {
                return;
            }

            if (pointer == MAX_LENGTH_ANSWERR - 1)
            {
                pointer = 0;
            }

            if (symbol == 0x0d && pointer == 0)
            {
                return;
            }

            if (symbol == 0x0d)
            {
                if (ready == true)
                {
                    Clear();
                }
                else
                {
                    buffer[pointer++] = '\0';
                    ready = true;
                }
            }
            else
            {
                buffer[pointer++] = symbol;
            }
        }
    }

    bool LastAnswer(char[MAX_LENGTH_ANSWERR]);

    String LastAnswer();

    // Возращает время до получения ответа
    uint Transmit(pchar, uint timeout = TIME_WAIT_ANSWER);
    void TransmitUINT8(uint8);
    void TransmitUINT(uint);

    // Передаёт сообщение и возвращает true, если принят ответ answer
    bool TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout = TIME_WAIT_ANSWER);

    void CallbackOnReceive(char);

    void Update();

    // Ожидает ответа 
    static void WaitAnswer(pchar, uint timeout = TIME_WAIT_ANSWER);

    static void Reset();
}


void SIM800::Update()
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        if (SIM800::TransmitAndWaitAnswer("AT+GSMBUSY=1", "OK"))
        {
            state = State::WAIT_REGISTRATION;
            meter.Reset();
        }
        else
        {
            Reset();
        }
        break;

    case State::WAIT_REGISTRATION:
        if (Command::RegistrationIsOk())
        {
            if (!SIM800::TransmitAndWaitAnswer("AT+CSTT=\"internet\",\"\",\"\"", "OK"))
            {
                Reset();
            }

            TimeMeterMS().Wait(1000);

            if (!SIM800::TransmitAndWaitAnswer("AT+CIICR", "OK"))
            {
                Reset();
            }

            TimeMeterMS().Wait(1000);

            SIM800::Transmit("AT+CIFSR");

            TimeMeterMS().Wait(1000);

            if (!Command::ConnectToTCP())
            {
                Reset();
            }
            else
            {
                MQTT::Connect();

                state = State::RUNNING;
            }
        }
        else if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        break;

    case State::RUNNING:
        break;
    }
}


uint SIM800::Transmit(pchar message, uint timeout)
{
    TimeMeterMS meter;

    Answer::Clear();

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);

    while ((meter.ElapsedTime() < timeout) && !Answer::ready)
    {
    }

    return meter.ElapsedTime();
}


void SIM800::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::TransmitUINT(uint value)
{
    HAL_USART_GPRS::Transmit(&value, 4);
}


bool SIM800::TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout)
{
    Transmit(message, timeout);

    char *last_answer = LastAnswer().c_str();

    return std::strcmp(last_answer, answer) == 0;
}


void SIM800::CallbackOnReceive(char symbol)
{
    Answer::Push(symbol);
}


bool SIM800::LastAnswer(char out[MAX_LENGTH_ANSWERR])
{
    if (Answer::ready)
    {
        std::strcpy(out, Answer::buffer);
        return true;
    }

    out[0] = '\0';

    return false;
}


String SIM800::LastAnswer()
{
    char buffer[MAX_LENGTH_ANSWERR];

    LastAnswer(buffer);

    return String(buffer);
}


void SIM800::Reset()
{
    state = State::START;
    Modem::CallbackOnErrorSIM800();
}
