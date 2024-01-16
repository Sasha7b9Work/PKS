/*
 * Utility.c
 *
 * Created: 27.01.2017 12:12:24
 *  Author: Admin
 */ 

#include "Utility.h"

//volatile static uint32_t counter_sec = 0;
volatile static uint32_t counter_millis = 0;


//Источник команды
volatile static uint8_t cmd_src = 0; //1 - rs485 console, 2 - modem, 3 - sms

uint32_t now(void) {
	
	/*
	uint32_t time;
	
	BKP_RTC_WaitForUpdate();
	time = BKP_RTC_GetCounter();
	
	if (!time) {
		NS_Set(NS_RTC);
	} else {
		NS_Clr(NS_RTC);
	}
	
	return time;*/
	//This need to be cleared
	return 0;
}

uint8_t cmd_src_read(void) {
	
	return cmd_src;
}
void cmd_src_write(uint8_t src) {
	cmd_src = src;
}

void addChar(char* input, char c) {
	uint16_t len = strlen (input);
	input[len]=c;
	input [len+1]='\0';
}

void clearString(char* input) {
	memset( input, 0, strlen(input) );
	input[0] = '\0';
}

void shiftString(char* input, int num) {
	
	uint16_t len = strlen (input);
	uint16_t p = 0;
	uint16_t i = 0;

	for (i = num; i < len; i++) {
		char c = input[i];
		input[p] = c;
		p++;
	}
	input[p]='\0';
}
void shiftData(char* input, int num, uint16_t len) {
	
	uint16_t p = 0;
	uint16_t i = 0;

	for (i = num; i < len; i++) {
		input[p] = input[i];
		p++;
	}
}

void HexString(uint8_t *input, uint16_t len, char* result) {
	
	const char *hex = "0123456789ABCDEF";
	char buf_str[3] = "";
	uint16_t i = 0;
	uint8_t  b = 0;
	
	for (i = 0; i < len; i++) {
		b = input[i];
		
		buf_str[0] = hex[(b >> 4) & 0xF];
		buf_str[1] = hex[b & 0xF];
		buf_str[2] = 0x00;
		
		strcat(result, buf_str);
	}
}

void dateFormat(uint32_t t, char* out) {
	
	/*
	if (t && t!=zval_32) {
		struct calendar_date sdate;
	
		calendar_timestamp_to_date(t, &sdate);
	
		sprintf(out, "%d-%02d-%02d %02d:%02d:%02d", sdate.year, sdate.month+1, sdate.date+1, sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}*/
}
void dateFormatS(uint32_t t, char* out) {
	/*
	if (t && t!=zval_32) {
		struct calendar_date sdate;
		
		calendar_timestamp_to_date(t,&sdate);
		
		sprintf(out,"%d%02d%02d%02d%02d%02d", sdate.year, sdate.month+1, sdate.date+1, sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}*/
	
}
void timeFormat(uint32_t t, char* out) {
	/*
	if (t) {
		struct calendar_date sdate;
		
		calendar_timestamp_to_date(t,&sdate);
		
		sprintf(out, "%02d:%02d:%02d", sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}*/
}
void Trim(char* input)
{
	int i = 0;
	int len = 0;
	while (isspace (input[i])) {
		i++;
	}
	if (i>0) {
		shiftString(input, i);
	}
	len = strlen (input);
	if (len>0) {
		i = len - 1;
		
		while (isspace (input[i]) && i>=0) {
			i--;
		}
		input[i+1]='\0';
	}
}
uint8_t toHex( char ch ) {
	return ( ( ch >= 'A' ) ? ( ch - 'A' + 0xA ) : ( ch - '0' ) ) & 0x0F;
}


