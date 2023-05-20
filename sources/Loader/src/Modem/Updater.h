// 2023/05/18 13:54:04 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Updater
{
    // Возвращает true, если Updater отработал (загружено обновление либо обновление не требуется)
    bool IsCompleted();

    // Эта функция вызывается при получении новых байт из файла на FTP-сервере
    void CallbackOnNewBytesFromFTP(uint8 *bytes, int number);
}
