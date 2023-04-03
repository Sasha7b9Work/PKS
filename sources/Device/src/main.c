#include "defines.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
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
    systick_config();
    
    HAL_Init();
    
    Display_Init();

    while(1)
    {
        Display_Update();
    }
}
