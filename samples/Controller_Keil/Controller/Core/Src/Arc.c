/*
 * Arc.c
 *
 * Created: 27.10.2019 10:56:37
 *  Author: Sergio
 */ 
#include "Arc.h"

uint32_t arc_date_num(uint16_t year, uint8_t month, uint8_t day) {
	return (uint32_t)(year) * 10000 + (uint32_t)(month) * 100 + (uint32_t)day;
}

//Находит последний адрес записи данных в сутках
uint32_t arc_addr_get(tp_arc_idx *rec_idx) {
	
	tp_arc_day rec_day;
	uint32_t addr = rec_idx->addr; //Стартовый адрес дня
	
	//sprintf(mess, "Start addr = %u", addr );
	//serial_write(mess);
	
	do {
		W25Q64_read_data(addr, &rec_day, sizeof(rec_day));
		
		//sprintf(mess, "Year = %u, month = %u, day = %u", rec_day.year, rec_day.month, rec_day.day);
		//serial_write(mess);
		
		if (rec_day.year==rec_idx->year && rec_day.month==rec_idx->month && rec_day.day==rec_idx->day) {
			addr += ARCH_REC_LEN;
			if (addr >= ARCH_ADR_MAX) {
				addr = 0;
			}
		} else {
			break;
		}
	} while (true);
	
	return addr;
}

//Находит количество записей данных в сутках
uint16_t arc_addr_count(tp_arc_idx *rec_idx) {
	
	tp_arc_day rec_day;
	uint32_t addr = rec_idx->addr; //Стартовый адрес дня
	
	uint16_t cnt = 0;
	
	do {
		W25Q64_read_data(addr, &rec_day, sizeof(rec_day));
		addr += ARCH_REC_LEN;
		if (addr >= ARCH_ADR_MAX) {
			addr = 0;
		}
		if (rec_day.year==rec_idx->year && rec_day.month==rec_idx->month && rec_day.day==rec_idx->day) {
			cnt++;
		} else {
			break;
		}
	} while (true);
	
	return cnt;
}

bool arc_idx_find(uint16_t year, uint8_t month, uint8_t day, tp_arc_idx *rec_idx) {
	
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	bool    is_found = false;

	for (uint8_t n = 1; n <= ARCH_IDX_REC_MAX; n++) {
	
		eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)rec_idx, rec_len);
		
		if (rec_idx->year == 0xFFFF) {
			break;
		} else if (rec_idx->year == year && rec_idx->month == month && rec_idx->day == day) {
			is_found = true;
			break;
		}
	}
	return is_found;
}

uint8_t arc_idx_last(void) {
	tp_arc_idx rec_idx;
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	
	uint32_t dt_rec  = 0;
	uint32_t dt_max  = 0;
	
	uint8_t  n     =  0;
	uint8_t  rec_num = 0;
		
	//Ищем последнюю
	for (n = 1; n <= ARCH_IDX_REC_MAX; n++) {
			
		eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
		
		if (rec_idx.year == 0xFFFF) {
			//Нашли пустую запись
			rec_num = n > 1 ? n - 1 : 0;
			break;
		} else {
			
			dt_rec = arc_date_num(rec_idx.year, rec_idx.month, rec_idx.day);
			if (dt_rec > dt_max) {
				dt_max = dt_rec;
				rec_num = n;
			}
		}
	}
	return rec_num;
}

