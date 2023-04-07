// 2023/03/30 10:52:20 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


/*
*   Измеритель параметров
*/


#define TIME_MEASURE 150                                        // В миллисекундах
#define NUM_PERIODS 7.5f                                        // Приблизительно периодов в одном измерении
#define NUM_POINTS  2000                                        // Число точек в одном измерении
#define POINTS_ON_PERIOD ((float)NUM_POINTS / NUM_PERIODS)      // Точек на периодв


struct Sample
{
    static const uint16 MAX = 4094;
    static const uint16 ZERO = 2047;
    static const uint16 MIN = 0;

    Sample(uint16 _rel) : rel(_rel) {}
    float ToVoltage() const;
    float ToCurrent() const;
    float VoltsInSample() const { return 380.0f / (float)ZERO; }
    float AmpersInSample() const { return 10.0f / (float)ZERO; }

private:

    uint16 rel;
};


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
