/*
 * Phase.c
 *
 * Created: 21.11.2017 23:37:44
 *  Author: Hyper
 */ 
#include "Phase.h"

tp_regulator_type Regulator;

tp_phase_type Phase[PHASE_MAX];

tp_phase_stat phase_stat[PHASE_MAX];

void Regulator_init(uint8_t wd) {
	Regulator.wd = wd;
	Regulator.dt_restart = now();
	Regulator.dt_reg = now();
	Regulator.dt_net = now();
	Regulator.dt_net_reset = now();
	Regulator.dt_archive = 0;
}

tp_regulator_type *Regulator_get(void) {
	return &Regulator;
}

void Regulator_reset(void) {
	
	HAL_GPIO_WritePin(PIN_NET_POWER_PORT, PIN_NET_POWER_PIN, 0);
	
	delay_millis(200);
	
	HAL_GPIO_WritePin(PIN_NET_POWER_PORT, PIN_NET_POWER_PIN, 1);
}

void Regulator_dt_net(void) {
	Regulator.dt_net = now();
}
void Regulator_dt_net_reset(void) {
	Regulator.dt_net_reset = now();
}
uint32_t Regulator_dt_net_reset_get(void) {
	return Regulator.dt_net_reset;
}
uint32_t Regulator_dt_net_get(void) {
	return Regulator.dt_net;
}
void Regulator_dt_reg(void) {
	Regulator.dt_reg = now();
}

//Phases
void Phase_init(void) {
	for (uint8_t n = 0; n < PHASE_MAX; n++) {
		Phase[n].num = n+1;
		
		if (!n) {
			Phase[n].let = 'A';
		} else if (n==1) {
			Phase[n].let = 'B';
		} else {
			Phase[n].let = 'C';
		}
		
		Phase[n].step        = 0;
		Phase[n].step_before = 0;
		Phase[n].next_step   = 0;
		Phase[n].is_running  = 0;
		Phase[n].is_scanning = 0;
		Phase[n].error       = 0;
		Phase[n].errnum      = 0;
		Phase[n].ports       = 0;
		Phase[n].errcont     = 0;
		Phase[n].dt_start    = 0;
		Phase[n].dt_end      = 0;
		Phase[n].dt_repair   = 0;
		Phase[n].net_count   = 0;
		Phase[n].dt_error    = 0;
		Phase[n].swithes     = 0;
		//Phase[n].net_total   = 0;
		
		Phase_stat_init(n);
	}
}
void Phase_stat_init(uint8_t n) {
	
	phase_stat[n].num = n+1;
	phase_stat[n].dt = 0;
	phase_stat[n].errcont = 0;
	phase_stat[n].sbef = 0;
	phase_stat[n].vbef = 0;
	phase_stat[n].step = 0;
	phase_stat[n].volt = 0;
	phase_stat[n].errnum = 0;
	
	for (uint8_t i = 0; i < 8; i++) {
		phase_stat[n].cont[i].on_t1 = 0;
		phase_stat[n].cont[i].on_t2 = 0;
		phase_stat[n].cont[i].of_t1 = 0;
		phase_stat[n].cont[i].of_t2 = 0;
	}
}
tp_phase_stat *Phase_stat_num(uint8_t p) {
	if (p>0 && p<=PHASE_MAX) {
		return &phase_stat[p-1];
	} else {
		return NULL;
	}
}
/*
tp_cont_type *Phase_stat_cont(uint8_t p, uint8_t n) {
	if (p>0 && p<=PHASE_MAX) {
		if (n>0 && p<=8) {
			return &phase_stat[p-1].cont[n-1];
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}
*/
void Phase_scanning_on(uint8_t n) {
	Phase[n].is_scanning = 1;
}
void Phase_scanning_off(uint8_t n) {
	Phase[n].is_scanning = 0;
}
bool Phase_is_scanning(void) {
	for (uint8_t n = 0; n < PHASE_MAX; n++) {
		if (Phase[n].is_scanning) {
			return true;
		}
	}
	return false;
}
tp_phase_type *Phase_get(char* param) {
	if ( !strcmp(param, "2") || !strcmp(param, "B") || !strcmp(param, "b") ) {
		return &Phase[1];
	} else if ( !strcmp(param, "3") || !strcmp(param, "C") || !strcmp(param, "c") ) {
		return &Phase[2];
	} else {
		return &Phase[0];
	}
}

