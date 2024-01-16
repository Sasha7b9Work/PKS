// 2024/01/16 12:03:44 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/SCPI/SCPI.h"
#include "Utils/Buffer.h"


namespace SCPI
{
    static StructSCPI commands[] =
    {
        { "INFO",   INFO::Process },
        { "HELL",   INFO::Process },
        { "PHASE",  INFO::Process },
        { "ERROR",  INFO::Process },
        { nullptr,  nullptr }
    };

    static Buffer<1024> buffer;
}



void SCPI::Append(char symbol)
{
    buffer.Append(symbol);
}


void SCPI::Update()
{

}
