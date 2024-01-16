/*
 * Utility.c
 *
 * Created: 27.01.2017 12:12:24
 *  Author: Admin
 */ 

#include "Utility.h"

//Источник команды
volatile static uint8_t cmd_src = 0; //1 - rs485 console, 2 - modem, 3 - sms

uint32_t now(void) {
	
	RTC_TimeTypeDef sTime;
	uint32_t time = 0;
	
	/*BKP_RTC_WaitForUpdate();
	time = BKP_RTC_GetCounter();*/
	
	//HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	//time = RTC_ReadTimeCounter();
	
	uint16_t high1 = 0U, high2 = 0U, low = 0U;

  high1 = READ_REG(hrtc.Instance->CNTH & RTC_CNTH_RTC_CNT);
  low   = READ_REG(hrtc.Instance->CNTL & RTC_CNTL_RTC_CNT);
  high2 = READ_REG(hrtc.Instance->CNTH & RTC_CNTH_RTC_CNT);

  if (high1 != high2)
  {
    /* In this case the counter roll over during reading of CNTL and CNTH registers,
       read again CNTL register then return the counter value */
    time = (((uint32_t) high2 << 16U) | READ_REG(hrtc.Instance->CNTL & RTC_CNTL_RTC_CNT));
  }
  else
  {
    /* No counter roll over during reading of CNTL and CNTH registers, counter
       value is equal to first value of CNTL and CNTH */
    time = (((uint32_t) high1 << 16U) | low);
  }
	
	if (!time) {
		NS_Set(NS_RTC);
	} else {
		NS_Clr(NS_RTC);
	}
	
	return time;
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
	
	if (t && t!=zval_32) {
		struct calendar_date sdate;
	
		calendar_timestamp_to_date(t, &sdate);
	
		sprintf(out, "%d-%02d-%02d %02d:%02d:%02d", sdate.year, sdate.month+1, sdate.date+1, sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}
}
void dateFormatS(uint32_t t, char* out) {

	if (t && t!=zval_32) {
		struct calendar_date sdate;
		
		calendar_timestamp_to_date(t,&sdate);
		
		sprintf(out,"%d%02d%02d%02d%02d%02d", sdate.year, sdate.month+1, sdate.date+1, sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}
	
}
void timeFormat(uint32_t t, char* out) {
	if (t) {
		struct calendar_date sdate;
		
		calendar_timestamp_to_date(t,&sdate);
		
		sprintf(out, "%02d:%02d:%02d", sdate.hour, sdate.minute, sdate.second);
	} else {
		clearString(out);
		strcat(out, "none");
	}
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
			sys_wdt_reset();
			wdt = m;
		}
		
	} while (m - t < ms);
}

void delay_micros(uint32_t mcs) {
	
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	counter_micros = 0;
	
	HAL_TIM_Base_Start(&htim3);
	
	while (counter_micros + __HAL_TIM_GET_COUNTER(&htim3) < mcs);
	
	HAL_TIM_Base_Stop(&htim3);
}

void counter_micros_start(void) {
	
	__HAL_TIM_SET_COUNTER(&htim3, 0);
	counter_micros = 0;
	HAL_TIM_Base_Start(&htim3);
}

uint32_t counter_micros_stop(void) {
	
	HAL_TIM_Base_Stop(&htim3);
	
	return counter_micros + __HAL_TIM_GET_COUNTER(&htim3);
}

uint32_t counter_micros_cur(void) {
	return counter_micros + __HAL_TIM_GET_COUNTER(&htim3);
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

void uint32_to_hex(uint32_t t, char* out) {
	
	uint8_t *byte_lu = (uint8_t *) &t;
	
	sprintf(out, "%02X%02X%02X%02X", byte_lu[3], byte_lu[2], byte_lu[1], byte_lu[0]);
}

void bytes_dump_str(void *data, uint8_t len, char* out) {
	
	uint8_t b = 0;
	const char *hex = "0123456789ABCDEF";
	char  buf_str[5];
	
	clearString(out);
	
	while (len) {
		
		b = *(uint8_t*)data;
		
		buf_str[0] = hex[(b >> 4) & 0xF];
		buf_str[1] = hex[b & 0xF];
		buf_str[2] = 0x00;
		
		strcat(out, buf_str);

		data = (uint8_t*)data + 1;
		len--;
	}
}

bool bytes_check(void *data1, void *data2, uint8_t len) {
	
	uint8_t b1 = 0;
	uint8_t b2 = 0;
	bool is_ok = true;
	
	while (len) {
		
		b1 = *(uint8_t*)data1;
		b2 = *(uint8_t*)data2;
		
		if (b1 != b2) {
			is_ok = false;
			break;
		}

		data1 = (uint8_t*)data1 + 1;
		data2 = (uint8_t*)data2 + 1;
		
		len--;
	}
	return is_ok;
}

uint16_t wdt_max_read(void) {
	uint16_t max = eeprom_read_uint16(EE_WDT_MAX);
	if (max == zval_16) {
		max = 0;
	}
	return max;
}

void wdt_max_write(uint16_t max) {
	if (!max) {
		max = zval_16;
	}
	eeprom_write_uint16(EE_WDT_MAX, max);
	
	sys_wdt_upd();
}

void wdt_cnt_reset(void) {
	counter_wdt = 0;
}

uint32_t wdt_cnt(void) {
	return counter_wdt;
}

void wdt_reset_check(uint16_t max) {
	if (max) {
		if (counter_wdt > (uint32_t)max * 60) {
			serial_write_addr("WDT reset");
			NVIC_SystemReset();
		}
	}
}