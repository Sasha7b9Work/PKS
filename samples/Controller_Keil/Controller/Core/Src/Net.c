#include "Net.h"

//Буферы
uint16_t vregA[NET_BUF_LEN];
uint16_t vregB[NET_BUF_LEN];
uint16_t vregC[NET_BUF_LEN];

float net_kt_volt[3];
float net_kt_curr[3];

//Пороги переключения - изменить на вольтодобавки
float steps_mass[10] = {
	1.047619047619048, //1+10/210, 1 ст.
	1.1,               //1+20/200, 2 ст.
	1.157894736842105, //1+30/190, 3 ст.
	1.222222222222222, //1+40/180, 4 ст.
	1.294117647058824, //1+50/170, 5 ст.
	
	0.9565217391304348, //1-10/230, -1 ст.
	0.9166666666666667, //1-20/240, -2 ст.
	0.88,               //1-30/250, -3 ст.
	0.8461538461538462, //1-40/260, -4 ст.
	0.8148148148148148  //1-50/270  -5 ст.
};

tp_net_type NetInfoA;
tp_net_type NetInfoB;
tp_net_type NetInfoC;

void net_init(void) {
	
	//Коэффициенты
	for (uint8_t n = 1; n <= 3; n++) {
		net_kt_volt[n-1] = net_kt_volt_get(n);
		net_kt_curr[n-1] = net_kt_curr_get(n);
	}
	
	//Инициализация буферов
	buffers_init();
	
	//Читаем пределы
	steps_read();
}

tp_net_type *net_info(uint8_t p) {
	if (p==1) {
		return &NetInfoA;
	
	} else if (p==2) {
		return &NetInfoB;
	
	} else {
		return &NetInfoC;
	}
}

void buffers_init(void) {
	
	for (uint8_t n = 0; n < NET_BUF_LEN; n++) {
		vregA[n] = 0;
		vregB[n] = 0;
		vregC[n] = 0;
	}
	
	NetInfoA.count = 0;
	NetInfoB.count = 0;
	NetInfoC.count = 0;
}

