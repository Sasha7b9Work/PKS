// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Log.h"
#include "Hardware/HAL/HAL.h"
#include <cstdarg>
#include <cstdio>
#include <cstring>


using namespace std;


namespace Log
{
    static const int SIZE_BUFFER = 1024;
    static int pointer = 0;
    char buffer[SIZE_BUFFER];
}


void Log::Write(char *format, ...)
{
    char message[100];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

#ifdef SOFTWARE_LOG

    int size = (int)strlen(message) + 1;

    if (pointer + size < SIZE_BUFFER)
    {
        memcpy(buffer + pointer, message, (uint)size);
        pointer += size;
    }

#else

    HAL_USART_LOG::Transmit(message);

#endif
}
