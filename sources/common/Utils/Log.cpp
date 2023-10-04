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
    static const int SIZE_BUFFER = 8192;
    static int pointer = 0;
    static int counter = 0;
}


char log_buffer[Log::SIZE_BUFFER];


void Log::Init()
{
    memset(log_buffer, 0, SIZE_BUFFER);
    pointer = 0;
}


void Log::ReceiveFromSIM800(char)
{
#ifdef SOFTWARE_LOG

    if (pointer >= SIZE_BUFFER)
    {
        Init();
    }

    log_buffer[pointer++] = symbol;

#endif

//    HAL_USART_LOG::Transmit(symbol);
}


void Log::Write(char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    int size = (int)strlen(message) + 1;

    if (pointer + size >= SIZE_BUFFER - 100)
    {
        Init();
    }

#ifdef SOFTWARE_LOG

    memcpy(log_buffer + pointer, message, (uint)size);
    pointer += size;

#endif

    char buffer[256];

    std::sprintf(buffer, "%d : %s", counter++, message); //-V512

    HAL_USART_LOG::Transmit(buffer);
}


void Log::WriteTrace(char *file, int line, char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[512];

    std::sprintf(buffer, "%d : %s     %s:%d", counter++, message, file, line);

    HAL_USART_LOG::Transmit(buffer);
}


void Log::Error(char *file, int line, char *format, ...)
{
    char message[256];

    std::va_list args;
    va_start(args, format);
    std::vsprintf(message, format, args);
    va_end(args);

    char buffer[512];

    std::sprintf(buffer, "%d : %s     %s:%d                                !!!!!!!!!!! ERRROR !!!!!!!!!!", counter++, message, file, line);

    HAL_USART_LOG::Transmit(buffer);

}
