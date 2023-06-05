// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Modem/MQTT/Sender/LevelContactors.h"
#include "Modem/MQTT/Sender/ContactorsIsOK.h"
#include <gd32f30x.h>
#include <cstring>
#include <cstdlib>


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

    static Contactor _contactors[3][10] =
    {
        {{ &pinKMA1 }, { &pinKMA1 }, { &pinKMA2 }, { &pinKMA3 }, { &pinKMA4 }, { &pinKMA5 }, { &pinKMA6 }, { &pinKMA7 }, { &pinKMA8 }, { &pinKMA9 } },
        {{ &pinKMB1 }, { &pinKMB1 }, { &pinKMB2 }, { &pinKMB3 }, { &pinKMB4 }, { &pinKMB5 }, { &pinKMB6 }, { &pinKMB7 }, { &pinKMB8 }, { &pinKMB9 } },
        {{ &pinKMC1 }, { &pinKMC1 }, { &pinKMC2 }, { &pinKMC3 }, { &pinKMC4 }, { &pinKMC5 }, { &pinKMC6 }, { &pinKMC7 }, { &pinKMC8 }, { &pinKMC9 } },
    };

    // Состояние контакторов
    namespace Level
    {
#ifdef FOUR_STEPS_VERSION
        static const int LESS_180 = -4;
        static const int ABOVE_280 = 4;
        static const int MIN = LESS_180;
        static const int MAX = ABOVE_280;
#endif

#ifdef FIVE_STEPS_VERSION
        static const int LESS_170 = -5;
        static const int ABOVE_290 = 5;
        static const int MIN = LESS_170;
        static const int MAX = ABOVE_290;
#endif

        static int current[3] = { 0, 0, 0 };
    }

    static void Enable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void Disable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void UpdatePhase(Phase::E, const PhaseMeasure &, bool good);

    // Возвращаемое значение true означает, что фаза находится в режиме перелючения. Измерения по ней производить нельзя
    bool IsBusy(Phase::E phase);

    // Устанавливает адрес на линиях MX
    static void SetAddressMX(uint);

    // Возвращает состояние реле, выбранного установленным ранее адресом по SetAddressMX()
    static bool StateRele();

    // Возвращает true, если реле по адресу address находится в состоянии переключения (нельзя замерять)
    static bool ReleIsBusy(uint address);

    // Сюда накапливаются состояния всех реле, чтобы потом одной строкой отослать неисправные
    static bool state_contactor[NUM_PINS_MX] =
    {
        false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false,
        false
    };
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
            _contactors[phase][i].Init();
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
        UpdatePhase((Phase::E)i, measure.measures[i], measure.is_good[i]);
    }
}


