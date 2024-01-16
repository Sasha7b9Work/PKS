#ifndef _GPIO_H
#define _GPIO_H

#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "stdbool.h"
#include "Config.h"
#include "Utility.h"
#include "eeprom.h"
#include "string.h"
//#include "ADC.h"


#define GP_MODE_OUT                 0
#define GP_MODE_ADC                 1
#define GP_MODE_INU                 2
#define GP_MODE_IND                 3
#define GP_MODE_IN                  4
#define GP_MODE_PWM                 5
#define GP_MODE_REG                 6

#define GP_STORE                    32
#define GP_STORE_MODE               0
#define GP_STORE_CONF               1
#define GP_STORE_CNT                3
#define GP_STORE_MIN                3
#define GP_STORE_MAX                7
#define GP_STORE_KT                 11
#define GP_STORE_ZVAL               15
#define GP_STORE_DTUPD              17
#define GP_STORE_DTPRV              21
#define GP_STORE_PHONE              25
#define GP_STORE_STATE              26

#define GP_CONFBIT_BIND             0
#define GP_CONFBIT_BIND_INV         4
#define GP_CONFBIT_LOG              5
#define GP_CONFBIT_SAVE             6
#define GP_CONFBIT_TPSCAN           6
#define GP_CONFBIT_COUNTER          7
#define GP_CONFBIT_MINBIND          10
#define GP_CONFBIT_MININV           14
#define GP_CONFBIT_INV_OUT          8
#define GP_CONFBIT_REG              11
#define GP_CONFBIT_JRN              15
#define GP_CONFBIT_INIT_TRUE        9
#define GP_CONFBIT_OPEN_DRAIN				10

#define NO_PULLUPDOWN		0
#define PULL_UP					1
#define PULL_DOWN				2

typedef void (*CallBack_t)(uint8_t num, uint8_t state);

typedef struct gpio_type {
	
	uint8_t      		mode;
	uint8_t					 	pin;
	uint16_t     			config;
	uint8_t      			val;
	uint32_t     			max;
	int16_t      			zvalue;
	float        			kt;
	uint8_t      			delay;   //??????? ????????
	uint32_t     			counter; //??????? ?????????
	uint32_t     			dtupd;
	uint16_t     			call_delay;
	uint8_t      			call_retries;
	CallBack_t				onCallBack;
} tp_gpio_type;

typedef struct gpio_def {
	uint8_t      			mod;
	uint16_t     			config;
	GPIO_TypeDef* 		port;
	uint16_t					pin;
} tp_gpio_def;

typedef struct gp_log_type {
	uint32_t dt;              //4
	uint8_t  tp;              //1
	uint8_t  num;             //1
	char     info[10];        //10
} tp_gp_log_type; //16


