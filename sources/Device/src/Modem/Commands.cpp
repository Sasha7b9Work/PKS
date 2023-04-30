// 2023/4/30 17:48:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Commands.h"
#include "Modem/Modem.h"


bool Modem::Command::RegistrationIsOk()
{
    char answer[Modem::MAX_LENGTH_ANSWERR];

    if (!SendAndWaitAnswer("AT+CREG?", answer))
    {
        return false;
    }

    return true;
}
