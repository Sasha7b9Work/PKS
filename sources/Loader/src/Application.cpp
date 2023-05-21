// 2023/05/18 13:19:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Application.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace Application
{
    //set Main Stack value
    __asm void MSR_MSP(uint)
    {
#ifndef WIN32
        MSR MSP, r0
            BX r14
#endif
    }
}


void Application::Run()
{
    typedef void (*iapfun)(void);
    iapfun  jump2app;

    jump2app = (iapfun) * (volatile uint *)(HAL_ROM::ADDR_APPLICATION + 4); //-V566
    MSR_MSP(*(volatile uint *)HAL_ROM::ADDR_APPLICATION);                        //initialize app pointer //-V566
    nvic_vector_table_set(HAL_ROM::ADDR_APPLICATION, 0);
    jump2app();                                                                 //jump to app
}

