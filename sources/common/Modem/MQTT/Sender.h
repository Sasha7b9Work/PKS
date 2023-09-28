// 2023/09/28 08:56:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Sender
{
    void CallbackOnReceiveChar(char);

    // Послать значение счётчика
    bool SendCounter(int);

    bool SendVersion();
}
