// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Hardware/HAL/HAL_PINS.h"


namespace HAL
{
    void Init();
    void ErrorHandler();
}


namespace HAL_ROM
{
    // С этого адреса расположено приложение
    static const uint ADDR_APPLICATION = 0x08000000U;
    static const uint MAX_SIZE_APPLICATION = 1024 * 76;

    // В это место записывается новая прошивка перед заменой старой на неё
    static const uint ADDR_STORAGE = 0x8013000U;
    static const uint MAX_SIZE_STORAGE = 1024 * 76;

    static const uint SIZE_PAGE = 2 * 1024;

    // Отсюда начинается отсчёт страниц
    static const uint ADDR_FLASH = (uint)0x08000000U;

    // num_sector от 0 до 127. Каждый сектор занимает 2 кБ
    void ErasePage(int num_page);

    bool PageIsEmpty(int num_page);

    // address должен быть кратен 4, size должен быть кратен 4
    void WriteData(uint address, uint8 *data, int size);
}


namespace HAL_USART_GPRS
{
    void Init();
    // Передать с завершающм 0x0d
    void Transmit(pchar);
    void Transmit(void *, int);
    void CallbackOnReceive(char);
}


namespace HAL_USART_LOG
{
    void Init();
    void Transmit(pchar);
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

    void UART3_IRQHandler(void);

#ifdef __cplusplus
}
#endif
