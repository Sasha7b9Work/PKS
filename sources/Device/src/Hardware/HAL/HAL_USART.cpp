// 2023/04/06 13:08:24 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include <gd32f30x.h>
#include <cstring>


void HAL_USART_GPRS::Init()
{
    pinUSART_GPRS_TX.Init();
    pinUSART_GPRS_RX.Init();

//    gpio_pin_remap_config(GPIO_USART1_REMAP, ENABLE);

    nvic_irq_enable(UART3_IRQn, 0, 0);

    usart_deinit(USART_GPRS_ADDR);
    usart_baudrate_set(USART_GPRS_ADDR, 9600);
    usart_receive_config(USART_GPRS_ADDR, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART_GPRS_ADDR, USART_TRANSMIT_ENABLE);

    usart_interrupt_enable(USART_GPRS_ADDR, USART_INT_RBNE);

    usart_enable(USART_GPRS_ADDR);
}


void HAL_USART_GPRS::Transmit(pchar message)
{
    int size = (int)std::strlen(message);

    for (int i = 0; i < size; i++)
    {
        usart_data_transmit(USART_GPRS_ADDR, (uint16)message[i]);

        while (RESET == usart_flag_get(USART_GPRS_ADDR, USART_FLAG_TBE)) { };
    }
}


void HAL_USART_GPRS::CallbackOnReceive(char symbol)
{
    Modem::CallbackOnReceive(symbol);
}
