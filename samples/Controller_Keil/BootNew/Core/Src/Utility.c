/*
 * Utility.c
 *
 * Created: 27.01.2017 12:12:24
 *  Author: Admin
 */ 

#include "Utility.h"

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
			HAL_GPIO_TogglePin(PIN_WDT_PORT, PIN_WDT_PIN);
			wdt = m;
		}
		
	} while (m - t < ms);
}

/*
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
*/
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

void blink(void) {
	for (uint8_t n = 0; n < 20; n++) {

			HAL_GPIO_TogglePin(PIN_LED_PORT, PIN_LED_PIN);

			delay_millis(50);
	}
}