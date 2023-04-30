// 2023/4/30 17:48:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Commands.h"
#include "Modem/Modem.h"
#include "Modem/Parser.h"
#include <cstring>


using namespace Parser;


bool Modem::Command::RegistrationIsOk()
{
    char answer[Modem::MAX_LENGTH_ANSWERR];

    if (!SendAndWaitAnswer("AT+CREG?", answer))
    {
        return false;
    }

    int num_commas = NumberSymbols(answer, ',');

    if (num_commas < 1 || num_commas > 3)
    {
        return false;
    }

    int pos_colon = PositionSymbol(answer, ':', 1);

    if (pos_colon < 0)
    {
        return false;
    }

    int pos_comma1 = PositionSymbol(answer, ',', 1);

    int pos_comma2 = PositionSymbol(answer, ',', 2);

    char word[32];

    GetWord(answer, word, pos_comma1, pos_comma2);

    if (std::strcmp(word, "0") == 0)            // Not registered, MT is not currently searching a new operator to register to
    {
        return false;
    }

    if (std::strcmp(word, "2") == 0)            // Not registered, but MT is currently searching a new operator to register to
    {
        return false;
    }

    if (std::strcmp(word, "3") == 0)            // Registration denied
    {
        return false;
    }

    if (std::strcmp(word, "4") == 0)            // Unknown
    {
        return false;
    }

    return true;
}
