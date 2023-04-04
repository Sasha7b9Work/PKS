// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


void HAL_Init(void);
void HAL_ErrorHandler(void);


void HAL_ADC_Init(void);
void HAL_ADC_Update(void);


void HAL_I2C0_Init(void);
void HAL_I2C0_Write8(uint8 data);
void HAL_I2C0_Write(uint8 *, int size);



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

#ifdef __cplusplus
}
#endif
