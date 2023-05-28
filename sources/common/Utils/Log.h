// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#ifdef ENABLE_LOG
#define LOG_WRITE(...)      Log::Write(__VA_ARGS__)
#else
#define LOG_WRITE(...)
#endif


#ifdef SOFTWARE_LOG
    extern char log_buffer[8192];
#endif


namespace Log
{
    void Init();

    void Write(char *format, ...);
};
