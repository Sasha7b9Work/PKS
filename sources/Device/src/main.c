#include "defines.h"
#include "Device.h"


int main(void)
{
    Device_Init();

    while(1)
    {
        Device_Update();
    }
}
