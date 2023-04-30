// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Modem
{
    static const int MAX_LENGTH_ANSWERR = 128;

    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void Transmit(pchar);

    // Посылает команду и возращает true, если принято ОК
    bool SendAndRecvOK(pchar);

    bool SendAndWaitAnswer(pchar cmd, char answer_out[MAX_LENGTH_ANSWERR], uint timeout = 1500);

    // Возвращает последний полученный ответ. После каждого вызова Transmit() ответ очищается
    // Последний символ ответа 0x0d <CR>
    pchar LastAnswer();

    void CallbackOnReceive(char);
}
