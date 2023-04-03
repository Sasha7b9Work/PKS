#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


#define I2C0_SPEED              400000
#define I2C0_SLAVE_ADDRESS7     0xA0
#define I2C_PAGE_SIZE           8


void HAL_I2C1_Init(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);

    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);


    /* enable I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* configure I2C clock */
    i2c_clock_config(I2C0,I2C0_SPEED,I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C0_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);
}


int8 HAL_I2C1_Read(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    return 0;
}


int8 HAL_I2C1_Read16(uint8 dev_id, uint8* data)
{
    return 0;
}


int8 HAL_I2C1_Write(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    return 0;
}


int8 HAL_I2C1_Write8(uint8 dev_id, uint8 data)
{
    return 0;
}
