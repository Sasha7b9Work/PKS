#include "I2C.h"

//volatile uint8_t chip_addr = M24M01_ADDR;

extern I2C_HandleTypeDef hi2c2; //M24M01

uint8_t M24M01_Start(uint8_t bank, uint32_t addr) {
		
	bank &= 0x03;
	
	uint8_t chip_addr = (M24M01_ADDR | (bank << 1));
	
	if (addr & 0xffff0000) {
		chip_addr |= 0x1;
	}
	
	chip_addr = (chip_addr << 1);
	
	return chip_addr;
}

uint8_t M24M01_write(uint8_t bank, uint32_t addr, uint8_t* buf, uint16_t len) {
	
	uint8_t chip_addr = M24M01_Start(bank, addr);
	
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c2, chip_addr, addr, I2C_MEMADD_SIZE_16BIT, buf, len, HAL_MAX_DELAY);
	
	if (status == HAL_OK)
		return true;
	else
		return false;
}

uint8_t M24M01_read(uint8_t bank, uint32_t addr, uint8_t* buf, uint16_t len) {
		
	uint8_t chip_addr = M24M01_Start(bank, addr);
	
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c2, chip_addr, addr, I2C_MEMADD_SIZE_16BIT, buf, len, HAL_MAX_DELAY);
	
	if (status == HAL_OK)
		return true;
	else
		return false;
}
