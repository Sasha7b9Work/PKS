// 2022/08/29 14:00:18 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Communicator
{
    // Эта функция вызывается из обработчика прерывания
    void AppendByte(uint8);

    // Эта функция вызывается в цикле
    void Update();
}
