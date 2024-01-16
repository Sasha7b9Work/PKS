/*
 * Tasks.c
 *
 * Created: 22.06.2017 17:39:32
 *  Author: Admin
 */ 
#include "Tasks.h"

//Tasks-----------------------------------------------------------------------------------------------------------------------------------------
void Task_init(void) {
	
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		Task_init_n(n);
		
		for (uint8_t i = 0; i < TASKS_PARAM_LEN; i++) {
			Tasks[n].param[i] = 0;
		}
	}
}
void Task_init_n(uint8_t n) {
	uint8_t i = 0;
	while (Tasks[n].cmds[i]!=NULL) {
		i++;
	}
	Tasks[n].n        = n;
	Tasks[n].cmds_cnt = i;
	Tasks[n].status   = 0;
	
	/*
	Tasks[n].enabled = false;
	Tasks[n].running = false;
	Tasks[n].repeat  = false;
	Tasks[n].error   = 0;
	Tasks[n].iter_manual = false;
	*/
	
	Tasks[n].index   = 0;
	Tasks[n].iter    = 0;
	
	Tasks[n].wait    = 0;
	
	Tasks[n].parent =  zval_8;
	Tasks[n].try    =  0;
	Tasks[n].dt_start =  0;
	Tasks[n].dt_end   = 0;
}
uint8_t Task_num(void) {
	return tasks_cnt;
}
tp_task_type *Task_by_name(char* name) {
	
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		
		if ( !strcmp(Tasks[n].name, name) ) {
			return &Tasks[n];
		}
	}
	return NULL;
}
tp_task_type *Task_by_num(uint8_t n) {
	
	if (n >= tasks_cnt) {n = tasks_cnt - 1;}
	
	return &Tasks[n];
}
tp_task_type *Task_start(char* name) {
	
	tp_task_type *Ptask = Task_by_name(name);
	if (Ptask!=NULL) {
		
		Task_start_tp(Ptask);
	}
	
	return Ptask;
}
void Task_start_tp(tp_task_type *Ptask) {
	
	Task_init_n(Ptask->n);
	
	Task_status_bit(Ptask, TASK_STATUS_ENABLED, true);
	Task_status_bit(Ptask, TASK_STATUS_RUNNING, true);
	Task_status_bit(Ptask, TASK_STATUS_REPEAT, false);
	
	Ptask->try     = 0;
	Ptask->dt_start = now();
	Ptask->dt_end   = 0;
}

void Task_repeat_set(tp_task_type *Ptask, bool repeat) {
	Task_status_bit(Ptask, TASK_STATUS_REPEAT, repeat);
}

void Task_status_bit(tp_task_type *Ptask, uint8_t bit, bool val) {
	if (val) {
		Ptask->status |= 1 << bit;
	} else {
		Ptask->status &= ~(1 << bit);
	}
}

bool Task_is_status(tp_task_type *Ptask, uint8_t bit) {
	return (Ptask->status >> bit) & 1 ? true : false;
}

void Task_error(tp_task_type *Ptask, uint8_t val) {
	if (val) {
		Ptask->status |= 1 << TASK_STATUS_ERROR;
	} else {
		Ptask->status &= ~(1 << TASK_STATUS_ERROR);
	}
}

void Task_iter_manual(tp_task_type *Ptask, uint8_t val) {
	if (val) {
		Ptask->status |= 1 << TASK_STATUS_ITERMAN;
	} else {
		Ptask->status &= ~(1 << TASK_STATUS_ITERMAN);
	}
}

bool Task_is_error(tp_task_type *Ptask) {
	return (Ptask->status >> TASK_STATUS_ERROR) & 1 ? true : false;
}

bool Task_is_running(tp_task_type *Ptask) {
	return (Ptask->status >> TASK_STATUS_RUNNING) & 1 ? true : false;
}

bool Task_is_enabled(tp_task_type *Ptask) {
	return (Ptask->status >> TASK_STATUS_ENABLED) & 1 ? true : false;
}

