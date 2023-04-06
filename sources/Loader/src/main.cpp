// 2023/04/05 10:13:53 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"


static void Reset()
{

}


int main()
{
    HAL::Init();

    for (int i = 0; i < 50; i++)
    {
        HAL_ROM::ErasePage(i);
    }

    Reset();
}
