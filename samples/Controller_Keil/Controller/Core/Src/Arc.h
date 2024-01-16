/*
 * Arc.h
 *
 * Created: 27.10.2019 10:55:40
 *  Author: Sergio
 */ 


#ifndef ARC_H_
#define ARC_H_

#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "ctype.h"
#include "Utility.h"
#include "Config.h"
#include "W25Q64.h"

typedef struct arc_idx {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint32_t addr;
} tp_arc_idx;
//2+1+1+4 = 8

typedef struct arc_phase {
	uint16_t Uavg;
	float    Iavg;
	float    Cavg;
	uint16_t Umin;
	uint16_t Umax;
	int8_t   Step;
	uint16_t NumSwitches;
	uint8_t  Errnum;
	uint8_t  Contnum;
} tp_arc_phase;
//2+4+4+2+2+1+2+1+1 = 19

typedef struct arc_rec {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
	uint8_t  hour;
	uint8_t  min;
	uint8_t  res;
	tp_arc_phase phase[3];
} tp_arc_rec;
//2+5 + 3*19 = 7 = 64

typedef struct arc_day {
	uint16_t year;
	uint8_t  month;
	uint8_t  day;
} tp_arc_day;



uint32_t arc_date_num(uint16_t year, uint8_t month, uint8_t day);
bool     arc_idx_find(uint16_t year, uint8_t month, uint8_t day, tp_arc_idx *rec_idx);
bool     arc_save(tp_arc_rec *rec, bool is_check);

uint32_t arc_addr_get(tp_arc_idx *rec_idx);
uint16_t arc_addr_count(tp_arc_idx *rec_idx);

uint8_t  arc_idx_count(void);
void     arc_idx_get(uint8_t rec, tp_arc_idx *rec_idx);
uint8_t  arc_idx_last(void);

bool     arc_rec_get(uint32_t addr, uint16_t rec, char* out);

void     arc_idx_clear(void);

#endif /* ARC_H_ */