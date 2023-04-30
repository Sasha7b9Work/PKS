// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Parser
{
    int NumberSymbols(pchar, char);

    // num начинается с единицы
    int PositionSymbol(pchar, char symbol, int num);

    char *GetWord(pchar, char out[32],  int pos_start, int pos_end);
}
