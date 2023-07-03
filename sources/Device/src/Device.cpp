// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Contactors.h"
#include "Memory/Memory.h"


void Device::Init()
{
    Log::Init();

    LOG_WRITE("   Device::Init()   ");

    HAL::Init();
    
    Contactors::Init();

    Display::Init();

    Modem::Init();

//    if (!Memory::Test())
//    {
//        Memory::Test();
//    }
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Display::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Modem::Update();

    HAL_PINS::Update();

    Contactors::Serviceability::Verify();
}
