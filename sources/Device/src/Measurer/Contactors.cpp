// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Modem/Modem.h"
#include <gd32f30x.h>
#include <cstring>


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
            POLARITY_LEVEL,
            TRANSIT_EXIT_1,
            TRANSIT_EXIT_2,
            TRANSIT_EXIT_3,
            TRANSIT_EXIT_4,
            TRANSIT_EXIT_5,
            TRANSIT_EXIT_6,
            END
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
    }

    static void Enable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void Disable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void UpdatePhase(Phase::E, const PhaseMeasure &);

    // Возвращаемое значение true означает, что фаза находится в режиме перелючения. Измерения по ней производить нельзя
    bool IsBusy(Phase::E phase);

    // Устанавливает адрес на линиях MX
    static void SetAddressMX(uint);

    // Возвращает состояние реле, выбранного установленным ранее адресом по SetAddressMX()
    static bool StateRele();

    // Возвращает true, если реле по адресу address находится в состоянии переключения (нельзя замерять)
    static bool ReleIsBusy(uint address);

    static const int NUM_CONTACTORS = 27;

    // Сюда накапливаются состояния всех реле, чтобы потом одной строкой отослать неисправные
    static bool state_contactor[NUM_CONTACTORS];

    // Отослать состояние всех реле
    static void SendStateRelays();
}


bool Contactors::IsBusy(Phase::E phase)
{
    return State::current[phase] != State::IDLE;
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

    pinMX0.Init();
    pinMX1.Init();
    pinMX2.Init();
    pinMX3.Init();
    pinMX4.Init();

    pinP1.Init(GPIO_MODE_IPU);
    pinP2.Init(GPIO_MODE_IPU);
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
    /*
    if (new_state == current[phase])
    {
        return;
    }
    
    current[phase] = new_state;
    
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
    
    new_state > 0 ? Disable(8, phase) : Enable(8, phase);       // POLARITY_LEVEL
    
    Enable(2, phase);                   // TRANSIT_EXIT_1
    Enable(3, phase);                   // TRANSIT_EXIT_2
    Enable(1, phase);                   // TRANSIT_EXIT_3
    Disable(2, phase);                  // TRANSIT_EXIT_4
    TimeMeterMS().Wait(TIME_WAIT_BIG);  // TRANSIT_EXIT_5
    Disable(3, phase)                   // TRANSIT_EXIT_6
    return;                             // TRANSIT_EXIT_7
    */


#define ENABLE_RELE(num, state)  { Enable(num, phase, state, meter[phase]);  break; }
#define DISABLE_RELE(num, state) { Disable(num, phase, state, meter[phase]); break; }

#define WAIT_ENABLE_RELE(num, state)  if(meter[phase].IsWorked()) { ENABLE_RELE(num, state); }
#define WAIT_DISABLE_RELE(num, state) if(meter[phase].IsWorked()) { DISABLE_RELE(num, state); }

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

            ENABLE_RELE(2, State::TRANSIT_EN_1);
        }

    case State::TRANSIT_EN_1:   WAIT_ENABLE_RELE(3, State::TRANSIT_EN_2);
    case State::TRANSIT_EN_2:   WAIT_DISABLE_RELE(1, State::TRANSIT_EN_3);
    case State::TRANSIT_EN_3:   WAIT_DISABLE_RELE(2, State::TRANSIT_EN_3);
    case State::TRANSIT_EN_4:       
        if (meter[phase].IsWorked()) { meter[phase].SetResponseTime(TIME_WAIT_BIG); State::current[phase] = State::TRANSIT_EN_5; } break;

    case State::TRANSIT_EN_5:
        if (meter[phase].IsWorked()) { DISABLE_RELE(3, State::TRANSIT_EN_6); } break;

    case State::TRANSIT_EN_6:
        if (meter[phase].IsWorked())
        {
            if (Level::current[phase] == Level::TRANSIT) { DISABLE_RELE(4, State::TRANSIT_DIS_1); }
            else if (Level::current[phase] == -4 || Level::current[phase] == 4) { ENABLE_RELE(4, State::STAGE_4_1); }
            else if (Level::current[phase] == -3 || Level::current[phase] == 3) { DISABLE_RELE(4, State::STAGE_3_1); }
            else if (Level::current[phase] == -2 || Level::current[phase] == 2) { DISABLE_RELE(4, State::STAGE_2_1); }
            else if (Level::current[phase] == -1 || Level::current[phase] == 1) { DISABLE_RELE(4, State::STAGE_1_1); }
            else
            {
                State::current[phase] = State::POLARITY_LEVEL;
            }
        }
        break;

    case State::TRANSIT_DIS_1:  WAIT_DISABLE_RELE(5, State::TRANSIT_DIS_2);
    case State::TRANSIT_DIS_2:  WAIT_DISABLE_RELE(6, State::TRANSIT_DIS_3);
    case State::TRANSIT_DIS_3:  WAIT_DISABLE_RELE(7, State::TRANSIT_DIS_4);
    case State::TRANSIT_DIS_4:  WAIT_DISABLE_RELE(8, State::TRANSIT_DIS_5);
    case State::TRANSIT_DIS_5:
        if (meter[phase].IsWorked())  State::current[phase] = State::IDLE;  break;

    case State::STAGE_4_1:      WAIT_ENABLE_RELE(5, State::STAGE_4_2);
    case State::STAGE_4_2:      WAIT_ENABLE_RELE(6, State::STAGE_4_3);
    case State::STAGE_4_3:      WAIT_ENABLE_RELE(7, State::POLARITY_LEVEL);

    case State::STAGE_3_1:      WAIT_DISABLE_RELE(5, State::STAGE_3_2);
    case State::STAGE_3_2:      WAIT_ENABLE_RELE(6, State::STAGE_3_3);
    case State::STAGE_3_3:      WAIT_ENABLE_RELE(7, State::POLARITY_LEVEL);

    case State::STAGE_2_1:      WAIT_DISABLE_RELE(5, State::STAGE_2_2);
    case State::STAGE_2_2:      WAIT_ENABLE_RELE(6, State::STAGE_2_3);
    case State::STAGE_2_3:      WAIT_ENABLE_RELE(7, State::POLARITY_LEVEL);

    case State::STAGE_1_1:      WAIT_DISABLE_RELE(5, State::STAGE_1_2);
    case State::STAGE_1_2:      WAIT_DISABLE_RELE(6, State::STAGE_1_3);
    case State::STAGE_1_3:      WAIT_DISABLE_RELE(7, State::POLARITY_LEVEL);

    case State::POLARITY_LEVEL:
        if (meter[phase].IsWorked())
        {
            if (Level::current[phase] > 0) { DISABLE_RELE(8, State::TRANSIT_EXIT_1) } else { ENABLE_RELE(8, State::TRANSIT_EXIT_1); }
        }
        break;
    case State::TRANSIT_EXIT_1:     WAIT_ENABLE_RELE(2, State::TRANSIT_EXIT_2);
    case State::TRANSIT_EXIT_2:     WAIT_ENABLE_RELE(3, State::TRANSIT_EXIT_3);
    case State::TRANSIT_EXIT_3:     WAIT_ENABLE_RELE(1, State::TRANSIT_EXIT_4);
    case State::TRANSIT_EXIT_4:     WAIT_DISABLE_RELE(2, State::TRANSIT_EXIT_5);
    case State::TRANSIT_EXIT_5:
        if (meter[phase].IsWorked())
        {
            meter[phase].SetResponseTime(TIME_WAIT_BIG);
            State::current[phase] = State::TRANSIT_EXIT_6;
        }
        break;
    case State::TRANSIT_EXIT_6:     WAIT_DISABLE_RELE(3, State::END);

    case State::END:
        if (meter[phase].IsWorked())
        {
            State::current[phase] = State::IDLE;
        }
        break;
    }
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


