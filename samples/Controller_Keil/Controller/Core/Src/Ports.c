/*
 * Ports.c
 *
 * Created: 04.08.2017 10:16:35
 *  Author: Admin
 */ 
#include "Ports.h"
#include "Phase.h"

uint16_t port_logic[3] = {0, 0, 0};
	
uint8_t  bind_out_A[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t  bind_out_B[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t  bind_out_C[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	
uint8_t  bind_in_A[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t  bind_in_B[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
uint8_t  bind_in_C[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

//uint32_t Port_values = 0;
uint8_t Port_values[PORTS_MAX];

extern tp_phase_type Phase[];

uint8_t Port_out_position[] = {
	7,  6,  5,  4,   3,  2, 1,  0,  8,  15, //A1 - A10
	14, 13, 12, 11, 10,  9, 23, 22, 21, 20, //B1 - B10
	19, 18, 17, 31, 30, 29, 28, 27, 26, 25  //C1 - C10
};

uint8_t Port_in_position[] = {
	0,  1,  2,  3,   4,  5, 6,  7,  8,  9,  //A1 - A10
	10, 11, 12, 13, 14, 15, 17, 18, 19, 20, //B1 - B10
	21, 22, 23, 25, 26, 27, 28, 29, 30, 31  //C1 - C10
};

uint8_t  port_scan_mode = 0;
uint8_t  port_scan_switch = 0;
uint8_t  port_scan_phase = 0;

uint16_t port_shift_count = 0;
uint8_t  port_shift_add = 0;
uint8_t  port_shift_set = 0;

void Ports_init(uint8_t wdt_reset) {
	
	//Какая-то проблема с инициализацией переменной port_scan_phase. Инициализируется в значение FF.
	port_scan_mode = 0;
	port_scan_switch = 0;
	port_scan_phase = 0;

	port_shift_count = 0;
	port_shift_add = 0;
	port_shift_set = 0;
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	//P1
	HAL_GPIO_WritePin(PIN_P1_PORT, PIN_P1_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_P1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_P1_PORT, &GPIO_InitStruct);
	
	//P2
	HAL_GPIO_WritePin(PIN_P2_PORT, PIN_P2_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_P2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_P2_PORT, &GPIO_InitStruct);
	
	//P_IN1
	GPIO_InitStruct.Pin = PIN_IN1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PIN_IN1_PORT, &GPIO_InitStruct);
	
	//P_IN2
	GPIO_InitStruct.Pin = PIN_IN2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(PIN_IN2_PORT, &GPIO_InitStruct);
	
	//DS
	HAL_GPIO_WritePin(PIN_DS_PORT, PIN_DS_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_DS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_DS_PORT, &GPIO_InitStruct);
	
	//ST
	HAL_GPIO_WritePin(PIN_ST_PORT, PIN_ST_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_ST_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_ST_PORT, &GPIO_InitStruct);
	
	//SH
	HAL_GPIO_WritePin(PIN_SH_PORT, PIN_SH_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_SH_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_SH_PORT, &GPIO_InitStruct);
	
	//OE
	HAL_GPIO_WritePin(PIN_OE_PORT, PIN_OE_PIN, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = PIN_OE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_OE_PORT, &GPIO_InitStruct);
	
	//DS2
	HAL_GPIO_WritePin(PIN_DS2_PORT, PIN_DS2_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_DS2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_DS2_PORT, &GPIO_InitStruct);
	
	//ST2
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_ST2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_ST2_PORT, &GPIO_InitStruct);
	
	//SH2
	HAL_GPIO_WritePin(PIN_SH2_PORT, PIN_SH2_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_SH2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_SH2_PORT, &GPIO_InitStruct);

	//NET_POWER
	HAL_GPIO_WritePin(PIN_NET_POWER_PORT, PIN_NET_POWER_PIN, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = PIN_NET_POWER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_NET_POWER_PORT, &GPIO_InitStruct);
	
	//CS net
	HAL_GPIO_WritePin(PIN_NET_CS_PORT, PIN_NET_CS_PIN, 1);
	
	GPIO_InitStruct.Pin = PIN_NET_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_NET_CS_PORT, &GPIO_InitStruct);
	
	//CS flash
	HAL_GPIO_WritePin(PIN_FLASH_CS_PORT, PIN_FLASH_CS_PIN, 1);
	
	GPIO_InitStruct.Pin = PIN_FLASH_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_FLASH_CS_PORT, &GPIO_InitStruct);
	
	//MODEM_POWER
	HAL_GPIO_WritePin(PIN_MODEM_POWER_PORT, PIN_MODEM_POWER_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_MODEM_POWER_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_MODEM_POWER_PORT, &GPIO_InitStruct);
	
	//MODEM_RESET
	HAL_GPIO_WritePin(PIN_MODEM_RESET_PORT, PIN_MODEM_RESET_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_MODEM_RESET_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_MODEM_RESET_PORT, &GPIO_InitStruct);
		
	//MODEM_STATUS
	GPIO_InitStruct.Pin = PIN_MODEM_STATUS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PIN_MODEM_STATUS_PORT, &GPIO_InitStruct);
	
	//MODEM_CONNECTED
	HAL_GPIO_WritePin(PIN_MODEM_CONNECTED_PORT, PIN_MODEM_CONNECTED_PIN, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = PIN_MODEM_CONNECTED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(PIN_MODEM_CONNECTED_PORT, &GPIO_InitStruct);
	
	#ifdef SIM_DETECT_DUAl
		gp_port_init(PIN_SIM1_DETECT_PORT, PIN_SIM1_DETECT_PIN, GPIO_MODE_INPUT, PULL_UP);
		gp_port_init(PIN_SIM2_DETECT_PORT, PIN_SIM2_DETECT_PIN, GPIO_MODE_INPUT, PULL_UP);
	#endif
	
	#ifdef SIM_DETECT_ONE
		gp_port_init(PIN_SIM_DETECT_PORT, PIN_SIM_DETECT_PIN, GPIO_MODE_INPUT, PULL_UP);
	#endif
	
	//Смещение адреса
	/*
	nvm_eeprom_read_buffer(EE_ADDR_SHIFT_ADD, (void *)&port_shift_add, sizeof(port_shift_add));
	if (port_shift_add == 0xFF) {port_shift_add = 0;}
	
	nvm_eeprom_read_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 4, (void *)&port_shift_count, sizeof(port_shift_count));
	if (port_shift_count == 0xFFFF) {
		port_shift_count = 0;
	} else {
		port_shift_set = 1;
	}
	
	
	if (wdt_reset) {
		Ports_read();
	} else {
		//Очищаем
		Ports_clear();
	}
	*/
	
	//Пока очищаем выходы
	Ports_renew();
	
	//Очищаяем регистр силового входа
	Ports_in_clr();
}
void Ports_clear(void) {
	
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		Port_values[n] = 0;
	}
	
	Ports_renew();
}
void Ports_read(void) {
	
	uint16_t  shift_1 = 0;
	uint16_t  shift_2 = 0;
	
	uint8_t   bit = 0;
	
	if (!port_shift_set) {
		Ports_clear();
		return;
	}
	//nvm_eeprom_read_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 0, (void *)&shift_1, sizeof(shift_1));
	//nvm_eeprom_read_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 2, (void *)&shift_2, sizeof(shift_2));
	
	//sprintf(mess, "read shift=%02X %02X", shift_1, shift_2);
	//serial_write(mess);
	
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		
		if (n < 16) {
			bit = (shift_1 >> n) & 1;
		} else {
			bit = (shift_2 >> (n-16)) & 1;
		}
		
		//sprintf(mess, "n=%u, bit=%u", n, bit);
		//serial_write(mess);
		
		Port_values[n] = bit;
	}
	Ports_renew();
}
void Ports_renew(void) {
	
	uint8_t  bit = 0;
	
	uint8_t  shift[PORTS_MAX];
	
	uint16_t  shift_1 = 0;
	uint16_t  shift_2 = 0;
	
	HAL_GPIO_WritePin(PIN_ST_PORT, PIN_ST_PIN, 0);
	
	//Чистим
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		shift[n] = 0;
	}
	
	for (uint8_t n = 0; n < PHASE_MAX; n++) {
		Phase[n].ports   = 0;
	}
	
	//Fill shift value
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		
		if (Port_values[n]) {
			
			bit = Port_out_position[n];
			
			//sprintf(mess, "n=%u, pos=%u", n, bit);
			//serial_write(mess);
			
			shift[bit] = 1;
			
			//Регистры
			if (n < 16) {
				shift_1 |= 1 << n;
			} else {
				shift_2 |= 1 << (n-16);
			}
			
			//Фазы
			if (n < 10) {
				Phase[0].ports |= 1 << n;
			} else if (n < 20) {
				Phase[1].ports |= 1 << (n-10);
			} else {
				Phase[2].ports |= 1 << (n-20);
			}
		}
	}
	
	//Move data
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		
		bit = (PORTS_MAX-1) - n;
		
		HAL_GPIO_WritePin(PIN_DS_PORT, PIN_DS_PIN, shift[bit] );
		
		//Сдвиг на 1
		HAL_GPIO_WritePin(PIN_SH_PORT, PIN_SH_PIN, 1);
		delay_micros(2);
		HAL_GPIO_WritePin(PIN_SH_PORT, PIN_SH_PIN, 0);
	}
	
	//Запись
	HAL_GPIO_WritePin(PIN_ST_PORT, PIN_ST_PIN, 1);
	delay_micros(5);
	HAL_GPIO_WritePin(PIN_ST_PORT, PIN_ST_PIN, 0);
	
	port_shift_count++;
	
	if (port_shift_count>=65000) {
		port_shift_count = 0;
		
		port_shift_add+=6;
		if (port_shift_add >= 84) {//14 раз
			port_shift_add = 0;
		}
		//nvm_eeprom_erase_and_write_buffer(EE_ADDR_SHIFT_ADD, (void *)&port_shift_add, sizeof(port_shift_add));
	}
	
	//EEPROM
	//nvm_eeprom_erase_and_write_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 0, (void *)&shift_1, sizeof(shift_1));
	//nvm_eeprom_erase_and_write_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 2, (void *)&shift_2, sizeof(shift_2));
	//nvm_eeprom_erase_and_write_buffer(EE_ADDR_SHIFT_ADR + port_shift_add + 4, (void *)&port_shift_count, sizeof(port_shift_count));
	
}