tp_phase_type *Phase_num(uint8_t p) {
	if (p>0 && p<=PHASE_MAX) {
		return &Phase[p-1];
	} else {
		return NULL;
	}
}

uint8_t Phase_ns_port(tp_phase_type *phase) {
	
	/*
	if (!phase->ns) {return 0;}
	
	uint8_t port = 0;
	uint8_t is_found = 0;
	
	for (port = 1; port <= 9; port++) {
		if ((phase->ns >> (port-1)) & 1) {
			is_found = 1;
			break;
		}
	}
	if (!is_found) {
		return 0;
	} else {
		return port;
	}
	*/
	return phase->errcont;
}

void Phase_ns_on(uint8_t n, uint8_t port) {
	
	//Phase[n-1].ns |= 1 << (port-1);
	Phase[n-1].errcont = port;
}
void Phase_ns_off(uint8_t n, uint8_t port) {
	//Phase[n-1].ns &= ~(1 <<(port-1));
	Phase[n-1].errcont = 0;
}
/*
uint16_t Phase_PS(tp_phase_type *phase, uint8_t grp) {
	if (phase->num == 1) {
		
		return grp == 2 ? PA2 : PA1;
		
	} else if (phase->num == 2) {
		
		return grp == 2 ? PB2 : PB1;
		
	} else {
		
		return grp == 2 ? PC2 : PC1;
	}
}
*/
void Phase_port_on(tp_phase_type *phase, uint16_t port) {
	if (phase->num == 1) {
	
		PortA_set(port);
	
	} else if (phase->num == 2) {

		PortB_set(port);
	
	} else {
		PortC_set(port);
	}
	Ports_renew();
}
void Phase_port_off(tp_phase_type *phase, uint16_t port) {
	if (phase->num == 1) {
		
		PortA_clr(port);
		
	} else if (phase->num == 2) {

		PortB_clr(port);
		
	} else {
		PortC_clr(port);
	}
	Ports_renew();
}
uint8_t Phase_cont_on(tp_phase_type *phase, uint16_t port, uint32_t *t1, uint32_t *t2) {
	
	*t1 = 0;
	*t2 = 0;
	
	bool p1 = false;
	bool p2 = false;
	
	uint32_t max_time = TASKS_MAX_SCAN_TIME; //0.2 сек
	bool     is_ok = false;
	uint32_t micros = 0;
	
	//Уже включен
	if (Port_get(phase->num, port)) {
		return 0;
	}
	
	//Чтение логики
	uint8_t logic = Ports_logic_get(phase->num, port);
	
	if (logic) {
		//Просто включаем
		Phase_port_on(phase, port);
		
		delay_millis(CONTACTOR_WORK_TIME);
		
		*t2 = (uint32_t) CONTACTOR_WORK_TIME * 1000;
		
		return 0;
	}
	
	//Установка входного порта
	Port_phase_in(phase->num, port);
	
	//Проверяем состояние контактов
	Ports_scan_mode(2);
	delay_millis(TASKS_WAIT_INIT);

	p2 = Ports_scan_P2();
	
	Ports_scan_mode(1);
	delay_millis(TASKS_WAIT_INIT);

	p1 = Ports_scan_P1();
	
	if (!p1 && !p2) {
		//Нет замыкания по обоим контактам, один из них должен быть замкнут
		
		Ports_scan_mode(0);
		Ports_in_clr();
		
		return 11;
	}
	
	//Разомкнут, включаем---------------------------------------------------------------------------------------------------
	Phase_port_on(phase, port);
	
	counter_micros_start();
	is_ok = false;
	
	do {
		//Сканируем
		
		if (!Ports_scan_P1()) {//Контакт разомкнулся, свободный полет
			is_ok = true;
		}
		micros = counter_micros_cur();
		
	} while (!is_ok && micros<=max_time);
	
	*t1 = counter_micros_stop();

	
	if (is_ok) {
		//Ловим замыкание
		Ports_scan_mode(2);
		
		counter_micros_start();
		is_ok = false;
		
		do {
			//Сканируем
			
			if (Ports_scan_P2()) {//Контакт замкнулся
				is_ok = true;
			}
			micros = counter_micros_cur();
			
		} while (!is_ok && micros <= max_time);
		
		*t2 = counter_micros_stop();
		
		Ports_scan_mode(0);
		Ports_in_clr();
		
		if (!is_ok) {
			//Контакт не замкнулся
			return 13;
		} else {
			//Замыкание прошло успешно
			return 0;
		}
	} else {
		//Контакт не разомкнулся
		Ports_scan_mode(0);
		Ports_in_clr();
		
		return 12;
	}
}
uint8_t Phase_cont_off(tp_phase_type *phase, uint16_t port, uint32_t *t1, uint32_t *t2) {
	
	*t1 = 0;
	*t2 = 0;
	
	bool p1 = false;
	bool p2 = false;
	
	uint32_t max_time = TASKS_MAX_SCAN_TIME; //0.2 сек
	bool     is_ok = false;
	uint32_t micros = 0;
	
	//Уже выключен
	if (!Port_get(phase->num, port)) {
		return 0;
	}
	
	//Чтение логики
	uint8_t logic = Ports_logic_get(phase->num, port);
	
	if (logic) {
		//Просто выключаем
		Phase_port_off(phase, port);
		
		delay_millis(CONTACTOR_WORK_TIME);
		
		*t2 = (uint32_t) CONTACTOR_WORK_TIME * 1000;
		
		return 0;
	}
	
	//Установка входного порта
	Port_phase_in(phase->num, port);

	//Проверяем состояние контактов
	Ports_scan_mode(1);
	delay_millis(TASKS_WAIT_INIT);

	p1 = Ports_scan_P1();
	
	Ports_scan_mode(2);
	delay_millis(TASKS_WAIT_INIT);

	p2 = Ports_scan_P2();
	
	if (!p1 && !p2) {
		//Нет замыкания по обоим контактам
		
		Ports_scan_mode(0);
		Ports_in_clr();
		
		return 21;
	}
	
	//Замкнут, отключаем---------------------------------------------------------------------------------------------------
	Phase_port_off(phase, port);
	
	counter_micros_start();
	is_ok = false;
	
	do {
		//Сканируем
					
		if (!Ports_scan_P2()) {//Контакт разомкнулся, свободный полет
			is_ok = true;
		}
		micros = counter_micros_cur();
					
	} while (!is_ok && micros <= max_time);
	
	*t1 = counter_micros_stop();
	
	if (is_ok) {

		//Ловим замыкание
		Ports_scan_mode(1);
		
		counter_micros_start();
		is_ok = false;
		
		do {
			//Сканируем
			if (Ports_scan_P1()) {//Контакт замкнулся
				is_ok = true;
			}
			micros = counter_micros_cur();
			
		} while (!is_ok && micros <= max_time);
		
		*t2 = counter_micros_stop();
		
		Ports_scan_mode(0);
		Ports_in_clr();
		
		if (!is_ok) {
			//Контакт не разомкнулся
			return 23;
		} else {
			//Размыкание прошло успешно
			return 0;
		}
	} else {
		//Контакт не разомкнулся
		Ports_scan_mode(0);
		Ports_in_clr();
		
		return 22;
	}
}
void Phase_err_set(tp_phase_type *phase, uint8_t cont, uint8_t errnum) {
	
	Phase_ns_on(phase->num, cont);

	phase->error    =  1;
	phase->errnum   =  errnum;
	phase->step     =  0;
	phase->dt_error =  now();
	
	//Определяем, какой	порт ошибочный
	uint8_t port = Phase_ns_port(phase);
	if (port) {
		
		//Для контакторов 1-3 ошибку лучше сбросить
		
		if (port > 3) {
	
			//Запуск таска принудительного транзита
			char task_name[20] = "";
			sprintf(task_name,"%c_FTrans", phase->let);
			Task_start(task_name);
		
		} else {
			//Отключаем все контакторы
			for (uint8_t n = 1; n <= 9; n++) {
				Port_clr(phase->num, n);
			}
			Ports_renew();
		}
	}
	
	//Информация об ошибке на SD карту
	/*
	char fname[16] = "";
	char str[64] = "";
	char dt[64] = "";
		
	dateFormat(phase->dt_error, dt);
		
	StrFlash(fname, 36); //  /error.log
		
	StrFlash(str, 37); //%s|%c|%02X|%u
	sprintf(mess, str, dt, phase->let, phase->errcont, phase->errnum);
		
	while (strlen(mess) < 29) {
		strcat(mess, " ");
	}
	strcat(mess, "\r");
		
	sdcard_file_append(fname, mess);
	*/
}
void Phase_err_clr(tp_phase_type *phase) {
	phase->error    = 0;
	phase->errnum   = 0;
	phase->errcont  = 0;
	phase->dt_error = 0;
}

