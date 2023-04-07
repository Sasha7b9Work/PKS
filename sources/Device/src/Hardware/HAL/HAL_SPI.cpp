// 2023/04/07 16:39:59 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>

/*
*   SPI2    Alternate
*   SCK     89  PB3
*   MISO    90  PB4
*   MOSI    91  PB5
*/

void HAL_SPI::Init()
{
    // SCK, MOSI
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_5);

    // MISO
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_4);

    spi_parameter_struct is;

    is.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    is.device_mode = SPI_MASTER;
    is.frame_size = SPI_FRAMESIZE_8BIT;
    is.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    is.nss = SPI_NSS_SOFT;
    is.prescale = SPI_PSC_256;
    is.endian = SPI_ENDIAN_MSB;
    spi_init(SPI_ADDR, &is);
}
