#include "eeprom.h"

#define   EEPROM_DELAY         5

static uint32_t eeprom_last_acc = 0;

void eeprom_read_buffer(uint32_t address, void *buf, uint16_t len) {
	
	if (!eeprom_last_acc || millis() - eeprom_last_acc < EEPROM_DELAY) {
		delay_millis(EEPROM_DELAY);
	}
	
	M24M01_read (EROM_BANK, EROM_ADDR + address, buf, len);
	
	eeprom_last_acc = millis();
}

uint8_t eeprom_read_byte(uint32_t address) {
	
	uint8_t value;
	
	eeprom_read_buffer(address, &value, 1);
	
	return value;
}

uint16_t eeprom_read_uint16(uint32_t address) {
	
	uint16_t value;
	
	eeprom_read_buffer(address, &value, 2);
	
	return value;
}

uint32_t eeprom_read_uint32(uint32_t address) {
	
	uint32_t value;
	
	eeprom_read_buffer(address, &value, 4);
	
	return value;
}

float eeprom_read_float(uint32_t address) {
	
	float value;
	
	eeprom_read_buffer(address, &value, 4);
	
	return value;
}

void eeprom_write_byte(uint32_t address, uint8_t value) {
	
	eeprom_write_buffer(address, (void *)&value, 1);
}

void eeprom_write_buffer(uint32_t address, const void *buf, uint16_t len) {
	
	if (!eeprom_last_acc || millis() - eeprom_last_acc < EEPROM_DELAY) {
		delay_millis(EEPROM_DELAY);
	}
	
	M24M01_write (EROM_BANK, EROM_ADDR + address, buf, len);
	
	eeprom_last_acc = millis();
}

void eeprom_write_uint16(uint32_t address, uint16_t val) {
	eeprom_write_buffer(address, (void *)&val, sizeof(val));
}

void eeprom_write_uint32(uint32_t address, uint32_t val) {
	eeprom_write_buffer(address, (void *)&val, sizeof(val));
}

void eeprom_write_float(uint32_t address, float val) {
	eeprom_write_buffer(address, (void *)&val, sizeof(val));
}