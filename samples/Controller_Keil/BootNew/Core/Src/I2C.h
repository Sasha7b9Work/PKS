#ifndef _I2C_H
#define _I2C_H

#include "stdbool.h"
#include "Utility.h"
#include "stm32f1xx_hal.h"
#include "Config.h"

#define M24M01_ADDR					0x50
#define I2C_REQUEST_WRITE		0x00
#define I2C_REQUEST_READ		0x01

uint8_t M24M01_Start(uint8_t bank, uint32_t addr);
uint8_t M24M01_write(uint8_t bank, uint32_t addr, uint8_t* buf, uint16_t len);
uint8_t M24M01_read(uint8_t bank, uint32_t addr, uint8_t* buf, uint16_t len);

#endif // _I2C_H
