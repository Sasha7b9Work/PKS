﻿// 2023/04/05 13:04:49 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


#define PAGE_SIZE (2 * 1024)


void HAL_ROM::ErasePage(int num_page)
{
    fmc_unlock();

    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    fmc_page_erase(FLASH_BASE + num_page * PAGE_SIZE);

    fmc_flag_clear(FMC_FLAG_BANK0_END);
    fmc_flag_clear(FMC_FLAG_BANK0_WPERR);
    fmc_flag_clear(FMC_FLAG_BANK0_PGERR);

    fmc_lock();
}


void HAL_ROM::WriteData(uint address, const void *_data, int size)
{
    uint8 *data = (uint8 *)_data;

    fmc_unlock();

    while (size % 4)
    {
        size++;
    }

    uint end = address + size;

    while (address < end)
    {
        fmc_word_program(address, *(uint *)data);
        data += 4;
        address += 4;

        fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
    }

    fmc_lock();
}
