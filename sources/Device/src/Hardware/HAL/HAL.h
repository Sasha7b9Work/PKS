// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


void HAL_Init(void);
void HAL_ErrorHandler(void);


void HAL_ADC_Init(void);
void HAL_ADC_Update(void);
float HAL_ADC_GetVoltage(int num, int ch);


void HAL_I2C1_Init(void);
int8 HAL_I2C1_Read(uint8 dev_id, uint8 reg_addr, uint8 *reg_data, uint16 len);
int8 HAL_I2C1_Read16(uint8 dev_id, uint8 *data);
int8 HAL_I2C1_Write(uint8 dev_id, uint8 reg_addr, uint8 *reg_data, uint16 len);
int8 HAL_I2C1_Write8(uint8 dev_id, uint8 data);



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

#ifdef __cplusplus
}
#endif
