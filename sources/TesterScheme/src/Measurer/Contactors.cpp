// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Utils/Math.h"
#include "Settings/Settings.h"
#include "Hardware/Timer.h"
#include "Modem/Modem.h"
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
        uint    address;
        bool    enabled;
        uint    time_action;    // Время последнего включения/выключения. Нужно для того, чтобы узнать когда можно считывать его состояние
        void Init(uint port, uint pin);
        void Enable();
        void Disable();
        // Читает "нативное" состояние выводов. p1 в бит 0, p2 в бит 2
        bool IsOK();
    private:
        int ReadNativeState();
    };

    static Contactor contactors[3][10] =
    {
        {{ &pinKMA1, 0  }, { &pinKMA1, 0  }, { &pinKMA2, 1  }, { &pinKMA3, 2  }, { &pinKMA4, 3  }, { &pinKMA5, 4  }, { &pinKMA6, 5  }, { &pinKMA7, 6  }, { &pinKMA8, 7  }, { &pinKMA9, 8  } },
        {{ &pinKMB1, 9  }, { &pinKMB1, 9  }, { &pinKMB2, 10 }, { &pinKMB3, 11 }, { &pinKMB4, 12 }, { &pinKMB5, 13 }, { &pinKMB6, 14 }, { &pinKMB7, 15 }, { &pinKMB8, 16 }, { &pinKMB9, 17 } },
        {{ &pinKMC1, 18 }, { &pinKMC1, 18 }, { &pinKMC2, 19 }, { &pinKMC3, 20 }, { &pinKMC4, 21 }, { &pinKMC5, 22 }, { &pinKMC6, 23 }, { &pinKMC7, 24 }, { &pinKMC8, 25 }, { &pinKMC9, 26 } },
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


    namespace Test
    {
        static int num_step = 0;
        static int num_next = 0;

        static int counter_bads[NUM_PINS_MX];

        // Проверить конкретное реле для всех трёх фаз
        static void VerifyRele(int num_rele);

        int GetCounterBad(Phase::E phase, int num)
        {
            return counter_bads[phase * 9 + num];
        }

        int GetCountSteps()
        {
            return num_step;
        }
    }
}


void Contactors::Test::Update()
{
    if (gset.OnlyMeasure())
    {
        return;
    }

    VerifyRele(num_next++);

    if (num_next == 8)
    {
        num_next = 0;
    }

    num_step++;
}


void Contactors::Test::VerifyRele(int num_rele)
{
    for (int phase = 0; phase < 3; phase++)
    {
        contactors[phase][num_rele].Enable();
    }

    for (int phase = 0; phase < 3; phase++)
    {
        Contactor &contactor = contactors[phase][num_rele];

        if (!contactor.IsOK())
        {
            counter_bads[contactor.address]++;
        }

        contactor.Disable();
    }

    for (int phase = 0; phase < 3; phase++)
    {
        Contactor &contactor = contactors[phase][num_rele];

        if (!contactor.IsOK())
        {
            counter_bads[contactor.address]++;
        }
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


int Contactors::Contactor::ReadNativeState()
{
//    while (TIME_MS < time_action + 5)
//    {
//        Modem::Update();
//    }

    int result = 0;

    if (pinP1.IsHi())
    {
        result |= 1;
    }

    if (pinP2.IsHi())
    {
        result |= 2;
    }

    return result;
}


bool Contactors::Contactor::IsOK()
{
    int state = ReadNativeState();

    return (enabled && state == 1) || (!enabled && state == 2);
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

    for (int phase = 0; phase < 3; phase++)
    {
        for (int rele = 0; rele < 10; rele++)
        {
            contactors[phase][rele].Disable();
        }
    }

    for (int i = 0; i < NUM_PINS_MX; i++)
    {
        Test::counter_bads[i] = 0;
    }
}


void Contactors::Contactor::Init(uint port, uint _pin)
{
    time_action = TIME_MS;

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

        time_action = TIME_MS;
    }
}


void Contactors::Contactor::Disable()
{
    if (enabled)
    {
        enabled = false;

        pin->Reset();

        time_action = TIME_MS;
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
