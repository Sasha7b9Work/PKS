// 2023/04/05 10:13:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Application.h"
#include <gd32f30x.h>


//    HAL::Init();
//
//    for (int page = 50; page < 100; page++)
//    {
//        if (!HAL_ROM::PageIsEmpty(page))
//        {
//            HAL_ROM::ErasePage(page - 50);
//
//            uint addr_write = HAL_ROM::ADDR_BASE + (page - 50) * HAL_ROM::SIZE_PAGE;
//
//            uint8 *data = (uint8 *)HAL_ROM::ADDR_SAVED_FIRMWARE + page * HAL_ROM::SIZE_PAGE; //-V566
//
//            HAL_ROM::WriteData(addr_write, data, HAL_ROM::SIZE_PAGE);
//        }
//    }



int main()
{
    Application::Run();
}
