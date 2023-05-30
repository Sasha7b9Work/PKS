// 2023/05/26 09:27:07 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


#define SIM800_RESET()    SIM800::Reset()


namespace SIM800
{
    void Update(pchar);

    // Возвращает true, если зарегистрирован в сети
    bool IsRegistered();

    pchar LevelSignal();

    void Reset();

    namespace Trans
    {
        // Передать с завершающим 0x0d
        void With0D(pchar);

    }

    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);

    void TransmitUINT8(uint8);

    void Transmit(pchar format, int param);

    void Transmit(pchar format, pchar param);
}
