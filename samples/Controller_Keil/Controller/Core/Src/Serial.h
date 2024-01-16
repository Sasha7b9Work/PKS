
#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Config.h"
#include "Utility.h"
#include "Commands.h"
#include "modem.h"

#define SINGLE_BYTE		1

extern uint8_t set_val;
extern char    cmdData[CMDDATA_SIZE];
extern char    mess[MESS_SIZE];

extern char    sep[];

typedef struct rs_command_type {
	uint16_t start;
	uint16_t len;
	uint16_t sum;
} tp_rs_command_type;

void       serial_init(void);
void       serial_params_read(void);
void       serial_process(void);

void       serial_timeout_set(uint16_t timeout);
uint16_t   serial_timeout_get(void);
uint8_t    serial_rs485_running(void);
void       serial_addr_read(void);

void       serial_write_start(void);
void       serial_write(char* strout);
void       serial_write_end(void);
void       serial_write_str(char* data);
void       serial_write_char(uint8_t c);

void       serial_write_addr(char* strout);

void       serial_data_clear(void);
void       serial_data_add(uint8_t inChar);
uint16_t   serial_data_len(void);
uint16_t   serial_data_trim(void);

bool       serial_is_command(void);
uint8_t    serial_cmd_ready(void);

void       serial_rs485_init(uint8_t cmd_index, uint16_t data_index);
void       serial_rs485_save(uint8_t cmd_index, uint16_t len, uint16_t sum);
void       serial_rs485_prepare(char* data);
void       serial_rs485_send(void);
void       serial_rs485_process(void);
void       serial_rs485_reply(void);

#endif /* SERIAL_H_ */