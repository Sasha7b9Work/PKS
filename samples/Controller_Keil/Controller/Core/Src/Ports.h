/*
 * Ports.h
 *
 * Created: 04.08.2017 10:16:24
 *  Author: Admin
 */ 


#ifndef PORTS_H_
#define PORTS_H_

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "Config.h"
#include "Utility.h"
//#include "Timers.h"
//#include <string.h>
//#include "Tasks.h"

extern uint16_t port_import_count;

void       Ports_init(uint8_t wdt_reset);
void       Ports_renew(void);
void       Ports_clear(void);

bool       Ports_scan_busy(uint8_t p);
void       Ports_scan_mode(uint8_t mode);

bool       Ports_scan_P1(void);
bool       Ports_scan_P2(void);

uint8_t    Port_get(uint8_t phase, uint8_t port);
void       Port_set(uint8_t phase, uint8_t port);
void       Port_clr(uint8_t phase, uint8_t port);

void       PortA_set(uint8_t p);
void       PortA_clr(uint8_t p);
uint8_t    PortA_get(uint8_t p);

void       PortB_set(uint8_t p);
void       PortB_clr(uint8_t p);
uint8_t    PortB_get(uint8_t p);

void       PortC_set(uint8_t p);
void       PortC_clr(uint8_t p);
uint8_t    PortC_get(uint8_t p);

void       Ports_LED(void);

void       Ports_read(void);

void       Ports_bind_out_read(void);
uint8_t    Ports_bind_out_get(uint8_t phase, uint8_t port);
void       Ports_bind_out_set(uint8_t phase, uint8_t port, uint8_t value);

void       Ports_bind_in_read(void);
uint8_t    Ports_bind_in_get(uint8_t phase, uint8_t port);
void       Ports_bind_in_set(uint8_t phase, uint8_t port, uint8_t value);

void       Port_phase_in(uint8_t phase, uint8_t port);
void       PortA_in(uint8_t p);
void       PortB_in(uint8_t p);
void       PortC_in(uint8_t p);
void       Ports_in_set(uint8_t port);
void       Ports_in_clr(void);

void       Ports_logic_read(void);
void       Ports_logic_set(uint8_t phase, uint8_t port,  uint8_t logic);
uint8_t    Ports_logic_get(uint8_t phase, uint8_t port);
uint16_t   Ports_logic_port(uint8_t phase);

uint8_t    Ports_scan_get(void);

#endif /* PORTS_H_ */