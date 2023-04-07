// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef DEV_BOARD
#define LOG_WRITE(...)      Log::Write(__VA_ARGS__)
#else
#define LOG_WRITE(...)
#endif


namespace Log
{
    void Write(char *format, ...);
};
