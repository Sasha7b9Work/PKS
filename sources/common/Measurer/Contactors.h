// 2023/04/09 14:54:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace Contactors
{
    void Init();

    void Update(const FullMeasure &);

    namespace Serviceability
    {
        // Проверить исправность каждого контактора
        // Здесь же проверяется исправность датчика 100 В
        void Verify();
    }

    void Test();
}