void Task_stop(tp_task_type *Ptask) {
	
	if (!Task_is_sub(Ptask)) {
		
		//Если запущена подрограмма, ее нужно остановить
		if (Task_sub_running(Ptask)) {
			
			bool is_found = false;
			uint8_t sub_n = 0;
			
			for (uint8_t i = 0; i < tasks_cnt; i++) {
				if (Ptask->n == Tasks[i].parent) {//Таск-подпрограмма имеет ссылку на родительский таск
					sub_n = i;
					is_found = true;
					break;
				}
			}
			if (is_found) {
				Task_stop_sub(&Tasks[sub_n]);
			}
		}
		
		Task_status_bit(Ptask, TASK_STATUS_ENABLED, false);
		Task_status_bit(Ptask, TASK_STATUS_RUNNING, false);
	
		Ptask->dt_end = now();
		Ptask->index = 0;
	}
}

tp_task_type *Task_get_sub(tp_task_type *Ptask) {
	
	//num - main num
	
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		
		if ( Tasks[n].parent == Ptask->n ) {
			return &Tasks[n];
		}
	}
	
	return NULL;
}
tp_task_type *Task_get_parent(tp_task_type *Psub) {
	
	uint8_t parent = Psub->parent;
	
	if (parent!=zval_8) {
		return &Tasks[parent];
	} else {
		return NULL;
	}	
}

void Task_stop_sub(tp_task_type *Ptask) {
	
	//Останов таска подпрограммы
	Task_status_bit(Ptask, TASK_STATUS_ENABLED, false);
	Task_status_bit(Ptask, TASK_STATUS_RUNNING, false);
	
	Ptask->dt_end = now();
	Ptask->index = 0;
	
	//Возобновляем основную программу
	uint8_t parent = Ptask->parent;
	tp_task_type *Parent_task = &Tasks[parent];
	Task_status_bit(Parent_task, TASK_STATUS_ENABLED, true);
	Task_status_bit(Parent_task, TASK_STATUS_SUBRUN, false);
	
	//Больше не подпрограмма
	Ptask->parent = zval_8;
}

//Является ли таск подпрограммой
bool Task_is_sub(tp_task_type *Ptask) {
	return Ptask->parent == zval_8 ? false : true;
}

//Запустил ли таск подпрограмму (является родительским таском)
bool Task_sub_running(tp_task_type *Ptask) {
	return Task_is_status(Ptask, TASK_STATUS_SUBRUN);
}

tp_task_type *Task_start_sub(char* name, tp_task_type *task) {
	
	tp_task_type *Ptask = Task_by_name(name);
	if (Ptask!=NULL) {
		
		Task_start_sub_tp(Ptask, task);
	}
	
	return Ptask;
}

bool Task_goto(tp_task_type *Ptask, uint8_t index) {
	
	if (index >= tasks_cnt) {
		Task_stop(Ptask);
	} else {
	
		Ptask->wait        = 0;
		Ptask->try         = 0;
		Ptask->iter        = 0;
		
		Task_error(Ptask, 0);
		Task_status_bit(Ptask, TASK_STATUS_ITERMAN, true);
	
		Ptask->index       = index;
	}
	return false;
}
void Task_repeat(tp_task_type *Ptask) {
	Ptask->iter        = 0;
	Task_status_bit(Ptask, TASK_STATUS_ITERMAN, false);
}
void Task_start_sub_tp(tp_task_type *Ptask, tp_task_type *task) {
		
	Task_init_n(Ptask->n);
	
	Task_status_bit(Ptask, TASK_STATUS_ENABLED, true);
	Task_status_bit(Ptask, TASK_STATUS_RUNNING, true);
	
	Ptask->parent  = task ->n;
	
	Ptask->dt_start = now();
	
	//У родительского таска ставим указатель
	Task_status_bit(task, TASK_STATUS_SUBRUN, true);
}

void Task_timers(void) {
	//Обработка таймеров
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		if (Task_is_status(&Tasks[n], TASK_STATUS_ENABLED) && Tasks[n].wait) {
			Tasks[n].wait--;
		}
	}
}

