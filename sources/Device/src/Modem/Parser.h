// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Parser
{
    int NumberSymbols(pchar, char);

    // num начинаетс€ с единицы
    int PositionSymbol(pchar, char symbol, int num);

    //  опирует в out символы, расположенные между позици€ми start и end
    pchar GetWord(pchar, char out[32], int start, int end);
}
