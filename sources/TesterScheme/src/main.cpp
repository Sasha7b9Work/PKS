// 2023/09/24 09:54:28 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"


int main()
{
    Device::Init();

    while(1)
    {
        Device::Update();
    }
}
