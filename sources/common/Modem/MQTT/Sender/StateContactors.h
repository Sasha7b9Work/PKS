// 2023/05/26 09:08:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Sender
{
    namespace StateContactors
    {
        void Send(const bool state[NUM_PINS_MX]);

        bool SendToSIM800();
    }
}
