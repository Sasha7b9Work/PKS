// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Modem
{
    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void Transmit(pchar);

    // Посылает команду и возращает true, если принято ОК
    bool SendAndRecvOK(pchar);

    // Возвращает последний полученный ответ. После каждого вызова Transmit() ответ очищается
    // Последний символ ответа 0x0d <CR>
    pchar LastAnswer();

    void CallbackOnReceive(char);
}
