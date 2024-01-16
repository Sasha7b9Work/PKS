#include "W25Q64.h"

void W25Q64_delay(void) {
	delay_micros(50);
}

void W25Q64_init(void) {
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	
	//Disable CS
	HAL_GPIO_WritePin(PIN_FLASH_CS_PORT, PIN_FLASH_CS_PIN, 1);
	
	GPIO_InitStruct.Pin = PIN_FLASH_CS_PIN;
  HAL_GPIO_Init(PIN_FLASH_CS_PORT, &GPIO_InitStruct);
	
	//Enable HOLD
	HAL_GPIO_WritePin(PIN_FLASH_HLD_PORT, PIN_FLASH_HLD_PIN, 0);
	
	GPIO_InitStruct.Pin = PIN_FLASH_HLD_PIN;
  HAL_GPIO_Init(PIN_FLASH_HLD_PORT, &GPIO_InitStruct);
}

void W25Q64_enableChip(void) {
	
	if (hspi1.Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_16) {
	
		/*
		Режим CPOL CPHA
		0     0    0
		1     0    1
		2     1    0
		3     1    1
		*/
		
		/* SPI1 parameter configuration*/
		hspi1.Instance = SPI1;
		hspi1.Init.Mode = SPI_MODE_MASTER;
		hspi1.Init.Direction = SPI_DIRECTION_2LINES;
		hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
		hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;    //SPI_POLARITY_LOW соответствует CPOL = 0 и SPI_POLARITY_HIGH соответствует CPOL = 1
		hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;        //SPI_PHASE_1EDGE соответствует  CPHA = 0 и SPI_PHASE_2EDGE   соответствует CPHA = 1
		hspi1.Init.NSS = SPI_NSS_SOFT;
		hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
		hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
		hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		hspi1.Init.CRCPolynomial = 10;
		if (HAL_SPI_Init(&hspi1) != HAL_OK)
		{
			Error_Handler();
		}
	}
	
	//Disable HOLD
	HAL_GPIO_WritePin(PIN_FLASH_HLD_PORT, PIN_FLASH_HLD_PIN, 1);
	
	//Enable CS
	HAL_GPIO_WritePin(PIN_FLASH_CS_PORT, PIN_FLASH_CS_PIN, 0);
	
	W25Q64_delay();
}
void W25Q64_disableChip(void) {
	
	//Disable CS
	HAL_GPIO_WritePin(PIN_FLASH_CS_PORT, PIN_FLASH_CS_PIN, 1);
	
	//Enable HOLD
	HAL_GPIO_WritePin(PIN_FLASH_HLD_PORT, PIN_FLASH_HLD_PIN, 0);
	
	//HAL_SPI_DeInit(&hspi1);
	
	W25Q64_delay();
}

uint8_t	W25Q64_spi(uint8_t data)
{
	uint8_t	ret;
	HAL_SPI_TransmitReceive(&hspi1, &data, &ret, 1, HAL_MAX_DELAY);
	return ret;
}

void W25Q64_write24(uint8_t reg, uint32_t data) {
	
	W25Q64_enableChip();
	
	//data send, MSB first
	//uint8_t tx_data[] = {reg, data >> 16, data >> 8, data};
	
	W25Q64_spi(reg);
	W25Q64_spi(data >> 16);
	W25Q64_spi(data >> 8);
	W25Q64_spi(data);

	//HAL_SPI_Transmit(&hspi1, tx_data, sizeof(tx_data), HAL_MAX_DELAY);
	
	W25Q64_disableChip();
}

uint32_t W25Q64_read24(uint8_t reg) {
	
	W25Q64_enableChip();
	
	uint32_t t0 = 0, t1 = 0, t2 = 0;
  W25Q64_enableChip();
	
  W25Q64_spi(reg);
  t0 = W25Q64_spi(W25QXX_DUMMY_BYTE);
  t1 = W25Q64_spi(W25QXX_DUMMY_BYTE);
  t2 = W25Q64_spi(W25QXX_DUMMY_BYTE);
	
	W25Q64_disableChip();
  
  return (t0 << 16) | (t1 << 8) | t2;
}


