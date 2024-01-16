#ifndef NET_H_
#define NET_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include "Utility.h"
#include "Config.h"
#include "stm32f1xx_hal.h"

typedef struct net_type {
	
	uint8_t     count;
	
	uint16_t    volt_last;
	uint16_t    volt_min;
	uint16_t    volt_max;
	uint32_t    volt_avg;
	
	float       curr_last;
	float       curr_min;
	float       curr_max;
	float       curr_avg;

	float       cos_last;
	float       cos_min;
	float       cos_max;
	float       cos_avg;

} tp_net_type;

extern uint8_t auto_points;
extern uint8_t auto_time;
extern uint8_t auto_boost;
extern uint8_t auto_kt;
extern uint8_t auto_steps_count;
extern uint16_t auto_volt_min;
extern uint16_t auto_volt_max;

extern uint16_t range_volt_min;
extern uint16_t range_volt_max;

extern uint8_t auto_volt_phase;
extern uint8_t debug_mess;
extern uint8_t debug_ports;
extern uint8_t debug_error;

extern float net_kt_volt[];
extern float net_kt_curr[];

void        net_init(void);
void        net_reset_counter();
tp_net_type *net_info(uint8_t p);

void        buffers_init(void);
void        buffers_save_last(tp_net_type *NetInfo, float volt, float curr, float coss);
uint16_t    buffers_save_one(tp_net_type *NetInfo, float fvolt, float curr, float coss);
void        buffers_save(void);

uint16_t    volt_avg_reg(uint8_t p, uint8_t count);
uint16_t    volt_avg(uint8_t p);
uint16_t    volt_last(uint8_t p);
uint16_t    volt_max(uint8_t p);
uint16_t    volt_min(uint8_t p);

float       curr_avg(uint8_t p);
float       curr_last(uint8_t p);

float       cos_avg(uint8_t p);
float       cos_last(uint8_t p);

void        net_kt_volt_set(uint8_t p, float val);
float       net_kt_volt_get(uint8_t p);

void        net_kt_curr_set(uint8_t p, float val);
float       net_kt_curr_get(uint8_t p);

void        steps_read(void);
float       steps_get(int8_t step);
void        steps_set(int8_t step, float volt);


void        steps_count_set(uint8_t step);
uint8_t     steps_count_get(void);

void        volt_minmax_set(uint16_t min, uint16_t max);
uint16_t    volt_min_get(void);
uint16_t    volt_max_get(void);

void        range_minmax_set(uint16_t min, uint16_t max);
uint16_t    range_min_get(void);
uint16_t    range_max_get(void);

void        boost_set(uint8_t on);
uint8_t     boost_get(void);

void        des_time_set(uint8_t step);
uint8_t     des_time_get(void);

void        points_set(uint8_t step);
uint8_t     points_get(void);

void        kt_set(uint8_t step);
uint8_t     kt_get(void);

void        debug_mess_set(uint8_t val);
uint8_t     debug_mess_get(void);

void        debug_error_set(uint8_t val);
uint8_t     debug_error_get(void);

void        debug_ports_set(uint8_t val);
uint8_t     debug_ports_get(void);

void        auto_volt_set(uint8_t val);
uint8_t     auto_volt_get(void);

#endif /* NET_H_ */