void Contactors::VerifyCondition()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 1)
    {
        return;
    }

    meter.Reset();

    static bool first = true;

    static uint address = 0;

    if (first)
    {
        SetAddressMX(address);
        first = false;
    }
    else
    {
        state_contactor[address] = ReleIsBusy(address) ? true : StateRele();

        address = Math::CircularIncrease(address, 0U, (uint)NUM_CONTACTORS);

        SetAddressMX(address);

        if (address == 0)   // Опросили все реле, будем посылать результат
        {
            SendStateRelays();
        }
    }
}


void Contactors::SetAddressMX(uint address)
{
    pinMX0.SetState((address & 1) != 0);
    pinMX1.SetState((address & 2) != 0);
    pinMX2.SetState((address & 4) != 0);
    pinMX3.SetState((address & 8) != 0);
    pinMX4.SetState((address & 16) == 0);
}


bool Contactors::StateRele()
{
    bool p1 = pinP1.IsHi();
    bool p2 = pinP2.IsHi();

    return (p1 && !p2) || (!p1 && p2);
}


bool Contactors::ReleIsBusy(uint address)
{
    if (address < 9)
    {
        return Contactors::IsBusy(Phase::A);
    }
    else if (address < 18)
    {
        return Contactors::IsBusy(Phase::B);
    }

    return Contactors::IsBusy(Phase::C);
}


void Contactors::SendStateRelays()
{
    Modem::Send::Contactors(state_contactor);

    return;

    static const char *const names[27] =
    {
        "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9"
        "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
        "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9"
    };

    static const uint SIZE_BUFFER = 32;

    char buffer[SIZE_BUFFER] = { '\0' };

    for (int i = 0; i < NUM_CONTACTORS; i++)
    {
        if (state_contactor[i] == false)
        {
            if (std::strlen(buffer) != 0)
            {
                std::strcat(buffer, " ");
            }
            std::strcat(buffer, names[i]);

            if (std::strlen(buffer) + std::strlen(names[0] + 2) > SIZE_BUFFER)
            {
                i = NUM_CONTACTORS;
            }
        }
    }

    if (std::strlen(buffer) == 0)
    {
        Modem::Send::Contactors(String("Ok"));
    }
    else
    {
        Modem::Send::Contactors(String(buffer));
    }
}
