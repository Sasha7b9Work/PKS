// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"


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


namespace HAL_I2C
{
    void Init();
    void Write8(uint8 data);
    void Write(uint8 *, int size);
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

    void TIMER1_IRQHandler(void);

    void UART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif
