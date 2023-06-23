// 2023/03/30 11:03:58 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Measurer.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/Sender/Measure.h"
#include <cmath>


namespace Contactors
{
    bool IsBusy(Phase::E phase);
}


namespace Measurer
{
    static struct FullMeasure measure;
    static struct FullMeasure measure5Sec;
    static struct FullMeasure measure1Min;


    static Sample voltA[NUM_SAMPLES];
    static Sample voltB[NUM_SAMPLES];
    static Sample voltC[NUM_SAMPLES];

    static Sample currentA[NUM_SAMPLES];
    static Sample currentB[NUM_SAMPLES];
    static Sample currentC[NUM_SAMPLES];

    // Сюда пишем true, если в начале чтения данных контакторы находятся в неустановившемся положении
    static bool bad_in_begin[Phase::Count] = { false, false, false };

    static int16 pos_adc_value = 0;             // Позиция текущих считываемых значений

    static FullMeasure Calculate();
}


namespace Calculator
{
    FullMeasure Average5Sec(const FullMeasure &);

    FullMeasure Average1Min(const FullMeasure &);
}


void Measurer::Update()
{
    if (BuffersFull())
    {
        measure = Calculate();

#ifdef DEVICE
        Sender::Measure::Send(measure);
#endif

        measure5Sec = Calculator::Average5Sec(measure);

        measure1Min = Calculator::Average1Min(measure);

        for (int i = 0; i < Phase::Count; i++)
        {
            bad_in_begin[i] = Contactors::IsBusy((Phase::E)i);
        }

        pos_adc_value = 0;
    }
}


FullMeasure Measurer::LastMeasure()
{
    return measure;
}


FullMeasure Measurer::Measure5Sec()
{
    return measure5Sec;
}


FullMeasure Measurer::Measure1Min()
{
    return measure1Min;
}


void Measurer::AppendMeasures(uint16 adc_values[6])
{
    static TimeMeterMS meter;

    if (pos_adc_value == 0)
    {
        meter.Reset();
    }

    if (pos_adc_value < NUM_SAMPLES)
    {
        voltA [pos_adc_value] = adc_values[0];
        voltB [pos_adc_value] = adc_values[1];
        voltC [pos_adc_value] = adc_values[2];

        currentA[pos_adc_value] = adc_values[3];
        currentB[pos_adc_value] = adc_values[4];
        currentC[pos_adc_value] = adc_values[5];

        pos_adc_value++;

        if (pos_adc_value == NUM_SAMPLES)
        {
#ifdef EMULATE
            Generator::GenerateVoltage(voltA);
            Generator::GenerateCurrent(currentA);

            Generator::GenerateVoltage(voltB);
            Generator::GenerateCurrent(currentB);

            Generator::GenerateVoltage(voltC);
            Generator::GenerateCurrent(currentC);
#endif

//            LOG_WRITE("time read measure %d ms", meter.ElapsedTime());
        }
    }
}


bool Measurer::BuffersFull()
{
    return pos_adc_value >= NUM_SAMPLES;
}


FullMeasure Measurer::Calculate()
{
    bool is_bad[Phase::Count] = { Contactors::IsBusy(Phase::A), Contactors::IsBusy(Phase::B), Contactors::IsBusy(Phase::C) };

    FullMeasure result;

    result.measures[0].Calculate(voltA, currentA);
    result.measures[1].Calculate(voltB, currentB);
    result.measures[2].Calculate(voltC, currentC);

    for (int i = 0; i < Phase::Count; i++)
    {
        result.is_good[i] = (!is_bad[i] && !bad_in_begin[i]);
        if (!result.is_good[i])
        {
            result.measures[i] = measure.measures[i];
        }
    }

    return result;
}


float Sample::ToVoltage() const
{
    // Vadc_p-p = Vrms_in / 5000 * 16 * 2 * sqrt(2)

    return ((float)rel - (float)ZERO) * VoltsInSample();
}


float Sample::ToCurrent() const
{
    return ((float)rel - (float)ZERO) * AmpersInSample();
}


float Sample::AmplitudeCurrent() const
{
    return 5.0f * 2.0f;
}


float Sample::AmplitudeVoltage() const
{
    // Vadc_max = Vin_max / 5000 * 16 * 2
    // Vadc_max * 5000 / 16 / 2 = Vin_max
    // Vin_max = Vadc_max * 5000 / 16 / 2 = 3.3 * 5000 / 16 / 2
    return 515.625f * 2.0f;
}


void Sample::FromVoltage(float level)
{
    rel = (uint16)(level / VoltsInSample() + ZERO);
}


void Sample::FromCurrent(float level)
{
    rel = (uint16)(level / AmpersInSample() + ZERO);
}
