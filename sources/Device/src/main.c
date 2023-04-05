#include "defines.h"
#include "Device.h"



#define MAIN_PROGRAM_START_ADDRESS  0x08032000

int main(void)
{  
//    Device_Init();

    __disable_irq();
    typedef void(*pFunction)();
    // Теперь переходим на основную программу
    pFunction JumpToApplication;
    JumpToApplication = (pFunction)(MAIN_PROGRAM_START_ADDRESS + 4); //-V2571
//    set_MSP(*(__IO uint *)MAIN_PROGRAM_START_ADDRESS);
    __enable_irq();
    JumpToApplication();


    while(1)
    {
        Device_Update();
    }
}
