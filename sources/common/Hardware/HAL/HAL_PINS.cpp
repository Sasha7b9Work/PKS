// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Modem/MQTT/Sender/GP.h"
#include <gd32f30x.h>
#include <cstdlib>


PinADC pinVolt1(GPIOA, GPIO_PIN_0, ADC_CHANNEL_0);
PinADC pinVolt2(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);
PinADC pinVolt3(GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);

PinADC pinCur1L(GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
PinADC pinCur2L(GPIOA, GPIO_PIN_5, ADC_CHANNEL_5);
PinADC pinCur3L(GPIOA, GPIO_PIN_7, ADC_CHANNEL_7);

PinADC pinCur1H(GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
PinADC pinCur2H(GPIOA, GPIO_PIN_6, ADC_CHANNEL_6);
PinADC pinCur3H(GPIOC, GPIO_PIN_4, ADC_CHANNEL_14);

const uint I2C_ADDR = I2C0;
PinI2C pinI2C_SCL(GPIOB, GPIO_PIN_8);
PinI2C pinI2C_SDA(GPIOB, GPIO_PIN_9);

const uint USART_GPRS_ADDR = UART3;
PinUSART_TX pinUSART_GPRS_TX(GPIOC, GPIO_PIN_10);
PinUSART_RX pinUSART_GPRS_RX(GPIOC, GPIO_PIN_11);

const uint USART_LOG_ADDR = USART1;
PinUSART_TX pinUSART_LOG_TX(GPIOB, GPIO_PIN_6);
PinUSART_RX pinUSART_LOG_RX(GPIOB, GPIO_PIN_7);

PinOUT pinGSM_PWR(GPIOA, GPIO_PIN_12);
PinOUT pinGSM_PWRKEY(GPIOA, GPIO_PIN_11);
PinIN  pinGSM_STATUS(GPIOA, GPIO_PIN_10);

PinOUT pinMX0(GPIOB, GPIO_PIN_0);
PinOUT pinMX1(GPIOB, GPIO_PIN_0);
PinOUT pinMX2(GPIOB, GPIO_PIN_0);
PinOUT pinMX3(GPIOB, GPIO_PIN_0);
PinOUT pinMX4(GPIOB, GPIO_PIN_0);

PinIN pinP1(GPIOE, GPIO_PIN_4);
PinIN pinP2(GPIOE, GPIO_PIN_3);

PinOUT pinKMA1(GPIOE, GPIO_PIN_0);
PinOUT pinKMA2(GPIOE, GPIO_PIN_0);
PinOUT pinKMA3(GPIOE, GPIO_PIN_0);
PinOUT pinKMA4(GPIOE, GPIO_PIN_0);
PinOUT pinKMA5(GPIOE, GPIO_PIN_0);
PinOUT pinKMA6(GPIOE, GPIO_PIN_0);
PinOUT pinKMA7(GPIOE, GPIO_PIN_0);
PinOUT pinKMA8(GPIOE, GPIO_PIN_0);
PinOUT pinKMA9(GPIOE, GPIO_PIN_0);
PinOUT pinKMB1(GPIOE, GPIO_PIN_0);
PinOUT pinKMB2(GPIOE, GPIO_PIN_0);
PinOUT pinKMB3(GPIOE, GPIO_PIN_0);
PinOUT pinKMB4(GPIOE, GPIO_PIN_0);
PinOUT pinKMB5(GPIOE, GPIO_PIN_0);
PinOUT pinKMB6(GPIOE, GPIO_PIN_0);
PinOUT pinKMB7(GPIOE, GPIO_PIN_0);
PinOUT pinKMB8(GPIOE, GPIO_PIN_0);
PinOUT pinKMB9(GPIOE, GPIO_PIN_0);
PinOUT pinKMC1(GPIOE, GPIO_PIN_0);
PinOUT pinKMC2(GPIOE, GPIO_PIN_0);
PinOUT pinKMC3(GPIOE, GPIO_PIN_0);
PinOUT pinKMC4(GPIOE, GPIO_PIN_0);
PinOUT pinKMC5(GPIOE, GPIO_PIN_0);
PinOUT pinKMC6(GPIOE, GPIO_PIN_0);
PinOUT pinKMC7(GPIOE, GPIO_PIN_0);
PinOUT pinKMC8(GPIOE, GPIO_PIN_0);
PinOUT pinKMC9(GPIOE, GPIO_PIN_0);

PinIN pinLevel4(GPIOC, GPIO_PIN_9);


struct ObservedPin : public PinIN
{
    ObservedPin(uint _port, uint _pin) : PinIN(_port, _pin), state(false) { }

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


static ObservedPin pinsGP[3] =
{
    ObservedPin(GPIOC, GPIO_PIN_2),
    ObservedPin(GPIOC, GPIO_PIN_1),
    ObservedPin(GPIOC, GPIO_PIN_0)
};


void HAL_PINS::Init()
{
    for (int i = 0; i < 3; i++)
    {
        pinsGP[i].Init(GPIO_MODE_IPU);
    }

    pinLevel4.Init(GPIO_MODE_IPU);
}


void HAL_PINS::Update()
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


void HAL_PINS::SendState()
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


void PinADC::Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}


void PinI2C::Init()
{
    gpio_pin_remap_config(GPIO_I2C0_REMAP, ENABLE);
    gpio_init(port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_TX::Init()
{
    gpio_init(port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_RX::Init()
{
    gpio_init(port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Init()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Set()
{
    GPIO_BOP(port) = pin;
}


void PinOUT::Reset()
{
    GPIO_BC(port) = pin;
}


void PinOUT::SetState(bool state)
{
    state ? Set() : Reset();
}


void PinIN::Init(uint input_mode)
{
    gpio_init(port, input_mode, GPIO_OSPEED_50MHZ, pin);
}


void PinIN::DeInit()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, pin);

    // ��������� � ����
    GPIO_BC(port) = pin;
}


bool PinIN::IsLow()
{
    return gpio_input_bit_get(port, pin) == RESET;
}


bool PinIN::IsHi()
{
    return !IsLow();
}
