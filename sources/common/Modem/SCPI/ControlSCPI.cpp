// 2024/01/16 14:36:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Modem/Server/Server.h"


namespace SCPI
{
    namespace CONTROL
    {
        static pchar Auto(pchar);
        static pchar Step(pchar);
        static pchar Volt(pchar);
        static pchar Boost(pchar);
        static pchar Time(pchar);
        static pchar Points(pchar);
        static pchar Range(pchar);
        static pchar Kt(pchar);

        static StructSCPI commands[] =
        {
            { "AUTO",   Auto },
            { "STEP",   Step },
            { "VOLT",   Volt },
            { "BOOST",  Boost },
            { "TIME",   Time },
            { "POINTS", Points },
            { "RANGE",  Range },
            { "KT",     Kt },
            { nullptr, nullptr }
        };
    }
}


pchar SCPI::CONTROL::Set(pchar message)
{
    return SCPI::Process(message, commands);
}


pchar SCPI::CONTROL::Auto(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Step(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Volt(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Boost(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Time(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Points(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Range(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::CONTROL::Kt(pchar)
{
    Server::SendUnsupportedCommand();

    return nullptr;
}
