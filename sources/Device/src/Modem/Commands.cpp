// 2023/4/30 17:48:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Commands.h"
#include "Modem/Modem.h"


bool Modem::Command::RegistrationIsOk()
{
    return SendAndRecvOK("AT+CREG?");
}
