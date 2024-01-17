// 2024/01/16 14:38:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Modem/Server/Server.h"


pchar SCPI::MODEM::Gprs(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::MODEM::Server(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::MODEM::Modem(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}
