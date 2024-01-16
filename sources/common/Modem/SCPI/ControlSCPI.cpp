// 2024/01/16 14:36:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"


pchar SCPI::CONTROL::Set(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}
