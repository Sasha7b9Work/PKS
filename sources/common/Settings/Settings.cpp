// 2023/06/22 14:00:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"


static Settings def_set =
{
};


Settings gset = def_set;


int Settings::GetKoeffCurrent()
{
    int result = *((int *)(HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS) + 4)); //-V566

    return (result <= 0) ? 16 : result;
}


int Settings::GetNumberSteps()
{
    int result = *((int *)HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS)); //-V566

    return (result < 0) ? 4 : result;
}


bool Settings::OnlyMeasure()
{
    bool result = (*((int *)HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS))) < 0;

    return result;
}
