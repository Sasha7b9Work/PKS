// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Modem
{
    void Init();

    void Update();

    // Есть обновление
    bool ExistUpdate();

    void Transmit(pchar);

    // Возвращает последний полученный ответ. После каждого вызова Transmit() ответ очищается
    pchar LastAnswer();

    void CallbackOnReceive(char);
}
