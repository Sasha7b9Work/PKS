/*
 * Timers.h
 *
 * Created: 23.10.2017 12:37:29
 *  Author: Admin
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
#include <stdbool.h>

#include "Config.h"
#include "Utility.h"

typedef unsigned long time_t;

/* Useful Constants */
#define SECS_PER_MIN  ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY  ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_WEEK * 52UL))
#define SECS_YR_2000  ((time_t)(946684800UL)) // the time at the start of y2k

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define dayOfWeek(_time_)  ((( _time_ / SECS_PER_DAY + 4)  % DAYS_PER_WEEK)+1) // 1 = Sunday
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  // this is number of days since Jan 1 1970
#define elapsedSecsToday(_time_)  (_time_ % SECS_PER_DAY)   // the number of seconds since last midnight
// The following macros are used in calculating alarms and assume the clock is set to a date later than Jan 1 1971
// Always set the correct time before settting alarms
#define previousMidnight(_time_) (( _time_ / SECS_PER_DAY) * SECS_PER_DAY)  // time at the start of the given day
#define nextMidnight(_time_) ( previousMidnight(_time_)  + SECS_PER_DAY )   // time at the end of the given day
#define elapsedSecsThisWeek(_time_)  (elapsedSecsToday(_time_) +  ((dayOfWeek(_time_)-1) * SECS_PER_DAY) )   // note that week starts on day 1
#define previousSunday(_time_)  (_time_ - elapsedSecsThisWeek(_time_))      // time at the start of the week for the given time
#define nextSunday(_time_) ( previousSunday(_time_)+SECS_PER_WEEK)          // time at the end of the week for the given time


/* Useful Macros for converting elapsed time to a time_t */
#define minutesToTime_t ((M)) ( (M) * SECS_PER_MIN)
#define hoursToTime_t   ((H)) ( (H) * SECS_PER_HOUR)
#define daysToTime_t    ((D)) ( (D) * SECS_PER_DAY) // fixed on Jul 22 2011
#define weeksToTime_t   ((W)) ( (W) * SECS_PER_WEEK)

#define AlarmHMS(_hr_, _min_, _sec_) (_hr_ * SECS_PER_HOUR + _min_ * SECS_PER_MIN + _sec_)

typedef void (*OnTick_t)(void);  // alarm callback function typedef

typedef enum {
	dtTimer,
	dtExplicitAlarm,
	dtDailyAlarm,
	dtWeeklyAlarm,
	dtMillis
} dtAlarmPeriod_t;

typedef struct timer_type {
	uint8_t           enabled;
	dtAlarmPeriod_t   alarmType;
	uint16_t          period;
	uint16_t          ms;
	uint8_t           ready;
	uint32_t          t_next;
	OnTick_t          onTickHandler;
} tp_timer_type;

void Timers_init(void);
void Timers_tick_sec(void);
void Timers_tick_ms(void);
void Timers_process(void);
void Timers_period_set(uint8_t n, uint16_t p);

void Timers_shift(int32_t dt);
void Timers_update(uint8_t n);
void Timers_renew(void);

void Timers_start(uint8_t n);
void Timers_stop(uint8_t n);
bool Timers_status(uint8_t n);

uint32_t Timers_next(uint8_t n);
uint32_t Timers_HMS(const int H, const int M, const int S);

#endif /* TIMERS_H_ */