uint8_t Port_get(uint8_t phase, uint8_t port) {
	if (phase==1) {
		return PortA_get(port);
	}
	if (phase==2) {
		return PortB_get(port);
	}
	if (phase==3) {
		return PortC_get(port);
	}
	return 0;
}
	
void Port_set(uint8_t phase, uint8_t port) {
	if (phase==1) {
		PortA_set(port);
	}
	if (phase==2) {
		PortB_set(port);
	}
	if (phase==3) {
		PortC_set(port);
	}
}
void Port_clr(uint8_t phase, uint8_t port) {
	if (phase==1) {
		PortA_clr(port);
	}
	if (phase==2) {
		PortB_clr(port);
	}
	if (phase==3) {
		PortC_clr(port);
	}
}

//1-10
void PortA_set(uint8_t p) {

	p = Ports_bind_out_get(0, p);
	
	Port_values[p-1] = 1;
}
void PortA_clr(uint8_t p) {
	
	p = Ports_bind_out_get(0, p);
	
	Port_values[p-1] = 0;
}
uint8_t PortA_get(uint8_t p) {
	
	p = Ports_bind_out_get(0, p);
	
	return Port_values[p-1];
}

void PortB_set(uint8_t p) {
	
	p = Ports_bind_out_get(1, p);
	
	Port_values[p-1+10] = 1;
}
void PortB_clr(uint8_t p) {
	
	p = Ports_bind_out_get(1, p);
	
	Port_values[p-1+10] = 0;
}
uint8_t PortB_get(uint8_t p) {
	
	p = Ports_bind_out_get(1, p);
	
	return Port_values[p-1+10];
}

