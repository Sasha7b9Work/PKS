// 2024/01/16 14:39:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"


namespace SCPI
{
    namespace NET
    {
        static pchar Empty(pchar);
        static pchar All(pchar);
        static pchar Auto(pchar);
        static pchar Cur(pchar);
        static pchar Cos(pchar);
        static pchar C(pchar);
        static pchar I(pchar);
        static pchar Va(pchar);
        static pchar Ver(pchar);
        static pchar Vin(pchar);
        static pchar V(pchar);
        static pchar Reset(pchar);

        static StructSCPI commands[] =
        {
            { "",      Empty },
            { "ALL",   All },
            { "AUTO",  Auto },
            { "CUR",   Cur },
            { "COS",   Cos },
            { "C",     C },
            { "I",     I },
            { "VA",    Va },
            { "VER",   Ver },
            { "VIN",   Vin },
            { "V",     V },
            { "RESET", Reset },
            { nullptr, nullptr }
        };
    }
}


pchar SCPI::NET::Net(pchar message)
{
    return SCPI::Process(message, commands);
}


pchar SCPI::NET::Empty(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::All(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Auto(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Cur(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Cos(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::C(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::I(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Va(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Ver(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Vin(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::V(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}


pchar SCPI::NET::Reset(pchar)
{
    SendUnsupportedCommand();

    return nullptr;
}
