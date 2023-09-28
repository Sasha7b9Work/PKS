// 2023/09/28 08:52:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/MQTT/_Sender/_Sender.h"
#include <gd32f30x.h>


struct ObservedPin : public PinIN
{
    ObservedPin() : PinIN(), state(false) { }

    bool IsHi()
    {
        prev_state = state;
        state = PinIN::IsHi();
        return state;
    }

    bool IsSwitched() const
    {
        return state != prev_state;
    }

    void ResetSwitch()
    {
        prev_state = state;
    }

    bool GetState() const
    {
        return state;
    }

private:
    bool state;
    bool prev_state;
};


static ObservedPin pinsGP[3];

void HAL_PINS_GP::Init()
{
    uint pins[3] = { GPIO_PIN_2, GPIO_PIN_1, GPIO_PIN_2 };

    for (int i = 0; i < 3; i++)
    {
        pinsGP[i]._Init(GPIOC, pins[i], GPIO_MODE_IPU);
    }

    pinLevel4._Init(GPIOC, GPIO_PIN_9, GPIO_MODE_IPU);
}


void HAL_PINS_GP::Update()
{
#ifdef DEVICE
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].IsHi();

        if (pinsGP[i].IsSwitched())
        {
            Sender::GP::Send(i + 1, !pinsGP[i].GetState());

            pinsGP[i].ResetSwitch();
        }
    }
#endif
}


void HAL_PINS_GP::SendState()
{
#ifdef DEVICE
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].IsHi();

        Sender::GP::Send(i + 1, !pinsGP[i].GetState());

        pinsGP[i].ResetSwitch();
    }
#endif
}
