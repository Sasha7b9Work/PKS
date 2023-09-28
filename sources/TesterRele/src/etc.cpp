// 2023/06/19 17:07:30 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Modem/SIM800.h"
#include "Modem/MQTT/_Sender/_Sender.h"
#include "Modem/MQTT/MQTT.h"


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


bool MQTT::InStateRunning()
{
    return false;
}


pchar SIM800::LevelSignal()
{
    return "";
}


bool SIM800::IsRegistered()
{
    return false;
}


void Sender::LevelContactors::Send(const int [Phase::Count])
{

}


bool Sender::StateContactors::AllIsOK(Phase::E)
{
    return false;
}


void Sender::StateContactors::Send100V(bool)
{

}


void Sender::StateContactors::SendState(uint, int)
{

}


void Sender::Measure::Send(const FullMeasure &)
{

}


int Sender::LevelContactors::Get(Phase::E)
{
    return 0;
}


int Sender::StateContactors::Get(int, int)
{
    return 0;
}
