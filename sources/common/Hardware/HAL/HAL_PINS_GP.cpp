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
        state = PinIN::IsHi();
        return state;
    }

    bool GetState() const
    {
        return state;
    }

private:
    bool state;
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


#ifdef DEVICE
void HAL_PINS_GP::Update(bool is_hi[Phase::Count])
{
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].IsHi();

        is_hi[i] = !pinsGP[i].GetState();
    }
}
#endif

