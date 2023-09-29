// 2023/09/28 08:56:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Storage/Storage.h"


namespace Sender
{
    void CallbackOnReceiveChar(char);

    void Reset();

    // Послать значение счётчика
    bool SendCounter(int);

    bool SendVersion();

    bool SendMeasures(const Measurements &);
}
