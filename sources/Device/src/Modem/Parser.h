// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Parser
{
    int NumberSymbols(pchar, char);
    int NumberSymbols(const String &, char);

    // num начинаетс€ с единицы
    int PositionSymbol(pchar, char symbol, int num);
    int PositionSymbol(const String &, char symbol, int num);

    //  опирует в out символы, расположенные между позици€ми start и end
    pchar GetWord(pchar, char out[32], int start, int end);
    String GetWord(const String &, int start, int end);
}
