// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Modules/SIM800C/SIM800C.h"


void Modem_Init(void)
{
    SIM800C_Init();
}


int Modem_ExistUpdate()
{
    return 1;
}
