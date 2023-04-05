#include "defines.h"
#include "Device.h"


#define FLASH_APP1_ADDR  0x08032000


typedef  void (*iapfun)(void);
iapfun      jump2app;

__asm void MSR_MSP(uint addr)
{
    MSR MSP, r0
    BX r14
}//set Main Stack value

void iap_load_app(uint appxaddr)
{
    jump2app = (iapfun) * (volatile uint *)(appxaddr + 4);      //the second address of app code is the program
    MSR_MSP(*(volatile uint *)appxaddr);                        //initialize app pointer
    jump2app();                                                 //jump to app
}


int main(void)
{
    iap_load_app(FLASH_APP1_ADDR);

    while(1)
    {
        Device_Update();
    }
}
