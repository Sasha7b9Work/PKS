// 2022/04/27 11:48:13 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Device.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Modem.h"
#include "Display/Display.h"
#include "Measurer/Measurer.h"
#include "FlashDisk/FlashDisk.h"
#include "Test.h"
#include "Utils/Updater.h"
#include "Hardware/Timer.h"
#include "Hardware/Contactor.h"
#include "Hardware/Modules/M25P80/M25P80.h"
#include <gd32f30x_rcu.h>


namespace Device
{
    static void UpdateModem();

    static uint8 TestMemory();
}


void Device::Init()
{
    HAL::Init();

    Modem::Init();

    Contactor::Init();

    FlashDisk::Init();

    Display::Init();
}


void Device::Update()
{
    Measurer::Update();

    if (Measurer::MeasureReady())
    {
        volatile FullMeasure measure = Measurer::GetMeasure();
//        Contactor::Update(measure);
    }

    Updater::Update();

    Device::UpdateModem();

//    TestMemory();

    Display::Update();
}


void Device::UpdateModem()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() > 1000)
    {
        meter.Reset();

        pchar answer = Modem::LastAnswer();
        LOG_WRITE("modem %s", answer);

        Modem::Transmit("AT");
    }
}


uint8 Device::TestMemory()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 100)
    {
        return false;
    }

    meter.Reset();

/*    M25P80::EraseSector(0);

    uint8 byte_write = 0x38;

    M25P80::WriteByte(byte_write);

    uint8 byte_read = M25P80::ReadByte();
*/

		HAL_SPI::Write(0x9F);
		//HAL_SPI::Write(0x00);
		//HAL_SPI::Write(0x00);
		//HAL_SPI::Write(0x00);
		uint8_t value;
		value = HAL_SPI::Write(0x00);
		LOG_WRITE((char *)value);
		value = HAL_SPI::Write(0x00);
		LOG_WRITE((char *)value);
		value = HAL_SPI::Write(0x00);
		LOG_WRITE((char *)value);
		value = HAL_SPI::Write(0x00);
		LOG_WRITE((char *)value);
		value = HAL_SPI::Write(0x00);
		LOG_WRITE((char *)value);

//    bool result = (byte_read == byte_write);

    return value;
}