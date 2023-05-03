// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace SIM800
{
    const int MAX_LENGTH_ANSWERR = 128;

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