void net_reset_counter(void) {
	NetInfoA.count = 0;
	NetInfoB.count = 0;
	NetInfoC.count = 0;
}
void buffers_save_last(tp_net_type *NetInfo, float volt, float curr, float coss) {
	
	NetInfo->volt_last = round(volt);
	NetInfo->curr_last = curr;
	NetInfo->cos_last  = coss;
}
uint16_t buffers_save_one(tp_net_type *NetInfo, float fvolt, float curr, float coss) {
	
	uint16_t volt = round(fvolt);
	
	//Обнуляем счетчик
	if (NetInfo->count >= NET_BUF_LEN) {
		NetInfo->count = 0;
	}
	NetInfo->count++;
	
	if (NetInfo->count==1) {
		NetInfo->volt_max = volt;
		NetInfo->volt_min = volt;
		NetInfo->volt_avg = volt;
		
		NetInfo->curr_max = curr;
		NetInfo->curr_min = curr;
		NetInfo->curr_avg = curr;
		
		NetInfo->cos_max  = coss;
		NetInfo->cos_min  = coss;
		NetInfo->cos_avg  = coss;
	} else {
		if (volt > NetInfo->volt_max) {NetInfo->volt_max = volt;}
		if (volt < NetInfo->volt_min) {NetInfo->volt_min = volt;}
		NetInfo->volt_avg += volt;
		
		if (curr > NetInfo->curr_max) {NetInfo->curr_max = curr;}
		if (curr < NetInfo->curr_min) {NetInfo->curr_min = curr;}
		
		NetInfo->curr_avg += curr;
		
		if (coss > NetInfo->cos_max) {NetInfo->cos_max = coss;}
		if (coss < NetInfo->cos_min) {NetInfo->cos_min = coss;}
		NetInfo->cos_avg += coss;
	}
	return volt;
}
void buffers_save(void) {
	
	Regulator_dt_net();
	
	tp_phase_type *phaseA = Phase_num(1);
	tp_phase_type *phaseB = Phase_num(2);
	tp_phase_type *phaseC = Phase_num(3);
		
	//Обнуление происходит при task_step_finish
	if (!phaseA->is_running) {
		if (phaseA->net_count < NET_BUF_LEN) {
			phaseA->net_count++;
		}
	}
	if (!phaseB->is_running) {
		if (phaseB->net_count < NET_BUF_LEN) {
			phaseB->net_count++;
		}
	}
	if (!phaseC->is_running) {
		if (phaseC->net_count < NET_BUF_LEN) {
			phaseC->net_count++;
		}
	}
		
	//Сдвигаем
	for (uint8_t n = NET_BUF_LEN-1; n > 0; n--) {
		if (!phaseA->is_running) {
			vregA[n] = vregA[n-1];
		}
		if (!phaseB->is_running) {
			vregB[n] = vregB[n-1];
		}
		if (!phaseC->is_running) {
			vregC[n] = vregC[n-1];
		}
	}
	
	//Напряжения
	uint32_t volt[3];
	float    fvolt[3];
	
	for (uint8_t n = 1; n <= 3; n++) {
		volt[n-1] = ADE7758_rms_val(n);
	}
	for (uint8_t n = 0; n < 3; n++) {
		fvolt[n] = volt[n] / net_kt_volt[n];
	}
	
	//Токи
	uint32_t curr[3];
	float    fcurr[3];
	
	for (uint8_t n = 4; n <= 6; n++) {
		curr[n-4] = ADE7758_rms_val(n);
	}
	for (uint8_t n = 0; n < 3; n++) {
		fcurr[n] = curr[n] / net_kt_curr[n];
	}
	
	//CosF
	float    cosF[3];
	for (uint8_t n = 1; n <= 3; n++) {
		cosF[n-1] = ADE7758_getCos(n);
	}
	
	//Phase A
	buffers_save_last(&NetInfoA, fvolt[0], fcurr[0], cosF[0]);
	
	if (!phaseA->is_running) {
		vregA[0] = buffers_save_one(&NetInfoA, fvolt[0], fcurr[0], cosF[0]);
	}
	
	//Phase B
	buffers_save_last(&NetInfoB, fvolt[1], fcurr[1], cosF[1]);
	
	if (!phaseB->is_running) {
		vregB[0] = buffers_save_one(&NetInfoB, fvolt[1], fcurr[1], cosF[1]);
	}
	
	//Phase C
	buffers_save_last(&NetInfoC, fvolt[2], fcurr[2], cosF[2]);
	
	if (!phaseC->is_running) {
		vregC[0] = buffers_save_one(&NetInfoC, fvolt[2], fcurr[2], cosF[2]);
	}
}

uint16_t volt_avg_reg(uint8_t p, uint8_t count) {
	
	uint16_t sum = 0;
	
	tp_phase_type *phase = Phase_num(p+1);
	
	if (count > phase->net_count) {count = phase->net_count;}
		
	if (!count) {return 0;}
	
	for (uint8_t n = 0; n < count; n++) {
		
		if (!p) {sum += vregA[n];}
		else if (p==1) {sum += vregB[n];}
		else {sum += vregC[n];}
	}
	return (uint16_t) sum / count;
}

uint16_t volt_avg(uint8_t p) {
	
	tp_net_type *PNetInfo;
		
	if (!p) {PNetInfo = &NetInfoA;}
	else if (p==1) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (uint16_t)PNetInfo->volt_avg / PNetInfo->count;
	}
}
uint16_t volt_last(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (uint16_t)PNetInfo->volt_last;
	}
}
uint16_t volt_max(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return PNetInfo->volt_max;
	}
}
uint16_t volt_min(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return PNetInfo->volt_min;
	}
}

float curr_avg(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (float)PNetInfo->curr_avg / PNetInfo->count;
	}
}

float curr_last(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (float)PNetInfo->curr_last;
	}
}

float cos_avg(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (float)PNetInfo->cos_avg / PNetInfo->count;
	}
}

