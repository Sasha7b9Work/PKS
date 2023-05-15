// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Parser
{
    // Возвращает num-ое слово. Разделителями являются ':', ' ', ','. Если слово в кавычках, то кавычки опускаются
    // Нумерация начинается с 1
    String GetWord(const String &, int num);
    String GetWordInQuotes(const String &, int num);

    int NumberSymbols(pchar, char);
    int NumberSymbols(const String &, char);

    // num начинается с единицы
    int PositionSymbol(pchar, char symbol, int num);
    int PositionSymbol(const String &, char symbol, int num);

    // Копирует в out символы, расположенные между позициями start и end
    pchar GetWord(pchar, char out[32], int start, int end);
    String GetWord(const String &, int start, int end);
}
