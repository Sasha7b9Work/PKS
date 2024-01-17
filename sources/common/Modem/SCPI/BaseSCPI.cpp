// 2024/01/16 14:37:43 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Modem/Server/Server.h"


pchar SCPI::BASE::Ver(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Addr(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Com(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Time(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Reset(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Wdt(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Timer(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::BASE::Task(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}
