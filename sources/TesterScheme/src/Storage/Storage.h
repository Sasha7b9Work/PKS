// 2023/09/14 11:48:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Measurer.h"


struct Measurements
{
    Measurements();

    uint counter;

    uint16 volts[Phase::Count];     // Здесь хранятся значения, увеличенные в 10 раз
    uint16 currents[Phase::Count];  // Здесь хранятся значения, увеличенные в 10 раз

    int bads[NUM_PINS_MX];

    void SetFullMeasure(const FullMeasure &);
    FullMeasure GetFullMeasure() const;

    int GetBad(Phase::E, int num) const;
};


struct MeasurementsReady
{
    bool volts[Phase::Count];
    bool currents[Phase::Count];

    bool km[Phase::Count][9];
    bool _100V;
    bool gp[Phase::Count];
    bool stage[Phase::Count];

    MeasurementsReady();
};


namespace Storage
{
    void Init();

    void Update();

    void Reset();
}
