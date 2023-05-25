// 2023/05/25 21:51:06 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace MQTT
{
    void PublishPacket(const char *topic, const char *message);

    namespace Send
    {
        // Послать запрос на передачу данных. После появления ">" отправить данные
        void SendRequest();
    }
}
