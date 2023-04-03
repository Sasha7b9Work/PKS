#include "defines.h"
#include "Display/Display.h"
#include "gd32f30x.h"
#include "systick.h"

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{  
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_init(RCU_GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_init(RCU_GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);

    systick_config();
    
    Display_Init();

    while(1)
    {
        Display_Update();
    }
}
