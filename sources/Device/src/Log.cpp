// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include <cstdarg>
#include <cstdio>


void Log::Write(char *format, ...)
{
    char message[100];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    HAL_USART_LOG::Transmit(message);
}
