#ifndef UTILITY_H_
#define UTILITY_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>

#include "main.h"

extern volatile uint32_t counter_millis;

void addChar(char* input, char c);
void Trim(char* input);
uint8_t toHex( char ch );
void subString(const char *input,int offset,int len,char *dest);

void clearString(char* input);
void shiftString(char* input, int num);
void HexString(uint8_t *input, uint16_t len, char* result);
void shiftData(char* input, int num, uint16_t len);

void     paramString     (char* input, uint16_t num, char* ret);
void     paramStringC    (char* input, uint16_t num, char ch, char* ret);
void     paramStringMax  (char* input, uint16_t num, char* ret, uint16_t max);
uint8_t  paramStringNum  (const char* input, uint16_t num, uint16_t *start, uint16_t *len);
void     lines_num       (uint16_t num, char* lines, char* response, uint16_t max);

uint32_t millis(void);
void     delay_millis(uint32_t ms);


void     task_wdt_reset(void);
void     watchdog_reset(void);

bool     checkTimeOut(uint32_t last_time, uint16_t timeout);
bool     bitRead(uint16_t number, uint16_t x);

uint16_t crc16(uint8_t *data, uint16_t length, uint16_t crc_init);

uint8_t  cmd_src_read(void);
void     cmd_src_write(uint8_t src);

void     phone_num(uint8_t pos, char* out);

uint16_t wdt_max_read(void);
void     wdt_max_write(uint16_t max);

void     blink(void);

#endif /* UTILITY_H_ */
