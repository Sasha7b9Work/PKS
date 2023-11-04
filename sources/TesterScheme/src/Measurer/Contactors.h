// 2023/04/09 14:54:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Contactors
{
    void Init();

    void Update();

    namespace Serviceability
    {
        // Проверить исправность каждого контактора
        // Здесь же проверяется исправность датчика 100 В - у него индекс 27
        void GetStates(int states[NUM_PINS_MX]);

        int GetState(Phase::E, int);

        void Update();

        bool AllIsOK(Phase::E);
    }
}
