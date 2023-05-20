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

    namespace Send
    {
        void Measure(const FullMeasure &);

        void GP(int num, bool state);

        void StateContactors(const bool[NUM_PINS_MX]);

        void LevelContactors(int [Phase::Count]);
    }

    namespace Mode
    {
        // Возвращает true, если напряжение подано
        bool Power();

        // Возвращает true, если зарегистрирован в сети
        bool Registered();

        // Возвращает true, если подключён к MQTT
        bool ConnectedToMQTT();

        // Уровень сигнала от 0 до 10
        pchar LevelSignal();
    }
}
