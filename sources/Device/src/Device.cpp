// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "FlashDisk/FlashDisk.h"
#include "Hardware/Timer.h"
#include "Measurer/Contactors.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include <gd32f30x_rcu.h>


namespace Device
{
//    static bool TestMemory();
}


void Device::Init()
{
    HAL::Init();
    
    Contactors::Init();

    Modem::Init();

    FlashDisk::Init();

    Display::Init();
}


void Device::Update()
{
    Measurer::Update();

    Display::Update();

    Contactors::Update(Measurer::Measure5Sec());

    Modem::Update();

    HAL_PINS::Update();

    Contactors::VerifyCondition();
}


//bool Device::TestMemory()
//{
//    static TimeMeterMS meter;
//
//    if (meter.ElapsedTime() < 100)
//    {
//        return false;
//    }
//
//    meter.Reset();
//
//    M25P80::EraseSector(0);
//
//    uint8 byte_write = 0x38;
//
//    M25P80::WriteByte(byte_write);
//
//    uint8 byte_read = M25P80::ReadByte();
//
//    bool result = (byte_read == byte_write);
//
//    return result;
//}
