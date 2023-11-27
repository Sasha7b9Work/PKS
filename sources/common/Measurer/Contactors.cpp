// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
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
            RELE_4,
            RELE_5,
            RELE_6,
            RELE_7,
            RELE_8,
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
        void Init(uint port, uint pin);
        void Enable();
        void Disable();
    };

    static Contactor contactors[3][10] =
    {
        {{ &pinKMA1 }, { &pinKMA1 }, { &pinKMA2 }, { &pinKMA3 }, { &pinKMA4 }, { &pinKMA5 }, { &pinKMA6 }, { &pinKMA7 }, { &pinKMA8 }, { &pinKMA9 } },
        {{ &pinKMB1 }, { &pinKMB1 }, { &pinKMB2 }, { &pinKMB3 }, { &pinKMB4 }, { &pinKMB5 }, { &pinKMB6 }, { &pinKMB7 }, { &pinKMB8 }, { &pinKMB9 } },
        {{ &pinKMC1 }, { &pinKMC1 }, { &pinKMC2 }, { &pinKMC3 }, { &pinKMC4 }, { &pinKMC5 }, { &pinKMC6 }, { &pinKMC7 }, { &pinKMC8 }, { &pinKMC9 } },
    };

    static Contactor &GetContactor(uint address)
    {
        if (address < 9)
        {
            return contactors[0][address + 1];
        }
        else if (address < 18)
        {
            return contactors[1][address - 8];
        }

        return contactors[2][address - 17];
    }

    // Состояние контакторов
    namespace Level
    {
        static int LESS_X()
        {
            return -gset.GetNumberSteps();
        }

        static int ABOVE_X()
        {
            return gset.GetNumberSteps();
        }

        static int MIN()
        {
            return LESS_X();
        }

        static int MAX()
        {
            return ABOVE_X();
        }

        // Номер включённой ступени
        static int levels[Phase::Count] = { 0, 0, 0 };
    }

    static void Enable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void Disable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void UpdatePhase(Phase::E, const PhaseMeasure &, bool good);

    // Возвращаемое значение true означает, что фаза находится в режиме перелючения. Измерения по ней производить нельзя
    bool IsBusy(Phase::E phase);

    // Устанавливает адрес на линиях MX
    static void SetAddressMX(uint);

    // Возвращает состояние реле, выбранного установленным ранее адресом по SetAddressMX()
    static int StateRele(uint address);

    // Возвращает true, если реле по адресу address находится в состоянии переключения (нельзя замерять)
    static bool ReleIsBusy(uint address);

    namespace Serviceability
    {
        static int states[NUM_PINS_MX] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0 };

        void GetStates(int _states[NUM_PINS_MX])
        {
            for (int i = 0; i < NUM_PINS_MX; i++)
            {
                _states[i] = states[i];
            }
        }

        int GetState(Phase::E phase, int num)
        {
            return states[phase * 9 + num];
        }

        bool AllIsOK(Phase::E phase)
        {
            int *state = &states[phase * 9];

            for (int i = 0; i < 8; i++)
            {
                if (*state == -1)
                {
                    return false;
                }

                state++;
            }

            return true;
        }
    }
}


bool Contactors::IsBusy(Phase::E phase)
{
    return State::current[phase] != State::IDLE;
}


void Contactors::Init()
{
    if (gset.OnlyMeasure())
    {
        return;
    }

    uint ports[3][10] =
    {
        //0    1      2      3      4      5      6      7      8      9
        {0,  GPIOE, GPIOE, GPIOE, GPIOB, GPIOB, GPIOB, GPIOC, GPIOE, GPIOE},
        {0,  GPIOE, GPIOE, GPIOE, GPIOE, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB},
        {0,  GPIOB, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD}
    };

    uint pins[3][10] =
    {
        //0          1            2            3            4            5            6            7            8            9
        {0, GPIO_PIN_9,  GPIO_PIN_8,  GPIO_PIN_7,  GPIO_PIN_2,  GPIO_PIN_1,  GPIO_PIN_0,  GPIO_PIN_5,  GPIO_PIN_10, GPIO_PIN_11},
        {0, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14},
        {0, GPIO_PIN_15, GPIO_PIN_8,  GPIO_PIN_9,  GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15}
    };

    for (int phase = 0; phase < 3; phase++)
    {
        for (int i = 1; i < 10; i++)
        {
            contactors[phase][i].Init(ports[phase][i], pins[phase][i]);
        }
    }

    if (gset.OnlyMeasure())
    {
        pinMX0._Init(GPIOB, GPIO_PIN_0);
        pinMX1._Init(GPIOB, GPIO_PIN_0);
        pinMX2._Init(GPIOB, GPIO_PIN_0);
        pinMX3._Init(GPIOB, GPIO_PIN_0);
        pinMX4._Init(GPIOB, GPIO_PIN_0);
    }
    else
    {
        pinMX0._Init(GPIOA, GPIO_PIN_8);
        pinMX1._Init(GPIOA, GPIO_PIN_9);
        pinMX2._Init(GPIOA, GPIO_PIN_10);
        pinMX3._Init(GPIOA, GPIO_PIN_11);
        pinMX4._Init(GPIOA, GPIO_PIN_12);
    }

    pinP1._Init(GPIOE, GPIO_PIN_4, GPIO_MODE_IPU);
    pinP2._Init(GPIOE, GPIO_PIN_3, GPIO_MODE_IPU);
}


void Contactors::Update(const FullMeasure &measure)
{
    if (gset.OnlyMeasure())
    {
        return;
    }

    for (int i = 0; i < 3; i++)
    {
        UpdatePhase((Phase::E)i, measure.measures[i], measure.is_good[i]);
    }
}


