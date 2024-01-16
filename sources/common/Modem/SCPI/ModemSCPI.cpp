// 2024/01/16 14:38:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"


pchar SCPI::MODEM::Gprs(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::MODEM::Server(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::MODEM::Modem(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}