void Phase_repair_on(tp_phase_type *phase) {
	phase->error     = 2;
	phase->dt_repair = now();
}

void task_net_start(void) {
	tp_task_type *task = Task_by_name("Net");
	
	if (task != NULL) {

		if (!Task_is_running(task)) {
			Task_start_tp(task);
		}
	}
}

void task_auto_volt(tp_phase_type *phase, uint16_t volt) {
	
	//if (p>0) {return;}
	
	//int8_t pstep = 0;
	int8_t step = 0;
	uint16_t target = 0;
	uint16_t threshold = 0;
	uint8_t is_error = 0;
	uint8_t is_found = 0;
	char task_name[20] = "";
	
	char str[64] = "";
	
	tp_task_type *task;
	
	//Определяем входное напряжение по выходному с учетом текущей ступени
	uint16_t rvolt = (uint16_t)((float)volt / steps_get(phase->step));
	
	if (volt >= auto_volt_max) {//Требуется понижение
		
		//Можем понижать, если есть бустер или включена повышающая ступень
		if (phase->step > 0 && !auto_boost || auto_boost) {
			
			if (debug_mess) {
				sprintf(mess, "Phase %c, volt %u > %u, rvolt = %u", phase->let, volt, auto_volt_max, rvolt);
				serial_write(mess);
			}
			
			//Пробуем следующую ступень, вплоть до максимальной
			for (int8_t n = phase->step - 1; n >= -auto_steps_count; n--) {
				
				//Напряжение после переключения на эту ступень
				target = rvolt * steps_get(n);
				
				if (target >= auto_volt_min && target <= auto_volt_max) {
					step = n;
					threshold = target;
					
					is_found = 1;
					break;
				}
			}
			//Не нашли предела регулирования, выбираем максимальную ступень
			if (!is_found) {
				step = -auto_steps_count;
				threshold = rvolt * steps_get(step);
				is_found = 1;
			}
		}
		
	} else if (volt <= auto_volt_min) {//Требуется повышение
		
		if (debug_mess) {
			sprintf(mess, "Phase %c, volt %u < %u, rvolt = %u", phase->let, volt, auto_volt_min, rvolt);
			serial_write(mess);
		}
		
		//Пробуем следующую ступень, вплоть до максимальной
		for (int8_t n = phase->step + 1; n <= auto_steps_count; n++) {
			
			//Напряжение после переключения на эту ступень
			target = rvolt * steps_get(n);
			
			if (target >= auto_volt_min && target <= auto_volt_max) {
				step = n;
				threshold = target;
				is_found = 1;
				
				break;
			}
		}
		//Не нашли предела регулирования, выбираем максимальную ступень
		if (!is_found) {
			step = auto_steps_count;
			threshold = rvolt * steps_get(step);
			is_found = 1;
		}
	} else {
		//Находимся в диапазоне
		
		//Если есть стабилизатор и находимся в понижении
		if (auto_boost && phase->step < 0) {
			
			//Проверяем, какое напряжение будет после повышения на одну ступень
			int8_t n = phase->step + 1;
		
			//Напряжение после переключения на эту ступень
			target = rvolt * steps_get(n);
		
			if (target >= auto_volt_min && target <= auto_volt_max - 5) {
				//Можем переключится
				step = n;
				threshold = target;
				is_found = 1;
			}
		}
	}
	if (is_found && step < 0 && !auto_boost) {
		step = 0; //Можем понизить только до транзита, если нет стабилизатора
		
		threshold = rvolt * steps_get(step);
		is_found = 1;
	}
	if (!is_error && phase->step == step) {
		is_error = 1;
	}
	if (!is_error) {
		
		if (is_found) {//требуется регулирование
			
			if (step >= 0) {
				sprintf(task_name,"%c%u", phase->let, step);
			} else {
				sprintf(task_name,"%c%u", phase->let, -step);
			}
			
			if (strlen(task_name)>0) {
				task = Task_by_name(task_name);
				
				if (task == NULL) {
					if (debug_error) {
						sprintf(mess, "task not found %s", task_name);
						serial_write(mess);
					}
					is_error = 1;
				} else {
					//Хотим перейти в это состояние
					phase->next_step = step;
					
					if (debug_mess) {
						sprintf(mess, "Phase %u next step %d", phase->num, phase->next_step);
						serial_write(mess);
					}
					
					Task_start_tp(task);
					
					if (debug_mess) {
						sprintf(mess, "started %s (%u)", task_name, task->n);
						serial_write(mess);
					}
				}
			}
		}
	}
}

