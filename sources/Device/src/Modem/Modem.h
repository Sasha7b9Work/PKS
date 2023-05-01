// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Modem
{
    static const int MAX_LENGTH_ANSWERR = 128;

    void Init();

    void Update();

    // true означает, что модем проинициализирован и готов к работе
    bool IsRunning();

    // Есть обновление
    bool ExistUpdate();

    bool LastAnswer(char [MAX_LENGTH_ANSWERR]);

    String LastAnswer();

    void CallbackOnReceive(char);
}
