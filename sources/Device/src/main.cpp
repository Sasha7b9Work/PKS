#include "defines.h"
#include "Device.h"


int main(void)
{
    Device::Init();

    while(1)
    {
        Device::Update();
    }
}
