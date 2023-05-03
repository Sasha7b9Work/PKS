// 2023/4/30 17:48:41 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Commands.h"
#include "Modem/Modem.h"
#include "Modem/Parser.h"
#include "Hardware/Timer.h"
#include <cstring>


using namespace Parser;


namespace SIM800
{
    extern const int MAX_LENGTH_ANSWERR = 128;
    extern const uint TIME_WAIT_ANSWER_DEFAULT = 1500;

    String LastAnswer();

    String FirstAnswer();

    uint Transmit(pchar);
}


bool Command::RegistrationIsOk()
{
    TimeMeterMS meter;

    SIM800::Transmit("AT+CREG?");

    String answer;

    int num_commas = 0;

    while (meter.ElapsedTime() < 10000)
    {
        answer = SIM800::LastAnswer();

        num_commas = NumberSymbols(answer.c_str(), ',');

        if (num_commas > 0)
        {
            break;
        }
    }

    int pos_colon = PositionSymbol(answer.c_str(), ':', 1);

    if (pos_colon < 0)
    {
        return false;
    }

    int pos_comma1 = PositionSymbol(answer.c_str(), ',', 1);

    int pos_comma2 = (int)std::strlen(answer.c_str());

    if (num_commas > 1)
    {
        pos_comma2 = PositionSymbol(answer.c_str(), ',', 2);
    }

    char word[32];

    GetWord(answer.c_str(), word, pos_comma1, pos_comma2);

    int stat = word[0] & 0x0f;

    if (stat == 0 ||    // Not registered, MT is not currently searching a new operator to register to
        stat == 2 ||    // Not registered, but MT is currently searching a new operator to register to
        stat == 3 ||    // Registration denied
        stat == 4)      // Unknown
    {
        return false;
    }

    return true;
}


bool Command::ConnectToTCP()
{
    SIM800::Transmit("AT+CIPSTART=\"TCP\",\"dev.rightech.io\",\"1883\"");

    TimeMeterMS meter;

    while (meter.ElapsedTime() < 30000)
    {
        String answer = SIM800::LastAnswer();

        int pos_space = PositionSymbol(answer.c_str(), ' ', 1);

        if (pos_space > 0)
        {
            char word[32];

            Parser::GetWord(answer.c_str(), word, -1, pos_space);

            if (std::strcmp(word, "ALREADY") == 0)
            {
                return true;
            }

            if (std::strcmp(word, "CONNECT") == 0)
            {
                Parser::GetWord(answer.c_str(), word, pos_space, (int)std::strlen(answer.c_str()));

                bool result = std::strcmp(word, "OK") == 0;

                return result;
            }
        }
    }

    return false;
}


bool Command::WaitCIPSTATUS(pchar value)
{
    SIM800::Transmit("AT+CIPSTATUS");

    TimeMeterMS meter;

    String answer;

    while (meter.ElapsedTime() < 10000)
    {
        answer = SIM800::LastAnswer();

        if (NumberSymbols(answer, ':') < 1)
        {
            continue;
        }

        int pos_colon = PositionSymbol(answer, ':', 1);

        String status = GetWord(answer, pos_colon, answer.Size());

        if (status == value)
        {
            return true;
        }
    }

    return false;
}