void PortC_set(uint8_t p) {
	
	p = Ports_bind_out_get(2, p);
	
	Port_values[p-1+20] = 1;
}
void PortC_clr(uint8_t p) {
	
	p = Ports_bind_out_get(2, p);
	
	Port_values[p-1+20] = 0;
}
uint8_t PortC_get(uint8_t p) {
	
	p = Ports_bind_out_get(2, p);
	
	return Port_values[p-1+20];
}

uint8_t Ports_scan_get(void) {
	return port_scan_phase;
}

bool Ports_scan_busy(uint8_t p) {
	if (!p) {
		//Сброс
		port_scan_phase = 0;
		return true;
	} else {
		if (!port_scan_phase || port_scan_phase==p) {
			port_scan_phase = p;
			return true;
		} else {
			return false;
		}
	}
}
void Ports_scan_mode(uint8_t mode) {
	if (!mode) {
		
		HAL_GPIO_WritePin(PIN_P2_PORT, PIN_P2_PIN, 0);
		HAL_GPIO_WritePin(PIN_P1_PORT, PIN_P1_PIN, 0);
		
	} else if (mode==1) {
		
		HAL_GPIO_WritePin(PIN_P1_PORT, PIN_P1_PIN, 1);
		HAL_GPIO_WritePin(PIN_P2_PORT, PIN_P2_PIN, 0);
		
	} else {
		HAL_GPIO_WritePin(PIN_P1_PORT, PIN_P1_PIN, 0);
		HAL_GPIO_WritePin(PIN_P2_PORT, PIN_P2_PIN, 1);
	}
	port_scan_mode = mode;
}

