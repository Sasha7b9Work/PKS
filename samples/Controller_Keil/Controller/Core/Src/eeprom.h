#ifndef _EEPROM_H
#define _EEPROM_H

#include "I2C.h"
#include "Config.h"

void         eeprom_read_buffer(uint32_t address, void *buf, uint16_t len);
uint8_t      eeprom_read_byte(uint32_t address);
uint16_t     eeprom_read_uint16(uint32_t address);
uint32_t     eeprom_read_uint32(uint32_t address);
float        eeprom_read_float(uint32_t address);

void         eeprom_write_buffer(uint32_t address, const void *buf, uint16_t len);
void         eeprom_write_byte(uint32_t address, uint8_t byte);
void         eeprom_write_uint16(uint32_t address, uint16_t val);
void         eeprom_write_uint32(uint32_t address, uint32_t val);
void         eeprom_write_float(uint32_t address, float val);

#endif