// 2023/06/06 10:36:10 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Globals.h"


namespace GL
{
    bool now_enabled = true;

    uint _RCU_RSTSCK = 0;
}


pchar GL::Firmware::Address()
{
    static char buffer[64];

    return buffer;
}


pchar GL::Firmware::Directory()
{
    static char buffer[64];

    return buffer;
}


pchar GL::Firmware::FileName()
{
    static char buffer[64];

    return buffer;
}


pchar GL::Firmware::Login()
{
    static char buffer[64];

    return buffer;
}


pchar GL::Firmware::Password()
{
    static char buffer[64];

    return buffer;
}