void paramString (char* input, uint16_t num, char* ret)
{
	paramStringC(input, num, '|', ret);
}
void paramStringC (char* input, uint16_t num, char ch, char* ret)
{
	uint16_t  cnt = 1;
	uint16_t i = 0;
	char c;
	
	clearString(ret);

	for (i = 0; i < strlen (input); i++) {
		c = input[i];
		if (c==ch) {
			cnt++;
			if (cnt>num) {
				break;
			}
		} else if (num==cnt) {
			addChar(ret, c);
		}
	}
	Trim(ret);
}
void paramStringMax (char* input, uint16_t num, char* ret, uint16_t max)
{
	uint16_t  cnt = 1;
	uint16_t i = 0;
	uint16_t n = 0;
	char c;
	
	clearString(ret);

	for (i = 0; i < strlen (input); i++) {
		c = input[i];
		if (c=='|') {
			cnt++;
			if (cnt > num) {
				break;
			}
		} else if (num==cnt) {
			addChar(ret, c);
			n++;
			if (n>=max-1) {
				break;
			}
		}
	}
	Trim(ret);
}
uint8_t paramStringNum (const char* input, uint16_t num, uint16_t *start, uint16_t *len)
{
	uint16_t  cnt = 1;
	uint16_t i = 0;
	uint8_t found = 0;
	
	*start = 0;
	*len = 0;

	for (i = 0; i < strlen (input); i++) {
		
		if (input[i] == '|') {
			cnt++;
			if (cnt>num) {
				break;
			}
		} else if (num==cnt) {
			if (!found) {
				found = 1;
				*start = i;
			}
			(*len)++;
		}
	}
	return found;
}

bool Temperature(uint8_t num, float *temp) {
	/*
	DeviceAddress TempSensAddr;
	bool ret = false;
	*temp  = -127;
	
	uint8_t dev = temp_addr(num);
	
	Dallas_begin();
	
	if (Dallas_getDeviceCount()) {
		//Три попытки
		for (uint8_t n = 0; n < 3; n++) {
			if (Dallas_getAddress(TempSensAddr, dev)) {
				if (Dallas_requestTemperaturesByAddress(TempSensAddr)) {
					*temp = Dallas_getTempC(TempSensAddr);
				
					ret = true;
					break;
				}
			}
			delay_millis(10);
		}
	}
	return ret;*/
	//This need to be cleared
	return 0;
}

float Temperature_body(void) {
	/*
	float temp;
	uint8_t num = is_Power() ? 1 : 2;
	
	if (!Temperature(num, &temp)) {
		//Читаем второй датчик
		if (Temperature(num==1 ? 2 : 1, &temp)) {
			NS_Clr(NS_TEMP);
		} else {
			NS_Set(NS_TEMP);
		}
	} else {
		NS_Clr(NS_TEMP);
	}
	
	return temp;*/
	//This need to be cleared
	return 0;
}

float Temperature_board(void) {
	/* nah
	float temp = DS3221_temperature();
	if (temp!=-1000) {
		return temp;
	} else {
		return 20.0;
	}*/
	
	return 0;
}

void lines_num(uint16_t num, char* lines, char* response, uint16_t max) {
	
	uint8_t  i = 0;
	uint8_t  answer = 0;
	
	clearString(response);

	Trim(lines);
	if (strlen(lines) > 0) {
		for (uint16_t n = 0; n < strlen(lines); n++) {
			
			if (lines[n] == 0x0D) {
				Trim(response);
				if (strlen(response) > 0) {
					i++;
					if (i == num) {
						answer = 1;
					} else if (answer == 0) {
						clearString(response);
					}
				}
			} else if (answer == 0 && strlen(response) < max-2) {
				addChar(response, lines[n]);
			}
			
		}
	}
}

void subString(const char *input,int offset,int len,char *dest)
{
	int input_len = strlen (input);
	
	if (!len) {
		len = input_len - offset;
	} else if (offset + len > input_len) {
		len = input_len - offset;
	}
	
	strncpy (dest, input + offset, len);

	dest[len]='\0';
}

void task_wdt_reset(void) {
	/*
	if (!IS_RESET) {
		wdt_reset();
	}*/
	
	//ioport_toggle_pin_level( PIN_WDT );
	
	//То, что внизу надо раскомментить
	/*if (PORT_ReadInputDataBit(PIN_WDT_PORT, PIN_WDT_PIN)) {
		PORT_ResetBits(PIN_WDT_PORT, PIN_WDT_PIN);
	} else {
		PORT_SetBits(PIN_WDT_PORT, PIN_WDT_PIN);
	}*/
}

