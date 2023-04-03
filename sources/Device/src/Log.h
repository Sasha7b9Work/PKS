// 2023/03/30 11:33:25 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define LOG_WRITE(...)      Log_Write(__VA_ARGS__)
#define LOG_ERROR(...)      Log_Error(__VA_ARGS__)


void Log_Write(pchar, ...);
void Log_Error(pchar, ...);