uint32_t W25Q64_JEDEC(void) {
	/*
	const uint8_t _capID[18]   =
	{0x10,   0x11,    0x12,    0x13,    0x14,  0x15,  0x16,  0x17,  0x18,   0x19,   0x41,  0x42,  0x43,  0x4B, 0x00, 0x01, 0x13, 0x37};

	const uint32_t _memSize[18]  =
	{KB(64), KB(128), KB(256), KB(512), MB(1), MB(2), MB(4), MB(8), MB(16), MB(32), MB(2), MB(4), MB(8), MB(8), KB(256), KB(512), MB(4), KB(512)};
	*/
	
	return W25Q64_read24(0x9F);
}

void W25Q64_write_enable(void) {
	
	//Именно так нужно и делать, CS low, 0x06, CS high
	W25Q64_enableChip();
	
	W25Q64_spi(0x06);

	W25Q64_disableChip();
	
	delay_millis(1);
}

void W25Q64_wait_status(void) {
	
	uint8_t  status = 0;
	
	W25Q64_enableChip();
	
	//Читаем статус
	W25Q64_spi(0x05);
	
	uint32_t wdt = millis();
	uint32_t m = 0;
	
	do {
		status = W25Q64_spi(W25QXX_DUMMY_BYTE);
		
		m = millis();
		
		if (m - wdt > 100)  {
			sys_wdt_reset();
			wdt = m;
			//break;
		}
		delay_millis(1);
	} while (status & 0x01); //busy bit
	
	W25Q64_disableChip();
}

void W25Q64_sector_erase(uint32_t addr) {
	
	W25Q64_write_enable();
	
	W25Q64_write24(0x20, addr); //CS high after last byte
	
	//Читаем статус
	W25Q64_wait_status();
}

void W25Q64_chip_erase(void) {
	
	W25Q64_write_enable();
	
	W25Q64_enableChip();
	W25Q64_spi(0x60);//0xC7
	W25Q64_disableChip();
	
	//Читаем статус
	W25Q64_wait_status();
}

void W25Q64_write_data(uint32_t addr, void *write_data, uint8_t write_length) {
	
	char str[64];
	
	if (! (addr & 0xFFF) ) {
		//Начало сектора, стираем
		
		//sprintf(str, "Erase = %u", addr);
		//serial_write(str);
		
		W25Q64_sector_erase(addr);
	}
	
	counter_micros_start();
	
	W25Q64_write_enable();
	
	W25Q64_enableChip();
	
	//sprintf(str, "Write  addr = %u; len = %u", addr, write_length);
	//serial_write(str);
	
	//Page Program
	W25Q64_spi(0x02);
	W25Q64_spi(addr >> 16);
	W25Q64_spi(addr >> 8);
	W25Q64_spi(addr);
	
	HAL_SPI_Transmit(&hspi1, write_data, write_length, HAL_MAX_DELAY);
	
	W25Q64_disableChip(); //CS high
	
	//uint32_t t1 = counter_micros_stop();
	
	//Читаем статус
	//counter_micros_start();
	
	W25Q64_wait_status();
	
	//uint32_t t2 = counter_micros_stop();
	
	//sprintf(str, "t1 = %u; t2 = %u", t1, t2);
	//serial_write(str);
	//t1 = 1200; t2 = 119
}

void W25Q64_read_data(uint32_t addr, void *read_data, uint8_t read_length) {
	
	char str[64];
	
	W25Q64_enableChip();
	
	//sprintf(str, "Read addr = %u; len = %u", addr, read_length);
	//serial_write(str);
	
	//Read Data
	W25Q64_spi(0x03);
	W25Q64_spi(addr >> 16);
	W25Q64_spi(addr >> 8);
	W25Q64_spi(addr);
	
	while (read_length) {
		
		*(uint8_t*)read_data = W25Q64_spi(W25QXX_DUMMY_BYTE);
		
		read_data = (uint8_t*)read_data + 1;
		
		read_length--;
	}
	
	W25Q64_disableChip();
	
	//uint32_t t = counter_micros_stop();
	//sprintf(str, "t = %u", t);
	//serial_write(str);
	
	//t = 1196 (500k), 147 (8M), если писать 64 байта. если 4 байта то 20мкс
}

void W25Q64_read_UID(uint8_t *read_data) {
	
	W25Q64_enableChip();
	
	W25Q64_spi(0x4B);
	
	for(uint8_t	i=0; i<4; i++) {
		W25Q64_spi(W25QXX_DUMMY_BYTE);
	}
	
	for(uint8_t	i=0; i<8; i++) {
		read_data[i] = W25Q64_spi(W25QXX_DUMMY_BYTE);
	}
	
	W25Q64_disableChip();
}