void gp_init(void);
void gp_conf_init(uint8_t num);
tp_gpio_type* gp_port(uint8_t num);
uint8_t gp_mode(uint8_t num);
bool gp_conf_bit(uint16_t conf, uint8_t x);
uint16_t gp_conf(uint8_t num);
uint8_t gp_bind(uint8_t num);
void gp_bind_renew(uint8_t num);
void gp_conf_write(uint8_t num, uint16_t conf);
void gp_bind_set(uint8_t num, uint8_t bind, uint8_t inverse);
uint8_t gp_get_state(uint8_t num);
uint8_t gp_bind_inverse(uint8_t num);
void gp_set(uint8_t num, uint8_t state);
void ioport_set_pin_level(uint8_t num, bool level);
bool gp_get(uint8_t num);
uint8_t ioport_get_pin_level(uint8_t pin);
void gp_mode_out(uint8_t num);
void ioport_set_pin_mode(uint8_t pin, uint32_t mode, uint8_t pull);
void gp_mode_in(uint8_t num);
void gp_set_state(uint8_t num, uint8_t state);
void gp_bind_value(uint8_t p, uint8_t state);
void gp_value(uint8_t num, uint8_t format, char* ret, bool dt_short);
uint32_t gp_get_upd(uint8_t num);
uint16_t gp_dtCall_get(uint8_t num);
uint16_t gp_call_retries_get(uint8_t num);
void gp_mode_save(uint8_t num, uint8_t mod);
void gp_kt_save(uint8_t num, float fvalue);
float gp_kt_read(uint8_t num);
int16_t gp_adc_rvalue(uint8_t num);
int16_t gp_zvalue_read(uint8_t num);
void gp_zvalue_write(uint8_t num, int16_t zvalue);
void gp_min_saveF(uint8_t num, float value);
uint32_t gp_min_read(uint8_t num);
void gp_bind_min_set(uint8_t num, uint8_t bind, uint8_t inverse);
float gp_min_getF(uint8_t num);
uint8_t gp_bind_min(uint8_t num);
uint8_t gp_bind_inverse_min(uint8_t num);
void gp_max_saveF(uint8_t num, float value);
float gp_max_getF(uint8_t num);
void gp_conf_write_bit(uint8_t num, uint8_t bit, uint16_t val);
void gp_conf_write_delay(uint8_t num, uint16_t val);
void gp_max_save(uint8_t num, uint32_t value);
uint8_t gp_imp_delay(uint8_t num);
uint32_t gp_max_get(uint8_t num);
uint8_t gp_delay(uint8_t num);
void gp_call_mode_set(uint8_t num, uint8_t mode);
void gp_call_save(uint8_t num, uint8_t rec, uint8_t calls, uint8_t interval);
uint8_t gp_call_read(uint8_t num);
uint8_t gp_call_rec(uint8_t byte);
uint8_t gp_call_number(uint8_t byte);
uint8_t gp_call_delay(uint8_t byte);
uint8_t gp_call_mode_get(uint8_t num);
uint32_t gp_cnt_get(uint8_t num);
void gp_cnt_set(uint8_t num, uint32_t counter);
void gp_call_reset(uint8_t num);
void gp_dtCall_set(uint8_t num, uint16_t sec);
void gp_dtCall_reset(uint8_t num);
void gp_call_retries_set(uint8_t num, uint8_t n);
uint32_t gp_max_read(uint8_t num);
void gp_reg_renew(void);
float    gp_adc_val(uint8_t num);
void gp_bind_renew_adc(uint8_t num, bool is_min, bool state, bool is_log);
void gp_dt_save(uint8_t num);
void gp_reg_bit(uint8_t bit, uint8_t val);
void gp_call_start(uint8_t num, uint8_t state);
void gp_call_init(uint8_t num);
uint8_t gp_reg_val(uint8_t num);
uint32_t gp_get_prv(uint8_t num);
uint8_t gp_mode_read(uint8_t num);
uint16_t gp_conf_read(uint8_t num);
void gp_input_process_ms(void);
void gp_input_process_sec(void);
void gp_input_check(uint8_t num);
void gp_dt2_save(uint8_t num);
void gp_conf_save(uint8_t num, uint8_t mod, uint16_t conf);
void gp_input_adc(uint8_t num);
uint16_t gp_log_idx(void);
uint16_t gp_log_reidx(void);
uint16_t gp_log_last(void);
bool gp_log_rec(uint16_t rec, tp_gp_log_type *log_rec);
void gp_log_cnt_set(uint16_t cnt);
uint16_t gp_log_cnt_get(void);
bool gp_log_add(uint8_t tp, uint8_t num, char* info);
void gp_log_clear(void);
void gp_log_fill(void);
void gp_log_idx_write(uint16_t idx);
uint16_t gp_log_idx_read(void);
void gp_log_state(uint8_t num, char* strval);
void gp_pin_init(uint8_t pin);
void gp_set_callback(uint8_t num, CallBack_t CallBack);
void gpin_init(uint8_t num);
void gp_rst_clk(GPIO_TypeDef* MDR_PORTx);
void gp_port_init(GPIO_TypeDef* GPIOx, uint16_t PORT_Pin, uint32_t PORT_MODE, uint8_t pullupdown);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

/*
void gpin_init_analog(MDR_PORT_TypeDef* MDR_PORTx, PORT_Pin_TypeDef PORT_Pin, PORT_OE_TypeDef PORT_OE);
void gpin_init(uint8_t pin, MDR_PORT_TypeDef* MDR_PORTx, PORT_Pin_TypeDef PORT_Pin, PORT_OE_TypeDef PORT_OE);
void gpin_init_pull_up(MDR_PORT_TypeDef* MDR_PORTx, PORT_Pin_TypeDef PORT_Pin);
void gpin_init_pull_down(MDR_PORT_TypeDef* MDR_PORTx, PORT_Pin_TypeDef PORT_Pin);
*/

#endif //_GPIO_H
