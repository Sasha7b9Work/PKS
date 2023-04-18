// 2023/04/05 10:13:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


static void JumpToApplication();


int main()
{
    HAL::Init();

    for (int page = 50; page < 100; page++)
    {
        if (!HAL_ROM::PageIsEmpty(page))
        {
            HAL_ROM::ErasePage(page - 50);

            uint addr_write = HAL_ROM::ADDR_BASE + (page - 50) * HAL_ROM::SIZE_PAGE;

            uint8 *data = (uint8 *)HAL_ROM::ADDR_SAVED_FIRMWARE + page * HAL_ROM::SIZE_PAGE; //-V566

            HAL_ROM::WriteData(addr_write, data, HAL_ROM::SIZE_PAGE);
        }
    }

    JumpToApplication();
}


//set Main Stack value
__asm void MSR_MSP(uint)
{
#ifndef WIN32
    MSR MSP, r0
        BX r14
#endif
}


static void JumpToApplication()
{
    typedef void (*iapfun)(void);
    iapfun  jump2app;

    jump2app = (iapfun) * (volatile uint *)(HAL_ROM::ADDR_APPLICATION + 4); //-V566
    MSR_MSP(*(volatile uint *)HAL_ROM::ADDR_APPLICATION);                        //initialize app pointer //-V566
    jump2app();                                                                 //jump to app
}
