// 2023/03/30 11:33:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define LOG_WRITE(...)      Log::Write(__VA_ARGS__)
#define LOG_ERROR(...)      Log::Error(__VA_ARGS__)


namespace Log
{
    void Write(pchar, ...);
    void Error(pchar, ...);
}