float cos_last(uint8_t p) {
	
	tp_net_type *PNetInfo;
	
	if (p==1) {PNetInfo = &NetInfoA;}
	else if (p==2) {PNetInfo = &NetInfoB;}
	else {PNetInfo = &NetInfoC;}
	
	if (!PNetInfo->count) {
		return 0;
	} else {
		return (float)PNetInfo->cos_last;
	}
}

void net_kt_volt_set(uint8_t p, float val) {
	
	uint16_t addr = 0;
	
	switch (p) {
		case 1:
			addr = EE_KT_VA;
		break;
		
		case 2:
			addr = EE_KT_VB;
		break;
		
		case 3:
			addr = EE_KT_VC;
		break;
	}
	net_kt_volt[p-1] = val;
	eeprom_write_float(addr, val);
}

float net_kt_volt_get(uint8_t p) {
	
	uint16_t addr = 0;
	
	switch (p) {
		case 1:
			addr = EE_KT_VA;
		break;
		
		case 2:
			addr = EE_KT_VB;
		break;
		
		case 3:
			addr = EE_KT_VC;
		break;
	}
	
	if (eeprom_read_uint32(addr) == zval_32) {
		return 1;
	}
	
	return eeprom_read_float(addr);
}

void net_kt_curr_set(uint8_t p, float val) {
	
	uint16_t addr = 0;
	
	switch (p) {
		case 1:
			addr = EE_KT_IA;
		break;
		
		case 2:
			addr = EE_KT_IB;
		break;
		
		case 3:
			addr = EE_KT_IC;
		break;
	}
	net_kt_curr[p-1] = val;
	eeprom_write_float(addr, val);
}

float net_kt_curr_get(uint8_t p) {
	
	uint16_t addr = 0;
	
	switch (p) {
		case 1:
			addr = EE_KT_IA;
		break;
		
		case 2:
			addr = EE_KT_IB;
		break;
		
		case 3:
			addr = EE_KT_IC;
		break;
	}
	
	if (eeprom_read_uint32(addr) == zval_32) {
		return 1;
	}
	
	return eeprom_read_float(addr);
}

void steps_read(void) {
	
	float    volt;
	uint32_t tmp;
	
	for (uint8_t n = 1; n <= 5; n++) {
		tmp = eeprom_read_uint32(EE_STEPS + 4*(n-1));
		if (tmp!=zval_32) {
			volt = eeprom_read_float(EE_STEPS + 4*(n-1));
			steps_mass[n-1] = volt;
		}
	}
	for (uint8_t n = 1; n <= 5; n++) {
		tmp = eeprom_read_uint32(EE_STEPS + 4*(n+5-1));
		if (tmp!=zval_32) {
			volt = eeprom_read_float(EE_STEPS + 4*(n+5-1));
			steps_mass[n+5-1] = volt;
		}
	}
}
float steps_get(int8_t step) {
	if (step > 0) {
		return steps_mass[step-1];
	} else if (step < 0) {
		return steps_mass[-step+5-1];
	} else {
		return 1;
	}
}
void steps_set(int8_t step, float volt) {
	if (step > 0) {
		eeprom_write_float(EE_STEPS + 4*(step-1), volt);
		steps_mass[step-1] = volt;
	}
	if (step < 0) {
		eeprom_write_float(EE_STEPS + 4*(-step+5-1), volt);
		steps_mass[-step+5-1] = volt;
	}
}

//------------------------------------------------------------------------------------------

void volt_minmax_set(uint16_t min, uint16_t max) {
	
	auto_volt_min = min;
	auto_volt_max = max;
	
	eeprom_write_uint16(EE_SET_VOLT_MIN, min);
	eeprom_write_uint16(EE_SET_VOLT_MAX, max);
}

uint16_t volt_min_get(void) {
	
	uint16_t val = eeprom_read_uint16(EE_SET_VOLT_MIN);
	
	if (val == zval_16) {
		val = SET_VOLT_MIN;
	}
	return val;
}

uint16_t volt_max_get(void) {
	
	uint16_t val = eeprom_read_uint16(EE_SET_VOLT_MAX);
	
	if (val == zval_16) {
		val = SET_VOLT_MAX;
	}
	return val;
}

