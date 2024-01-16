// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Storage/Storage.h"
#include "Measurer/Contactors.h"
#include "Modem/MQTT/Sender.h"
#include "Hardware/Timer.h"
#include "Storage/MemoryStorage.h"
#include "Modem/SCPI/SCPI.h"


void Device::Init()
{
    Log::Init();

    HAL::Init();

    Contactors::Init();

    Storage::Init();

    Display::Init();

    SCPI::Init();

    Modem::Init();
}


void Device::Update()
{
    HAL_FWDGT::Update();

    Measurer::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Contactors::Serviceability::Update();

    Storage::Update();

    Display::Update();

    Modem::Update();

    SCPI::Update();
}


int Device::GetServerPort()
{
    return SERVER_PORT;
}


pchar Device::GetServerIP()
{
    return SERVER_IP;
}
