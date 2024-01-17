// 2023/06/19 17:07:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Modem/SIM800.h"


void Modem::Update()
{

}


bool Modem::Mode::Power()
{
    return false;
}


void Modem::CallbackOnReceive(char)
{

}


pchar SIM800::LevelSignal()
{
    return "";
}


bool SIM800::IsRegistered()
{
    return false;
}
