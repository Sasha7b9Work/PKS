#ifndef UTILITY_H_
#define UTILITY_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include <stdio.h>


//#include "Lib/calendar.h"

/*
#include "Config.h"
#include "Timers.h"
#include "eeprom.h"
#include "Tasks.h"
#include "wdt.h"
#include "gpio.h"
#include "Water.h"*/

#include "main.h"

#define Max(a, b)           (((a) > (b)) ?  (a) : (b))
#define max(a, b)           Max(a, b)

extern uint8_t IS_RESET;
extern uint16_t NS_BITS;

extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim3;
extern uint8_t           set_val;

extern volatile uint32_t counter_millis;
extern volatile uint32_t counter_micros;
extern volatile uint32_t counter_wdt;

uint32_t now(void);

void     dateFormat(uint32_t t, char* out);
void     timeFormat(uint32_t t, char* out);
void     dateFormatS(uint32_t t, char* out);

void     addChar(char* input, char c);
void     Trim(char* input);
uint8_t  toHex( char ch );
void     subString(const char *input,int offset,int len,char *dest);

void     clearString(char* input);
void     shiftString(char* input, int num);
void     HexString(uint8_t *input, uint16_t len, char* result);
void     shiftData(char* input, int num, uint16_t len);

void     paramString     (char* input, uint16_t num, char* ret);
void     paramStringC    (char* input, uint16_t num, char ch, char* ret);
void     paramStringMax  (char* input, uint16_t num, char* ret, uint16_t max);
uint8_t  paramStringNum  (const char* input, uint16_t num, uint16_t *start, uint16_t *len);
void     lines_num       (uint16_t num, char* lines, char* response, uint16_t max);

uint32_t millis(void);
void     delay_millis(uint32_t ms);
void     delay_micros(uint32_t mcs);

void     counter_micros_start(void);
uint32_t counter_micros_stop(void);
uint32_t counter_micros_cur(void);


void     task_wdt_reset(void);
void     watchdog_reset(void);

bool     checkTimeOut(uint32_t last_time, uint16_t timeout);
bool     bitRead(uint16_t number, uint16_t x);

bool     NS_Get(uint16_t x);
void     NS_Set(uint16_t x);
void     NS_Clr(uint16_t x);

uint16_t crc16(uint8_t *data, uint16_t length, uint16_t crc_init);

uint8_t  cmd_src_read(void);
void     cmd_src_write(uint8_t src);

void     phone_num(uint8_t pos, char* out);

uint16_t wdt_max_read(void);
void     wdt_max_write(uint16_t max);
uint32_t wdt_cnt(void);
void     wdt_cnt_reset(void);
void     wdt_reset_check(uint16_t max);

void     uint32_to_hex(uint32_t t, char* out);
void     bytes_dump_str(void *data, uint8_t len, char* out);
bool     bytes_check(void *data1, void *data2, uint8_t len);

#endif /* UTILITY_H_ */
