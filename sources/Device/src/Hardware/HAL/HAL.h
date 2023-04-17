// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"


struct PackedTime
{
    uint year    : 6;
    uint month   : 4;
    uint day     : 5;
    uint hours   : 5;
    uint minutes : 6;
    uint seconds : 6;
    PackedTime(uint h = 11, uint m = 11, uint s = 11, uint d = 11, uint mo = 11, uint y = 11) :
        year(y), month(mo), day(d), hours(h), minutes(m), seconds(s) {};
};


namespace HAL
{
    void Init();
    void ErrorHandler();
}


namespace HAL_ADC
{
    void Init();
    // Вызывать, когда считаны новые значения
    void Callback();
}


// Для дисплея
namespace HAL_I2C
{
    void Init();
    void Write(uint8 command, uint8 *data, int size);
}


// Для внешней памяти
namespace HAL_SPI
{
    void Init();
    void Write(uint8);
    void Write(const void *buffer, int size);
    void WriteRead(const void *out, void *in, int size);
}


namespace HAL_USART_GPRS
{
    void Init();
    void Transmit(pchar);
    void CallbackOnReceive(char);
}


namespace HAL_USART_LOG
{
    void Init();
    void Transmit(pchar);
}


namespace HAL_ROM
{
    static const uint ADDR_BASE = 0x08000000U;
    static const uint SIZE_PAGE = 2 * 1024;
    static const uint ADDR_SAVED_FIRMWARE = ADDR_BASE + 50 * SIZE_PAGE;
    static const uint ADDR_BOOTLOADER = ADDR_BASE + 100 * SIZE_PAGE;

    // num_sector от 0 до 127. Каждый сектор занимает 2 кБ
    void ErasePage(int num_page);

    // address должен быть кратен 4, size должен быть кратен 4
    void WriteData(uint address, uint8 *data, int size);
}


namespace HAL_RTC
{
    void Init();

    PackedTime CurrentTime();
}



#ifdef __cplusplus
extern "C" {
#endif 

    /* function declarations */
    /* this function handles NMI exception */
    void NMI_Handler(void);
    /* this function handles HardFault exception */
    void HardFault_Handler(void);
    /* this function handles MemManage exception */
    void MemManage_Handler(void);
    /* this function handles BusFault exception */
    void BusFault_Handler(void);
    /* this function handles UsageFault exception */
    void UsageFault_Handler(void);
    /* this function handles SVC exception */
    void SVC_Handler(void);
    /* this function handles DebugMon exception */
    void DebugMon_Handler(void);
    /* this function handles PendSV exception */
    void PendSV_Handler(void);
    /* this function handles SysTick exception */
    void SysTick_Handler(void);

    void ADC0_1_IRQHandler(void);

    void UART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif
