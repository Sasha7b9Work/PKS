#include "defines.h"
#include "Display/Display.h"
#include "Hardware/HAL/HAL.h"
#include "gd32f30x.h"
#include "systick.h"


int main(void)
{  
    Display_Init();

    while(1)
    {
        Display_Update();
    }
}