void bell_start(void)
{
	//ioport_set_pin_level( PIN_BELL, true );
	//>PORT_SetBits(PIN_BELL_PORT, PIN_BELL_PIN);
}
void bell_stop(void)
{
	//ioport_set_pin_level( PIN_BELL, false );
	//>PORT_ResetBits(PIN_BELL_PORT, PIN_BELL_PIN);
}

void counter_micros_start(void) {
	//>TIMER_SetCounter(TIMER1, 0);
	//>TIMER_Cmd(TIMER1, ENABLE);
}

uint32_t counter_micros_stop(void) {
	
	//Частота таймера - 40 МГц
	//>uint32_t micros = TIMER1->CNT / 40;
	
	//>TIMER_Cmd(TIMER1, DISABLE);
	
	return 0;//>micros;
}

uint32_t millis(void) {
	return counter_millis;
}
void delay_millis(uint32_t ms) {
	uint32_t t = millis();
	uint32_t wdt = t;
	uint32_t m   = t;
	do {
		
		m = millis();
		
		if (m - wdt > 100)  {
			watchdog_reset();
			wdt = m;
		}
		
	} while (m - t < ms);
}

void delay_micros(uint32_t mcs) {
	
	//>TIMER_SetCounter(TIMER1, 0);
	//>TIMER_Cmd(TIMER1, ENABLE);
	
	//>while (TIMER1->CNT < mcs);
	
	//>TIMER_Cmd(TIMER1, DISABLE);
}

void watchdog_reset(void) {
	
	/*
	if (!IS_RESET) {
		wdt_reset();
	}
	
	//ioport_toggle_pin_level( PIN_WDT );
	if (PORT_ReadInputDataBit(PIN_WDT_PORT, PIN_WDT_PIN)) {
		PORT_ResetBits(PIN_WDT_PORT, PIN_WDT_PIN);
	} else {
		PORT_SetBits(PIN_WDT_PORT, PIN_WDT_PIN);
	}*/
}

bool Timers_temp(void) {
	
	/*
	float temp_min = (float)temp_min_get();
	float temp_max = (float)temp_max_get();
	
	//Температура внутри корпуса. Учитывается питание и возможная неисправность одного из датчиков.
	float temp = Temperature_body();
		
	if (NS_Get(NS_TEMP)) {
			
		//Ни один из датчиков не работает, врубаем оба нагрева
		gp_set_state(GP_HEAT, 1);
		#ifdef HEAT_SPLIT
			gp_set_state(GP_HEAT_2, 1);
		#endif
			
	} else {
		
		if (!is_Power()) {
			//Если мы на батарейном питании, то пороги ниже
			temp_min = 1;
			temp_max = 3;
		}
		
		if (gp_get_state(GP_HEAT)) {
			//Нагреватель включен
			if (temp >= temp_max) {
				gp_set_state(GP_HEAT, 0);
			}
		} else {
			//Нагреватель выключен
			if (temp <= temp_min) {
				gp_set_state(GP_HEAT, 1);
			}
		}
		
		#ifdef HEAT_SPLIT
			float temp_inn;
			
			//Если не удается прочитать внутренний датчик температуры, то берем внешний
			if (!Temperature(2, &temp_inn)) {
				temp_inn = temp;
			}
			if (gp_get_state(GP_HEAT_2)) {
				//Нагреватель включен
				if (temp_inn >= temp_max) {
					gp_set_state(GP_HEAT_2, 0);
				}
			} else {
				//Нагреватель выключен
				if (temp_inn <= temp_min) {
					gp_set_state(GP_HEAT_2, 1);
				}
			}
		#endif
	}*/
	
	//This need to be cleared
	return false;
}

