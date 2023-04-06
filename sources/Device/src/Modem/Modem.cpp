// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Modules/SIM800C/SIM800C.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace Modem
{
    static const int SIZE_BUFFER = 128;
    static char answer[SIZE_BUFFER] = { '\0' };
    static int pointer = 0;
}


void Modem::Init()
{
    SIM800C::Init();

    Modem::Transmit("ATE0");
    Modem::Transmit("ATV0");
}


bool Modem::ExistUpdate()
{
    return false;
}


void Modem::Transmit(pchar message)
{
    answer[0] = '\0';
    pointer = 0;

    HAL_USART::Transmit(message);

    static const char end_message[2] = { 0x13, 0 };

    HAL_USART::Transmit(end_message);
}


pchar Modem::LastAnswer()
{
    return answer;
}


void Modem::CallbackOnReceive(char symbol)
{
    if (pointer < SIZE_BUFFER - 1)
    {
        static char buffer[2] = { 0, 0 };
        buffer[0] = symbol;

        std::strcat(answer, buffer);
        pointer++;
    }
}