void Contactors::UpdatePhase(Phase::E phase, const PhaseMeasure &measure, bool is_good)
{
#define ENABLE_RELE(num, state)  { Enable((num), (phase), (state), (meter[phase])); }
#define DISABLE_RELE(num, state) { Disable((num), (phase), (state), (meter[phase])); }

#define CHANGE_RELE(num, state, enabled) if(enabled) ENABLE_RELE((num), (state)) else DISABLE_RELE((num), (state))

#define WAIT_ENABLE_RELE(num, state)  if(meter[phase].IsFinished()) { ENABLE_RELE((num), (state)); }
#define WAIT_DISABLE_RELE(num, state) if(meter[phase].IsFinished()) { DISABLE_RELE((num), (state)); }

    static TimeMeterMS meter[3];
    static int st[3] = { 0, 0, 0 };

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

    switch (State::current[phase])
    {
    case State::IDLE:
        {
            if (!is_good)
            {
                break;
            }

            int new_level = 0;

            if (!Contactors::Serviceability::AllIsOK(phase))                            // Если хотя бы один контактор на фазе неисправен
            {
                new_level = 0;

                for (int i = 0; i < 10; i++)
                {
                    contactors[phase][i].Disable();
                }

                return;
            }
            else
            {
                float inU = measure.voltage + (float)Level::levels[phase] * 10.0f;

                if (inU < 155.0f)
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

                    new_level = Math::Limitation(Level::levels[phase] + num_steps, Level::MIN(), Level::MAX());
                }
            }
    
            if (new_level == Level::levels[phase])
            {
                break;
            }
    
            Level::levels[phase] = new_level;

            ENABLE_RELE(2, State::TRANSIT_EN_1);
        }

        break;

    case State::TRANSIT_EN_1:   WAIT_ENABLE_RELE(3, State::TRANSIT_EN_2);       break; //-V525
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
            st[phase] = Level::levels[phase] > 0 ? Level::levels[phase] : -Level::levels[phase];

            CHANGE_RELE(1, State::RELE_4, states[st[phase]][0]);   // KM1

            meter[phase].SetResponseTime(1000);
        }
        break;

    case State::RELE_4:
        if (meter[phase].IsFinished())
        {
            if (gset.GetNumberSteps() == 5)
            {
                CHANGE_RELE(4, State::RELE_5, states[st[phase]][1]);   // KM4
            }
            else
            {
                State::current[phase] = State::RELE_5;
            }
        }
        break;

    case State::RELE_5:
        if (meter[phase].IsFinished()) { CHANGE_RELE(5, State::RELE_6, states[st[phase]][2]);   // KM5 //-V525
        }                                                                               break;

    case State::RELE_6:
        if (meter[phase].IsFinished()) { CHANGE_RELE(6, State::RELE_7, states[st[phase]][3]);   // KM6
        }                                                                               break;

    case State::RELE_7:
        if (meter[phase].IsFinished()) { CHANGE_RELE(7, State::RELE_8, states[st[phase]][4]);   // KM7
        }                                                                               break;

    case State::RELE_8:
        if (meter[phase].IsFinished()) { CHANGE_RELE(8, State::POLARITY_LEVEL, states[st[phase]][4]);   // KM8
        }                                                                               break;

    case State::POLARITY_LEVEL:
        if (meter[phase].IsFinished())
        {
            if (Level::levels[phase] == 0)
            {
                State::current[phase] = State::IDLE;
            }
            else
            {
                if (Level::levels[phase] > 0)               // Идём в понижение
                {
                    ENABLE_RELE(9, State::TRANSIT_EXIT_1);
                }
                else                                        // Идём в повшение
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
}


void Contactors::GetLevels(int levels[Phase::Count])
{
    for (int i = 0; i < Phase::Count; i++)
    {
        levels[i] = Level::levels[i];
    }
}


int Contactors::GetLevel(Phase::E phase)
{
    return Level::levels[phase];
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


void Contactors::Contactor::Init(uint port, uint _pin)
{
    pin->_Init(port, _pin);

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


void Contactors::Serviceability::Update()
{
    static uint address = 0;
    static bool first = true;

    if (first)
    {
        first = false;
        SetAddressMX(address);
    }
    else
    {
        if (!ReleIsBusy(address))
        {
            if (address == 27)
            {
                states[address] = !pinP2.IsHi() ? 1 : 0;
            }
            else
            {
                states[address] = StateRele(address);

                if (address == 8 || address == 17 || address == 26)  // Адреса 9-х реле
                {
                    states[address] = 0;
                }
                if (gset.GetNumberSteps() == 4)
                {
                    if (address == 3 || address == 12 || address == 21)     // Для 4-х ступенчатого варианта 4-е реле не опрашиваем
                    {
                        states[address] = 0;
                    }
                }
            }
        }

        address++;

        if (address == 28)
        {
            address = 0;
        }

        SetAddressMX(address == 27 ? 31 : address);
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


int Contactors::StateRele(uint address)
{
    bool p1 = pinP1.IsHi();
    bool p2 = pinP2.IsHi();

    if ((p1 && p2) || (!p1 && !p2)) //-V728
    {
        return -1;
    }

    if (p1 && !GetContactor(address).enabled)
    {
        return -1;
    }

    if (p2 && GetContactor(address).enabled)
    {
        return -1;
    }

    if (p1)
    {
        return 0;
    }

    return 1;
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


void Contactors::Test()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 200)
    {
        return;
    }

    meter.Reset();

    for (int i = 0; i < 3; i++)
    {
        for (int j = 1; j < 10; j++)
        {
            Contactor &contactor = contactors[i][j];

            if (contactor.enabled)
            {
                contactor.Disable();
            }
            else
            {
                contactor.Enable();
            }
        }
    }
}