void range_minmax_set(uint16_t min, uint16_t max) {
	
	range_volt_min = min;
	range_volt_max = max;
	
	eeprom_write_uint16(EE_SET_RANGE_MIN, min);
	eeprom_write_uint16(EE_SET_RANGE_MAX, max);
}

uint16_t range_min_get(void) {
	
	uint16_t val = eeprom_read_uint16(EE_SET_RANGE_MIN);
	
	if (val == zval_16) {
		val = NET_MIN_VOLT;
	}
	return val;
}

uint16_t range_max_get(void) {
	
	uint16_t val = eeprom_read_uint16(EE_SET_RANGE_MAX);
	
	if (val == zval_16) {
		val = NET_MAX_VOLT;
	}
	return val;
}


void boost_set(uint8_t on) {
	auto_boost = on;
	eeprom_write_byte(EE_SET_BOOST, on);
}

uint8_t boost_get(void) {
	
	uint8_t step = eeprom_read_byte(EE_SET_BOOST);
	
	if (step == zval_8) {
		step = SET_BOOST;
	}
	return step;
}

void steps_count_set(uint8_t step) {
	auto_steps_count = step;
	eeprom_write_byte(EE_SET_STEPS, step);
}

uint8_t steps_count_get(void) {
	
	uint8_t step = eeprom_read_byte(EE_SET_STEPS);
	
	if (step == zval_8) {
		step = SET_STEPS;
	}
	return step;
}

void des_time_set(uint8_t step) {
	auto_time = step;
	eeprom_write_byte(EE_SET_DES_TIME, step);
}

uint8_t des_time_get(void) {
	
	uint8_t step = eeprom_read_byte(EE_SET_DES_TIME);
	if (step == zval_8 || !step) {
		step = SET_DES_TIME;
	}
	return step;
}

void points_set(uint8_t step) {
	auto_points = step;
	eeprom_write_byte(EE_SET_POINTS, step);
}

uint8_t points_get(void) {
	
	uint8_t step = eeprom_read_byte(EE_SET_POINTS);
	if (step == zval_8) {
		step = SET_POINTS;
	}
	return step;
}

void kt_set(uint8_t step) {
	auto_kt = step;
	eeprom_write_byte(EE_SET_KT, step);
}

uint8_t kt_get(void) {
	
	uint8_t step = eeprom_read_byte(EE_SET_KT);
	if (step == zval_8) {
		step = SET_KT;
	}
	return step;
}

void debug_mess_set(uint8_t val) {
	debug_mess = val;
	eeprom_write_byte(EE_DEBUG_MESS, val);
}

uint8_t debug_mess_get(void) {
	
	uint8_t val = eeprom_read_byte(EE_DEBUG_MESS);
	if (val == zval_8) {
		val = SET_DEBUG_MESS;
	}
	return val;
}

void debug_error_set(uint8_t val) {
	debug_error = val;
	eeprom_write_byte(EE_DEBUG_ERROR, val);
}

uint8_t debug_error_get(void) {
	
	uint8_t val = eeprom_read_byte(EE_DEBUG_ERROR);
	if (val == zval_8) {
		val = SET_DEBUG_ERROR;
	}
	return val;
}

void debug_ports_set(uint8_t val) {
	debug_ports = val;
	eeprom_write_byte(EE_DEBUG_PORTS, val);
}

uint8_t debug_ports_get(void) {
	
	uint8_t val = eeprom_read_byte(EE_DEBUG_PORTS);
	if (val == zval_8) {
		val = SET_DEBUG_PORTS;
	}
	return val;
}

void auto_volt_set(uint8_t val) {
	auto_volt_phase = val;
	eeprom_write_byte(EE_AUTO_VOLT, val);
}

uint8_t auto_volt_get(void) {
	
	uint8_t val = eeprom_read_byte(EE_AUTO_VOLT);
	if (val == zval_8) {
		val = SET_AUTO_VOLT;
	}
	return val;
}