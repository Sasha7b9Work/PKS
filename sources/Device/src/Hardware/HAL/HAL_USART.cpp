// 2023/04/06 13:08:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_USART::Init()
{
    pinUSART_TX.Init();
    pinUSART_RX.Init();

    usart_interrupt_enable(USART_ADDR, USART_INT_TBE);
}
