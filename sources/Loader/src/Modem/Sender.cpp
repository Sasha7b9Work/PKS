// 2023/06/08 16:06:01 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/MQTT/Sender/Sender.h"


void Sender::StateContactors::SendState(uint, int)
{

}


bool Sender::StateContactors::AllIsOK(Phase::E)
{
    return false;
}


void Sender::StateContactors::Send100V(bool)
{

}
