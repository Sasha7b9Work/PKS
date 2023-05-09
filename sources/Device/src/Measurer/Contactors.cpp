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
            IDLE,
            TRANSIT_EN_1,
            TRANSIT_EN_2,
            TRANSIT_EN_3,
            TRANSIT_EN_4,
            TRANSIT_EN_5,
            TRANSIT_EN_6,
            TRANSIT_DIS_1,
            TRANSIT_DIS_2,
            TRANSIT_DIS_3,
            TRANSIT_DIS_4,
            TRANSIT_DIS_5,
            STAGE_4_1,
            STAGE_4_2,
            STAGE_4_3,
            STAGE_3_1,
            STAGE_3_2,
            STAGE_3_3,
            STAGE_2_1,
            STAGE_2_2,
            STAGE_2_3,
            STAGE_1_1,
            STAGE_1_2,
            STAGE_1_3,
            POLARITY_STAGE
        };

        static E current[3];
    };

    State::E State::current[3] = { State::IDLE, State::IDLE, State::IDLE };

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
    namespace Level
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

    static void Enable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void Disable(int contactor, Phase::E, State::E next, TimeMeterMS &);

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
    static TimeMeterMS meter[3];

    switch (State::current[phase])
    {
    case State::IDLE:
        {
            int new_stage = 0;
            float delta = 0.0f;
            int num_steps = 0;
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
    
            new_stage = Math::Limitation(Level::current[phase] + num_steps, Level::MIN, Level::MAX);
    
            if (new_stage == Level::current[phase])
            {
                break;
            }
    
            Level::current[phase] = new_stage;

            Measurer::DisableMeasure(phase);
    
            Enable(2, phase, State::TRANSIT_EN_1, meter[phase]);
        }
        break;

#define DISABLE_RELE(num, state)    if (meter[phase].IsWorked())        Enable(num, phase, state, meter[phase]);        break;

    case State::TRANSIT_EN_1:       DISABLE_RELE(3, State::TRANSIT_EN_2);
    case State::TRANSIT_EN_2:       DISABLE_RELE(1, State::TRANSIT_EN_3);
    case State::TRANSIT_EN_3:
        if (meter[phase].IsWorked())         Disable(2, phase, State::TRANSIT_EN_3, meter[phase]);      break;

    case State::TRANSIT_EN_4:
        if (meter[phase].IsWorked()) { meter[phase].SetResponseTime(TIME_WAIT_BIG); State::current[phase] = State::TRANSIT_EN_5; } break;

    case State::TRANSIT_EN_5:
        if (meter[phase].IsWorked())  Disable(3, phase, State::TRANSIT_EN_6, meter[phase]);             break;

    case State::TRANSIT_EN_6:
        if (Level::current[phase] == Level::TRANSIT)
        {
            Disable(4, phase, State::TRANSIT_DIS_1, meter[phase]);
        }
        if (Level::current[phase] == -4 || Level::current[phase] == 4)
        {
            Enable(4, phase, State::STAGE_4_1, meter[phase]);
        }
        else if (Level::current[phase] == -3 || Level::current[phase] == 3)
        {
            Disable(4, phase, State::STAGE_3_1, meter[phase]);
        }
        else if (Level::current[phase] == -2 || Level::current[phase] == 2)
        {
            Disable(4, phase, State::STAGE_2_1, meter[phase]);
        }
        else if (Level::current[phase] == -1 || Level::current[phase] == 1)
        {
            Disable(4, phase, State::STAGE_1_1, meter[phase]);
        }

        State::current[phase] = State::POLARITY_STAGE;

        break;

    case State::TRANSIT_DIS_1:
        if (meter[phase].IsWorked())    Disable(5, phase, State::TRANSIT_DIS_2, meter[phase]);    break;

    case State::TRANSIT_DIS_2:
        if (meter[phase].IsWorked())     Disable(6, phase, State::TRANSIT_DIS_3, meter[phase]);     break;

    case State::TRANSIT_DIS_3:
        if (meter[phase].IsWorked())     Disable(7, phase, State::TRANSIT_DIS_4, meter[phase]);      break;

    case State::TRANSIT_DIS_4:
        if (meter[phase].IsWorked())  Disable(8, phase, State::TRANSIT_DIS_5, meter[phase]);        break;

    case State::TRANSIT_DIS_5:

    case State::STAGE_4_1:

        break;

    case State::STAGE_4_2:

        break;
    }
}


void Contactors::Level::Change(Phase::E phase, int delta)
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


void Contactors::Level::Set(Phase::E phase, int new_state)
{
    if (new_state == current[phase])
    {
        return;
    }

    current[phase] = new_state;

    /*
    Enable(2, phase);
    Enable(3, phase);                       // TRANSIT_EN_1
    Disable(1, phase);                      // TRANSIT_EN_2
    Disable(2, phase);                      // TRANSIT_EN_3
    TimeMeterMS().Wait(TIME_WAIT_BIG);      // TRANSIT_EN_4
    Disable(3, phase);                      // TRANSIT_EN_5

    if (new_state == TRANSIT)               // TRANSIT_EN_6
    {
        Disable(4, phase);
        Disable(5, phase);                  // TRANSIT_DIS_1
        Disable(6, phase);                  // TRANSIT_DIS_2
        Disable(7, phase);                  // TRANSIT_DIS_3
        Disable(8, phase);                  // TRANSIT_DIS_4

        return;                             // TRANSIT_DIS_5
    }

    if (new_state == -4 || new_state == 4)
    {
        Enable(4, phase);
        Enable(5, phase);                   // STAGE_4_1
        Enable(6, phase);                   // STAGE_4_2
        Enable(7, phase);                   // STAGE_4_3
    }
    else if (new_state == -3 || new_state == 3)
    {
        Disable(4, phase);
        Enable(5, phase);                   // STAGE_3_1
        Enable(6, phase);                   // STAGE_3_2
        Enable(7, phase);                   // STAGE_3_3
    }
    else if (new_state == -2 || new_state == 2)
    {
        Disable(4, phase);
        Disable(5, phase);                  // STAGE_2_1
        Enable(6, phase);                   // STAGE_2_2
        Enable(7, phase);                   // STAGE_2_3
    }
    else if (new_state == -1 || new_state == 1)
    {
        Disable(4, phase);
        Disable(5, phase);                  // STAGE_1_1
        Disable(6, phase);                  // STAGE_1_2
        Disable(7, phase);                  // STAGE_1_3
    }

    new_state > 0 ? Disable(8, phase) : Enable(8, phase);       // POLARITY_STAGE

    Enable(2, phase);
    Enable(3, phase);
    Enable(1, phase);
    Disable(2, phase);
    TimeMeterMS().Wait(TIME_WAIT_BIG);
    Disable(3, phase);
    */
}


void Contactors::Enable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    contactors[phase][num].Enable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
}


void Contactors::Disable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    contactors[phase][num].Disable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
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
    }
}


void Contactors::Contactor::Disable()
{
    if (enabled)
    {
        enabled = false;

        pin->Set();
    }
}


void Contactors::CallbackOnTimerVerivy()
{

}
