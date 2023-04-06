// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Измеритель параметров
*/


#define NUM_PERIODS 7.0f
#define NUM_POINTS  2000
#define POINTS_ON_PERIOD ((float)NUM_POINTS / NUM_PERIODS)


struct PhaseMeasure
{
    float current;
    float voltage;
    float power;

    void CalculatePower();
};


struct FullMeasure
{
    PhaseMeasure measures[3];
};


namespace Measurer
{
    void Update();

    void AppendMeasures(uint16 adc_values[6]);

    // Не ноль, если буферы заполнены
    bool BuffersFull();

    // Возвращает не ноль, если готово новое измерение
    bool MeasureReady();

    FullMeasure GetMeasure();
}
