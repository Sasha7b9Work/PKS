// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Modem
{
    static const int MAX_LENGTH_ANSWERR = 128;

    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    bool SendAndWaitAnswer(pchar cmd, char answer_out[MAX_LENGTH_ANSWERR], uint timeout = 1500);

    void CallbackOnReceive(char);
}
