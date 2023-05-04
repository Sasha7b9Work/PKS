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

    uint Transmit(pchar);
}


bool Command::ConnectToTCP()
{
    SIM800::Transmit("AT+CIPSTART=\"TCP\",\"dev.rightech.io\",\"1883\"");

    TimeMeterMS meter;

    while (meter.ElapsedTime() < 30000)
    {
//        String answer = SIM800::LastAnswer();

        String answer;

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
