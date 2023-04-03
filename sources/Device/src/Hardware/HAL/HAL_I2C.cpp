#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


namespace HAL_I2C2
{
}


void HAL_I2C2::Init(void)
{
}


int8 HAL_I2C2::Read(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    return 0;
}


int8 HAL_I2C2::Read16(uint8 dev_id, uint8* data)
{
    return 0;
}


int8 HAL_I2C2::Write(uint8 dev_id, uint8 reg_addr, uint8* reg_data, uint16 len)
{
    return 0;
}


int8 HAL_I2C2::Write8(uint8 dev_id, uint8 data)
{
    return 0;
}
