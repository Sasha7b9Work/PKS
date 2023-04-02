#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace HAL_I2C2
{
    static I2C_HandleTypeDef hi2c1;

    void *handle = &hi2c1;
}


void HAL_I2C2::Init(void)
{
    hi2c1.Instance = I2C2;
    hi2c1.Init.Timing = 0x2000090E;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
        HAL::ErrorHandler();
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
        HAL::ErrorHandler();
    }
}


void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle) //-V2009
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    if (i2cHandle->Instance == I2C2)
    {
        /* USER CODE BEGIN I2C2_MspInit 0 */

        /* USER CODE END I2C2_MspInit 0 */

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**I2C2 GPIO Configuration
        PA9     ------> I2C2_SCL
        PA10     ------> I2C2_SDA
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();
        /* USER CODE BEGIN I2C2_MspInit 1 */

        /* USER CODE END I2C2_MspInit 1 */
    }
}


int8 HAL_I2C2::Read(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (uint16)(dev_id << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, 5);

    if (status == HAL_OK)
    {
        rslt = 0;
    }
    else
    {
        rslt = -1;
    }
    return rslt;
}


int8 HAL_I2C2::Read16(uint8 dev_id, uint8* data)
{
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&hi2c1, (uint16)((dev_id << 1) + 1), data, 2, 10);

    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    return (status == HAL_OK) ? 0 : -1;
}


int8 HAL_I2C2::Write(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {

    }
    int8_t rslt = 0; /* Return 0 for Success, non-zero for failure */

    HAL_StatusTypeDef status = HAL_OK;
    status = HAL_I2C_Mem_Write(&hi2c1, (uint16)(dev_id << 1), reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, 0xffff);

    if (status == HAL_OK)
    {
        rslt = 0;
    }
    else
    {
        rslt = -1;
    }
    return rslt;
}


int8 HAL_I2C2::Write8(uint8 dev_id, uint8 data)
{
    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, (uint16)(dev_id << 1), &data, 1, 10);

    while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY)
    {
    }

    return (status == HAL_OK) ? 0 : -1;
}
