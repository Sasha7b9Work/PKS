/*
 * Tasks.h
 *
 * Created: 22.06.2017 17:39:16
 *  Author: Admin
 */ 


#ifndef TASKS_H_
#define TASKS_H_

#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "Config.h"
#include "Serial.h"

//-----------------------------------------------------------------------------------------------------------------------
//Argument should be - (tp_task_type *task) ???
typedef bool (*OnTask_t)();     // callback function typedef
//void *task

typedef struct task_type {
	char     name[TASKS_NAME_LEN];
	uint8_t  n;
	uint8_t  status;
	uint8_t  index;
	uint16_t iter;
	uint16_t wait; //max 65 sec
	OnTask_t cmds[TASKS_CMDS_MAX];
	uint8_t  cmds_cnt;
	uint8_t  parent;
	uint8_t  try;
	uint16_t param[TASKS_PARAM_LEN];
	uint32_t dt_start;
	uint32_t dt_end;
} tp_task_type;

//index ?? ????? ???? ?????? ?????????? ?????? - TASKS_CMDS_MAX
//????? ?????????? cmds_cnt ? index - ???????? ??????...

extern uint8_t tasks_cnt;

//Status
#define TASK_STATUS_ENABLED              0
#define TASK_STATUS_RUNNING              1
#define TASK_STATUS_REPEAT               2
#define TASK_STATUS_ITERMAN              3
#define TASK_STATUS_ERROR                4
#define TASK_STATUS_SUBRUN               5

extern tp_task_type Tasks[];

uint8_t       Task_num(void);

tp_task_type *Task_start(char* name);
void          Task_start_tp(tp_task_type *Ptask);

void          Task_status_bit(tp_task_type *Ptask, uint8_t bit, bool val);
bool          Task_is_status(tp_task_type *Ptask, uint8_t bit);
void          Task_error(tp_task_type *Ptask, uint8_t val);
void          Task_iter_manual(tp_task_type *Ptask, uint8_t val);
void          Task_repeat_set(tp_task_type *Ptask, bool repeat);
bool          Task_is_error(tp_task_type *Ptask);
bool          Task_is_running(tp_task_type *Ptask);
bool          Task_is_enabled(tp_task_type *Ptask);

tp_task_type *Task_start_sub(char* name, tp_task_type *task);
void          Task_start_sub_tp(tp_task_type *Ptask, tp_task_type *task);

tp_task_type *Task_by_name(char* name);
tp_task_type *Task_by_num(uint8_t n);

tp_task_type *Task_get_sub(tp_task_type *Ptask);
tp_task_type *Task_get_parent(tp_task_type *Psub);

void          Task_process(void);
void          Task_process_n(tp_task_type *task);
void          Task_init(void);
void          Task_timers(void);
void          Task_init_n(uint8_t n);
bool          Task_pin_level(tp_task_type *task, bool pin_level, bool check);
bool          Task_try(tp_task_type *task, uint8_t max);
bool          Task_goto(tp_task_type *Ptask, uint8_t index);
void          Task_repeat(tp_task_type *Ptask);

void          Task_stop(tp_task_type *Ptask);
void          Task_stop_sub(tp_task_type *Ptask);

void          Task_stat(char* result);
void          Task_stat_one(tp_task_type *task, char* result);
void          Task_dt_update(uint32_t time);

bool          Task_is_sub(tp_task_type *Ptask);
bool          Task_sub_running(tp_task_type *Ptask);


#endif /* TASKS_H_ */
