// 2023/04/06 13:08:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include <gd32f30x.h>
#include <cstring>


void HAL_USART::Init()
{
    pinUSART_TX.Init();
    pinUSART_RX.Init();

    gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);

    nvic_irq_enable(USART1_IRQn, 0, 0);

    usart_deinit(USART_ADDR);
    usart_baudrate_set(USART_ADDR, 9600);
    usart_receive_config(USART_ADDR, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART_ADDR, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(USART_ADDR, USART_INT_RBNE);

    usart_enable(USART_ADDR);
}


void HAL_USART::Transmit(pchar message)
{
    int size = (int)std::strlen(message);

    for (int i = 0; i < size; i++)
    {
        while (RESET == usart_flag_get(USART_ADDR, USART_FLAG_TBE)) { }

        usart_data_transmit(USART_ADDR, (uint16)message[i]);
    }
}


void HAL_USART::CallbackOnReceive(char symbol)
{
    Modem::CallbackOnReceive(symbol);
}
