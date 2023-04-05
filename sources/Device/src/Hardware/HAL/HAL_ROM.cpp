// 2023/04/05 13:04:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_ROM::EraseSector(int)
{
    fmc_unlock();
}


void HAL_ROM::WriteData(uint, uint8 *, int)
{

}