void task_archiver(void) {
	//Нет авторегулирования
	if (!auto_volt_phase) {return;}
		
	//Нет получения сетевых данных
	if (!Timers_status(TIMER_NET)) {return;}
	
	struct calendar_date sdate;
	uint32_t time = now();
	calendar_timestamp_to_date(time, &sdate);
	
	if (sdate.second) {return;}
	
	tp_phase_type *phase;
		
	uint32_t ver = W25Q64_JEDEC();
	
	if (ver) {
	
		tp_regulator_type *regulator = Regulator_get();
		
		tp_arc_rec  arc_rec;
		
		arc_rec.year  = sdate.year;
		arc_rec.month = sdate.month + 1;
		arc_rec.day   = sdate.date  + 1;
		arc_rec.hour  = sdate.hour;
		arc_rec.min   = sdate.minute;
		arc_rec.res   = 0xFF;
	
		for (uint8_t n = 1; n <= PHASE_MAX; n++) {
			
			phase = Phase_num(n);
			
			arc_rec.phase[n-1].Uavg = volt_avg(n);
			arc_rec.phase[n-1].Umax = volt_max(n);
			arc_rec.phase[n-1].Umin = volt_min(n);
			
			arc_rec.phase[n-1].Iavg = curr_avg(n) * auto_kt;
			arc_rec.phase[n-1].Cavg = cos_avg(n);
			
			arc_rec.phase[n-1].Step = phase->step;
			arc_rec.phase[n-1].NumSwitches = phase->swithes;
			arc_rec.phase[n-1].Errnum =  phase->errnum;
			arc_rec.phase[n-1].Contnum =  phase->errcont;
		}
	
		if (arc_save(&arc_rec, true)) {
			regulator->dt_archive = now();
		}
	}
	
	//Обнуляем количество переключений
	for (uint8_t n = 1; n <= PHASE_MAX; n++) {
		phase = Phase_num(n);
		phase->swithes = 0;
	}
	
	//Обнуляем счетчик
	net_reset_counter();
	
	if (now() - Regulator_dt_net_reset_get() >= NET_RESET_PERIOD) {
		
		Task_start("NetReset");
	}
}

