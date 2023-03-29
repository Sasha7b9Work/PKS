// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct DirectionSPI
{
    enum E
    {
        Memory,         // Работа с флеш-памятью
        Reader,         // Работа с карт-ридером
        Count
    };
};


namespace HAL
{
    void Init();
}


namespace HAL_ADC
{
    void Init();

    void Update();

    float GetVoltage(int num, int ch);
}


namespace HAL_I2C1
{
    void DeInit();

    int8 Read(uint8 dev_id, uint8 reg_addr, uint8 *reg_data, uint16 len);
    int8 Read16(uint8 dev_id, uint8 *data);

    int8 Write(uint8 dev_id, uint8 reg_addr, const uint8 *reg_data, uint16 len);
    int8 Write8(uint8 dev_id, uint8 data);
}


#ifdef __cplusplus
extern "C" {
#endif 

    void NMI_Handler(void);
    void HardFault_Handler(void);
    void MemManage_Handler(void);
    void BusFault_Handler(void);
    void UsageFault_Handler(void);
    void SVC_Handler(void);
    void DebugMon_Handler(void);
    void PendSV_Handler(void);
    void SysTick_Handler(void);
        
#ifdef __cplusplus
}
#endif
