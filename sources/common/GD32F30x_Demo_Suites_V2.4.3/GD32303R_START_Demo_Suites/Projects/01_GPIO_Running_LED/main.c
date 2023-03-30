/*!
    \file    main.c
    \brief   systick LED demo

    \version 2022-06-18, V2.0.0, demo for GD32F30x
*/

/*
    Copyright (c) 2022, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include "gd32f30x.h"
#include "gd32f303r_start.h"
#include "systick.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
    /* enable the LED1,2,3,4 clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* configure LED1,2,3,4 GPIO port */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

    gpio_bit_reset(GPIOA, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);


    systick_config();
    while(1) {
        /* turn on LED1 */
        GPIO_BOP(GPIOA) = GPIO_PIN_7;
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED2 */
        GPIO_BOP(GPIOA) = GPIO_PIN_8;
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED3 */
        GPIO_BOP(GPIOA) = GPIO_PIN_9;
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn on LED4 */
        GPIO_BOP(GPIOA) = GPIO_PIN_10;
        /* insert 200 ms delay */
        delay_1ms(200);

        /* turn off LEDs */
        GPIO_BC(GPIOA) = GPIO_PIN_7;
        GPIO_BC(GPIOA) = GPIO_PIN_8;
        GPIO_BC(GPIOA) = GPIO_PIN_9;
        GPIO_BC(GPIOA) = GPIO_PIN_10;
        /* insert 200 ms delay */
        delay_1ms(200);
    }
}
