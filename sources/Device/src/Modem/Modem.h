// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


#define MODEM_RESET()  Modem::Reset(__FUNCTION__, __LINE__)


namespace Modem
{
    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void CallbackOnReceive(char);

    // После любой необработанной ошибки SIM800 вызывать эту функцию
    void Reset(pchar file, int line);

    namespace Mode
    {
        // Возвращает true, если напряжение подано
        bool Power();

        // Возвращает true, если зарегистрирован в сети
        bool Registered();

        // Уровень сигнала от 0 до 10
        pchar LevelSignal();
    }
}