bool Task_try(tp_task_type *task, uint8_t max) {
	
	char out[64];
	
	task->try++;
	
	if (task->try >= max) {
		Task_error(task, 1);
		return true;
	} else {
		
		sprintf(out, "Try = %u, %s, id=%u", task->try+1, task->name, task->n);
		serial_write(out);
			
		//Начинаем сначала
		task->iter = 0;
		Task_status_bit(task, TASK_STATUS_ITERMAN, true);
		
		return false;
	}
}

void Task_stat(char* result) {
	
	clearString(result);
	
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		
		sprintf(mess, "%u=%s:%u,%u,%u;", Tasks[n].n, Tasks[n].name, Task_is_enabled(TASK_STATUS_ENABLED), Task_is_running(&Tasks[n]), Task_is_error(&Tasks[n]));
		strcat(result, mess);
	}
}

void Task_stat_one(tp_task_type *task, char* result) {
	
	char dtstart[32];
	timeFormat(task->dt_start, dtstart);
	
	char dtend[32];
	timeFormat(task->dt_end, dtend);
	
	sprintf(result, "n=%u,en=%u,run=%u,idx=%u/%u,itr=%u,err=%u,sub=%u,par=%u,w=%u,start=%s,end=%s",
		task->n,
	Task_is_enabled(task),
	Task_is_running(task),
	task->index,
	task->cmds_cnt,
	task->iter,
	Task_is_error(task),
	Task_is_sub(task),
	task->parent,
	task->wait,
	dtstart,
	dtend);
}

void Task_dt_update(uint32_t time) {
	
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		if (Task_is_running(&Tasks[n])) {
			Tasks[n].dt_start = time;
		}
	}
}

void Task_process(void) {
	
	//Алгоритмы
	for (uint8_t n = 0; n < tasks_cnt; n++) {
		
		Task_process_n(&Tasks[n]);
		
	}
}

void Task_process_n(tp_task_type *task) {
	
	bool     ret;
	
	if (Task_is_enabled(task) && !task->wait) {
		
		if (now() - task->dt_start > TASKS_RUN_MAX) {
			
			//Останавливаем, если время работы превышает максимальное
			sprintf(mess, "Stop task %u (%s)", task->n, task->name);
			serial_write(mess);
			
			if (Task_is_sub(task)) {
				Task_stop_sub(task);
				
				Task_stop( Task_get_parent(task) );
			} else {
				Task_stop(task);
			}
			
			return; //следующий таск
		}
		
		//Выполняем действие
		ret = task->cmds[task->index](task);
		
		//Если ошибка
		if (Task_is_error(task)) {
			
			//Останавливаем выполнение
			Task_is_sub(task) ? Task_stop_sub(task) : Task_stop(task);
			
			return; //следующий таск
		}
		
		if (Task_sub_running(task)) {
				
			//Запущена подпрограмма
			Task_status_bit(task, TASK_STATUS_ENABLED, false);
			task->iter++;
			
			return; //следующий таск
				
		}
		
		//успешное завершение
		if (ret) {
			
			//sprintf(mess, "Complete task %u (%s)", n, Tasks[n].name);
			//serial_write(mess);
			
			//Следующее действие
			task->index++;
			task->iter = 0;
				
			//Последнее действие
			if (task->index >= task->cmds_cnt) {
					
				//Подпрограмма
				if (Task_is_sub(task)) {
					//sprintf(mess, "Stop sub %u, %s", task->n, task->name);
					//serial_write(mess);
					
					Task_stop_sub(task);
					
				} else if (!Task_is_status(task, TASK_STATUS_REPEAT)) {
					//Задание закончено
					Task_stop(task);
				} else {
					//Начинаем заново
					task->index = 0;
				}
			}
		} else {
			//fail, увеличиваем итерацию
			if (!Task_is_status(task, TASK_STATUS_ITERMAN)) {
				task->iter++;
			} else {
				Task_status_bit(task, TASK_STATUS_ITERMAN, false);
			}
		}
	}
}