//Запуск раз в 1 секунду
void task_auto_start(void) {
	
	if (!auto_volt_phase) {return ;}
		
	tp_phase_type *phase;
	
	uint16_t volt = 0;
	char task_name[20] = "";
	char str[64] = "";
	
	uint16_t rvolt = 0;
	
	
	//Фазы
	for (uint8_t pnum = 1; pnum <= PHASE_MAX; pnum++) {
		
		phase = Phase_num(pnum);
		
		if (phase->error) {
			
			//phase->error==1 идет ремонт

			//Авторемонт
			if (phase->error==1 && now() - phase->dt_repair > CONTACTOR_REPAIR_RETRY) {
				//Запускаем
				sprintf(task_name,"%cRep", phase->let);
				Task_start(task_name);
			}
		} else {
			
			//Идет регулирование, пропускаем
			if (phase->is_running) {
				continue;
			}
				
			//Последнее измеренное напряжение
			volt = volt_last(pnum);
				
			//Ловим превышение максимального напряжения, например при резком отключении мощной нагрузки
			//Включена повышающая ступень и было регулирование
			if (phase->dt_end) {
				if (volt >= range_volt_max && phase->step > 0) {
					
					//Уходим в транзит
					sprintf(task_name, "%c0", phase->let);
					Task_start(task_name);
					
					continue;
				}
					
				//Контроль работы 9 контактора
				//проверяем не сразу, а через пару секунд после регулирования
				/*
				uint8_t sec = 2;
				if (sec > auto_time) {sec = auto_time;}
					
				if ( auto_boost && ((uint8_t)(now() - phase->dt_end) == 2) && volt > 0) {
						
					if ((phase->step < 0 || phase->step_before < 0)  && phase->step != phase->step_before) {//задействовался 9 контактор и ступень изменилась
					
						//Проверяем напряжение сразу после регулирования
						//Только если есть 9 контактор
						
						if (debug_mess) {
							StrFlash(str, 38); //phase=%u, step=%d, sbefore=%d, volt=%u, vbefore=%u
							sprintf(mess, str, phase->let, phase->step, phase->step_before, volt, phase->volt_before);
							serial_write(mess);
						}
					
						bool is_error = false;
							
						if (phase->step > phase->step_before) {
							//Было повышение ступени, VB < 210, VA > VB (напряжение должно повысится)
							if (volt < phase->volt_before) {
								//Вместо повышения получили понижение
								is_error = true;
							}
						}
						if (phase->step < phase->step_before) {
							//Было понижение ступени, VB > 230, VA < VB (напряжение должно понизится)
							if (volt > phase->volt_before) {
								//Вместо понижения получили повышение
								is_error = true;
							}
						}
						if (is_error) {
							if (debug_error) {
								StrFlash(str, 39);
								serial_write(39);//Error 9
							}
								
							Phase_err_set(phase, 9, 10);

							continue;
						}
					}
				}
				*/
			}
				
			//Точек измерения недостаточно
			if (phase->net_count < auto_points) {continue;}
				
			//Контроль времени регулирования, не чаще чем auto_time
			if ( (uint8_t)(now() - phase->dt_end) > auto_time) {
					
				//Среднее напряжение за несколько точек
				volt = volt_avg_reg(pnum-1, auto_points);
					
				//Определяем входное напряжение по выходному с учетом текущей ступени
				rvolt = (uint16_t)((float)volt / steps_get(phase->step));
	
				//Можно регулировать, напряжение должно быть больше минимально допустимого, добавляем лаг в 5в
				if ((phase->step && rvolt >= range_volt_min) || (!phase->step && rvolt >= range_volt_min+5)  ) {
						
					task_auto_volt(phase, volt);
						
				} else {
					if (phase->step) {
						//Уходим в транзит, если мы еще не в транзите
						if (debug_mess) {
							sprintf(mess, "Phase=%u; volt=%u; min=%u", phase->num, rvolt, range_volt_min);
							serial_write(mess);
						}
							
						//Уходим в транзит
						sprintf(task_name, "%c0", phase->let);
						Task_start(task_name);
					}
				}
			}
		}
	}
}