// 2024/01/16 14:39:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"


pchar SCPI::NET::Net(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}
