#ifndef UTILITY_H_
#define UTILITY_H_

/*
#include <MDR32F9Q2I.h>
#include <MDR32FxQI_rst_clk.h>
#include <MDR32FxQI_timer.h>
#include <MDR32FxQI_bkp.h>*/

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

#define Max(a, b)           (((a) > (b)) ?  (a) : (b))
#define max(a, b)           Max(a, b)

extern uint8_t IS_RESET;
extern uint16_t NS_BITS;

void Timer2_Init(void);
void Timer2_IRQHandler(void);
void Timer1_Init(void);

uint32_t now(void);
//uint32_t now_rtc(void);
//void now_set(uint32_t time);

void dateFormat(uint32_t t, char* out);
void timeFormat(uint32_t t, char* out);
void dateFormatS(uint32_t t, char* out);

void addChar(char* input, char c);
void Trim(char* input);
uint8_t toHex( char ch );
void subString(const char *input,int offset,int len,char *dest);

void clearString(char* input);
void shiftString(char* input, int num);
void HexString(uint8_t *input, uint16_t len, char* result);
void shiftData(char* input, int num, uint16_t len);

void paramString     (char* input, uint16_t num, char* ret);
void paramStringC    (char* input, uint16_t num, char ch, char* ret);
void paramStringMax  (char* input, uint16_t num, char* ret, uint16_t max);
uint8_t paramStringNum  (const char* input, uint16_t num, uint16_t *start, uint16_t *len);
void lines_num       (uint16_t num, char* lines, char* response, uint16_t max);

bool  Temperature(uint8_t num, float *temp);
float Temperature_body(void);
float Temperature_board(void);

uint32_t millis(void);
void delay_millis(uint32_t ms);
void delay_micros(uint32_t mcs);

void counter_micros_start(void);
uint32_t counter_micros_stop(void);


void task_wdt_reset(void);
void watchdog_reset(void);

bool    Timers_temp(void);

bool    checkTimeOut(uint32_t last_time, uint16_t timeout);
bool    bitRead(uint16_t number, uint16_t x);

bool    NS_Get(uint16_t x);
void    NS_Set(uint16_t x);
void    NS_Clr(uint16_t x);

uint16_t crc16(uint8_t *data, uint16_t length, uint16_t crc_init);

void    temp_minmax_set(int16_t min, int16_t max);
int16_t temp_min_get(void);
int16_t temp_max_get(void);
uint8_t temp_addr(uint8_t c);

void    bell_start(void);
void    bell_stop(void);

uint8_t cmd_src_read(void);
void    cmd_src_write(uint8_t src);

void    phone_num(uint8_t pos, char* out);

uint16_t wdt_max_read(void);
void     wdt_max_write(uint16_t max);

void    buf_save(void);

#endif /* UTILITY_H_ */