bool Ports_scan_P1(void) {
	return HAL_GPIO_ReadPin(PIN_IN1_PORT, PIN_IN1_PIN);
}

bool Ports_scan_P2(void) {
	return HAL_GPIO_ReadPin(PIN_IN2_PORT, PIN_IN2_PIN);
}

//1-10
uint8_t Ports_bind_out_get(uint8_t phase, uint8_t port) {
	port--;
	
	switch(phase) {
		case 0:
			return bind_out_A[port];
		break;
		
		case 1:
			return bind_out_B[port];
		break;
		
		case 2:
			return bind_out_C[port];
		break;
		
		default: return 0;
	}
}

//1-10
uint8_t Ports_bind_in_get(uint8_t phase, uint8_t port) {
	port--;
	
	switch(phase) {
		case 0:
			return bind_in_A[port];
		break;
		
		case 1:
			return bind_in_B[port];
		break;
		
		case 2:
			return bind_in_C[port];
		break;
		
		default: return 0;
	}
}

//1-10
void Ports_bind_out_set(uint8_t phase, uint8_t port, uint8_t value) {
	
	uint16_t addr = 0;
	port--;
	
	if (phase==0) {
		addr = EE_BIND_OUTA;
		bind_out_A[port] = value;
	}
	else if (phase==1) {
		addr = EE_BIND_OUTB;
		bind_out_B[port] = value;
	}
	else {
		addr = EE_BIND_OUTC;
		bind_out_C[port] = value;
	}
	eeprom_write_buffer(addr + port, (void *)&value, 1);
}

void Ports_bind_out_read(void) {
	uint8_t val = 0;
	
	for (uint8_t n = 0; n < CONTACTOR_MAX; n++) {
		eeprom_read_buffer(EE_BIND_OUTA + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_out_A[n] = val;
		}

		eeprom_read_buffer(EE_BIND_OUTB + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_out_B[n] = val;
		}

		eeprom_read_buffer(EE_BIND_OUTC + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_out_C[n] = val;
		}
	}
}

//1-10
void Ports_bind_in_set(uint8_t phase, uint8_t port, uint8_t value) {
	
	uint16_t addr = 0;
	port--;
	
	if (phase==0) {
		addr = EE_BIND_INA;
		bind_in_A[port] = value;
	}
	else if (phase==1) {
		addr = EE_BIND_INB;
		bind_in_B[port] = value;
	}
	else {
		addr = EE_BIND_INC;
		bind_in_C[port] = value;
	}
	eeprom_write_buffer(addr + port, (void *)&value, 1);
}