void Contactors::UpdatePhase(Phase::E phase, const PhaseMeasure &measure, bool is_good)
{
#define ENABLE_RELE(num, state)  { Enable(num, phase, state, meter[phase]); }
#define DISABLE_RELE(num, state) { Disable(num, phase, state, meter[phase]); }

#define CHANGE_RELE(num, state, enabled) if(enabled) ENABLE_RELE(num, state) else DISABLE_RELE(num, state)

#define WAIT_ENABLE_RELE(num, state)  if(meter[phase].IsFinished()) { ENABLE_RELE(num, state); }
#define WAIT_DISABLE_RELE(num, state) if(meter[phase].IsFinished()) { DISABLE_RELE(num, state); }

    static TimeMeterMS meter[3];

    switch (State::current[phase])
    {
    case State::IDLE:
        {
            if (!is_good)
            {
                break;
            }
            
            float inU = measure.voltage + (float)Level::current[phase] * 10.0f;
            int new_level = 0;
            
            if (inU < 160.5f)
            {
                new_level = 0;
            }
            else
            {
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

                new_level = Math::Limitation(Level::current[phase] + num_steps, Level::MIN, Level::MAX);
            }
    
            if (new_level == Level::current[phase])
            {
                break;
            }
    
            Level::current[phase] = new_level;

            ENABLE_RELE(2, State::TRANSIT_EN_1);
        }

        break;

    case State::TRANSIT_EN_1:   WAIT_ENABLE_RELE(3, State::TRANSIT_EN_2);       break;
    case State::TRANSIT_EN_2:   WAIT_DISABLE_RELE(1, State::TRANSIT_EN_3);      break;
    case State::TRANSIT_EN_3:   WAIT_DISABLE_RELE(2, State::TRANSIT_EN_4);      break;
    case State::TRANSIT_EN_4:
        if (meter[phase].IsFinished())
        {
            meter[phase].SetResponseTime(TIME_WAIT_BIG);
            State::current[phase] = State::TRANSIT_EN_5;
        }
        break;

    case State::TRANSIT_EN_5:
        if (meter[phase].IsFinished())
        {
            DISABLE_RELE(3, State::TRANSIT_EN_6);
        }
        break;

    case State::TRANSIT_EN_6:
        if (meter[phase].IsFinished())
        {
#ifdef FIVE_STEPS_VERSION
            static const bool states[6][5] =
            {
            //    KM1    KM4    KM5    KM6   KM7,8
                {false, false, false, false, false},    // Транзит
                {true,  false, false, false, false},    // 1
                {true,  false, false, false, true},     // 2
                {true,  false, false, true,  true},     // 3
                {true,  false, true,  true,  true},     // 4
                {true,  true,  true,  true,  true}      // 5
            };

            int st = Level::current[phase] > 0 ? Level::current[phase] : -Level::current[phase];

            CHANGE_RELE(1, State::POLARITY_LEVEL, states[st][0]);
            CHANGE_RELE(4, State::POLARITY_LEVEL, states[st][1]);
            CHANGE_RELE(5, State::POLARITY_LEVEL, states[st][2]);
            CHANGE_RELE(6, State::POLARITY_LEVEL, states[st][3]);
            CHANGE_RELE(7, State::POLARITY_LEVEL, states[st][4]);
            CHANGE_RELE(8, State::POLARITY_LEVEL, states[st][4]);
#endif
#ifdef FOUR_STEPS_VERSION
            static const bool states[6][5] =
            {
                //    KM1    KM4    KM5   KM6,7
                    {false, false, false, false},   // Транзит
                    {true,  false, false, false},   // 1
                    {true,  false, false, true},    // 2
                    {true,  false, true,  true},    // 3
                    {true,  true,  true,  true}     // 4
            };

            int st = Level::current[phase] > 0 ? Level::current[phase] : -Level::current[phase];

            CHANGE_RELE(1, State::POLARITY_LEVEL, states[st][0]);
            CHANGE_RELE(4, State::POLARITY_LEVEL, states[st][1]);
            CHANGE_RELE(5, State::POLARITY_LEVEL, states[st][2]);
            CHANGE_RELE(6, State::POLARITY_LEVEL, states[st][3]);
            CHANGE_RELE(7, State::POLARITY_LEVEL, states[st][3]);
#endif
        }
        break;

    case State::POLARITY_LEVEL:
        if (meter[phase].IsFinished())
        {
            if (Level::current[phase] == 0)
            {
                State::current[phase] = State::IDLE;
            }
            else
            {
                if (Level::current[phase] > 0)
                {
                    ENABLE_RELE(9, State::TRANSIT_EXIT_1);
                }
                else
                {
                    DISABLE_RELE(9, State::TRANSIT_EXIT_1);
                }
            }
        }
        break;
    case State::TRANSIT_EXIT_1:     WAIT_ENABLE_RELE(2, State::TRANSIT_EXIT_2);     break;
    case State::TRANSIT_EXIT_2:     WAIT_ENABLE_RELE(3, State::TRANSIT_EXIT_3);     break;
    case State::TRANSIT_EXIT_3:     WAIT_ENABLE_RELE(1, State::TRANSIT_EXIT_4);     break;
    case State::TRANSIT_EXIT_4:     WAIT_DISABLE_RELE(2, State::TRANSIT_EXIT_5);    break;
    case State::TRANSIT_EXIT_5:
        if (meter[phase].IsFinished())
        {
            meter[phase].SetResponseTime(TIME_WAIT_BIG);
            State::current[phase] = State::TRANSIT_EXIT_6;
        }
        break;
    case State::TRANSIT_EXIT_6:     WAIT_DISABLE_RELE(3, State::END);       break;

    case State::END:
        if (meter[phase].IsFinished())
        {
            State::current[phase] = State::IDLE;
        }
        break;
    }

#ifdef DEVICE
    Sender::LevelContactors::Send(Level::current);
#endif
}


void Contactors::Enable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    _contactors[phase][num].Enable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
}


void Contactors::Disable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    _contactors[phase][num].Disable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
}


void Contactors::Contactor::Init()
{
    pin->Init();

    enabled = false;

    pin->Reset();
}


void Contactors::Contactor::Enable()
{
    if (!enabled)
    {
        enabled = true;

        pin->Set();
    }
}


void Contactors::Contactor::Disable()
{
    if (enabled)
    {
        enabled = false;

        pin->Reset();
    }
}


void Contactors::VerifyServiceability()
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
        if (!ReleIsBusy(address))
        {
            bool state = StateRele();

            state_contactor[address] = state;
        }

        if (address == 27)                          // Был выставлен адрес P2 = 31
        {
            state_contactor[address] = !pinP2.IsHi();
        }

        address = Math::CircularIncrease(address, 0U, (uint)NUM_PINS_MX);

        if (address == 27)                          // 28-й элемент массива - адрес пина P2 для контроля напряжения 100В
        {
            address = 31;
        };

        SetAddressMX(address);

        if (address == 31)
        {
            address = 27;
        }

        if (address == 0)   // Опросили все реле, будем посылать результат
        {
#ifdef DEVICE
            Sender::ContactorsIsOK::Send(state_contactor);
#endif
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

    return ((p1 && !p2) || (!p1 && p2));
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
