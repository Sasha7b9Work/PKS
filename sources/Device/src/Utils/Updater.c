// 2023/04/05 12:25:03 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Utils/Updater.h"
#include "Modem/Modem.h"
#include "Hardware/HAL/HAL.h"


#define FLASH_APP1_ADDR  0x08032000


typedef  void (*iapfun)(void);
iapfun      jump2app;


#ifdef WIN32
    #define __asm
#endif

__asm void MSR_MSP(uint addr)
{
#ifndef WIN32
    MSR MSP, r0
    BX r14
#endif
}//set Main Stack value


static void JumpToApplication()
{
    jump2app = (iapfun) * (volatile uint *)(FLASH_APP1_ADDR + 4);       //the second address of app code is the program
    MSR_MSP(*(volatile uint *)FLASH_APP1_ADDR);                         //initialize app pointer
    jump2app();                                                         //jump to app
}


static int GetSizeFirmware()
{
    return 0;
}


static void EraseSector(int sector)
{

}


static void WriteSector(int sector, uint8 data[2048])
{

}


// Получить часть прошивки
static void GetPartFirmware(int part, uint8 data[2048])
{

}


// Сохранить часть прошивки 
static void SaveParthFirmware(int part, uint8 data[2048])
{
    EraseSector(part);

    WriteSector(part, data);
}


static void LoadFirmware()
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


static void Update()
{
    LoadFirmware();

    JumpToApplication();
}


void Updater_Update()
{
    if (Modem_ExistUpdate())
    {
        Update();
    }
}
