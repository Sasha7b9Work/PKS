// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


namespace SIM800
{
    static const uint TIME_WAIT_ANSWER = 1500;
}


namespace Modem
{
    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void CallbackOnReceive(char);

    // После любой необработанной ошибки SIM800 вызывать эту функцию
    void CallbackOnErrorSIM800();

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
