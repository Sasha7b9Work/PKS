// 2023/06/22 14:00:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Settings/Settings.h"


static Settings def_set =
{
};


Settings gset = def_set;


float Settings::GetKoeffCurrent()
{
    return 30.0f;
}


bool Settings::IsControllingRelays()
{
    return false;
}