bool arc_save(tp_arc_rec *rec, bool is_check) {
	
	tp_arc_idx rec_idx;
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	uint8_t rec_num = 0;
	bool    is_new  = false;
	
	uint16_t year  = rec->year;
	uint8_t  month = rec->month;
	uint8_t  day   = rec->day;
	
	uint32_t addr  = 0;
	uint8_t  n     = 0;
	
	
	//Ищем запись индекса для записи
	for (n = 1; n <= ARCH_IDX_REC_MAX; n++) {
		
		eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
		
		if (rec_idx.year == 0xFFFF) {
			//Нашли пустую запись
			rec_num = n;
			is_new  = true;
			break;
		} else if (rec_idx.year == year && rec_idx.month == month && rec_idx.day == day) {
			//Нашли существующую запись
			rec_num = n;
			is_new  = false;
			break;
		}
	}
	//sprintf(mess, "rec_num = %u; is_new = %u", rec_num, is_new);
	//serial_write(mess);
	
	if (rec_num) {
		if (is_new) {
			//Новая запись в индексе
			if (rec_num==1) {
				//Первая запись
				addr = 0;
			} else {
				//Предыдущая запись
				n = rec_num - 1;
				eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
				addr = arc_addr_get( &rec_idx );
			}
			//serial_write("is_new");
			
			//Пишем
			n = rec_num;
			rec_idx.year = year;
			rec_idx.month = month;
			rec_idx.day = day;
			rec_idx.addr = addr;
			
			eeprom_write_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
		} else {
			//serial_write("exist");
			
			//Запись дня уже есть
			n = rec_num;
			eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
			addr = arc_addr_get( &rec_idx );
		}
		
	} else {
		uint32_t dt_rec  = 0;
		uint32_t dt_max  = 0;
		
		//Все записи заняты, ищем последнюю
		for (n = 1; n <= ARCH_IDX_REC_MAX; n++) {
			
			eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
			
			dt_rec = arc_date_num(rec_idx.year, rec_idx.month, rec_idx.day);
			if (dt_rec > dt_max) {
				dt_max = dt_rec;
				rec_num = n;
			}
		}
		
		//sprintf(mess, "Max rec_num = %u", rec_num);
		//serial_write(mess);
		
		//Берем адрес последней записи
		n = rec_num;
		eeprom_read_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
		addr = arc_addr_get( &rec_idx );
		
		//Следующая запись
		rec_num++;
		
		//Пишем
		n = rec_num > ARCH_IDX_REC_MAX ? 1 : rec_num;
		rec_idx.year = year;
		rec_idx.month = month;
		rec_idx.day = day;
		rec_idx.addr = addr;
		
		//sprintf(mess, "Save rec_num = %u", n);
		//serial_write(mess);
		
		eeprom_write_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
	}
	
	//sprintf(mess, "Addr = %u, Year = %u, month = %u, day = %u", addr, rec->year, rec->month, rec->day);
	//serial_write(mess);
	
	uint8_t data[ARCH_REC_LEN];
	uint8_t start = 0;
	
	memcpy(&data[start], &rec->year, 2); start+=2;
	
	data[start] = rec->month; start++;
	data[start] = rec->day; start++;
	data[start] = rec->hour; start++;
	data[start] = rec->min; start++;
	data[start] = rec->res; start++;
	
	for (uint8_t n = 0; n < PHASE_MAX; n++) {
		
			memcpy(&data[start], &rec->phase[n].Uavg, 2); start+=2;
			memcpy(&data[start], &rec->phase[n].Umax, 2); start+=2;
			memcpy(&data[start], &rec->phase[n].Umin, 2); start+=2;
		
			memcpy(&data[start], &rec->phase[n].Iavg, 4); start+=4;
			memcpy(&data[start], &rec->phase[n].Cavg, 4); start+=4;
		
			data[start] = rec->phase[n].Step; start++;
			memcpy(&data[start], &rec->phase[n].NumSwitches, 2); start+=2;
			data[start] = rec->phase[n].Errnum;  start++;
			data[start] = rec->phase[n].Contnum;  start++;
	}
	
	W25Q64_write_data(addr, data, ARCH_REC_LEN);
	
	/*
	W25Q64_read_data(addr, check, ARCH_REC_LEN);
	
	if ( bytes_check(data, check, ARCH_REC_LEN) ) {
		serial_write("Check OK");
	} else {
		serial_write("Check FAIL");
	}
	*/
	
	
	if (is_check) {
		uint8_t check[ARCH_REC_LEN];

		W25Q64_read_data(addr, check, ARCH_REC_LEN);
	
		return bytes_check(data, check, ARCH_REC_LEN);
	} else {
		return true;
	}
	
	return true;
}

