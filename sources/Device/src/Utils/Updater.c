// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Updater.h"
#include "Modem/Modem.h"


#define FLASH_APP1_ADDR  0x08032000


typedef  void (*iapfun)(void);
iapfun      jump2app;


#ifdef WIN32
    #define __asm
#endif

__asm void MSR_MSP(uint
#ifndef WIN32
addr
#endif
)
{
#ifndef WIN32
    MSR MSP, r0
    BX r14
#endif
}//set Main Stack value


static void JumpToApplication()
{
    jump2app = (iapfun) * (volatile uint *)(FLASH_APP1_ADDR + 4);       //the second address of app code is the program
    MSR_MSP(*(volatile uint *)FLASH_APP1_ADDR);                         //initialize app pointer
    jump2app();                                                         //jump to app
}


static void Update()
{
    JumpToApplication();
}


void Updater_Update()
{
    if (Modem_ExistUpdate())
    {
        Update();
    }
}
