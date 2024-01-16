#ifndef W25Q64_H_
#define W25Q64_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include "Utility.h"
#include "Config.h"
#include "stm32f1xx_hal.h"

extern   SPI_HandleTypeDef hspi1;

#define  W25QXX_DUMMY_BYTE         0xA5

void     W25Q64_init(void);
void     W25Q64_write_enable(void);
void     W25Q64_write_disable(void);
void     W25Q64_wait_status(void);

uint32_t W25Q64_JEDEC(void);
void     W25Q64_read_UID(uint8_t *read_data);

void     W25Q64_sector_erase(uint32_t addr);
void     W25Q64_chip_erase(void);

uint8_t	 W25Q64_spi(uint8_t data);
void     W25Q64_write_data(uint32_t addr, void *write_data, uint8_t write_length);
void     W25Q64_read_data(uint32_t addr, void *read_data, uint8_t read_length);



#endif /* W25Q64_H_ */