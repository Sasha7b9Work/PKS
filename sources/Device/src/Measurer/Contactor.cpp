// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactor.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"


namespace Contactor
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
    struct State
    {
        enum E
        {
            Less190,    // Менее 190 В
            Less200,
            Less210,
            Less220,
            Normal,
            Over240,
            Over250,
            Over260,
            Over270,    // Более 270 В
            Count
        };
    };

    static State::E state[3] = { State::Normal, State::Normal, State::Normal };

    static void SetState(Phase::E, State::E);

    // Изменить состояние контакторов на delta шагов
    static void ChangeState(Phase::E, int delta);
}


void Contactor::Init()
{
    for (int i = 0; i < Cout; i++)
    {
        pins[i]->Init();
    }
}


void Contactor::Update(const FullMeasure &measure)
{
    for (int i = 0; i < 3; i++)
    {
        UpdatePhase((Phase::E)i, measure.measures[i]);
    }
}


void Contactor::UpdatePhase(Phase::E phase, const PhaseMeasure &measure)
{
    if (measure.voltage < 220.0f)
    {
        float delta = 220.0f - measure.voltage;

        int num_steps = (int)(delta / 10.0f + 1.0f);

        ChangeState(phase, -num_steps);
    }

    if (measure.voltage > 240.0f)
    {
        float delta = measure.voltage - 240.0f;

        int num_steps = (int)(delta / 10.0f + 1.0f);

        ChangeState(phase, num_steps);
    }
}


void Contactor::ChangeState(Phase::E phase, int delta)
{
    int new_state = (int)state[phase] + delta;

    if (new_state < 0)
    {
        new_state = 0;
    }
    else if (new_state >= State::Count)
    {
        new_state = (int)(State::Count - 1);
    }

    SetState(phase, (State::E)new_state);
}


void Contactor::SetState(Phase::E phase, State::E new_state)
{
    if (new_state == state[phase])
    {
        return;
    }

    state[phase] = new_state;

    if (new_state == State::Less190)
    {

    }
    else if (new_state == State::Less200)
    {

    }
    else if (new_state == State::Less210)
    {

    }
    else if (new_state == State::Less220)
    {

    }
    else if (new_state == State::Normal)
    {

    }
    else if (new_state == State::Over240)
    {

    }
    else if (new_state == State::Over250)
    {

    }
    else if (new_state == State::Over260)
    {

    }
    else if (new_state == State::Over270)
    {

    }
}
