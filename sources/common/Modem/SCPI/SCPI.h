// 2024/01/16 12:03:40 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Modem/SCPI/BaseSCPI.h"
#include "Modem/SCPI/ControlSCPI.h"
#include "Modem/SCPI/InfoSCPI.h"
#include "Modem/SCPI/ModemSCPI.h"
#include "Modem/SCPI/NetSCPI.h"


struct StructSCPI
{
    pchar  string;
    pchar (*func)(pchar);       // Функция возвращает указатель на последний обработанный символ
};


namespace SCPI
{
    void Append(char);

    void Update();
}
