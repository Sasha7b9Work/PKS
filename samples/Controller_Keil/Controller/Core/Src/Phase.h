/*
 * Phase.h
 *
 * Created: 21.11.2017 23:37:36
 *  Author: Hyper
 */ 


#ifndef PHASE_H_
#define PHASE_H_

#include "Ports.h"
#include "calendar.h"
#include "Tasks.h"

typedef struct regulator_type {
	uint8_t  wd;
	uint32_t dt_restart;
	uint32_t dt_reg;
	uint32_t dt_net;
	uint32_t dt_net_reset;
	uint32_t dt_archive;
} tp_regulator_type;

typedef struct phase_type {
	int8_t   num;
	char     let;
	int8_t   step;
	int8_t   next_step;
	int8_t   step_before;
	bool     is_running;
	bool     is_scanning;
	uint8_t  net_count;
	//uint8_t  net_total;
	uint8_t  error;
	uint8_t  errnum;
	uint8_t  swithes; //кол-во переключений
	uint16_t ports;
	uint16_t errcont;
	uint16_t volt_before;
	uint32_t dt_start;
	uint32_t dt_end;
	uint32_t dt_repair;
	uint32_t dt_error;
} tp_phase_type;

//Необходим размер записи в 32 байта и адрес по границе 32 байт
typedef struct phase_type_save {
	uint16_t counter;     //2
	uint8_t  running;     //3
	uint32_t dt_end;      //7
	uint32_t dt_start;    //11
	uint32_t dt_error;    //15
	uint32_t dt_repair;   //19
	uint8_t  errnum;      //20
	uint16_t errcont;     //22
	int8_t   step;        //23
	int8_t   step_before; //24
	int8_t   step_next;   //25
	
	uint16_t volt_before; //27

	uint8_t   task_num;   //28
	uint8_t   task_index; //29

	uint8_t   sub_num;    //30
	uint8_t   sub_index;  //31
	uint8_t   e0;         //32
} tp_phase_type_save;

typedef struct cont_type {
	uint32_t on_t1;
	uint32_t on_t2;
	uint32_t of_t1;
	uint32_t of_t2;
} tp_cont_type;

typedef struct phase_type_stat {
	uint8_t  num;
	uint32_t dt;
	int8_t   sbef;
	uint16_t vbef;
	int8_t   step;
	uint16_t volt;
	uint16_t errcont;
	uint8_t  errnum;
	tp_cont_type cont[8];
} tp_phase_stat;

//Phases-----------------------------------------------------------------------------------------------------------------------
void Phase_init(void);
void Phase_scanning_on(uint8_t n);
void Phase_scanning_off(uint8_t n);
bool Phase_is_scanning(void);
tp_phase_type *Phase_get(char* param);
tp_phase_type *Phase_num(uint8_t p);

void Phase_stat_init(uint8_t n);
tp_phase_stat *Phase_stat_num(uint8_t p);
void Phase_archive_stat(char* let, tp_phase_stat *stat);

void Phase_ns_on(uint8_t n, uint8_t port);
void Phase_ns_off(uint8_t n, uint8_t port);
uint8_t Phase_ns_port(tp_phase_type *phase);

uint16_t Phase_PS(tp_phase_type *phase, uint8_t grp);
void Phase_port_on(tp_phase_type *phase, uint16_t port);
void Phase_port_off(tp_phase_type *phase, uint16_t port);

void Phase_err_set(tp_phase_type *phase, uint8_t cont, uint8_t errnum);
void Phase_err_clr(tp_phase_type *phase);
void Phase_repair_on(tp_phase_type *phase);

uint8_t Phase_cont_on(tp_phase_type *phase, uint16_t port, uint32_t *t1, uint32_t *t2);
uint8_t Phase_cont_off(tp_phase_type *phase, uint16_t port, uint32_t *t1, uint32_t *t2);

void Regulator_init(uint8_t wd);
tp_regulator_type *Regulator_get(void);
void Regulator_dt_net(void);
void Regulator_dt_reg(void);
uint32_t Regulator_dt_net_get(void);
void Regulator_dt_net_reset(void);
uint32_t Regulator_dt_net_reset_get(void);

void task_net_start(void);
void task_auto_start(void);
void task_archiver(void);


#endif /* PHASE_H_ */