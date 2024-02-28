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
    if (HAL::IsLayout())
    {
        return 20;
    }

    int result = *((int *)(HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS) + 4)); //-V566

    return (result <= 0) ? 16 : result;
}


int Settings::GetNumberSteps()
{
    if (HAL::IsLayout())
    {
        return 5;
    }

    int result = *((int *)HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS)); //-V566

    return (result < 0) ? 4 : result;
}


bool Settings::OnlyMeasure()
{
    if (HAL::IsLayout())
    {
        return false;
    }

    bool result = (*((int *)HAL_ROM::AddressPage(HAL_ROM::PAGE_SETTINGS))) < 0;

    return result;
}