uint8_t arc_idx_count(void) {
	
	tp_arc_idx rec_idx;
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	uint8_t n;

	for (n = 0; n < ARCH_IDX_REC_MAX; n++) {
		
		eeprom_read_buffer(EE_ARCH_IDX_ADDR + n * rec_len, (void *)&rec_idx, rec_len);
		
		if (rec_idx.year == 0xFFFF) {
			break;
		}
	}
	return n;
}

void arc_idx_get(uint8_t rec, tp_arc_idx *rec_idx) {
	
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	
	if (rec > ARCH_IDX_REC_MAX) {rec = ARCH_IDX_REC_MAX;}
	
	eeprom_read_buffer(EE_ARCH_IDX_ADDR + (rec - 1) * rec_len, (void *)rec_idx, rec_len);
}

bool arc_rec_get(uint32_t addr, uint16_t rec, char* out) {
	
	tp_arc_rec arc_rec;
	uint16_t rec_size = ARCH_REC_LEN; //sizeof(arc_rec)
	bool ret = false;
	uint8_t data[ARCH_REC_LEN];
	
	clearString(out);
	
	addr += (uint32_t)(rec-1) * rec_size;
		
	//W25Q64_read_data(addr, &arc_rec, rec_size);
	W25Q64_read_data(addr, data, rec_size);
	
	uint8_t start = 0;
	
	memcpy(&arc_rec.year, &data[start], 2); start+=2;
	
	if (arc_rec.year != 0xFFFF) {
		ret = true;
		
		char str[64] = "";
		char fstr[16] = "";
		
		arc_rec.month = data[start]; start++;
		arc_rec.day = data[start]; start++;
		arc_rec.hour = data[start]; start++;
		arc_rec.min = data[start]; start++;
		arc_rec.res = data[start]; start++;
		
		sprintf(str, "%04u-%02u-%02u %02u:%02u:00;", arc_rec.year, arc_rec.month, arc_rec.day, arc_rec.hour, arc_rec.min);
		strcat(out, str);
		
		for (uint8_t n = 0; n < PHASE_MAX; n++) {
			
			memcpy(&arc_rec.phase[n].Uavg, &data[start], 2); start+=2;
			memcpy(&arc_rec.phase[n].Umax, &data[start], 2); start+=2;
			memcpy(&arc_rec.phase[n].Umin, &data[start], 2); start+=2;
		
			memcpy(&arc_rec.phase[n].Iavg, &data[start], 4); start+=4;
			memcpy(&arc_rec.phase[n].Cavg, &data[start], 4); start+=4;
		
			arc_rec.phase[n].Step = data[start]; start++;
			memcpy(&arc_rec.phase[n].NumSwitches, &data[start], 2); start+=2;
			arc_rec.phase[n].Errnum  = data[start];  start++;
			arc_rec.phase[n].Contnum = data[start];  start++;
			
			sprintf(str, "%u;%u;%u;%0.2f;%0.3f;%d;%u;%u;%u;",
				arc_rec.phase[n].Uavg,
				arc_rec.phase[n].Umax,
				arc_rec.phase[n].Umin,
				arc_rec.phase[n].Iavg,
				arc_rec.phase[n].Cavg,
				arc_rec.phase[n].Step,
				arc_rec.phase[n].NumSwitches,
				arc_rec.phase[n].Errnum,
				arc_rec.phase[n].Contnum
			);
			strcat(out, str);
		}
	
	}
	
	return ret;
}

void arc_idx_clear(void) {
	
	tp_arc_idx rec_idx;
	uint8_t rec_len = ARCH_IDX_REC_LEN;
	
	rec_idx.year = 0xFFFF;
	rec_idx.month = 0xFF;
	rec_idx.day = 0xFF;
	rec_idx.addr = 0xFFFFFFFF;
	
	uint32_t wdt = millis();
	uint32_t m = 0;
	
	for (uint8_t n = 1; n <= ARCH_IDX_REC_MAX; n++) {
		
		eeprom_write_buffer(EE_ARCH_IDX_ADDR + (n-1) * rec_len, (void *)&rec_idx, rec_len);
		
		m = millis();
		
		if (m - wdt > 100)  {
			sys_wdt_reset();
			wdt = m;
		}
	}
}