// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Updater.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace Updater
{
#define SIZE_SECTOR                 (2 * 1024)
#define FLASH_ADDR_SAVED_FIRMWARE   (FLASH_BASE + 50 * SIZE_SECTOR)
#define FLASH_ADDR_BOOTLOADER       (FLASH_BASE + 100 * SIZE_SECTOR)


    typedef  void (*iapfun)(void);
    iapfun      jump2app;


#ifdef WIN32
#define __asm
#endif

    __asm void MSR_MSP(uint)
    {
#ifndef WIN32
        MSR MSP, r0
            BX r14
#endif
    }//set Main Stack value


    static void JumpToBootloader()
    {
        jump2app = (iapfun) * (volatile uint *)(FLASH_ADDR_BOOTLOADER + 4);       //the second address of app code is the program
        MSR_MSP(*(volatile uint *)FLASH_ADDR_BOOTLOADER);                         //initialize app pointer
        jump2app();                                                         //jump to app
    }

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

    HAL_ROM::WriteData(FLASH_ADDR_SAVED_FIRMWARE + part * SIZE_SECTOR, data, 2048);
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
