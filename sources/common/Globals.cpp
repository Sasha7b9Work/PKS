// 2023/06/06 10:36:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Globals.h"
#include "Utils/StringUtils.h"
#include "Hardware/HAL/HAL.h"
#include <cstring>


namespace GL
{
    bool now_enabled = true;

    uint _RCU_RSTSCK = 0;
}


bool GL::Firmware::IsExist()
{
    uint8 *pointer = (uint8 *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA;
    const uint8 * const end = pointer + 128;

    while (pointer < end)
    {
        if (*pointer == '\0')
        {
            return true;
        }

        pointer++;
    }

    return false;
}


pchar GL::Firmware::Address()
{
    static char buffer[64];

    SU::GetString((char *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA, 1, buffer);

    return buffer;
}


pchar GL::Firmware::Directory()
{
    static char directory[64];

    SU::GetString((char *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA, 4, directory);

    return directory;
}


pchar GL::Firmware::FileName()
{
    static char name[64];

    SU::GetString((char *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA, 5, name);

    return name;
}


pchar GL::Firmware::Login()
{
    static char buffer[64];

    SU::GetString((char *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA, 2, buffer);

    return buffer;
}


pchar GL::Firmware::Password()
{
    static char buffer[64];

    SU::GetString((char *)HAL_ROM::ADRR_PAGE_UPGRADE_DATA, 3, buffer);

    return buffer;
}
