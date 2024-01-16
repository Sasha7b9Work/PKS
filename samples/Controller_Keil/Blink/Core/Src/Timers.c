/*
 * Timers.c
 *
 * Created: 23.10.2017 12:37:41
 *  Author: Admin
 */ 
#include "Timers.h"

tp_timer_type timers[TIMERS_MAX];

void Timers_init(void) {
	
	//Таймеры
	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		timers[n].enabled = 0;
		timers[n].ms     = 0;
		timers[n].ready   = 0;
		timers[n].alarmType   = dtTimer;
	}
}
void Timers_tick_sec(void) {
	//Обработка таймеров
	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		if (timers[n].enabled && timers[n].alarmType != dtMillis && !timers[n].ready) {
			if (now() >= timers[n].t_next) {
				timers[n].ready = 1;
			}
		}
	}
}
void Timers_tick_ms(void) {
	//Обработка таймеров
	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		if (timers[n].enabled && timers[n].alarmType == dtMillis) {
			if (timers[n].ms < timers[n].period) {
				timers[n].ms++;
			} else {
				timers[n].ready   = 1;
			}
		}
	}
}
void Timers_process(void) {

	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		if (timers[n].enabled && timers[n].ready) {
			
			timers[n].ready = 0;
			
			//Выполняем
			timers[n].onTickHandler();
			
			Timers_update(n);
		}
	}
}

void Timers_renew(void) {
	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		if (timers[n].enabled) {
			Timers_update(n);
		}
	}
}
void Timers_shift(int32_t dt) {
	for (uint8_t n = 0; n < TIMERS_MAX; n++) {
		if (timers[n].enabled) {
			timers[n].t_next = (uint32_t)(timers[n].t_next + dt);
		}
	}
}
void Timers_update(uint8_t n) {
	
	uint32_t time = now();
	
	if (timers[n].alarmType == dtDailyAlarm) {
		//if this is a daily alarm
		if (timers[n].period + previousMidnight(time) <= time) {
			// if time has passed then set for tomorrow
			timers[n].t_next = timers[n].period + nextMidnight(time);
		} else {
			// set the date to today and add the time given in value
			timers[n].t_next = timers[n].period + previousMidnight(time);
		}
	} else if (timers[n].alarmType == dtTimer) {
		
		timers[n].t_next = time + timers[n].period;
	
	} else if (timers[n].alarmType == dtMillis) {
	
		timers[n].ms = 0;
	}
	/*
	if (timers[n].t_next<=time) {
		Timers_stop(n);
	}
	*/
}

void Timers_start(uint8_t n) {
	if (n < TIMERS_MAX) {
		timers[n].enabled = 1;
		Timers_update(n);
	}
}
void Timers_period_set(uint8_t n, uint16_t p) {
	if (n < TIMERS_MAX) {
		timers[n].period = p;
	}
}
void Timers_stop(uint8_t n) {
	if (n < TIMERS_MAX) {
		timers[n].enabled = 0;
	}
}
bool Timers_status(uint8_t n) {
	if (n < TIMERS_MAX) {
		return timers[n].enabled;
	} else {
		return 0;
	}
}
uint32_t Timers_next(uint8_t n) {
	if (timers[n].enabled) {
		return timers[n].t_next;
	} else {
		return 0;
	}
}
uint32_t Timers_HMS(const int H, const int M, const int S) {
	return AlarmHMS(H,M,S);
}
