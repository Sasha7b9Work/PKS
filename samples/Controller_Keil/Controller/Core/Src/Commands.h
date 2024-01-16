#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "Config.h"
#include "Serial.h"
#include "Utility.h"
#include "calendar.h"
#include "gpio.h"
#include "I2C.h"
#include "main.h"
#include "Tasks.h"
#include "modem.h"
#include "Arc.h"
#include "ADE7758.h"
#include "W25Q64.h"
#include "Net.h"
#include "Phase.h"
#include "stm32f1xx_hal.h"

extern char      cmdData[CMDDATA_SIZE];
extern char      mess[MESS_SIZE];
extern char      out[256];

extern char      server_ip[25];
extern uint16_t  RS485_ADDR;

extern uint32_t  t_server_wait;
extern uint32_t  acc_max;
extern uint32_t  acc_cur;

extern char     modem_phone[20];

extern RTC_HandleTypeDef hrtc;

void command_process(char* data);
void command_unknown(char* cmd);

void command_ver(char* cmd);
void command_info(char* cmd);
void command_hello(char* cmd, char* data);
void command_reset(char* cmd, char* data);
void command_task(char* cmd, char* data);
void command_time(char* cmd, char* data);
void command_gp(char* cmd, char* data);
void command_mem(char* cmd, char* data);
void command_elog(char* cmd, char* data);
void command_rom(char* cmd, char* data);
void command_romcrc(char* cmd, char* data);
void command_flash(char* cmd, char* data);
void command_server(char* cmd, char* data);
void command_addr(char* cmd, char* data);
void command_com(char* cmd, char* data);
void command_rs(char* cmd, char* data);
void command_ns(char* cmd, char* data);
void command_timer(char* cmd, char* data);
void command_set(char* cmd, char* data);
void command_wdt(char* cmd, char* data);
void command_modem(char* cmd, char* data);
void command_phone(char* cmd, char* data);
void command_gprs(char* cmd, char* data);
void command_sms(char* cmd, char* data);
void command_test(char* cmd, char* data);

void command_arc(char* cmd, char* data);
void command_bind(char* cmd, char* data);
void command_blink(char* cmd, char* data);
void command_cont(char* cmd, char* data);
void command_contlog(char* cmd, char* data);
void command_port(char* cmd, char* data);
void command_in(char* cmd, char* data);
void command_scan(char* cmd, char* data);
void command_phase(char* cmd, char* data);
void command_cal(char* cmd, char* data);
void command_net(char* cmd, char* data);
void command_run(char* cmd, char* data);
void command_step(char* cmd, char* data);
void command_debug(char* cmd, char* data);

#endif /* COMMANDS_H_ */
