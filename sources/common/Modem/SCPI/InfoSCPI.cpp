// 2024/01/16 14:38:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Modem/SCPI/InfoSCPI.h"


pchar SCPI::INFO::Info(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::INFO::Hello(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::INFO::Phase(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::INFO::Error(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}
