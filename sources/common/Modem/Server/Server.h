// 2024/01/17 08:18:33 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Server
{
    void Update(pchar answer);

    bool IsConnected();

    void Reset();

    void Send(char *format, ...);

    void SendUnsupportedCommand();
}
