// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Storage/Storage.h"
#include "Measurer/Contactors.h"
#include "Hardware/Timer.h"


void Device::Init()
{
    Log::Init();

    HAL::Init();

    Contactors::Init();

    Storage::Init();

    Display::Init();

    Modem::Init();
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Contactors::Test::Update();
    LOG_WRITE("num step %d", Contactors::Test::GetCountSteps());

    Storage::Update();

    Display::Update();

    Modem::Update();
}
