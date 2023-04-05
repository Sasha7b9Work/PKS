// 2023/04/05 10:13:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


int main(void)
{
    HAL_Init();

    int counter = 0;

    while (1)
    {
        counter++;
    }
}
