// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace Updater
{
    void Update();

    static void JumpToBootloader();

    static void LoadFirmware();

    // Сохранить часть прошивки 
    static void SaveParthFirmware(int part, uint8 data[2048]);

    static int GetSizeFirmware();

    // Получить часть прошивки
    static void GetPartFirmware(int, uint8[2048]);
}

int Updater::GetSizeFirmware()
{
    return 0;
}


void Updater::GetPartFirmware(int , uint8 [2048])
{

}


void Updater::SaveParthFirmware(int part, uint8 data[2048])
{
    HAL_ROM::ErasePage(part + 50);

    HAL_ROM::WriteData(HAL_ROM::ADDR_SAVED_FIRMWARE + part * HAL_ROM::SIZE_PAGE, data, 2048);
}


void Updater::LoadFirmware()
{
    int size = GetSizeFirmware();

    uint8 data[2048];

    int part = 0;

    while (size > 0)
    {
        GetPartFirmware(part, data);

        SaveParthFirmware(part, data);

        size -= 2048;

        part++;
    }
}


void Updater::Update()
{
    if (Modem::ExistUpdate())
    {
        LoadFirmware();

        JumpToBootloader();
    }
}


#ifdef GUI
#else
    //set Main Stack value
    __asm void MSR_MSP(uint)
    {
    #ifndef WIN32
        MSR MSP, r0
        BX r14
    #endif
    }
#endif


void Updater::JumpToBootloader()
{
    typedef void (*iapfun)(void);
    iapfun  jump2app;

    jump2app = (iapfun) * (volatile uint *)(HAL_ROM::ADDR_BOOTLOADER + 4); //-V566
#ifndef GUI
    MSR_MSP(*(volatile uint *)HAL_ROM::ADDR_BOOTLOADER);                        //initialize app pointer //-V566
#endif
    jump2app();                                                                 //jump to app
}
