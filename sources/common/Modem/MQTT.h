// 2023/05/25 21:51:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace MQTT
{
    void Update(pchar);

    bool InStateRunning();

    void Reset();

    void CallbackOnReceiveData(pchar);

    namespace Packet
    {
        void Publish(const char *topic, const char *message);
    }
}
