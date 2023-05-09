// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"


namespace Contactors
{
    static const uint TIME_WAIT_BIG = 5000;
    static const uint TIME_WAIT_SMALL = 100;

    struct State
    {
        static Phase::E phase;
        static PinOUT *pin;

        enum E
        {
            IDLE
        };
    };

    static State::E state = State::IDLE;

    struct Contactor
    {
        PinOUT *pin;
        bool    enabled;
        void Init();
        void Enable();
        void Disable();
    };

    static Contactor contactors[3][10] =
    {
        {{ &pinKMA1 }, { &pinKMA1 }, { &pinKMA2 }, { &pinKMA3 }, { &pinKMA4 }, { &pinKMA5 }, { &pinKMA6 }, { &pinKMA7 }, { &pinKMA8 }, { &pinKMA9 } },
        {{ &pinKMB1 }, { &pinKMB1 }, { &pinKMB2 }, { &pinKMB3 }, { &pinKMB4 }, { &pinKMB5 }, { &pinKMB6 }, { &pinKMB7 }, { &pinKMB8 }, { &pinKMB9 } },
        {{ &pinKMC1 }, { &pinKMC1 }, { &pinKMC2 }, { &pinKMC3 }, { &pinKMC4 }, { &pinKMC5 }, { &pinKMC6 }, { &pinKMC7 }, { &pinKMC8 }, { &pinKMC9 } },
    };

    // Состояние контакторов
    namespace Stage
    {
        static const int LESS_190 = -4;
//        static const int LESS_200 = -3;
//        static const int LESS_210 = -2;
//        static const int LESS_220 = -1;
        static const int TRANSIT = 0;
//        static const int ABOVE_240 = 1;
//        static const int ABOVE_250 = 2;
//        static const int ABOVE_260 = 3;
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

    static void UpdatePhase(Phase::E, const PhaseMeasure &);
}


void Contactors::Init()
{
    for (int phase = 0; phase < 3; phase++)
    {
        for (int i = 1; i < 10; i++)
        {
            contactors[phase][i].Init();
        }
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
    static int new_state = 0;
    static float delta = 0.0f;
    static int num_steps = 0;

    switch (state)
    {
    case State::IDLE:
        if (measure.voltage < 220.0f)
        {
            delta = 220.0f - measure.voltage;

            num_steps = -(int)(delta / 10.0f + 1.0f);
        }
        else if (measure.voltage > 240.0f)
        {
            delta = measure.voltage - 240.0f;

            num_steps = (int)(delta / 10.0f + 1.0f);
        }
        else
        {
            break;
        }

        HAL_TIM1::Disable();

        new_state = Math::Limitation(Stage::current[phase] + num_steps, Stage::MIN, Stage::MAX);

        if (new_state == Stage::current[phase])
        {
            break;
        }

        Stage::current[phase] = new_state;

        break;
    }
}


void Contactors::Stage::Change(Phase::E phase, int delta)
{
    HAL_TIM1::Disable();

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

    HAL_TIM1::Enable();
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
    TimeMeterMS().Wait(TIME_WAIT_BIG);
    Disable(3, phase);

    if (new_state == TRANSIT)
    {
        Disable(4, phase);
        Disable(5, phase);
        Disable(6, phase);
        Disable(7, phase);
        Disable(8, phase);

        return;
    }

    if (new_state == -4 || new_state == 4)
    {
        Enable(4, phase);
        Enable(5, phase);
        Enable(6, phase);
        Enable(7, phase);
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
    TimeMeterMS().Wait(TIME_WAIT_BIG);
    Disable(3, phase);
}


void Contactors::Enable(int num, Phase::E phase)
{
    contactors[phase][num].Enable();
}


void Contactors::Disable(int num, Phase::E phase)
{
    contactors[phase][num].Disable();
}


void Contactors::Contactor::Init()
{
    pin->Init();

    enabled = false;

    pin->Set();
}


void Contactors::Contactor::Enable()
{
    if (!enabled)
    {
        enabled = true;

        pin->Reset();

        TimeMeterMS().Wait(100);
    }
}


void Contactors::Contactor::Disable()
{
    if (enabled)
    {
        enabled = false;

        pin->Set();

        TimeMeterMS().Wait(100);
    }
}


void Contactors::CallbackOnTimerVerivy()
{

}
