// 2023/05/29 15:29:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/SIM800.h"


namespace Sender
{
    void Reset();

    void ResetMeter();

    void Update(pchar answer);
}