bool checkTimeOut(uint32_t last_time, uint16_t timeout) {

	int32_t t_diff = millis() - last_time;

	if (t_diff >= timeout || t_diff < 0) {

		return true;
	} else {
		return false;
	}
}

bool bitRead(uint16_t number, uint16_t x) {
	return (number >> x) & 1;
}

bool NS_Get(uint16_t x) {
	//>return (NS_BITS >> x) & 1;
	return false;
}
void NS_Set(uint16_t x) {
	/*
	if (!NS_Get(x)) {
		gp_log_add(ELOG_NS_ON, x, "");
		
		if (MODEM_CONNECTED) {
	
			char line[32];
			sprintf(line, "ns|on|%u|%02X", x, NS_BITS);
	
			modem_srv_write(line);
		}
	}
	NS_BITS |= 1 << x;*/
}
void NS_Clr(uint16_t x) {
	/*
	if (NS_Get(x)) {
		gp_log_add(ELOG_NS_OFF, x, "");
		
		if (MODEM_CONNECTED) {
			
			char line[32];
			sprintf(line, "ns|off|%u|%02X", x, NS_BITS);
			
			modem_srv_write(line);
		}
	}
	NS_BITS &= ~(1 << x);*/
}
uint16_t crc16(uint8_t *data, uint16_t length, uint16_t crc_init) {
	
	//unsigned short crc = 0xFFFF;
  unsigned char i;
	uint16_t poly = 0x1021;
	uint16_t mask = 0x8000;

  while (length--)
  {
    crc_init ^= *data++ << 8;

    for (i = 0; i < 8; i++)
      crc_init = (crc_init & mask) ? ((crc_init << 1) ^ poly) : (crc_init << 1);
  }
  return crc_init;
}

void temp_minmax_set(int16_t min, int16_t max) {
	/*
	eeprom_write_byte(EE_TEMP_SET, set_val);
	eeprom_write_uint16(EE_TEMP_MIN, min);
	eeprom_write_uint16(EE_TEMP_MAX, max);*/
	
}

int16_t temp_min_get(void) {
	/*
	uint8_t set = eeprom_read_byte(EE_TEMP_SET);
	
	if (set == set_val) {
		return eeprom_read_uint16(EE_TEMP_MIN);
	} else {
		return TEMP_HEAT_ON;
	}*/
	//This need to be cleared
	return 0;
}

int16_t temp_max_get(void) {
	
	/*
	uint8_t set = eeprom_read_byte(EE_TEMP_SET);
	
	if (set == set_val) {
		
		return eeprom_read_uint16(EE_TEMP_MAX);
	} else {
		return TEMP_HEAT_OFF;
	}*/
	//This need to be cleared
	return 0;
}

uint8_t temp_addr(uint8_t c) {
	/*
	uint8_t ival = 0;
	if (c==1) {
		ival = eeprom_read_byte(EE_T1_NUM);
	}
	if (c==2) {
		ival = eeprom_read_byte(EE_T2_NUM);
	}
	if (ival == zval_8) {ival = 0;}
	return ival;*/
	//This need to be cleared
	return 0;
}

void phone_num(uint8_t pos, char* out) {
	/*
	uint8_t  phone[PHONES_REC_SIZE];
	clearString(out);
	char str[16] = "";
	
	eeprom_read_buffer(PHONES_ADDR_START + (pos-1)*PHONES_REC_SIZE, (void *)&phone, PHONES_REC_SIZE);
	if (phone[0] != zval_8) {
		
		strcat(out, "+7");

		for (uint8_t n = 0; n < PHONES_REC_SIZE; n++) {
			sprintf(str, "%02X", phone[n]) ;
			strcat(out, str);
		}
	}*/
}
uint16_t wdt_max_read(void) {
	/*
	uint16_t max = eeprom_read_uint16(EE_WDT_MAX);
	if (max == zval_16) {
		max = 0;
	}
	return max;*/
	//This need to be cleared
	return 0;
}
void wdt_max_write(uint16_t max) {
	/*
	if (!max) {
		max = zval_16;
	}
	eeprom_write_uint16(EE_WDT_MAX, max);*/
}
