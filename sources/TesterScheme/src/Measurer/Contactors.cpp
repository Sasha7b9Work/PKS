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


void Contactors::Update()
{
    if (gset.OnlyMeasure())
    {
        return;
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

    if (p1 && GetContactor(address).enabled)
    {
        return -1;
    }

    if (p2 && !GetContactor(address).enabled)
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
