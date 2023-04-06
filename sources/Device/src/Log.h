// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define LOG_WRITE(...)      Log::Write(__VA_ARGS__)


namespace Log
{
    void Write(char *format, ...);
};
