// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Измеритель параметров
*/


#define NUM_POINTS 2000
#define POINTS_ON_PERIOD (2000 / 6)


struct PhaseMeasure
{
    float current;
    float voltage;
    float power;
};


struct FullMeasure
{
    PhaseMeasure measures[3];
};


namespace Measurer
{
    void Update(void);

    void AppendMeasures(uint16 adc_values[6]);

    // Не ноль, если буферы заполнены
    bool BuffersFull(void);

    // Возвращает не ноль, если готово новое измерение
    bool MeasureReady(void);

    FullMeasure GetMeasure(void);
}