void Ports_bind_in_read(void) {
	uint8_t val = 0;
	
	for (uint8_t n = 0; n < CONTACTOR_MAX; n++) {
		eeprom_read_buffer(EE_BIND_INA + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_in_A[n] = val;
		}

		eeprom_read_buffer(EE_BIND_INB + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_in_B[n] = val;
		}

		eeprom_read_buffer(EE_BIND_INC + n, (void *)&val, 1);
		if (val!=0xFF && val>0 && val<=CONTACTOR_MAX) {
			bind_in_C[n] = val;
		}
	}
}

void Ports_in_clr(void) {
	
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 0);
	HAL_GPIO_WritePin(PIN_DS2_PORT, PIN_DS2_PIN, 0);
	
	//Move data
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		
		//Сдвиг на 1
		HAL_GPIO_WritePin(PIN_SH2_PORT, PIN_SH2_PIN, 1);
		delay_micros(5);
		HAL_GPIO_WritePin(PIN_SH2_PORT, PIN_SH2_PIN, 0);
	}
	
	//Запись в выход
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 1);
	delay_micros(5);
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 0);
}

void Ports_in_set(uint8_t port) {
	
	uint8_t  bit = 0;
	
	uint8_t  shift[PORTS_MAX];
	
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		shift[n] = 0;
	}
	
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 0);
	
	//Fill shift value
	bit = Port_in_position[port];
		
	shift[bit] = 1;
	
	//Move data
	for (uint8_t n = 0; n < PORTS_MAX; n++) {
		
		bit = (PORTS_MAX-1) - n;
		
		HAL_GPIO_WritePin(PIN_DS2_PORT, PIN_DS2_PIN, shift[bit]);
		
		//Сдвиг на 1
		HAL_GPIO_WritePin(PIN_SH2_PORT, PIN_SH2_PIN, 1);
		delay_micros(5);
		HAL_GPIO_WritePin(PIN_SH2_PORT, PIN_SH2_PIN, 0);
	}
	
	//Запись в выход
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 1);
	delay_micros(5);
	HAL_GPIO_WritePin(PIN_ST2_PORT, PIN_ST2_PIN, 0);
	
	//IN = 163 mcs
}

void PortA_in(uint8_t p) {
	
	p = Ports_bind_in_get(0, p);
	
	Ports_in_set(p-1);
}

void PortB_in(uint8_t p) {
	
	p = Ports_bind_in_get(1, p);
	
	Ports_in_set(p-1+10);
}

void PortC_in(uint8_t p) {
	
	p = Ports_bind_in_get(2, p);
	
	Ports_in_set(p-1+20);
}

void Port_phase_in(uint8_t phase, uint8_t port) {
	
	if (phase == 1) {
		
		PortA_in(port);
		
	} else if (phase == 2) {
		
		PortB_in(port);
		
	} else {
		
		PortC_in(port);
	}
}

void Ports_logic_set(uint8_t phase, uint8_t port,  uint8_t logic) {
	
	if (logic) {
		port_logic[phase-1] |= 1 << (port - 1);
	} else {
		port_logic[phase-1] &= ~ (1 << (port-1));
	}
	eeprom_write_buffer(EE_CONT_LOGIC, (void *)&port_logic, sizeof(port_logic)*3);
}

void Ports_logic_read(void) {
	eeprom_read_buffer(EE_CONT_LOGIC, (void *)&port_logic, sizeof(port_logic)*3);
	for (uint8_t n = 0; n < 3; n++) {
		if (port_logic[n]==0xFFFF) {
			port_logic[n] = 0;
		}
	}
}

uint8_t Ports_logic_get(uint8_t phase, uint8_t port) {
	return (port_logic[phase-1] >> (port-1)) & 1;
}

uint16_t Ports_logic_port(uint8_t phase) {
	return port_logic[phase-1];
}