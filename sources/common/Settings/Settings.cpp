// 2023/06/22 14:00:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"
#include "Hardware/HAL/HAL.h"


static Settings def_set =
{
};


Settings gset = def_set;


float Settings::GetKoeffCurrent()
{
    bool pin1 = pinLevel1.IsHi();
    bool pin2 = pinLevel2.IsHi();
    bool pin3 = pinLevel3.IsHi();

    if (pin1 && pin2 && pin3)
    {
        return 30.0f;
    }

    if (!pin1 && pin2 && pin3)
    {
        return 20.0f;
    }

    if (!pin1 && !pin2 && pin3)
    {
        return 16.0f;
    }

    if (pin1 && !pin2 && !pin3)
    {
        return 40.0f;
    }

    return 50.0f;
}


bool Settings::IsControllingRelays()
{
    return pinLevel4.IsHi();
}
