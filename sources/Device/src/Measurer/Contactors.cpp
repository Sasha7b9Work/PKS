// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/Timer.h"


namespace Contactors
{
    enum E
    {
        MX0,
        MX1,
        MX2,
        MX3,
        MX4,
        KMA1,
        KMA2,
        KMA3,
        KMA4,
        KMA5,
        KMA6,
        KMA7,
        KMA8,
        KMA9,
        KMB1,
        KMB2,
        KMB3,
        KMB4,
        KMB5,
        KMB6,
        KMB7,
        KMB8,
        KMB9,
        KMC1,
        KMC2,
        KMC3,
        KMC4,
        KMC5,
        KMC6,
        KMC7,
        KMC8,
        KMC9,
        Cout
    };

    static PinOUT *pins[Cout] =
    {
        &pinOutMX0,
        &pinOutMX1,
        &pinOutMX2,
        &pinOutMX3,
        &pinOutMX4,
        &pinOutKMA1,
        &pinOutKMA2,
        &pinOutKMA3,
        &pinOutKMA4,
        &pinOutKMA5,
        &pinOutKMA6,
        &pinOutKMA7,
        &pinOutKMA8,
        &pinOutKMA9,
        &pinOutKMB1,
        &pinOutKMB2,
        &pinOutKMB3,
        &pinOutKMB4,
        &pinOutKMB5,
        &pinOutKMB6,
        &pinOutKMB7,
        &pinOutKMB8,
        &pinOutKMB9,
        &pinOutKMC1,
        &pinOutKMC2,
        &pinOutKMC3,
        &pinOutKMC4,
        &pinOutKMC5,
        &pinOutKMC6,
        &pinOutKMC7,
        &pinOutKMC8,
        &pinOutKMC9
    };

    struct PinOUT *pinsKM[3][10] =
    {
        {nullptr, &pinOutKMA1, &pinOutKMA2, &pinOutKMA3, &pinOutKMA4, &pinOutKMA5, &pinOutKMA6, &pinOutKMA7, &pinOutKMA8, &pinOutKMA9},
        {nullptr, &pinOutKMB1, &pinOutKMB2, &pinOutKMB3, &pinOutKMB4, &pinOutKMB5, &pinOutKMB6, &pinOutKMB7, &pinOutKMB8, &pinOutKMB9},
        {nullptr, &pinOutKMC1, &pinOutKMC2, &pinOutKMC3, &pinOutKMC4, &pinOutKMC5, &pinOutKMC6, &pinOutKMC7, &pinOutKMC8, &pinOutKMC9}
    };

    struct PinIN *pinsInKM[3][10] =
    {
        {nullptr, &pinInKMA1, &pinInKMA2, &pinInKMA3, &pinInKMA4, &pinInKMA5, &pinInKMA6, &pinInKMA7, &pinInKMA8, &pinInKMA9},
        {nullptr, &pinInKMB1, &pinInKMB2, &pinInKMB3, &pinInKMB4, &pinInKMB5, &pinInKMB6, &pinInKMB7, &pinInKMB8, &pinInKMB9},
        {nullptr, &pinInKMC1, &pinInKMC2, &pinInKMC3, &pinInKMC4, &pinInKMC5, &pinInKMC6, &pinInKMC7, &pinInKMC8, &pinInKMC9}
    };

    static void UpdatePhase(Phase::E, const PhaseMeasure &);

    // Состояние контакторов
    namespace Stage
    {
        static const int LESS_190 = -4;
        static const int LESS_200 = -3;
        static const int LESS_210 = -2;
        static const int LESS_220 = -1;
        static const int TRANSIT = 0;
        static const int ABOVE_240 = 1;
        static const int ABOVE_250 = 2;
        static const int ABOVE_260 = 3;
        static const int ABOVE_270 = 4;

        static const int MIN = LESS_190;
        static const int MAX = ABOVE_270;

        static int current[3] = { 0, 0, 0 };

        static void Set(Phase::E, int stage);

        // Изменить состояние контакторов на delta шагов
        static void Change(Phase::E, int delta);
    }

    static void Enable(int contactor, Phase::E);

    static void Disable(int contactor, Phase::E);
}


void Contactors::Init()
{
    for (int i = 0; i < Cout; i++)
    {
        pins[i]->Init();

        pins[i]->SetState(true);
    }

    for (int phase = 0; phase < 3; phase++)
    {
        Stage::Set((Phase::E)phase, 0);
    }
}


void Contactors::Update(const FullMeasure &measure)
{
    for (int i = 0; i < 3; i++)
    {
        UpdatePhase((Phase::E)i, measure.measures[i]);
    }
}


void Contactors::UpdatePhase(Phase::E phase, const PhaseMeasure &measure)
{
    if (measure.voltage < 220.0f)
    {
        float delta = 220.0f - measure.voltage;

        int num_steps = (int)(delta / 10.0f + 1.0f);

        Stage::Change(phase, -num_steps);
    }

    if (measure.voltage > 240.0f)
    {
        float delta = measure.voltage - 240.0f;

        int num_steps = (int)(delta / 10.0f + 1.0f);

        Stage::Change(phase, num_steps);
    }
}


void Contactors::Stage::Change(Phase::E phase, int delta)
{
    int new_state = (int)current[phase] + delta;

    if (new_state < MIN)
    {
        new_state = MIN;
    }
    else if (new_state >= MAX)
    {
        new_state = MAX;
    }

    Set(phase, new_state);
}


void Contactors::Stage::Set(Phase::E phase, int new_state)
{
    if (new_state == current[phase])
    {
        return;
    }

    current[phase] = new_state;

    Enable(2, phase);
    Enable(3, phase);
    Disable(1, phase);
    Disable(2, phase);
    TimeMeterMS().Wait(5000);
    Disable(3, phase);

    if (new_state == TRANSIT)
    {
        Disable(4, phase);
        Disable(5, phase);
        Disable(6, phase);
        Disable(7, phase);

        return;
    }

    if (new_state == -4 || new_state == 4)
    {
        Enable(4, phase);
        Enable(5, phase);
        Enable(6, phase);
        Enable(7, phase);
        Enable(8, phase);
    }
    else if (new_state == -3 || new_state == 3)
    {
        Disable(4, phase);
        Enable(5, phase);
        Enable(6, phase);
        Enable(7, phase);
    }
    else if (new_state == -2 || new_state == 2)
    {
        Disable(4, phase);
        Disable(5, phase);
        Enable(6, phase);
        Enable(7, phase);
    }
    else if (new_state == -1 || new_state == 1)
    {
        Disable(4, phase);
        Disable(5, phase);
        Disable(6, phase);
        Disable(7, phase);
    }

    new_state > 0 ? Disable(8, phase) : Enable(8, phase);

    Enable(2, phase);
    Enable(3, phase);
    Enable(1, phase);
    Disable(2, phase);
    TimeMeterMS().Wait(5000);
    Disable(3, phase);
}


void Contactors::Enable(int, Phase::E)
{

}


void Contactors::Disable(int, Phase::E)
{

}
