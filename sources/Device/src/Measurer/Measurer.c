// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Measurer/Calculator.h"


static void Measurer_Calculate(void);

static struct FullMeasure measure;

static int measure_ready = 0;               // Не ноль, если измерение готово. Устанавливается в 0 после считывания

static uint16 currentA[NUM_POINTS];
static uint16 currentB[NUM_POINTS];
static uint16 currentC[NUM_POINTS];

static uint16 voltA[NUM_POINTS];
static uint16 voltB[NUM_POINTS];
static uint16 voltC[NUM_POINTS];

static int16 pos_adc_value = 0;             // Позиция текущих считываемых значений


static void CalculatePower(struct PhaseMeasure *);


void Measurer_Update()
{
    if (Measurer_BuffersFull())
    {
        Measurer_Calculate();

        measure_ready = 1;
    }
}


void Measurer_AppendMeasures(uint16 adc_values[6])
{
    if (pos_adc_value < NUM_POINTS)
    {
        currentA[pos_adc_value] = adc_values[0];
        currentB[pos_adc_value] = adc_values[1];
        currentC[pos_adc_value] = adc_values[2];

        voltA[pos_adc_value] = adc_values[3];
        voltB[pos_adc_value] = adc_values[4];
        voltC[pos_adc_value] = adc_values[5];

        pos_adc_value++;
    }
}


int Measurer_BuffersFull()
{
    return pos_adc_value >= NUM_POINTS;
}


static void CalculatePower(struct PhaseMeasure *meas)
{
    meas->power = meas->current * meas->voltage;
}


static void Measurer_Calculate()
{
    measure.measures[0].current = Calculator_CalculateCurrentRMS(currentA);
    measure.measures[1].current = Calculator_CalculateCurrentRMS(currentB);
    measure.measures[2].current = Calculator_CalculateCurrentRMS(currentC);

    measure.measures[0].voltage = Calculator_CalculateVoltageRMS(voltA);
    measure.measures[1].voltage = Calculator_CalculateVoltageRMS(voltB);
    measure.measures[2].voltage = Calculator_CalculateVoltageRMS(voltC);

    CalculatePower(&measure.measures[0]);
    CalculatePower(&measure.measures[1]);
    CalculatePower(&measure.measures[2]);
}


int Measurer_MeasureReady()
{
    return measure_ready;
}


struct FullMeasure Measurer_GetMeasure()
{
    measure_ready = 0;

    return measure;
}
