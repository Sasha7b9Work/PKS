/*
 * modem.h
 *
 * Created: 18.01.2017 23:49:31
 *  Author: Hyper
 */ 


#ifndef MODEM_H_
#define MODEM_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "Tasks.h"
#include "Config.h"
#include "Utility.h"
#include "Serial.h"
#include "fifo.h"

extern char lbl_OK[];
extern char lbl_err[];
extern char lbl_fail[];
extern char lbl_ok[];
extern char sep[];
extern uint8_t is_first_start;

extern bool     MODEM_ALIVE;
extern bool     MODEM_NET_REG;
extern bool     MODEM_GPRS;
extern bool     MODEM_AUTO_CONNECT;
extern bool     MODEM_CONNECTED;

extern uint32_t MODEM_LAST_SEND;
extern uint32_t MODEM_LAST_CHECK;
extern uint32_t MODEM_LAST_ALIVE;

//extern uint8_t  modem_reply;

extern char serial_data[];
extern char mess[];
extern char cmdData[];
extern uint8_t  set_val;

extern char     modem_data[];
extern uint16_t modem_get_timeout;
extern uint32_t modem_get_time;
extern char     modem_imei[];

extern uint32_t  modem_serial_task;
extern uint32_t  modem_hello_time;

extern bool     modem_is_sending;

extern char modem_srv_b1[];
extern char modem_srv_b2[];

void     modem_flash(char* buff, uint8_t i);

void     modem_task_check(void);

uint8_t  modem_cmd_process(void);

uint8_t  modem_tcp_write_start(void);
uint8_t  modem_tcp_write(char* string);
uint8_t  modem_tcp_write_end(void);

void     modem_write(char* mess);
void     modem_write_char(uint8_t c);
void     modem_write_str(char* str);
void     modem_write_tx(char* mess);
void     modem_write_prepare(void);
void     modem_write_esc(void);

void     modem_serial_init(void);
void     modem_serial_clean(void);

void     modem_connected(void);
void     modem_disconnected(void);
void     modem_connect_check(void);
bool     modem_is_connected(void);

void     modem_serial_process(void);

uint8_t  modem_at(char* ATcommand, char* expected_answer, uint16_t timeout);
uint8_t  modem_at_try(char* ATcommand, char* expected_answer, uint16_t timeout, uint8_t trys);
void     modem_at_num(char* ATcommand, uint16_t num, uint16_t timeout, char* response, uint16_t max);
uint8_t  modem_waitResp(char* expected_answer_1, char* expected_answer_2, uint16_t timeout);

bool     modem_data_check(char* answer);
bool     modem_data_num(uint16_t num, char* response, uint16_t max);
bool     modem_data_ready(void);

void     modem_status(char* s);
void     modem_status_n(uint8_t n);
void     modem_blink(uint16_t ms, uint8_t sec);
void     modem_tcp_status(char *ret, int ret_len);

bool     modem_buff_add(char* buff, char* string);
bool     modem_srv_write(char* string);
bool     modem_srv_write_ready(void);
void     modem_write_b1(void);
void     modem_write_b2(void);
void     modem_buf_cpy(void);

bool     modem_sim_detect(void);

void     modem_dt_shift(int32_t dt);

uint32_t modem_serial_time(void);

void     modem_power_on(void);
void     modem_power_off(void);
void     modem_param(char* input, uint8_t num);


bool     modem_sim_detect(void);
bool     modem_sim_change(void);
uint8_t  modem_sim_cur(void);
void     modem_phone_set(char* string);

#endif /* MODEM_H_ */
